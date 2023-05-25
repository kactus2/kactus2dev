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

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::ComponentInstanceReader()
//-----------------------------------------------------------------------------
ComponentInstanceReader::ComponentInstanceReader() : CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createComponentInstanceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentInstanceReader::createComponentInstanceFrom(
    QDomNode const& instanceNode, Document::Revision docRevision) const
{
    QSharedPointer<ComponentInstance> newComponentInstance(new ComponentInstance());
    NameGroupReader::parseNameGroup(instanceNode, newComponentInstance);

    QString instanceName = instanceNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue();
    newComponentInstance->setInstanceName(instanceName);


    QDomNode componentRefNode = instanceNode.firstChildElement(QStringLiteral("ipxact:componentRef"));

    QSharedPointer<ConfigurableVLNVReference> componentRef =
        parseConfigurableVLNVReference(componentRefNode, VLNV::COMPONENT);
    newComponentInstance->setComponentRef(componentRef);

    if (docRevision == Document::Revision::Std14)
    {
        QString isPresent = instanceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
        newComponentInstance->setIsPresent(isPresent);
    }

    if (docRevision == Document::Revision::Std22)
    {
        parsePowerDomainLinks(instanceNode, newComponentInstance);
    }

    parseExtensions(instanceNode, newComponentInstance);

    return newComponentInstance;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePowerDomainLinks()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePowerDomainLinks(QDomNode const& instanceNode,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomElement domainsNode = instanceNode.firstChildElement(QStringLiteral("ipxact:powerDomainLinks"));
    QDomNodeList domainNodeList = domainsNode.childNodes();

    const int count = domainNodeList.count();
    for (int index = 0; index < count; ++index)
    {
        auto const& domainNode = domainNodeList.at(index);
        auto externalLink = domainNode.firstChildElement(QStringLiteral("ipxact:externalPowerDomainReference")).firstChild().nodeValue();
        auto internalLink = domainNode.firstChildElement(QStringLiteral("ipxact:internalPowerDomainReference")).firstChild().nodeValue();

        auto link = QSharedPointer<ComponentInstance::PowerDomainLink>(
            new ComponentInstance::PowerDomainLink({ externalLink.toStdString(), internalLink.toStdString() }));
        instance->getPowerDomainLinks()->append(link);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseExtensions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseExtensions(const QDomNode& componentInstanceNode,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomElement extensionsNode = componentInstanceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomNodeList extensionNodeList = extensionsNode.childNodes();

    QDomElement uuidElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:uuid"));
    if (!uuidElement.isNull())
    {
        instance->setUuid(uuidElement.firstChild().nodeValue());
    }

    QDomElement draftNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:draft"));
    parseDraft(draftNode, instance);

    QDomElement positionNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:position"));
    parsePosition(positionNode, instance);
    
    QDomElement importNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:imported"));
    parseImport(importNode, instance);
    
    QDomElement portPositionNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:portPositions"));
    parsePortPositions(portPositionNode, instance);

    QDomElement adHocVisibilityNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:adHocVisibilities"));
    parseAdHocVisibilities(adHocVisibilityNode, instance);

    QDomElement apiPositionsNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:apiInterfacePositions"));
    parseApiInterfacePositions(apiPositionsNode, instance);
 
    QDomElement comPositionsNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:comInterfacePositions"));
    parseComInterfacePositions(comPositionsNode, instance);

    QDomElement propertyNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:propertyValues"));
    parsePropertyValues(propertyNode, instance);

    QDomElement fileSetRefNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:fileSetRef"));
    parseFileSetRef(fileSetRefNode, instance);

    QDomElement mappingNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:mapping"));
    parseMapping(mappingNode, instance);

    parseVendorExtensions(componentInstanceNode, instance);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseDraft()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseDraft(QDomElement const& draftNode,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!draftNode.isNull())
    {
        instance->setDraft(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePosition()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePosition(QDomElement const& positionElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!positionElement.isNull())
    {
        int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
        int positionY = positionElement.attribute(QStringLiteral("y")).toInt();
        instance->setPosition(QPointF(positionX, positionY));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseImport()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseImport(QDomElement const& importElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!importElement.isNull())
    {
        QString importReference = importElement.attribute(QStringLiteral("importRef"));

        if (!importReference.isEmpty())
        {
            instance->setImportRef(importReference);
        }
        else
        {
            instance->setImported(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePortPositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePortPositions(QDomElement const& portPositionsElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> portPositions = createMappedPositions(portPositionsElement, 
        QStringLiteral("kactus2:portPosition"), QStringLiteral("busRef"));
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
void ComponentInstanceReader::parseAdHocVisibilities(QDomElement const& adHocElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> adHocPositions = createMappedPositions(adHocElement,
        QStringLiteral("kactus2:adHocVisible"), QStringLiteral("portName"));
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
void ComponentInstanceReader::parseApiInterfacePositions(QDomElement const& apiElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> apiInterfacePositions = createMappedPositions(apiElement,
        QStringLiteral("kactus2:apiInterfacePosition"), QStringLiteral("apiRef"));
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
void ComponentInstanceReader::parseComInterfacePositions(QDomElement const& comElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> comInterfacePositions = createMappedPositions(comElement,
        QStringLiteral("kactus2:comInterfacePosition"), QStringLiteral("comRef"));
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
void ComponentInstanceReader::parsePropertyValues(QDomElement const& propertyElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomNodeList propertyValueNodes = propertyElement.elementsByTagName(QStringLiteral("kactus2:propertyValue"));
    QMap<QString, QString> newPropertyValues;

    for (int propertyIndex = 0; propertyIndex < propertyValueNodes.count(); ++propertyIndex)
    {
        QDomNode propertyNode = propertyValueNodes.at(propertyIndex);
        QDomNamedNodeMap propertyAttributes = propertyNode.attributes();

        QString propertyName = propertyAttributes.namedItem(QStringLiteral("name")).firstChild().nodeValue();
        QString propertyValue = propertyAttributes.namedItem(QStringLiteral("value")).firstChild().nodeValue();
        newPropertyValues.insert(propertyName, propertyValue);
    }

    instance->setPropertyValues(newPropertyValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createMappedPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstanceReader::createMappedPositions(QDomElement const& positionElement,
    QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    QMap<QString, QPointF> positionMap;

    QDomNodeList positionNodeList = positionElement.elementsByTagName(itemIdentifier);
    for (int positionIndex = 0; positionIndex < positionNodeList.count(); ++positionIndex)
    {
        QDomNode singlePositionNode = positionNodeList.at(positionIndex);
        QDomElement positionElement = singlePositionNode.toElement();

        QString interfaceReference = positionElement.attribute(referenceIdentifier);

        if (!positionElement.hasAttribute(QStringLiteral("x")))
        {
            positionElement = positionElement.firstChildElement(QStringLiteral("kactus2:position"));
        }

        int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
        int positionY = positionElement.attribute(QStringLiteral("y")).toInt();

        positionMap.insert(interfaceReference, QPointF(positionX, positionY));
    }

    return positionMap;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseFileSetRef()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseFileSetRef(QDomElement const& fileSetRefElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!fileSetRefElement.isNull())
    {
        QString fileSetReference = fileSetRefElement.firstChild().nodeValue();

        if (!fileSetReference.isEmpty())
        {
            instance->setFileSetRef(fileSetReference);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseMapping()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseMapping(QDomElement const& mappingElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!mappingElement.isNull())
    {
        QString mapping = mappingElement.attribute(QStringLiteral("hwRef"));

        if (!mapping.isEmpty())
        {
            instance->setMapping(mapping);
        }
    }
}
