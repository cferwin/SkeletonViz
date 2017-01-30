#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkImageViewer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>

int main(int argc, char **argv) {
	// ITK definitions for handling the slice image
	typedef itk::Image<unsigned short, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter < ImageType> ConnectorType;
	ReaderType::Pointer reader = ReaderType::New();
	ConnectorType::Pointer connector = ConnectorType::New();

	// Import the ITK slice image
	if (argc < 2) {
		cout << "Include an image path as argument 1." << endl;
		return 1;
	}
	reader->SetFileName(argv[1]);
	
	// Convert to VTK image
	connector->SetInput(reader->GetOutput());
	connector->Update();

	// Plug the image into a texture
	vtkTexture *sliceTexture = vtkTexture::New();
	sliceTexture->SetInputData(connector->GetOutput());
	sliceTexture->InterpolateOn();
	sliceTexture->Update();

	// Set up a plane for the slice texture
	vtkPlaneSource *plane = vtkPlaneSource::New();
	vtkPolyDataMapper *planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());
	vtkActor *planeActor = vtkActor::New();
	planeActor->SetMapper(planeMapper);
	planeActor->SetTexture(sliceTexture);

	// Set up VTK renderer
	vtkRenderer *ren = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
	iren->SetRenderWindow(renWin);
	iren->SetInteractorStyle(istyle);

	// Add plane actor to the scene
	ren->AddActor(planeActor);
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);

	// Render the scene
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

	return 0;
}
