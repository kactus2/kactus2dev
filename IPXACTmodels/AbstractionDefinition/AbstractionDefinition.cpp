//-----------------------------------------------------------------------------
// File: AbstractionDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Implementation for ipxact:busDefinition element.
//-----------------------------------------------------------------------------

#include "AbstractionDefinition.h"

#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/common/Choice.h>

#include <IPXACTmodels/utilities/Copy.h>

#include <QDomDocument>
#include <QString>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>


//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::AbstractionDefinition(VLNV const& vlnv, Document::Revision revision):
Document(vlnv, revision)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::AbstractionDefinition(AbstractionDefinition const& other):
Document(other),
    busType_(other.busType_),
    extends_(other.extends_)
{
    Copy::copyList(other.logicalPorts_, logicalPorts_);
    Copy::copyList(other.choices_, choices_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::operator=()
//-----------------------------------------------------------------------------
AbstractionDefinition & AbstractionDefinition::operator=(AbstractionDefinition const& other)
{
	if (this != &other)
    {
		Document::operator=(other);
        busType_ = other.busType_;
		extends_ = other.extends_;

        logicalPorts_->clear();
        Copy::copyList(other.logicalPorts_, logicalPorts_);

        choices_->clear();
        Copy::copyList(other.choices_, choices_);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::~AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::~AbstractionDefinition()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> AbstractionDefinition::clone() const
{
	return QSharedPointer<Document>(new AbstractionDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setVlnv()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setVlnv(VLNV const& vlnv)
{
    VLNV abstractionDefinitionVLNV(vlnv);
    abstractionDefinitionVLNV.setType(VLNV::ABSTRACTIONDEFINITION);
    Document::setVlnv(abstractionDefinitionVLNV);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setBusType(VLNV const& targetBus)
{
    busType_ = targetBus;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setBusType()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinition::getBusType() const
{
    return busType_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getExtends()
//-----------------------------------------------------------------------------
VLNV AbstractionDefinition::getExtends() const
{
    return extends_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setExtends(VLNV const& extends)
{
    extends_ = extends;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> AbstractionDefinition::getDependentVLNVs() const 
{
    QList<VLNV> vlnvList;

    if (busType_.isValid())
    {
        vlnvList.append(busType_);
    }

    if (extends_.isValid())
    {
        vlnvList.append(extends_);
    }

    return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList AbstractionDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::hasPort()
//-----------------------------------------------------------------------------
bool AbstractionDefinition::hasPort(QString const& portName, General::InterfaceMode mode) const
{
    if (logicalPorts_)
    {
		if (mode == General::INTERFACE_MODE_COUNT)
		{
			for (auto const& port: *logicalPorts_)
			{
				if (port->getLogicalName() == portName && port->hasWire())
				{
					return true;
				}
			}
		}

        for (auto const& port : *logicalPorts_)
        {
            if (port->getLogicalName() == portName)
            {
                QSharedPointer<WireAbstraction> portWire = port->getWire();
                QSharedPointer<TransactionalAbstraction> portTransactional = port->getTransactional();
                if ((mode == General::MASTER || mode == General::MIRRORED_MASTER || mode == General::INITIATOR || mode == General::MIRRORED_INITIATOR) &&
                    ((portWire && portWire->hasMasterPort()) ||
                    (portTransactional && portTransactional->hasMasterPort())))
                {
                    return true;
                }
                else if ((mode == General::SLAVE || mode == General::MIRRORED_SLAVE || mode == General::TARGET || mode == General::MIRRORED_TARGET) &&
                    ((portWire && portWire->hasSlavePort()) ||
                    (portTransactional && portTransactional->hasSlavePort())))
                {
                    return true;
                }
                else if ((mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM) &&
                    ((portWire && !portWire->getSystemPorts()->isEmpty()) ||
                    (portTransactional && portTransactional->getSystemPorts()->isEmpty())))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPortNames()
//-----------------------------------------------------------------------------
QStringList AbstractionDefinition::getPortNames(General::InterfaceMode mode) const
{
    QStringList portNames;
    if (logicalPorts_)
    {
        for (auto const& port : *logicalPorts_)
        {
            if (port->hasWire())
            {
                if ((mode == General::MASTER || mode == General::MIRRORED_MASTER) && port->getWire()->hasMasterPort())
                {
                    portNames.append(port->getLogicalName());
                }
                else if ((mode == General::SLAVE || mode == General::MIRRORED_SLAVE) && port->getWire()->hasSlavePort())
                {
                    portNames.append(port->getLogicalName());
                }
                else if ((mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM) && 
                    !port->getWire()->getSystemPorts()->isEmpty())
                {
                    portNames.append(port->getLogicalName());
                }
            }        
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPortDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction AbstractionDefinition::getPortDirection(QString const& portName,
    General::InterfaceMode mode, QString const& newSystemGroup) const
{
    if (logicalPorts_)
    {
        for (auto const& port : *logicalPorts_)
        {
            if (port->getLogicalName() == portName && port->hasWire())
            {
                return port->getWire()->getDirection(mode, newSystemGroup);            
            }        
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPortInitiative()
//-----------------------------------------------------------------------------
QString AbstractionDefinition::getPortInitiative(QString const& portName, General::InterfaceMode mode,
    QString const& systemGroup) const
{
    if (logicalPorts_)
    {
        for (auto port : *logicalPorts_)
        {
            if (port->getLogicalName() == portName && port->hasTransactional())
            {
                return port->getTransactional()->getInitiative(mode, systemGroup);
            }
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinition::getPort(QString const& portName) const
{
    if (logicalPorts_)
    {
        for (auto const& port : *logicalPorts_)
        {
            if (port->getLogicalName() == portName)
            {
                return port;            
            } 
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getLogicalPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortAbstraction> > > AbstractionDefinition::getLogicalPorts() const
{
    return logicalPorts_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPortsWithWire()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortAbstraction> > AbstractionDefinition::getPortsWithWire() const
{
    QList<QSharedPointer<PortAbstraction> > wirePorts;
    std::for_each(logicalPorts_->cbegin(), logicalPorts_->cend(), [&wirePorts](QSharedPointer<PortAbstraction> portAbstraction)
        {
            if (portAbstraction->hasWire())
            {
                wirePorts.append(portAbstraction);
            }
        });
    return wirePorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPortsWithTransactional()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortAbstraction> > AbstractionDefinition::getPortsWithTransactional() const
{
    QList<QSharedPointer<PortAbstraction> > transPorts;
    std::for_each(logicalPorts_->cbegin(), logicalPorts_->cend(), [&transPorts](QSharedPointer<PortAbstraction> portAbstraction)
        {
            if (portAbstraction->hasTransactional())
            {
                transPorts.append(portAbstraction);
            }
        });
    return transPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::setChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinition::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    choices_ = choices;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > AbstractionDefinition::getChoices() const
{
    return choices_;
}
