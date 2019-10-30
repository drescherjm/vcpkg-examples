#include "CADe_Private.h"

#include <itkImageDuplicator.h>
#include <utility>
#include <itkStatisticsImageFilter.h>
#include <itkImageFileReader.h>
#include <itkShiftScaleImageFilter.h>

#include <itkConvolutionImageFilter.h>
#include <itkPasteImageFilter.h>
#include <itkCastImageFilter.h>
#include "itkImageFileWriter.h"
#include "itkTIFFImageIO.h"

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

CADe::cePrivate::cePrivate() : m_nMaximumScores{ MAXIMUM_SCORES }
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

			for (const auto& infoKernel : m_infoTemplates) {
				retVal = processKernel(infoKernel, strImageFilePath, image, inputRegion, outputRegion);
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

			retVal = writeOutputFile(info, strImageFilePath, pOutputImage);
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

	convolutionFilter->GetOutput()->SetRequestedRegion(inputRegion);

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

bool CADe::cePrivate::writeOutputFile(const TemplateInfoType& info, std::string strInputImage, OutputImageType::Pointer output)
{
	bool retVal{ true };

	using WriterType = itk::ImageFileWriter<OutputImageType>;
	using TIFFIOType = itk::TIFFImageIO;

	TIFFIOType::Pointer tiffIO = TIFFIOType::New();

	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(getOutputFileName(strInputImage, info.first));
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

std::string CADe::cePrivate::getOutputFileName(const std::string& strImagePath, const std::string& strKernelPath)
{
	std::string strImageName = getFileNameNoPath(strImagePath);
	std::string strKernelName = getFileNameNoPath(strKernelPath);

	std::string retVal = strImageName + "_" + strKernelName + ".tiff";

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
