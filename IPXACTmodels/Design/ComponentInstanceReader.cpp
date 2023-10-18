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
// Function: ComponentInstanceReader::createComponentInstanceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentInstanceReader::createComponentInstanceFrom(
    QDomNode const& instanceNode, Document::Revision docRevision)
{
    QSharedPointer<ComponentInstance> newComponentInstance(new ComponentInstance());
    NameGroupReader::parseNameGroup(instanceNode, newComponentInstance);

    QString instanceName = instanceNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue();
    newComponentInstance->setInstanceName(instanceName);

    QDomNode componentRefNode = instanceNode.firstChildElement(QStringLiteral("ipxact:componentRef"));

    QSharedPointer<ConfigurableVLNVReference> componentRef =
        CommonItemsReader::parseConfigurableVLNVReference(componentRefNode, VLNV::COMPONENT);
    newComponentInstance->setComponentRef(componentRef);

    if (docRevision == Document::Revision::Std14)
    {
        QString isPresent = instanceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
        newComponentInstance->setIsPresent(isPresent);
    }

    if (docRevision == Document::Revision::Std22)
    {
        Details::parsePowerDomainLinks(instanceNode, newComponentInstance);
    }

    Details::parseExtensions(instanceNode, newComponentInstance);

    return newComponentInstance;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePowerDomainLinks()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parsePowerDomainLinks(QDomNode const& instanceNode,
    QSharedPointer<ComponentInstance> instance)
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
            new ComponentInstance::PowerDomainLink({ externalLink, internalLink }));
        instance->getPowerDomainLinks()->append(link);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseExtensions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseExtensions(const QDomNode& componentInstanceNode,
    QSharedPointer<ComponentInstance> instance)
{
    QDomElement extensionsNode = componentInstanceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomNodeList extensionNodeList = extensionsNode.childNodes();

    if (QDomElement uuidElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:uuid")); 
        !uuidElement.isNull())
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

    CommonItemsReader::parseVendorExtensions(componentInstanceNode, instance);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseDraft()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseDraft(QDomElement const& draftNode,
    QSharedPointer<ComponentInstance> instance)
{
    instance->setDraft(!draftNode.isNull());
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePosition()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parsePosition(QDomElement const& positionElement,
    QSharedPointer<ComponentInstance> instance)
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
void ComponentInstanceReader::Details::parseImport(QDomElement const& importElement,
    QSharedPointer<ComponentInstance> instance)
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
void ComponentInstanceReader::Details::parsePortPositions(QDomElement const& portPositionsElement,
    QSharedPointer<ComponentInstance> instance)
{
    QMap<QString, QPointF> portPositions = createMappedPositions(portPositionsElement, 
        QStringLiteral("kactus2:portPosition"), QStringLiteral("busRef"));

    for (auto portIterator = portPositions.cbegin(); portIterator != portPositions.cend(); ++portIterator)
    {
        instance->updateBusInterfacePosition(portIterator.key(), portIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseAdHocVisibilities()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseAdHocVisibilities(QDomElement const& adHocElement,
    QSharedPointer<ComponentInstance> instance)
{
    QMap<QString, QPointF> adHocPositions = createMappedPositions(adHocElement,
        QStringLiteral("kactus2:adHocVisible"), QStringLiteral("portName"));

    for (auto adHocIterator = adHocPositions.cbegin(); adHocIterator != adHocPositions.cend(); ++adHocIterator)
    {
        instance->updateAdHocPortPosition(adHocIterator.key(), adHocIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseApiInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseApiInterfacePositions(QDomElement const& apiElement,
    QSharedPointer<ComponentInstance> instance)
{
    QMap<QString, QPointF> apiInterfacePositions = createMappedPositions(apiElement,
        QStringLiteral("kactus2:apiInterfacePosition"), QStringLiteral("apiRef"));

    for (auto apiInterfaceIterator = apiInterfacePositions.cbegin(); 
        apiInterfaceIterator != apiInterfacePositions.cend(); ++apiInterfaceIterator)
    {
        instance->updateApiInterfacePosition(apiInterfaceIterator.key(), apiInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseComInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseComInterfacePositions(QDomElement const& comElement,
    QSharedPointer<ComponentInstance> instance)
{
    QMap<QString, QPointF> comInterfacePositions = createMappedPositions(comElement,
        QStringLiteral("kactus2:comInterfacePosition"), QStringLiteral("comRef"));

    for (auto comInterfaceIterator = comInterfacePositions.cbegin(); 
        comInterfaceIterator != comInterfacePositions.cend(); ++comInterfaceIterator)
    {
        instance->updateComInterfacePosition(comInterfaceIterator.key(), comInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parsePropertyValues(QDomElement const& propertyElement,
    QSharedPointer<ComponentInstance> instance)
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
QMap<QString, QPointF> ComponentInstanceReader::Details::createMappedPositions(QDomElement const& positionElement,
    QString const& itemIdentifier, QString const& referenceIdentifier)
{
    QMap<QString, QPointF> positionMap;

    QDomNodeList positionNodeList = positionElement.elementsByTagName(itemIdentifier);
    const int NODE_COUNT = positionNodeList.count();
    for (int positionIndex = 0; positionIndex < NODE_COUNT; ++positionIndex)
    {
        QDomElement currentPosition = positionNodeList.at(positionIndex).toElement();

        QString interfaceReference = currentPosition.attribute(referenceIdentifier);

        if (!currentPosition.hasAttribute(QStringLiteral("x")))
        {
            currentPosition = currentPosition.firstChildElement(QStringLiteral("kactus2:position"));
        }

        int positionX = currentPosition.attribute(QStringLiteral("x")).toInt();
        int positionY = currentPosition.attribute(QStringLiteral("y")).toInt();

        positionMap.insert(interfaceReference, QPointF(positionX, positionY));
    }

    return positionMap;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseFileSetRef()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::Details::parseFileSetRef(QDomElement const& fileSetRefElement,
    QSharedPointer<ComponentInstance> instance)
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
void ComponentInstanceReader::Details::parseMapping(QDomElement const& mappingElement,
    QSharedPointer<ComponentInstance> instance)
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
