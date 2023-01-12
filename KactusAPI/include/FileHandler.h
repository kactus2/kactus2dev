//-----------------------------------------------------------------------------
// File: FileHandler.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.10.2019
//
// Description:
// Handles data regarding files.
//-----------------------------------------------------------------------------

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "KactusAPI/KactusAPIGlobal.h"

#include <QMultiMap>
#include <QString>

//-----------------------------------------------------------------------------
//! Handles data regarding files.
//-----------------------------------------------------------------------------
namespace FileHandler
{
    /*!
     *  Construct a table for file types.
     *
     *      @return Table with file extension as key and file type as value.
     */
    KACTUS2_API QMultiMap<QString, QString> constructFileSuffixTable();

    /*!
     *  Get a file type for the selected file extension.
     *
     *      @param [in] fileSuffixTable     Table containing file types combined with file name extensions.
     *      @param [in] suffix              The selected extension.
     *
     *      @return File type matching the selected extension.
     */
    KACTUS2_API QString getFileTypeForSuffix(QMultiMap<QString, QString> const& fileSuffixTable, QString const& suffix);
};

#endif // FILEHANDLER_H
