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
	virtual ~DocumentWriter();

protected:

    /*!
     *  Writes the comments preceding the document into XML.
     *
     *      @param [in] writer      The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    void writeTopComments(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;
    
    /*!
     *  Writes the xml processing instructions for IP-XACT 2014 documents.
     *
     *      @param [in] writer      The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    void writeXmlProcessingInstructions(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

    /*!
     *  Writes the namespace declarations for IP-XACT 2014 documents.
     *
     *      @param [in] writer   The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    void writeNamespaceDeclarations(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

    /*!
     *  Writes the document name group data.
     *
     *      @param [in] writer          The writer to use.
     *      @param [in] document        The document whose document name group to write.
     */
    void writeDocumentNameGroup(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

    /*!
     *  Writes the document description into XML.
     *
     *      @param [in] writer       The used XML writer.
     *      @param [in] document     The document whose description to write.
     */
    void writeDescription(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

     /*!
     *  Writes the parameters of a given document into XML.
     *
     *      @param [in] writer       The used XML writer.
     *      @param [in] document     The document whose parameters to write.
     */
    void writeParameters(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

    /*!
     *  Writes the assertions of a given document into XML.
     *
     *      @param [in] writer       The used XML writer.
     *      @param [in] document     The document whose assertions to write.
     */
    void writeAssertions(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

private:

	// Disable copying.
	DocumentWriter(DocumentWriter const& rhs);
	DocumentWriter& operator=(DocumentWriter const& rhs);

};

#endif // DOCUMENTWRITER_H