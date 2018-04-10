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

#include <Magick++.h>

//#include <filesystem> //Can extend to use the new C++17 filesystem header to just take a path 
//and use all files from that. 

#include "testimg_handler.hpp"
#include "steg_logger.hpp"

using namespace cv;
using namespace std;

const string text_divider("____________________________________________________________________");
const string depthString(" bit colour, max pixel value: ");
//For now we'll just populate this vector with the image names
vector<string> image_names = {
	//"JPG_Test.jpg",		
	"PNG_Test.png",
	"GIF_Test.gif"
	//"PPM_Test.ppm"
};

int main(int argc, char **argv)
{
	
	cout << "CV build information: " << cv::getBuildInformation() << endl;

	std::shared_ptr<steg_logger> main_logger(new steg_logger(Log_level::DEFAULT));

	image_handler img_hand(image_names, main_logger);



	img_hand.jpg_write_images();




}