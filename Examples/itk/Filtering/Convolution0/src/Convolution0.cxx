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
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkConvolutionImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkTIFFImageIO.h"
#include <itkRescaleIntensityImageFilter.h>

#ifdef ENABLE_QUICKVIEW
#include "QuickView.h"
#endif

//using InputImageType = itk::Image<unsigned short, 2>;
using OutputImageType = itk::Image<float, 2>;

using InputImageType = OutputImageType;

static void
CreateKernel(InputImageType::Pointer kernel, unsigned int width);

int main(int argc, char* argv[])
{
	// Verify command line arguments
	if (argc < 2)
	{
		std::cerr << "Usage: ";
		std::cerr << argv[0] << "inputImageFile [width]" << std::endl;
		return EXIT_FAILURE;
	}

	// Parse command line arguments
	unsigned int width = 3;
	if (argc > 2)
	{
		width = std::stoi(argv[2]);
	}

	InputImageType::Pointer kernel = InputImageType::New();
	CreateKernel(kernel, width);

	using ReaderType = itk::ImageFileReader<InputImageType>;
	using FilterType = itk::ConvolutionImageFilter<InputImageType, InputImageType, OutputImageType>;

	// Create and setup a reader
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(argv[1]);

	using RescaleType = itk::RescaleIntensityImageFilter<InputImageType>;

	RescaleType::Pointer rescaler = RescaleType::New();

	rescaler->SetOutputMinimum(0);

	rescaler->SetInput(reader->GetOutput());
	rescaler->Update();
	// Convolve image with kernel.
	FilterType::Pointer convolutionFilter = FilterType::New();
	convolutionFilter->NormalizeOn();
	convolutionFilter->SetInput(reader->GetOutput());
#if ITK_VERSION_MAJOR >= 4
	convolutionFilter->SetKernelImage(kernel);
#else
	convolutionFilter->SetImageKernelInput(kernel);
#endif
#ifdef ENABLE_QUICKVIEW
	QuickView viewer;
	viewer.AddImage<ImageType>(reader->GetOutput(), true, itksys::SystemTools::GetFilenameName(argv[1]));

	std::stringstream desc;
	desc << "ConvolutionFilter\n"
		<< "Kernel Witdh = " << width;
	viewer.AddImage<ImageType>(convolutionFilter->GetOutput(), true, desc.str());
	viewer.Visualize();
#endif
	//convolutionFilter->Update();

	using StatsType = itk::StatisticsImageFilter<OutputImageType>;

	StatsType::Pointer stats = StatsType::New();

	stats->SetInput(convolutionFilter->GetOutput());

	stats->Update();

	stats->Print(std::cout);

	using WriterType = itk::ImageFileWriter<OutputImageType>;
	using TIFFIOType = itk::TIFFImageIO;

	TIFFIOType::Pointer tiffIO = TIFFIOType::New();
	//   tiffIO->SetPixelType(itk::ImageIOBase::SCALAR);
	//   tiffIO->SetNumberOfComponents(1);
	//   tiffIO->SetCompressionToLZW();

	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("output.tiff");
	writer->SetInput(convolutionFilter->GetOutput());
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

void
CreateKernel(InputImageType::Pointer kernel, unsigned int width)
{
	InputImageType::IndexType start;
	start.Fill(0);

	InputImageType::SizeType size;
	size.Fill(width);

	InputImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);

	kernel->SetRegions(region);
	kernel->Allocate();

	itk::ImageRegionIterator<InputImageType> imageIterator(kernel, region);

	while (!imageIterator.IsAtEnd())
	{
		// imageIterator.Set(255);
		imageIterator.Set(1);

		++imageIterator;
	}
}
