#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
	String imageName("HappyFish.jpg"); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}
	Mat image;
	image = imread(samples::findFile(imageName), IMREAD_GRAYSCALE | IMREAD_ANYDEPTH); // Read the file
	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.


	cv::Point min_loc, max_loc;
	double min, max;
	cv::minMaxLoc(image, &min, &max, &min_loc, &max_loc);


	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, 5.0) * 255.0);
	Mat res = image.clone();
	LUT(image, lookUpTable, res);

	imshow("Display window", res);                // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}