//-----------------------------------------------------------------------------
// File: PortListSortProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.10.2013
//
// Description:
// Sorting proxy model for port lists.
//-----------------------------------------------------------------------------

#include "PortListSortProxyModel.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::PortListSortProxyModel(QSharedPointer<Component> component, QObject *parent) :
QSortFilterProxyModel(parent),
component_(component),
filterDirection_(DirectionFilter::ANY),
hideConnected_(true),
connectedPorts_(),
filterPorts_(),
abstraction_()
{
    onConnectionsReset();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterDirection()
//-----------------------------------------------------------------------------
PortListSortProxyModel::DirectionFilter PortListSortProxyModel::filterDirection() const
{
    return filterDirection_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterPortName()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterPortName(QString const& portName)
{
    setFilterRegExp(QRegExp(portName, Qt::CaseInsensitive));
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterPorts()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterPorts(QStringList const& ports)
{
    filterPorts_ = ports;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterInDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterInDirection(bool filterInDirection)
{
    if (filterDirection_ == DirectionFilter::IN && !filterInDirection)
    {
        filterDirection_ = DirectionFilter::ANY;
    }
    else if (filterDirection_ == DirectionFilter::OUT && filterInDirection)
    {
        filterDirection_ = DirectionFilter::INOUT;
    }
    else if (filterDirection_ == DirectionFilter::INOUT && !filterInDirection)
    {
        filterDirection_ = DirectionFilter::OUT;
    }
    else if (filterDirection_ == DirectionFilter::ANY && filterInDirection)
    {
        filterDirection_ = DirectionFilter::IN;
    }
    
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterOutDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterOutDirection(bool filterOutDirection)
{
    if (filterDirection_ == DirectionFilter::IN && filterOutDirection)
    {
        filterDirection_ = DirectionFilter::INOUT;
    }
    else if (filterDirection_ == DirectionFilter::OUT && !filterOutDirection)
    {
        filterDirection_ = DirectionFilter::ANY;
    }
    else if (filterDirection_ == DirectionFilter::INOUT && !filterOutDirection)
    {
        filterDirection_ = DirectionFilter::IN;
    }
    else if (filterDirection_ == DirectionFilter::ANY && filterOutDirection)
    {
        filterDirection_ = DirectionFilter::OUT;
    }

    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterHideConnected()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterHideConnected() const
{
    return hideConnected_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterPortNames()
//-----------------------------------------------------------------------------
QStringList PortListSortProxyModel::filterPortNames() const
{
    return filterPorts_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterHideConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterHideConnected(bool hide)
{
    hideConnected_ = hide;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onConnectionsChanged()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onConnectionsReset()
{
    connectedPorts_.clear();

    addConnectedPortsFromAbstraction(abstraction_);

    foreach (QSharedPointer<BusInterface> busIf, *component_->getBusInterfaces())
    {
        if (busIf->getAbstractionTypes() && !busIf->getAbstractionTypes()->isEmpty())
        {
            if (busIf->getAbstractionTypes()->size() == 1 &&
                busIf->getAbstractionTypes()->first()->getViewReferences()->isEmpty())
            {
                QSharedPointer<AbstractionType> singleAbstraction = busIf->getAbstractionTypes()->first();
                addConnectedPortsFromAbstraction(singleAbstraction);
            }
        }
    }

    invalidateFilter(); 
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::addConnectedPortsFromAbstraction()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::addConnectedPortsFromAbstraction(QSharedPointer<AbstractionType> abstraction)
{
    if (abstraction && abstraction->getPortMaps() && !abstraction->getPortMaps()->isEmpty())
    {
        foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
        {
            if (portMap->getPhysicalPort())
            {
                QString physicalName = portMap->getPhysicalPort()->name_;
                if (!connectedPorts_.contains(physicalName))
                {
                    connectedPorts_.append(physicalName);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{   
    QModelIndex index = sourceModel()->index(source_row, 0);
    QString portName = sourceModel()->data(index).toString();

    // Check filter for direction.
    QSharedPointer<Port> currentPort = component_->getPort(portName);
    if (filterDirection_ != DirectionFilter::ANY && 
        currentPort->getDirection() != static_cast<DirectionTypes::Direction>(filterDirection_))
    {
        return false;
    }

    // Check filter for connected ports.
    if (hideConnected_ && connectedPorts_.contains(portName))
    {
        return false;
    }

    // Check filter for allowed port names.
    if (!filterPorts_.isEmpty() && !filterPorts_.contains(portName))
    {
        return false;
    }

    // Check filter for port name.
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onPortConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onPortConnected(QString const& portName)
{
    if (!connectedPorts_.contains(portName))
    {
        connectedPorts_.append(portName);
        invalidateFilter();
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onPortDisconnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onPortDisconnected(QString const& portName)
{
    if (connectedPorts_.contains(portName))
    {
        connectedPorts_.removeAll(portName);
        invalidateFilter();
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setNewAbstractionType()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setNewAbstractionType(QSharedPointer<AbstractionType> newAbstraction)
{
    abstraction_ = newAbstraction;
}
