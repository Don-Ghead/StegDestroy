// $Id$
//------------------------------------------------------------------------------------
///
/// @file 
///
/// @brief <description>
///
/// @section VERSION
/// 1.0
///
///
/// @section DESCRIPTION 
/// <long description>
//------------------------------------------------------------------------------------


#ifdef STEGDESTROYLIB_EXPORTS
#define STEGDESTROYLIB_API __declspec(dllexport)
#else
#define STEGDESTROYLIB_API __declspec(dllimport)
#endif

#pragma once 
#ifndef _CML_STEG_DATA_TYPES_HPP
#define _CML_STEG_DATA_TYPES_HPP

#include <string>
#include <memory>

#include <Magick++.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

///
/// @brief The srl namespace contains all of the StegDestroy buildings blocks, functionality
/// and documentation needed to implement steganography removal. Nested within CML 
///
namespace srl
{


    /*************************************************************************
    *
    *					Srl Exception Classes + data types
    *
    *************************************************************************/

    ///
    /// @brief   Basic abstraction for exceptions raised by OpenCV/Magick++
    /// 
    enum Srl_exception_status
    {
        SRL_EXCEPT_NONE ,
        SRL_EXCEPT_OPENCV ,
        SRL_EXCEPT_IMAGEMAGICK ,
        SRL_EXCEPT_OTHER,
        SRL_EXCEPT_READ
    };

    ///
    /// @brief   basic exception class which holds the explicit error information needed to 
    /// diagnose an error for either OpenCV or ImageMagick libraries. 
    ///
    /// @param[in] exception The error returned from either OpenCV or ImageMagick. 
    ///
    struct Srl_exception
    {
    private:

        ///
        ///	@brief	holds the openCV version of an exception
        ///
        unique_ptr<cv::Exception> m_cv_except_ap;

        ///
        /// @brief	holds the Magick++ version of an exception
        ///	
        unique_ptr<Magick::Exception> m_im_except_ap;

        ///
        ///	@brief helper function used to determine what error message to return 
        ///
        bool is_cv_exception( void );

    public:

        ///
        /// @brief Constructors must take one of either CV or IM exceptions
        ///
        Srl_exception( std::unique_ptr<cv::Exception> exception );
        Srl_exception::Srl_exception( std::unique_ptr<Magick::Exception> exception );

        ///
        /// @brief  simple destructor
        ///
        ~Srl_exception() {}

        ///
        /// @brief	Retrieves a string containing the basic diagnostic information for the exception member 
        ///
        /// @param[out] except_str returns basic exception information in friendly string format
        ///
        bool get_basic_except_info( string& except_str );

    }; //Srl_exception


    /*************************************************************************
    *
    *					Srl general data types + functions
    *
    *************************************************************************/

    ///
    /// @brief list of img format strings always supported by OpenCV. Must be terminated 
    /// with an empty string. 
    ///
    static const string OPENCV_SUPPORTED_LIST[] = { "jpeg", "jpg" , "jpe", "jp2", "bmp", "png", "pbm",
                                                    "tiff", "tif", "dib", "pbm", "pgm", "ppm", "ras", "sr", "" };

    ///
    /// @brief Levels of compression to use during JPEG encoding.
    ///
    enum Srl_img_compression_level
    {
        SRL_COMPRESSION_LOW ,
        SRL_COMPRESSION_DEFAULT ,
        SRL_COMRPESSION_HIGH
    };


    ///
    /// @brief indicates whether the image format is supported by the OpenCV library
    ///
    /// @param[in]   format    string value indicating the type of image
    ///
    bool is_format_CV_supported( string format );

    ///
    /// @brief indicates whether the image format is supported by the Magick++ library
    ///
    /// @param[in]   format    string value indicating the type of image
    ///
    bool is_format_magick_supported( string format );

    ///
    /// @brief indicates whether the image format is supported by the SDK 
    ///
    /// @param[in]   format    string value indicating the type of image
    ///
    bool is_format_supported( string format );

    ///
    ///	@brief	holds a singular pair of img format to string values
    ///
    typedef std::pair<Srl_img_format_enum , std::string > Srl_img_format_pair;

    ///
    ///	@brief	Typedef'ed map of enum to string to hold our image formats and facilitate the interface
    ///
    typedef std::map<Srl_img_format_enum , std::string> Srl_img_format_map;

    ///
    /// @brief	a static map to hold the values for the supported formats
    ///
    static Srl_img_format_map img_format_mappings = {
        {SRL_IMG_FORMAT_JPEG_CVIM, "jpeg"},{SRL_IMG_FORMAT_JPEG_CVIM, "jpg"},
        //{},{},{},{},{},
        //{},{},{},{},{},{},{},
    };



    enum Srl_img_format_enum
    {
        SRL_IMG_FORMAT_JPEG_CVIM
    };


} // srl


#endif //_CML_DATA_TYPES_HPP
