#include "SkeletonViz.h"
#include "ClippingPlaneInteractionCallback.h"
#include "ClippingPlaneMaker.h"
#include "ClippingPlaneAddRemoveCallback.h"
#include "VisualizationModeCallback.h"
#include "BoneOnlyPipeline.h"
#include "BoneAndTissuePipeline.h"
#include <vtkRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera.h>

int main(int argc, char **argv) {
    // Set the default visualization type
    currentVisualizationType = BONE_ONLY;       // Segmented skeletal structure only
    //currentVisualizationType = BONE_AND_TISSUE; // Bone in the context of partially-visible surrounding tissue

    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    ReaderType::Pointer reader = ReaderType::New();

    // Kinda validate input to support both command line input and make debugger use simpler.
    // TODO: Improve checks on input
    if (argc == 4) {
        nameGenerator->SetSeriesFormat(argv[1]);
        nameGenerator->SetStartIndex(std::stoi(argv[2]));
        nameGenerator->SetEndIndex(std::stoi(argv[3]));
    }
    else {
        std::cout << "USAGE: " << std::endl;
        std::cout << "SkeletonViz.exe <File Path Template> <Start Index> <End Index>" << std::endl;
        std::cout << "File Path Template -- A standardized file name/path for each numbered image" << std::endl;
        std::cout << "      with \"%d\" standing in for the number, such as \"C:\\foo %d.tif\" for" << std::endl;
        std::cout << "      files foo 1.tif, foo 2.tif, etc." << std::endl;
        std::cout << "Start Index -- Number of the first image." << std::endl;
        std::cout << "End Index -- Number of the last image." << std::endl << std::endl;
        std::cout << "For Example:" << std::endl;
        std::cout << "SkeletonViz.exe \"C:\\images\\img (%d).tif\" 1 404" << std::endl;
        std::cout << "To load files img (1).tif through img (404).tif" << std::endl;
    }

    // Generate file paths for the reader
    nameGenerator->SetIncrementIndex(1);
    std::vector<std::string> filePaths = nameGenerator->GetFileNames();

    // Load slice image files into memory with series reader.
    reader->SetFileNames(filePaths);
    reader->Update();

    // Set up VTK renderer and camera
    vtkRenderer *ren = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(istyle);

    // Create pipelines
    BoneAndTissuePipeline boneAndTissue = BoneAndTissuePipeline(reader);
    BoneOnlyPipeline boneOnly = BoneOnlyPipeline(reader);

    // Add a visualization mode callback to change rendering modes (i.e. change between pipelines) on the fly
    VisualizationModeCallback *modeCallback = VisualizationModeCallback::New();
    modeCallback->ren = ren;
    modeCallback->renWin = renWin;
    modeCallback->iren = iren;
    modeCallback->boneOnly = &boneOnly;
    modeCallback->boneAndTissue = &boneAndTissue;
    iren->AddObserver(vtkCommand::KeyPressEvent, modeCallback);

    // Start in BoneOnly mode
    modeCallback->ActivateBoneOnly();
	
	// Render the scene
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

	// Clean Up
	iren->Delete();
	ren->Delete();
	renWin->Delete();
    modeCallback->Delete();

	return 0;
}
