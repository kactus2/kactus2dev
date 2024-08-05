//-----------------------------------------------------------------------------
// File: businterfaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------

#include "BusInterfaceReader.h"

#include "InitiatorInterface.h"
#include "MirroredTargetInterface.h"
#include "TargetInterface.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>

#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/utilities/XmlUtils.h>

#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::createbusinterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusinterfaceReader::createBusinterfaceFrom(QDomNode const& businterfaceNode,
    Document::Revision docRevision)
{
	QSharedPointer<BusInterface> newbusinterface (new BusInterface());

	newbusinterface->setAttributes(CommonItemsReader::parseAttributes(businterfaceNode));

    NameGroupReader::parseNameGroup(businterfaceNode, newbusinterface);

    if (docRevision == Document::Revision::Std14)
    {
        auto presenceElement = businterfaceNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
        newbusinterface->setIsPresent(CommonItemsReader::parseIsPresent(presenceElement));
    }

    QDomElement businterfaceElement = businterfaceNode.toElement();
    
    Details::parseBusType(businterfaceElement, newbusinterface);

    Details::parseAbstractionTypes(businterfaceElement, newbusinterface, docRevision);

    Details::parseInterfaceMode(businterfaceElement, newbusinterface, docRevision);

    Details::parseConnectionRequired(businterfaceElement, newbusinterface);

    Details::parseBitsInLau(businterfaceElement, newbusinterface);

    Details::parseBitSteering(businterfaceElement, newbusinterface);

    Details::parseEndianess(businterfaceElement, newbusinterface);

    newbusinterface->getParameters()->append(*CommonItemsReader::parseAndCreateParameters(businterfaceNode));

    Details::parseBusInterfaceExtensions(businterfaceNode, newbusinterface);

    return newbusinterface;
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseBusType()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseBusType(QDomElement& businterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement busTypeElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:busType"));
    newbusinterface->setBusType(CommonItemsReader::parseVLNVAttributes(busTypeElement, VLNV::BUSDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseConnectionRequired()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseConnectionRequired(QDomElement& businterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement requirementNode = businterfaceElement.firstChildElement(QStringLiteral("ipxact:connectionRequired"));
    if (!requirementNode.isNull())
    {
        QString connectionRequired = requirementNode.childNodes().at(0).nodeValue();
        newbusinterface->setConnectionRequired(connectionRequired == QLatin1String("true"));
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseBitsInLau()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseBitsInLau(QDomElement& businterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement lauElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:bitsInLau"));
    if (!lauElement.isNull())
    {
        QString bitsInLau = lauElement.firstChild().nodeValue();
        newbusinterface->setBitsInLau(bitsInLau);
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseBitSteering()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseBitSteering(QDomElement& businterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement bitSteeringElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:bitSteering"));
    if (!bitSteeringElement.isNull())
    {
        QString bitSteering = bitSteeringElement.childNodes().at(0).nodeValue();

        newbusinterface->setBitSteering(bitSteering);

        newbusinterface->setBitSteeringAttributes(CommonItemsReader::parseAttributes(bitSteeringElement));
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseEndianess()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseEndianess(QDomElement& businterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement endiannessElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:endianness"));
    if (!endiannessElement.isNull())
    {
        QString endianness = endiannessElement.childNodes().at(0).nodeValue();
        if (endianness == QLatin1String("little"))
        {
            newbusinterface->setEndianness(BusInterface::LITTLE);
        }
        else if (endianness == QLatin1String("big"))
        {
            newbusinterface->setEndianness(BusInterface::BIG);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::readAbstractionTypes()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseAbstractionTypes(QDomElement const& businterfaceElement,
    QSharedPointer<BusInterface> busInterface, Document::Revision docRevision)
{
    auto abstractionTypesElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:abstractionTypes"));

    auto abstractionNodes = abstractionTypesElement.elementsByTagName(QStringLiteral("ipxact:abstractionType"));

    const int ABSTRACTIONTYPE_COUNT = abstractionNodes.count();
	for (int i = 0; i < ABSTRACTIONTYPE_COUNT; ++i)
    {
        QSharedPointer<AbstractionType> newAbstractionType(new AbstractionType());

        QDomNode abstractionNode = abstractionNodes.at(i);

        parseViewReferences(abstractionNode, newAbstractionType);

        QDomElement abstractionRefElement = abstractionNode.firstChildElement(QStringLiteral("ipxact:abstractionRef"));
        newAbstractionType->setAbstractionRef(CommonItemsReader::parseConfigurableVLNVReference(abstractionRefElement,
            VLNV::ABSTRACTIONDEFINITION));

        QDomElement portMapsElement = abstractionNode.firstChildElement(QStringLiteral("ipxact:portMaps"));
        parsePortMaps(portMapsElement, newAbstractionType, docRevision);

        busInterface->getAbstractionTypes()->append(newAbstractionType);
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceReader::parseAbstractionTypes()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseViewReferences(QDomNode const& abstractionNode,
    QSharedPointer<AbstractionType> newAbstractionType)
{
    QDomElement abstractionElement = abstractionNode.toElement();
    if (!abstractionElement.isNull())
    {
        QSharedPointer<QStringList> viewList(new QStringList());

        QDomNodeList abstractionViewNodes =
            abstractionNode.toElement().elementsByTagName(QStringLiteral("ipxact:viewRef"));

        const int VIEW_COUNT = abstractionViewNodes.count();
        for (int i = 0; i < VIEW_COUNT; ++i)
        {
            QString viewReference = abstractionViewNodes.at(i).firstChild().nodeValue();
            viewList->append(viewReference);
        }

        newAbstractionType->setViewReferences(viewList);
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parsePortMaps()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parsePortMaps(QDomElement const& portMapsElement,
    QSharedPointer<AbstractionType> abstractionType, Document::Revision docRevision)
{
    QDomNodeList portMapNodes = portMapsElement.elementsByTagName(QStringLiteral("ipxact:portMap"));

    int portMapCount = portMapNodes.count();
    for (int i = 0; i < portMapCount; i++)
    {
        QDomElement portMapElement = portMapNodes.at(i).toElement();

        QSharedPointer<PortMap> portMap (new PortMap());

        if (portMapElement.hasAttribute(QStringLiteral("invert")))
        {
            portMap->setInvert(portMapElement.attribute(QStringLiteral("invert")) == QLatin1String("true"));
        }

        if (docRevision == Document::Revision::Std14)
        {
            QDomElement isPresentElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isPresent"));
            portMap->setIsPresent(isPresentElement.firstChild().nodeValue());
        }

        parseLogicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:logicalPort")), portMap);
        parsePhysicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:physicalPort")), portMap);

        QDomElement logicalTieOffElement = portMapElement.firstChildElement(QStringLiteral("ipxact:logicalTieOff"));

        portMap->setLogicalTieOff(logicalTieOffElement.firstChild().nodeValue());

        if (QDomElement isInformativeElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isInformative"));
            isInformativeElement.firstChild().nodeValue().isEmpty())
        {
            portMap->clearIsInformative(); // Set unspecified value if value doesn't exist.
        }
        else
        {
            portMap->setIsInformative(isInformativeElement.firstChild().nodeValue() == QLatin1String("true"));
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(portMapElement, portMap);
        }

        abstractionType->getPortMaps()->append(portMap);
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseLogicalPort()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseLogicalPort(QDomElement const& logicalPortElement,
    QSharedPointer<PortMap> portMap)
{
    QDomElement nameElement = logicalPortElement.firstChildElement(QStringLiteral("ipxact:name"));
    QString portName = XmlUtils::removeWhiteSpace(nameElement.firstChild().nodeValue());

    QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort(portName));

    if (QDomElement rangeElement = logicalPortElement.firstChildElement(QStringLiteral("ipxact:range")); 
        !rangeElement.isNull())
    {
        logicalPort->range_ = QSharedPointer<Range>(new Range(CommonItemsReader::parseRange(rangeElement)));
    }

	portMap->setLogicalPort(logicalPort);
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parsePhysicalPort()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parsePhysicalPort(QDomElement const& physicalPortElement,
    QSharedPointer<PortMap> portMap)
{
    if (physicalPortElement.isNull())
    {
        return;
    }

    QDomElement nameElement = physicalPortElement.firstChildElement(QStringLiteral("ipxact:name"));
    QString portName = XmlUtils::removeWhiteSpace(nameElement.firstChild().nodeValue());
	QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort(portName));

    QDomElement partSelectElement = physicalPortElement.firstChildElement(QStringLiteral("ipxact:partSelect"));
    if (!partSelectElement.isNull())
    {
        QSharedPointer<PartSelect> newPartSelect = CommonItemsReader::parsePartSelect(partSelectElement);
        physicalPort->partSelect_ = newPartSelect;
    }

	portMap->setPhysicalPort(physicalPort);
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseInterfaceMode()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseInterfaceMode(QDomElement const& busInterfaceElement,
    QSharedPointer<BusInterface> newbusinterface, Document::Revision docRevision)
{
    if (docRevision == Document::Revision::Std14)
    {
        QDomElement masterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:master"));
        if (!masterNode.isNull())
        {
            QSharedPointer<InitiatorInterface> newmode(new InitiatorInterface());
            parseMasterInterface(masterNode, newmode);

            newbusinterface->setMaster(newmode);
            return;
        }

        QDomElement slaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:slave"));
        if (!slaveNode.isNull())
        {
            QSharedPointer<TargetInterface> newmode(new TargetInterface());
            parseSlaveInterface(slaveNode, newmode);

            newbusinterface->setSlave(newmode);
            return;
        }

        QDomElement mirroredMasterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredMaster"));
        if (!mirroredMasterNode.isNull())
        {
            newbusinterface->setInterfaceMode(General::MIRRORED_MASTER);
            return;
        }

        QDomElement mirroredSlaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSlave"));
        if (!mirroredSlaveNode.isNull())
        {
            QSharedPointer<MirroredTargetInterface> newmode(new MirroredTargetInterface());
            parseMirroredSlaveInterface(mirroredSlaveNode, newmode);

            newbusinterface->setMirroredSlave(newmode);
            return;
        }
    }
    else if (docRevision == Document::Revision::Std22)
    {
        QDomElement initiatorNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:initiator"));
        if (!initiatorNode.isNull())
        {
            QSharedPointer<InitiatorInterface> newmode(new InitiatorInterface());
            parseInitiatorInterface(initiatorNode, newmode);

            newbusinterface->setInitiator(newmode);
            return;
        }

        QDomElement targetNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:target"));
        if (!targetNode.isNull())
        {
            QSharedPointer<TargetInterface> newmode(new TargetInterface());
            parseTargetInterface(targetNode, newmode, Document::Revision::Std22);

            newbusinterface->setTarget(newmode);
            return;
        }

        QDomElement mirroredTargetNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredTarget"));
        if (!mirroredTargetNode.isNull())
        {
            QSharedPointer<MirroredTargetInterface> newmode(new MirroredTargetInterface());
            parseMirroredTargetInterface(mirroredTargetNode, newmode);

            newbusinterface->setMirroredTarget(newmode);
            return;
        }

        QDomElement mirroredInitiatorNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredInitiator"));
        if (!mirroredInitiatorNode.isNull())
        {
            newbusinterface->setInterfaceMode(General::MIRRORED_INITIATOR);
            return;
        }
    }

    QDomElement systemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:system"));
    if (!systemNode.isNull())
    {
        QString systemGroup = systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::SYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
    }


    QDomElement mirroredSystemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSystem"));
	if (!mirroredSystemNode.isNull())
	{
        QString systemGroup = mirroredSystemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::MIRRORED_SYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
	}

    QDomElement monitorNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:monitor"));
	if (!monitorNode.isNull())
    {
        QSharedPointer<BusInterface::MonitorInterface> newmode(new BusInterface::MonitorInterface());
        parseMonitorInterface(monitorNode, newmode);

        newbusinterface->setMonitor(newmode);        
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMasterInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMasterInterface(QDomElement const& masterInterfaceElement,
    QSharedPointer<InitiatorInterface> masterInterface)
{
    QDomElement addressSpaceRefElement = masterInterfaceElement.firstChildElement(QStringLiteral("ipxact:addressSpaceRef"));
 
    QString addressSpaceRef = addressSpaceRefElement.attribute(QStringLiteral("addressSpaceRef"));

    masterInterface->setAddressSpaceRef(XmlUtils::removeWhiteSpace(addressSpaceRef));

    QDomElement isPresentElement = addressSpaceRefElement.firstChildElement(QStringLiteral("ipxact:isPresent"));
    if (!isPresentElement.isNull())
    {
        masterInterface->setIsPresent(isPresentElement.firstChild().nodeValue());
    }

    QDomElement baseAddressElement = addressSpaceRefElement.firstChildElement(QStringLiteral("ipxact:baseAddress"));

    QString baseAddress = baseAddressElement.firstChild().nodeValue();
    masterInterface->setBaseAddress(XmlUtils::removeWhiteSpace(baseAddress));
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseSlaveInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseSlaveInterface(QDomElement const& slaveIntefaceElement,
    QSharedPointer<TargetInterface> slaveInterface)
{
    parseTargetInterface(slaveIntefaceElement, slaveInterface, Document::Revision::Std14);
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMirroredSlaveInterface(QDomElement const& mirroredInterfaceElement,
    QSharedPointer<MirroredTargetInterface> mirroredSlaveInterface)
{
    QDomElement baseAddressElement = mirroredInterfaceElement.firstChildElement(QStringLiteral("ipxact:baseAddresses"));

    if (baseAddressElement.isNull())
    {
        return;
    }

    QDomNodeList remapAddressNodes = baseAddressElement.elementsByTagName(QStringLiteral("ipxact:remapAddress"));
    const int REMAP_ADDRESSES_COUNT = remapAddressNodes.count();
    for (int i = 0; i < REMAP_ADDRESSES_COUNT; ++i)
    {
        QDomNode remapAddressNode = remapAddressNodes.at(i);
        QString remapAddress = remapAddressNode.firstChild().nodeValue();

        QSharedPointer<MirroredTargetInterface::RemapAddress> remap(
            new MirroredTargetInterface::RemapAddress(remapAddress));

        remap->remapAttributes_ = CommonItemsReader::parseAttributes(remapAddressNode);
        remap->state_ = remap->remapAttributes_.take(QStringLiteral("state"));

        mirroredSlaveInterface->getRemapAddresses()->append(remap);
    }

    QDomNode rangeNode = baseAddressElement.firstChildElement(QStringLiteral("ipxact:range"));
    mirroredSlaveInterface->setRange(rangeNode.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseInitiatorInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseInitiatorInterface(QDomElement const& initiatorNode, 
    QSharedPointer<InitiatorInterface> initiatorInterface)
{
    QDomElement addressSpaceRefElement = initiatorNode.firstChildElement(QStringLiteral("ipxact:addressSpaceRef"));

    QString addressSpaceRef = addressSpaceRefElement.attribute(QStringLiteral("addressSpaceRef"));

    initiatorInterface->setAddressSpaceRef(XmlUtils::removeWhiteSpace(addressSpaceRef));

    CommonItemsReader::parseVendorExtensions(addressSpaceRefElement, initiatorInterface);

    QDomElement baseAddressElement = addressSpaceRefElement.firstChildElement(QStringLiteral("ipxact:baseAddress"));

    QString baseAddress = baseAddressElement.firstChild().nodeValue();
    initiatorInterface->setBaseAddress(XmlUtils::removeWhiteSpace(baseAddress));

    initiatorInterface->setModeRefs(parseModeRefs(addressSpaceRefElement));
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseTargetInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseTargetInterface(QDomElement const& targetNode,
    QSharedPointer<TargetInterface> targetInterface, Document::Revision docRevision)
{
    QDomElement memoryMapRefElement = targetNode.firstChildElement(QStringLiteral("ipxact:memoryMapRef"));

    QString memoryMapRef = memoryMapRefElement.attribute(QStringLiteral("memoryMapRef"));
    targetInterface->setMemoryMapRef(XmlUtils::removeWhiteSpace(memoryMapRef));

    targetInterface->setModeRefs(parseModeRefs(memoryMapRefElement));

    QDomNodeList bridgeNodes = targetNode.elementsByTagName(QStringLiteral("ipxact:transparentBridge"));
    int bridgeCount = bridgeNodes.count();
    for (int i = 0; i < bridgeCount; i++)
    {
        QDomElement bridgeElement = bridgeNodes.at(i).toElement();

        QSharedPointer<TransparentBridge> newBridge(new TransparentBridge());

        auto attributeName = QStringLiteral("initiatorRef");
        if (docRevision == Document::Revision::Std14)
        {
            attributeName = QStringLiteral("masterRef");
        }

        newBridge->setInitiatorRef(XmlUtils::removeWhiteSpace(bridgeElement.attribute(attributeName)));

        if (docRevision == Document::Revision::Std14)
        {
            newBridge->setIsPresent(CommonItemsReader::parseIsPresent(bridgeElement.firstChildElement(
                QStringLiteral("ipxact:isPresent"))));
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(bridgeElement, newBridge);
        }

        // Another bridge is extracted.
        targetInterface->getBridges()->append(newBridge);
    }

    QDomNodeList fileSetRefGroupNodes = targetNode.elementsByTagName(QStringLiteral("ipxact:fileSetRefGroup"));
    int filesetRefCount = fileSetRefGroupNodes.count();
    // Go through all child elements.
    for (int i = 0; i < filesetRefCount; i++)
    {
        QDomElement fileSetRefElement = fileSetRefGroupNodes.at(i).toElement();

        QSharedPointer<TargetInterface::FileSetRefGroup> fileSetGroup(new TargetInterface::FileSetRefGroup());

        QDomNode groupNode = fileSetRefElement.firstChildElement(QStringLiteral("ipxact:group"));
        fileSetGroup->group_ = XmlUtils::removeWhiteSpace(groupNode.firstChild().nodeValue());

        if (auto fileSetRefs = CommonItemsReader::parseFileSetReferences(fileSetRefElement, docRevision);
            fileSetRefs->isEmpty() == false)
        {
            fileSetGroup->fileSetRefs_ = fileSetRefs;
        }

        // Another file set ref group is extracted.
        targetInterface->getFileSetRefGroup()->append(fileSetGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMirroredTargetInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMirroredTargetInterface(QDomElement const& mirroredInterfaceElement,
    QSharedPointer<MirroredTargetInterface> mirroredSlaveInterface)
{
    QDomElement baseAddressElement = mirroredInterfaceElement.firstChildElement(QStringLiteral("ipxact:baseAddresses"));

    if (baseAddressElement.isNull())
    {
        return;
    }


    QDomNodeList remapAddressesNodes = baseAddressElement.elementsByTagName(QStringLiteral("ipxact:remapAddresses"));
    const int REMAP_ADDRESS_COUNT = remapAddressesNodes.count();
    for (int i = 0; i < REMAP_ADDRESS_COUNT; ++i)
    {
        QDomElement remapAddressesElement = remapAddressesNodes.at(i).toElement();

        QDomNode remapAddressNode = remapAddressesElement.firstChildElement(QStringLiteral("ipxact:remapAddress"));
        QString remapAddress = remapAddressNode.firstChild().nodeValue();

        QSharedPointer<MirroredTargetInterface::RemapAddress> remap(
            new MirroredTargetInterface::RemapAddress(remapAddress));

        remap->modeRefs_ = parseModeRefs(remapAddressesElement);

        auto modeRefNodes = remapAddressesElement.elementsByTagName(QStringLiteral("ipxact:modeRef"));
        const int MODE_COUNT = modeRefNodes.count();
        for (int j = 0; j < MODE_COUNT; ++j)
        {
            auto modeNode = modeRefNodes.at(j);
            
            auto priority = modeNode.attributes().namedItem(QStringLiteral("priority")).nodeValue();
            if (priority.isEmpty() == false)
            {
                auto modeRef = modeNode.firstChild().nodeValue();
                remap->priorities_.insert(modeRef, priority.toUInt());
            }
        }


        mirroredSlaveInterface->getRemapAddresses()->append(remap);
    }

    QDomNode rangeNode = baseAddressElement.firstChildElement(QStringLiteral("ipxact:range"));
    mirroredSlaveInterface->setRange(rangeNode.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMonitorInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMonitorInterface(QDomElement const& monitorElement,
	QSharedPointer<BusInterface::MonitorInterface> monitorInterface)
{
	QString interfaceMode = monitorElement.attribute(QStringLiteral("interfaceMode"));
	interfaceMode = XmlUtils::removeWhiteSpace(interfaceMode);

	monitorInterface->interfaceMode_ = General::str2Interfacemode(interfaceMode, General::MONITOR);
    monitorInterface->group_ = monitorElement.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseBusInterfaceExtensions()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseBusInterfaceExtensions(QDomNode const& interfaceNode,
    QSharedPointer<BusInterface> newInterface)
{
    QDomNode extensionsNode = interfaceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomElement positionElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:position"));
    if (!positionElement.isNull())
    {
        newInterface->setDefaultPos(CommonItemsReader::parsePoint(positionElement));
    }

    CommonItemsReader::parseVendorExtensions(interfaceNode, newInterface);
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::Details::parseModeRefs()
//-----------------------------------------------------------------------------
QStringList BusinterfaceReader::Details::parseModeRefs(QDomElement const& containingElement)
{
    QStringList modeRefs;
    auto modeRefNodes = containingElement.elementsByTagName(QStringLiteral("ipxact:modeRef"));

    const int MODE_COUNT = modeRefNodes.count();
    for (int i = 0; i < MODE_COUNT; ++i)
    {
        auto modeNode = modeRefNodes.at(i);

        modeRefs.append(modeNode.firstChild().nodeValue());
    }

    return modeRefs;
}
