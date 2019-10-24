#include "itkImage.h"
#include "itkRandomImageSource.h"
#include "itkDerivativeImageFilter.h"

// This example does not seem to work
// The code is based on https://itk.org/ITKExamples/src/Core/Common/ApplyAFilterOnlyToASpecifiedRegionOfAnImage/Documentation.html

#ifdef ENABLE_QUICKVIEW
#include "QuickView.h"
#endif

int
main(int, char *[])
{
  constexpr unsigned int Dimension = 2;
  using PixelType = float;

  using ImageType = itk::Image<PixelType, Dimension>;

  ImageType::SizeType smallSize;
  smallSize.Fill(10);

  ImageType::IndexType index;
  index.Fill(0);

  ImageType::RegionType region(index, smallSize);

  ImageType::SizeType bigSize;
  bigSize.Fill(1000);

  using RandomSourceType = itk::RandomImageSource<ImageType>;
  RandomSourceType::Pointer randomImageSource = RandomSourceType::New();
  randomImageSource->SetNumberOfWorkUnits(1); // to produce non-random results
  randomImageSource->SetSize(bigSize);
  randomImageSource->GetOutput()->SetRequestedRegion(smallSize);

  std::cout << "Created random image." << std::endl;

  using DerivativeImageFilterType = itk::DerivativeImageFilter<ImageType, ImageType>;

  DerivativeImageFilterType::Pointer derivativeFilter = DerivativeImageFilterType::New();
  derivativeFilter->SetInput(randomImageSource->GetOutput());
  derivativeFilter->SetDirection(0); // "x" axis
  derivativeFilter->GetOutput()->SetRequestedRegion(smallSize);
 // derivativeFilter->Update();

  std::cout << "Computed derivative." << std::endl;


#ifdef ENABLE_QUICKVIEW
  QuickView viewer;
  viewer.AddImage<ImageType>(randomImageSource->GetOutput(), true, "Input Image" );
  viewer.AddImage<ImageType>(derivativeFilter->GetOutput(), true, "Output Image");
  viewer.SetViewPortSize(955);
  viewer.Visualize();
#endif

  return EXIT_SUCCESS;
}