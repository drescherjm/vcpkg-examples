#pragma once

#ifndef CADE_PRIVATE_H
#define CADE_PRIVATE_H

#include "CADe.h"

#include <cstdint>
#include <vector>
#include <filesystem>
#include <itkImage.h>
#include "CADe_Score.h"

/////////////////////////////////////////////////////////////////////////////////////////

const uint16_t IMAGE_SIZE_X{ 1100 };
const uint16_t IMAGE_SIZE_Y{ 1100 };

const uint16_t TEMPLATE_SIZE_MIN_X{ 3 };
const uint16_t TEMPLATE_SIZE_MIN_Y{ 3 };

const uint16_t IGNORE_SIZE_TOP{ 160 };
const uint16_t IGNORE_SIZE_LEFT{ 160 };

const uint8_t   MAXIMUM_SCORES{ 10 };

namespace fs = std::filesystem;
using OutputImageType = itk::Image<float, 2>;
using ConvolutionImageType = itk::Image<double, 2>;
using InputImageType = OutputImageType;

/////////////////////////////////////////////////////////////////////////////////////////

class CADe::cePrivate
{
public:
	cePrivate();
public:
	using TemplateInfoType = std::pair<std::string, uint16_t>;
	using InputPixelType = OutputImageType::InternalPixelType;

	using ScoreVector = std::vector<CADeScore>;

	enum TemplateType {
		TT_UNKNOWN,
		TT_SMALL,
		TT_BIG
	};

public:
	std::pair<bool, TemplateType> getTemplateType(const TemplateInfoType& info);
	bool hasImages() const;
	bool hasTemplates() const;
	bool processImage(std::string strImageFilePath);
	bool verifyImageSize(InputImageType::Pointer& image, InputImageType::RegionType& region,
		std::string& strImagePath);

	bool verifyKernelImageSize(InputImageType::Pointer& image, std::string& strKernelImagePath);

	std::pair<InputPixelType, InputPixelType>	getMinMax(InputImageType::Pointer pImage);

	template <typename ImageType>
	typename ImageType::Pointer					LoadImage(std::string strKernel);

	InputImageType::RegionType		calculateDesiredOutputRegion(const InputImageType::RegionType& region);

	bool processKernel(const TemplateInfoType& info,
		const std::string& strImageFilePath,
		InputImageType::Pointer pImage,
		InputImageType::RegionType& inputRegion,
		InputImageType::RegionType& outputRegion,
		ScoreVector & vecScores);

	OutputImageType::Pointer	performConvolution(const TemplateInfoType& info,
		InputImageType::Pointer pImage,
		InputImageType::Pointer pKernelImage,
		InputImageType::RegionType& inputRegion,
		InputImageType::RegionType& outputRegion);

	bool	calculateScores(const TemplateInfoType& info,
		OutputImageType::Pointer outputImage,
		ScoreVector& vecScores);

	bool	writeScoreOutputImageFile(const TemplateInfoType& info,
		std::string strInputImage,
		OutputImageType::Pointer output);

	std::string		getOutputFileName(const std::string& strImagePath, const std::string& strKernelPath);
	std::string		getFileNameNoPath(const std::string& strFilePath);

	void setPixel(OutputImageType* pImage, const OutputImageType::RegionType& region, 
		int nX, int nY, double fColor) const;

	void drawFilledCircleInImage(OutputImageType* pImage, 
		int xPos, int yPos, int nRadius, double fColor, double fCenterColor) const;
public:
	std::vector<TemplateInfoType>	m_infoTemplates;
	std::vector<std::string>		m_imageFiles;
	std::string						m_StrErrorMessages;
	uint8_t							m_nMaximumScores;
	bool							m_bDebugMode;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CADE_PRIVATE_H