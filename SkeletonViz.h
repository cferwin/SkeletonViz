#pragma once
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkNumericSeriesFileNames.h>

// Define data types
typedef vtkFixedPointVolumeRayCastMapper MapperType;
//typedef itk::Image<unsigned short, 2> ImageType;
typedef itk::Image<double, 3> ImageType;						// vtkFixedPointVolumeRayCastMapper can't support short data
typedef itk::ImageSeriesReader<ImageType> ReaderType;
typedef itk::ImageToVTKImageFilter <ImageType> ConnectorType;	// To convert image data from ITK to VTK format.
typedef itk::NumericSeriesFileNames NameGeneratorType;