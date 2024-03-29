//-----------------------------------------------------------------------------
// File: VersionHelper.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 31.8.2016
//
// Description:
// A module that returns variations of version number as QStrings.
//-----------------------------------------------------------------------------

#include <version.h>
#include <KactusAPI/include/VersionHelper.h>

// Used to convert defined values to string.
// Double stringize is required to expand possible macros in str.
#define DOUBLE_STR(x) #x
#define TOSTRING(str) DOUBLE_STR(str)

//-----------------------------------------------------------------------------
// Function: VersionHelper::createVersionString()
//-----------------------------------------------------------------------------
QString VersionHelper::createVersionString()
{
#if defined (_WIN64) || (__LP64__) || (_LP64)
    int bits = 64;
#else
    int bits = 32;
#endif

    return QStringLiteral("%1.%2.%3 %4-bit").arg(QString::number(VERSION_MAJOR),
        QString::number(VERSION_MINOR),
        QString::number(VERSION_BUILDNO),
        QString::number(bits));
}

//-----------------------------------------------------------------------------
// Function: VersionHelper::versionFull()
//-----------------------------------------------------------------------------
QString VersionHelper::versionFull()
{
    return QStringLiteral(TOSTRING(VERSION_FULL));
}

//-----------------------------------------------------------------------------
// Function: VersionHelper::versionFileStr()
//-----------------------------------------------------------------------------
QString VersionHelper::versionFileStr()
{
    return QStringLiteral(VERSION_FILESTR);
}
