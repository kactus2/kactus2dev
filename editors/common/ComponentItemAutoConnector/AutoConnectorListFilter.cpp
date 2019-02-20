//-----------------------------------------------------------------------------
// File: AutoConnectorListFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.02.2019
//
// Description:
// Sorting proxy model for auto connector lists.
//-----------------------------------------------------------------------------

#include "AutoConnectorListFilter.h"

//-----------------------------------------------------------------------------
// Function: AutoConnectorListFilter::AutoConnectorListFilter()
//-----------------------------------------------------------------------------
AutoConnectorListFilter::AutoConnectorListFilter(QTableWidget* itemTable, int const& tableColumn,
    QCheckBox* hideConnectedBox, QObject* parent):
QSortFilterProxyModel(parent),
tableColumn_(tableColumn),
itemTable_(itemTable),
hideConnectedBox_(hideConnectedBox)
{
    connect(hideConnectedBox, SIGNAL(stateChanged(int)), this, SLOT(invalidate()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorListFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool AutoConnectorListFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{   
    QModelIndex index = sourceModel()->index(source_row, 0);
    QString itemName = sourceModel()->data(index).toString();

    if (hideConnectedBox_->isChecked() && itemIsConnected(itemName))
    {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorListFilter::itemIsConnected()
//-----------------------------------------------------------------------------
bool AutoConnectorListFilter::itemIsConnected(QString const& itemName) const
{
    for (int i = 0; i < itemTable_->rowCount(); ++i)
    {
        QTableWidgetItem* tableItem = itemTable_->item(i, tableColumn_);
        if (tableItem && tableItem->text() == itemName)
        {
            return true;
        }
    }

    return false;
}
