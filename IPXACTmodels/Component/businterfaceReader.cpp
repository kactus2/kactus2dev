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

#include "businterfaceReader.h"
#include "../masterinterface.h"
#include "../XmlUtils.h"
#include "../masterinterface.h"
#include "../mirroredslaveinterface.h"
#include "../slaveinterface.h"
#include <IPXACTmodels/common/NameGroupReader.h>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>
#include <IPXACTmodels/common/ParameterReader.h>

//-----------------------------------------------------------------------------
// Function: businterfaceReader::businterfaceReader()
//-----------------------------------------------------------------------------
businterfaceReader::businterfaceReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::~businterfaceReader()
//-----------------------------------------------------------------------------
businterfaceReader::~businterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::createbusinterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> businterfaceReader::createbusinterfaceFrom(QDomNode const& businterfaceNode) const
{
	// Create the new bus interface.
	QSharedPointer<BusInterface> newbusinterface (new BusInterface());

	// Get the attributes for the bus interface.
	QMap<QString, QString> attributes;

	QDomNamedNodeMap attributeMap = businterfaceNode.attributes();
	for (int j = 0; j < attributeMap.size(); ++j)
	{
		QString name = attributeMap.item(j).nodeName();
		QString value = attributeMap.item(j).nodeValue();
		attributes[name] = value;
	}

	// Set the attributes for the bus interface.
	newbusinterface->setAttributes( attributes );

	// Parse presence, name group, and vendor extensions with pre-existing parsers.
	parseIsPresent(businterfaceNode, newbusinterface);
	parseNameGroup(businterfaceNode, newbusinterface);

	// All abstraction types will be added here.
    QSharedPointer<QList<QSharedPointer<BusInterface::AbstractionType> > > abstractionTypes
        (new QList<QSharedPointer<BusInterface::AbstractionType> > ());

	// Go through all the child nodes and call appropriate constructors.
	QDomNodeList children = businterfaceNode.childNodes();

	for (int i = 0; i < children.size(); ++i)
	{
		// If node is a comment, then skip it.
		if ( children.at(i).isComment() )
		{
			continue;
		}

		// Try to extract the interface mode from the child node.
		readInterfaceMode(children.at(i), newbusinterface);

		// Extract abstraction types from the child node.
		if (children.at(i).nodeName() == QString("ipxact:abstractionTypes"))
		{
			readAbstractionTypes(children.at(i), abstractionTypes);
		}

		// Get the bus type.
		else if (children.at(i).nodeName() == QString("ipxact:busType"))
		{
			QDomNode busTypeNode = children.at(i);

			// Create the vlnv and set a pointer for it.
            newbusinterface->setBusType(parseVLNVAttributes(busTypeNode, VLNV::BUSDEFINITION));
		}

		// Get bool connectionRequired.
		else if (children.at(i).nodeName() == QString("ipxact:connectionRequired"))
		{
			// If true is found then it is set but false is the default value.
			QString connection = children.at(i).childNodes().at(0).nodeValue();
			newbusinterface->setConnectionRequired( General::str2Bool(connection, false) );
		}

		// Get bits in lau.
		else if (children.at(i).nodeName() == QString("ipxact:bitsInLau"))
		{
            QString bitsInLau = children.at(i).firstChild().nodeValue();
            newbusinterface->setBitsInLau(bitsInLau);
		}

		// Get the bit steering.
		else if (children.at(i).nodeName() == QString("ipxact:bitSteering"))
		{
			QString bitSteering = children.at(i).childNodes().at(0).nodeValue();
			newbusinterface->setBitSteering( General::str2BitSteering(bitSteering) );

			// Get the attributes for bitSteering element.
			QDomNode tempNode = children.at(i);
			newbusinterface->setBitSteeringAttributes( XmlUtils::parseAttributes(tempNode) );
		}

		// Get the endianness.
		else if (children.at(i).nodeName() == QString("ipxact:endianness"))
		{
			newbusinterface->setEndianness( General::str2Endianness(children.at(i).childNodes().
				at(0).nodeValue(), General::LITTLE) );
		}

		// Get the parameters.
		else if (children.at(i).nodeName() == QString("ipxact:parameters"))
		{
			ParameterReader reader;
			QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = newbusinterface->getParameters();

			// Go through all parameters.
			for (int j = 0; j < children.at(i).childNodes().count(); ++j)
			{
				QDomNode parameterNode = children.at(i).childNodes().at(j);
				parameters->append(reader.createParameterFrom(parameterNode));
			}
		}
	}

    readBusInterfaceExtensions(businterfaceNode, newbusinterface);

	newbusinterface->setAbstractionTypes(abstractionTypes);

    return newbusinterface;
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::parseIsPresent()
//-----------------------------------------------------------------------------
void businterfaceReader::parseIsPresent(QDomNode const& businterfaceNode, QSharedPointer<BusInterface> newbusinterface) const
{
	QString newIsPresent = businterfaceNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();
	if (!newIsPresent.isEmpty())
	{
		newbusinterface->setIsPresent(newIsPresent);
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::parseNameGroup()
//-----------------------------------------------------------------------------
void businterfaceReader::parseNameGroup(QDomNode const& businterfaceNode,
	QSharedPointer<BusInterface> newbusinterface) const
{
	NameGroupReader nameReader;
	nameReader.parseNameGroup(businterfaceNode, newbusinterface);
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readAbstractionTypes()
//-----------------------------------------------------------------------------
void businterfaceReader::readAbstractionTypes(QDomNode& inspected,
    QSharedPointer<QList<QSharedPointer<BusInterface::AbstractionType> > > abstractionTypes) const
{
	for (int j = 0; j < inspected.childNodes().count(); ++j)
	{
		QDomNode abstractionNode = inspected.childNodes().at(j);

		// Search for Kactus2 extensions. Others are culled by another function.
		if (abstractionNode.nodeName() == "ipxact:abstractionType")
		{
			QSharedPointer<BusInterface::AbstractionType> newAbstractionType(new BusInterface::AbstractionType());
            abstractionTypes->append(newAbstractionType);

			for (int k = 0; k < abstractionNode.childNodes().count(); ++k)
			{
				QDomNode childNode = abstractionNode.childNodes().at(k);

				// Get view reference
				if (childNode.nodeName() == QString("ipxact:viewRef"))
				{
					QString viewRef = childNode.firstChild().nodeValue();
					newAbstractionType->viewRef_ = XmlUtils::removeWhiteSpace(viewRef);
				}

				// Get abstraction reference.
				if (childNode.nodeName() == QString("ipxact:abstractionRef"))
				{
					newAbstractionType->abstractionRef_ =
						parseConfigurableVLNVReference(childNode,VLNV::ABSTRACTIONDEFINITION);
				}

				// Get port maps.
				else if (childNode.nodeName() == QString("ipxact:portMaps"))
				{
					readPortMaps(childNode, newAbstractionType);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readPortMaps()
//-----------------------------------------------------------------------------
void businterfaceReader::readPortMaps(QDomNode &inspected,
    QSharedPointer<BusInterface::AbstractionType> newAbstractionType) const
{
	// Call constructors for all port map items.
	for (int l = 0; l < inspected.childNodes().count(); ++l)
	{
		// Skip comments.
		if (inspected.childNodes().at(l).isComment())
		{
			continue;
		}

		// Call constructor and give the child node representing the single choice as parameter.
		QDomNode portNode = inspected.childNodes().at(l);

        QSharedPointer<PortMap> newPort (new PortMap());
        newAbstractionType->portMaps_->append(newPort);

		for (int i = 0; i < portNode.childNodes().count(); ++i)
		{
			QDomNode tempNode = portNode.childNodes().at(i);

			if (tempNode.nodeName() == QString("ipxact:logicalPort"))
			{
				readLogicalPort(tempNode, newPort);
			}
			else if (tempNode.nodeName() == QString("ipxact:physicalPort"))
			{
				readPhysicalPort(tempNode, newPort);
			}
			else if (tempNode.nodeName() == QString("ipxact:logicalTieOff"))
			{
				newPort->setLogicalTieOff( tempNode.firstChild().nodeValue() );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readLogicalPort()
//-----------------------------------------------------------------------------
void businterfaceReader::readLogicalPort(QDomNode& tempNode, QSharedPointer<PortMap> newPort) const
{
	QSharedPointer<PortMap::LogicalPort> newLogPort( new PortMap::LogicalPort() );

	// search childNodes for name and vector elements
	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:name"))
		{
			// get the logical name and strip whitespace characters
			QString logicalPort = tempNode.childNodes().at(j).childNodes().at(0).nodeValue();
			newLogPort->name_ = XmlUtils::removeWhiteSpace(logicalPort);
		}
		else if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:range"))
		{
			QString leftRange = tempNode.childNodes().at(j).firstChildElement("ipxact:left").firstChild().nodeValue();
			QString rightRange = tempNode.childNodes().at(j).firstChildElement("ipxact:right").firstChild().nodeValue();

			newLogPort->range_ = QSharedPointer<Range>(new Range(leftRange, rightRange));
		}
	}

	// Use the created logical port.
	newPort->setLogicalPort( newLogPort );
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readPhysicalPort()
//-----------------------------------------------------------------------------
void businterfaceReader::readPhysicalPort(QDomNode& tempNode, QSharedPointer<PortMap> newPort) const
{
	QSharedPointer<PortMap::PhysicalPort> newPhysPort( new PortMap::PhysicalPort() );

	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:name"))
		{
			// Get the name and strip whitespace characters.
			QString logicalPort = tempNode.childNodes().at(j).childNodes().at(0).nodeValue();
			newPhysPort->name_ = XmlUtils::removeWhiteSpace(logicalPort);
		}
		else if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:partSelect"))
		{
			// Part select includes range.
			QDomNode rangeNode = tempNode.childNodes().at(j).firstChildElement("ipxact:range");

			QString leftRange = rangeNode.firstChildElement("ipxact:left").firstChild().nodeValue();
			QString rightRange = rangeNode.firstChildElement("ipxact:right").firstChild().nodeValue();

			QSharedPointer<PartSelect> newPartSelect (new PartSelect(leftRange, rightRange));

			// Part select includes indices.
			QDomElement indicesNode = tempNode.childNodes().at(j).firstChildElement("ipxact:indices");

			if (!indicesNode.isNull())
			{
				QDomNodeList indexNodes = indicesNode.elementsByTagName("ipxact:index");
				for (int index = 0; index < indexNodes.size(); ++index)
				{
					QDomNode singleIndexNode = indexNodes.at(index);
					QString indexValue = singleIndexNode.firstChild().nodeValue();

					newPartSelect->getIndices()->append(indexValue);
				}
			}

			// Use the created part select.
			newPhysPort->partSelect_ = newPartSelect;
		}
	}

	// Use the created physical port.
	newPort->setPhysicalPort( newPhysPort );
}



void businterfaceReader::readInterfaceMode(QDomNode& inspected, QSharedPointer<BusInterface> newbusinterface) const
{
	// Master is the interface mode.
	if (inspected.nodeName() == QString("ipxact:master"))
	{
		newbusinterface->setInterfaceMode( General::MASTER );

		QSharedPointer<MasterInterface> newmode(new MasterInterface());

		readMasterInterface(inspected, newmode);

		newbusinterface->setMaster( newmode );
	}

	// Slave is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:slave"))
	{
		newbusinterface->setInterfaceMode( General::SLAVE );

		QSharedPointer<SlaveInterface> newmode(new SlaveInterface());

		readSlaveInterface(inspected, newmode);

		newbusinterface->setSlave( newmode );
	}

	// System is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:system"))
	{
		newbusinterface->setInterfaceMode( General::SYSTEM );

		// Group is the known child node for system.
		for (int j = 0; j < inspected.childNodes().count(); ++j)
		{
			if (inspected.childNodes().at(j).nodeName() == QString("ipxact:group"))
			{
				QString tsystem = inspected.childNodes().at(j).childNodes().at(0).nodeValue();
				tsystem = XmlUtils::removeWhiteSpace(tsystem);

				newbusinterface->setSystem(tsystem);
			}
		}
	}

	// MirroredSlave is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:mirroredSlave"))
	{
		newbusinterface->setInterfaceMode( General::MIRROREDSLAVE );

		QSharedPointer<MirroredSlaveInterface> newmode(new MirroredSlaveInterface());

		readMirroredSlaveInterface(inspected, newmode);

		newbusinterface->setMirroredSlave( newmode );
	}

	// MirroredMaster is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:mirroredMaster"))
	{
		newbusinterface->setInterfaceMode( General::MIRROREDMASTER );

		QSharedPointer<MasterInterface> newmode(new MasterInterface());

		readMasterInterface(inspected, newmode);

		newbusinterface->setMaster( newmode );
	}

	// MirroredSystem is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:mirroredSystem"))
	{
		newbusinterface->setInterfaceMode( General::MIRROREDSYSTEM );

		// Group is the known child node for mirrored system.
		for (int j = 0; j < inspected.childNodes().count(); ++j)
		{
			if (inspected.childNodes().at(j).nodeName() == QString("ipxact:group"))
			{
				QString tsystem = inspected.childNodes().at(j).childNodes().at(0).nodeValue();
				tsystem = XmlUtils::removeWhiteSpace(tsystem);

				newbusinterface->setSystem(tsystem);
			}
		}
	}

	// Monitor is the interface mode.
	else if (inspected.nodeName() == QString("ipxact:monitor"))
	{
		newbusinterface->setInterfaceMode( General::MONITOR );

		QSharedPointer<BusInterface::MonitorInterface> newmode(new BusInterface::MonitorInterface());

		readMonitorInterface(inspected, newmode);

		newbusinterface->setMonitor( newmode );
	}
}

void businterfaceReader::readMasterInterface(QDomNode &inspected, QSharedPointer<MasterInterface> newmode) const
{
	// Go through all child items.
	for (int i = 0; i < inspected.childNodes().count(); ++i)
	{
		QDomNode tempNode = inspected.childNodes().at(i);

		if (tempNode.nodeName() == QString("ipxact:addressSpaceRef"))
		{
			// Get the addressSpaceRef attribute.
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString addressSpaceRef = attributeMap.namedItem(QString(
				"ipxact:addressSpaceRef")).nodeValue();
			newmode->setAddressSpaceRef( XmlUtils::removeWhiteSpace(addressSpaceRef) );

			// Search the baseAddress element.
			for (int j = 0; j < tempNode.childNodes().count(); ++j)
			{
				if (tempNode.childNodes().at(j).nodeName() == QString(
					"ipxact:baseAddress"))
				{
					tempNode = tempNode.childNodes().at(j);
					QString baseAddress = tempNode.childNodes().at(0).nodeValue();
					newmode->setBaseAddress( XmlUtils::removeWhiteSpace(baseAddress) );
					break;
				}
			}

			// Get the base address attributes.
			QMap<QString, QString> baseAttributes = XmlUtils::parseAttributes(tempNode);

			// If it contains the optional attribute prompt then set the prompt value in prompt_
			// and remove the attribute from the QMap to avoid duplicate data.
			QMap<QString, QString>::iterator i = baseAttributes.find(QString("ipxact:prompt"));
			if (i != baseAttributes.end())
			{
				// Save the prompt value.
				newmode->setPrompt( i.value() );

				// Remove from map.
				baseAttributes.erase(i);
			}

			// Finally, set the extracted base attributes as field for the interface.
			newmode->setBaseAttributes( baseAttributes );
		}
	}
}

void businterfaceReader::readSlaveInterface(QDomNode &inspected, QSharedPointer<SlaveInterface> newmode) const
{
	// List of extracted bridges.
	QList<QSharedPointer<SlaveInterface::Bridge> > bridges;
	QList<QSharedPointer<SlaveInterface::FileSetRefGroup> > fileSetRefGroup;

	// Go through all child elements.
	for (int i = 0; i < inspected.childNodes().count(); ++i)
	{
		QDomNode tempNode = inspected.childNodes().at(i);

		if (tempNode.nodeName() == QString("ipxact:memoryMapRef"))
		{
			// The wanted value is an attribute.
			QDomNamedNodeMap attributeMap = tempNode.attributes();

			// Strip whitespace characters.
			QString memoryMapRef= attributeMap.namedItem(QString(
				"memoryMapRef")).nodeValue();
			newmode->setMemoryMapRef( XmlUtils::removeWhiteSpace(memoryMapRef) );
		}

		else if (tempNode.nodeName() == QString("ipxact:transparentBridge"))
		{
			QSharedPointer<SlaveInterface::Bridge> newBridge(new SlaveInterface::Bridge());

			QDomNamedNodeMap attributeMap = tempNode.attributes();
			newBridge->masterRef_ = attributeMap.namedItem(QString("masterRef")).
				nodeValue();
			newBridge->masterRef_ = XmlUtils::removeWhiteSpace(newBridge->masterRef_);

			QString newIsPresent = tempNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();

			if (!newIsPresent.isEmpty())
			{
				newBridge->isPresent_ = newIsPresent;
			}

			// Another bridge is extracted.
			bridges.append(newBridge);
		}

		else if (tempNode.nodeName() == QString("ipxact:fileSetRefGroup"))
		{
			QSharedPointer<SlaveInterface::FileSetRefGroup> newFSG(new SlaveInterface::FileSetRefGroup());

			// Go through child nodes.
			for (int i = 0; i < tempNode.childNodes().count(); ++i)
			{
				QDomNode subTempNode = tempNode.childNodes().at(i);

				if (subTempNode.nodeName() == QString("ipxact:group"))
				{
					// Strip whitespace characters.
					newFSG->group_ = subTempNode.childNodes().at(0).nodeValue();
					newFSG->group_ = XmlUtils::removeWhiteSpace(newFSG->group_);
				}

				else if (subTempNode.nodeName() == QString("ipxact:fileSetRef"))
				{
					// Strip whitespace characters.
					QString temp = subTempNode.childNodes().at(0).nodeValue();
					temp = XmlUtils::removeWhiteSpace(temp);
					newFSG->fileSetRefs_.append(temp);
				}
			}

			// Another file set ref group is extracted.
			fileSetRefGroup.append( newFSG );
		}
	}

	// Finally, set the extracted briedges and fileset ref groups as fields for the interface.
	newmode->setBridges( bridges );
	newmode->setFileSetRefGroup( fileSetRefGroup );
}

void businterfaceReader::readMirroredSlaveInterface(QDomNode &inspected, QSharedPointer<MirroredSlaveInterface> newmode) const
{
	QDomNode baseAddrNode;
	bool baseAddressFound = false;

	// Find the ipxact:baseAddresses node and start the parsing from it.
	for (int i = 0; i < inspected.childNodes().count(); ++i)
	{
		if (inspected.childNodes().at(i).nodeName() == QString("ipxact:baseAddresses"))
		{
			baseAddrNode = inspected.childNodes().at(i);
			baseAddressFound = true;
		}
	}
	// If the optional ipxact:baseAddresses node was not found theres nothing to parse.
	if (!baseAddressFound)
	{
		return;
	}

	QList<QSharedPointer<MirroredSlaveInterface::RemapAddress> > readdrs;

	for (int i = 0; i < baseAddrNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = baseAddrNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("ipxact:remapAddress"))
		{
			QSharedPointer<MirroredSlaveInterface::RemapAddress> newremap(new MirroredSlaveInterface::RemapAddress());

			newremap->remapAddress_ = tempNode.childNodes().at(0).nodeValue();

			// Get the attributes.
			newremap->remapAttributes_ = XmlUtils::parseAttributes(tempNode);

			QMap<QString, QString>::iterator i = newremap->remapAttributes_.find(QString("state"));

			if (i != newremap->remapAttributes_.end())
			{
				// Save the attribute value and remove it from QMap to avoid duplicates.
				newremap->state_ = i.value();
				newremap->remapAttributes_.erase(i);
			}

			// This is a remap address.
			readdrs.append( newremap );
		}
		else if (tempNode.nodeName() == QString("ipxact:range"))
		{
			// Get range and its associated attributes.
			newmode->setRange( tempNode.childNodes().at(0).nodeValue() );
		}
	}

	newmode->setRemapAddresses( readdrs );
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::createMonitorInterface()
//-----------------------------------------------------------------------------
void businterfaceReader::readMonitorInterface(QDomNode& inpstected,
	QSharedPointer<BusInterface::MonitorInterface> newmode) const
{
	// Get the interfaceMode attribute.
	QDomNamedNodeMap attributeMap = inpstected.attributes();
	QString interfaceMode = attributeMap.namedItem(QString("interfaceMode")).nodeValue();
	interfaceMode = XmlUtils::removeWhiteSpace(interfaceMode);

	newmode->interfaceMode_ = General::str2Interfacemode(interfaceMode, General::MONITOR);

	// Get the monitor group element.
	for (int i = 0; i < inpstected.childNodes().count(); ++i)
	{
		if (inpstected.childNodes().at(i).nodeName() == QString( "ipxact:group"))
		{
			newmode->group_ = inpstected.childNodes().at(i).childNodes().at(0).
					nodeValue();
			newmode->group_ = XmlUtils::removeWhiteSpace(newmode->group_);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readBusInterfaceExtensions()
//-----------------------------------------------------------------------------
void businterfaceReader::readBusInterfaceExtensions(QDomNode const& interfaceNode,
    QSharedPointer<BusInterface> newInterface) const
{
    QDomNode extensionsNode = interfaceNode.firstChildElement("ipxact:vendorExtensions");
    QDomElement positionElement = extensionsNode.firstChildElement("kactus2:position");
    if (!positionElement.isNull())
    {
        int positionX = positionElement.attribute("x").toInt();
        int positionY = positionElement.attribute("y").toInt();
        newInterface->setDefaultPos(QPointF(positionX, positionY));
    }

    parseVendorExtensions(interfaceNode, newInterface);
}