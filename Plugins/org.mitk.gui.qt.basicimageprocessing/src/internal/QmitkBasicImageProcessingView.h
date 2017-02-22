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

#if !defined(QmitkBasicImageProcessingView_H__INCLUDED)
#define QmitkBasicImageProcessingView_H__INCLUDED

#include <QmitkAbstractView.h>

#include <org_mitk_gui_qt_basicimageprocessing_Export.h>
#include "ui_QmitkBasicImageProcessingViewControls.h"

#include "QmitkStepperAdapter.h"

#include <berryISelectionListener.h>

#include <mitkDataStorageSelection.h>
#include <mitkBasicImageProcessor.h>

/*!
\brief This module allows to use some basic image processing filters for preprocessing, image enhancement and testing purposes

Several basic ITK image processing filters, like denoising, morphological and edge detection
are encapsulated in this module and can be selected via a list and an intuitive parameter input.
The selected filter will be applied on the image, and a new image showing the output is displayed
as result.
Also, some image arithmetic operations are available.

Images can be 3D or 4D.
In the 4D case, the filters work on the 3D image selected via the
time slider. The result is also a 3D image.

\class QmitkBasicImageProcessing
\author Tobias Schwarz
\version 1.0 (3M3)
\date 2009-05-10
\ingroup Bundles
*/

#define PROC_COORD_TOLERANCE 0.001
#define PROC_DIR_TOLERANCE   0.001


class BASICIMAGEPROCESSING_EXPORT QmitkBasicImageProcessing : public QmitkAbstractView
{
  Q_OBJECT

public:

  /*!
  \brief default constructor
  */
  QmitkBasicImageProcessing();

  /*!
  \brief default destructor
  */
  virtual ~QmitkBasicImageProcessing();

  /*!
  \brief method for creating the widget containing the application controls, like sliders, buttons etc.
  */
  virtual void CreateQtPartControl(QWidget *parent) override;

  /*!
  \brief method for creating the connections of main and control widget
  */
  virtual void CreateConnections();

  ///
  /// Sets the focus to an internal widget.
  ///
  virtual void SetFocus() override;

  /*!
  \brief Invoked when the DataManager selection changed
  */
  virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer part, const QList<mitk::DataNode::Pointer>& nodes) override;


  protected slots:

    /*
    * When an action is selected in the "one image ops" list box
    */
    void SelectAction(int action);

    /*
    * When an action is selected in the "two image ops" list box
    */
    void SelectAction2(int operation);

    /*
    * The "Execute" button in the "one image ops" box was triggered
    */
    void StartButtonClicked();

    /*
    * The "Execute" button in the "two image ops" box was triggered
    */
    void StartButton2Clicked();

    /*
    *  Switch between the one and the two image operations GUI
    */
    void ChangeGUI();

    void SelectInterpolator(int interpolator);

private:

  /*
  * After a one image operation, reset the "one image ops" panel
  */
  void ResetOneImageOpPanel();

  /*
  * Helper method to reset the parameter set panel
  */
  void ResetParameterPanel();

  /*
  * After a two image operation, reset the "two image ops" panel
  */
  void ResetTwoImageOpPanel();

  /*!
  * controls containing sliders for scrolling through the slices
  */
  Ui::QmitkBasicImageProcessingViewControls      * m_Controls;
  mitk::DataStorageSelection::Pointer              m_SelectedImageNode;
  QmitkStepperAdapter*                             m_TimeStepperAdapter;

  mitk::mitkBasicImageProcessor::ActionType        m_SelectedAction;
  mitk::mitkBasicImageProcessor::OperationType     m_SelectedOperation;
  mitk::mitkBasicImageProcessor::InterpolationType m_SelectedInterpolation;

};

#endif // !defined(QmitkBasicImageProcessing_H__INCLUDED)


