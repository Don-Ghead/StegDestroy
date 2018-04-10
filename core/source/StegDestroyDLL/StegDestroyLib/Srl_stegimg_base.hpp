#pragma once

#ifndef _SRL_STEGIMG_BASE_HPP
#define _SRL_STEGIMG_BASE_HPP

#include "Srl_steg_data_types.hpp"
#include <string>
#include <memory>


namespace srl
{
	///
	/// @brief	Srl base image wrapper meant to be used as a base class from which 
	///			to create component implementations 
	///
	class Srl_steg_image_base
	{
		/***************************************************
		*
		*					Members
		*
		****************************************************/
	public:

		///
		/// @brief	Contains error status of the image 
		///			
		Srl_exception_status m_err_status;

	protected:

		///
		/// @brief void pointer to the raw buffer data of the image
		///
		void *m_raw_buf_p;

		///
		/// @brief	string version of the format in the form ".jpg"
		///			
		std::string m_format;

		///
		/// @brief	shared pointer to exception base class meant to be initiialized with 
		///			a component specific implementation of the exception class
		///	
		std::shared_ptr<Srl_exception_base> m_exception;

		/***************************************************
		*
		*					CTOR & DTOR
		*
		****************************************************/
	public:
		///
		/// @brief	Primary constructor for the steg image base, instantiates required members
		///
		/// @param[in]	raw_data	void pointer to the raw image data
		/// 
		/// @param[in]	img_format	string variant of the image format
		///
		/// @param[in]	status		Exception status if not SRL_EXCEPT_NONE
		///
		/// @param[in]	exception	nullptr unless image was created with an exception (never?)
		///
		Srl_steg_image_base(void* raw_data, 
							std::string img_format, 
							Srl_exception_status status = SRL_EXCEPT_NONE,
							std::shared_ptr<Srl_exception_base> exception = nullptr);

		/***************************************************
		*
		*					CORE
		*
		****************************************************/

		///
		/// @brief	retrieves the pointer to the raw buffer data
		///
		void* data(void);

		///
		/// @brief	Pure virtual function requiring implementation in all derived classes. 
		///
		/// @return	Returns exception member of the specific component (e.g. jpgscrub_exception)
		///
		/// @note	this may not be necessary and it may be possible to just return the exception member 
		///			and it can be cast to the specific component there, not sure
		///
		virtual std::shared_ptr<Srl_exception_base> error(void) = 0;

		///
		/// @brief	Pure virtual function requiring implementation in all derived classes. Encodes image to given 
		///			string variant format (e.g. ".jpg")
		///
		virtual bool encode(std::string format) = 0;
	};

}

#endif