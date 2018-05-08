//-----------------------------------------------------------------------------
// File: ParameterConfigurableElementsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.09.2017
//
// Description:
// A filter class for displaying only the configurable elements with existing referenced parameters.
//-----------------------------------------------------------------------------

#include "ParameterConfigurableElementsFilter.h"

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementsFilter::ParameterConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ParameterConfigurableElementsFilter::ParameterConfigurableElementsFilter(QObject* parent):
ConfigurableElementsFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementsFilter::~ParameterConfigurableElementsFilter()
//-----------------------------------------------------------------------------
ParameterConfigurableElementsFilter::~ParameterConfigurableElementsFilter()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool ParameterConfigurableElementsFilter::filterAcceptsRow(int source_row, QModelIndex const& source_parent) const
{
    bool showImmediatesOrEditable = ConfigurableElementsFilter::filterAcceptsRow(source_row, source_parent);
    bool itemDefaultIsEmpty = isIndexedDefaultValueEmpty(source_row);

    return showImmediatesOrEditable && !itemDefaultIsEmpty;
}

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementsFilter::isIndexedDefaultValueEmpty()
//-----------------------------------------------------------------------------
bool ParameterConfigurableElementsFilter::isIndexedDefaultValueEmpty(int sourceRow) const
{
    QModelIndex defaultValueIndex = sourceModel()->index(sourceRow, ConfigurableElementsColumns::DEFAULT_VALUE);
    QString defaultValue = sourceModel()->data(defaultValueIndex, Qt::DisplayRole).toString();

    return defaultValue.isEmpty();
}
