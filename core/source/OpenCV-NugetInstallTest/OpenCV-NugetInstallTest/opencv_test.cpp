/*
 * This is a simple demo to read some images from the disk and use a few of OpenCV's image manipulation functions 
 * to change the images in a few ways. Won't make it interactive because we just want to make sure it's working
 * So will be coupled with a folder containing test images at the same directory level. 
 *
 * This will also be a good method of clarifying some stuff up with OpenCV as there are a few uncertainties 
 * with some of the underlying functionality. For example, OpenCV uses channels to determine colour depth so
 * JPG's by default are read using a 3 channel (for the Luma, and two chroma values presumably) but the default 
 * colour depth and whether it is using unsigned(most likely) or signed i'm unsure. 
 */

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>
#include <opencv2/highgui.hpp>
#include <iostream>


using namespace cv;
using namespace std;

const string text_divider("____________________________________________________________________");

int main(void)
{
	string origin_image_filepath("..\\resources\\images\\testimgs_original\\");
	string converted_image_filepath("..\\resources\\images\\testimgs_converted\\");
	string depthString(" bit colour, max pixel value: ");

	vector<string> image_names = {
		"Cover.bmp",
		"Cover.gif",
		"Cover.jpg",
		"Cover.png",
		"Cover.ppm",
		"Llama.jpg"	// This one is actually a JPG and OCV reads images from disk using the first few identifier bytes 
						// So this shouldn't be a problem unless windows changes it for some reason
	};

	vector<Mat> images_original;

	for (int i = 0; i < image_names.size(); i++)
	{
		Mat temp_matrix = imread(origin_image_filepath + image_names[i]);
		images_original.push_back(temp_matrix); //Sweet sweet vector memory management 
	}

	for (int i = 0; i < image_names.size(); i++)
	{
		cout << "Underlying Matrix structure for " << image_names[i] << endl << endl;
		cout << "Matrix Colour Depth: ";

		int bit_depth, bit_depth_max;
		switch (images_original[i].depth())
		{
			
			case CV_8U:
				bit_depth = 8;
				bit_depth_max = CHAR_MAX;
				break;

			case CV_8S:
				bit_depth = 7;
				bit_depth_max = UCHAR_MAX;
				break;
				
			case CV_16U:
				bit_depth = 16;
				bit_depth_max = SHRT_MAX;
				break;

			case CV_16S:
				bit_depth = 15;
				bit_depth_max = USHRT_MAX;
				break;

			default:
				cout << "Encountered 32bit float or signed integer" << endl;
		}
		cout << bit_depth << endl << "Max pixel value: " << bit_depth_max << endl;
		cout << "Number of channels for image type: " << images_original[i].channels() << endl;
		Size cvmat_size = images_original[i].size();
		cout << "Height (Number of rows): " << cvmat_size.height << endl << "Width (Number of columns): " << cvmat_size.width << endl;
		cout << "Raw image size (in bytes) = (height * width * channels) * sizeof(colour depth)" << endl;
		cout << "Image size (in kb): " << ( cvmat_size.height * cvmat_size.width * images_original[i].channels() )/1000 << endl;
		cout << text_divider << endl;

		if (imwrite(converted_image_filepath + image_names[i], images_original[i]))
		{
			cout << "Succesfully wrote " << image_names[i] << " to converted file path" << endl;
		}
	}


	while (1)
	{

	}
}