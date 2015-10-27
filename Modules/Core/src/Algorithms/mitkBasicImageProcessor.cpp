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
#include "mitkBasicImageProcessor.txx"


mitk::mitkBasicImageProcessor::mitkBasicImageProcessor()
{
}

mitk::mitkBasicImageProcessor::~mitkBasicImageProcessor()
{
}

mitk::Image::Pointer
mitk::mitkBasicImageProcessor::ProcessImage(
  mitk::Image::Pointer image,
  ActionType action,
  int p1,
  int p2,
  double dp1,
  double dp2,
  double dp3)
{
  mitk::Image::Pointer outputImage = mitk::Image::New();

  switch (image->GetPixelType().GetComponentType())
  {
    case itk::ImageIOBase::UCHAR:
    {
      typedef itk::Image<unsigned char, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::CHAR:
    {      
      typedef itk::Image<char, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      typedef itk::Image<unsigned short, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      typedef itk::Image<short, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      typedef itk::Image<unsigned int, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::INT:
    {
      typedef itk::Image<int, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      typedef itk::Image<float, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {
      typedef itk::Image<double, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( image, itkImage );
      itkResultImage = ProcessImageByType<ImageType>(itkImage, action, p1, p2, dp1, dp2, dp3);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    default:
      return 0;
  }

  switch (action)
  {
    // Binary threshold results in a binary image which must have unsigned char pixel type
    // So, we force-cast the output here
    case BINARYTHRESHOLD:
    {
      typedef itk::Image<unsigned char, 3> ImageType;
      ImageType::Pointer itkImage = ImageType::New();

      mitk::CastToItkImage(outputImage, itkImage);
      mitk::CastToMitkImage(itkImage, outputImage);

      outputImage->DisconnectPipeline();
    }
  }

  return outputImage;
}

mitk::Image::Pointer 
mitk::mitkBasicImageProcessor::ProcessTwoImages(
  mitk::Image::Pointer inputImage,
  mitk::Image::Pointer referenceImage,
  OperationType operation)
{
  mitk::Image::Pointer outputImage = mitk::Image::New();

  switch (referenceImage->GetPixelType().GetComponentType())
  {
    case itk::ImageIOBase::UCHAR:
    {
      typedef itk::Image<unsigned char, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::CHAR:
    {      
      typedef itk::Image<char, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      typedef itk::Image<unsigned short, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      typedef itk::Image<short, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      typedef itk::Image<unsigned int, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::INT:
    {
      typedef itk::Image<int, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ProcessTwoImagesByType<ImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      typedef itk::Image< float, 3 > FloatImageType;
      typedef itk::Image<int, 3>     IntImageType;
      IntImageType::Pointer itkInputImage     = IntImageType::New();
      IntImageType::Pointer itkReferenceImage = IntImageType::New();
      IntImageType::Pointer itkResultImage    = 0;

      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );

      itkResultImage = ProcessTwoImagesByType<IntImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      typedef itk::CastImageFilter< IntImageType, FloatImageType > CastFilterType;

      CastFilterType::Pointer caster = CastFilterType::New();
      caster->SetInput( itkResultImage );
      caster->Update();

      mitk::CastToMitkImage(caster->GetOutput(), outputImage);


      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {

      typedef itk::Image< double, 3 > DoubleImageType;
      typedef itk::Image<int, 3>     IntImageType;
      IntImageType::Pointer itkInputImage     = IntImageType::New();
      IntImageType::Pointer itkReferenceImage = IntImageType::New();
      IntImageType::Pointer itkResultImage    = 0;
        
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
        
      itkResultImage = ProcessTwoImagesByType<IntImageType>(itkInputImage, itkReferenceImage, operation);
      itkResultImage->DisconnectPipeline();

      typedef itk::CastImageFilter< IntImageType, DoubleImageType > CastFilterType;
  
      CastFilterType::Pointer caster = CastFilterType::New();
      caster->SetInput( itkResultImage );
      caster->Update();

      mitk::CastToMitkImage(caster->GetOutput(), outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    default:
      return 0;
  }

  return outputImage;
}

mitk::Image::Pointer 
mitk::mitkBasicImageProcessor::ResampleInputToReference(
  mitk::Image::Pointer inputImage,
  mitk::Image::Pointer referenceImage,
  int parameter)
{
  mitk::Image::Pointer outputImage = mitk::Image::New();

  switch (referenceImage->GetPixelType().GetComponentType())
  {
    case itk::ImageIOBase::UCHAR:
    {
      typedef itk::Image<unsigned char, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::CHAR:
    {      
      typedef itk::Image<char, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      typedef itk::Image<unsigned short, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      typedef itk::Image<short, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      typedef itk::Image<unsigned int, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::INT:
    {
      typedef itk::Image<int, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      typedef itk::Image<float, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {
      typedef itk::Image<double, 3> ImageType;
      ImageType::Pointer itkInputImage = ImageType::New();
      ImageType::Pointer itkReferenceImage = ImageType::New();
      ImageType::Pointer itkResultImage = 0;
      mitk::CastToItkImage( inputImage, itkInputImage );
      mitk::CastToItkImage( referenceImage, itkReferenceImage );
      itkResultImage = ResampleTo<ImageType>(itkInputImage, itkReferenceImage, parameter);
      itkResultImage->DisconnectPipeline();

      mitk::CastToMitkImage(itkResultImage, outputImage);
      
      outputImage->DisconnectPipeline();
      break;
    }

    default:
      return 0;
  }

  return outputImage;
}
