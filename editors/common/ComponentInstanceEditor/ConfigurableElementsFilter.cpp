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

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>

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
    bool itemIsEditable = itemAtIndexIsEditable(source_row, source_parent);

    return showImmediateValues_ || itemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::itemAtIndexIsEditable()
//-----------------------------------------------------------------------------
bool ConfigurableElementsFilter::itemAtIndexIsEditable(int sourceRow, QModelIndex const& sourceParent) const
{
    QModelIndex itemIndex = sourceModel()->index(sourceRow, ConfigurableElementsColumns::VALUE, sourceParent);
    return itemIndex.flags().testFlag(Qt::ItemIsEditable);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsFilter::immediateValuesAreVisible()
//-----------------------------------------------------------------------------
bool ConfigurableElementsFilter::immediateValuesAreVisible() const
{
    return showImmediateValues_;
}
