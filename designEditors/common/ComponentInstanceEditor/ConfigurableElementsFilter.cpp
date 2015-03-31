//-----------------------------------------------------------------------------
// File: ConfigurableElementsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A filter class for configurable elements.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsFilter.h"

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::ConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ConfigurableElementsFilter::ConfigurableElementsFilter(QObject* parent):
QSortFilterProxyModel(parent),
indexOfRemovedElement_(QModelIndex())
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::~ConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ConfigurableElementsFilter::~ConfigurableElementsFilter()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsFilter::onRemoveItem(QModelIndex const& filterIndex)
{
    indexOfRemovedElement_ = mapToSource(filterIndex);

    emit removeItem(indexOfRemovedElement_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool ConfigurableElementsFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!source_parent.isValid())
    {
        QModelIndex itemIndex = sourceModel()->index(source_row, 0, source_parent);

        if (sourceModel()->rowCount(itemIndex) == 0)
        {
            return false;
        }
    }

    return true;
}
