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
#include <IPXACTmodels/common/QualifierReader.h>

//-----------------------------------------------------------------------------
// Function: PortReader::createPortFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortReader::createPortFrom(QDomNode const& portNode, Document::Revision docRevision)
{
    QSharedPointer<Port> newPort (new Port());

    NameGroupReader::parseNameGroup(portNode, newPort);

    newPort->setIsPresent(portNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());

    QDomElement wireElement = portNode.firstChildElement(QStringLiteral("ipxact:wire"));
    if (!wireElement.isNull())
    {
        Details::parseWire(wireElement, newPort, docRevision);
    }
    QDomElement transactionalElement = portNode.firstChildElement(QStringLiteral("ipxact:transactional"));
    if (!transactionalElement.isNull())
    {
        Details::parseTransactional(transactionalElement, newPort);
    }

    Details::parseArrays(portNode, newPort);

    Details::parsePortExtensions(portNode, newPort);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWire()
//-----------------------------------------------------------------------------
void PortReader::Details::parseWire(QDomElement const& wireElement, QSharedPointer<Port> newPort,
    Document::Revision docRevision)
{
    QDomElement directionElement = wireElement.firstChildElement(QStringLiteral("ipxact:direction"));
    QString directionString = directionElement.firstChild().nodeValue();

    QSharedPointer<Wire> newWire (new Wire());
    newWire->setDirection(DirectionTypes::str2Direction(directionString, DirectionTypes::IN));

    if (docRevision == Document::Revision::Std22)
    {
        auto qualifierElement = wireElement.firstChildElement(QStringLiteral("ipxact:qualifier"));
        QualifierReader::parseQualifier(qualifierElement, newWire->getQualifier(), docRevision);
    }

    QString allLogicalDirectionsAllowed = wireElement.attribute(QStringLiteral("allLogicalDirectionsAllowed"));
    if (!allLogicalDirectionsAllowed.isEmpty())
    {
        newWire->setAllLogicalDirectionsAllowed(true);
    }

    parseWireVectors(wireElement.firstChildElement(QStringLiteral("ipxact:vectors")), newWire, docRevision);

    QDomElement wireTypeDefsElement = wireElement.firstChildElement(QStringLiteral("ipxact:wireTypeDefs"));
    if (!wireTypeDefsElement.isNull())
    {
        newWire->setWireTypeDefs(parseWireTypeDefinitions(wireTypeDefsElement, QStringLiteral("ipxact:wireTypeDef"),
            QStringLiteral("constrained")));
    }

    parseWireDefaultValue(wireElement, newWire);

    newPort->setWire(newWire);
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWireVectors()
//-----------------------------------------------------------------------------
void PortReader::Details::parseWireVectors(QDomElement const& vectorsElement, QSharedPointer<Wire> newWire,
    Document::Revision docRevision)
{
    QDomNodeList vectorNodeList = vectorsElement.elementsByTagName(QStringLiteral("ipxact:vector"));
    for (int vectorIndex = 0; vectorIndex < vectorNodeList.count(); ++vectorIndex)
    {
        QDomNode vectorNode = vectorNodeList.at(vectorIndex);

        newWire->setVectorLeftBound(vectorNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue());
        newWire->setVectorRightBound(vectorNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue());

        auto vector = newWire->getVector();
        if (docRevision == Document::Revision::Std22)
        {
            vector->setId(vectorNode.toElement().attribute(QStringLiteral("vectorId")));
        }

    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWireTypeDefinitions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WireTypeDef> > > PortReader::Details::parseWireTypeDefinitions(
    QDomElement const& typeDefinitionsElement, QString const& elementName, QString const& attributeName)
{
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions (new QList<QSharedPointer<WireTypeDef> >);

    QDomNodeList wireYypeDefinitionNodeList = typeDefinitionsElement.elementsByTagName(elementName);
    for (int wireTypeIndex = 0; wireTypeIndex < wireYypeDefinitionNodeList.count(); ++wireTypeIndex)
    {
        QDomElement wireTypeDefinitionElement = wireYypeDefinitionNodeList.at(wireTypeIndex).toElement();

        QDomElement nameElement = wireTypeDefinitionElement.firstChildElement(QStringLiteral("ipxact:typeName"));
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
// Function: PortReader::Details::parseTypeDefinitions()
//-----------------------------------------------------------------------------
void PortReader::Details::parseTypeDefinitions(QDomElement const& wireTypeDefinitionElement,
    QSharedPointer<WireTypeDef> newWireTypeDefinition)
{
    QDomNodeList typeDefinitionList = wireTypeDefinitionElement.elementsByTagName(QStringLiteral("ipxact:typeDefinition"));
    QSharedPointer<QStringList> definitionList(new QStringList());
    for (int typeDefinitionIndex = 0; typeDefinitionIndex < typeDefinitionList.count(); ++typeDefinitionIndex)
    {
        QString definition = typeDefinitionList.at(typeDefinitionIndex).firstChild().nodeValue();
        definitionList->append(definition);
    }
    if (!definitionList->isEmpty())
    {
        newWireTypeDefinition->setTypeDefinitions(definitionList);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseViewReferences()
//-----------------------------------------------------------------------------
void PortReader::Details::parseViewReferences(QDomElement const& wireTypeDefinitionElement,
    QSharedPointer<WireTypeDef> newWireTypeDefinition)
{
    QDomNodeList viewRefNodeList = wireTypeDefinitionElement.elementsByTagName(QStringLiteral("ipxact:viewRef"));
    QSharedPointer<QStringList> viewRefs(new QStringList());

    for (int viewIndex = 0; viewIndex < viewRefNodeList.count(); ++viewIndex)
    {
        QString viewName = viewRefNodeList.at(viewIndex).firstChild().nodeValue();
        viewRefs->append(viewName);
    }

    if (!viewRefs->isEmpty())
    {
        newWireTypeDefinition->setViewRefs(viewRefs);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWireDefaultValue()
//-----------------------------------------------------------------------------
void PortReader::Details::parseWireDefaultValue(QDomElement const& wireElement, QSharedPointer<Wire> newWire)
{
    QDomElement driversElement = wireElement.firstChildElement(QStringLiteral("ipxact:drivers"));

    if (!driversElement.isNull())
    {
        QDomNodeList driverNodeList = driversElement.elementsByTagName(QStringLiteral("ipxact:driver"));
        for (int driverIndex = 0; driverIndex < driverNodeList.count(); ++driverIndex)
        {
            auto driver = QSharedPointer<Driver>(new Driver());
            if (QDomElement rangeElement = driverNodeList.at(driverIndex).firstChildElement(QStringLiteral("ipxact:range"));
                !rangeElement.isNull())
            {
                driver->setRange(CommonItemsReader::parseRange(rangeElement));
            }

            QDomNode driverDefaultNode = driverNodeList.at(driverIndex).firstChildElement(QStringLiteral("ipxact:defaultValue"));
            QString defaultValue = driverDefaultNode.firstChild().nodeValue();
            driver->setDefaultValue(defaultValue);

            newWire->setDriver(driver);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseTransactional()
//-----------------------------------------------------------------------------
void PortReader::Details::parseTransactional(QDomElement const& transactionalElement, QSharedPointer<Port> newPort)
{
    QSharedPointer<Transactional> newTransactional(new Transactional());

    QString allLogicalInitiativesAllowed = transactionalElement.attribute(
        QStringLiteral("allLogicalInitiativesAllowed"));
    if (!allLogicalInitiativesAllowed.isEmpty())
    {
        newTransactional->setAllLogicalInitiativesAllowed(true);
    }

    QDomElement initiativeElement = transactionalElement.firstChildElement(QStringLiteral("ipxact:initiative"));
    QString initiative = initiativeElement.firstChild().nodeValue();
    newTransactional->setInitiative(initiative);

    parseTransactionalKind(transactionalElement.firstChildElement(QStringLiteral("ipxact:kind")), newTransactional);

    QDomElement busWidthElement = transactionalElement.firstChildElement(QStringLiteral("ipxact:busWidth"));
    if (!busWidthElement.isNull())
    {
        QString busWidth = busWidthElement.firstChild().nodeValue();
        newTransactional->setBusWidth(busWidth);
    }

    parseTransactionalProtocol(transactionalElement, newTransactional);

    QDomElement typeDefinitions = transactionalElement.firstChildElement(QStringLiteral("ipxact:transTypeDefs"));
    if (!typeDefinitions.isNull())
    {
        newTransactional->setTransTypeDefs(parseWireTypeDefinitions(typeDefinitions, 
            QStringLiteral("ipxact:transTypeDef"), QStringLiteral("exact")));
    }

    parseTransactionalConnectionsMinMax(transactionalElement, newTransactional);

    newPort->setTransactional(newTransactional);
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseTransactionalKind()
//-----------------------------------------------------------------------------
void PortReader::Details::parseTransactionalKind(QDomElement const& kindElement,
    QSharedPointer<Transactional> transactional)
{
    if (!kindElement.isNull())
    {
        QString transactionalKind = kindElement.firstChild().nodeValue();
        if (transactionalKind == QLatin1String("custom") && kindElement.hasAttribute(QStringLiteral("custom")))
        {
            transactionalKind = kindElement.attribute(QStringLiteral("custom"));
        }
        transactional->setKind(transactionalKind);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseTransactionalProtocol()
//-----------------------------------------------------------------------------
void PortReader::Details::parseTransactionalProtocol(QDomNode const& transactionalNode,
    QSharedPointer<Transactional> transactional)
{
    QDomNode protocolNode = transactionalNode.firstChildElement(QStringLiteral("ipxact:protocol"));
    if (!protocolNode.isNull())
    {
        ProtocolReader protocolReader;
        QSharedPointer<Protocol> transactionalProtocol = protocolReader.createProtocolFrom(protocolNode);
        transactional->setProtocol(transactionalProtocol);
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseTransactionalConnectionsMinMax()
//-----------------------------------------------------------------------------
void PortReader::Details::parseTransactionalConnectionsMinMax(QDomElement const& transactionalElement,
    QSharedPointer<Transactional> transactional)
{
    QDomElement connectionsElement = transactionalElement.firstChildElement(QStringLiteral("ipxact:connection"));
    if (!connectionsElement.isNull())
    {
        QDomElement maxElement = connectionsElement.firstChildElement(QStringLiteral("ipxact:maxConnections"));
        if (!maxElement.isNull())
        {
            QString maxConnections = maxElement.firstChild().nodeValue();
            transactional->setMaxConnections(maxConnections);
        }

        QDomElement minElement = connectionsElement.firstChildElement(QStringLiteral("ipxact:minConnections"));
        if (!minElement.isNull())
        {
            QString minConnections = minElement.firstChild().nodeValue();
            transactional->setMinConnections(minConnections);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseArrays()
//-----------------------------------------------------------------------------
void PortReader::Details::parseArrays(QDomNode const& portNode, QSharedPointer<Port> newPort)
{
    QDomElement arraysElement = portNode.firstChildElement(QStringLiteral("ipxact:arrays"));

    QDomNodeList arrayNodeList = arraysElement.elementsByTagName(QStringLiteral("ipxact:array"));

    for (int arrayIndex = 0; arrayIndex < arrayNodeList.count(); ++arrayIndex)
    {
        QDomNode arrayNode = arrayNodeList.at(arrayIndex);

        QString arrayLeft = arrayNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString arrayRight = arrayNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        newPort->getArrays()->append(QSharedPointer<Array>(new Array(arrayLeft, arrayRight)));
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parsePortExtensions()
//-----------------------------------------------------------------------------
void PortReader::Details::parsePortExtensions(QDomNode const& portNode, QSharedPointer<Port> newPort)
{
    QDomElement extensionsNode = portNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement adHocElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:adHocVisible"));
    if (!adHocElement.isNull())
    {
        newPort->setAdHocVisible(true);
    }

    QDomElement positionElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:position"));
    if (!positionElement.isNull())
    {
        parsePosition(positionElement, newPort);
    }

    QDomElement tagsElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:portTags"));
    if (!tagsElement.isNull())
    {
        newPort->setPortTags(tagsElement.firstChild().nodeValue());
    }

    CommonItemsReader::parseVendorExtensions(portNode, newPort);
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parsePosition()
//-----------------------------------------------------------------------------
void PortReader::Details::parsePosition(QDomElement const& positionElement, QSharedPointer<Port> newPort)
{
    int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
    int positionY = positionElement.attribute(QStringLiteral("y")).toInt();
    newPort->setDefaultPos(QPointF(positionX, positionY));
}