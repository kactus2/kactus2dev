//-----------------------------------------------------------------------------
// File: DocumentReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Base class for XML readers.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DocumentReader : public CommonItemsReader
{    
public:

    /*!
     *  The constructor.
     */
    DocumentReader();

    /*!
     *  The destructor.
     */
    virtual ~DocumentReader();

    /*!
     *  Parses the document revision from XML.
     *
     *    @param [in] document    The XML description of the document.
     */
    static Document::Revision getXMLDocumentRevision(QDomNode const& document);

    /*!
     *  Parses the document name group data.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document to insert the parsed data.
     */
    static void parseDocumentNameGroup(QDomNode const& documentNode, QSharedPointer<Document> document);

    /*!
     *  Parses the comments preceding the document from XML.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document to insert the parsed comments.
     */
    static void parseTopComments(QDomNode const& documentNode, QSharedPointer<Document> document);

    static void parseXMLProcessingInstructions(QDomNode const& documentNode, QSharedPointer<Document> document);

    static void parseNamespaceDeclarations(QDomNode const& documentNode, QSharedPointer<Document> document);

    /*!
     *  Parses the VLNV from XML to a document.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document in which the parsed VLNV is inserted.
     *    @param [in] type            The type of the VLNV.
     */
    static void parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document, VLNV::IPXactType type);

    /*!
     *  Parses the description from XML to a document.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document in which the parsed description is inserted.
     */
    static void parseDescription(QDomNode const& documentNode, QSharedPointer<Document> document);

    /*!
     *  Parses the parameters from XLM to a document.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document in which the parsed parameters are inserted.
     */
    static void parseParameters(QDomNode const& documentNode, QSharedPointer<Document> document);

    /*!
     *  Parses the assertions from XML to a document.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document in which the parsed assertions are inserted.
     */
    static void parseAssertions(QDomNode const& documentNode, QSharedPointer<Document> document);
    
    /*!
     *  Parses Kactus2 specific vendor extensions before other vendor extensions from XML.
     *
     *    @param [in] documentNode    The XML description of the document.
     *    @param [in] document        The document in which the parsed extensions are stored.
     */
     static void parseKactusAndVendorExtensions(QDomNode const& documentNode,
        QSharedPointer<Document> document);

    /*!
     *  Parses Kactus2 attributes for classifying IP from XML.
     *
     *    @param [in] attributesNode      The XML description of the attributes.
     *    @param [in] document            The document in which the parsed attributes are stored.
     */
     static void parseKactusAttributes(QDomNode const& attributesNode, QSharedPointer<Document> document);

    /*!
     *  Parses document tags from XML.
     *
     *    @param [in] tagsGroupNode   XML description of the tags.
     *    @param [in] document        The containing component.
     */
     static void parseTags(QDomNode const& tagsGroupNode, QSharedPointer<Document> document);

private:

	// Disable copying.
    DocumentReader(DocumentReader const& rhs);
    DocumentReader& operator=(DocumentReader const& rhs);

};

#endif // DOCUMENTREADER_H