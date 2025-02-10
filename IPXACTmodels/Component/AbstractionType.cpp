//-----------------------------------------------------------------------------
// File: AbstractionType.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:abstractionType in bus interface.
//-----------------------------------------------------------------------------

#include "AbstractionType.h"

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: AbstractionType()
//-----------------------------------------------------------------------------
AbstractionType::AbstractionType():
viewReferences_(new QStringList()),
abstractionRef_(),
portMaps_(new QList<QSharedPointer<PortMap> >())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionType::~AbstractionType()
//-----------------------------------------------------------------------------
AbstractionType::~AbstractionType()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionType::AbstractionType()
//-----------------------------------------------------------------------------
AbstractionType::AbstractionType(AbstractionType const& other):
viewReferences_(new QStringList()),
abstractionRef_(),
portMaps_(new QList<QSharedPointer<PortMap> >())
{
    if (other.abstractionRef_)
    {
        abstractionRef_ = QSharedPointer<ConfigurableVLNVReference>
            (new ConfigurableVLNVReference(*other.abstractionRef_.data()));
    }

	foreach (QSharedPointer<PortMap> portMap, *other.portMaps_)
    {
        if (portMap)
        {
            QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(new PortMap(*portMap.data()));
            portMaps_->append(copy);
		}
	}

    copyViewReferences(other.viewReferences_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::operator=()
//-----------------------------------------------------------------------------
AbstractionType& AbstractionType::operator=(AbstractionType const& other)
{
	if (this != &other)
	{
        abstractionRef_.clear();
        if (other.abstractionRef_)
        {
            abstractionRef_ = QSharedPointer<ConfigurableVLNVReference>
                (new ConfigurableVLNVReference(*other.abstractionRef_.data()));
        }

		portMaps_->clear();
		foreach (QSharedPointer<PortMap> portMap, *other.portMaps_)
		{
			if (portMap)
			{
				QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(new PortMap(*portMap.data()));
				portMaps_->append(copy);
			}
		}

        viewReferences_->clear();
        copyViewReferences(other.viewReferences_);
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::copyViewReferences()
//-----------------------------------------------------------------------------
void AbstractionType::copyViewReferences(QSharedPointer<QStringList> newViewReferences)
{
    foreach (QString viewReference, *newViewReferences)
    {
        viewReferences_->append(viewReference);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::setViewReferences()
//-----------------------------------------------------------------------------
void AbstractionType::setViewReferences(QSharedPointer<QStringList> newViewReferences)
{
    viewReferences_ = newViewReferences;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getViewReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> AbstractionType::getViewReferences() const
{
    return viewReferences_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::setAbstractionRef()
//-----------------------------------------------------------------------------
void AbstractionType::setAbstractionRef(QSharedPointer<ConfigurableVLNVReference> abstractionRef)
{
    abstractionRef_ = abstractionRef;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getAbstractionRef()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> AbstractionType::getAbstractionRef() const
{
    return abstractionRef_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getPortMaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortMap> > > AbstractionType::getPortMaps() const
{
    return portMaps_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getPhysicalPortNames()
//-----------------------------------------------------------------------------
QStringList AbstractionType::getPhysicalPortNames() const
{
    QStringList portNames;

    if (portMaps_)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getPhysicalPort())
            {
                portNames.append(portMap->getPhysicalPort()->name_);
            }
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getLogicalPortNames()
//-----------------------------------------------------------------------------
QStringList AbstractionType::getLogicalPortNames() const
{
    QStringList portNames;

    if (portMaps_)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getLogicalPort())
            {
                portNames.append(portMap->getLogicalPort()->name_);
            }
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::hasLogicalPort()
//-----------------------------------------------------------------------------
bool AbstractionType::hasLogicalPort(QString const& portName) const
{
    if (portMaps_)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getLogicalPort() && portMap->getLogicalPort()->name_ == portName)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::hasPhysicalPort()
//-----------------------------------------------------------------------------
bool AbstractionType::hasPhysicalPort(QString const& portName) const
{
    if (portMaps_)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getPhysicalPort() && portMap->getPhysicalPort()->name_ == portName)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getMappedLogicalPortName()
//-----------------------------------------------------------------------------
QString AbstractionType::getMappedLogicalPortName(QString const& physicalPortName) const
{
    if (portMaps_)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getPhysicalPort() && portMap->getLogicalPort() &&
                portMap->getPhysicalPort()->name_.compare(physicalPortName) == 0)
            {
                return portMap->getLogicalPort()->name_;
            }
        }
    }

    return QString();
}
