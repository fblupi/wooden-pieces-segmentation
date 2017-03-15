#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

using namespace std;

int main(int argc, char *argv[])
{
	float variance = 1;
	float upperThreshold = 10;
	float lowerThreshold = 0;

	if (argc < 2) {
		cerr << "Usage: " << endl;
		cerr << argv[0] << " inputImageFile" << endl;
		return EXIT_FAILURE;
	}

	if (argc > 2) {
		variance = atof(argv[2]);
	}

	if (argc > 3) {
		upperThreshold = atof(argv[3]);
	}

	if (argc > 4) {
		lowerThreshold = atof(argv[4]);
	}

	const unsigned int Dimension = 2;
	typedef signed short CharPixelType;
	typedef float RealPixelType;

	typedef itk::Image<CharPixelType, Dimension> CharImageType;
	typedef itk::Image<RealPixelType, Dimension> RealImageType;

	typedef itk::CastImageFilter<CharImageType, RealImageType> CastToRealFilterType;
	typedef itk::CannyEdgeDetectionImageFilter<RealImageType, RealImageType> CannyFilterType;
	typedef itk::RescaleIntensityImageFilter<RealImageType, CharImageType> RescaleFilterType;

	typedef itk::ImageFileReader<CharImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter<CharImageType> ImageToVTKImageType;
	typedef itk::VTKImageToImageFilter<CharImageType> VTKImageToImageType;

	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName(argv[1]);
	reader->Update();

	VTKImageToImageType::Pointer connectorInput = VTKImageToImageType::New();
	connectorInput->SetInput(reader->GetOutput());
	connectorInput->Update();

	CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();
	CannyFilterType::Pointer filter = CannyFilterType::New();
	RescaleFilterType::Pointer rescale = RescaleFilterType::New();
	
	toReal->SetInput(connectorInput->GetOutput());
	filter->SetInput(toReal->GetOutput());
	filter->SetVariance(variance);
	filter->SetUpperThreshold(upperThreshold);
	filter->SetLowerThreshold(lowerThreshold);
	filter->Update();
	rescale->SetInput(filter->GetOutput());

	ImageToVTKImageType::Pointer connectorOutput = ImageToVTKImageType::New();
	connectorOutput->SetInput(rescale->GetOutput());
	connectorOutput->Update();

	vtkSmartPointer<vtkImageViewer2> imageViewerInput = vtkSmartPointer<vtkImageViewer2>::New();
	imageViewerInput->SetInputData(reader->GetOutput());

	vtkSmartPointer<vtkImageViewer2> imageViewerOutput = vtkSmartPointer<vtkImageViewer2>::New();
	imageViewerOutput->SetInputData(connectorOutput->GetOutput());

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractorInput = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	imageViewerInput->SetupInteractor(renderWindowInteractorInput);
	imageViewerInput->Render();
	imageViewerInput->GetRenderer()->ResetCamera();
	imageViewerInput->Render();

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractorOutput = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	imageViewerOutput->SetupInteractor(renderWindowInteractorOutput);
	imageViewerOutput->Render();
	imageViewerOutput->GetRenderer()->ResetCamera();
	imageViewerOutput->Render();

	renderWindowInteractorOutput->Start();
	renderWindowInteractorInput->Start();

	return EXIT_SUCCESS;
}