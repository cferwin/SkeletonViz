#pragma once
#include <vtkPlaneWidget.h>
#include "ClippingPlaneInteractionCallback.h"
#include <vtkPlaneWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProp3D.h>
#include "SkeletonViz.h"
#include <vector>

class ClippingPlaneMaker {
public:
	ClippingPlaneMaker();
	// Sets up everything needed for a clipping plane and adds it to the mapper automatically.
	// Returns a reference to the new widget.
	static vtkPlaneWidget *AddClippingPlane(vtkRenderWindowInteractor *iren, vtkProp3D* prop, MapperType *mapper);

	// Deletes the specified plane widget.
	// Returns true if successful, false otherwise.
	static bool RemoveClippingPlane(vtkPlaneWidget *plane);

private:
	static std::vector<vtkPlaneWidget*> planeWidgets;
};