/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 *         filename: utils.cpp
 *         
 *         Description: This file contains definitions for general purpose 
 *         functions that can be used in the whole software.
 */

#include "utils.h"

//-----------------------------------------------------------------------------
// Function: Utils::getCurrentUser()
//-----------------------------------------------------------------------------
QString Utils::getCurrentUser()
{
#ifdef Q_OS_WIN
#pragma warning (disable: 4996)
    return getenv("USERNAME");
#pragma warning (default: 4996)
#else
    return getenv("USER");
#endif
}
