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

#ifndef _SRL_EXPORT_H
#define _SRL_EXPORT_H

#include <pil/types.h>

/**
 MACRO Definition
 **/

#ifndef _CTL_STATIC
#ifdef _SRL_IMPL
#define SRL_EXPORT PIL_EXPORT
#else
#define SRL_EXPORT PIL_IMPORT
#endif // _ICL_IMPL
#else
#define SRL_EXPORT
#endif // _ICL_IMPL

#endif // _CTL_STATIC
