//-----------------------------------------------------------------------------
// File: DocumentReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.08.2015
//
// Description:
// Base class for XML readers.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTREADER_H
#define DOCUMENTREADER_H

#include "Document.h"

#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Base class for XML readers.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DocumentReader : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent object.
     */
    DocumentReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~DocumentReader();

protected:

    /*!
     *  Parses the comments preceding the document from XML.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document to insert the parsed comments.
     */
    void parseTopComments(QDomNode const& documentNode, QSharedPointer<Document> document) const;

    /*!
     *  Parses the VLNV from XML to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed VLNV is inserted.
     *      @param [in] type            The type of the VLNV.
     */
    void parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document, VLNV::IPXactType type) const;

    VLNV parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType) const;

    /*!
     *  Parses the description from XML to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed description is inserted.
     */
    void parseDescription(QDomNode const& documentNode, QSharedPointer<Document> document) const;

    /*!
     *  Parses the parameters from XLM to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed parameters are inserted.
     */
    void parseParameters(QDomNode const& documentNode, QSharedPointer<Document> document) const;

    /*!
     *  Parses the assertions from XML to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed assertions are inserted.
     */
    void parseAssertions(QDomNode const& documentNode, QSharedPointer<Document> document) const;

    /*!
     *  Parses the vendor extensions from XML to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed vendor extensions are stored.
     */
    void parseVendorExtensions(QDomNode const& documentNode, QSharedPointer<Extendable> document) const;

    /*!
     *  Parses a configurable VLNV.
     *
     *      @param [in] configurableVLNVNode    The XML description of the configurable VLNV.
     *      @param [in] type                    The type of the configurable VLNV.
     */
    QSharedPointer<ConfigurableVLNVReference> parseConfigurableVLNVReference(
        QDomNode const& configurableVLNVNode, VLNV::IPXactType type) const;

    /*!
     *  Parses a single configurable element value.
     *
     *      @param [in] configurableElementNode     The XML description of the configurable element value.
     */
    QSharedPointer<ConfigurableElementValue> parseConfigurableElementValue(
        QDomNode const& configurableElementNode) const;

private:

	// Disable copying.
    DocumentReader(DocumentReader const& rhs);
    DocumentReader& operator=(DocumentReader const& rhs);

};

#endif // DOCUMENTREADER_H