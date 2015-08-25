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

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Document;
class Extendable;
class VendorExtension;
class VLNV;

//-----------------------------------------------------------------------------
//! Base class for IP-XACT document writers.
//-----------------------------------------------------------------------------
class DocumentWriter : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    DocumentWriter(QObject* parent = 0);

	//! The destructor.
	~DocumentWriter();

protected:

    /*!
     *  Writes the comments preceding the document into XML.
     *
     *      @param [in] writer      The writer to use.
     *      @param [in] document    The document whose comments to write.
     */
    void writeTopComments(QXmlStreamWriter& writer, QSharedPointer<Document> document) const;

    /*!
     *  Writes the namespace declarations for IP-XACT 2014 documents.
     *
     *      @param [in] writer   The writer to use.
     */
    void writeNamespaceDeclarations(QXmlStreamWriter& writer) const;

    /*!
     *  Writes the given VLNV as separate elements into XML.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] VLNV        The VLNV to write.
     */
    void writeVLNVElements(QXmlStreamWriter& writer, VLNV const& VLNV) const;
    
    /*!
     *  Writes the given VLNV as attributes for the current element into XML.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] VLNV        The VLNV to write.
     */
    void writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv) const;
    
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


    /*!
     *  Writes the vendor extensions of a given element into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] element     The element whose vendor extensions to write.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer,  QSharedPointer<Extendable> element) const;

    /*!
     *  Write the isPresent element.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] isPresent   The value for the isPresent element.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent) const;

private:

	// Disable copying.
	DocumentWriter(DocumentWriter const& rhs);
	DocumentWriter& operator=(DocumentWriter const& rhs);

};

#endif // DOCUMENTWRITER_H