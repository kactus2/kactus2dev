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
viewRef_(),
abstractionRef_(),
portMaps_(new QList<QSharedPointer<PortMap> >())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionType::AbstractionType()
//-----------------------------------------------------------------------------
AbstractionType::AbstractionType(AbstractionType const& other):
viewRef_(other.viewRef_),
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
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::operator=()
//-----------------------------------------------------------------------------
AbstractionType& AbstractionType::operator=(AbstractionType const& other)
{
	if (this != &other)
	{
        viewRef_ = other.viewRef_;

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
	}

	return *this;
}


//-----------------------------------------------------------------------------
// Function: AbstractionType::setViewRef()
//-----------------------------------------------------------------------------
void AbstractionType::setViewRef(QString const& viewName)
{
    viewRef_ = viewName;
}

//-----------------------------------------------------------------------------
// Function: AbstractionType::getViewRef()
//-----------------------------------------------------------------------------
QString AbstractionType::getViewRef() const
{
    return viewRef_;
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
