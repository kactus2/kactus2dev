//-----------------------------------------------------------------------------
// File: FileBuilderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Writer class for fileBuilder element.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERWRITER_H
#define FILEBUILDERWRITER_H

#include "CommonItemsWriter.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>

class FileBuilder;

//-----------------------------------------------------------------------------
//! Writer class for fileBuilder element.
//-----------------------------------------------------------------------------
class FileBuilderWriter : public CommonItemsWriter
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent of this writer.
     */
    FileBuilderWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileBuilderWriter();

    /*!
     *  Write the default file builder.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] nameGroup   The selected file builder.
     */
    void writeDefaultFileBuilder(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder) const;

private:

    // Disable copying.
    FileBuilderWriter(FileBuilderWriter const& rhs);
    FileBuilderWriter& operator=(FileBuilderWriter const& rhs);

    /*!
     *  Write the file type.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] fileBuilder     The selected file builder.
     */
    void writeFileType(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder) const;

    /*!
     *  Write the build model of the file builder.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] fileBuilder     The selected file builder.
     */
    void writeBuildModel(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder) const;
};

#endif // FILEBUILDERWRITER_H