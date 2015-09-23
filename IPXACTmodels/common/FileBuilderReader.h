//-----------------------------------------------------------------------------
// File: FileBuilderReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Reader class for file builder element.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERREADER_H
#define FILEBUILDERREADER_H

#include "FileBuilder.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for file builder element.
//-----------------------------------------------------------------------------
class FileBuilderReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent of this reader.
     */
    FileBuilderReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileBuilderReader();

    /*!
     *  Reads the default file builder.
     *
     *      @param [in] fileBuilderNode     XML description of the default file builder.
     *
     *      @return The created default file builder.
     */
    QSharedPointer<FileBuilder> createDefaultFileBuilderFrom(QDomNode const& fileBuilderNode) const;

private:

    // Disable copying.
    FileBuilderReader(FileBuilderReader const& rhs);
    FileBuilderReader& operator=(FileBuilderReader const& rhs);

    /*!
     *  Reads the file builder file type.
     *
     *      @param [in] filebuilderNode     XML description of the file builder.
     *      @param [in] newFileBuilder      The selected file builder.
     */
    void parseFileType(QDomNode const& filebuilderNode, QSharedPointer<FileBuilder> newFileBuilder) const;

    /*!
     *  Reads the file builder command, flags and replace default flags value.
     *
     *      @param [in] filebuilderNode     XML description of the file builder.
     *      @param [in] newFileBuilder      The selected file builder.
     */
    void parseBuildModel(QDomNode const& fileBuilderNode, QSharedPointer<FileBuilder> newFileBuilder) const;
};

#endif // FILEBUILDERREADER_H