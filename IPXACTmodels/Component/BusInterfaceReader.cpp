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

#include "MasterInterface.h"
#include "MirroredSlaveInterface.h"
#include "SlaveInterface.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>

#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/utilities/XmlUtils.h>

#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::createbusinterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusinterfaceReader::createBusinterfaceFrom(QDomNode const& businterfaceNode)
{
	QSharedPointer<BusInterface> newbusinterface (new BusInterface());

	newbusinterface->setAttributes(CommonItemsReader::parseAttributes(businterfaceNode));

    NameGroupReader::parseNameGroup(businterfaceNode, newbusinterface);

    auto presenceElement = businterfaceNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
    newbusinterface->setIsPresent(CommonItemsReader::parseIsPresent(presenceElement));


    QDomElement businterfaceElement = businterfaceNode.toElement();
    
    Details::parseBusType(businterfaceElement, newbusinterface);

    Details::parseAbstractionTypes(businterfaceElement, newbusinterface);

    Details::parseInterfaceMode(businterfaceElement, newbusinterface);

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
        if (bitSteering == QLatin1String("on"))
        {
            newbusinterface->setBitSteering(BusInterface::BITSTEERING_ON);
        }
        else if (bitSteering == QLatin1String("off"))
        {
            newbusinterface->setBitSteering(BusInterface::BITSTEERING_OFF);
        }

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
    QSharedPointer<BusInterface> busInterface)
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
        parsePortMaps(portMapsElement, newAbstractionType);

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
    QSharedPointer<AbstractionType> abstractionType)
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

        QDomElement isPresentElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isPresent"));

        portMap->setIsPresent(isPresentElement.firstChild().nodeValue());


        parseLogicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:logicalPort")), portMap);
        parsePhysicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:physicalPort")), portMap);

        QDomElement logicalTieOffElement = portMapElement.firstChildElement(QStringLiteral("ipxact:logicalTieOff"));

        portMap->setLogicalTieOff(logicalTieOffElement.firstChild().nodeValue());


        QDomElement isInformativeElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isInformative"));

        portMap->setIsInformative(isInformativeElement.firstChild().nodeValue() == QLatin1String("true"));


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

    QDomElement rangeElement = logicalPortElement.firstChildElement(QStringLiteral("ipxact:range"));
    if (!rangeElement.isNull())
    {
        QString leftRange = rangeElement.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString rightRange = rangeElement.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        logicalPort->range_ = QSharedPointer<Range>(new Range(leftRange, rightRange));
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
        // Part select includes range.
        QDomNode rangeNode = partSelectElement.firstChildElement(QStringLiteral("ipxact:range"));
        QString leftRange = rangeNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString rightRange = rangeNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        QSharedPointer<PartSelect> newPartSelect (new PartSelect(leftRange, rightRange));

        // Part select includes indices.
        QDomElement indicesNode = partSelectElement.firstChildElement(QStringLiteral("ipxact:indices"));
        if (!indicesNode.isNull())
        {
            QDomNodeList indexNodes = indicesNode.elementsByTagName(QStringLiteral("ipxact:index"));

            int indexCount = indexNodes.count();
            for (int index = 0; index < indexCount; index++)
            {
                QString indexValue = indexNodes.at(index).firstChild().nodeValue();
                newPartSelect->getIndices()->append(indexValue);
            }
        }

        physicalPort->partSelect_ = newPartSelect;
    }

	portMap->setPhysicalPort(physicalPort);
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseInterfaceMode()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseInterfaceMode(QDomElement const& busInterfaceElement,
    QSharedPointer<BusInterface> newbusinterface)
{
    QDomElement masterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:master"));
	if (!masterNode.isNull())
	{
		QSharedPointer<MasterInterface> newmode(new MasterInterface());
        parseMasterInterface(masterNode, newmode);
        
        newbusinterface->setInterfaceMode(General::MASTER);
		newbusinterface->setMaster(newmode);
        return;
	}

	QDomElement slaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:slave"));
    if (!slaveNode.isNull())
	{
		QSharedPointer<SlaveInterface> newmode(new SlaveInterface());
		parseSlaveInterface(slaveNode, newmode);

        newbusinterface->setInterfaceMode(General::SLAVE);
		newbusinterface->setSlave(newmode);
        return;
	}

    QDomElement systemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:system"));
    if (!systemNode.isNull())
    {
        QString systemGroup = systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::SYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
    }

    QDomElement mirroredMasterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredMaster"));
    if (!mirroredMasterNode.isNull())
    {		
        newbusinterface->setInterfaceMode(General::MIRROREDMASTER);
        return;
    }

    QDomElement mirroredSlaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSlave"));
	if (!mirroredSlaveNode.isNull())
	{
		QSharedPointer<MirroredSlaveInterface> newmode(new MirroredSlaveInterface());
		parseMirroredSlaveInterface(mirroredSlaveNode, newmode);

        newbusinterface->setInterfaceMode(General::MIRROREDSLAVE);
		newbusinterface->setMirroredSlave(newmode);
        return;
	}

    QDomElement mirroredSystemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSystem"));
	if (!mirroredSystemNode.isNull())
	{
        QString systemGroup = mirroredSystemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::MIRROREDSYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
	}

    QDomElement monitorNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:monitor"));
	if (!monitorNode.isNull())
    {
        QSharedPointer<BusInterface::MonitorInterface> newmode(new BusInterface::MonitorInterface());
        parseMonitorInterface(monitorNode, newmode);

        newbusinterface->setInterfaceMode(General::MONITOR);
        newbusinterface->setMonitor(newmode);        
    }
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMasterInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMasterInterface(QDomElement const& masterInterfaceElement,
    QSharedPointer<MasterInterface> masterInterface)
{
    QDomElement addressSpaceRefElement = masterInterfaceElement.firstChildElement(QStringLiteral("ipxact:addressSpaceRef"));
 
    QString addressSpaceRef = addressSpaceRefElement.attribute(QStringLiteral("addressSpaceRef"));

        addressSpaceRef = addressSpaceRefElement.attribute(QStringLiteral("ipxact:addressSpaceRef"));
    

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
    QSharedPointer<SlaveInterface> slaveInterface)
{
    QDomElement memoryMapRefElement = slaveIntefaceElement.firstChildElement(QStringLiteral("ipxact:memoryMapRef"));

        QString memoryMapRef = memoryMapRefElement.attribute(QStringLiteral("memoryMapRef"));
        slaveInterface->setMemoryMapRef(XmlUtils::removeWhiteSpace(memoryMapRef));
    

    QDomNodeList bridgeNodes = slaveIntefaceElement.elementsByTagName(QStringLiteral("ipxact:transparentBridge"));
    int bridgeCount = bridgeNodes.count();
    for (int i = 0; i < bridgeCount; i++)
    {
        QDomElement bridgeElement = bridgeNodes.at(i).toElement();
  
        QSharedPointer<TransparentBridge> newBridge(new TransparentBridge());
        newBridge->setMasterRef(XmlUtils::removeWhiteSpace(bridgeElement.attribute(QStringLiteral("masterRef"))));

        QString bridgePresence = bridgeElement.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
        if (!bridgePresence.isEmpty())
        {
            newBridge->setIsPresent(bridgePresence);
        }

        // Another bridge is extracted.
        slaveInterface->getBridges()->append(newBridge);
    }

    QDomNodeList fileSetRefGroupNodes = slaveIntefaceElement.elementsByTagName(QStringLiteral("ipxact:fileSetRefGroup"));
    int filesetRefCount = fileSetRefGroupNodes.count();
	// Go through all child elements.
    for (int i = 0; i < filesetRefCount; i++)
    {
        QDomElement fileSetRefElement = fileSetRefGroupNodes.at(i).toElement();

        QSharedPointer<SlaveInterface::FileSetRefGroup> fileSetGroup(new SlaveInterface::FileSetRefGroup());

        QDomNode groupNode = fileSetRefElement.firstChildElement(QStringLiteral("ipxact:group"));
        fileSetGroup->group_ = XmlUtils::removeWhiteSpace(groupNode.firstChild().nodeValue());

        QDomNodeList filesetNodes = fileSetRefElement.elementsByTagName(QStringLiteral("ipxact:fileSetRef"));
        int filesetNodeCount = filesetNodes.count();
        for (int j = 0; j < filesetNodeCount; j++)
        {              
              fileSetGroup->fileSetRefs_.append(filesetNodes.at(j).firstChild().nodeValue());
        }

        // Another file set ref group is extracted.
        slaveInterface->getFileSetRefGroup()->append( fileSetGroup );
	}
}

//-----------------------------------------------------------------------------
// Function: BusinterfaceReader::parseMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::Details::parseMirroredSlaveInterface(QDomElement const& mirroredInterfaceElement,
    QSharedPointer<MirroredSlaveInterface> mirroredSlaveInterface)
{
    QDomElement baseAddressElement = mirroredInterfaceElement.firstChildElement(QStringLiteral("ipxact:baseAddresses"));

    if (baseAddressElement.isNull())
    {
        return;
    }

    QDomNodeList remapAddressNodes = baseAddressElement.elementsByTagName(QStringLiteral("ipxact:remapAddress"));
    int remapAddressCount = remapAddressNodes.count();
    for (int i = 0; i < remapAddressCount; i++)
    { 
        QDomNode remapAddressNode = remapAddressNodes.at(i);
        QString remapAddress = remapAddressNode.childNodes().at(0).nodeValue();

        QSharedPointer<MirroredSlaveInterface::RemapAddress> remap(
            new MirroredSlaveInterface::RemapAddress(remapAddress));

        remap->remapAttributes_ = CommonItemsReader::parseAttributes(remapAddressNode);
        remap->state_ = remap->remapAttributes_.take(QStringLiteral("state"));

        mirroredSlaveInterface->getRemapAddresses()->append(remap);	
    }
   
    QDomNode rangeNode = baseAddressElement.firstChildElement(QStringLiteral("ipxact:range"));
    mirroredSlaveInterface->setRange(rangeNode.firstChild().nodeValue() );
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
