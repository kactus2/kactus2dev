//-----------------------------------------------------------------------------
// File: ComponentInstanceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// XML reader class for IP-XACT component instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceReader.h"

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::ComponentInstanceReader()
//-----------------------------------------------------------------------------
ComponentInstanceReader::ComponentInstanceReader(QObject* parent /* = 0 */) :
DocumentReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::ComponentInstanceReader()
//-----------------------------------------------------------------------------
ComponentInstanceReader::~ComponentInstanceReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createComponentInstanceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentInstanceReader::createComponentInstanceFrom(
    QDomNode const& instanceNode) const
{
    QString instanceName = instanceNode.firstChildElement("ipxact:instanceName").firstChild().nodeValue();
    QString displayName = instanceNode.firstChildElement("ipxact:displayName").firstChild().nodeValue();
    QString description = instanceNode.firstChildElement("ipxact:description").firstChild().nodeValue();
    QString isPresent = instanceNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();

    QDomNode componentRefNode = instanceNode.firstChildElement("ipxact:componentRef");
    QSharedPointer<ConfigurableVLNVReference> componentRef =
        parseConfigurableVLNVReference(componentRefNode, VLNV::COMPONENT);

    QDomElement extensionNode = instanceNode.firstChildElement("ipxact:vendorExtensions");
    QDomElement uuidElement = extensionNode.firstChildElement("kactus2:uuid");
    QString uuid("");
    if (!uuidElement.isNull())
    {
        uuid = uuidElement.firstChild().nodeValue();
    }

    QSharedPointer<ComponentInstance> newComponentInstance (new ComponentInstance(instanceName, displayName,
        description, componentRef, QPointF(), uuid));
    newComponentInstance->setIsPresent(isPresent);

    parseExtensions(instanceNode, newComponentInstance);

    return newComponentInstance;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseExtensions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseExtensions(const QDomNode& componentInstanceNode,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomNode extensionsNode = componentInstanceNode.firstChildElement("ipxact:vendorExtensions");
    QDomNodeList extensionNodeList = extensionsNode.childNodes();

    for (int extensionIndex = 0; extensionIndex < extensionNodeList.count(); ++extensionIndex)
    {
        QDomNode singleExtension = extensionNodeList.at(extensionIndex);
        QDomElement extensionElement = singleExtension.toElement();

        if (extensionElement.nodeName() == "kactus2:position")
        {
            parsePosition(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:imported")
        {
            parseImport(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:portPositions")
        {
            parsePortPositions(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:adHocVisibilities")
        {
            parseAdHocVisibilities(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:apiInterfacePositions")
        {
            parseApiInterfacePositions(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:comInterfacePositions")
        {
            parseComInterfacePositions(extensionElement, instance);
        }
        else if (extensionElement.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(extensionElement, instance);
        }
    }

    parseVendorExtensions(componentInstanceNode, instance);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePosition()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePosition(const QDomElement& positionElement,
    QSharedPointer<ComponentInstance> instance) const
{
    int positionX = positionElement.attribute("x").toInt();
    int positionY = positionElement.attribute("y").toInt();
    instance->setPosition(QPointF(positionX, positionY));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseImport()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseImport(const QDomElement& importElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QString importReference = importElement.attribute("importRef");
    instance->setImportRef(importReference);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePortPositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePortPositions(const QDomElement& portPositionsElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> portPositions = createMappedPositions(portPositionsElement, "kactus2:portPosition",
        "busRef");
    QMapIterator<QString, QPointF> portIterator(portPositions);
    while (portIterator.hasNext())
    {
        portIterator.next();
        instance->updateBusInterfacePosition(portIterator.key(), portIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseAdHocVisibilities()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseAdHocVisibilities(const QDomElement& adHocElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> adHocPositions = createMappedPositions(adHocElement, "kactus2:adHocVisible",
        "portName");
    QMapIterator<QString, QPointF> adHocIterator(adHocPositions);
    while (adHocIterator.hasNext())
    {
        adHocIterator.next();
        instance->updateAdHocPortPosition(adHocIterator.key(), adHocIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseApiInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseApiInterfacePositions(const QDomElement& apiElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> apiInterfacePositions = createMappedPositions(apiElement,
        "kactus2:apiInterfacePosition", "apiRef");
    QMapIterator<QString, QPointF> apiInterfaceIterator(apiInterfacePositions);
    while (apiInterfaceIterator.hasNext())
    {
        apiInterfaceIterator.next();
        instance->updateApiInterfacePosition(apiInterfaceIterator.key(), apiInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseComInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseComInterfacePositions(const QDomElement& comElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> comInterfacePositions = createMappedPositions(comElement,
        "kactus2:comInterfacePosition", "comRef");
    QMapIterator<QString, QPointF> comInterfaceIterator(comInterfacePositions);
    while (comInterfaceIterator.hasNext())
    {
        comInterfaceIterator.next();
        instance->updateComInterfacePosition(comInterfaceIterator.key(), comInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePropertyValues(const QDomElement& propertyElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomNodeList propertyValueNodes = propertyElement.elementsByTagName("kactus2:propertyValue");
    QMap<QString, QString> newPropertyValues;

    for (int propertyIndex = 0; propertyIndex < propertyValueNodes.count(); ++propertyIndex)
    {
        QDomNode propertyNode = propertyValueNodes.at(propertyIndex);
        QDomNamedNodeMap propertyAttributes = propertyNode.attributes();

        QString propertyName = propertyAttributes.namedItem("name").firstChild().nodeValue();
        QString propertyValue = propertyAttributes.namedItem("value").firstChild().nodeValue();
        newPropertyValues.insert(propertyName, propertyValue);
    }

    instance->setPropertyValues(newPropertyValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createMappedPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstanceReader::createMappedPositions(const QDomElement& positionElement,
    QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    QMap<QString, QPointF> positionMap;

    QDomNodeList positionNodeList = positionElement.elementsByTagName(itemIdentifier);
    for (int positionIndex = 0; positionIndex < positionNodeList.count(); ++positionIndex)
    {
        QDomNode singlePositionNode = positionNodeList.at(positionIndex);
        QDomElement positionElement = singlePositionNode.toElement();

        QString interfaceReference = positionElement.attribute(referenceIdentifier);
        int positionX = positionElement.attribute("x").toInt();
        int positionY = positionElement.attribute("y").toInt();

        positionMap.insert(interfaceReference, QPointF(positionX, positionY));
    }

    return positionMap;
}