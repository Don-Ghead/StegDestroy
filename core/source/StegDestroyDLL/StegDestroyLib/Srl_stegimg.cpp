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

#include "Srl_stegimg.hpp"

using namespace srl;
using namespace Magick;
using namespace cv;
using namespace std;

/***************************************************
*
*					Steg Image base
*
****************************************************/

Srl_steg_image_base::Srl_steg_image_base(void* raw_data,
	std::string img_format,
	Srl_exception_status status,
	std::shared_ptr<Srl_exception_base> exception)
	
	:	m_raw_buf_p(raw_data),
		m_format(img_format),
		m_err_status(status),
		m_exception(exception)
		
{
}

void* Srl_steg_image_base::data(void)
{
	return m_raw_buf_p;
}

/***************************************************
*
*				JpgScrub steg_image
*
****************************************************/
Srl_steg_image::Srl_steg_image( unsigned char * data_p , size_t data_length , Srl_img_format_pair img_format )
    :   Srl_steg_image_base( (void*)data_p, img_format.second),
		m_format(img_format),
        m_exception_p(nullptr),
        m_err_status(SRL_EXCEPT_NONE)
{
    if ( is_format_magick_supported( img_format.second ) )
    {
        try
        {
            // as this can fail, we'll avoid potential memory leak by putting this first step
            // into it's own try catch 
            Blob temp_blob( data_p , data_length );
            try
            {
                // this is where we try to read our data into the blob and the image so there is potential 
                // for a fair few things going wrong hence the number of errors to catch
                m_img_p.reset( new Magick::Image( temp_blob ) );
            }
			catch (Magick::Error & e)
			{
				m_exception_p.reset(new Srl_exception(e));
				m_err_status = SRL_ERROR_IMAGEMAGICK;
			}
            // If we got here then we have successfully read the image data into the image member
			m_mat_p = nullptr;
        }
        catch ( Magick::Exception & e )
        {
			m_exception_p.reset(new Srl_exception(e));
			m_err_status = SRL_EXCEPT_IMAGEMAGICK;
        }
    }

	//EXAMPLE CODE----------------
#include <iostream>
	string error_msg;
	if (is_format_magick_supported(img_format.second))
	{
		try
		{
			// as this can fail, we'll avoid potential memory leak by putting this first step
			// into it's own try catch 
			Blob temp_blob(data_p, data_length);
			try
			{
				// this is where we try to read our data into the blob and the image so there is potential 
				// for a fair few things going wrong hence the number of errors to catch
				m_img_p.reset(new Magick::Image(temp_blob));
			}
			catch (Magick::WarningCoder &e)
			{
				if ("CODER::FORMAT_INVALID" == e.what())
				{
					error_msg = "Warning: non-critical error with format of :" + img_format.second;
					m_err_status = SRL_WARNING_FORMAT_INVALID;
				}
				else if("CODER::WRONG_CH_TYPE" == e.what())
				{
					error_msg = "Warning: non-critical error with channel type of:" + img_format.second;
					m_err_status = SRL_WARNING_CHTYPE;
				}
			}
			catch (Magick::Error & e)
			{
				m_exception_p.reset(new Srl_exception(e));
				m_err_status = SRL_ERROR_IMAGEMAGICK;
			}
			// If we got here then we have successfully read the image data into the image member
			m_mat_p = nullptr;
		}
		catch (Magick::Exception & e)
		{
			m_exception_p.reset(new Srl_exception(e));
			m_err_status = SRL_EXCEPT_IMAGEMAGICK;
		}
	}

	//EXAMPLE CODE-----------

	//Magick++ doesn't provide a validate() function like the scripting versions do, perhaps
	//because this is done by default in the image constructor but because we're carpet catching 
	//errors & exceptions, any issues with the image should be caught in there so check if our 
	//exception member was set instead
    else if( nullptr == m_exception_p ) 
    {
        //we haven't been able to read the image using Magick++ so try with OpenCV
        //Try to Decode the buffer to a matrix 
        try
        {
			m_mat_p.reset( new cv::Mat );
            imdecode( std::vector<char>( *data_p ) , IMREAD_COLOR , m_mat_p.get() );
        }
        catch ( cv::Exception & e )
        {
			m_exception_p.reset(new Srl_exception(e));
			m_err_status = SRL_EXCEPT_OPENCV;
        }

        //Make sure that the data is there 
        if ( nullptr == m_mat_p->data  )
        {  //this value will be checked for before any encoding is done 
            m_err_status = SRL_EXCEPT_READ;
        }
		else
		{
			m_img_p = nullptr;
		}
    }
}

//Rework with smart pointers
Srl_steg_image::Srl_steg_image( Srl_steg_image & img_copy )
	:	Srl_steg_image_base(img_copy.data(), img_copy.format(), img_copy.exception_status(), img_copy.m_exception),
		m_format(img_copy.m_format),
		m_exception_p(img_copy.exception()),
		m_err_status(img_copy.m_err_status)
{
    m_err_status = SRL_EXCEPT_NONE;
    //First copy the image data from either CV or Magick member
    if ( nullptr != img_copy.m_img_p.get() )
    {
        m_img_p.reset(new shared_ptr<Magick::Image>( img_copy.m_img_p));
		m_mat_p = nullptr;
    }
    else if ( nullptr != img_copy.m_mat_p.get() )
    {
        m_mat_p.reset(new shared_ptr<cv::Mat>(img_copy.m_mat_p));
    }
    //now the format + check the error status to see if we need to get the exception
    m_format = img_copy.m_format;
    if ( SRL_EXCEPT_NONE != img_copy.m_err_status )
    {
        m_err_status = img_copy.m_err_status;
		m_exception_p.reset(new Srl_exception(img_copy.exception));
    }
}

Srl_steg_image::~Srl_steg_image()
{
    //All managed using smart pointers so nothing to worry about 
}

///
/// @brief returns the friendly format name 
///
std::string Srl_steg_image::format( void ) const
{
    return m_format.second;
}

///
/// @brief returns the Srl_exception member if there is one otherwise nullptr
///
std::shared_ptr<Srl_exception> Srl_steg_image::exception( void ) 
{
    return m_exception_p;
}

///
/// @brief returns the exception status value, default: SRL_EXCEPT_NONE
///
Srl_exception_status Srl_steg_image::exception_status( void )
{
    return m_err_status;
}

///
/// @brief gets pointer to image data buffer
///
const void* Srl_steg_image::get_img_data( void ) const
{
    if ( nullptr != m_img_p.get() )
    {
        Blob blob;
        m_img_p->write( &blob );
        return blob.data();
    }
    else 
    {   //This will either return the data or nullptr 
        return m_mat_p->data;
    }
}

///
/// @brief compression lvl optional parameter (has default value)
///
bool Srl_steg_image::encode( Srl_img_format_pair img_format_in, Srl_jpgscrub_compression_level compression_lvl)
{
	bool success = false;
	if (nullptr != m_img_p.get()) 
	{
		try
		{
			m_img_p->magick(img_format_in.second);
		}
		catch (Magick::Exception &e)
		{
			m_exception_p.reset(new Srl_exception(e));
			m_err_status = SRL_EXCEPT_IMAGEMAGICK;
		}
	}
	else if (nullptr != m_mat_p.get())
	{
		vector<int> cv_params;
		vector<uchar> cv_outbuf;

		//Push the JPEG quality parameters on 
		cv_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		cv_params.push_back(compression_lvl);

		//This reserves the current amount of memory the matrix holds, in theory we're 
		//Only ever going to be shrinking images in size, however multiply by 1.5 for a buffer
		size_t reserve_bytes = 1.5 * m_mat_p->rows * m_mat_p->cols * m_mat_p->channels;
		cv_outbuf.reserve(reserve_bytes);

		try {
			if (imencode(img_format_in.second, *m_mat_p, cv_outbuf, cv_params))
			{
				//Sucessfully encoded the image into the new output buffer
				//Should be able to just copy it into the member matrix without it 
				//being deallocated.
				*m_mat_p = imdecode(cv_outbuf, IMREAD_COLOR);
			}
		}
		catch(cv::Exception &e)
		{
			m_exception_p.reset(new Srl_exception(e));
			m_err_status = SRL_EXCEPT_OPENCV;
		}
	}
	else
	{
		m_err_status = SRL_ERROR_OTHER;
	}
}
