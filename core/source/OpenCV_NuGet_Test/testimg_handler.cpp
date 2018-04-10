#include "testimg_handler.hpp"


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
	cout << "Encoding Images to JPG..." << endl;
	
	//Create image paths for each test set
	string original_quality_image_path(converted_image_filepath + "LSB\\OPENCV" + original_format_quality_filepath);
	string quality_image_path(converted_image_filepath + "LSB\\OPENCV" + jpeg_quality_filepath);
	string chroma_image_path(converted_image_filepath + "LSB\\OPENCV" + jpeg_chroma_filepath);
	string quality_chroma_image_path(converted_image_filepath + "LSB\\OPENCV" + jpeg_quality_chroma_filepath);
	//To handle the writing out of files
	string filename_no_extension, original_extension;

	for (int i = 0; i < m_imgs_cv.size(); i++)
	{
		// [filename].jpg
		filename_no_extension = m_imgs_cv[i].m_filename.substr(0, m_imgs_cv[i].m_filename.find_last_of("."));
		// filename.[jpg]
		original_extension = m_imgs_cv[i].m_filename.substr(m_imgs_cv[i].m_filename.find_last_of(".") + 1);

		//TRy decoding it to buffer to write out after

		//Start at max quality and work down
		for (int j = MAX_JPG_QUALITY; j >= MIN_JPG_QUALITY; j -= JPG_QUALITY_INCREMENT)
		{
			
				//Change only compression quality
				vector<int> jpg_params;
				vector<unsigned char> temp_buf;
				jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				jpg_params.push_back(j);
				jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
				jpg_params.push_back(100);

				cout << "Size before Jpeg: " << m_imgs_cv[i].m_img_mat.total() * m_imgs_cv[i].m_img_mat.elemSize() << endl;

				
				if(imencode(".jpg", m_imgs_cv[i].m_img_mat, temp_buf, jpg_params))
				{ 
					Mat temp_mat(temp_buf);
					temp_mat = imdecode( temp_mat, CV_LOAD_IMAGE_COLOR);
					cout << "Size after: " << m_imgs_cv[i].m_img_mat.total() * m_imgs_cv[i].m_img_mat.elemSize() << endl;

					write_image(quality_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg",
						temp_mat,
						jpg_params);
					
					vector<int> png_params;
					//get_img_params(png_params, m_imgs_cv[i].m_filename);
					//We don't want to try and write it back to it's original format if the previous failed as well
					write_image(	original_quality_image_path + filename_no_extension + '_' + std::to_string(j) + ".png",
									temp_mat,
									png_params);
				}
				else
				{
					cout << "Quality - imwrite unable to write: " << quality_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg" << endl;
				}

				//change only chroma quality factor
				jpg_params.clear();
				jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				jpg_params.push_back(100);
				jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
				jpg_params.push_back(j);
				write_image(	chroma_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg", 
								m_imgs_cv[i].m_img_mat,
								jpg_params);
				

				//change both compression & chroma quality 
				jpg_params.clear();
				jpg_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				jpg_params.push_back(j);
				jpg_params.push_back(cv::IMWRITE_JPEG_CHROMA_QUALITY);
				jpg_params.push_back(j);
				write_image(	quality_chroma_image_path + filename_no_extension + '_' + std::to_string(j) + ".jpg",
								m_imgs_cv[i].m_img_mat,
								jpg_params);
				
			
		}
	}

	//Create the image path: Magick++ does not allow access to changing sampling factor directly
	//So can only test image quality which encompasses sampling factor but does not allow variable 
	//control over it to isolate image factors that may affect the end result of the steganography
	//quality_image_path = converted_image_filepath + "LSB\\MAGICK" + jpeg_quality_filepath;

	//string filename_no_extension;
	//Magick::Image img_copy, img_copy_2;

	//for (int j = 0; j < m_imgs_magick.size(); j++)
	//{
	//	//Find the last "." and copy from the start of the filename to that point - 1 
	//	filename_no_extension = m_imgs_magick[j].m_filename.substr(0, m_imgs_magick[j].m_filename.find_last_of(".") - 1);

	//	cout << "Writing images in JPG quality 0-100 in 10 increments suffixed with _JPG[0-100] using Magick++: " << endl;

	//	//Start at max quality and work down
	//	for (int i = MAX_JPG_QUALITY; i <= MIN_JPG_QUALITY; i -= JPG_QUALITY_INCREMENT)
	//	{
	//		try
	//		{
	//			//Change quality only
	//			m_imgs_magick[j].m_img_magick.quality(i);
	//			m_imgs_magick[j].m_img_magick.magick("jpg");
	//			m_imgs_magick[j].m_img_magick.write(quality_image_path + filename_no_extension + "_" + to_string(i) + ".jpg");				
	//		}
	//		catch (Magick::Exception & e)
	//		{
	//			cout << "Exception occurred during magick write, error description: " << e.what() << endl;
	//			success = false;
	//		}
	//	}
	//}

	//quality_image_path = converted_image_filepath + "LSB\\OPENCV" + jpeg_quality_filepath;

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
