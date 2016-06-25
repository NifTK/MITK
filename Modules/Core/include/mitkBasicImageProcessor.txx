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

#include "mitkBasicImageProcessor.h"

// Morphological Operations
#include <itkBinaryBallStructuringElement.h>
#include <itkGrayscaleDilateImageFilter.h>
#include <itkGrayscaleErodeImageFilter.h>
#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>

// Smoothing
#include <itkMedianImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
//#include <itkTotalVariationDenoisingImageFilter.h>

// Threshold
#include <itkBinaryThresholdImageFilter.h>
#include <itkThresholdImageFilter.h>

// Derivatives
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkLaplacianImageFilter.h>
#include <itkSobelEdgeDetectionImageFilter.h>

// Resampling
#include <itkCastImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include "itkWindowedSincInterpolateImageFunction.h"
#include <itkLinearInterpolateImageFunction.h>
#include <itkGaussianInterpolateImageFunction.h>

// Image Arithmetics
#include <itkAddImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkDivideImageFilter.h>

// Boolean operations
#include <itkOrImageFilter.h>
#include <itkAndImageFilter.h>
#include <itkXorImageFilter.h>

// Flip Image
#include <itkFlipImageFilter.h>

// Intensity manipulation
#include <itkInvertIntensityImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkShiftScaleImageFilter.h>


template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ProcessImageByType(
  InputImageType* input,
  ActionType action,
  int p1,
  int /*p2*/,
  double dp1,
  double dp2,
  double dp3)
{
  typename InputImageType::Pointer outputImage = 0;
  
  switch (action)
  {
    case GAUSSIAN:
    {
      outputImage = GaussianFilter<InputImageType>(input, p1);

/*
      // Print out dimensions for debugging purposes
      int m_Dimension=outputImage->GetImageDimension();
      unsigned int i;
      unsigned int *tmpDimensions = new unsigned int[m_Dimension>4?m_Dimension:4];

      for(i=0;i<m_Dimension;++i)
      {
        tmpDimensions[i]=outputImage->GetLargestPossibleRegion().GetSize().GetSize()[i];
        MITK_INFO <<"Dim " <<i <<" tmpDimensions[i]: " <<tmpDimensions[i];
      }
*/
      break;
    }

    case MEDIAN:
    {
      outputImage = MedianFilter<InputImageType>(input, p1);
      break;
    }

    //case TOTALVARIATION:
    //{
    //  outputImage = TotalVariationFilter<InputImageType>(input, p1, p2);
    //  break;
    //}

    case DILATION:
    {
      outputImage = DilateImage<InputImageType>(input, p1);
      break;
    }

    case EROSION:
    {
      outputImage = ErodeImage<InputImageType>(input, p1);
      break;
    }

    case OPENING:
    {
      outputImage = OpenImage<InputImageType>(input, p1);
      break;
    }

    case CLOSING:
    {
      outputImage = CloseImage<InputImageType>(input, p1);
      break;
    }

    case GRADIENT:
    {
      outputImage = GradientFilter<InputImageType>(input, p1);
      break;
    }

    case LAPLACIAN:
    {
      outputImage = LaplacianFilter<InputImageType>(input);
      break;
    }

    case SOBEL:
    {
      outputImage = SobelFilter<InputImageType>(input);
      break;
    }

    case THRESHOLD:
    {
      outputImage = ThresholdFilter<InputImageType>(input, dp1, dp2);
      break;
    }

    case BINARYTHRESHOLD:
    {
      outputImage = BinaryThresholdFilter<InputImageType>(input, dp1, dp2);
      break;
    }

    case INVERSION:
    {
      outputImage = InvertImage<InputImageType>(input);
      break;
    }

    case DOWNSAMPLING:
    {
      outputImage = DownsampleImage<InputImageType>(input, dp1);
      break;
    }

    case FLIPPING:
    {
      outputImage = FlipImage<InputImageType>(input, p1);
      break;
    }

    case RESAMPLING:
    {
      outputImage = ResampleImage<InputImageType>(input, p1, dp1, dp2, dp3);
      break;
    }

    case RESCALE:
    {
      outputImage = RescaleImage<InputImageType>(input, dp1, dp2, dp3);
      break;
    }

    case RESCALE2:
    {
      outputImage = Rescale2Image<InputImageType>(input, dp1, dp2, dp3);
      break;
    }

  default:
    return outputImage;
  }

  return outputImage;
}


template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::GaussianFilter( InputImageType* input, int param1)
{
  typedef itk::DiscreteGaussianImageFilter< InputImageType, InputImageType> GaussianFilterType;
  typename GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
  gaussianFilter->SetInput( input );
  gaussianFilter->SetVariance( param1 );
  gaussianFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(gaussianFilter->GetOutput());
  duplicator->Update();
  
  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::MedianFilter( InputImageType* input, int param1)
{
  typedef itk::MedianImageFilter< InputImageType, InputImageType > MedianFilterType;
  typename MedianFilterType::Pointer medianFilter = MedianFilterType::New();
  typename MedianFilterType::InputSizeType size;
  size.Fill(param1);
  medianFilter->SetRadius( size );
  medianFilter->SetInput(input);
  medianFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(medianFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

/*
template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::TotalVariationFilter( InputImageType* input, int param1, int param2)
{
  typedef double                                                                WorkPixelType;
  typedef itk::Image< WorkPixelType, 3 >                                        WorkImageType;
  typedef itk::TotalVariationDenoisingImageFilter<WorkImageType, WorkImageType> TotalVariationFilterType;
  
  typedef itk::CastImageFilter< InputImageType, WorkImageType > CastFilterType;
  typedef itk::CastImageFilter< WorkImageType, InputImageType > CastBackFilterType;
  
  typename CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput( input );
  caster->Update();

  TotalVariationFilterType::Pointer TVFilter = TotalVariationFilterType::New();
  TVFilter->SetInput( caster->GetOutput() );
  TVFilter->SetNumberIterations(param1);
  TVFilter->SetLambda(double(param2)/1000.);
  TVFilter->UpdateLargestPossibleRegion();

  typename CastBackFilterType::Pointer backCaster = CastBackFilterType::New();
  backCaster->SetInput( TVFilter->GetOutput() );
  backCaster->Update();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(backCaster->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}
*/
template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::DilateImage( InputImageType* input, int param1)
{
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::BinaryBallStructuringElement<InputPixelType, 3>           BallType;
  typedef itk::GrayscaleDilateImageFilter<InputImageType, InputImageType, BallType> DilationFilterType;

  BallType binaryBall;
  binaryBall.SetRadius( param1 );
  binaryBall.CreateStructuringElement();

  typename DilationFilterType::Pointer dilationFilter = DilationFilterType::New();
  dilationFilter->SetInput( input );
  dilationFilter->SetKernel( binaryBall );
  dilationFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(dilationFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ErodeImage( InputImageType* input, int param1)
{
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::BinaryBallStructuringElement<InputPixelType, 3>           BallType;
  typedef itk::GrayscaleErodeImageFilter<InputImageType, InputImageType, BallType>  ErosionFilterType;
  BallType binaryBall;
  binaryBall.SetRadius( param1 );
  binaryBall.CreateStructuringElement();

  typename ErosionFilterType::Pointer erosionFilter = ErosionFilterType::New();
  erosionFilter->SetInput( input );
  erosionFilter->SetKernel( binaryBall );
  erosionFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(erosionFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::OpenImage( InputImageType* input, int param1)
{
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::BinaryBallStructuringElement<InputPixelType, 3>                                    BallType;
  typedef itk::GrayscaleMorphologicalOpeningImageFilter<InputImageType, InputImageType, BallType> OpeningFilterType;
  BallType binaryBall;
  binaryBall.SetRadius( param1 );
  binaryBall.CreateStructuringElement();

  typename OpeningFilterType::Pointer openingFilter = OpeningFilterType::New();
  openingFilter->SetInput( input );
  openingFilter->SetKernel( binaryBall );
  openingFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(openingFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::CloseImage( InputImageType* input, int param1)
{
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::BinaryBallStructuringElement<InputPixelType, 3>                                    BallType;
  typedef itk::GrayscaleMorphologicalClosingImageFilter<InputImageType, InputImageType, BallType> ClosingFilterType;
  BallType binaryBall;
  binaryBall.SetRadius( param1 );
  binaryBall.CreateStructuringElement();

  typename ClosingFilterType::Pointer closingFilter = ClosingFilterType::New();
  closingFilter->SetInput( input );
  closingFilter->SetKernel( binaryBall );
  closingFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(closingFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::GradientFilter( InputImageType* input, int param1)
{
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType, InputImageType > GradientFilterType;
  typename GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
  gradientFilter->SetInput( input );
  gradientFilter->SetSigma( param1 );
  gradientFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(gradientFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::LaplacianFilter( InputImageType* input)
{
  typedef double                                                    WorkPixelType;
  typedef itk::Image< WorkPixelType, 3 >                            WorkImageType;
  typedef itk::LaplacianImageFilter< WorkImageType, WorkImageType > LaplacianFilterType;
  
  typedef itk::CastImageFilter< InputImageType, WorkImageType > CastFilterType;
  typedef itk::CastImageFilter< WorkImageType, InputImageType > CastBackFilterType;
  
  typename CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput( input );
  caster->Update();

  typename LaplacianFilterType::Pointer laplacianFilter = LaplacianFilterType::New();
  laplacianFilter->SetInput( caster->GetOutput() );
  laplacianFilter->UpdateLargestPossibleRegion();

  typename CastBackFilterType::Pointer backCaster = CastBackFilterType::New();
  backCaster->SetInput( laplacianFilter->GetOutput() );
  backCaster->Update();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(backCaster->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::SobelFilter( InputImageType* input)
{
  typedef double                                                               WorkPixelType;
  typedef itk::Image< WorkPixelType, 3 >                                       WorkImageType;
  typedef itk::SobelEdgeDetectionImageFilter< WorkImageType, WorkImageType > SobelFilterType;
  
  typedef itk::CastImageFilter< InputImageType, WorkImageType > CastFilterType;
  typedef itk::CastImageFilter< WorkImageType, InputImageType > CastBackFilterType;
  
  typename CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput( input );
  caster->Update();

  typename SobelFilterType::Pointer sobelFilter = SobelFilterType::New();
  sobelFilter->SetInput( caster->GetOutput() );
  sobelFilter->UpdateLargestPossibleRegion();

  typename CastBackFilterType::Pointer backCaster = CastBackFilterType::New();
  backCaster->SetInput( sobelFilter->GetOutput() );
  backCaster->Update();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(backCaster->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ThresholdFilter( InputImageType* input, double param1, double param2)
{  
  typedef itk::ThresholdImageFilter< InputImageType > ThresholdFilterType;
  typename ThresholdFilterType::Pointer thFilter = ThresholdFilterType::New();
  thFilter->ThresholdOutside(param1 < param2 ? param1 : param2, param2 > param1 ? param2 : param1);
  thFilter->SetOutsideValue(0);
  thFilter->SetInput(input);
  thFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(thFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::BinaryThresholdFilter( InputImageType* input, double param1, double param2)
{  
  typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > ThresholdFilterType;
  typename ThresholdFilterType::Pointer thFilter = ThresholdFilterType::New();
  thFilter->SetLowerThreshold(param1 < param2 ? param1 : param2);
  thFilter->SetUpperThreshold(param2 > param1 ? param2 : param1);
  thFilter->SetInsideValue(1);
  thFilter->SetOutsideValue(0);
  thFilter->SetInput(input);
  thFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(thFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::InvertImage( InputImageType* input)
{
  typedef itk::MinimumMaximumImageCalculator<InputImageType> MinMaxCalculatorType;
  typename MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
  calculator->SetImage( input );
  calculator->Compute();

  typedef itk::InvertIntensityImageFilter< InputImageType, InputImageType > InversionFilterType;
  typename InversionFilterType::Pointer invFilter = InversionFilterType::New();
  invFilter->SetMaximum( calculator->GetMinimum() + calculator->GetMaximum() );
  invFilter->SetInput(input);
  invFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(invFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::DownsampleImage( InputImageType* input, double dparam1)
{
  typedef itk::ResampleImageFilter< InputImageType, InputImageType > ResampleImageFilterType;
  typename ResampleImageFilterType::Pointer downsampler = ResampleImageFilterType::New();
  downsampler->SetInput( input );

  // Sinc interpolation
  //const unsigned int WindowRadius = 5;
  //typedef itk::ConstantBoundaryCondition< InputImageType > BoundaryConditionType;
  //typedef itk::Function::HammingWindowFunction<WindowRadius> WindowFunctionType;
  //typedef itk::WindowedSincInterpolateImageFunction<
  //          InputImageType, WindowRadius, WindowFunctionType,
  //          BoundaryConditionType, double  > SincInterpolatorType;
  //SincInterpolatorType::Pointer   sincInterpolator  = SincInterpolatorType::New();

  // Nearest Neighbour interpolation
  //typedef itk::NearestNeighborInterpolateImageFunction< InputImageType, double > NearestInterpolatorType;
  //typename NearestInterpolatorType::Pointer interpolator = NearestInterpolatorType::New();

  // Gaussian interpolation
  //typedef itk::GaussianInterpolateImageFunction<InputImageType, double> GaussianInterpolatorType;
  //typename GaussianInterpolatorType::Pointer gaussian_interpolator = GaussianInterpolatorType::New();

  // Linear interpolation - it is good enough for generic use
  typename itk::LinearInterpolateImageFunction<InputImageType, double>::Pointer linear_interpolator = itk::LinearInterpolateImageFunction<InputImageType, double>::New();

  // Set interpolator type and default value
  downsampler->SetInterpolator( linear_interpolator );
  downsampler->SetDefaultPixelValue( 0 );

  typename ResampleImageFilterType::SpacingType spacing = input->GetSpacing();
  spacing *= dparam1;
  downsampler->SetOutputSpacing( spacing );

  downsampler->SetOutputOrigin( input->GetOrigin() );
  downsampler->SetOutputDirection( input->GetDirection() );

  typename ResampleImageFilterType::SizeType size = input->GetLargestPossibleRegion().GetSize();
  for ( int i = 0; i < 3; ++i )
  {
    size[i] /= dparam1;
  }

  downsampler->SetSize( size );
  downsampler->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(downsampler->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::FlipImage( InputImageType* input, int param1)
{
  typedef itk::FlipImageFilter< InputImageType > FlipImageFilterType;
  typename FlipImageFilterType::Pointer flipper = FlipImageFilterType::New();
 
  itk::FixedArray<bool, 3> flipAxes;
  for(int i=0; i<3; ++i)
  {
    if (i == param1)
    {
      flipAxes[i] = true;
    }
    else
    {
      flipAxes[i] = false;
    }
  }
  
  flipper->SetInput(input );
  flipper->SetFlipAxes(flipAxes);
  flipper->UpdateLargestPossibleRegion();

  typedef itk::ChangeInformationImageFilter< InputImageType >  InfoFilterType;
  typename InfoFilterType::Pointer infoChangeFilter = InfoFilterType::New();
  
  typename InputImageType::PointType     origin    = input-> GetOrigin();
  typename InputImageType::SpacingType   spacing   = input->GetSpacing();
  typename InputImageType::DirectionType direction = input->GetDirection();

  infoChangeFilter->SetOutputSpacing( spacing );
  infoChangeFilter->ChangeSpacingOn();
  infoChangeFilter->SetOutputOrigin( origin );
  infoChangeFilter->ChangeOriginOn();
  infoChangeFilter->SetOutputDirection( direction );
  infoChangeFilter->ChangeDirectionOn();

  infoChangeFilter->SetInput(flipper->GetOutput());
  infoChangeFilter->Update();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(infoChangeFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ResampleImage( InputImageType* input, int param1, double dparam1, double dparam2, double dparam3)
{
  typedef itk::LinearInterpolateImageFunction< InputImageType, double >          LinearInterpolatorType;
  typedef itk::NearestNeighborInterpolateImageFunction< InputImageType, double > NearestInterpolatorType;
  typedef itk::ResampleImageFilter< InputImageType, InputImageType >             ResampleImageFilterType;

  typename ResampleImageFilterType::Pointer resampler = ResampleImageFilterType::New();

  std::string selectedInterpolator;
  switch (param1)
  {
  case 0:
    {
      typename LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Linear";
      break;
    }
  case 1:
    {
      typename NearestInterpolatorType::Pointer interpolator = NearestInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Nearest";
      break;
    }
  default:
    {
      typename LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Linear";
      break;
    }
  }
  resampler->SetInput( input );
  resampler->SetOutputOrigin( input->GetOrigin() );

  typename InputImageType::SizeType input_size = input->GetLargestPossibleRegion().GetSize();
  typename InputImageType::SpacingType input_spacing = input->GetSpacing();

  typename InputImageType::SizeType output_size;
  typename InputImageType::SpacingType output_spacing;

  output_size[0] = input_size[0] * (input_spacing[0] / dparam1);
  output_size[1] = input_size[1] * (input_spacing[1] / dparam2);
  output_size[2] = input_size[2] * (input_spacing[2] / dparam3);
  output_spacing [0] = dparam1;
  output_spacing [1] = dparam2;
  output_spacing [2] = dparam3;

  resampler->SetSize( output_size );
  resampler->SetOutputSpacing( output_spacing );
  resampler->SetOutputDirection( input->GetDirection() );

  resampler->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(resampler->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}


template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::RescaleImage( InputImageType* input, double dparam1, double dparam2, double /*dparam3*/)
{
  typedef itk::RescaleIntensityImageFilter< InputImageType, InputImageType > RescaleImageFilterType;
  typename RescaleImageFilterType::Pointer rescaler = RescaleImageFilterType::New();
  rescaler->SetInput(input);
  rescaler->SetOutputMinimum(dparam1);
  rescaler->SetOutputMaximum(dparam2);
  
  rescaler->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(rescaler->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::Rescale2Image( InputImageType* input, double dparam1, double /*dparam2*/, double /*dparam3*/)
{
  typedef itk::ShiftScaleImageFilter< InputImageType, InputImageType > RescaleImageFilterType;
  typename RescaleImageFilterType::Pointer rescaler = RescaleImageFilterType::New();
  rescaler->SetInput(0, input);
  rescaler->SetScale(dparam1);
  
  rescaler->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(rescaler->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ProcessTwoImagesByType(
  InputImageType* input,
  InputImageType* reference,
  OperationType   operation)
{
  typename InputImageType::Pointer outputImage = 0;
  
  switch (operation)
  {
    case ADD:
    {
      outputImage = AddImages<InputImageType>(input, reference);
      break;
    }

    case SUBTRACT:
    {
      outputImage = SubtractImages<InputImageType>(input, reference);
      break;
    }

    case MULTIPLY:
    {
      outputImage = MultiplyImages<InputImageType>(input, reference);
      break;
    }

    case DIVIDE:
    {
      outputImage = DivideImages<InputImageType>(input, reference);
      break;
    }

    case AND:
    {
      outputImage = AndImages<InputImageType>(input, reference);
      break;
    }

    case OR:
    {
      outputImage = OrImages<InputImageType>(input, reference);
      break;
    }

    case XOR:
    {
      outputImage = XorImages<InputImageType>(input, reference);
      break;
    }

  default:
    return outputImage;
  }

  return outputImage;
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::AddImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::AddImageFilter< InputImageType, InputImageType, InputImageType > AddFilterType;
  typename AddFilterType::Pointer addFilter = AddFilterType::New();
  addFilter->SetInput1( input );
  addFilter->SetInput2( reference );
  addFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  addFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  addFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(addFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::SubtractImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::SubtractImageFilter< InputImageType, InputImageType, InputImageType > SubtractFilterType;
  typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();
  subtractFilter->SetInput1( input );
  subtractFilter->SetInput2( reference );
  subtractFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  subtractFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  subtractFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(subtractFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::MultiplyImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::MultiplyImageFilter< InputImageType, InputImageType, InputImageType > MultiplyFilterType;
  typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
  multiplyFilter->SetInput1( input );
  multiplyFilter->SetInput2( reference );
  multiplyFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  multiplyFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  multiplyFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(multiplyFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::DivideImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::DivideImageFilter< InputImageType, InputImageType, InputImageType > DivideFilterType;
  typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();
  divideFilter->SetInput1( input );
  divideFilter->SetInput2( reference );
  divideFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  divideFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  divideFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(divideFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::OrImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::OrImageFilter< InputImageType, InputImageType, InputImageType > OrFilterType;
  typename OrFilterType::Pointer orFilter = OrFilterType::New();
  orFilter->SetInput1( input );
  orFilter->SetInput2( reference );
  orFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  orFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  orFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(orFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::AndImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::AndImageFilter< InputImageType, InputImageType, InputImageType > AndFilterType;
  typename AndFilterType::Pointer andFilter = AndFilterType::New();
  andFilter->SetInput1( input );
  andFilter->SetInput2( reference );
  andFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  andFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  andFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(andFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::XorImages( InputImageType* input, InputImageType* reference)
{
  typedef itk::XorImageFilter< InputImageType, InputImageType, InputImageType > XorFilterType;
  typename XorFilterType::Pointer xorFilter = XorFilterType::New();
  xorFilter->SetInput1( input );
  xorFilter->SetInput2( reference );
  xorFilter->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  xorFilter->SetDirectionTolerance(PROC_DIR_TOLERANCE);
  xorFilter->UpdateLargestPossibleRegion();

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(xorFilter->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}

template <typename InputImageType>
typename InputImageType::Pointer
  mitk::mitkBasicImageProcessor::ResampleTo( InputImageType* input, InputImageType* reference, int param1)
{

  typedef itk::LinearInterpolateImageFunction< InputImageType, double >          LinearInterpolatorType;
  typedef itk::NearestNeighborInterpolateImageFunction< InputImageType, double > NearestInterpolatorType;
  typedef itk::ResampleImageFilter< InputImageType, InputImageType >             ResampleImageFilterType;

  typename ResampleImageFilterType::Pointer resampler = ResampleImageFilterType::New();

  std::string selectedInterpolator;
  switch (param1)
  {
  case 0:
    {
      typename LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Linear";
      break;
    }
  case 1:
    {
      typename NearestInterpolatorType::Pointer interpolator = NearestInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Nearest";
      break;
    }
  default:
    {
      typename LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      selectedInterpolator = "Linear";
      break;
    }
  }
  
  resampler->SetInput( input );
  resampler->SetReferenceImage( reference );
  resampler->SetUseReferenceImage( true );
  resampler->SetDefaultPixelValue( 0 );

  resampler->SetCoordinateTolerance(PROC_COORD_TOLERANCE);
  resampler->SetDirectionTolerance(PROC_DIR_TOLERANCE);

  try
  {
    resampler->UpdateLargestPossibleRegion();
  }
  catch( const itk::ExceptionObject &e)
  {
    MITK_WARN << "Updating resampling filter failed. ";
    MITK_WARN << "REASON: " << e.what();
  }

  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(resampler->GetOutput());
  duplicator->Update();

  return duplicator->GetModifiableOutput();
}
