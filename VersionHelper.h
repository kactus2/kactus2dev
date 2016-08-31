//-----------------------------------------------------------------------------
// File: VersionHelper.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 31.08.2016
//
// Description:
// A module that returns variations version number.
//-----------------------------------------------------------------------------

#ifndef VersionHelper_H
#define VersionHelper_H

#include "QString"

    /*!
     *  Returns a formatted full version along with bitness of the compilation.
     */
    QString createVersionString();

    /*!
     *  Returns full version, separated by dots.
     */
    QString versionFull();

    /*!
    *  Returns full version, separated by commas.
     */
    QString versionFileStr();

#endif // VersionHelper_H