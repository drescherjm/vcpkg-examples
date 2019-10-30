#pragma once

#pragma once

#ifndef CONVOLUTION3_H
#define CONVOLUTION3_H

#include <string>
#include <itkImage.h>

/////////////////////////////////////////////////////////////////////////////////////////

//using InputImageType = itk::Image<unsigned short, 2>;
using OutputImageType = itk::Image<float, 2>;
using ConvolutionImageType = itk::Image<double, 2>;
using InputImageType = OutputImageType;

/////////////////////////////////////////////////////////////////////////////////////////

int						writeOutputFile(std::string strImageFile, std::string strKernel, OutputImageType::Pointer output);
InputImageType::Pointer LoadKernel(std::string strKernel);

std::pair<float, float> getMinMax(InputImageType::Pointer pImage);

std::string				getFileNameNoPath(const std::string& strFilePath);
std::string				getOutputFileName(const std::string& strImagePath, const std::string& strKernelPath);
void					drawFilledCircleInImage(OutputImageType* pImage, int xPos, int yPos, int nRadius, double fColor);
/////////////////////////////////////////////////////////////////////////////////////////

template <typename ImageType>
typename ImageType::Pointer LoadImage(std::string strKernel)
{
	using ReaderType = itk::ImageFileReader<ImageType>;

	ReaderType::Pointer kernelReader = ReaderType::New();
	kernelReader->SetFileName(strKernel);
	kernelReader->Update();

	auto minMax = getMinMax(kernelReader->GetOutput());

	using RescaleType = itk::ShiftScaleImageFilter<ImageType, ImageType>;

	RescaleType::Pointer rescaler = RescaleType::New();

	rescaler->SetShift(-minMax.first);

	rescaler->SetInput(kernelReader->GetOutput());
	rescaler->Update();

	//auto newMinMax = getMinMax(rescaler->GetOutput());

	using DuplicatorType = itk::ImageDuplicator<ImageType>;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(rescaler->GetOutput());
	duplicator->Update();

	return duplicator->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ConvolutionType, typename OutputType, typename std::enable_if<std::is_same<ConvolutionType, OutputType>::value>::type * = nullptr>
typename OutputType::Pointer transformFinalOutputForFileWriting(ConvolutionImageType* pImageData)
{
	using DuplicatorType = itk::ImageDuplicator<OutputImageType>;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(pImageData);
	duplicator->Update();

	return duplicator->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename ConvolutionType, typename OutputType, typename std::enable_if<!std::is_same<ConvolutionType, OutputType>::value>::type * = nullptr>
typename OutputType::Pointer transformFinalOutputForFileWriting(ConvolutionImageType* pImageData)
{
	using CastType = itk::CastImageFilter<ConvolutionImageType, OutputImageType>;

	CastType::Pointer castFilt = CastType::New();
	castFilt->SetInput(pImageData);
	castFilt->Update();

	using DuplicatorType = itk::ImageDuplicator<OutputImageType>;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(castFilt->GetOutput());
	duplicator->Update();

	return duplicator->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CONVOLUTION3_H