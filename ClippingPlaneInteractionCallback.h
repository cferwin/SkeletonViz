#pragma once
#include "SkeletonViz.h" // MapperType
#include <vtkCommand.h>
#include <vtkPlaneWidget.h>
#include <vtkPlane.h>
#include <vtkObject.h>

// Define a callback for modifying a clipping plane
class ClippingPlaneInteractionCallback : public vtkCommand {
public:
	static ClippingPlaneInteractionCallback *New();

	void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE;

	vtkPlaneWidget *widget;
	vtkPlane *plane;
	MapperType *mapper;
};
