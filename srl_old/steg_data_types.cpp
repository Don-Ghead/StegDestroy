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

#include "steg_data_types.hpp"

#include <algorithm>

using namespace cv;
using namespace std;


///
/// @brief The srl namespace contains all of the StegDestroy buildings blocks, functionality
/// and documentation needed to implement steganography removal. Nested within CML 
///
namespace srl
{
    bool is_format_supported( string format )
    {
        if ( !format.empty() )
        {
            string lower_format = format;
            std::transform( lower_format.begin() , lower_format.end() , lower_format.begin() , ::tolower );

            if ( is_format_CV_supported( format ) || is_format_magick_supported( format ) )
            {   //if supported by CV drop out earlier to avoid unnecessary processing
                return true;
            }
            else
            {
                return false;
            }

        }
        // format string empty TODO
    }


    bool is_format_CV_supported( string format )
    {
        bool supported = false;
        if ( !format.empty() )
        {
            // Lower the string for performance
            string lower_format = format;
            std::transform( lower_format.begin() , lower_format.end() , lower_format.begin() , ::tolower );

            unsigned int index = 0;

            while ( !supported && !OPENCV_SUPPORTED_LIST[index].empty() )
            {
                if ( lower_format.compare( OPENCV_SUPPORTED_LIST[index] ) )
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
    *			Srl Exception Classes + data types implementation
    *
    *************************************************************************/

    ///
    /// @brief destructors 
    ///
    Srl_exception::Srl_exception( std::unique_ptr<cv::Exception> exception )
        : m_im_except_ap( nullptr )
    {
        m_cv_except_ap = std::move( exception );
    }
    Srl_exception::Srl_exception( std::unique_ptr<Magick::Exception> exception )
        : m_cv_except_ap( nullptr )
    {
        m_im_except_ap = std::move( exception );
    }

    ///
    /// @brief returns true if the exception is from OpenCV else it's Magick++
    ///
    bool Srl_exception::is_cv_exception( void )
    {
        if ( nullptr != m_cv_except_ap )
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
    bool Srl_exception::get_basic_except_info( string& except_str )
    {
        if ( !except_str.empty() )
        {
            if ( is_cv_exception() )
            {
                except_str = m_cv_except_ap->msg;
            }
            else
            {
                except_str = m_im_except_ap->what();
            }
        }
        else
        {
            return false;
        }
    }
} // srl
