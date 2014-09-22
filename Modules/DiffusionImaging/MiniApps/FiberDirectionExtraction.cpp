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

#include "MiniAppManager.h"
#include <mitkBaseDataIOFactory.h>
#include <mitkBaseData.h>
#include <mitkImageCast.h>
#include <mitkImageToItk.h>
#include <metaCommand.h>
#include "ctkCommandLineParser.h"
#include <usAny.h>
#include <itkImageFileWriter.h>
#include <mitkIOUtil.h>
#include <boost/lexical_cast.hpp>
#include <itkEvaluateDirectionImagesFilter.h>
#include <itkTractsToVectorImageFilter.h>
#include <mitkCoreObjectFactory.h>

#define _USE_MATH_DEFINES
#include <math.h>

int FiberDirectionExtraction(int argc, char* argv[])
{
    ctkCommandLineParser parser;

    parser.setTitle("Fiber Direction Extraction");
    parser.setCategory("Fiber Tracking and Processing Methods");
    parser.setDescription("");
    parser.setContributor("MBI");

    parser.setArgumentPrefix("--", "-");
    parser.addArgument("input", "i", ctkCommandLineParser::InputFile, "Input:", "input tractogram (.fib, vtk ascii file format)", us::Any(), false);
    parser.addArgument("out", "o", ctkCommandLineParser::OutputDirectory, "Output:", "output root", us::Any(), false);
    parser.addArgument("mask", "m", ctkCommandLineParser::InputFile, "Mask:", "mask image");
    parser.addArgument("athresh", "a", ctkCommandLineParser::Float, "Angular threshold:", "angular threshold in degrees. closer fiber directions are regarded as one direction and clustered together.", 25, true);
    parser.addArgument("peakthresh", "t", ctkCommandLineParser::Float, "Peak size threshold:", "peak size threshold relative to largest peak in voxel", 0.2, true);
    parser.addArgument("verbose", "v", ctkCommandLineParser::Bool, "Verbose:", "output optional and intermediate calculation results");
    parser.addArgument("numdirs", "d", ctkCommandLineParser::Int, "Max. num. directions:", "maximum number of fibers per voxel", 3, true);
    parser.addArgument("normalize", "n", ctkCommandLineParser::Bool, "Normalize:", "normalize vectors");

    map<string, us::Any> parsedArgs = parser.parseArguments(argc, argv);
    if (parsedArgs.size()==0)
        return EXIT_FAILURE;

    string fibFile = us::any_cast<string>(parsedArgs["input"]);

    string maskImage("");
    if (parsedArgs.count("mask"))
        maskImage = us::any_cast<string>(parsedArgs["mask"]);

    float peakThreshold = 0.2;
    if (parsedArgs.count("peakthresh"))
        peakThreshold = us::any_cast<float>(parsedArgs["peakthresh"]);

    float angularThreshold = 25;
    if (parsedArgs.count("athresh"))
        angularThreshold = us::any_cast<float>(parsedArgs["athresh"]);

    string outRoot = us::any_cast<string>(parsedArgs["out"]);

    bool verbose = false;
    if (parsedArgs.count("verbose"))
        verbose = us::any_cast<bool>(parsedArgs["verbose"]);

    int maxNumDirs = 3;
    if (parsedArgs.count("numdirs"))
        maxNumDirs = us::any_cast<int>(parsedArgs["numdirs"]);

    bool normalize = false;
    if (parsedArgs.count("normalize"))
        normalize = us::any_cast<bool>(parsedArgs["normalize"]);

    try
    {
        typedef itk::Image<unsigned char, 3>                                    ItkUcharImgType;
        typedef itk::Image< itk::Vector< float, 3>, 3 >                         ItkDirectionImage3DType;
        typedef itk::VectorContainer< unsigned int, ItkDirectionImage3DType::Pointer >   ItkDirectionImageContainerType;

        // load fiber bundle
        mitk::FiberBundleX::Pointer inputTractogram = dynamic_cast<mitk::FiberBundleX*>(mitk::IOUtil::LoadDataNode(fibFile)->GetData());

        // load/create mask image
        ItkUcharImgType::Pointer itkMaskImage = NULL;
        if (maskImage.compare("")!=0)
        {
            MITK_INFO << "Using mask image";
            itkMaskImage = ItkUcharImgType::New();
            mitk::Image::Pointer mitkMaskImage = dynamic_cast<mitk::Image*>(mitk::IOUtil::LoadDataNode(maskImage)->GetData());
            mitk::CastToItkImage<ItkUcharImgType>(mitkMaskImage, itkMaskImage);
        }

        // extract directions from fiber bundle
        itk::TractsToVectorImageFilter<float>::Pointer fOdfFilter = itk::TractsToVectorImageFilter<float>::New();
        fOdfFilter->SetFiberBundle(inputTractogram);
        fOdfFilter->SetMaskImage(itkMaskImage);
        fOdfFilter->SetAngularThreshold(cos(angularThreshold*M_PI/180));
        fOdfFilter->SetNormalizeVectors(normalize);
        fOdfFilter->SetUseWorkingCopy(false);
        fOdfFilter->SetSizeThreshold(peakThreshold);
        fOdfFilter->SetMaxNumDirections(maxNumDirs);
        fOdfFilter->Update();
        ItkDirectionImageContainerType::Pointer directionImageContainer = fOdfFilter->GetDirectionImageContainer();

        // write direction images
        for (unsigned int i=0; i<directionImageContainer->Size(); i++)
        {
            itk::TractsToVectorImageFilter<float>::ItkDirectionImageType::Pointer itkImg = directionImageContainer->GetElement(i);
            typedef itk::ImageFileWriter< itk::TractsToVectorImageFilter<float>::ItkDirectionImageType > WriterType;
            WriterType::Pointer writer = WriterType::New();

            string outfilename = outRoot;
            outfilename.append("_DIRECTION_");
            outfilename.append(boost::lexical_cast<string>(i));
            outfilename.append(".nrrd");

            MITK_INFO << "writing " << outfilename;
            writer->SetFileName(outfilename.c_str());
            writer->SetInput(itkImg);
            writer->Update();
        }

        if (verbose)
        {
            // write vector field
            mitk::FiberBundleX::Pointer directions = fOdfFilter->GetOutputFiberBundle();
            mitk::CoreObjectFactory::FileWriterList fileWriters = mitk::CoreObjectFactory::GetInstance()->GetFileWriters();
            for (mitk::CoreObjectFactory::FileWriterList::iterator it = fileWriters.begin() ; it != fileWriters.end() ; ++it)
            {
                if ( (*it)->CanWriteBaseDataType(directions.GetPointer()) ) {
                    string outfilename = outRoot;
                    outfilename.append("_VECTOR_FIELD.fib");
                    (*it)->SetFileName( outfilename.c_str() );
                    (*it)->DoWrite( directions.GetPointer() );
                }
            }

            // write num direction image
            {
                ItkUcharImgType::Pointer numDirImage = fOdfFilter->GetNumDirectionsImage();
                typedef itk::ImageFileWriter< ItkUcharImgType > WriterType;
                WriterType::Pointer writer = WriterType::New();

                string outfilename = outRoot;
                outfilename.append("_NUM_DIRECTIONS.nrrd");

                MITK_INFO << "writing " << outfilename;
                writer->SetFileName(outfilename.c_str());
                writer->SetInput(numDirImage);
                writer->Update();
            }
        }

        MITK_INFO << "DONE";
    }
    catch (itk::ExceptionObject e)
    {
        MITK_INFO << e;
        return EXIT_FAILURE;
    }
    catch (std::exception e)
    {
        MITK_INFO << e.what();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        MITK_INFO << "ERROR!?!";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
RegisterDiffusionMiniApp(FiberDirectionExtraction);
