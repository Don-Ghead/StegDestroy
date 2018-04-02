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
		///					each into it's own shared_ptr to an Srl_steg_image and added to a vector.
		///					It is done like this to make use of the efficiency of the std::move operator
		///
		Srl_jpgscrub_stegimg_handler( std::vector<std::shared_ptr<Srl_steg_image> > &img_data_v );

		~Srl_jpgscrub_stegimg_handler();

		/*************************************************************************
		*
		*					        Accessors
		*
		*************************************************************************/

	public:

		///
		/// @brief  Retrieve vector of all images
		///
		/// @note	if calling this method prior to destruction of the img_handler the user must 
		///			transfer ownership using the std::move() function, or else the images will go 
		///			out of scope. 
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
		/// @description	for now this will be a single compression level applied to all images.
		///					it will however be updated later to allow for different levels on individual images
		///
		Srl_jpgscrub_compression_level m_compression_level;

		///
		///	@brief	m_images_p		Vector containing shared_ptrs to all images, element indices for the vector
		///							passed to the constructor remain the same throughout the program.
		///
		std::vector< shared_ptr<Srl_steg_image> > m_images_v;

		///
		///	@brief	m_err_images_p	Vector containing shared_ptrs to only the images that encountered errors during
		///							the processing or encoding of the images themselves. Read/Write errors are likely
		///							the cause of an external reason so would not be included for example. 
		///
		std::vector< shared_ptr<Srl_steg_image> > m_err_images_v;


		/*************************************************************************
		*
		*					            Methods
		*
		*************************************************************************/
	public:

		///
		/// @brief	Converts the CV::Matrix to a Magick::Image
		///
		/// @description	TODO
		///
		/// @param[in]	cv_mat		Reference to CV matrix to be converted
		///
		/// @param[in]	magick_img	Reference to Magick Image to be filled 
		///
		/// @return void
		///
		static void mat_to_magick(cv::Mat &cv_mat, Magick::Image &magick_img);

		///
		/// @brief	Converts the Magick::Image to a CV::Matrix
		///
		/// @description	TODO
		///
		/// @param[in]	magick_img	Reference to Magick Image to be converted
		///
		/// @param[in]	cv_mat		Reference to CV matrix to be filled
		///
		/// @return void
		///
		static void magick_to_mat(Magick::Image &magick_img, cv::Mat &cv_mat);

		///
		/// @brief	Function to encode all images data into the provided format. Returns true if it succeeded without error
		///
		/// @description	If image is already in the specified format it will re-encode it using the compression level
		///					member. 
		///
		/// @param[in]	img_format_in	image format to encode to 
		///
		/// @return		Srl_exception_status	SRL_EXCEPT_NONE if all images were encoded without major errors
		/// 
		Srl_exception_status encode_all_to_format(Srl_img_format_pair img_format);

		///
		/// @brief	Function to encode  images back to their original formats using the built in image format member
		///
		/// @description	If image is already in host format post encoding (i.e. image was a JPEG to begin with) then 
		///					nothing happens to the image. 
		///
		/// @return		Srl_exception_status	SRL_EXCEPT_NONE if all images were encoded without major errors
		///
		Srl_exception_status encode_all_to_original_format(void);
	};

}


#endif