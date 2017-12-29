// $Id$
//------------------------------------------------------------------------------------
///
/// @file 
///
/// @brief <description>
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

// Pragma once to disable header stop warning
#pragma once

#ifndef _SRL_STEGIMG_HPP
#define _SRL_STEGIMG_HPP

#include "steg_data_types.hpp"
#include <opencv2\core\types_c.h>
#include <utility>
#include <string>



    ///
    /// @brief The srl namespace contains all of the StegDestroy buildings blocks, functionality
    /// and documentation needed to implement steganography removal. Nested within CML 
    ///
namespace srl
{
    ///
    /// @brief   This is the core container for all image based steganography removal. it has the ability to hold 
    ///	data using the OpenCV  Matrix class or the Magick++ Image class to account for as many image formats as possible
    ///
    /// @param[in] exception The error returned from either OpenCV or ImageMagick. 
    ///
    class Srl_steg_image
    {
    

        /*************************************************************************
        *
        *					Constructors + Destructors
        *
        *************************************************************************/
    public:
        ///
        /// @brief	default constructor for the steg image; Use get_format_pair() to create pair which you pass to ctor
        ///
        /// @description	TODO
        ///
        /// @param[in]	data_p		a pointer to the raw image data 
        /// @param[in]	img_format	a pair of enum to string created using get_format_pair()
        ///
        Srl_steg_image( unsigned char* img_data_p , size_t data_length, Srl_img_format_pair img_format );

        ///
        /// @brief	Alternative constructor for the steg image, takes an IplImage instead
        ///
        /// @description	TODO
        ///
        /// @param[in]	data_p		a pointer to the image data contained within an IplImage struct
        /// @param[in]	img_format	a pair of enum to string created using get_format_pair()
        ///
        Srl_steg_image( const IplImage* data_p , Srl_img_format_pair );

        ///
        /// @brief	Copy constructor for the steg image; 
        ///
        /// @description    TODO/NOTE - MIGHT NOT BE NECESSARY BUT IS GOOD TO HAVE 
        ///
        /// @param[in]	    img_copy_r  a const reference to the image to be copied
        ///
        Srl_steg_image( const Srl_steg_image& img_copy_r );

        ///
        /// @brief  destructor
        ///
        ~Srl_steg_image();


        /*************************************************************************
        *
        *					        Accessors
        *
        *************************************************************************/

    public:
        ///
        /// @brief  retrieves the string equivalent of the image format pair
        ///
        std::string format( void ) const;       
            
        ///
        /// @brief  retrieves the m_exception_p member to translate to a friendly error message by the handler
        ///
        const Srl_exception const* exception( void ) const;

        ///
        /// @brief retrieves exception status from the exception member pair 
        ///
        Srl_exception_status exception_status( void );

        ///
        /// @brief  retrieves the pointer to the buffer containing the image data
        ///
        /// @description    TODO
        ///
        const void* get_img_data( void ) const;
        

        /*************************************************************************
        *
        *					            Members
        *
        *************************************************************************/

    private:
        ///
        ///	@brief	m_mat_p		Matrix used to store any OpenCV compliant image files for cleaning
        ///
        shared_ptr<cv::Mat> m_mat_ap;

        ///
        ///	@brief	m_img_p		Image class used to store any ImageMagick compliant image files that OpenCV couldn't handle
        ///
        shared_ptr<Magick::Image> m_img_ap;

        ///
        /// @brief	m_format	Holds the pair of Enum to String for this image
        ///
        Srl_img_format_pair m_format;
        
        ///
        ///	@brief	m_err_status	contains the current error status or NONE if there were no problems 
        ///
        Srl_exception_status m_err_status;

        ///
        /// @brief	m_exception_p	pointer to the exception raised during processing if any, otherwise Nullptr
        ///
        unique_ptr<Srl_exception> m_exception_ap;


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
        /// @param[in]	data_p		a pointer to the image data contained within an IplImage struct
        ///
        /// @return bool    true if img format was successfully encoded or false if an error occurred
        ///
        bool encode( Srl_img_format_enum img_format_in);
    };
}



#endif //_SRL_STEGIMG_HPP