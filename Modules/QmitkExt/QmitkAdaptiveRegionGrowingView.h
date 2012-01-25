/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date: 2009-07-15 08:05:19 +0200 (Mi, 15 Jul 2009) $
Version:   $Revision: 11185 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef QMITK_AdaptiveRegionGrowingView_H
#define QMITK_AdaptiveRegionGrowingView_H


#include "mitkDataStorage.h"
#include "itkImage.h"
#include "mitkGeometry3D.h"
#include "mitkPointSet.h"
//#include "mitkVesselSegmentationUIExports.h"

#include "qwidget.h"
#include "ui_QmitkAdaptiveRegionGrowingViewControls.h"

class QmitkStdMultiWidget;
class DataNode;
class QmitkAdaptiveRegionGrowingViewControls;


/*!
*
* \brief QmitkAdaptiveRegionGrowingView
*
* Adaptive Region Growing View class of the segmentation part of the navi-Broncho-Suite.
* User can choose between automatic or manual tracheobronchial tree and blood vessel segmentation
*
*/

class QmitkAdaptiveRegionGrowingView : public QWidget
{

  Q_OBJECT

public:
  typedef QmitkAdaptiveRegionGrowingView   Self;

  /**
  * @brief Constructor.
  **/
  QmitkAdaptiveRegionGrowingView(QWidget *parent=0);

  /** \brief Destructor. */
  virtual ~QmitkAdaptiveRegionGrowingView();

  /** \brief Method to create the connections for the component. This Method is obligatory even if no connections is needed*/
  virtual void CreateConnections();

  ///** \brief Method to set the default data storage.*/
  virtual void SetDataStorage(mitk::DataStorage* dataStorage);

  void SetMultiWidget(QmitkStdMultiWidget* multiWidget);

  void SetDataNodeNames(std::string labledSegmentation, std::string binaryImage, /*std::string vesselTree,*/ std::string surface, std::string seedPoint);

  void EnableControls(bool enable);

  void SetInputImageNode(mitk::DataNode* node);

  void Deactivated();
  void Activated();


  /**
  * @brief The created GUI from the .ui-File. This Attribute is obligatory
  */
  Ui::QmitkAdaptiveRegionGrowingViewControls m_Controls;


 // itkSetMacro(labledSegmentation,std::string);

//signals:

protected slots:

    void SetSeedPointToggled(bool toggled);

    void RunSegmentation();

    void ChangeLevelWindow(int newValue);//called, when the Level Window is changed via the slider in the ControlWidget

    //****called, when the slider-position is modified via the +/- buttons
    void IncreaseSlider();

    void DecreaseSlider();
    //***

    void ConfirmSegmentation();

    void UseVolumeRendering(bool on);

    void OnDefineThresholdBoundaries(bool);

    void SetLowerThresholdValue(int lowerThreshold);

    void SetUpperThresholdValue(int upperThreshold);

    void SetRegionGrowingDirectionUpwards(bool);

protected:

  //Pointer to the main widget to be able to reach the renderer
  QmitkStdMultiWidget* m_MultiWidget;

  mitk::DataStorage* m_DataStorage;

  mitk::DataNode::Pointer m_InputImageNode;

  void DeactivateSeedPointMode();
  void ActivateSeedPointMode();

  void OnPointAdded();

private:

  std::string m_NAMEFORORGIMAGE;
  std::string m_NAMEFORSEEDPOINT;
  std::string m_NAMEFORLABLEDSEGMENTATIONIMAGE;
  std::string m_NAMEFORBINARYIMAGE;
  std::string m_NAMEFORSURFACE;

  mitk::ScalarType m_LOWERTHRESHOLD; //Hounsfield value
  mitk::ScalarType m_UPPERTHRESHOLD; //Hounsfield value
  mitk::ScalarType m_SeedPointValueMean;

  int m_DetectedLeakagePoint;

  bool m_CurrentRGDirectionIsUpwards; // defines fixed threshold (true = LOWERTHRESHOLD fixed, false = UPPERTHRESHOLD fixed)

  int m_SeedpointValue;
  bool m_SliderInitialized;
  bool m_UseVolumeRendering;
  bool m_UpdateSuggestedThreshold;
  float m_SuggestedThValue;

  long m_PointSetAddObserverTag;

  template < typename TPixel, unsigned int VImageDimension >
  void StartRegionGrowing( itk::Image< TPixel, VImageDimension >* itkImage, mitk::Geometry3D* imageGeometry, mitk::PointSet::PointType seedPoint );

  void InitializeLevelWindow();

  void EnableVolumeRendering(bool enable);

  void UpdateVolumeRenderingThreshold(int thValue);

};


#endif
