//-----------------------------------------------------------------------------
// File: FileWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Writer class for IP-XACT file element.
//-----------------------------------------------------------------------------

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "File.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT file element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    FileWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileWriter();

    /*!
     *  Write a file to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeFile(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

private:

    //! No copying allowed.
    FileWriter(FileWriter const& rhs);
    FileWriter& operator=(FileWriter const& rhs);

    /*!
     *  Write the file types.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeFileTypes(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the file attributes.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the structural status of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeIsStructural(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the include file status of the value.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeIsIncludeFile(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the logical name of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeLogicalName(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the exported names of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeExportedNames(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the build command of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeBuildCommand(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the dependencies of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeDependencies(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the defines of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeDefines(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the image types of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeImageTypes(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the description of the file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeDescription(QXmlStreamWriter& writer, QSharedPointer<File> file) const;

    /*!
     *  Write the file extensions.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file to be written.
     */
    void writeFileExtensions(QXmlStreamWriter& writer, QSharedPointer<File> file) const;
};

#endif // FILEWRITER_H