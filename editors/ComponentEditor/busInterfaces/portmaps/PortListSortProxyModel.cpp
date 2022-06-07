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

#include <KactusAPI/include/PortsInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMappingColumns.h>
#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::PortListSortProxyModel(int const& abstractionIndex, PortMapInterface* portMapInterface,
    AbstractionTypeInterface* abstractionInterface, QObject *parent):
QSortFilterProxyModel(parent),
filterDirection_(DirectionFilter::ANY),
hideConnected_(true),
connectedPorts_(),
filterPorts_(),
visibleType_(PortTypes::WIRETYPE),
portMapInterface_(portMapInterface),
portInterface_(portMapInterface_->getPhysicalPortInterface()),
abstractionIndex_(abstractionIndex),
abstractionInterface_(abstractionInterface)
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

    addConnectedPortsFromAbstraction();

/*
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
*/

    invalidateFilter(); 
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::addConnectedPortsFromAbstraction()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::addConnectedPortsFromAbstraction()
{
    QStringList physicalPorts;
    std::vector<std::string> portMapPhysicalPorts =
        abstractionInterface_->getPhysicalPortsFromPortMaps(abstractionIndex_);
    for (auto portName : portMapPhysicalPorts)
    {
        physicalPorts.append(QString::fromStdString(portName));
    }

    if (!physicalPorts.isEmpty())
    {
        for (auto portName : physicalPorts)
        {
            if (!connectedPorts_.contains(portName))
            {
                connectedPorts_.append(portName);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{   
    QModelIndex nameIndex = sourceModel()->index(source_row, 0);
    QString portName = sourceModel()->data(nameIndex).toString();

    // Check filter for direction.
    QString directionString = QString::fromStdString(portInterface_->getDirection(portName.toStdString()));
    DirectionTypes::Direction portDirection =
        DirectionTypes::str2Direction(directionString, DirectionTypes::DIRECTION_INVALID);
    if (filterDirection_ != DirectionFilter::ANY &&
        portDirection != static_cast<DirectionTypes::Direction>(filterDirection_))
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
    if (visibleType_ == PortTypes::WIRETYPE && !portInterface_->portIsWire(portName.toStdString()))
    {
        return false;
    }
    else if (visibleType_ == PortTypes::TRANSACTIONALTYPE &&
        !portInterface_->portIsTransactional(portName.toStdString()))
    {
        return false;
    }

    // Check filter for port name.
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterAcceptsColumn()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    if ((visibleType_ == PortTypes::WIRETYPE && source_column == PortMappingColumns::INITIATIVE) ||
        (visibleType_ == PortTypes::TRANSACTIONALTYPE &&
            (source_column == PortMappingColumns::DIRECTION || source_column == PortMappingColumns::LEFT_BOUND ||
            source_column == PortMappingColumns::RIGHT_BOUND)))
    {
        return false;
    }
    else
    {
        return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onChangeFilteredType()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onChangeFilteredType(QString const& newVisibleType)
{
    visibleType_ = newVisibleType;
    invalidateFilter();
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
    for (int i = 0; i < portMapInterface_->itemCount(); ++i)
    {
        QString physicalPortName = QString::fromStdString(portMapInterface_->getPhysicalPortName(i));
        if (physicalPortName == portName)
        {
            return;
        }
    }

    connectedPorts_.removeAll(portName);
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setNewAbstractionType()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setNewAbstractionType(int const& newAbstractionIndex)
{
    abstractionIndex_ = newAbstractionIndex;
}
