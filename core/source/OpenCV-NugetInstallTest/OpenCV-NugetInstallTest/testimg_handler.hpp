
#pragma once

#ifndef _TESTIMG_HANDLER_HPP
#define _TESTIMG_HANDLER_HPP

//#define USING_OCV

#include <string>
#include <memory>
#include <vector>



#include <opencv2\core.hpp>
//#include <opencv2\imgproc.hpp>
#include <opencv2\opencv.hpp>

#include "steg_logger.hpp"

/***************************************************************

							IMAGE_HANDLER

****************************************************************/

struct cv_test_image
{
	std::string m_filename;
	cv::Mat m_img_mat;
};

struct magick_test_image
{
	std::string m_filename;
	std::string m_format;
	//Magick::Image m_img_magick;
};

/*
	Directory structure
	-------------------
	payload_image_filepath
	origin_image_filepath
	converted_image_filepath
		->STEG_TYPE (e.g. LSB)
		  ->OPENCV|MAGICK
			->jpeg_chroma
			->jpeg_quality
			->original_chroma
			->original_quality

	-------------------------
	STEG_TYPE in the above is the only thing that needs to be added between the filepath 
	specifiers in the form ("TYPE") e.g. ("LSB")
*/
const std::string payload_image_filepath("..\\resources\\Images\\testimgs_payload\\");
const std::string origin_image_filepath("..\\resources\\Images\\testimgs_original\\");
const std::string converted_image_filepath("..\\resources\\Images\\testimgs_converted\\");
const std::string temp_stegimg_filepath("..\\resources\\Images\\testimgs_converted\\LSB\\OPENCV_PAYLOAD_JPG");

const std::string origin_image_filepath_absolute("C:\\Users\\Donghead\\source\\repos\\OpenCV-NugetInstallTest\\resources\\Images\\testimgs_original\\");

const std::string jpeg_chroma_filepath("\\JPEG_CHROMA\\");
const std::string jpeg_quality_filepath("\\JPEG_QUALITY\\");
const std::string jpeg_quality_chroma_filepath("\\JPEG_QUALITY_CHROMA\\");
const std::string original_format_chroma_filepath("\\ORIGINAL_FORMAT_CHROMA\\");
const std::string original_format_quality_filepath("\\ORIGINAL_FORMAT_QUALITY\\");
const std::string original_format_quality_chroma_filepath("\\ORIGINAL_FORMAT_QUALITY_CHROMA\\");

//This class handles only the reading and writing of images to disk

class image_handler
{
private:

	/////////////////////
	//PRIVATE MEMBERS  //
	/////////////////////

	std::vector<struct cv_test_image> m_imgs_cv;

	std::vector<struct magick_test_image> m_imgs_magick;

	//Multiple objects can have access to this logger resource at any given time
	//But ownership remains that of the caller (main in this case)
	std::shared_ptr<steg_logger> m_logger;

public:

	/************************************************

	CTOR + DTOR

	*************************************************/

	image_handler( std::vector<std::string> filenames, std::shared_ptr<steg_logger> steg_log ); 

	~image_handler();

	/************************************************

	UTILITY

	*************************************************/
	void get_images( std::vector<struct cv_test_image> &cv_imgs, std::vector<struct magick_test_image> &magick_imgs);

	void update_images( std::vector<struct cv_test_image> &cv_imgs, std::vector<struct magick_test_image> &magick_imgs);

	/************************************************

	CORE HANDLER WORK

	*************************************************/
	
	//encode and write the images in CV/Magick containers to JPEG using quality values:
	// [0-100] += 10
	bool jpg_write_images(void);

	//Encodes the images back to their original format and writes them to disk
	bool original_write_images(void);

	bool get_img_params(vector<int> &params, std::string filename);

	//this will write the passed in Matrix to the 
	bool write_image(std::string filepath, cv::Mat &mat, std::vector<int> &params);

};

#endif