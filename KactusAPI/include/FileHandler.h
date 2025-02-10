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
#include <QRegularExpression>
#include <QString>

//-----------------------------------------------------------------------------
//! Handles data regarding files.
//-----------------------------------------------------------------------------
namespace FileHandler
{
    //! Regular expression to validate URLs.
    const QRegularExpression URL_VALIDITY_REG_EXP = QRegularExpression(
        "^[a-z]{3,9}[:][/]{2}[a-z0-9]+([-.][a-z0-9]+)*([/][-a-z0-9_.;,?=&%#~+]*)*$",
        QRegularExpression::CaseInsensitiveOption);

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

    /*!
     *  Resolve environment variables in a given URI to form a file path.
     *
     *      @param [in] ipxactURI
     *
     *      @return The resolved file path.
     */
    KACTUS2_API QString resolvePath(QString const& ipxactURI);

    /*!
     *  Resolve environment variables in a given URI to form a URI.
     *
     *      @param [in] ipxactURI
     *
     *      @return The resolved URI.
     */
    KACTUS2_API QString resolveURI(QString const& ipxactURI);

    /*!
     *  Check if given ipxactURI is valid.
     *
     *      @param [in] relativeTo  Path to resolve relative file paths from.
     *      @param [in] ipxactURI   The URI to check.
     *
     *      @return True, if the ipxactURI is valid, otherwise false.
     */
    KACTUS2_API bool isValidURI(QString const& relativeTo, QString const& ipxactURI);

    /*!
     *  Check if given file path exists in the file system.
     *
     *      @param [in] relativeTo  Path to resolve relative file paths from.
     *      @param [in] filePath    The path to check.
     *
     *      @return True, if the path exists, otherwise false.
     */
    KACTUS2_API bool filePathExists(QString const& relativeTo, QString const& filePath);

    /*!
     *  Check if given input is a URI.
     *
     *      @param [in] uri The input to check.
     *
     *      @return True, if the input is a URI, otherwise false.
     */
    KACTUS2_API bool isURI(QString const& uri);
};

#endif // FILEHANDLER_H

