#include "testimg_handler.hpp"

#include <opencv2\imgcodecs\imgcodecs.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <chrono>
#include <omp.h>

using namespace std;
using std::cout;
using namespace cv;

image_handler::image_handler(vector<std::string> filenames, std::shared_ptr<steg_logger> steg_log)
	: m_logger(steg_log)
{
	for (int i = 0; i < filenames.size(); i++)
	{
		try //Read images into opencv container 
		{
			Mat temp_matrix = imread(origin_image_filepath + filenames[i], IMREAD_COLOR);
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

		//try // Try reading image files into Magick container
		//{			
		//	Magick::Image temp_img;
		//	temp_img.read(filenames[i]);
		//	cout << "Read image" << endl;
		//	magick_test_image magick_img;
		//	magick_img.m_filename = filenames[i];
		//	magick_img.m_img_magick = temp_img;
		//	magick_img.m_format = temp_img.magick();
		//	m_imgs_magick.push_back(magick_img);
		//}
		//catch (Magick::WarningCoder &warning)
		//{
		//	// Process coder warning while loading file (e.g. TIFF warning)
		//	// Maybe the user will be interested in these warnings (or not).
		//	// If a warning is produced while loading an image, the image
		//	// can normally still be used (but not if the warning was about
		//	// something important!)
		//	cerr << "Coder Warning: " << warning.what() << endl;
		//}
		//catch (Magick::Exception &e)
		//{
		//	cout << "Exception thrown during CV Reading of: " << origin_image_filepath + filenames[i] << endl;
		//	cout << "Exception description: " << e.what() << endl;
		//}
	}

}

image_handler::~image_handler()
{
}

void image_handler::get_images(vector<cv_test_image>& cv_imgs, vector<magick_test_image>& magick_imgs)
{
	cv_imgs = std::move(m_imgs_cv);
	//magick_imgs = std::move(m_imgs_magick);
}

void image_handler::update_images(vector<cv_test_image>& cv_imgs, vector<magick_test_image>& magick_imgs)
{
	m_imgs_cv = std::move(cv_imgs);
	//m_imgs_magick = std::move(magick_imgs);
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
	cout << "Cleaning images..." << endl;

	//To handle the writing out of files
	string filename_no_extension, original_extension;

	//	omp_set_num_threads(20);
	//#pragma omp parallel for 
	//Start at max quality and work down
	//int j = 70;
	vector<int> jpg_params, no_params;
	int num_img_iter = 0;
	std::chrono::duration<double> jpg_total, bmp_total;

	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < m_imgs_cv.size(); i++)
	{
		num_img_iter = 0; //Reset because we iterate through based on the number of images present.
							//the number of iterations is in relation to the below loop for quality parameter
		// [filename].jpg
		filename_no_extension = m_imgs_cv[i].m_filename.substr(0, m_imgs_cv[i].m_filename.find_last_of("."));
		// filename.[jpg]
		original_extension = m_imgs_cv[i].m_filename.substr(m_imgs_cv[i].m_filename.find_last_of(".") + 1);

		for (int j = MAX_JPG_QUALITY; j >= MIN_JPG_QUALITY; j -= 1)
		{
			//Clear params otherwise previous params pushed on will override each other
			jpg_params.clear();
			vector<unsigned char> temp_buf;

			jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
			jpg_params.push_back(j);
			if (original_extension == "jpg")
			{
				auto jpg_start = std::chrono::system_clock::now();
				write_image(converted_image_filepath + filename_no_extension + '_' + std::to_string(j) + ".jpg",
					m_imgs_cv[i].m_img_mat,
					jpg_params);
				auto jpg_end = std::chrono::system_clock::now();
				jpg_total += jpg_end - jpg_start;
			}
			else
			{
				auto bmp_start = std::chrono::system_clock::now();
				imencode(".jpg", m_imgs_cv[i].m_img_mat, temp_buf, jpg_params);
				Mat temp_mat(temp_buf);
				Mat write_mat = imdecode(temp_mat, CV_LOAD_IMAGE_COLOR);
				//cout << "Size after: " << m_imgs_cv[i].m_img_mat.total() * m_imgs_cv[i].m_img_mat.elemSize() << endl;

				write_image(converted_image_filepath + filename_no_extension + '_' + std::to_string(j) + "." + original_extension,
					write_mat,
					no_params);
				auto bmp_end = std::chrono::system_clock::now();
				bmp_total += bmp_end - bmp_start;
			}
			++num_img_iter;
		}
	}
	auto end = std::chrono::system_clock::now();

	cout << "Number of iterations: " << num_img_iter << endl << endl;

	//Compute total time for all images to be processed
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << endl << "Total Elapsed time for processing all images: " << elapsed_seconds.count() << "sec" << endl;
	auto calc_total = jpg_total + bmp_total;
	cout << "Calculated total based on each format chrono combined: " << calc_total.count() << endl;

	cout << "General overhead = " << (elapsed_seconds - calc_total).count() << "sec" << endl;
	cout << "_______________________________________________" << endl << endl;

	cout << endl << "Total elapsed time for image formats;" << endl << endl;
	cout << "\tJPG Total: " << jpg_total.count() << endl;
	cout << "\tBMP Total: " << bmp_total.count() << endl << endl;

	//Compute avg time for each img type to be processed
	auto jpg_avg = jpg_total / num_img_iter;
	auto bmp_avg = bmp_total / num_img_iter;

	cout << endl << "Average elapsed time for image formats;" << endl << endl;
	cout << "\tJPG Average: " << jpg_avg.count() << endl;
	cout << "\tBMP Average: " << bmp_avg.count() << endl << endl;

			/*
			//change only chroma quality factor
			jpg_params.clear();
			jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
			jpg_params.push_back(j);
			write_image(	chroma_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg",
							m_imgs_cv[i].m_img_mat,
							jpg_params);

			int x, y;
			uchar maskb, maskg, maskr;

			//Iterate Through x, y like a standard matrix container in C++
			Vec3b pixel_intensity = m_imgs_cv[i].m_img_mat.at<Vec3b>(x, y);
			//clear LSB of RGB bytes
			pixel_intensity.val[0] &= ~(1UL << 1); //Blue
			pixel_intensity.val[1] &= ~(1UL << 1); //Green
			pixel_intensity.val[2] &= ~(1UL << 1); //Red

			//Masks here are 1000 0000 to copy LSB
			maskb &= pixel_intensity.val[0]; //Blue
			maskg &= pixel_intensity.val[1]; //Green
			maskr &= pixel_intensity.val[2]; //Red

			//change both compression & chroma quality
			jpg_params.clear();
			jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
			jpg_params.push_back(j);
			jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
			jpg_params.push_back(j);
			write_image(	quality_chroma_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg",
							m_imgs_cv[i].m_img_mat,
							jpg_params);
			*/
	
	return success;
}

bool image_handler::original_write_images()
{
	bool success = false;

	return success;
}

bool image_handler::get_img_params(vector<int>& params, std::string filename)
{
	bool success = false;
	//Always clear, never leave up to caller.
	params.clear();

	// filename.[jpg]
	string extension = filename.substr(filename.find_last_of(".") + 1);

	if (("jpg" == extension) || ("jpeg" == extension))
	{
		params.push_back(cv::IMWRITE_JPEG_QUALITY);

	}
	else if ("png" == extension)
	{
		params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		params.push_back(3);
	}
	else if ("ppm" == extension)
	{
		params.push_back(cv::IMWRITE_PXM_BINARY);
	}
	else
	{
		cout << "Error retrieving params - Unsupported format: " << extension << endl;
	}

	return success;
}

bool image_handler::write_image(std::string filepath, cv::Mat &mat, std::vector<int> &params)
{
	bool status = true;

	try //write CV images 
	{
		//Implicitly converts back to original format given the filename extension
		imwrite(filepath, mat, params);
	}
	catch (cv::Exception &e)
	{
		cout << "Exception thrown during CV writing of: " << filepath << endl;
		cout << "Exception description: " << e.what() << endl;
		status = false;
	}


	//for(int j = 0; j < m_imgs_magick.size(); j++)
	//{
	//	try
	//	{
	//		//get image format from end of word & set the magick value to it to ensure that we write the original format out
	//		string img_format = m_imgs_magick[j].m_filename.substr(m_imgs_magick[j].m_filename.find_last_of("."));
	//		m_imgs_magick[j].m_img_magick.magick(img_format);
	//		m_imgs_magick[j].m_img_magick.write(converted_image_filepath + m_imgs_magick[j].m_filename);
	//	}
	//	catch (Magick::Exception & error)
	//	{
	//		// Handle problem while rotating or writing outfile.
	//		cerr << "Caught Magick++ exception during write: " << error.what() << endl;
	//		status = 2;
	//	}
	//}
	return status;
}
