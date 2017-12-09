// fast.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "cv_fast_local_laplacian.h"
#include "hls_fast_local_laplacian.h"


int main(int argc, char* argv[])
{
	std::cout << "----- Fast Local Laplacian Pyramid -----" << std::endl;

	if(argc != 2){
		std::cerr << "Usage: " << argv[0] << " <input image>" << std::endl;
		return -1;
	}

	// Load image
	cv::Mat img_input;
	img_input = cv::imread(std::string(argv[1]));
	if (img_input.data == NULL) {
		std::cerr << "Failed to load image: " << argv[1] << std::endl;
		return -1;
	}

	// Scaling
	img_input.convertTo(img_input, CV_32FC3, 1.0f/255.0f);

	// Convert to grayscale
	cv::Mat yuv;
	cv::cvtColor(img_input, yuv, CV_BGR2YUV);
	
	std::vector<cv::Mat> planes;
	cv::split(yuv, planes);

#if 0
	// Show image
	cv::imshow("Original Grayscale Image", planes.at(0));
	cv::waitKey();
	cv::destroyWindow("Original Grayscale Image");
#endif

	// Parameters
	const float sigma = 0.1;
	const int N = 10;		// Num. of descretization step
	const float fact = 5;

	cv::Mat cv_in;
	cv::Mat hls_in;

	planes.at(0).copyTo(cv_in);
	planes.at(0).copyTo(hls_in);


	// OpenCV implementation
	cv::Mat cv_out;
	cv::Mat rgb;			// RGB image for output

	local_laplacian(cv_in, cv_out, sigma, fact, N);

	planes.at(0) = cv_out;
	cv::merge(planes, rgb);

	// YUV -> RGB -> 8 bit image
	cv::cvtColor(rgb, rgb, CV_YUV2BGR);
	rgb = rgb*255.0f;
	rgb.convertTo(rgb, CV_8UC3);

	// Show output image
//	cv::imshow("CV - Local Laplacian Pyramid", cv_out);
	cv::imshow("Output - OpenCV", rgb);
	cv::waitKey();
//	cv::destroyWindow("CV - Local Laplacian Pyramid");
	cv::destroyWindow("Output - OpenCV");

	// Save output image
	cv::imwrite("cv.tif", rgb);


	// HLS implementation
	cv::Mat hls_out;		// Enhanced grayscale image
	cv::Mat rgb2;		// RGB image for output

	hls_local_laplacian_wrap(hls_in, hls_out, sigma, fact, N);

	planes.at(0) = hls_out;
	cv::merge(planes, rgb2);

	// YUV -> 32-bit floating point RGB -> 8-bit RGB image
	cv::cvtColor(rgb2, rgb2, CV_YUV2BGR);
	rgb2 = rgb2*255.0f;
	rgb2.convertTo(rgb2, CV_8UC3);

	cv::imshow("Output - HLS", rgb2);
	cv::waitKey();
	cv::destroyWindow("Output - HLS");

	cv::imwrite("hls.tif", rgb2);


	std::cout << "----- Done -----" << std::endl;

    return 0;
}
