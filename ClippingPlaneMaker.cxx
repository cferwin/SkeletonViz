#include "ClippingPlaneMaker.h"
#include <vtkPlaneWidget.h>

std::vector<vtkPlaneWidget*> ClippingPlaneMaker::planeWidgets;

vtkPlaneWidget* ClippingPlaneMaker::AddClippingPlane(vtkRenderWindowInteractor *iren, vtkProp3D* prop, MapperType *mapper) {
	// Set up a clipping plane widget
	vtkPlaneWidget *plane = vtkPlaneWidget::New();
	plane->SetInteractor(iren);
	plane->SetPlaceFactor(1.25);
	plane->SetProp3D(prop);
	plane->PlaceWidget();
	plane->On();

	// Set a callback for interaction
	ClippingPlaneInteractionCallback *callback = ClippingPlaneInteractionCallback::New();
	callback->widget = plane;
	callback->mapper = mapper;
	plane->AddObserver(vtkCommand::InteractionEvent, callback);

	planeWidgets.push_back(plane);
	return plane;
};

bool ClippingPlaneMaker::RemoveClippingPlane(vtkPlaneWidget *plane) {
	std::vector<vtkPlaneWidget*>::iterator it;
	for (it = planeWidgets.begin(); it != planeWidgets.end(); it++) {
		// Plane exists; delete it
		(*it)->Delete();
		return true;
	}

	// Could not find the plane, the user should know
	return false;
}

ClippingPlaneMaker::ClippingPlaneMaker() {
	//
}
