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
#ifndef QMITKIMAGESTATISTICSCALCULATIONTHREAD_H_INCLUDED
#define QMITKIMAGESTATISTICSCALCULATIONTHREAD_H_INCLUDED

//QT headers
#include <QThread>
#include <QEvent>

//mitk headers
#include "mitkImage.h"
#include "mitkPlanarFigure.h"
#include "mitkImageStatisticsCalculator.h"

// itk headers
#ifndef __itkHistogram_h
#include <itkHistogram.h>
#endif


/** /brief This class is executed as background thread for image statistics calculation.
  * Documentation: This class is derived from QThread and is intended to be used by QmitkImageStatisticsView
                   to run the image statistics calculation in a background thread keepung the gui usable.
  *   \ingroup Plugins/MeasurementToolbox
  */

class  QmitkImageStatisticsCalculationThread : public QThread
{
  Q_OBJECT

public:

  typedef itk::Statistics::Histogram<double> HistogramType;

  /*! 
  /brief standard constructor. */
  QmitkImageStatisticsCalculationThread();
  /*! 
  /brief standard destructor. */
  ~QmitkImageStatisticsCalculationThread();
  /*! 
  /brief Initializes the object with necessary data. */
  void Initialize( mitk::Image::Pointer image, mitk::Image::Pointer binaryImage, mitk::PlanarFigure::Pointer planarFig );
  /*! 
  /brief returns the calculated image statistics. */
  mitk::ImageStatisticsCalculator::Statistics GetStatisticsData();
  /*! 
  /brief */
  mitk::Image::Pointer GetStatisticsImage();
  /*! 
  /brief Set the time step of the image you want to process. */
  void SetTimeStep( int times );
  /*! 
  /brief Get the time step of the image you want to process. */
  int GetTimeStep();
  /*! 
  /brief Set flag to ignore zero valued voxels */
  void SetIgnoreZeroValueVoxel( bool _arg );
  /*! 
  /brief Get status of zero value voxel ignoring. */
  bool GetIgnoreZeroValueVoxel();
  /*! 
  /brief Returns the histogram of the currently selected time step. */
  HistogramType::Pointer GetTimeStepHistogram();
  /*! 
  /brief Returns a flag indicating if the statistics have changed during calculation */
  bool GetStatisticsChangedFlag();
  /*! 
  /brief Returns a flag the indicates if the statistics are updated successfully */
  bool GetStatisticsUpdateSuccessFlag();
  /*! 
  /brief Method called once the thread is executed. */
  void run();

private:
  //member declaration
  mitk::Image::Pointer m_StatisticsImage;                         ///< member variable holds the input image for which the statistics need to be calculated.
  mitk::Image::Pointer m_BinaryMask;                              ///< member variable holds the binary mask image for segmentation image statistics calculation.
  mitk::PlanarFigure::Pointer m_PlanarFigureMask;                 ///< member variable holds the planar figure for segmentation image statistics calculation.
  mitk::ImageStatisticsCalculator::Statistics m_StatisticsStruct; ///< member variable holds the result struct.
  int m_TimeStep;                                                 ///< member variable holds the time step for statistics calculation
  bool m_IgnoreZeros;                                             ///< member variable holds flag to indicate if zero valued voxel should be suppressed
  bool m_StatisticChanged;                                        ///< flag set if statistics have changed
  bool m_CalculationSuccessful;                                   ///< flag set if statistics calculation was successful
  HistogramType::Pointer m_TimeStepHistogram;                     ///< member holds the histogram of the current time step.
};
#endif // QMITKIMAGESTATISTICSCALCULATIONTHREAD_H_INCLUDED
