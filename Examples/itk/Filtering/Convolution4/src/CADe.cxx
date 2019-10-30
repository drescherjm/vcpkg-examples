#include "CADe.h"
#include <itkImage.h>
#include <itkImageDuplicator.h>
#include <utility>
#include <itkStatisticsImageFilter.h>
#include <itkImageFileReader.h>
#include <itkShiftScaleImageFilter.h>
#include <filesystem>
#include <itkConvolutionImageFilter.h>
#include <itkPasteImageFilter.h>
#include <itkCastImageFilter.h>

/////////////////////////////////////////////////////////////////////////////////////////

const uint16_t IMAGE_SIZE_X{ 1100 };
const uint16_t IMAGE_SIZE_Y{ 1100 };

const uint16_t TEMPLATE_SIZE_MIN_X{ 3 };
const uint16_t TEMPLATE_SIZE_MIN_Y{ 3 };

const uint16_t IGNORE_SIZE_TOP{ 160 };
const uint16_t IGNORE_SIZE_LEFT{ 160 };

namespace fs = std::filesystem;
using OutputImageType = itk::Image<float, 2>;
using ConvolutionImageType = itk::Image<double, 2>;
using InputImageType = OutputImageType;

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

class CADe::cePrivate
{
public:
	using TemplateInfoType = std::pair<std::string, uint16_t>;
	using InputPixelType = OutputImageType::InternalPixelType;

public:
	bool hasImages() const;
	bool hasTemplates() const;
	bool processImage(std::string strImageFilePath);
	bool verifyImageSize(InputImageType::Pointer& image, InputImageType::RegionType & region, 
		std::string & strImagePath);

	bool verifyKernelImageSize(InputImageType::Pointer& image, std::string& strKernelImagePath);
	
	std::pair<InputPixelType, InputPixelType>	getMinMax(InputImageType::Pointer pImage);

	template <typename ImageType>
	typename ImageType::Pointer					LoadImage(std::string strKernel);

	InputImageType::RegionType		calculateDesiredOutputRegion(const InputImageType::RegionType& region);

	bool	processKernel(const TemplateInfoType & info,
		InputImageType::Pointer pImage,
		InputImageType::RegionType& inputRegion,
		InputImageType::RegionType& outputRegion
	);

	OutputImageType::Pointer	performConvolution(const TemplateInfoType& info,
		InputImageType::Pointer pImage,
		InputImageType::Pointer pKernelImage,
		InputImageType::RegionType& inputRegion,
		InputImageType::RegionType& outputRegion);

public:
	std::vector<TemplateInfoType>	m_infoTemplates;
	std::vector<std::string>		m_imageFiles;
	std::string						m_StrErrorMessages;
};

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

		sstream << "ERROR: The image ( " << strImagePath << " ) is not readable or smaller than " << IMAGE_SIZE_X << " by " << IMAGE_SIZE_Y << ". \n";

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

		sstream << "ERROR: The template image ( " << strKernelImagePath << " ) is not readable or smaller than " << IMAGE_SIZE_X << " by " << IMAGE_SIZE_Y << ". \n";

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
				retVal = processKernel(infoKernel, image, inputRegion, outputRegion);
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
	InputImageType::Pointer pImage, 
	InputImageType::RegionType& inputRegion, 
	InputImageType::RegionType& outputRegion)
{
	std::string strKernel = info.first;

	bool retVal = fs::is_regular_file(strKernel);

	if (retVal) {
		InputImageType::Pointer pKernelImage = LoadImage<InputImageType>(strKernel);



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

CADe::CADe() : m_pPrivate{ std::make_unique<cePrivate>() }
{

}

/////////////////////////////////////////////////////////////////////////////////////////

CADe::~CADe()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void CADe::addTemplate(std::string strPath, uint16_t nTemplateSize)
{
	m_pPrivate->m_infoTemplates.emplace_back(strPath,nTemplateSize);
}

/////////////////////////////////////////////////////////////////////////////////////////

void CADe::addImageFile(std::string strPath)
{
	m_pPrivate->m_imageFiles.emplace_back(strPath);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool CADe::excute()
{
	bool retVal = m_pPrivate->hasImages() && m_pPrivate->hasTemplates();

	if (retVal) {
		for (const auto& strImage : m_pPrivate->m_imageFiles) {
			retVal = m_pPrivate->processImage(strImage);
			if (!retVal) {
				break;
			}
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string CADe::getErrorMessages() const
{
	return m_pPrivate->m_StrErrorMessages;
}

/////////////////////////////////////////////////////////////////////////////////////////
