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
#include <vtkCamera.h>

int main(int argc, char **argv) {
	typedef itk::Image<unsigned short, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter < ImageType> ConnectorType;
	ReaderType::Pointer reader = ReaderType::New();
	ConnectorType::Pointer connector = ConnectorType::New();

	if (argc < 2) {
		cout << "Include an image path as argument 1." << endl;
		return 1;
	}
	reader->SetFileName(argv[1]);
	
	connector->SetInput(reader->GetOutput());
	
	vtkImageViewer *viewer = vtkImageViewer::New();
	vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();

	connector->Update();
	viewer->SetupInteractor(renderWindowInteractor);
	viewer->SetInputData(connector->GetOutput());
	viewer->SetSize(512, 512);
	viewer->Render();
	viewer->SetColorWindow(255);
	viewer->SetColorLevel(128);

	renderWindowInteractor->Start();

	return 0;
}
