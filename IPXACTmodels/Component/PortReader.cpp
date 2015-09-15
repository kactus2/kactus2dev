//-----------------------------------------------------------------------------
// File: PortReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.09.2015
//
// Description:
// Reader class for IP-XACT port element.
//-----------------------------------------------------------------------------

#include "PortReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ProtocolReader.h>

//-----------------------------------------------------------------------------
// Function: PortReader::PortReader()
//-----------------------------------------------------------------------------
PortReader::PortReader(QObject* parent /* = 0 */):
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortReader::~PortReader()
//-----------------------------------------------------------------------------
PortReader::~PortReader()
{

}

//-----------------------------------------------------------------------------
// Function: PortReader::createPortFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortReader::createPortFrom(QDomNode const& portNode) const
{
    QSharedPointer<Port> newPort (new Port());

    NameGroupReader nameReader;
    nameReader.parseNameGroup(portNode, newPort);

    newPort->setIsPresent(portNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue());

    QDomElement wireElement = portNode.firstChildElement("ipxact:wire");
    if (!wireElement.isNull())
    {
        parseWire(wireElement, newPort);
    }
    QDomElement transactionalElement = portNode.firstChildElement("ipxact:transactional");
    if (!transactionalElement.isNull())
    {
        parseTransactional(transactionalElement, newPort);
    }

    parseArrays(portNode, newPort);

    parsePortExtensions(portNode, newPort);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseWire()
//-----------------------------------------------------------------------------
void PortReader::parseWire(QDomElement const& wireElement, QSharedPointer<Port> newPort) const
{
    QDomElement directionElement = wireElement.firstChildElement("ipxact:direction");
    QString directionString = directionElement.firstChild().nodeValue();

    QSharedPointer<Wire> newWire (new Wire());
    newWire->setDirection(DirectionTypes::str2Direction(directionString, DirectionTypes::IN));

    QString allLogicalDirectionsAllowed = wireElement.attribute("allLogicalDirectionsAllowed");
    if (!allLogicalDirectionsAllowed.isEmpty())
    {
        newWire->setAllLogicalDirectionsAllowed(true);
    }

    parseWireVectors(wireElement.firstChildElement("ipxact:vectors"), newWire);

    QDomElement wireTypeDefsElement = wireElement.firstChildElement("ipxact:wireTypeDefs");
    if (!wireTypeDefsElement.isNull())
    {
        newWire->setWireTypeDefs(parseWireTypeDefinitions(wireTypeDefsElement, "ipxact:wireTypeDef", "constrained"));
    }

    parseWireDefaultValue(wireElement, newWire);

    newPort->setWire(newWire);
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseWireVectors()
//-----------------------------------------------------------------------------
void PortReader::parseWireVectors(QDomElement const& vectorsElement, QSharedPointer<Wire> newWire) const
{
    QDomNodeList vectorNodeList = vectorsElement.elementsByTagName("ipxact:vector");
    for (int vectorIndex = 0; vectorIndex < vectorNodeList.count(); ++vectorIndex)
    {
        QDomNode vectorNode = vectorNodeList.at(vectorIndex);
        newWire->setVectorLeftBound(vectorNode.firstChildElement("ipxact:left").firstChild().nodeValue());
        newWire->setVectorRightBound(vectorNode.firstChildElement("ipxact:right").firstChild().nodeValue());
    }

}

//-----------------------------------------------------------------------------
// Function: PortReader::parseWireTypeDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > PortReader::parseWireTypeDefinitions(
    QDomElement const& typeDefinitionsElement, QString const& elementName, QString const& attributeName) const
{
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions (new QList<QSharedPointer<WireTypeDef> >);

    QDomNodeList wireYypeDefinitionNodeList = typeDefinitionsElement.elementsByTagName(elementName);
    for (int wireTypeIndex = 0; wireTypeIndex < wireYypeDefinitionNodeList.count(); ++wireTypeIndex)
    {
        QDomElement wireTypeDefinitionElement = wireYypeDefinitionNodeList.at(wireTypeIndex).toElement();

        QDomElement nameElement = wireTypeDefinitionElement.firstChildElement("ipxact:typeName");
        QString typeName = nameElement.firstChild().nodeValue();
        QSharedPointer<WireTypeDef> newWireTypeDefinition (new WireTypeDef(typeName));
        if (!nameElement.attribute(attributeName).isEmpty())
        {
            newWireTypeDefinition->setConstrained(true);
        }

        parseTypeDefinitions(wireTypeDefinitionElement, newWireTypeDefinition);

        parseViewReferences(wireTypeDefinitionElement, newWireTypeDefinition);

        typeDefinitions->append(newWireTypeDefinition);
    }

    return typeDefinitions;
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseTypeDefinitions()
//-----------------------------------------------------------------------------
void PortReader::parseTypeDefinitions(QDomElement const& wireTypeDefinitionElement,
    QSharedPointer<WireTypeDef> newWireTypeDefinition) const
{
    QDomNodeList typeDefinitionList = wireTypeDefinitionElement.elementsByTagName("ipxact:typeDefinition");
    QStringList definitionList;
    for (int typeDefinitionIndex = 0; typeDefinitionIndex < typeDefinitionList.count(); ++typeDefinitionIndex)
    {
        QString definition = typeDefinitionList.at(typeDefinitionIndex).firstChild().nodeValue();
        definitionList.append(definition);
    }
    if (!definitionList.isEmpty())
    {
        newWireTypeDefinition->setTypeDefinitions(definitionList);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseViewReferences()
//-----------------------------------------------------------------------------
void PortReader::parseViewReferences(QDomElement const& wireTypeDefinitionElement,
    QSharedPointer<WireTypeDef> newWireTypeDefinition) const
{
    QDomNodeList viewRefNodeList = wireTypeDefinitionElement.elementsByTagName("ipxact:viewRef");
    QStringList viewRefs;

    for (int viewIndex = 0; viewIndex < viewRefNodeList.count(); ++viewIndex)
    {
        QString viewName = viewRefNodeList.at(viewIndex).firstChild().nodeValue();
        viewRefs.append(viewName);
    }

    if (!viewRefs.isEmpty())
    {
        newWireTypeDefinition->setViewRefs(viewRefs);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseWireDefaultValue()
//-----------------------------------------------------------------------------
void PortReader::parseWireDefaultValue(QDomElement const& wireElement, QSharedPointer<Wire> newWire) const
{
    QDomElement driversElement = wireElement.firstChildElement("ipxact:drivers");

    if (!driversElement.isNull())
    {
        QDomNodeList driverNodeList = driversElement.elementsByTagName("ipxact:driver");
        for (int driverIndex = 0; driverIndex < driverNodeList.count(); ++driverIndex)
        {
            QDomNode driverDefaultNode = driverNodeList.at(driverIndex).firstChildElement("ipxact:defaultValue");
            QString defaultValue = driverDefaultNode.firstChild().nodeValue();
            newWire->setDefaultDriverValue(defaultValue);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseTransactional()
//-----------------------------------------------------------------------------
void PortReader::parseTransactional(QDomElement const& transactionalElement, QSharedPointer<Port> newPort) const
{
    QSharedPointer<Transactional> newTransactional(new Transactional());

    QString allLogicalInitiativesAllowed = transactionalElement.attribute("allLogicalInitiativesAllowed");
    if (!allLogicalInitiativesAllowed.isEmpty())
    {
        newTransactional->setAllLogicalInitiativesAllowed(true);
    }

    QDomElement initiativeElement = transactionalElement.firstChildElement("ipxact:initiative");
    QString initiative = initiativeElement.firstChild().nodeValue();
    newTransactional->setInitiative(initiative);

    parseTransactionalKind(transactionalElement.firstChildElement("ipxact:kind"), newTransactional);

    QDomElement busWidthElement = transactionalElement.firstChildElement("ipxact:busWidth");
    if (!busWidthElement.isNull())
    {
        QString busWidth = busWidthElement.firstChild().nodeValue();
        newTransactional->setBusWidth(busWidth);
    }

    parseTransactionalProtocol(transactionalElement, newTransactional);

    QDomElement typeDefinitions = transactionalElement.firstChildElement("ipxact:transTypeDefs");
    if (!typeDefinitions.isNull())
    {
        newTransactional->setTransTypeDefs(parseWireTypeDefinitions(typeDefinitions, "ipxact:transTypeDef",
            "exact"));
    }

    parseTransactionalConnectionsMinMax(transactionalElement, newTransactional);

    newPort->setTransactional(newTransactional);
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseTransactionalKind()
//-----------------------------------------------------------------------------
void PortReader::parseTransactionalKind(QDomElement const& kindElement,
    QSharedPointer<Transactional> transactional) const
{
    if (!kindElement.isNull())
    {
        QString transactionalKind = kindElement.firstChild().nodeValue();
        if (transactionalKind == "custom" && kindElement.hasAttribute("custom"))
        {
            transactionalKind = kindElement.attribute("custom");
        }
        transactional->setKind(transactionalKind);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseTransactionalProtocol()
//-----------------------------------------------------------------------------
void PortReader::parseTransactionalProtocol(QDomNode const& transactionalNode,
    QSharedPointer<Transactional> transactional) const
{
    QDomNode protocolNode = transactionalNode.firstChildElement("ipxact:protocol");
    if (!protocolNode.isNull())
    {
        ProtocolReader protocolReader;
        QSharedPointer<Protocol> transactionalProtocol = protocolReader.createProtocolFrom(protocolNode);
        transactional->setProtocol(transactionalProtocol);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseTransactionalConnectionsMinMax()
//-----------------------------------------------------------------------------
void PortReader::parseTransactionalConnectionsMinMax(QDomElement const& transactionalElement,
    QSharedPointer<Transactional> transactional) const
{
    QDomElement connectionsElement = transactionalElement.firstChildElement("ipxact:connection");
    if (!connectionsElement.isNull())
    {
        QDomElement maxElement = connectionsElement.firstChildElement("ipxact:maxConnections");
        if (!maxElement.isNull())
        {
            QString maxConnections = maxElement.firstChild().nodeValue();
            transactional->setMaxConnections(maxConnections);
        }

        QDomElement minElement = connectionsElement.firstChildElement("ipxact:minConnections");
        if (!minElement.isNull())
        {
            QString minConnections = minElement.firstChild().nodeValue();
            transactional->setMinConnections(minConnections);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parseArrays()
//-----------------------------------------------------------------------------
void PortReader::parseArrays(QDomNode const& portNode, QSharedPointer<Port> newPort) const
{
    QDomElement arraysElement = portNode.firstChildElement("ipxact:arrays");

    QDomNodeList arrayNodeList = arraysElement.elementsByTagName("ipxact:array");

    for (int arrayIndex = 0; arrayIndex < arrayNodeList.count(); ++arrayIndex)
    {
        QDomNode arrayNode = arrayNodeList.at(arrayIndex);

        QString arrayLeft = arrayNode.firstChildElement("ipxact:left").firstChild().nodeValue();
        QString arrayRight = arrayNode.firstChildElement("ipxact:right").firstChild().nodeValue();

        QSharedPointer<Array> newArray (new Array(arrayLeft, arrayRight));

        newPort->getArrays()->append(newArray);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::parsePortExtensions()
//-----------------------------------------------------------------------------
void PortReader::parsePortExtensions(QDomNode const& portNode, QSharedPointer<Port> newPort) const
{
    QDomElement extensionsNode = portNode.firstChildElement("ipxact:vendorExtensions");

    QDomElement adHocElement = extensionsNode.firstChildElement("kactus2:adHocVisible");
    if (!adHocElement.isNull())
    {
        newPort->setAdHocVisible(true);
    }

    QDomElement positionElement = extensionsNode.firstChildElement("kactus2:position");
    if (!positionElement.isNull())
    {
        parsePosition(positionElement, newPort);
    }

    QDomElement tagsElement = extensionsNode.firstChildElement("kactus2:portTags");
    if (!tagsElement.isNull())
    {
        newPort->setPortTags(tagsElement.firstChild().nodeValue());
    }

    parseVendorExtensions(portNode, newPort);
}

//-----------------------------------------------------------------------------
// Function: PortReader::parsePosition()
//-----------------------------------------------------------------------------
void PortReader::parsePosition(QDomElement const& positionElement, QSharedPointer<Port> newPort) const
{
    int positionX = positionElement.attribute("x").toInt();
    int positionY = positionElement.attribute("y").toInt();
    newPort->setDefaultPos(QPointF(positionX, positionY));
}