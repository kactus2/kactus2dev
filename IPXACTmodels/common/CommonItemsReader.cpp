//-----------------------------------------------------------------------------
// File: CommonItemsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#include "CommonItemsReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/common/ChoiceReader.h>

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::createVLNVFrom()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::createVLNVFrom(QDomNode const& vlnvNode, VLNV::IPXactType type)
{
    QString vendor = vlnvNode.firstChildElement(QStringLiteral("ipxact:vendor")).firstChild().nodeValue();
    QString library = vlnvNode.firstChildElement(QStringLiteral("ipxact:library")).firstChild().nodeValue();
    QString name = vlnvNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
    QString version = vlnvNode.firstChildElement(QStringLiteral("ipxact:version")).firstChild().nodeValue();
    
    return VLNV(type, vendor, library, name, version);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVLNVAttributes()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType)
{
    QDomNamedNodeMap attributes = vlnvNode.attributes();

    VLNV attributedVLNV;
    attributedVLNV.setType(vlnvType);
    attributedVLNV.setVendor(attributes.namedItem(QStringLiteral("vendor")).nodeValue());
    attributedVLNV.setLibrary(attributes.namedItem(QStringLiteral("library")).nodeValue());
    attributedVLNV.setName(attributes.namedItem(QStringLiteral("name")).nodeValue());
    attributedVLNV.setVersion(attributes.namedItem(QStringLiteral("version")).nodeValue());

    return attributedVLNV;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAndCreateParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > CommonItemsReader::parseAndCreateParameters(
    QDomNode const& itemNode, Document::Revision revision)
{
    QDomNodeList parameterNodeList = itemNode.firstChildElement(QStringLiteral("ipxact:parameters")).childNodes();
    
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters (new QList<QSharedPointer<Parameter> > ());
    if (!parameterNodeList.isEmpty())
    {
        int parameterCount = parameterNodeList.count();
        for (int parameterIndex = 0;parameterIndex < parameterCount; parameterIndex++)
        {
            newParameters->append(ParameterReader::createParameterFrom(parameterNodeList.at(parameterIndex), revision));
        }
    }
    return newParameters;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsReader::parseVendorExtensions(QDomNode const& itemNode, QSharedPointer<Extendable> element)
{
    QDomNodeList extensionNodes = itemNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions")).childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QDomNode extensionNode = extensionNodes.at(i);

        if (!extensionNode.nodeName().startsWith(QStringLiteral("kactus2:")))
        {
            QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
            element->getVendorExtensions()->append(extension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableVLNVReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> CommonItemsReader::parseConfigurableVLNVReference(
    QDomNode const& configurableVLNVNode, VLNV::IPXactType type)
{
    QDomNamedNodeMap attributeMap = configurableVLNVNode.attributes();

    QString vendor = attributeMap.namedItem(QStringLiteral("vendor")).nodeValue();
    QString library = attributeMap.namedItem(QStringLiteral("library")).nodeValue();
    QString name = attributeMap.namedItem(QStringLiteral("name")).nodeValue();
    QString version = attributeMap.namedItem(QStringLiteral("version")).nodeValue();

    QSharedPointer<ConfigurableVLNVReference> vlnvReference(
        new ConfigurableVLNVReference(type, vendor, library, name, version));

    QDomNode configurableElementsNode = configurableVLNVNode.firstChildElement(QStringLiteral("ipxact:configurableElementValues"));

    QDomNodeList configurableElementNodeList = configurableElementsNode.childNodes();
    for (int i = 0; i < configurableElementNodeList.size(); ++i)
    {
        QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
            parseConfigurableElementValue(configurableElementNodeList.at(i));

        vlnvReference->getConfigurableElementValues()->append(newConfigurableElementValue);
    }

    return vlnvReference;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableElementValue()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> CommonItemsReader::parseConfigurableElementValue(
    QDomNode const& configurableElementNode)
{
    QSharedPointer<ConfigurableElementValue> newConfigurableElementValue (new ConfigurableElementValue());

    newConfigurableElementValue->setConfigurableValue(configurableElementNode.firstChild().nodeValue());

    QDomNamedNodeMap attributeMap = configurableElementNode.attributes();

    for (int i = 0; i < attributeMap.size(); ++i)
    {
        QDomNode attributeItem = attributeMap.item(i);
        newConfigurableElementValue->insertAttribute(
            attributeItem.nodeName(), attributeItem.firstChild().nodeValue());
    }

    return newConfigurableElementValue;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> CommonItemsReader::parseAttributes(QDomNode const& attributedNode)
{
    QMap<QString, QString> attributes;

    QDomNamedNodeMap attributeMap = attributedNode.attributes();
    for (int j = 0; j < attributeMap.size(); ++j)
    {
        QString name = attributeMap.item(j).nodeName();
        QString value = attributeMap.item(j).nodeValue();
        attributes.insert(name, value);
    }

    return attributes;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseIsPresent()
//-----------------------------------------------------------------------------
QString CommonItemsReader::parseIsPresent(QDomElement const& isPresentElement)
{
    if (!isPresentElement.isNull())
    {
        return isPresentElement.firstChild().nodeValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parsePoint()
//-----------------------------------------------------------------------------
QPointF CommonItemsReader::parsePoint(QDomNode const& node)
{
    int x = node.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt();
    int y = node.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt();

    return QPointF(x, y);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > CommonItemsReader::parseChoices(QDomNode const& itemNode)
{
    QDomElement choicesElement = itemNode.firstChildElement(QStringLiteral("ipxact:choices"));
    
    QSharedPointer<QList<QSharedPointer<Choice> > > parsedChoices(new QList<QSharedPointer<Choice> >);

    if (!choicesElement.isNull())
    {
        ChoiceReader choiceReader;

        QDomNodeList choiceNodeList = choicesElement.elementsByTagName(QStringLiteral("ipxact:choice"));
        for (int choiceIndex = 0; choiceIndex < choiceNodeList.count(); ++choiceIndex)
        {
            QDomNode choiceNode = choiceNodeList.at(choiceIndex);
            QSharedPointer<Choice> newChoice = choiceReader.createChoiceFrom(choiceNode);

            parsedChoices->append(newChoice);
        }
    }

    return parsedChoices;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parsePartSelect()
//-----------------------------------------------------------------------------
QSharedPointer<PartSelect> CommonItemsReader::parsePartSelect(QDomNode const& partSelectNode)
{
    QDomNode rangeNode = partSelectNode.firstChildElement(QStringLiteral("ipxact:range"));

    QString leftRange = rangeNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
    QString rightRange = rangeNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

    QSharedPointer<PartSelect> newPartSelect(new PartSelect(leftRange, rightRange));

    QDomElement indicesNode = partSelectNode.firstChildElement(QStringLiteral("ipxact:indices"));

    if (!indicesNode.isNull())
    {
        QDomNodeList indexNodes = indicesNode.elementsByTagName(QStringLiteral("ipxact:index"));
        for (int index = 0; index < indexNodes.size(); ++index)
        {
            QDomNode singleIndexNode = indexNodes.at(index);
            QString indexValue = singleIndexNode.firstChild().nodeValue();

            newPartSelect->getIndices()->append(indexValue);
        }
    }

    return newPartSelect;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseRange()
//-----------------------------------------------------------------------------
Range CommonItemsReader::parseRange(QDomElement const& rangeElement)
{
    return Range(
        rangeElement.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue(),
        rangeElement.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseModeReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > CommonItemsReader::parseModeReferences(QDomNode const& itemNode)
{
    auto modeRefNodes = itemNode.childNodes();

    QSharedPointer<QList<QSharedPointer<ModeReference> > >modeRefList(new QList<QSharedPointer<ModeReference> >());

    for (int i = 0; i < modeRefNodes.size(); ++i)
    {
        if (auto const& modeRefNode = modeRefNodes.at(i);
            modeRefNode.nodeName() == QStringLiteral("ipxact:modeRef"))
        {
            QSharedPointer<ModeReference> newModeRef(new ModeReference());

            newModeRef->setReference(modeRefNode.firstChild().nodeValue());

            if (auto const& priority = modeRefNode.attributes().namedItem(QStringLiteral("priority")).nodeValue();
                !priority.isEmpty())
            {
                newModeRef->setPriority(priority);
            }

            modeRefList->append(newModeRef);
        }
    }

    return modeRefList;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseFileSetReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FileSetRef> > > CommonItemsReader::parseFileSetReferences(
    QDomElement const& itemNode, Document::Revision docRevision)
{
    QDomNodeList fileSetReferenceNodeList = itemNode.elementsByTagName(QStringLiteral("ipxact:fileSetRef"));

    QSharedPointer<QList<QSharedPointer<FileSetRef> > > newFileSetRefList(new QList<QSharedPointer<FileSetRef> >());

    for (int i = 0; i < fileSetReferenceNodeList.count(); ++i)
    {
        QDomNode fileSetNode = fileSetReferenceNodeList.at(i);
        QString referenceName = fileSetNode.firstChildElement(QStringLiteral("ipxact:localName")).firstChild().nodeValue();

        QSharedPointer<FileSetRef> newReference(new FileSetRef());
        
        newReference->setReference(referenceName);

        if (docRevision == Document::Revision::Std14)
        {
            newReference->setIsPresent(
                fileSetNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
        }
        else if (docRevision == Document::Revision::Std22)
        {
            parseVendorExtensions(fileSetNode, newReference);
        }

        newFileSetRefList->append(newReference);
    }

    return newFileSetRefList;
}
