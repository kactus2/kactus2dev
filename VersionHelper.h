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

#ifndef VersionHelper_H
#define VersionHelper_H

#include "QString"

    /*!
     *  Returns a formatted full version info including the bitness of the build.
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

#endif // VersionHelper_H