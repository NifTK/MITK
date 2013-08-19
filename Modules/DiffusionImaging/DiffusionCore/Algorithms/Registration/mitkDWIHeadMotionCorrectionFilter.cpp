/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#ifndef MITKDIFFUSIONIMAGETODIFFUSIONIMAGEFILTER_CPP
#define MITKDIFFUSIONIMAGETODIFFUSIONIMAGEFILTER_CPP

#include "mitkDWIHeadMotionCorrectionFilter.h"

#include "itkSplitDWImageFilter.h"
#include "itkB0ImageExtractionToSeparateImageFilter.h"

#include "mitkImageTimeSelector.h"

#include "mitkPyramidImageRegistrationMethod.h"
#include "mitkImageToDiffusionImageSource.h"

#include "mitkIOUtil.h"

template< typename DiffusionPixelType>
mitk::DWIHeadMotionCorrectionFilter<DiffusionPixelType>
::DWIHeadMotionCorrectionFilter()
{

}

template< typename DiffusionPixelType>
void mitk::DWIHeadMotionCorrectionFilter<DiffusionPixelType>
::GenerateData()
{
  typedef itk::SplitDWImageFilter< DiffusionPixelType, DiffusionPixelType> SplitFilterType;

  DiffusionImageType* input = const_cast<DiffusionImageType*>(this->GetInput(0));

  //
  //  (1) Extract the b-zero images to a 3d+t image, register them by NCorr metric and
  //     rigid registration : they will then be used are reference image for registering
  //     the gradient images
  //
  typedef itk::B0ImageExtractionToSeparateImageFilter< DiffusionPixelType, DiffusionPixelType> B0ExtractorType;
  typename B0ExtractorType::Pointer b0_extractor = B0ExtractorType::New();
  b0_extractor->SetInput( input->GetVectorImage() );
  b0_extractor->SetDirections( input->GetDirections() );
  b0_extractor->Update();

  mitk::Image::Pointer b0Image = mitk::Image::New();
  b0Image->InitializeByItk( b0_extractor->GetOutput() );
  b0Image->SetImportChannel( b0_extractor->GetOutput()->GetBufferPointer(),
                             mitk::Image::CopyMemory );

  // (2.1) Use the extractor to access the extracted b0 volumes
  mitk::ImageTimeSelector::Pointer t_selector =
      mitk::ImageTimeSelector::New();

  t_selector->SetInput( b0Image );
  t_selector->SetTimeNr(0);
  t_selector->Update();

  // first unweighted image as reference space for the registration
  mitk::Image::Pointer b0referenceImage = t_selector->GetOutput();

  mitk::PyramidImageRegistrationMethod::Pointer registrationMethod = mitk::PyramidImageRegistrationMethod::New();
  registrationMethod->SetFixedImage( b0referenceImage );
  registrationMethod->SetTransformToRigid();

  // the unweighted images are of same modality
  registrationMethod->SetCrossModalityOff();

  // Initialize the temporary output image
  mitk::Image::Pointer registeredB0Image = b0Image->Clone();
  const unsigned int numberOfb0Images = b0Image->GetTimeSteps();

  mitk::ImageTimeSelector::Pointer t_selector2 =
      mitk::ImageTimeSelector::New();

  t_selector2->SetInput( b0Image );

  for( unsigned int i=1; i<numberOfb0Images; i++)
  {

    t_selector2->SetTimeNr(i);
    t_selector2->Update();

    registrationMethod->SetMovingImage( t_selector2->GetOutput() );

    try
    {
      MITK_INFO << " === (" << i <<"/"<< numberOfb0Images-1 << ") :: Starting registration";
      registrationMethod->Update();
    }
    catch( const itk::ExceptionObject& e)
    {
      mitkThrow() << "Failed to register the b0 images, the PyramidRegistration threw an exception: \n" << e.what();
    }

    // import volume to the inter-results
    registeredB0Image->SetImportVolume( registrationMethod->GetResampledMovingImage()->GetData(),
                                       i, 0, mitk::Image::ReferenceMemory );

  }


  //
  // (2) Split the diffusion image into a 3d+t regular image, extract only the weighted images
  //
  typename SplitFilterType::Pointer split_filter = SplitFilterType::New();
  split_filter->SetInput (input->GetVectorImage() );
  split_filter->SetExtractAllAboveThreshold(20, input->GetB_ValueMap() );

  try
  {
    split_filter->Update();
  }
  catch( const itk::ExceptionObject &e)
  {
    mitkThrow() << " Caught exception from SplitImageFilter : " << e.what();
  }

  mitk::Image::Pointer splittedImage = mitk::Image::New();
  splittedImage->InitializeByItk( split_filter->GetOutput() );
  splittedImage->SetImportChannel( split_filter->GetOutput()->GetBufferPointer(),
                                   mitk::Image::CopyMemory );


  //
  // (3) Use again the time-selector to access the components separately in order
  //     to perform the registration of  Image -> unweighted reference
  //

  mitk::PyramidImageRegistrationMethod::Pointer weightedRegistrationMethod
      = mitk::PyramidImageRegistrationMethod::New();

  weightedRegistrationMethod->SetTransformToAffine();
  weightedRegistrationMethod->SetCrossModalityOn();
  //
  //   - (3.1) Create a reference image by averaging the aligned b0 images
  //
  //   !!!FIXME: For rapid prototyping using the first one
  //

  weightedRegistrationMethod->SetFixedImage( b0referenceImage );

  //
  //   - (3.2) Register all timesteps in the splitted image onto the first reference
  //
  unsigned int maxImageIdx = splittedImage->GetTimeSteps();
  mitk::TimeSlicedGeometry* tsg = splittedImage->GetTimeSlicedGeometry();
  tsg->ExpandToNumberOfTimeSteps( maxImageIdx+1 );

  mitk::Image::Pointer registeredWeighted = mitk::Image::New();
  registeredWeighted->Initialize( splittedImage->GetPixelType(0), *tsg );

  // insert the first unweighted reference as the first volume
  registeredWeighted->SetImportVolume( b0referenceImage->GetData(),
                                      0,0, mitk::Image::CopyMemory );


  // mitk::Image::Pointer registeredWeighted = splittedImage->Clone();
  // this time start at 0, we have only gradient images in the 3d+t file
  // the reference image comes form an other image
  mitk::ImageTimeSelector::Pointer t_selector_w =
      mitk::ImageTimeSelector::New();

  t_selector_w->SetInput( splittedImage );

  for( unsigned int i=0; i<maxImageIdx; i++)
  {
    t_selector_w->SetTimeNr(i);
    t_selector_w->Update();

    weightedRegistrationMethod->SetMovingImage( t_selector_w->GetOutput() );

    try
    {
      MITK_INFO << " === (" << i+1 <<"/"<< maxImageIdx << ") :: Starting registration";
      weightedRegistrationMethod->Update();
    }
    catch( const itk::ExceptionObject& e)
    {
      mitkThrow() << "Failed to register the b0 images, the PyramidRegistration threw an exception: \n" << e.what();
    }

    // allow expansion
    registeredWeighted->SetImportVolume( weightedRegistrationMethod->GetResampledMovingImage()->GetData(),
                                         i+1, 0, mitk::Image::CopyMemory);
  }


  //
  // (4) Cast the resulting image back to an diffusion weighted image
  //
  typename DiffusionImageType::GradientDirectionContainerType *gradients = input->GetDirections();
  typename DiffusionImageType::GradientDirectionContainerType::Pointer gradients_new =
      DiffusionImageType::GradientDirectionContainerType::New();
  typename DiffusionImageType::GradientDirectionType bzero_vector;
  bzero_vector.fill(0);

  // compose the direction vector
  //  - no direction for the first image
  //  - correct ordering of the directions based on the index list
  gradients_new->push_back( bzero_vector );

  typename SplitFilterType::IndexListType index_list = split_filter->GetIndexList();
  typename SplitFilterType::IndexListType::const_iterator lIter = index_list.begin();

  while( lIter != index_list.end() )
  {
    gradients_new->push_back( gradients->at( *lIter ) );
    ++lIter;
  }

  typename mitk::ImageToDiffusionImageSource< DiffusionPixelType >::Pointer caster =
      mitk::ImageToDiffusionImageSource< DiffusionPixelType >::New();

  caster->SetImage( registeredWeighted );
  caster->SetBValue( input->GetB_Value() );
  caster->SetGradientDirections( gradients_new.GetPointer() );

  try
  {
    caster->Update();
  }
  catch( const itk::ExceptionObject& e)
  {
    MITK_ERROR << "Casting back to diffusion image failed: ";
    mitkThrow() << "Subprocess failed with exception: " << e.what();
  }

  OutputImagePointerType output = this->GetOutput();
  output = caster->GetOutput();

  std::cout << "Last line : Generate Data " << std::endl;

}

template< typename DiffusionPixelType>
void mitk::DWIHeadMotionCorrectionFilter<DiffusionPixelType>
::GenerateOutputInformation()
{
  if( ! this->GetInput(0) )
  {
    mitkThrow() << "No input specified!";
  }

  OutputImagePointerType output = this->GetOutput();
  output->CopyInformation( this->GetInput(0) );
}

#endif // MITKDIFFUSIONIMAGETODIFFUSIONIMAGEFILTER_CPP
