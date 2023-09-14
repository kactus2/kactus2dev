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
 // Function: Utils::ValidityOptions::ImplementationOptions()
 //-----------------------------------------------------------------------------
Utils::ImplementationOptions::ImplementationOptions():
hw_(true),
sw_(true),
system_(true) {
}

//-----------------------------------------------------------------------------
// Function: Utils::ValidityOptions::HierarchyOptions()
//-----------------------------------------------------------------------------
Utils::HierarchyOptions::HierarchyOptions():
flat_(true),
product_(true),
board_(true),
chip_(true),
soc_(true),
ip_(true) {
}

//-----------------------------------------------------------------------------
// Function: Utils::ValidityOptions::FirmnessOptions()
//-----------------------------------------------------------------------------
Utils::FirmnessOptions::FirmnessOptions():
templates_(true),
mutable_(true),
fixed_(true)
{
}

//-----------------------------------------------------------------------------
// Function: Utils::ValidityOptions::TypeOptions()
//-----------------------------------------------------------------------------
Utils::TypeOptions::TypeOptions():
components_(true),
buses_(true),
catalogs_(true),
apis_(false),
advanced_(false)
{
}

//-----------------------------------------------------------------------------
// Function: Utils::ValidityOptions::ValidityOptions()
//-----------------------------------------------------------------------------
Utils::ValidityOptions::ValidityOptions() :
    valid_(true),
    invalid_(true)
{
}

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
