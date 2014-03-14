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

#include "mitkTestingMacros.h"
#include <mitkTestingConfig.h>
#include <mitkTestFixture.h>

#include <mitkIOUtil.h>
#include <mitkInteractionTestHelper.h>
#include <mitkStandaloneDataStorage.h>
#include <mitkToolManager.h>
#include <mitkGlobalInteraction.h>
#include <mitkDataNode.h>


class mitkToolInteractionTestSuite : public mitk::TestFixture
{

  CPPUNIT_TEST_SUITE(mitkToolInteractionTestSuite);
  /* ####### example ######
  MITK_TEST(AddToolInteractionTest);
  MITK_TEST(AddToolInteraction_4D_Test);
  #########################*/
  CPPUNIT_TEST_SUITE_END();


private:

  mitk::DataStorage* m_DataStorage;
  mitk::ToolManager::Pointer m_ToolManager;

public:

  int GetToolIdByToolName (const std::string& toolName)
  {
    //find tool from toolname
    int numberOfTools = m_ToolManager->GetTools().size();
    int toolId = 0;
    for(; toolId < numberOfTools; ++toolId)
    {
      mitk::Tool* currentTool = m_ToolManager->GetToolById(toolId);
      if(toolName.compare( currentTool->GetNameOfClass() ) == 0)
      {
        return toolId;
      }
    }
    return -1;
  }

  void RunTestWithParameters(const std::string& patientImagePath,
                             const std::string& referenceSegmentationImage,
                             const std::string& toolName,
                             const std::string& interactionPattern,
                             unsigned int timestep=0)
  {
    //Create test helper to initialize all necessary objects for interaction
    mitk::InteractionTestHelper interactionTestHelper(GetTestDataFilePath(interactionPattern));

    //Use data storage of test helper
    m_DataStorage = interactionTestHelper.GetDataStorage().GetPointer();

    //create ToolManager
    m_ToolManager = mitk::ToolManager::New(m_DataStorage);
    m_ToolManager->InitializeTools();
    m_ToolManager->RegisterClient();//This is needed because there must be at least one registered. Otherwise tools can't be activated.

    //Load patient image
    mitk::Image::Pointer patientImage = mitk::IOUtil::LoadImage(GetTestDataFilePath(patientImagePath));
    CPPUNIT_ASSERT(patientImage.IsNotNull());
    mitk::DataNode::Pointer patientImageNode = mitk::DataNode::New();
    patientImageNode->SetData(patientImage);

    //Activate tool to work with
    int toolID = GetToolIdByToolName(toolName);
    mitk::Tool* tool = m_ToolManager->GetToolById(toolID);

    CPPUNIT_ASSERT(tool != NULL);

    //Create empty segmentation working image
    mitk::DataNode::Pointer workingImageNode = mitk::DataNode::New();
    const std::string organName = "test";
    mitk::Color color;//actually it dosn't matter which color we are using
    color.SetRed(1);  //but CreateEmptySegmentationNode expects a color parameter
    color.SetGreen(0);
    color.SetBlue(0);
    workingImageNode = tool->CreateEmptySegmentationNode(patientImage, organName, color);

    CPPUNIT_ASSERT(workingImageNode.IsNotNull());
    CPPUNIT_ASSERT(workingImageNode->GetData() != NULL);

    //add images to datastorage
    interactionTestHelper.AddNodeToStorage(patientImageNode);
    interactionTestHelper.AddNodeToStorage(workingImageNode);

    //set reference and working image
    m_ToolManager->SetWorkingData(workingImageNode);
    m_ToolManager->SetReferenceData(patientImageNode);

    //set time step
    interactionTestHelper.SetTimeStep(timestep);

    //load interaction events
    m_ToolManager->ActivateTool(toolID);

    CPPUNIT_ASSERT(m_ToolManager->GetActiveTool() != NULL);

    //Start Interaction
    interactionTestHelper.PlaybackInteraction();

    //load reference segmentation image
    mitk::Image::Pointer segmentationReferenceImage = mitk::IOUtil::LoadImage(GetTestDataFilePath(referenceSegmentationImage));

    mitk::Image::Pointer currentSegmentationImage = mitk::Image::New();
    currentSegmentationImage = dynamic_cast<mitk::Image*>(workingImageNode->GetData());

    //compare reference with interaction result
    MITK_ASSERT_EQUAL(segmentationReferenceImage, currentSegmentationImage, "Reference equals interaction result." );
  }

  void setUp()
  {
  }

  void tearDown()
  {
    m_ToolManager->ActivateTool(-1);
    m_ToolManager = NULL;
  }

/*############ example ###################
  void AddToolInteractionTest()
  {
    RunTestWithParameters("Pic3D.nrrd", "Segmentation/ReferenceSegmentations/AddTool.nrrd", "AddContourTool", "Segmentation/InteractionPatterns/AddTool.xml");
  }


  void AddToolInteraction_4D_Test()
  {

    RunTestWithParameters("US4DCyl.nrrd", "Segmentation/ReferenceSegmentations/AddTool_4D.nrrd", "AddContourTool", "Segmentation/InteractionPatterns/AddTool_4D.xml", 1);
  }
#########################################*/

};

MITK_TEST_SUITE_REGISTRATION(mitkToolInteraction)
