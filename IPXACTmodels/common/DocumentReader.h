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

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Base class for XML readers.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DocumentReader : public CommonItemsReader
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

    void parseXMLProcessingInstructions(QDomNode const& documentNode, QSharedPointer<Document> document) const;

    /*!
     *  Parses the VLNV from XML to a document.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed VLNV is inserted.
     *      @param [in] type            The type of the VLNV.
     */
    void parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document, VLNV::IPXactType type) const;

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
     *  Parses Kactus2 specific vendor extensions before other vendor extensions from XML.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] document        The document in which the parsed extensions are stored.
     */
    virtual void parseKactusAndVendorExtensions(QDomNode const& documentNode,
        QSharedPointer<Document> document) const;

    /*!
     *  Parses Kactus2 attributes for classifying IP from XML.
     *
     *      @param [in] attributesNode      The XML description of the attributes.
     *      @param [in] document            The document in which the parsed attributes are stored.
     */
    void parseKactusAttributes(QDomNode const& attributesNode, QSharedPointer<Document> document) const;

private:

	// Disable copying.
    DocumentReader(DocumentReader const& rhs);
    DocumentReader& operator=(DocumentReader const& rhs);

};

#endif // DOCUMENTREADER_H