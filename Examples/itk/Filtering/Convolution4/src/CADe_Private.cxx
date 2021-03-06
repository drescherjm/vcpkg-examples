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

#include "CADe_Private.h"

#include <itkImageDuplicator.h>
#include <utility>
#include <itkStatisticsImageFilter.h>
#include <itkImageFileReader.h>
#include <itkShiftScaleImageFilter.h>

#include <itkConvolutionImageFilter.h>
#include <itkPasteImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkTIFFImageIO.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <fstream>

#ifdef ENABLE_QUICKVIEW
#include <QuickView.h>
#endif

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

CADe::cePrivate::cePrivate() : m_nMaximumScoresPerTemplate{ MAXIMUM_SCORES_PER_TEMPLATE }, 
m_nMaximumScoresPerImage{ MAXIMUM_SCORES_PER_IMAGE },
m_bDebugMode{false}, m_bWriteAfterScoreDebugImage{true}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

template <typename ImageType>
typename ImageType::Pointer CADe::cePrivate::LoadImage(std::string strKernel)
{
	// This loads an image and rescales the pixel values by making the 
	// minimum value of the loaded image 0.

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

bool CADe::cePrivate::hasImages() const
{
	return !m_imageFiles.empty();
}

bool CADe::cePrivate::hasTemplates() const
{
	return !m_infoTemplates.empty();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::verifyImageSize(InputImageType::Pointer& image,
	InputImageType::RegionType& region,
	std::string& strImagePath)
{
	InputImageType::SizeType size = region.GetSize();

	if ((size[0] < IMAGE_SIZE_X) || (size[1] < IMAGE_SIZE_Y)) {

		std::ostringstream sstream;

		sstream << "ERROR: The image ( " << strImagePath << " ) is not readable or smaller than "
			<< IMAGE_SIZE_X << " by " << IMAGE_SIZE_Y << ". \n";

		m_StrErrorMessages += sstream.str();

		return false;
	}
	else
		return true;

}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::verifyKernelImageSize(InputImageType::Pointer& image,
	std::string& strKernelImagePath)
{
	auto region = image->GetLargestPossibleRegion();

	InputImageType::SizeType size = region.GetSize();

	if ((size[0] < TEMPLATE_SIZE_MIN_X) || (size[1] < TEMPLATE_SIZE_MIN_X)) {

		std::ostringstream sstream;

		sstream << "ERROR: The template image ( " << strKernelImagePath << " ) is not readable or smaller than "
			<< TEMPLATE_SIZE_MIN_X << " by " << TEMPLATE_SIZE_MIN_X << ". \n";

		m_StrErrorMessages += sstream.str();

		return false;
	}
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::processImage(std::string strImageFilePath)
{
	bool retVal = fs::is_regular_file(strImageFilePath);

	if (retVal) {
		InputImageType::Pointer image = LoadImage<InputImageType>(strImageFilePath);

		InputImageType::RegionType region = image->GetLargestPossibleRegion();
		InputImageType::RegionType inputRegion = region;

		retVal = verifyImageSize(image, region, strImageFilePath);

		if (retVal) {
			InputImageType::RegionType outputRegion = calculateDesiredOutputRegion(inputRegion);

			ScoreVector vecScores;

			for (const auto& infoKernel : m_infoTemplates) {
				retVal = processKernel(infoKernel, strImageFilePath, image, inputRegion, outputRegion,vecScores);
				if (!retVal) {
					break;
				}
			}

			if (retVal) {
				retVal = calculateFinalImageScores(vecScores);
				if (retVal) {
					retVal = writeScoreOutputCSVFile(strImageFilePath, vecScores);
				}
			}

		}
	}
	else
	{
		std::ostringstream sstream;
		sstream << "ERROR: Can not open the image file " << strImageFilePath << "\n";
		m_StrErrorMessages += sstream.str();
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::pair<CADe::cePrivate::InputPixelType, CADe::cePrivate::InputPixelType>
CADe::cePrivate::getMinMax(InputImageType::Pointer pImage)
{
	using StatsType = itk::StatisticsImageFilter<OutputImageType>;
	StatsType::Pointer stats = StatsType::New();
	stats->SetInput(pImage);
	stats->Update();
	stats->Print(std::cout);

	return { stats->GetMinimum(),stats->GetMaximum() };
}

/////////////////////////////////////////////////////////////////////////////////////////

InputImageType::RegionType
CADe::cePrivate::calculateDesiredOutputRegion(const InputImageType::RegionType& region)
{
	// The algorithm ignores a 160 pixel region on all sides of the image. This 
	// function calculates the region to output.

	InputImageType::RegionType retVal = region;

	auto lowerIndex = retVal.GetIndex();

	lowerIndex[0] += IGNORE_SIZE_LEFT;
	lowerIndex[1] += IGNORE_SIZE_TOP;

	auto upperIndex = retVal.GetUpperIndex();
	upperIndex[0] -= IGNORE_SIZE_LEFT;
	upperIndex[1] -= IGNORE_SIZE_TOP;

	retVal.SetIndex(lowerIndex);
	retVal.SetUpperIndex(upperIndex);

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::processKernel(const TemplateInfoType& info,
	const std::string& strImageFilePath,
	InputImageType::Pointer pImage,
	InputImageType::RegionType& inputRegion,
	InputImageType::RegionType& outputRegion,
	ScoreVector & vecScores)
{
	std::string strKernel = info.first;

	bool retVal = fs::is_regular_file(strKernel);

	if (retVal) {
		InputImageType::Pointer pKernelImage = LoadImage<InputImageType>(strKernel);
		retVal = verifyKernelImageSize(pKernelImage, strKernel);
		if (retVal) {
			auto pOutputImage = performConvolution(info, pImage, pKernelImage, inputRegion, outputRegion);

			retVal = writeScoreOutputImageFile(info, strImageFilePath, pOutputImage);
			if (retVal) {

				if (m_bDebugMode) {

#ifdef ENABLE_QUICKVIEW
					QuickView viewer;
					viewer.AddImage<InputImageType>(pImage, true, itksys::SystemTools::GetFilenameName(strImageFilePath));
					viewer.AddImage<InputImageType>(pOutputImage, true, std::string("Output for ") + itksys::SystemTools::GetFilenameName(strKernel));
					viewer.SetViewPortSize(955);
					viewer.Visualize();
#endif

				}

				retVal = calculateScores(info, pOutputImage, vecScores);


				if (retVal && m_bWriteAfterScoreDebugImage) {

					TemplateInfoType tyTemp = info;

					// Hack to append _AfterScore to the base name of the output image file.
					tyTemp.first = getFileNameNoPath(info.first);
					tyTemp.first += "_AfterScore";

					writeScoreOutputImageFile(tyTemp, strImageFilePath, pOutputImage);
				}
			}
		}
	}
	else {
		std::ostringstream sstream;
		sstream << "ERROR: Can not open the kernel file " << strKernel << "\n";
		m_StrErrorMessages += sstream.str();
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

OutputImageType::Pointer CADe::cePrivate::performConvolution(const TemplateInfoType& info,
	InputImageType::Pointer pImage,
	InputImageType::Pointer pKernelImage,
	InputImageType::RegionType& inputRegion,
	InputImageType::RegionType& outputRegion)
{
	using FilterType = itk::ConvolutionImageFilter<InputImageType, InputImageType, ConvolutionImageType>;

	auto lowerIndex = outputRegion.GetIndex();

	FilterType::Pointer convolutionFilter = FilterType::New();
	convolutionFilter->NormalizeOn();
	convolutionFilter->SetInput(pImage);
	convolutionFilter->SetKernelImage(pKernelImage);

	convolutionFilter->GetOutput()->SetRequestedRegion(outputRegion);

	using PasteType = itk::PasteImageFilter<ConvolutionImageType, ConvolutionImageType>;

	// The following creates the output convolution with the 160 pixel border.
	ConvolutionImageType::Pointer blankCanvas = ConvolutionImageType::New();
	blankCanvas->SetRegions(inputRegion);
	blankCanvas->Allocate();
	blankCanvas->FillBuffer(0.0);
	blankCanvas->SetSpacing(pImage->GetSpacing());

	PasteType::Pointer pasteImageFilter = PasteType::New();

	pasteImageFilter->SetSourceImage(convolutionFilter->GetOutput());
	pasteImageFilter->SetSourceRegion(convolutionFilter->GetOutput()->GetRequestedRegion());
	pasteImageFilter->SetDestinationImage(blankCanvas);
	pasteImageFilter->SetDestinationIndex(lowerIndex);

	pasteImageFilter->Update();

	// If we used double as the pixel type for the ConvolutionImageType we need to cast that to float. The 
	// transformFinalOutputForFileWriting() will perform this conversion.
	return transformFinalOutputForFileWriting<ConvolutionImageType, OutputImageType>(pasteImageFilter->GetOutput());
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::writeScoreOutputImageFile(const TemplateInfoType& info, std::string strInputImage, 
	OutputImageType::Pointer output)
{
	bool retVal{ true };

	using WriterType = itk::ImageFileWriter<OutputImageType>;
	using TIFFIOType = itk::TIFFImageIO;

	TIFFIOType::Pointer tiffIO = TIFFIOType::New();

	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(getOutputFileName(strInputImage, info.first,".tiff"));
	writer->SetInput(output);
	writer->SetImageIO(tiffIO);

	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::ostringstream sstream;

		sstream << "Error: Could not write output file. " << error << "\n";

		m_StrErrorMessages += sstream.str();

		retVal = false;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string CADe::cePrivate::getFileNameNoPath(const std::string& strFilePath)
{
	fs::path path(strFilePath);

	return path.stem().string();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string CADe::cePrivate::getOutputFileName(const std::string& strImagePath, const std::string& strKernelPath,
	const std::string strExtension)
{
	std::string strImageName = getFileNameNoPath(strImagePath);
	std::string strKernelName = getFileNameNoPath(strKernelPath);

	std::string retVal = strImageName + "_" + strKernelName + strExtension;

	if (fs::exists(retVal)) {
		for (uint16_t i = 0; i < 999; ++i) {
			std::ostringstream fn;
			fn << strImageName << '_' << strKernelName << '_' << std::setfill('0') << std::setw(3) << i << strExtension;

			if (!fs::exists(fn.str())) {
				retVal = fn.str();
				break;
			}
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::pair<bool, CADe::cePrivate::TemplateType> CADe::cePrivate::getTemplateType(const TemplateInfoType& info)
{
	std::pair<bool, CADe::cePrivate::TemplateType> retVal{ false,TT_UNKNOWN };

	if (info.second == 50) {
		retVal.first = true;
		retVal.second = TT_SMALL;
	}

	if (info.second == 100) {
		retVal.first = true;
		retVal.second = TT_BIG;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void CADe::cePrivate::setPixel(OutputImageType* pImage, const OutputImageType::RegionType& region, int nX, int nY, double fColor) const
{
	OutputImageType::IndexType nPos;
	nPos[0] = nX;
	nPos[1] = nY;
	if (region.IsInside(nPos)) {
		pImage->SetPixel(nPos, fColor);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void CADe::cePrivate::drawFilledCircleInImage(OutputImageType* pImage, int xPos, int yPos, int nRadius, 
	double fColor, double fCenterColor) const
{

	OutputImageType::RegionType outputRegion = pImage->GetLargestPossibleRegion();

	int x0 = xPos;
	int y0 = yPos;

	int x = nRadius;
	int y = 0;
	int xChange = 1 - (nRadius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y)
	{
		for (int i = x0 - x; i <= x0 + x; i++)
		{
			setPixel(pImage, outputRegion, i, y0 + y, fColor);
			setPixel(pImage, outputRegion, i, y0 - y, fColor);
		}
		for (int i = x0 - y; i <= x0 + y; i++)
		{
			setPixel(pImage, outputRegion, i, y0 + x, fColor);
			setPixel(pImage, outputRegion, i, y0 - x, fColor);
		}

		y++;
		radiusError += yChange;
		yChange += 2;
		if (((radiusError << 1) + xChange) > 0)
		{
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}

	if (fColor != fCenterColor) {
		setPixel(pImage, outputRegion, x0, y0, fCenterColor);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::calculateScores(const TemplateInfoType& info, OutputImageType::Pointer outputImage, ScoreVector& vecScores)
{
	using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<OutputImageType>;
	ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
	imageCalculatorFilter->SetImage(outputImage);

	auto res = getTemplateType(info);

	bool retVal = res.first;

	if (retVal) {

		int nRadius = 0;

		switch (res.second)
		{
		case TT_SMALL:
			nRadius = 100;
		break;
		case TT_BIG:
			nRadius = 200;
		break;
		default:
			return false;
			break;
		}
		
		for (int i = 0; i < m_nMaximumScoresPerTemplate; ++i) {
			imageCalculatorFilter->Compute();

			OutputImageType::IndexType maximumLocation = imageCalculatorFilter->GetIndexOfMaximum();
			auto nMaximum = imageCalculatorFilter->GetMaximum();

			if (nMaximum <= 2) {
				break;
			}

			vecScores.emplace_back(nMaximum, maximumLocation[0], maximumLocation[1], info.second);

			drawFilledCircleInImage(outputImage, maximumLocation[0], maximumLocation[1], nRadius, 0.0, 1.0);
		}
	}
	else {
		std::ostringstream sstream;
		sstream << "ERROR: The template image ( " << info.first << " ) has an unsupported radius size: " << info.second << "\n";

		m_StrErrorMessages += sstream.str();
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::calculateFinalImageScores(ScoreVector& vecScores)
{
	bool retVal = !vecScores.empty();

	if (retVal) {

		ScoreVector vecToProcss;
		vecToProcss.reserve(m_nMaximumScoresPerImage);

		vecToProcss.swap(vecScores);

		std::sort(vecToProcss.begin(), vecToProcss.end(), [](const CADeScore& s1, const CADeScore& s2) { return s1.m_fScore > s2.m_fScore; });

		for (const auto& item : vecToProcss) {
			if (vecScores.empty()) {
				// Put the highest scored item in the vector.
				vecScores.emplace_back(item);
			}
			else {
				using PointType = itk::Point<uint16_t, 2>;

				PointType p0;
				p0[0] = item.m_nX;
				p0[1] = item.m_nY;

				bool bAdd = true;

				for (const auto& higherScoredItem : vecScores) {
					PointType p1;
					p1[0] = higherScoredItem.m_nX;
					p1[1] = higherScoredItem.m_nY;

					bAdd = (p0.EuclideanDistanceTo(p1) >= MINIMUM_DISTANCE_BETWEEN_FINAL_SCORES);
					if (!bAdd) {
						break;
					}
				}

				if (bAdd) {
					vecScores.emplace_back(item);
				}

				if (vecScores.size() >= m_nMaximumScoresPerImage) {
					break;
				}

			}
		}

	}
	return !vecScores.empty();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::cePrivate::writeScoreOutputCSVFile(std::string strInputImage, const ScoreVector& vecScores)
{
	bool retVal = !vecScores.empty();
	
	if (retVal) {

		std::string strOutputFile = getOutputFileName(strInputImage, "CADe", ".csv");
		
		std::ofstream output(strOutputFile);

		output << "Score, x, y, Template Size \n";

		for (const auto& score : vecScores) {
			output << score.m_fScore << ", " << score.m_nX << ", " << score.m_nY << ", " << score.m_nTemplateSize << "\n";
		}

		output.flush();
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
