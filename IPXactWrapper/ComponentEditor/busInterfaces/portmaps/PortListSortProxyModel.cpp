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
    onConnectionsReset();
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
// Function: PortListSortProxyModel::setFilterPortName()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterPortName(QString const& portName)
{
    setFilterRegExp(QRegExp(portName, Qt::CaseInsensitive));
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterInDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterInDirection(bool enabled)
{
    switch(filterDirection_)
    {
    case IN:
        {
            if (!enabled)
            {
                filterDirection_ = ANY;
            }
            break;
        }
    case OUT:
        {
            if (enabled)
            {
                filterDirection_ = INOUT;
            }
            break;
        }

    case INOUT:
        {
            if (!enabled)
            {
                filterDirection_ = OUT;
            }
            break;
        }

    case ANY:
    default:
        {
            if (enabled)
            {
                filterDirection_ = IN;
            }
        }
    }
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterOutDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterOutDirection(bool enabled)
{
    switch(filterDirection_)
    {
    case IN:
        {
            if (enabled)
            {
                filterDirection_ = INOUT;
            }
            break;
        }
    case OUT:
        {
            if (!enabled)
            {
                filterDirection_ = ANY;
            }
            break;
        }

    case INOUT:
        {
            if (!enabled)
            {
                filterDirection_ = IN;
            }
        }

    case ANY:
    default:
        {
            if (enabled)
            {
                filterDirection_ = OUT;
            }
        }
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
// Function: PortListSortProxyModel::setFilterHideConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterHideConnected(bool hide /*= true*/)
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