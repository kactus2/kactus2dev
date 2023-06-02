//-----------------------------------------------------------------------------
// File: DocumentReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.08.2015
//
// Description:
// Base class for XML readers.
//-----------------------------------------------------------------------------

#include "DocumentReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/TagData.h>

//-----------------------------------------------------------------------------
// Function: DocumentReader::DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::DocumentReader() : CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::~DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::~DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::getXMLDocumentRevision()
//-----------------------------------------------------------------------------
Document::Revision DocumentReader::getXMLDocumentRevision(QDomNode const& document)
{
    QDomNamedNodeMap attributeMap = document.attributes();

    auto ipxactSchemaAttribute = attributeMap.namedItem(QStringLiteral("xmlns:ipxact"));
    QString schemaURI = ipxactSchemaAttribute.nodeValue();

    if (schemaURI.contains(QStringLiteral("1685-2014")))
    {
        return Document::Revision::Std14;
    }
    else if (schemaURI.contains(QStringLiteral("1685-2022")))
    {
        return Document::Revision::Std22;
    }
    else
    {
        return Document::Revision::Unknown;
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseDocumentNameGroup()
//-----------------------------------------------------------------------------
void DocumentReader::parseDocumentNameGroup(QDomNode const& documentNode, QSharedPointer<Document> document)
{
    if (document->getRevision() == Document::Revision::Std22)
    {
        document->setDisplayName(
            documentNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
        document->setShortDescription(
            documentNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
    }

    document->setDescription(
        documentNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseTopComments()
//-----------------------------------------------------------------------------
void DocumentReader::parseTopComments(QDomNode const& documentNode, QSharedPointer<Document> document)
{
    QStringList comments;

    QDomNodeList nodeList = documentNode.childNodes();
    QDomNode singleDocumentNode = documentNode.firstChildElement();

    for (int i = 0; i < nodeList.size() && nodeList.at(i) != singleDocumentNode; ++i)
    {
        if (nodeList.at(i).isComment())
        {
            comments.append(nodeList.at(i).nodeValue());
        }
    }

    document->setTopComments(comments);
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseXMLProcessingInstructions()
//-----------------------------------------------------------------------------
void DocumentReader::parseXMLProcessingInstructions(QDomNode const& documentNode, 
    QSharedPointer<Document> document)
{
    QDomNodeList nodeList = documentNode.childNodes();
    QDomNode singleDocumentNode = documentNode.firstChildElement();

    for (int i = 0; i < nodeList.size() && nodeList.at(i) != singleDocumentNode; i++)
    {
        if (nodeList.at(i).isProcessingInstruction())
        {
            QDomProcessingInstruction instruction = nodeList.at(i).toProcessingInstruction();
            if (!instruction.data().startsWith(QStringLiteral("version")))
            {
                document->addXmlProcessingInstructions(instruction.target(), instruction.data());
            }            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::parseNamespaceDeclarations()
//-----------------------------------------------------------------------------
void DocumentReader::parseNamespaceDeclarations(QDomNode const& documentNode, 
    QSharedPointer<Document> document)
{
    QDomNamedNodeMap attributeMap = documentNode.attributes();
    QString nameSpaceIdentifier = QStringLiteral("xmlns:");

    for (int i = 0; i < attributeMap.count(); ++i)
    {
        QDomNode attr = attributeMap.item(i);

        QString name = attr.nodeName();

        // Must be xmlns.
        if (name.startsWith(nameSpaceIdentifier))
        {
            document->addXmlNameSpace(name.remove(nameSpaceIdentifier), attr.nodeValue());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseVLNV()
//-----------------------------------------------------------------------------
void DocumentReader::parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document,
    VLNV::IPXactType type)
{
    document->setVlnv(createVLNVFrom(documentNode, type));
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseDescription()
//-----------------------------------------------------------------------------
void DocumentReader::parseDescription(QDomNode const& documentNode, QSharedPointer<Document> document)
{
    document->setDescription(documentNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseParameters()
//-----------------------------------------------------------------------------
void DocumentReader::parseParameters(QDomNode const& documentNode, QSharedPointer<Document> document)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(
        documentNode, document->getRevision());

    for (auto parameter : *newParameters)
    {
        document->getParameters()->append(parameter);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseAssertions()
//-----------------------------------------------------------------------------
void DocumentReader::parseAssertions(QDomNode const& documentNode, QSharedPointer<Document> document)
{
    QDomNodeList assertionNodeList = documentNode.firstChildElement(QStringLiteral("ipxact:assertions")).childNodes();

    if (!assertionNodeList.isEmpty())
    {
        int assertionCount = assertionNodeList.count();
        for (int i = 0; i < assertionCount; ++i)
        {
            QDomNode assertionNode = assertionNodeList.at(i);

            QSharedPointer<Assertion> newAssertion (new Assertion());
            newAssertion->setName(assertionNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());
            newAssertion->setDisplayName(assertionNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().
                nodeValue());
            newAssertion->setDescription(assertionNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().
                nodeValue());
            newAssertion->setAssert(assertionNode.firstChildElement(QStringLiteral("ipxact:assert")).firstChild().nodeValue());

            document->getAssertions()->append(newAssertion);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseKactusAndVendorExtensions()
//-----------------------------------------------------------------------------
void DocumentReader::parseKactusAndVendorExtensions(QDomNode const& documentNode, 
    QSharedPointer<Document> document)
{
    QDomElement extensionNodes = documentNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomNode versionNode = extensionNodes.firstChildElement(QStringLiteral("kactus2:version"));
    if (!versionNode.isNull())
    {
        document->setVersion(versionNode.firstChild().nodeValue());
    }

    QDomNode attributesNode = extensionNodes.firstChildElement(QStringLiteral("kactus2:kts_attributes"));
    if (!attributesNode.isNull())
    {
        parseKactusAttributes(attributesNode, document);
    }

    QDomNode tagsNode = extensionNodes.firstChildElement(QLatin1String("kactus2:tags"));
    if (!tagsNode.isNull())
    {
        parseTags(tagsNode, document);
    }

    QDomNode licenseNode = extensionNodes.firstChildElement(QStringLiteral("kactus2:license"));
    if (!licenseNode.isNull())
    {
        document->setLicense(licenseNode.firstChild().nodeValue());
    }

    parseVendorExtensions(documentNode, document);
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseKactusAttributes()
//-----------------------------------------------------------------------------
void DocumentReader::parseKactusAttributes(QDomNode const& attributesNode, QSharedPointer<Document> document)
{
    QDomNode hierarchyNode = attributesNode.firstChildElement(QStringLiteral("kactus2:kts_productHier"));
    if (!hierarchyNode.isNull())
    {
        KactusAttribute::ProductHierarchy hierarchy = 
            KactusAttribute::hierarchyFrom(hierarchyNode.firstChild().nodeValue());
        document->setHierarchy(hierarchy);
    }

    QDomNode implementationNode = attributesNode.firstChildElement(QStringLiteral("kactus2:kts_implementation"));
    if (!implementationNode.isNull())
    {
        KactusAttribute::Implementation implementation = 
            KactusAttribute::implementationFrom(implementationNode.firstChild().nodeValue());
        document->setImplementation(implementation);
    }

    QDomNode firmnessNode = attributesNode.firstChildElement(QStringLiteral("kactus2:kts_firmness"));
    if (!firmnessNode.isNull())
    {
        KactusAttribute::Firmness firmness = KactusAttribute::firmnessFrom(firmnessNode.firstChild().nodeValue());
        document->setFirmness(firmness);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseTags()
//-----------------------------------------------------------------------------
void DocumentReader::parseTags(QDomNode const& tagsGroupNode, QSharedPointer<Document> document)
{
    QVector<TagData> documentTags;

    QDomNodeList tagNodeList = tagsGroupNode.childNodes();
    for (int i = 0; i < tagNodeList.size(); ++i)
    {
        QDomNode tagNode = tagNodeList.at(i);
        QDomElement nameElement = tagNode.firstChildElement(QLatin1String("kactus2:name"));
        QDomElement colorElement = tagNode.firstChildElement(QLatin1String("kactus2:color"));

        if (!nameElement.isNull() && !colorElement.isNull())
        {
            TagData newTag({ nameElement.text(), colorElement.text() });

            documentTags.append(newTag);
        }
    }

    if (!documentTags.isEmpty())
    {
        document->setTags(documentTags);
    }
}
