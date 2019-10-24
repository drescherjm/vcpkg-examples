#pragma once

#ifndef CONVOLUTION1_H
#define CONVOLUTION1_H

#include <string>
#include <itkImage.h>

/////////////////////////////////////////////////////////////////////////////////////////

//using InputImageType = itk::Image<unsigned short, 2>;
using OutputImageType = itk::Image<float, 2>;
using InputImageType = OutputImageType;

/////////////////////////////////////////////////////////////////////////////////////////

int writeOutputFile(std::string strImageFile, std::string strKernel, OutputImageType::Pointer output);
InputImageType::Pointer LoadKernel(std::string strKernel);

/////////////////////////////////////////////////////////////////////////////////////////

#endif // CONVOLUTION1_H