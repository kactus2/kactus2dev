//-----------------------------------------------------------------------------
// File: VersionHelper.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 31.8.2016
//
// Description:
// A module that returns variations version number.
//-----------------------------------------------------------------------------

#ifndef VERSIONHELPER_H
#define VERSIONHELPER_H

#include "QString"

namespace VersionHelper
{
    /*!
     *  Returns a formatted full version info including the bit information of the build.
     */
    QString createVersionString();

    /*!
     *  Returns full version info, separated by dots.
     */
    QString versionFull();

    /*!
    *  Returns full version info, separated by commas.
     */
    QString versionFileStr();
}

#endif // VERSIONHELPER_H