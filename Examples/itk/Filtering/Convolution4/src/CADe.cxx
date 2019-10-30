#include "CADe.h"
#include <itkImage.h>
#include <itkImageDuplicator.h>
#include <utility>
#include <itkStatisticsImageFilter.h>
#include <itkImageFileReader.h>
#include <itkShiftScaleImageFilter.h>

/////////////////////////////////////////////////////////////////////////////////////////

const uint16_t IMAGE_SIZE_X{ 1100 };
const uint16_t IMAGE_SIZE_Y{ 1100 };

const uint16_t IGNORE_SIZE_TOP{ 160 };
const uint16_t IGNORE_SIZE_LEFT{ 160 };

/////////////////////////////////////////////////////////////////////////////////////////

class CADe::cePrivate
{
public:
	using OutputImageType = itk::Image<float, 2>;
	using ConvolutionImageType = itk::Image<double, 2>;
	using InputImageType = OutputImageType;
	using TemplateInfoType = std::pair<std::string, uint16_t>;
	using InputPixelType = OutputImageType::InternalPixelType;

public:
	bool hasImages() const;
	bool hasTemplates() const;
	bool processImage(std::string strImageFilePath);
	bool verifyImageSize(InputImageType::Pointer& image, InputImageType::RegionType & region, 
		std::string & strImagePath);
	
	std::pair<InputPixelType, InputPixelType>	getMinMax(InputImageType::Pointer pImage);

	template <typename ImageType>
	typename ImageType::Pointer					LoadImage(std::string strKernel);

	InputImageType::RegionType	calculateDesiredOutputRegion(const InputImageType::RegionType& region);
public:
	std::vector<TemplateInfoType>	m_infoTemplates;
	std::vector<std::string>		m_imageFiles;
	std::string						m_StrErrorMessages;
};

/////////////////////////////////////////////////////////////////////////////////////////

template <typename ImageType>
typename ImageType::Pointer CADe::cePrivate::LoadImage(std::string strKernel)
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

bool CADe::cePrivate::processImage(std::string strImageFilePath)
{
	InputImageType::Pointer image = LoadImage<InputImageType>(strImageFilePath);

	InputImageType::RegionType region = image->GetLargestPossibleRegion();
	InputImageType::RegionType inputRegion = region;
	
	bool retVal = verifyImageSize(image, region,strImageFilePath);

	if (retVal) {
		
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

CADe::cePrivate::InputImageType::RegionType 
CADe::cePrivate::calculateDesiredOutputRegion(const InputImageType::RegionType& region)
{
	// The algorithm ignores a 160 pixel region on all sides of the image. This 
	// function calculates the region to output.

	CADe::cePrivate::InputImageType::RegionType retVal = region;

	auto lowerIndex = retVal.GetIndex();

	lowerIndex[0] += 160;
	lowerIndex[1] += 160;

	auto upperIndex = retVal.GetUpperIndex();
	upperIndex[0] -= 160;
	upperIndex[1] -= 160;

	retVal.SetIndex(lowerIndex);
	retVal.SetUpperIndex(upperIndex);

	return retVal;
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
