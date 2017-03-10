//-----------------------------------------------------------------------------
// File: ConfigurableElementsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A filter class for configurable elements.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsFilter.h"

#include "ConfigurableElementsColumns.h"

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::ConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ConfigurableElementsFilter::ConfigurableElementsFilter(QObject* parent):
QSortFilterProxyModel(parent),
    showImmediateValues_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::~ConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ConfigurableElementsFilter::~ConfigurableElementsFilter()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::setShowImmediateValues()
//-----------------------------------------------------------------------------
void ConfigurableElementsFilter::setShowImmediateValues(bool show)
{
    showImmediateValues_ = show;
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onRemoveItem()
//-----------------------------------------------------------------------------
void ConfigurableElementsFilter::onRemoveItem(QModelIndex const& filterIndex)
{
    emit removeItem(mapToSource(filterIndex));
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool ConfigurableElementsFilter::filterAcceptsRow(int source_row, QModelIndex const& source_parent) const
{
    if (!source_parent.isValid())
    {
        QModelIndex categoryIndex = sourceModel()->index(source_row, 0, QModelIndex());
        return sourceModel()->rowCount(categoryIndex) != 0;
    }

    else
    {
        QModelIndex itemIndex = sourceModel()->index(source_row, ConfigurableElementsColumns::VALUE, source_parent);
        return (showImmediateValues_ || itemIndex.flags().testFlag(Qt::ItemIsEditable));
    }
}
