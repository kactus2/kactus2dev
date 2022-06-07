//-----------------------------------------------------------------------------
// File: PortMapSortFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.06.2022
//
// Description:
// Sort filter proxy model for port maps.
//-----------------------------------------------------------------------------

#include "PortMapSortFilter.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: PortMapSortFilter::PortMapSortFilter()
//-----------------------------------------------------------------------------
PortMapSortFilter::PortMapSortFilter(PortMapInterface* portMapInterface, QObject *parent):
QSortFilterProxyModel(parent),
portMapInterface_(portMapInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapSortFilter::data()
//-----------------------------------------------------------------------------
QVariant PortMapSortFilter::data(const QModelIndex &index, int role) const
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
// Function: PortMapSortFilter::getBackgroundColorForIndex()
//-----------------------------------------------------------------------------
QColor PortMapSortFilter::getBackgroundColorForIndex(QModelIndex const& index) const
{
    QModelIndex sourceIndex = mapToSource(index);

    std::string logicalTieOff = portMapInterface_->getLogicalTieOffValue(sourceIndex.row());
    bool physicalPortExists = portMapInterface_->hasPhysicalPort(sourceIndex.row());

    if (index.column() == PortMapsColumns::LOGICAL_PORT ||
        (index.column() == PortMapsColumns::PHYSICAL_PORT && logicalTieOff.empty()) ||
        (index.column() == PortMapsColumns::TIEOFF && !physicalPortExists))
    {
        return KactusColors::MANDATORY_FIELD;
    }


    int previousRow = index.row() - 1;
    std::string previousName("");
    QColor previousColor = KactusColors::REGULAR_FIELD;
    if (previousRow < 0)
    {
        return previousColor;
    }
    else
    {
        QModelIndex previousIndex = mapToSource(index.sibling(previousRow, PortMapsColumns::LOGICAL_PORT));
        previousName = portMapInterface_->getLogicalPortName(previousIndex.row());
    }

    std::string name = portMapInterface_->getLogicalPortName(sourceIndex.row());

    previousColor = getPreviousColor(previousRow, index);

    if (name == previousName)
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
// Function: PortMapSortFilter::getPreviousColor()
//-----------------------------------------------------------------------------
QColor PortMapSortFilter::getPreviousColor(int const& previousRow, QModelIndex const& index) const
{
    QModelIndex previousColorIndex = index.sibling(previousRow, 0);

    QColor previousColor = previousColorIndex.data(Qt::BackgroundRole).value<QColor>();
    while ((previousColor == KactusColors::DISABLED_FIELD || previousColor == KactusColors::MANDATORY_FIELD) &&
        previousColorIndex.column() < PortMapsColumns::COLUMN_COUNT)
    {
        previousColorIndex = previousColorIndex.sibling(previousRow, previousColorIndex.column() + 1);
        previousColor = previousColorIndex.data(Qt::BackgroundRole).value<QColor>();
    }

    return previousColor;
}
