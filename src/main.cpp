#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << std::endl;
		std::cerr << argv[0] << " inputImageFile" << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::Image<signed short, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter<ImageType> ImageToVTKImageType;
	typedef itk::VTKImageToImageFilter<ImageType> VTKImageToImageType;
	typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> FilterType;

	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName(argv[1]);
	reader->Update();

	VTKImageToImageType::Pointer connectorInput = VTKImageToImageType::New();
	connectorInput->SetInput(reader->GetOutput());
	connectorInput->Update();

	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(connectorInput->GetOutput());
	filter->SetOutsideValue(150);
	filter->SetInsideValue(180);
	filter->SetLowerThreshold(0);
	filter->SetUpperThreshold(255);
	filter->Update();

	ImageToVTKImageType::Pointer connectorOutput = ImageToVTKImageType::New();
	connectorOutput->SetInput(filter->GetOutput());
	connectorOutput->Update();

	vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
	imageViewer->SetInputData(connectorOutput->GetOutput());
	
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	imageViewer->SetupInteractor(renderWindowInteractor);
	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}