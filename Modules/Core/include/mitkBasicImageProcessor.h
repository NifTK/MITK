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

#if !defined(mitkBasicImageProcessor_H__INCLUDED)
#define mitkBasicImageProcessor_H__INCLUDED

#include "MitkCoreExports.h"

/*!
\brief This module contains some basic image processing filters for preprocessing, image enhancement and testing purposes

Several basic ITK image processing filters, like denoising, morphological and edge detection
are encapsulated in this module. The input mitk::Image is converted to ITK, then a selected filter will be applied on it
and a new image is returned as mitk::Image. Also, some image arithmetic operations are available.

Images can be 3D or 4D.
In the 4D case, the filters work on the 3D image selected via the
time slider. The result is also a 3D image.

\sa mitkFunctionality, QObject

\class QmitkBasicImageProcessing
\author Tobias Schwarz
\version 1.0 (3M3)
\date 2009-05-10
\ingroup Bundles
*/


// Includes for image casting between ITK and MITK
#include "mitkImageCast.h"
#include "mitkITKImageImport.h"
#include "mitkImage.h"

// ITK includes (general)
#include <itkVectorImage.h>
#include <itkImageFileWriter.h>
#include <itkImageIOBase.h>
#include <itkImageDuplicator.h>
#include <itkChangeInformationImageFilter.h>




#define PROC_COORD_TOLERANCE 0.001
#define PROC_DIR_TOLERANCE   0.001

namespace mitk
{
class MITKCORE_EXPORT mitkBasicImageProcessor
{

public:

  /*!
  \brief default constructor
  */
  mitkBasicImageProcessor();

  /*!
  \brief default destructor
  */
  virtual ~mitkBasicImageProcessor();

    enum ActionType {
    NOACTIONSELECTED,
    CATEGORY_DENOISING,
    GAUSSIAN,
    MEDIAN,
    TOTALVARIATION,
    CATEGORY_MORPHOLOGICAL,
    DILATION,
    EROSION,
    OPENING,
    CLOSING,
    CATEGORY_EDGE_DETECTION,
    GRADIENT,
    LAPLACIAN,
    SOBEL,
    CATEGORY_MISC,
    THRESHOLD,
    BINARYTHRESHOLD,
    INVERSION,
    DOWNSAMPLING,
    FLIPPING,
    RESAMPLING,
    RESCALE
  };

  enum OperationType{
    TWOIMAGESNOACTIONSELECTED,
    CATEGORY_ARITHMETIC,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    RESAMPLE_TO,
    CATEGORY_BOOLEAN,
    AND,
    OR,
    XOR
  };

  enum InterpolationType{
    LINEAR,
    NEAREST
  };

  mitk::Image::Pointer ProcessImage(mitk::Image::Pointer image, ActionType action, int p1 = 0, int p2 = 0, double dp1 = 0, double dp2 = 0, double dp3 = 0);
  mitk::Image::Pointer ProcessTwoImages(mitk::Image::Pointer inputImage, mitk::Image::Pointer referenceImage, OperationType operation);
  mitk::Image::Pointer ResampleInputToReference(mitk::Image::Pointer inputImage, mitk::Image::Pointer referenceImage, int parameter);

private:

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~~~   SINGLE IMAGE OPERATIONS   ~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template <typename InputImageType>
  typename InputImageType::Pointer
  ProcessImageByType(InputImageType* input, ActionType action, int p1 = 0, int p2 = 0, double dp1 = 0, double dp2 = 0, double dp3 = 0);
  
  template <typename InputImageType>
  typename InputImageType::Pointer
    GaussianFilter( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    MedianFilter( InputImageType* input, int param1);

  //template <typename InputImageType>
  //typename InputImageType::Pointer
  //  TotalVariationFilter( InputImageType* input, int param1, int param2);

  template <typename InputImageType>
  typename InputImageType::Pointer
    DilateImage( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    ErodeImage( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    OpenImage( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    CloseImage( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    GradientFilter( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    LaplacianFilter( InputImageType* input);

  template <typename InputImageType>
  typename InputImageType::Pointer
    SobelFilter( InputImageType* input);

  template <typename InputImageType>
  typename InputImageType::Pointer
    ThresholdFilter( InputImageType* input, double param1, double param2);

  template <typename InputImageType>
  typename InputImageType::Pointer
    BinaryThresholdFilter( InputImageType* input, double param1, double param2);

  template <typename InputImageType>
  typename InputImageType::Pointer
    InvertImage( InputImageType* input);

  template <typename InputImageType>
  typename InputImageType::Pointer
    DownsampleImage( InputImageType* input, double dparam1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    FlipImage( InputImageType* input, int param1);

  template <typename InputImageType>
  typename InputImageType::Pointer
    ResampleImage( InputImageType* input, int param1, double dparam1, double dparam2, double dparam3);

  template <typename InputImageType>
  typename InputImageType::Pointer
    RescaleImage( InputImageType* input, double dparam1, double dparam2, double dparam3);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~~~   MULTI IMAGE OPERATIONS   ~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  template <typename InputImageType>
  typename InputImageType::Pointer
  ProcessTwoImagesByType(InputImageType* input, InputImageType* reference, OperationType operation);

  template <typename InputImageType>
  typename InputImageType::Pointer
    AddImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    SubtractImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    MultiplyImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    DivideImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    OrImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    AndImages( InputImageType* input, InputImageType* reference);

  template <typename InputImageType>
  typename InputImageType::Pointer
    XorImages( InputImageType* input, InputImageType* reference);


  template <typename InputImageType>
  typename InputImageType::Pointer
    ResampleTo( InputImageType* input, InputImageType* reference, int param1);

};

}
#endif // !defined(QmitkBasicImageProcessing_H__INCLUDED)


