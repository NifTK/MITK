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

#include "QmitkBasicImageProcessingView.h"

// QT includes (GUI)
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qmessagebox.h>

// Berry includes (selection service)
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// MITK includes (GUI)
#include "QmitkStdMultiWidget.h"
#include "QmitkDataNodeSelectionProvider.h"
#include "mitkDataNodeObject.h"

// MITK includes (general)
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateDimension.h"
#include "mitkNodePredicateAnd.h"
#include "mitkImageTimeSelector.h"
#include "mitkVectorImageMapper2D.h"
#include "mitkProperties.h"

// Includes for image casting between ITK and MITK
#include "mitkImageCast.h"
#include "mitkITKImageImport.h"


QmitkBasicImageProcessing::QmitkBasicImageProcessing()
: QmitkAbstractView(),
  m_Controls(NULL),
  m_SelectedImageNode(NULL),
  m_TimeStepperAdapter(NULL)
{
}

QmitkBasicImageProcessing::~QmitkBasicImageProcessing()
{
}

void QmitkBasicImageProcessing::CreateQtPartControl(QWidget *parent)
{
  if (m_Controls == NULL)
  {
    m_Controls = new Ui::QmitkBasicImageProcessingViewControls;
    m_Controls->setupUi(parent);
    this->CreateConnections();


    //setup predictaes for combobox

    mitk::NodePredicateDimension::Pointer dimensionPredicate = mitk::NodePredicateDimension::New(3);
    mitk::NodePredicateDataType::Pointer imagePredicate = mitk::NodePredicateDataType::New("Image");
    m_Controls->m_ImageSelector2->SetDataStorage(this->GetDataStorage());
    m_Controls->m_ImageSelector2->SetPredicate(mitk::NodePredicateAnd::New(dimensionPredicate, imagePredicate));
  }
  m_Controls->gbTwoImageOps->hide();

  m_SelectedImageNode = mitk::DataStorageSelection::New(this->GetDataStorage(), false);

  // Setup Controls

  this->m_Controls->cbWhat1->clear();
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::NOACTIONSELECTED, "Please select operation");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_DENOISING, "--- Denoising ---");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::GAUSSIAN, "Gaussian");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::MEDIAN, "Median");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::TOTALVARIATION, "Total Variation");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_MORPHOLOGICAL, "--- Morphological ---");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::DILATION, "Dilation");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::EROSION, "Erosion");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::OPENING, "Opening");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::CLOSING, "Closing");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_EDGE_DETECTION, "--- Edge Detection ---");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::GRADIENT, "Gradient");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::LAPLACIAN, "Laplacian (2nd Derivative)");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::SOBEL, "Sobel Operator");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_MISC, "--- Misc ---");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::THRESHOLD, "Threshold");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::BINARYTHRESHOLD, "Binary Threshold");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::INVERSION, "Image Inversion");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::DOWNSAMPLING, "Downsampling");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::FLIPPING, "Flipping");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::RESAMPLING, "Resample to");
  this->m_Controls->cbWhat1->insertItem( mitk::mitkBasicImageProcessor::RESCALE, "Rescale image values");

  this->m_Controls->cbWhat2->clear();
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::TWOIMAGESNOACTIONSELECTED, "Please select on operation" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_ARITHMETIC, "--- Arithmetric operations ---" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::ADD, "Add to Image 1:" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::SUBTRACT, "Subtract from Image 1:" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::MULTIPLY, "Multiply with Image 1:" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::RESAMPLE_TO, "Resample Image 1 to fit geometry:" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::DIVIDE, "Divide Image 1 by:" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::CATEGORY_BOOLEAN, "--- Boolean operations ---" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::AND, "AND" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::OR, "OR" );
  this->m_Controls->cbWhat2->insertItem( mitk::mitkBasicImageProcessor::XOR, "XOR" );

  this->m_Controls->cbParam4->clear();
  this->m_Controls->cbParam4->insertItem( mitk::mitkBasicImageProcessor::LINEAR, "Linear" );
  this->m_Controls->cbParam4->insertItem( mitk::mitkBasicImageProcessor::NEAREST, "Nearest neighbor" );

  this->m_Controls->cbParam5->clear();
  this->m_Controls->cbParam5->insertItem( mitk::mitkBasicImageProcessor::LINEAR, "Linear" );
  this->m_Controls->cbParam5->insertItem( mitk::mitkBasicImageProcessor::NEAREST, "Nearest neighbor" );

  m_Controls->dsbParam1->hide();
  m_Controls->dsbParam2->hide();
  m_Controls->dsbParam3->hide();
  m_Controls->tlParam3->hide();
  m_Controls->tlParam4->hide();
  m_Controls->cbParam4->hide();
}

void QmitkBasicImageProcessing::CreateConnections()
{
  if ( m_Controls )
  {
    connect( (QObject*)(m_Controls->cbWhat1), SIGNAL( activated(int) ), this, SLOT( SelectAction(int) ) );
    connect( (QObject*)(m_Controls->btnDoIt), SIGNAL(clicked()),(QObject*) this, SLOT(StartButtonClicked()));

    connect( (QObject*)(m_Controls->cbWhat2), SIGNAL( activated(int) ), this, SLOT( SelectAction2(int) ) );
    connect( (QObject*)(m_Controls->btnDoIt2), SIGNAL(clicked()),(QObject*) this, SLOT(StartButton2Clicked()));

    connect( (QObject*)(m_Controls->rBOneImOp), SIGNAL( clicked() ), this, SLOT( ChangeGUI() ) );
    connect( (QObject*)(m_Controls->rBTwoImOp), SIGNAL( clicked() ), this, SLOT( ChangeGUI() ) );

    connect( (QObject*)(m_Controls->cbParam4), SIGNAL( activated(int) ), this, SLOT( SelectInterpolator(int) ) );
  }

  m_TimeStepperAdapter = new QmitkStepperAdapter((QObject*) m_Controls->sliceNavigatorTime,
    GetRenderWindowPart()->GetTimeNavigationController()->GetTime(), "sliceNavigatorTimeFromBIP");
}

void QmitkBasicImageProcessing::SetFocus()
{
  m_Controls->rBOneImOp->setFocus();
}

//datamanager selection changed
void QmitkBasicImageProcessing::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer>& nodes)
{
  //any nodes there?
  if (!nodes.empty())
  {
  // reset GUI
//  this->ResetOneImageOpPanel();
  m_Controls->sliceNavigatorTime->setEnabled(false);
  m_Controls->leImage1->setText("Select an Image in Data Manager");
  m_Controls->tlWhat1->setEnabled(false);
  m_Controls->cbWhat1->setEnabled(false);
  m_Controls->tlWhat2->setEnabled(false);
  m_Controls->cbWhat2->setEnabled(false);

  m_SelectedImageNode->RemoveAllNodes();
  //get the selected Node
  mitk::DataNode* _DataNode = nodes.front();
  *m_SelectedImageNode = _DataNode;
  //try to cast to image
  mitk::Image::Pointer tempImage = dynamic_cast<mitk::Image*>(m_SelectedImageNode->GetNode()->GetData());

    //no image
    if( tempImage.IsNull() || (tempImage->IsInitialized() == false) )
    {
      m_Controls->leImage1->setText("Not an image.");
      return;
    }

    //2D image
    if( tempImage->GetDimension() < 3)
    {
      m_Controls->leImage1->setText("2D images are not supported.");
      return;
    }

    //image
    m_Controls->leImage1->setText(QString(m_SelectedImageNode->GetNode()->GetName().c_str()));

    // button coding
    if ( tempImage->GetDimension() > 3 )
    {
      m_Controls->sliceNavigatorTime->setEnabled(true);
      m_Controls->tlTime->setEnabled(true);
    }
    m_Controls->tlWhat1->setEnabled(true);
    m_Controls->cbWhat1->setEnabled(true);
    m_Controls->tlWhat2->setEnabled(true);
    m_Controls->cbWhat2->setEnabled(true);
  }
}

void QmitkBasicImageProcessing::ChangeGUI()
{
  if(m_Controls->rBOneImOp->isChecked())
  {
    m_Controls->gbTwoImageOps->hide();
    m_Controls->gbOneImageOps->show();
  }
  else if(m_Controls->rBTwoImOp->isChecked())
  {
    m_Controls->gbOneImageOps->hide();
    m_Controls->gbTwoImageOps->show();
  }
}

void QmitkBasicImageProcessing::ResetOneImageOpPanel()
{
  m_Controls->tlParam1->setText("Param1");
  m_Controls->tlParam2->setText("Param2");

  m_Controls->cbWhat1->setCurrentIndex(0);

  m_Controls->tlTime->setEnabled(false);

  this->ResetParameterPanel();

  m_Controls->btnDoIt->setEnabled(false);
  m_Controls->cbHideOrig->setEnabled(false);
}

void QmitkBasicImageProcessing::ResetParameterPanel()
{
  m_Controls->tlParam->setEnabled(false);
  m_Controls->tlParam1->setEnabled(false);
  m_Controls->tlParam2->setEnabled(false);
  m_Controls->tlParam3->setEnabled(false);
  m_Controls->tlParam4->setEnabled(false);

  m_Controls->sbParam1->setEnabled(false);
  m_Controls->sbParam2->setEnabled(false);
  m_Controls->dsbParam1->setEnabled(false);
  m_Controls->dsbParam2->setEnabled(false);
  m_Controls->dsbParam3->setEnabled(false);
  m_Controls->cbParam4->setEnabled(false);
  m_Controls->sbParam1->setValue(0);
  m_Controls->sbParam2->setValue(0);
  m_Controls->dsbParam1->setValue(0);
  m_Controls->dsbParam2->setValue(0);
  m_Controls->dsbParam3->setValue(0);

  m_Controls->sbParam1->show();
  m_Controls->sbParam2->show();
  m_Controls->dsbParam1->hide();
  m_Controls->dsbParam2->hide();
  m_Controls->dsbParam3->hide();
  m_Controls->cbParam4->hide();
  m_Controls->tlParam3->hide();
  m_Controls->tlParam4->hide();
}

void QmitkBasicImageProcessing::ResetTwoImageOpPanel()
{
  m_Controls->cbWhat2->setCurrentIndex(0);

  m_Controls->tlImage2->setEnabled(false);
  m_Controls->m_ImageSelector2->setEnabled(false);

  m_Controls->btnDoIt2->setEnabled(false);
}

void QmitkBasicImageProcessing::SelectAction(int action)
{
  if ( ! m_SelectedImageNode->GetNode() ) return;

  // Prepare GUI
  this->ResetParameterPanel();
  m_Controls->btnDoIt->setEnabled(false);
  m_Controls->cbHideOrig->setEnabled(false);

  QString text1 = "No Parameters";
  QString text2 = "No Parameters";
  QString text3 = "No Parameters";
  QString text4 = "No Parameters";

  if (action != 20)
  {
    m_Controls->dsbParam1->hide();
    m_Controls->dsbParam2->hide();
    m_Controls->dsbParam3->hide();
    m_Controls->tlParam3->hide();
    m_Controls->tlParam4->hide();
    m_Controls->sbParam1->show();
    m_Controls->sbParam2->show();
    m_Controls->cbParam4->hide();
  }
  // check which operation the user has selected and set parameters and GUI accordingly
  switch (action)
  {
  case mitk::mitkBasicImageProcessor::GAUSSIAN:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::GAUSSIAN;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Variance:";

      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 2 );
      break;
    }

  case mitk::mitkBasicImageProcessor::MEDIAN:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::MEDIAN;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Radius:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 3 );
      break;
    }

  case mitk::mitkBasicImageProcessor::TOTALVARIATION:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::TOTALVARIATION;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      m_Controls->tlParam2->setEnabled(true);
      m_Controls->sbParam2->setEnabled(true);
      text1 = "Number Iterations:";
      text2 = "Regularization\n(Lambda/1000):";
      m_Controls->sbParam1->setMinimum( 1 );
      m_Controls->sbParam1->setMaximum( 1000 );
      m_Controls->sbParam1->setValue( 40 );
      m_Controls->sbParam2->setMinimum( 0 );
      m_Controls->sbParam2->setMaximum( 100000 );
      m_Controls->sbParam2->setValue( 1 );
      break;
    }

  case mitk::mitkBasicImageProcessor::DILATION:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::DILATION;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Radius:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 3 );
      break;
    }

  case mitk::mitkBasicImageProcessor::EROSION:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::EROSION;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Radius:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 3 );
      break;
    }

  case mitk::mitkBasicImageProcessor::OPENING:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::OPENING;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Radius:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 3 );
      break;
    }

  case mitk::mitkBasicImageProcessor::CLOSING:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::CLOSING;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "&Radius:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 3 );
      break;
    }

  case mitk::mitkBasicImageProcessor::GRADIENT:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::GRADIENT;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "Sigma of Gaussian Kernel:\n(in Image Spacing Units)";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 200 );
      m_Controls->sbParam1->setValue( 2 );
      break;
    }

  case mitk::mitkBasicImageProcessor::LAPLACIAN:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::LAPLACIAN;
      break;
    }

  case mitk::mitkBasicImageProcessor::SOBEL:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::SOBEL;
      break;
    }

  case mitk::mitkBasicImageProcessor::THRESHOLD:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::THRESHOLD;
      m_Controls->sbParam1->hide();
      m_Controls->sbParam2->hide();
      m_Controls->dsbParam1->show();
      m_Controls->tlParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->dsbParam2->show();
      m_Controls->tlParam2->show();
      m_Controls->dsbParam2->setEnabled(true);
      m_Controls->tlParam2->setEnabled(true);
      text1 = "Lower threshold:";
      text2 = "Upper threshold:";
      m_Controls->dsbParam1->setMinimum( -100000 );
      m_Controls->dsbParam1->setMaximum( 100000 );
      m_Controls->dsbParam1->setValue( 0 );
      m_Controls->dsbParam2->setMinimum( -100000 );
      m_Controls->dsbParam2->setMaximum( 100000 );
      m_Controls->dsbParam2->setValue( 300 );
      break;
    }

  case mitk::mitkBasicImageProcessor::BINARYTHRESHOLD:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::BINARYTHRESHOLD;
      m_Controls->sbParam1->hide();
      m_Controls->sbParam2->hide();
      m_Controls->dsbParam1->show();
      m_Controls->tlParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->dsbParam2->show();
      m_Controls->tlParam2->show();
      m_Controls->dsbParam2->setEnabled(true);
      m_Controls->tlParam2->setEnabled(true);
      text1 = "Lower threshold:";
      text2 = "Upper threshold:";
      m_Controls->dsbParam1->setMinimum( -100000 );
      m_Controls->dsbParam1->setMaximum( 100000 );
      m_Controls->dsbParam1->setValue( 0 );
      m_Controls->dsbParam2->setMinimum( -100000 );
      m_Controls->dsbParam2->setMaximum( 100000 );
      m_Controls->dsbParam2->setValue( 300 );
      break;
    }
  case mitk::mitkBasicImageProcessor::INVERSION:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::INVERSION;
      break;
    }

  case mitk::mitkBasicImageProcessor::DOWNSAMPLING:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::DOWNSAMPLING;
      m_Controls->sbParam1->hide();
      m_Controls->sbParam2->hide();
      m_Controls->dsbParam1->show();
      m_Controls->tlParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam1->setEnabled(true);
      text1 = "Downsampling by Factor:";
      m_Controls->dsbParam1->setMinimum( 1.0 );
      m_Controls->dsbParam1->setMaximum( 100.0 );
      m_Controls->dsbParam1->setValue( 2.0 );
      break;
    }

  case mitk::mitkBasicImageProcessor::FLIPPING:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::FLIPPING;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(true);
      text1 = "Flip across axis:";
      m_Controls->sbParam1->setMinimum( 0 );
      m_Controls->sbParam1->setMaximum( 2 );
      m_Controls->sbParam1->setValue( 1 );
      break;
    }

  case mitk::mitkBasicImageProcessor::RESAMPLING:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::RESAMPLING;
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->sbParam1->setEnabled(false);
      m_Controls->sbParam1->hide();
      m_Controls->dsbParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam2->setEnabled(true);
      m_Controls->sbParam2->setEnabled(false);
      m_Controls->sbParam2->hide();
      m_Controls->dsbParam2->show();
      m_Controls->dsbParam2->setEnabled(true);
      m_Controls->tlParam3->show();
      m_Controls->tlParam3->setEnabled(true);
      m_Controls->dsbParam3->show();
      m_Controls->dsbParam3->setEnabled(true);
      m_Controls->tlParam4->show();
      m_Controls->tlParam4->setEnabled(true);
      m_Controls->cbParam4->show();
      m_Controls->cbParam4->setEnabled(true);

      m_Controls->dsbParam1->setMinimum(0.01);
      m_Controls->dsbParam1->setMaximum(10.0);
      m_Controls->dsbParam1->setSingleStep(0.1);
      m_Controls->dsbParam1->setValue(0.3);
      m_Controls->dsbParam2->setMinimum(0.01);
      m_Controls->dsbParam2->setMaximum(10.0);
      m_Controls->dsbParam2->setSingleStep(0.1);
      m_Controls->dsbParam2->setValue(0.3);
      m_Controls->dsbParam3->setMinimum(0.01);
      m_Controls->dsbParam3->setMaximum(10.0);
      m_Controls->dsbParam3->setSingleStep(0.1);
      m_Controls->dsbParam3->setValue(1.5);

      text1 = "x-spacing:";
      text2 = "y-spacing:";
      text3 = "z-spacing:";
      text4 = "Interpolation:";
      break;
    }

  case mitk::mitkBasicImageProcessor::RESCALE:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::RESCALE;
      m_Controls->sbParam1->hide();
      m_Controls->sbParam2->hide();
      m_Controls->dsbParam1->show();
      m_Controls->tlParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam1->setEnabled(true);
      m_Controls->dsbParam2->show();
      m_Controls->tlParam2->show();
      m_Controls->dsbParam2->setEnabled(true);
      m_Controls->tlParam2->setEnabled(true);
      
      m_Controls->dsbParam1->setMinimum(-100000.0);
      m_Controls->dsbParam1->setMaximum(100000.0);
      m_Controls->dsbParam1->setSingleStep(1.0);
      m_Controls->dsbParam1->setValue(0.0);
      m_Controls->dsbParam2->setMinimum(-100000.0);
      m_Controls->dsbParam2->setMaximum(100000.0);
      m_Controls->dsbParam2->setSingleStep(1.0);
      m_Controls->dsbParam2->setValue(1000.0);

      text1 = "Output minimum:";
      text2 = "Output maximum:";
      break;
    }

  case mitk::mitkBasicImageProcessor::RESCALE2:
    {
      m_SelectedAction = mitk::mitkBasicImageProcessor::RESCALE2;
      m_Controls->dsbParam1->show();
      m_Controls->tlParam1->show();
      m_Controls->dsbParam1->setEnabled(true);
      m_Controls->tlParam1->setEnabled(true);
      text1 = "Scaling value:";
      break;
    }

  default: return;
  }

  m_Controls->tlParam->setEnabled(true);
  m_Controls->tlParam1->setText(text1);
  m_Controls->tlParam2->setText(text2);
  m_Controls->tlParam3->setText(text3);
  m_Controls->tlParam4->setText(text4);

  m_Controls->btnDoIt->setEnabled(true);
  m_Controls->cbHideOrig->setEnabled(true);
}

void QmitkBasicImageProcessing::StartButtonClicked()
{
  if(!m_SelectedImageNode->GetNode()) return;

  this->BusyCursorOn();

  mitk::Image::Pointer inputImage;
  mitk::Image::Pointer resultImage;

  try
  {
    inputImage = dynamic_cast<mitk::Image*>(m_SelectedImageNode->GetNode()->GetData());
  }
  catch ( std::exception &e )
  {
  QString exceptionString = "An error occured during image loading:\n";
  exceptionString.append( e.what() );
    QMessageBox::warning( NULL, "Basic Image Processing", exceptionString , QMessageBox::Ok, QMessageBox::NoButton );
    this->BusyCursorOff();
    return;
  }

  // check if input image is valid, casting does not throw exception when casting from 'NULL-Object'
  if ( (! inputImage) || (inputImage->IsInitialized() == false) )
  {
    this->BusyCursorOff();

    QMessageBox::warning( NULL, "Basic Image Processing", "Input image is broken or not initialized. Returning.", QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }

  // check if operation is done on 4D a image time step
  if(inputImage->GetDimension() > 3)
  {
    mitk::ImageTimeSelector::Pointer timeSelector = mitk::ImageTimeSelector::New();
    timeSelector->SetInput(inputImage);
    timeSelector->SetTimeNr( ((QmitkSliderNavigatorWidget*)m_Controls->sliceNavigatorTime)->GetPos() );
    timeSelector->Update();
    inputImage = timeSelector->GetOutput();
  }

  std::stringstream nameAddition("");

  int param1 = m_Controls->sbParam1->value();
  int param2 = m_Controls->sbParam2->value();
  double dparam1 = m_Controls->dsbParam1->value();
  double dparam2 = m_Controls->dsbParam2->value();
  double dparam3 = m_Controls->dsbParam3->value();

  try
  {
    mitk::mitkBasicImageProcessor * imgProc = new mitk::mitkBasicImageProcessor();

    switch (m_SelectedAction)
    {
      case mitk::mitkBasicImageProcessor::GAUSSIAN:
        nameAddition << "_Gaussian_var_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::MEDIAN:
        nameAddition << "_Median_radius_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::TOTALVARIATION:
        nameAddition << "_TV_Iter_" << param1 << "_L_" << param2;
        break;
      case mitk::mitkBasicImageProcessor::DILATION:
        nameAddition << "_Dilated_by_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::EROSION:
        nameAddition << "_Eroded_by_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::OPENING:
        nameAddition << "_Opened_by_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::CLOSING:
        nameAddition << "_Closed_by_" << param1;
      case mitk::mitkBasicImageProcessor::GRADIENT:
        nameAddition << "_Gradient_sigma_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::LAPLACIAN:
        nameAddition << "_Second_Derivative";
        break;
      case mitk::mitkBasicImageProcessor::SOBEL:
        nameAddition << "_Sobel";
        break;
      case mitk::mitkBasicImageProcessor::THRESHOLD:
        nameAddition << "_Threshold";
        break;
      case mitk::mitkBasicImageProcessor::BINARYTHRESHOLD:
        nameAddition << "_Binary_Threshold";
        break;
      case mitk::mitkBasicImageProcessor::INVERSION:
        nameAddition << "_Inverted";
        break;
      case mitk::mitkBasicImageProcessor::DOWNSAMPLING:
        nameAddition << "_Downsampled_by_" << param1;
        break;
      case mitk::mitkBasicImageProcessor::FLIPPING:
        nameAddition << "_Flipped" << param1;
        break;
      case mitk::mitkBasicImageProcessor::RESAMPLING:
        param1 = m_SelectedInterpolation;
        if (m_SelectedInterpolation == mitk::mitkBasicImageProcessor::NEAREST)
          nameAddition << "_Resampled_" << "Nearest";
        else
          nameAddition << "_Resampled_" << "Linear";
        break;
      case mitk::mitkBasicImageProcessor::RESCALE:
        nameAddition << "_Rescaled";
        break;
      case mitk::mitkBasicImageProcessor::RESCALE2:
        nameAddition << "_Rescaled";
        break;
    }

    resultImage = imgProc->ProcessImage(inputImage, m_SelectedAction, param1, param2, dparam1, dparam2, dparam3);

    this->BusyCursorOff();

    delete imgProc;
  }
  catch (...)
  {
    this->BusyCursorOff();
    QMessageBox::warning(NULL, "Warning", "Problem when applying filter operation. Check your input...");
    return;
  }

  // adjust level/window to new image
  mitk::LevelWindow levelwindow;
  levelwindow.SetAuto( resultImage );
  mitk::LevelWindowProperty::Pointer levWinProp = mitk::LevelWindowProperty::New();
  levWinProp->SetLevelWindow( levelwindow );

  // compose new image name
  std::string name = m_SelectedImageNode->GetNode()->GetName();
  if (name.find(".pic.gz") == name.size() -7 )
  {
    name = name.substr(0,name.size() -7);
  }
  name.append( nameAddition.str() );

  // create final result MITK data storage node
  mitk::DataNode::Pointer result = mitk::DataNode::New();
  result->SetProperty( "levelwindow", levWinProp );
  result->SetProperty( "name", mitk::StringProperty::New( name.c_str() ) );
  result->SetData( resultImage );

  //// for vector images, a different mapper is needed
  //if(isVectorImage > 1)
  //{
  //  mitk::VectorImageMapper2D::Pointer mapper =
  //    mitk::VectorImageMapper2D::New();
  //  result->SetMapper(1,mapper);
  //}

  // reset GUI to ease further processing
//  this->ResetOneImageOpPanel();

  // add new image to data storage and set as active to ease further processing
  GetDataStorage()->Add( result, m_SelectedImageNode->GetNode() );
  if ( m_Controls->cbHideOrig->isChecked() == true )
    m_SelectedImageNode->GetNode()->SetProperty( "visible", mitk::BoolProperty::New(false) );
  // TODO!! m_Controls->m_ImageSelector1->SetSelectedNode(result);

  // show the results
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  this->BusyCursorOff();

}

void QmitkBasicImageProcessing::SelectAction2(int operation)
{

  if (operation != 6)
  {
    m_Controls->tlParam5->hide();
    m_Controls->tlParam5->setEnabled(false);
    m_Controls->cbParam5->hide();
    m_Controls->cbParam5->setEnabled(false);
  }

  // check which operation the user has selected and set parameters and GUI accordingly
  switch (operation)
  {
  case 2:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::ADD;
    break;
  case 3:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::SUBTRACT;
    break;
  case 4:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::MULTIPLY;
    break;
  case 5:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::DIVIDE;
    break;
  case 6:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::RESAMPLE_TO;
    m_Controls->tlParam5->show();
    m_Controls->tlParam5->setEnabled(true);
    m_Controls->cbParam5->show();
    m_Controls->cbParam5->setEnabled(true);
    break;
  case 8:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::AND;
    break;
  case 9:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::OR;
    break;
  case 10:
    m_SelectedOperation = mitk::mitkBasicImageProcessor::XOR;
    break;
  default:
//    this->ResetTwoImageOpPanel();
    return;
  }
  m_Controls->tlImage2->setEnabled(true);
  m_Controls->m_ImageSelector2->setEnabled(true);
  m_Controls->btnDoIt2->setEnabled(true);
}

void QmitkBasicImageProcessing::StartButton2Clicked()
{
  mitk::Image::Pointer resultImage;
  
  mitk::Image::Pointer newImage1 = dynamic_cast<mitk::Image*>
    (m_SelectedImageNode->GetNode()->GetData());
  
  mitk::Image::Pointer newImage2 = dynamic_cast<mitk::Image*>
    (m_Controls->m_ImageSelector2->GetSelectedNode()->GetData());

  // check if images are valid
  if( (!newImage1) || (!newImage2) || (newImage1->IsInitialized() == false) || (newImage2->IsInitialized() == false) )
  {
    itkGenericExceptionMacro(<< "At least one of the input images are broken or not initialized. Returning");
    return;
  }

  this->BusyCursorOn();
//  this->ResetTwoImageOpPanel();

  // check if 4D image and use filter on correct time step
  int time = ((QmitkSliderNavigatorWidget*)m_Controls->sliceNavigatorTime)->GetPos();
  if(newImage1->GetDimension() > 3)
  {
    mitk::ImageTimeSelector::Pointer timeSelector = mitk::ImageTimeSelector::New();

    timeSelector->SetInput(newImage1);
    timeSelector->SetTimeNr( time );
    timeSelector->UpdateLargestPossibleRegion();
    newImage1 = timeSelector->GetOutput();
    newImage1->DisconnectPipeline();

    timeSelector->SetInput(newImage2);
    timeSelector->SetTimeNr( time );
    timeSelector->UpdateLargestPossibleRegion();
    newImage2 = timeSelector->GetOutput();
    newImage2->DisconnectPipeline();
  }

  std::string nameAddition = "";
  int param1 = m_Controls->cbParam5->currentIndex();
  
  try
  {
    mitk::mitkBasicImageProcessor * imgProc = new mitk::mitkBasicImageProcessor();

    switch (m_SelectedOperation)
    {
      case mitk::mitkBasicImageProcessor::ADD:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_Added";
        break;
      case mitk::mitkBasicImageProcessor::SUBTRACT:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_Subtracted";
        break;
      case mitk::mitkBasicImageProcessor::MULTIPLY:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_Multiplied";
        break;
      case mitk::mitkBasicImageProcessor::DIVIDE:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_Divided";
        break;
      case mitk::mitkBasicImageProcessor::AND:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_AND";
        break;
      case mitk::mitkBasicImageProcessor::OR:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_OR";
        break;
      case mitk::mitkBasicImageProcessor::XOR:
        resultImage = imgProc->ProcessTwoImages(newImage1, newImage2, m_SelectedOperation);
        nameAddition = "_XOR";
        break;
      case mitk::mitkBasicImageProcessor::RESAMPLE_TO:
        resultImage = imgProc->ResampleInputToReference(newImage1, newImage2, param1);
        nameAddition = "_Resampled";
        break;
    }

    this->BusyCursorOff();

    delete imgProc;
  }
  catch (const itk::ExceptionObject& e )
  {
    this->BusyCursorOff();
    QMessageBox::warning(NULL, "ITK Exception", e.what() );
    QMessageBox::warning(NULL, "Warning", "Problem when applying arithmetic operation to two images. Check dimensions of input images.");
    return;
  }

  // adjust level/window to new image and compose new image name
  mitk::LevelWindow levelwindow;
  levelwindow.SetAuto( resultImage );
  mitk::LevelWindowProperty::Pointer levWinProp = mitk::LevelWindowProperty::New();
  levWinProp->SetLevelWindow( levelwindow );
  std::string name = m_SelectedImageNode->GetNode()->GetName();
  if (name.find(".pic.gz") == name.size() -7 )
  {
    name = name.substr(0,name.size() -7);
  }

  // create final result MITK data storage node
  mitk::DataNode::Pointer result = mitk::DataNode::New();
  result->SetProperty( "levelwindow", levWinProp );
  result->SetProperty( "name", mitk::StringProperty::New( (name + nameAddition ).c_str() ));
  result->SetData( resultImage );
  GetDataStorage()->Add( result, m_SelectedImageNode->GetNode() );

  // show only the newly created image
  m_SelectedImageNode->GetNode()->SetProperty( "visible", mitk::BoolProperty::New(false) );
  m_Controls->m_ImageSelector2->GetSelectedNode()->SetProperty( "visible", mitk::BoolProperty::New(false) );

  // show the newly created image
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  this->BusyCursorOff();
}

void QmitkBasicImageProcessing::SelectInterpolator(int interpolator)
{
  switch (interpolator)
  {
  case 0:
    {
      m_SelectedInterpolation = mitk::mitkBasicImageProcessor::LINEAR;
      break;
    }
  case 1:
    {
      m_SelectedInterpolation = mitk::mitkBasicImageProcessor::NEAREST;
      break;
    }
  }
}
