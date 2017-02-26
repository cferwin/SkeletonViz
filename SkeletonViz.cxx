#include "BoneOnlyPipeline.h"
#include "SkeletonViz.h"
#include "ClippingPlaneInteractionCallback.h"
#include "ClippingPlaneMaker.h"
#include "ClippingPlaneAddRemoveCallback.h"
#include "VisualizationModeCallback.h"

int main(int argc, char **argv) {
    // Set the default visualization type
    currentVisualizationType = BONE_ONLY;       // Segmented skeletal structure only
    //currentVisualizationType = BONE_AND_TISSUE; // Bone in the context of partially-visible surrounding tissue

    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    ReaderType::Pointer reader = ReaderType::New();

    // Kinda validate input to support both command line input and make debugger use simpler.
    // TODO: Improve checks on input and remove debugger part later on.
    if (argc >= 4) {
        nameGenerator->SetSeriesFormat(argv[1]);
        nameGenerator->SetStartIndex(std::stoi(argv[2]));
        nameGenerator->SetEndIndex(std::stoi(argv[3]));
    }
    else {
        // TODO: Remove this block once main development is done.
        nameGenerator->SetSeriesFormat("F:\\prog\\ITK\\SkeletonViz\\build\\Debug\\data\\img (%d).tif");
        nameGenerator->SetStartIndex(1);
        nameGenerator->SetEndIndex(113);
    }

    // Generate file paths for the reader
    nameGenerator->SetIncrementIndex(1);
    std::vector<std::string> filePaths = nameGenerator->GetFileNames();

    // Load slice image files into memory with series reader.
    reader->SetFileNames(filePaths);
    reader->Update();

    //
    std::cout << "Set up renderer..." << std::endl;
    // Set up VTK renderer and camera
    vtkRenderer *ren = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(istyle);

    std::cout << "Creating pipelines..." << std::endl;
    // Create pipelines
    BoneOnlyPipeline boneOnly = BoneOnlyPipeline(reader);
    boneOnly.addToRenderer(ren, iren);

    std::cout << "Creating callbacks..." << std::endl;
    // Add a visualization mode callback to change rendering modes (i.e. change between pipelines) on the fly
    VisualizationModeCallback *modeCallback = VisualizationModeCallback::New();
    modeCallback->ren = ren;
    modeCallback->renWin = renWin;
    modeCallback->iren = iren;
    modeCallback->boneOnly = &boneOnly;
    iren->AddObserver(vtkCommand::KeyPressEvent, modeCallback);
	
    std::cout << "rendering..." << std::endl;
	// Render the scene
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

    std::cout << "cleaning up.." << std::endl;

	// Clean Up
	iren->Delete();
	ren->Delete();
	renWin->Delete();
    modeCallback->Delete();

	return 0;
}
