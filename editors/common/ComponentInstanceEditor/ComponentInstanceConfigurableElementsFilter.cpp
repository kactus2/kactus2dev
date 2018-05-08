//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.08.2017
//
// Description:
// A filter class for component instance configurable elements.
//-----------------------------------------------------------------------------

#include "ComponentInstanceConfigurableElementsFilter.h"

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::ComponentInstanceConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsFilter::ComponentInstanceConfigurableElementsFilter(QObject* parent):
QSortFilterProxyModel(parent),
showImmediateValues_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::~ComponentInstanceConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ComponentInstanceConfigurableElementsFilter::~ComponentInstanceConfigurableElementsFilter()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::setShowImmediateValues()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsFilter::setShowImmediateValues(bool show)
{
    showImmediateValues_ = show;
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool ComponentInstanceConfigurableElementsFilter::filterAcceptsRow(int source_row,
    QModelIndex const& source_parent) const
{
    if (!source_parent.isValid())
    {
        QModelIndex categoryIndex = sourceModel()->index(source_row, 0, QModelIndex());

        return sourceModel()->rowCount(categoryIndex) != 0 &&
            (indexContainsEditableChildren(categoryIndex) || showImmediateValues_);
    }

    else
    {
        QModelIndex itemIndex = sourceModel()->index(source_row, ConfigurableElementsColumns::VALUE, source_parent);
        return (showImmediateValues_ || itemIndex.flags().testFlag(Qt::ItemIsEditable));
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::indexContainsEditableChildren()
//-----------------------------------------------------------------------------
bool ComponentInstanceConfigurableElementsFilter::indexContainsEditableChildren(QModelIndex const& index) const
{
    QModelIndex nameIndex = index.sibling(index.row(), ConfigurableElementsColumns::NAME);
    QString indexName = nameIndex.data(Qt::DisplayRole).toString();

    for (int i = 0; i < index.model()->rowCount(index); ++i)
    {
        QModelIndex childIndex = sourceModel()->index(i, ConfigurableElementsColumns::VALUE, index);
        if (childIndex.flags().testFlag(Qt::ItemIsEditable))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceConfigurableElementsFilter::onInvalidateFilter()
//-----------------------------------------------------------------------------
void ComponentInstanceConfigurableElementsFilter::onInvalidateFilter()
{
    invalidate();
}
