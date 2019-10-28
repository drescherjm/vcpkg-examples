/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "Convolution3.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkConvolutionImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkTIFFImageIO.h"
#include <itkRescaleIntensityImageFilter.h>
#include <itkShiftScaleImageFilter.h>
#include <filesystem>
#include "itkImageDuplicator.h"
#include <itkCastImageFilter.h>

#ifdef ENABLE_QUICKVIEW
#include "QuickView.h"
#endif
#include "itkPasteImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include <itkEllipseSpatialObject.h>

namespace fs = std::filesystem;


/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{

	// Verify command line arguments
	if (argc < 2)
	{
		std::cerr << "Usage: ";
		std::cerr << argv[0] << "inputImageFile [kernel_file]" << std::endl;
		return EXIT_FAILURE;
	}

	std::string strKernel;
	if (argc > 2)
	{
		strKernel = argv[2];
	}
	else {
		//strKernel = R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr50\sig50_template.tiff)";

		//strKernel = R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr100\sig100_template.tiff)";

		//strKernel = R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr25\sig25_template.tiff)";

		strKernel = R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr12\sig12_template.tiff)";

		//strKernel = R"(J:\images\clean\Procesed Images\Breast\2019_11_CADE\Templates\sigr12\sig6_template.tiff)";
	}

	if (!fs::is_regular_file(strKernel)) {
		std::cerr << "ERROR: Can not open the kernel file " << strKernel << std::endl;
		return EXIT_FAILURE;
	}

	std::string strImageFile = argv[1];

	if (!fs::is_regular_file(strImageFile)) {
		std::cerr << "ERROR: Can not open the image file " << strImageFile << std::endl;
		return EXIT_FAILURE;
	}

	using ReaderType = itk::ImageFileReader<InputImageType>;
	using FilterType = itk::ConvolutionImageFilter<InputImageType, InputImageType, ConvolutionImageType>;
	
	InputImageType::Pointer image = LoadImage<InputImageType>(strImageFile);
	InputImageType::Pointer kernelImage = LoadKernel(strKernel);
	 
	InputImageType::RegionType region = image->GetLargestPossibleRegion();
	InputImageType::RegionType inputRegion = region;
	InputImageType::SizeType size = region.GetSize();
	
	if ( (size[0] < 1100) || (size[1] < 1110) ) {
		std::cerr << "ERROR: Image is smaller than 1110 x 1110";
		return EXIT_FAILURE;
	}
	
	auto lowerIndex = region.GetIndex();

	lowerIndex[0] += 160;
	lowerIndex[1] += 160;

	auto upperIndex = region.GetUpperIndex();
	upperIndex[0] -= 160;
	upperIndex[1] -= 160;

	region.SetIndex(lowerIndex);
	region.SetUpperIndex(upperIndex);

	//image->SetRequestedRegion(region);

	// Convolve image with kernel.
	FilterType::Pointer convolutionFilter = FilterType::New();
	convolutionFilter->NormalizeOn();
	convolutionFilter->SetInput(image);
	convolutionFilter->SetKernelImage(kernelImage);

	convolutionFilter->GetOutput()->SetRequestedRegion(region);

	using PasteType = itk::PasteImageFilter<ConvolutionImageType, ConvolutionImageType>;
	
	ConvolutionImageType::Pointer blankCanvas = ConvolutionImageType::New();

	blankCanvas->SetRegions(inputRegion);
	blankCanvas->Allocate();
	blankCanvas->FillBuffer(0);
	blankCanvas->SetSpacing(image->GetSpacing());

	PasteType::Pointer pasteImageFilter = PasteType::New();

	pasteImageFilter->SetSourceImage(convolutionFilter->GetOutput());
	pasteImageFilter->SetSourceRegion(convolutionFilter->GetOutput()->GetRequestedRegion());
	pasteImageFilter->SetDestinationImage(blankCanvas);
	pasteImageFilter->SetDestinationIndex(lowerIndex);

	pasteImageFilter->Update();
	//pasteImageFilter->Print(std::cout);
	
	image->SetRequestedRegion(inputRegion);
	image->Print(std::cout);

	OutputImageType::Pointer outputImage = transformFinalOutputForFileWriting<ConvolutionImageType, OutputImageType>(pasteImageFilter->GetOutput());

	outputImage->Print(std::cout);


	using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<OutputImageType>;

	ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
	imageCalculatorFilter->SetImage(outputImage);
	imageCalculatorFilter->Compute();


	OutputImageType::IndexType maximumLocation = imageCalculatorFilter->GetIndexOfMaximum();
	auto nMaximum = imageCalculatorFilter->GetMaximum();

	drawFilledCircleInImage(outputImage, maximumLocation[0], maximumLocation[1], 200);
	
#ifdef ENABLE_QUICKVIEW
	QuickView viewer;
	viewer.AddImage<InputImageType>(image, true, itksys::SystemTools::GetFilenameName(argv[1]));
	viewer.AddImage<InputImageType>(outputImage, true, "Output");
	//viewer.AddImage<InputImageType>(pasteImageFilter->GetOutput(), true, itksys::SystemTools::GetFilenameName(strKernel));
	viewer.SetViewPortSize(955);
	viewer.Visualize();
#endif




// 	using StatsType = itk::StatisticsImageFilter<OutputImageType>;
// 
// 	StatsType::Pointer stats = StatsType::New();
// 	stats->SetInput(outputImage);
// 	stats->Update();
// 
// 	stats->Print(std::cout);

	return writeOutputFile(strImageFile, strKernel, outputImage);
}

/////////////////////////////////////////////////////////////////////////////////////////

InputImageType::Pointer LoadKernel(std::string strKernel)
{
	using KernelReaderType = itk::ImageFileReader<InputImageType>;

	KernelReaderType::Pointer kernelReader = KernelReaderType::New();
	kernelReader->SetFileName(strKernel);
	kernelReader->Update();

	auto minMax = getMinMax(kernelReader->GetOutput());
	
	using RescaleType = itk::ShiftScaleImageFilter<InputImageType,InputImageType>;

	RescaleType::Pointer rescaler = RescaleType::New();

	rescaler->SetShift(-minMax.first);

	rescaler->SetInput(kernelReader->GetOutput());
	rescaler->Update();
	
	auto newMinMax = getMinMax(rescaler->GetOutput());

	using DuplicatorType = itk::ImageDuplicator<InputImageType>;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(rescaler->GetOutput());
	duplicator->Update();

	return duplicator->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::pair<float, float> getMinMax(InputImageType::Pointer pImage)
{
	using StatsType = itk::StatisticsImageFilter<OutputImageType>;
	StatsType::Pointer stats = StatsType::New();
	stats->SetInput(pImage);
	stats->Update();
	stats->Print(std::cout);

	return { stats->GetMinimum(),stats->GetMaximum() };
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string getFileNameNoPath(const std::string& strFilePath)
{
	fs::path path(strFilePath);

	return path.stem().string();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string getOutputFileName(const std::string& strImagePath, const std::string& strKernelPath)
{
	std::string strImageName = getFileNameNoPath(strImagePath);
	std::string strKernelName = getFileNameNoPath(strKernelPath);

	std::string retVal = strImageName + "_" + strKernelName  + ".tiff";

	if (fs::exists(retVal)) {
		for (uint16_t i = 0; i < 999; ++i) {
			std::ostringstream fn;
			fn << strImageName << '_' << strKernelName << '_' << std::setfill('0') << std::setw(3) << i << ".tiff";

			if (!fs::exists(fn.str())) {
				retVal = fn.str();
				break;
			}
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void drawFilledCircleInImage(OutputImageType* pImage, int xPos, int yPos, int nRadius)
{
// 	using EllipseType = itk::EllipseSpatialObject<2>;
// 
// 	EllipseType::Pointer elipse = EllipseType::New();
// 	elipse->SetRadius(nRadius);
// 	elipse->SetDefaultInsideValue(0.0);
// 
// 	using SpatialFilter = itk::SpatialObjectToImageFilter<EllipseType, OutputImageType>;
// 
// 	SpatialFilter::Pointer pOut = SpatialFilter::New();
// 
// 	pOut->SetSize(pImage->GetLargestPossibleRegion());
// 	pOut->SetInput(elipse->GetOutout)

	OutputImageType::IndexType centerPoint, localIndex;

	centerPoint[0] = xPos;
	centerPoint[1] = yPos;

	double fRadius = (pImage->GetSpacing()[0]) * nRadius;

	for (double angle = 0; angle <= itk::Math::twopi; angle += itk::Math::pi / 60.0)
	{
		using IndexValueType = OutputImageType::IndexType::IndexValueType;

		localIndex[0] = itk::Math::Round<IndexValueType>(centerPoint[0] + (fRadius * std::cos(angle)));

		localIndex[1] = itk::Math::Round<IndexValueType>(centerPoint[1] + (fRadius * std::sin(angle)));
		OutputImageType::RegionType outputRegion = pImage->GetLargestPossibleRegion();

		if (outputRegion.IsInside(localIndex))
		{
			pImage->SetPixel(localIndex, 0);
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////

int writeOutputFile(std::string strImageFile, std::string strKernel, OutputImageType::Pointer output)
{
	using WriterType = itk::ImageFileWriter<OutputImageType>;
	using TIFFIOType = itk::TIFFImageIO;

	TIFFIOType::Pointer tiffIO = TIFFIOType::New();


	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(getOutputFileName(strImageFile, strKernel));
	writer->SetInput(output);
	writer->SetImageIO(tiffIO);

	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject& error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////
