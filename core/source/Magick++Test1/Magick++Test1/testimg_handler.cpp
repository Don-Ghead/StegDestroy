#include "testimg_handler.hpp"

#include "stdafx.h"

using namespace std;
using namespace cv;

image_handler::image_handler(vector<std::string> filenames, std::shared_ptr<steg_logger> steg_log)
	: m_logger(steg_log)
{
	for (int i = 0; i < filenames.size(); i++)
	{
		try //Read images into opencv container 
		{
			Mat temp_matrix = imread(origin_image_filepath + filenames[i]);
			cv_test_image test_img;
			test_img.m_filename = filenames[i]; //Just assign filename and we'll add to converted img filepath later
			test_img.m_img_mat = temp_matrix;
			m_imgs_cv.push_back(test_img);
		}
		catch (cv::Exception &e)
		{
			cout << "Exception thrown during CV Reading of: " << filenames[i] << endl;
			cout << "Exception description: " << e.what() << endl;
		}

		try // Try reading image files into Magick container
		{			
			Magick::Image temp_img;
			temp_img.read(filenames[i]);
			cout << "Read image" << endl;
			magick_test_image magick_img;
			magick_img.m_filename = filenames[i];
			magick_img.m_img_magick = temp_img;
			magick_img.m_format = temp_img.magick();
			m_imgs_magick.push_back(magick_img);
		}
		catch (Magick::WarningCoder &warning)
		{
			// Process coder warning while loading file (e.g. TIFF warning)
			// Maybe the user will be interested in these warnings (or not).
			// If a warning is produced while loading an image, the image
			// can normally still be used (but not if the warning was about
			// something important!)
			cerr << "Coder Warning: " << warning.what() << endl;
		}
		catch (Magick::Exception &e)
		{
			cout << "Exception thrown during CV Reading of: " << origin_image_filepath + filenames[i] << endl;
			cout << "Exception description: " << e.what() << endl;
		}
	}
	
}

image_handler::~image_handler()
{
}

void image_handler::get_images(vector<cv_test_image>& cv_imgs, vector<magick_test_image>& magick_imgs)
{
	cv_imgs = std::move(m_imgs_cv);
	magick_imgs = std::move(m_imgs_magick);
}

void image_handler::update_images(vector<cv_test_image>& cv_imgs, vector<magick_test_image>& magick_imgs)
{
	m_imgs_cv = std::move(cv_imgs);
	m_imgs_magick = std::move(magick_imgs);
}

#define MIN_JPG_QUALITY 0 
#define MAX_JPG_QUALITY 100
#define JPG_QUALITY_INCREMENT 10

//For now we'll just do the testing with Magick and extend to CV once we've got it working. 
bool image_handler::jpg_write_images()
{
	//Verifying an image has been read correctly relies mostly on catching warnings & errors
	//So in this case set success to true and change to false if we catch any critical problems
	bool success = true;
	cout << "Encoding Images to JPG..." << endl;
	
	//Create the image path: Magick++ does not allow access to changing sampling factor directly
	//So can only test image quality which encompasses sampling factor but does not allow variable 
	//control over it to isolate image factors that may affect the end result of the steganography
	string quality_image_path(converted_image_filepath + "LSB\\OPENCV" + jpeg_quality_filepath);

	for (int i = 0; i < m_imgs_cv.size(); i++)
	{
		cv::Mat img_copy, img_copy_2;
		img_copy = img_copy_2 = m_imgs_cv[i].m_img_mat;
		//Start at max quality and work down
		for (int j = MAX_JPG_QUALITY; j <= MIN_JPG_QUALITY; j -= JPG_QUALITY_INCREMENT)
		{
			try //write CV images 
			{
				//Change only compression quality
				vector<int> jpg_params;
				jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				jpg_params.push_back(j);
				if (!imwrite(converted_image_filepath + m_imgs_cv[i].m_filename, m_imgs_cv[i].m_img_mat, jpg_params))
				{
					cout << "imwrite unable to write: " << m_imgs_cv[i].m_filename << " to converted file path" << endl;
				}

				//change only chroma quality factor
				jpg_params.clear();
				jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
				jpg_params.push_back(j);
				if (!imwrite(converted_image_filepath + m_imgs_cv[i].m_filename, img_copy, jpg_params))
				{
					cout << "imwrite unable to write: "  << m_imgs_cv[i].m_filename << " to converted file path" << endl;
				}

				//change both compression & chroma quality 
				jpg_params.clear();
				jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				jpg_params.push_back(j);
				jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
				jpg_params.push_back(j);
				if (!imwrite(converted_image_filepath + m_imgs_cv[i].m_filename, img_copy_2, jpg_params))
				{
					cout << "imwrite unable to write: "  << m_imgs_cv[i].m_filename << " to converted file path" << endl;
				}
			}
			catch (cv::Exception &e)
			{
				cout << "Exception thrown during CV writing of: " << m_imgs_cv[i].m_filename << endl;
				cout << "Exception description: " << e.what() << endl;
				success = false;
			}
		}
	}

	//Create the image path: Magick++ does not allow access to changing sampling factor directly
	//So can only test image quality which encompasses sampling factor but does not allow variable 
	//control over it to isolate image factors that may affect the end result of the steganography
	quality_image_path = converted_image_filepath + "LSB\\MAGICK" + jpeg_quality_filepath;

	string filename_no_extension;
	Magick::Image img_copy, img_copy_2;

	for (int j = 0; j < m_imgs_magick.size(); j++)
	{
		//Find the last "." and copy from the start of the filename to that point - 1 
		filename_no_extension = m_imgs_magick[j].m_filename.substr(0, m_imgs_magick[j].m_filename.find_last_of(".") - 1);

		cout << "Writing images in JPG quality 0-100 in 10 increments suffixed with _JPG[0-100] using Magick++: " << endl;

		//Start at max quality and work down
		for (int i = MAX_JPG_QUALITY; i <= MIN_JPG_QUALITY; i -= JPG_QUALITY_INCREMENT)
		{
			try
			{
				//Change quality only
				m_imgs_magick[j].m_img_magick.quality(i);
				m_imgs_magick[j].m_img_magick.magick("jpg");
				m_imgs_magick[j].m_img_magick.write(quality_image_path + filename_no_extension + "_" + to_string(i) + ".jpg");				
			}
			catch (Magick::Exception & e)
			{
				cout << "Exception occurred during magick write, error description: " << e.what() << endl;
				success = false;
			}
		}
	}

	quality_image_path = converted_image_filepath + "LSB\\OPENCV" + jpeg_quality_filepath;

	return success;
}

int image_handler::write_image(bool return_to_orig_format)
{
	int status = 0; //Non-zero indicates failure
	string quality_image_path(converted_image_filepath + "LSB\\MAGICK" + original_format_quality_filepath);
	for (int i = 0; i < m_imgs_cv.size(); i++)
	{
		try //write CV images 
		{
			//Implicitly converts back to original format
			if (imwrite(converted_image_filepath + m_imgs_cv[i].m_filename, m_imgs_cv[i].m_img_mat))
			{
				cout << "Succesfully wrote " << m_imgs_cv[i].m_filename << " to converted file path" << endl;
			}
		}
		catch (cv::Exception &e)
		{
			cout << "Exception thrown during CV writing of: " << m_imgs_cv[i].m_filename << endl;
			cout << "Exception description: " << e.what() << endl;
			status = 1;
		}
	}

	for(int j = 0; j < m_imgs_magick.size(); j++)
	{
		try
		{
			//get image format from end of word & set the magick value to it to ensure that we write the original format out
			string img_format = m_imgs_magick[j].m_filename.substr(m_imgs_magick[j].m_filename.find_last_of("."));
			m_imgs_magick[j].m_img_magick.magick(img_format);
			m_imgs_magick[j].m_img_magick.write(converted_image_filepath + m_imgs_magick[j].m_filename);
		}
		catch (Magick::Exception & error)
		{
			// Handle problem while rotating or writing outfile.
			cerr << "Caught Magick++ exception during write: " << error.what() << endl;
			status = 2;
		}
	}
	return status;
}
