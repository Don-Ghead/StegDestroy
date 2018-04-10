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

#include "stdafx.h"
#include "Srl_steg_data_types.hpp"

#include <algorithm>

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
	*					Srl General Data Type Utility
	*
	*************************************************************************/

	///
	/// @brief	get image format pair, check for invalid return
	///
	Srl_img_format_pair get_format_pair(string format)
	{
		Srl_img_format_map::const_iterator iter = img_format_mappings.find(format);
		if (!(img_format_mappings.end() == iter) )
		{	
			return iter->second;
		}
		else
		{
			return INVALID_IMG_FORMAT_PAIR;
		}
	}

	///
	/// @brief	Wrapper for image format support functions
	///
	/// @description	CHecks to see if the provided format is supported by either OCV or IM
	///
	/// @param[in]	format	string format
	///
	/// @return	bool	Returns true if format supported by one of them & no otherwise
    bool is_format_supported( string format )
    {
		bool success = false;
        if ( !format.empty() )
        {
            string lowercase_format = format;
            std::transform( lowercase_format.begin() , lowercase_format.end() , lowercase_format.begin() , ::tolower );

            if ( is_format_CV_supported( format ) || is_format_magick_supported( format ) )
            {   //if supported by CV drop out earlier to avoid unnecessary processing
                success = true;
            }

        }
        // format string empty TODO
		return false;
    }

    bool is_format_CV_supported( string format )
    {
        bool supported = false;
        if ( !format.empty() )
        {
            // Lower the string for performance
            string lowercase_format = format;
            std::transform( lowercase_format.begin() , lowercase_format.end() , lowercase_format.begin() , ::tolower );

            unsigned int index = 0;

            while ( !supported && !OPENCV_SUPPORTED_LIST[index].empty() )
            {
                if ( lowercase_format.compare( OPENCV_SUPPORTED_LIST[index] ) )
                {
                    supported = true;
                    break;
                }
                index++;
            }
        }
        return supported;
    }

    bool is_format_magick_supported( string format )
    {
        // Lower the string for performance
        bool supported = false;
        if ( !format.empty() )
        {            
            string lower_format = format;
            std::transform( lower_format.begin() , lower_format.end() , lower_format.begin() , ::tolower );

            // the 
            std::list<Magick::CoderInfo> coderList;
            Magick::coderInfoList( &coderList ,
                Magick::CoderInfo::TrueMatch ,   // match Readable formats
                Magick::CoderInfo::TrueMatch ,   // match Writeable formats
                Magick::CoderInfo::AnyMatch     // Multi-frame is not a primary concern
                );                              // only secondary for particular image formats

            std::list<Magick::CoderInfo>::iterator entry = coderList.begin();

            // Iterate through list of supported formats
            while ( !supported && ( entry != coderList.end() ) )
            {
                if ( format.compare( entry->name() ) )
                {   //Matched on format name now check read/write capabilities
                    if ( entry->isReadable() && entry->isWritable() )
                    {
                        supported = true;
                    }
                }
            }           
        }
        return supported;
    }

    /*************************************************************************
    *
    *			Srl Exception Base implementation
    *
    *************************************************************************/

	///
	/// @brief Exception base constructor, assigns error message and line of error  
	///
	/// @param[in]	except_msg	message passed from down from children
	///
	/// @param[in]	err_val		integer value that can represent whatever the implementation designates
	///
	Srl_exception_base::Srl_exception_base(std::string except_msg, int err_val)
		:	m_except_msg(except_msg),
			m_except_err_code(err_val)
	{	
	}

	void Srl_exception_base::get_basic_except_info(std::string &except_string, int &err_val)
	{
		except_string = m_except_msg;
		err_val = m_except_err_code;
	}
	
	/*************************************************************************
	*
	*			Srl JpgScrub Exception implementation
	*
	*************************************************************************/

    ///
    /// @brief CV Exception constructor, assigns error message and line of error  
    ///
	/// @param[in]	exception	cv::Exception to be handled
	///
    Srl_exception::Srl_exception( cv::Exception &exception )
        :	Srl_exception_base(exception.what(), exception.line),
			m_cv_except_p(new cv::Exception(exception)),
			m_im_except_p( nullptr )			
    {
    }

	///
	/// @brief Magick Exception constructor, assigns error message and 0  
	///
	/// @param[in]	exception	cv::Exception to be handled
	///
    Srl_exception::Srl_exception( Magick::Exception &exception )
        :	Srl_exception_base(exception.what(), 0), //Magick++ does not return integer error values
			m_im_except_p(new Magick::Exception(exception)),
			m_cv_except_p(nullptr)
    {
    }

	///
	/// @brief Srl_Exception copy constructor  
	///
	/// @param[in]	exception	cv::Exception to be handled
	///
	Srl_exception::Srl_exception(srl::Srl_exception & exception)
		:	Srl_exception_base(exception.m_except_msg, exception.m_except_err_code),
			m_cv_except_p(nullptr), 
			m_im_except_p(nullptr)
	{
		if (exception.is_cv_exception())
		{
			m_cv_except_p.reset(new cv::Exception(exception.get_cv_except()));
		}
		else
		{
			m_im_except_p.reset(new Magick::Exception(exception.get_magick_except()));
		}
	}

	cv::Exception Srl_exception::get_cv_except(void)
	{
		return *m_cv_except_p;
	}

	Magick::Exception Srl_exception::get_magick_except(void)
	{
		return *m_im_except_p;
	}

    ///
    /// @brief returns true if the exception is from OpenCV else it's Magick++
    ///
    bool Srl_exception::is_cv_exception( void )
    {
        if ( nullptr != m_cv_except_p )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    ///
    /// @brief retrieves the basic exception info in string format
    ///
	/// @param[out]	except_str	string reference to update with basic exception info
    bool Srl_exception::get_basic_except_info( string& except_str )
    {
		bool success = false;
        if ( !except_str.empty() )
        {
            if ( is_cv_exception() )
            {
                except_str = m_cv_except_p->msg;
				success = true;
            }
            else
            {
                except_str = m_im_except_p->what();
				success = true;
            }
        }
        else
        {
            success = false;
        }
		return success;
    }

} // END srl NAMESPACE
