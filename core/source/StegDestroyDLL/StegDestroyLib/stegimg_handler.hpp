// $Id$
//------------------------------------------------------------------------------------
///
/// @file 
///
/// @brief Primary stegimg handler for the Srl::jpgscrub project
///
/// @section VERSION
/// 4.10.0
///
/// @section COPYRIGHT
/// &copy; June 2017 Clearswift Ltd. All rights reserved.
///
/// @section DESCRIPTION 
/// <long description>
//------------------------------------------------------------------------------------

#pragma once

#ifndef _STEGIMG_HANDLER_HPP
#define _STEGIMG_HANDLER_HPP

#include <vector>
#include "stegimg.hpp"



namespace srl
{
	
	class Srl_jpgscrub_stegimg_handler
	{
		/*************************************************************************
		*
		*					Constructors + Destructors
		*
		*************************************************************************/
	public:
		///
		/// @brief	default constructor for the steg image handler, takes Srl_stegimg wrapped images
		///
		/// @description	Using this constructor means the user has already called the img helper
		///					functions to determine whether the img format is supported and wrapped 
		///					each into it's own Srl_steg_image, adding to a vector and passing in by reference.
		///
		Srl_jpgscrub_stegimg_handler( std::vector<Srl_steg_image> &img_data_v );

		///
		/// @brief	constructor for the steg image handler, takes raw image data instead
		///
		/// @description	This is for raw data which has an unknown format (as far as the program is concerned)
		///					and will attempt to decode each one into an Srl_stegimg so that post construction, both 
		///					handler constructors would be at the same stage of processing. 
		///
		Srl_jpgscrub_stegimg_handler(std::vector<void*> &img_data_v);

		~Srl_jpgscrub_stegimg_handler();

		/*************************************************************************
		*
		*					        Accessors
		*
		*************************************************************************/

	public:
		///
		/// @brief  Retrieve the vector of Srl_steg_images 
		///
		vector< shared_ptr<Srl_steg_image> > get_images();
		
		/*************************************************************************
		*
		*					            Members
		*
		*************************************************************************/

	private:

		///
		///	@brief	m_compression_level		Enumerated value to simplify known compression levels
		///
		Srl_jpgscrub_compression_level m_compression_level;

		///
		///	@brief	m_images_p		Vector containing shared_ptrs to all images, element indices for the vector
		///							passed to the constructor remain the same throughout the program.
		///
		std::vector< shared_ptr<Srl_steg_image> > m_images_p;

		///
		///	@brief	m_err_images_p	Vector containing shared_ptrs to only the images that encountered errors during
		///							the processing or encoding of the images themselves. Read/Write errors are likely
		///							the cause of an external reason so would not be included for example. 
		///
		std::vector< shared_ptr<Srl_steg_image> > m_err_images_p;


		/*************************************************************************
		*
		*					            Methods
		*
		*************************************************************************/
	public:

		///
		/// @brief	Function to encode the image data into the provided format. Returns true if it succeeded without error
		///
		/// @description	TODO
		///
		/// @param[in]	img_format_in	image format to encode to 
		///
		/// @return bool    true if img format was successfully encoded or false if an error occurred
		///
		static void mat_to_magick(cv::Mat &cv_mat, Magick::Image magick_img);

		static void magick_to_mat(Magick::Image &magick_img, cv::Mat &cv_mat);

		///
		/// @brief	Function to encode the image data into the provided format. Returns true if it succeeded without error
		///
		/// @description	TODO
		///
		/// @param[in]	img_format_in	image format to encode to 
		///
		/// @return bool    true if img format was successfully encoded or false if an error occurred
		///
		static Srl_exception_status encode_to_format(Srl_img_format_enum img_format);
	};

}


#endif