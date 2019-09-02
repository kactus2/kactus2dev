//-----------------------------------------------------------------------------
// File: TagData.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.08.2019
//
// Description:
// Struct for tag information.
//-----------------------------------------------------------------------------

#ifndef TAGDATA_H
#define TAGDATA_H

#include <QString>

//-----------------------------------------------------------------------------
//! Struct for tag information.
//-----------------------------------------------------------------------------
struct TagData
{
    //! Tag name.
    QString name_;

    //! Tag color in hex code ("#RRGGBB").
    QString color_;
};

#endif // TAGDATA_H
