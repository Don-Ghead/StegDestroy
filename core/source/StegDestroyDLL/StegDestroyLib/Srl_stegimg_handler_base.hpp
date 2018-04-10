#pragma once

#ifndef _SRL_STEGIMG_HANDLER_BASE_HPP
#define _SRL_STEGIMG_HANDLER_BASE_HPP

#include <memory>
#include <vector>
#include "Srl_stegimg_base.hpp"
#include "Srl_steg_logger.hpp"

namespace srl
{
	///
	/// @brief	Srl base stegimg handler for the JPGscrub component of the library
	///
	class Srl_stegimg_handler_base
	{
		/***************************************************
		*
		*					Members
		*
		****************************************************/
	protected:
		
		///
		/// @brief	Global Container of shared_ptr to all images
		///
		std::vector<std::shared_ptr<Srl_steg_image_base>> m_images_p;

		///
		/// @brief	Global Container of shared_ptr to error images
		///
		std::vector<std::shared_ptr<Srl_steg_image_base>> m_err_images_p;

	public:

		///
		/// @brief	has copy of Steg_logger member  
		///	
		std::shared_ptr<steg_logger> m_logger;

		/***************************************************
		*
		*					Methods
		*
		****************************************************/

	public:

		Srl_stegimg_handler_base(std::shared_ptr<steg_logger> logger);

		///
		/// @brief	Used for any post construction initialisation that may need to be done 
		///	
		virtual void initialise() = 0;

		///
		/// @brief	relinquishes ownership of the list of images retrieved
		///
		virtual vector<std::shared_ptr<Srl_stegimg_handler_base>> retrieve_images(bool failed_imgs) = 0;

		///
		/// @brief	cleans all images using specified configuration options, adding all images which 
		///			threw exceptions or critical warnings to the err_imgs vector
		///
		virtual bool clean_images(void) = 0;
	};
}

#endif