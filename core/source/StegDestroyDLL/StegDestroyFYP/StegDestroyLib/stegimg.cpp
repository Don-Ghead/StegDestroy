// $Id$
//------------------------------------------------------------------------------------
///
/// @file   stegimg.cpp
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

#include "stegimg.hpp"
using namespace srl;
using namespace Magick;
using namespace cv;

Srl_steg_image::Srl_steg_image( unsigned char * data_p , size_t data_length , Srl_img_format_pair img_format )
    :   m_img_ap(nullptr),
        m_mat_ap(nullptr),
        m_format(img_format),
        m_exception_ap(nullptr),
        m_err_status(SRL_EXCEPT_NONE)
{
    bool success = false;
    string err_status;

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
                m_img_ap->read( temp_blob );
            }
            catch ( ErrorBlob * e )
            {
                //TODO Insert try catch actions;
            }
            catch ( ErrorMissingDelegate & e )
            {
                err_status = e.what();
            }
            catch ( ErrorDelegate & e )
            {
                err_status = e.what();
            }
            catch ( ErrorImage & e )
            {
                err_status = e.what();
            }
            // If we got here then we have successfully read the image data into the image member
            success = true;
        }
        catch ( Magick::Exception & e )
        {
            err_status = e.what();
        }
    }
    else if( !success ) 
    {
        //we haven't been able to read the image using Magick++ so try with OpenCV
        //Try to Decode the buffer to a matrix 
        try
        {
            imdecode( std::vector<char>( *data_p ) , 10 , m_mat_ap.get() );
        }
        catch ( cv::Exception & e )
        {
            //TODO 
        }

        //Make sure that the data is there 
        if ( nullptr == m_mat_ap->data )
        {  //this value will be checked for before any encoding is done 
            m_err_status = SRL_EXCEPT_READ;
        }
    }
        
}

Srl_steg_image::Srl_steg_image( const IplImage * data_p , Srl_img_format_pair img_format)
    :   m_img_ap( nullptr ) ,
        m_mat_ap( nullptr ) ,
        m_format( img_format ) ,
        m_exception_ap( nullptr ) ,
        m_err_status( SRL_EXCEPT_NONE )
{
    if ( is_format_CV_supported( img_format.second ) )
    {
        try
        {
            // try to read in out data to the CV matrix
            *m_mat_ap = cv::cvarrToMat( data_p );
        }
        catch ( cv::Exception &e )
        {
            // TODO
        }
    }
    // if that wasn't successful for whatever reason we still can try to read it into a Blob
    if( nullptr == m_mat_ap->data )
    {
        try
        {
            // Construct a Blob instance first so that we don't have to worry
            // about object construction failure due to a minor warning exception
            // being thrown.
            Blob temp_blob;
            try
            {
                temp_blob.update( data_p->imageData , data_p->imageSize );
                // this is where we try to read our data into the blob and the image so there is potential 
                // for a fair few things going wrong hence the number of errors to catch
                m_img_ap->read( temp_blob );
            }
            catch(Magick::Exception &e)
            {
                //For now we'll just carpet catch all exceptions for ease 
            }
                        
        }
        catch ( Magick::Exception &e )
        {

        }
    }
    // TODO - Not sure if there's anyway to verify we've read it at this point, presumably
    // if it hasn't thrown then it was successful?
}

Srl_steg_image::Srl_steg_image( const Srl_steg_image & img_copy_r )
{
    m_err_status = SRL_EXCEPT_NONE;
    //First copy the image data 
    if ( nullptr != img_copy_r.m_img_ap.get() )
    {
        m_img_ap = img_copy_r.m_img_ap;
    }
    else if ( nullptr != img_copy_r.m_mat_ap.get() )
    {
        m_mat_ap = img_copy_r.m_mat_ap;
    }
    //now the format + check the error status to see if we need to get the exception
    m_format = img_copy_r.m_format;
    if ( SRL_EXCEPT_NONE != img_copy_r.m_err_status )
    {
        m_err_status = img_copy_r.m_err_status;
        if ( nullptr != img_copy_r.exception() )
        {
             //m_exception_ap.get() = img_copy_r.exception(); //TODO - Figure out how to copy the exception if there is one 
        }
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
Srl_exception const * Srl_steg_image::exception( void ) const
{
    return m_exception_ap.get();
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
    if ( nullptr != m_img_ap.get() )
    {
        Blob blob;
        m_img_ap->write( &blob );
        return blob.data();
    }
    else 
    {   //This will either return the data or nullptr 
        return m_mat_ap->data;
    }
}

bool Srl_steg_image::encode( Srl_img_format_enum img_format_in )
{
    return false;
}
