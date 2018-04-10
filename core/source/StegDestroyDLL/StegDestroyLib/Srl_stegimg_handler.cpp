//------------------------------------------------------------------------------------
///
/// @file   stegimg.cpp
///
/// @brief <description>
///
///
/// @section DESCRIPTION 
/// <long description>
//------------------------------------------------------------------------------------

#include "stdafx.h"

#include "Srl_stegimg_handler.hpp"
#include "Srl_steg_data_types.hpp"

using namespace srl;
using namespace std;

typedef std::vector<shared_ptr<Srl_steg_image>>::iterator image_iterator;

Srl_stegimg_handler_base::Srl_stegimg_handler_base(std::shared_ptr<steg_logger> logger)
	: m_logger(logger)
{
}

Srl_jpgscrub_stegimg_handler::Srl_jpgscrub_stegimg_handler(std::vector<std::shared_ptr<Srl_steg_image> >& img_data_v)
	: Srl_stegimg_handler_base(m_logger)
{
	//Swap ownership to our own image vector member 
	m_images_v = std::move(img_data_v);

	//Set to default for testing, will be extended to use values [0-100; +=10]
	m_compression_level = SRL_COMPRESSION_DEFAULT;
}

Srl_jpgscrub_stegimg_handler::~Srl_jpgscrub_stegimg_handler()
{
	//Handled by smart pointers
}

vector<shared_ptr<Srl_steg_image>> srl::Srl_jpgscrub_stegimg_handler::get_images()
{
	return m_images_v;
}

void Srl_jpgscrub_stegimg_handler::mat_to_magick(cv::Mat &cv_mat, Magick::Image &magick_img)
{
	//Construct an Magick Image using the array style conversion
	// (Columns, Rows, Colour mapping, Storage Type, and pointer to the data)
	magick_img = Magick::Image(cv_mat.cols, cv_mat.rows, "BGR", Magick::CharPixel, cv_mat.data);
}

void Srl_jpgscrub_stegimg_handler::magick_to_mat(Magick::Image &magick_img, cv::Mat &cv_mat)
{
	//Write the contents of the Magick Image to the cv_mat data member
	// (x, y, columsn, rows, colour mapping, storage type, pointer to data )
	magick_img.write(0, 0, magick_img.columns(), magick_img.rows(), "BGR", Magick::CharPixel, cv_mat.data);
}

Srl_exception_status Srl_jpgscrub_stegimg_handler::encode_all_to_format(Srl_img_format_pair img_format)
{
	Srl_exception_status status = SRL_EXCEPT_NONE;

	for (image_iterator iter = m_images_v.begin(); iter != m_images_v.end(); ++iter)
	{
		if (!(*iter)->encode(img_format, m_compression_level))
		{
			//Error occured during encoding 
			status = (*iter)->exception_status();
			string err_string;
			(*iter)->exception()->get_basic_except_info(err_string);
			m_logger_p->add_logfile_detail(err_string);
			m_err_images_v.push_back((*iter));
		}
	}
	//Let the caller know that an error occurred on at least one image and 
	//the caller can handle the Error images as desired
	return status;
}

Srl_exception_status srl::Srl_jpgscrub_stegimg_handler::encode_all_to_original_format()
{
	Srl_exception_status status = SRL_EXCEPT_NONE;

	for (image_iterator iter = m_images_v.begin(); iter != m_images_v.end(); ++iter)
	{
		if (!(*iter)->encode( (*iter)->m_format , m_compression_level))
		{
			//Error occured during encoding 
			status = (*iter)->exception_status();
			m_err_images_v.push_back((*iter));
		}
	}
	return status;
}
