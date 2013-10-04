//-----------------------------------------------------------------------------
// File: PortListSortProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.10.2013
//
// Description:
// Sorting proxy model for port lists.
//-----------------------------------------------------------------------------

#include "PortListSortProxyModel.h"

#include <models/businterface.h>
#include <models/component.h>
#include <models/generaldeclarations.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::PortListSortProxyModel(QSharedPointer<Component> component,
    QObject *parent) :
QSortFilterProxyModel(parent),
component_(component),
filterDirection_(ANY),
hideConnected_(true),
connectedPorts_()
{
    onConnectionsChanged();
}

//-----------------------------------------------------------------------------
// Function: ~PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::~PortListSortProxyModel()
{
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterDirection()
//-----------------------------------------------------------------------------
PortListSortProxyModel::DirectionFilter PortListSortProxyModel::filterDirection() const
{
    return filterDirection_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterDirection(DirectionFilter direction)
{
    filterDirection_ = direction;
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
// Function: PortListSortProxyModel::setFilterHideConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterHideConnected(bool hide /*= true*/)
{
    hideConnected_ = hide;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterDirection(QString const& direction)
{
    if (QString::compare(direction, "any", Qt::CaseInsensitive) == 0)
    {
        setFilterDirection(ANY);
    }
    else if (QString::compare(direction, "in", Qt::CaseInsensitive) == 0)
    {
        setFilterDirection(IN);
    }
    else if (QString::compare(direction, "inout", Qt::CaseInsensitive) == 0)
    {
        setFilterDirection(INOUT);
    }
    else if (QString::compare(direction, "out", Qt::CaseInsensitive) == 0)
    {
        setFilterDirection(OUT);
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onConnectionsChanged()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onConnectionsChanged()
{
    connectedPorts_.clear();
    foreach (QSharedPointer<BusInterface> busIf, component_->getBusInterfaces())
    {
        foreach (QSharedPointer<General::PortMap> portMap, busIf->getPortMaps())
        {
            if (!connectedPorts_.contains(portMap->physicalPort_))
            {
                connectedPorts_.append(portMap->physicalPort_);
            }
        }
    }
    invalidateFilter(); 
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{   
    QModelIndex index = sourceModel()->index(source_row, 0);
    QString portName = sourceModel()->data(index).toString();

    // Check filter for direction.
    if (filterDirection_ != ANY && component_->getPortDirection(portName) != filterDirection_)
    {
        return false;
    }

    // Check filter for connected ports.
    if (hideConnected_ && connectedPorts_.contains(portName))
    {
        return false;
    }

    // Check filter for port name.
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}