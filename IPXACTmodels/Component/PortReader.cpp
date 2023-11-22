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

    if (QDomElement wireElement = portNode.firstChildElement(QStringLiteral("ipxact:wire")); 
        !wireElement.isNull())
    {
        newPort->setWire(Details::createWireFrom(wireElement, docRevision));
    }
    
    if (QDomElement transactionalElement = portNode.firstChildElement(QStringLiteral("ipxact:transactional")); 
        !transactionalElement.isNull())
    {
        Details::parseTransactional(transactionalElement, newPort);
    }

    if (docRevision == Document::Revision::Std22)
    {
        if (QDomElement structuredElement = portNode.firstChildElement(QStringLiteral("ipxact:structured")); 
            !structuredElement.isNull())
        {
            newPort->setStructured(Details::createStructuredFrom(structuredElement));
        }
    }

    Details::parseArrays(portNode, newPort);

    Details::parsePortExtensions(portNode, newPort);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWire()
//-----------------------------------------------------------------------------
QSharedPointer<Wire> PortReader::Details::createWireFrom(QDomElement const& wireElement, 
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

    parseWireVectors(wireElement, newWire, docRevision);

    QDomElement wireTypeDefsElement = wireElement.firstChildElement(QStringLiteral("ipxact:wireTypeDefs"));
    if (!wireTypeDefsElement.isNull())
    {
        newWire->setWireTypeDefs(parseWireTypeDefinitions(wireTypeDefsElement, QStringLiteral("ipxact:wireTypeDef"),
            QStringLiteral("constrained")));
    }

    parseWireDefaultValue(wireElement, newWire);

    return newWire;
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseWireVectors()
//-----------------------------------------------------------------------------
void PortReader::Details::parseWireVectors(QDomElement const& wireElement, QSharedPointer<Wire> newWire,
    Document::Revision docRevision)
{
    newWire->getVectors()->append(parseVectors(wireElement, docRevision));
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseVectors()
//-----------------------------------------------------------------------------
QList<Vector> PortReader::Details::parseVectors(QDomElement const& parentElement, Document::Revision docRevision)
{
    auto const vectorsElement = parentElement.firstChildElement(QStringLiteral("ipxact:vectors"));

    auto const vectorNodeList = vectorsElement.elementsByTagName(QStringLiteral("ipxact:vector"));

    QList<Vector> vectors;
    const auto VECTOR_COUNT = vectorNodeList.size();
    for (int i = 0; i < VECTOR_COUNT; ++i)
    {
        vectors.append(parseVector(vectorNodeList.at(i), docRevision));
    }

    return vectors;
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseVector()
//-----------------------------------------------------------------------------
Vector PortReader::Details::parseVector(QDomNode const& vectorNode, Document::Revision docRevision)
{
    auto readVector = Vector();

    readVector.setLeft(vectorNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue());
    readVector.setRight(vectorNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue());

    if (docRevision == Document::Revision::Std22)
    {
        readVector.setId(vectorNode.toElement().attribute(QStringLiteral("vectorId")));
    }

    return readVector;
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
    auto const connectionsElement = transactionalElement.firstChildElement(QStringLiteral("ipxact:connection"));
    if (!connectionsElement.isNull())
    {
        auto const maxElement = connectionsElement.firstChildElement(QStringLiteral("ipxact:maxConnections"));
        transactional->setMaxConnections(maxElement.firstChild().nodeValue());

        auto const minElement = connectionsElement.firstChildElement(QStringLiteral("ipxact:minConnections"));
        transactional->setMinConnections(minElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseStructured()
//-----------------------------------------------------------------------------
QSharedPointer<Structured> PortReader::Details::createStructuredFrom(QDomElement const& structuredElement)
{
    QSharedPointer<Structured> newStructured(new Structured());

    newStructured->setPacked(structuredElement.attribute(QStringLiteral("packed")) == QStringLiteral("true"));

    parseStructuredType(structuredElement, newStructured);

    parseStructuredVectors(structuredElement, newStructured);

    parseSubPorts(structuredElement, newStructured);

    return newStructured;
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseStructuralType()
//-----------------------------------------------------------------------------
void PortReader::Details::parseStructuredType(QDomElement const& structuredElement,
    QSharedPointer<Structured> newStructured)
{
    using namespace DirectionTypes;
    if (auto const structElement = structuredElement.firstChildElement(QStringLiteral("ipxact:struct"));
        structElement.isNull() == false)
    {
        newStructured->setType(Structured::Type::Struct);
        newStructured->setDirection(str2Direction(structElement.attribute(QStringLiteral("direction"))));
    }
    else if (auto const unionElement = structuredElement.firstChildElement(QStringLiteral("ipxact:union"));
        unionElement.isNull() == false)
    {
        newStructured->setType(Structured::Type::Union);
        newStructured->setDirection(str2Direction(unionElement.attribute(QStringLiteral("direction"))));
    }
    else if (auto const interfaceElement = structuredElement.firstChildElement(QStringLiteral("ipxact:interface"));
        interfaceElement.isNull() == false)
    {
        newStructured->setType(Structured::Type::Interface);
        if (interfaceElement.attribute(QStringLiteral("phantom")) == QStringLiteral("true"))
        {
            newStructured->setDirection(DIRECTION_PHANTOM);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseStructuralVectors()
//-----------------------------------------------------------------------------
void PortReader::Details::parseStructuredVectors(QDomElement const& structuredElement,
    QSharedPointer<Structured> newStructured)
{
    newStructured->getVectors()->append(parseVectors(structuredElement, Document::Revision::Std22));
}

//-----------------------------------------------------------------------------
// Function: PortReader::Details::parseSubPorts()
//-----------------------------------------------------------------------------
void PortReader::Details::parseSubPorts(QDomElement const& structuredElement, QSharedPointer<Structured> newStructured)
{
    const auto subPortsElement = structuredElement.firstChildElement(QStringLiteral("ipxact:subPorts"));
    const auto subPortList = subPortsElement.childNodes();

    const auto SUBPORT_COUNT = subPortList.count();
    for (int i = 0; i < SUBPORT_COUNT; ++i)
    {
        const auto subPortElement = subPortList.at(i).toElement();

        QSharedPointer<SubPort> parsedSubPort(new SubPort());
        parsedSubPort->setIsIO(subPortElement.attribute(QStringLiteral("isIO")) == QStringLiteral("true"));
        NameGroupReader::parseNameGroup(subPortElement, parsedSubPort);

        if (QDomElement wireElement = subPortElement.firstChildElement(QStringLiteral("ipxact:wire"));
            !wireElement.isNull())
        {
            parsedSubPort->setWire(Details::createWireFrom(wireElement, Document::Revision::Std22));
        }
        else if (QDomElement nestedStructured = subPortElement.firstChildElement(QStringLiteral("ipxact:structured"));
            !nestedStructured.isNull())
        {
            parsedSubPort->setStructured(Details::createStructuredFrom(nestedStructured));
        }

        newStructured->getSubPorts()->append(parsedSubPort);
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

    if (QDomElement adHocElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:adHocVisible")); 
        !adHocElement.isNull())
    {
        newPort->setAdHocVisible(true);
    }

    if (QDomElement positionElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:position")); 
        !positionElement.isNull())
    {
        parsePosition(positionElement, newPort);
    }

    if (QDomElement tagsElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:portTags")); 
        !tagsElement.isNull())
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