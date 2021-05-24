//-----------------------------------------------------------------------------
// File: AbstractionDefinitionPortsSortFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.11.2017
//
// Description:
// Sort filter proxy model for abstraction definition ports.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionPortsSortFilter.h"

#include <common/KactusColors.h>

#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>
#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::AbstractionDefinitionPortsSortFilter()
//-----------------------------------------------------------------------------
AbstractionDefinitionPortsSortFilter::AbstractionDefinitionPortsSortFilter(
    PortAbstractionInterface* portInterface, QObject *parent):
QSortFilterProxyModel(parent),
portInterface_(portInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::data()
//-----------------------------------------------------------------------------
QVariant AbstractionDefinitionPortsSortFilter::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole)
    {
        return getBackgroundColorForIndex(index);
    }

    else
    {
        return QSortFilterProxyModel::data(index, role);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::getBackgroundColorForIndex()
//-----------------------------------------------------------------------------
QColor AbstractionDefinitionPortsSortFilter::getBackgroundColorForIndex(QModelIndex const& index) const
{
    bool systemGroupIsMandatory = isSystemGroupMandatory(index);
    if (index.column() == LogicalPortColumns::NAME || systemGroupIsMandatory)
    {
        return KactusColors::MANDATORY_FIELD;
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP && !systemGroupIsMandatory)
    {
        return KactusColors::DISABLED_FIELD;
    }

    int previousRow = index.row() - 1;
    QString previousName("");
    QColor previousColor = KactusColors::REGULAR_FIELD;
    if (previousRow < 0)
    {
        return previousColor;
    }
    else
    {
        QModelIndex previousNameIndex = index.sibling(previousRow, LogicalPortColumns::NAME);
        previousName = previousNameIndex.data(Qt::DisplayRole).toString();
    }

    QModelIndex nameIndex = index.sibling(index.row(), LogicalPortColumns::NAME);
    QString name = nameIndex.data(Qt::DisplayRole).toString();

    QModelIndex previousColorIndex = index.sibling(previousRow, LogicalPortColumns::DESCRIPTION);
    previousColor = previousColorIndex.data(Qt::BackgroundRole).value<QColor>();

    if (name.compare(previousName) == 0)
    {
        return previousColor;
    }
    else if (previousColor == KactusColors::REGULAR_FIELD)
    {
        return KactusColors::FIELD_COLOR;
    }
    
    return KactusColors::REGULAR_FIELD;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::isSystemGroupMandatory()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionPortsSortFilter::isSystemGroupMandatory(QModelIndex const& index) const
{
    if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        QModelIndex modeIndex = index.sibling(index.row(), LogicalPortColumns::MODE);
        QString portMode = modeIndex.data(Qt::DisplayRole).toString();
        if (portMode.compare(QStringLiteral("system"), Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionPortsSortFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (source_parent.isValid())
    {
        return false;
    }

    std::string portName = portInterface_->getIndexedItemName(source_row);
    return portInterface_->portIsWire(portName);
}


//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionPortsSortFilter::getPortInterface()
//-----------------------------------------------------------------------------
PortAbstractionInterface* AbstractionDefinitionPortsSortFilter::getPortInterface() const
{
    return portInterface_;
}

