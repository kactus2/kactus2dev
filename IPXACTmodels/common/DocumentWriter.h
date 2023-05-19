//-----------------------------------------------------------------------------
// File: DocumentWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.08.2015
//
// Description:
// Base class for IP-XACT document writers.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTWRITER_H
#define DOCUMENTWRITER_H

#include "CommonItemsWriter.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

class Document;

//-----------------------------------------------------------------------------
//! Base class for IP-XACT document writers.
//-----------------------------------------------------------------------------
class DocumentWriter : public CommonItemsWriter
{
public:

    //! The constructor.
    DocumentWriter();

	//! The destructor.
	~DocumentWriter() override = default;

    /*!
     *  Writes the comments preceding the document into XML.
     *
     *      @param [in] writer      The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    static void writeTopComments(QXmlStreamWriter& writer, QSharedPointer<Document> document);
    
    /*!
     *  Writes the xml processing instructions for IP-XACT 2014 documents.
     *
     *      @param [in] writer      The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    static void writeXmlProcessingInstructions(QXmlStreamWriter& writer, QSharedPointer<Document> document);

    /*!
     *  Writes the namespace declarations for IP-XACT 2014 documents.
     *
     *      @param [in] writer   The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    static void writeNamespaceDeclarations(QXmlStreamWriter& writer, QSharedPointer<Document> document);

    static void writeDocumentNameGroup(QXmlStreamWriter& writer, QSharedPointer<Document> document);

     /*!
     *  Writes the parameters of a given document into XML.
     *
     *      @param [in] writer       The used XML writer.
     *      @param [in] document     The document whose parameters to write.
     */
    static void writeParameters(QXmlStreamWriter& writer, QSharedPointer<Document> document);

    /*!
     *  Writes the assertions of a given document into XML.
     *
     *      @param [in] writer       The used XML writer.
     *      @param [in] document     The document whose assertions to write.
     */
    static void writeAssertions(QXmlStreamWriter& writer, QSharedPointer<Document> document);

private:

	// Disable copying.
	DocumentWriter(DocumentWriter const& rhs);
	DocumentWriter& operator=(DocumentWriter const& rhs);

};

#endif // DOCUMENTWRITER_H