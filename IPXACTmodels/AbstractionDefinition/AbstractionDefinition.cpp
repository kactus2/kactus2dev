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

#include "PortAbstraction.h"
#include "WireAbstraction.h"

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
AbstractionDefinition::AbstractionDefinition(): 
Document(), 
    busType_(),
    extends_(),
    logicalPorts_(new QList<QSharedPointer<PortAbstraction> >())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::AbstractionDefinition()
//-----------------------------------------------------------------------------
AbstractionDefinition::AbstractionDefinition(AbstractionDefinition const& other):
Document(other),
    busType_(other.busType_),
    extends_(other.extends_),
    logicalPorts_(new QList<QSharedPointer<PortAbstraction> >())
{
    foreach (QSharedPointer<PortAbstraction> port, *other.logicalPorts_)
    {
        logicalPorts_->append(QSharedPointer<PortAbstraction>(new PortAbstraction(*port)));
    }
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

        foreach (QSharedPointer<PortAbstraction> port, *other.logicalPorts_)
        {
            logicalPorts_->append(QSharedPointer<PortAbstraction>(new PortAbstraction(*port)));
        }
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
bool AbstractionDefinition::hasPort(QString const& portName, General::InterfaceMode mode)
{
    if (logicalPorts_)
    {
        foreach (QSharedPointer<PortAbstraction> port, *logicalPorts_)
        {
            if (port->getLogicalName() == portName && port->hasWire())
            {
                if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && port->getWire()->hasMasterPort())
                {
                    return true;
                }
                else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && port->getWire()->hasSlavePort())
                {
                    return true;
                }
                else if ((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM) && 
                    !port->getWire()->getSystemPorts()->isEmpty())
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
        foreach (QSharedPointer<PortAbstraction> port, *logicalPorts_)
        {
            if (port->hasWire())
            {
                if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && port->getWire()->hasMasterPort())
                {
                    portNames.append(port->getLogicalName());
                }
                else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && port->getWire()->hasSlavePort())
                {
                    portNames.append(port->getLogicalName());
                }
                else if ((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM) && 
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
DirectionTypes::Direction AbstractionDefinition::getPortDirection(QString const& portName, General::InterfaceMode mode)
{
    if (logicalPorts_)
    {
        foreach (QSharedPointer<PortAbstraction> port, *logicalPorts_)
        {
            if (port->getLogicalName() == portName && port->hasWire())
            {
                return port->getWire()->getDirection(mode);            
            }        
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinition::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinition::getPort(QString const& portName)
{
    if (logicalPorts_)
    {
        foreach (QSharedPointer<PortAbstraction> port, *logicalPorts_)
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
