//-----------------------------------------------------------------------------
// File: MissingConfigurableElementsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.08.2017
//
// Description:
// A filter class for displaying only the unknown configurable elements.
//-----------------------------------------------------------------------------

#include "MissingConfigurableElementsFilter.h"

//-----------------------------------------------------------------------------
// Function: MissingConfigurableElementsFilter::MissingConfigurableElementsFilter()
//-----------------------------------------------------------------------------
MissingConfigurableElementsFilter::MissingConfigurableElementsFilter(QObject* parent):
ParameterConfigurableElementsFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: MissingConfigurableElementsFilter::~MissingConfigurableElementsFilter()
//-----------------------------------------------------------------------------
MissingConfigurableElementsFilter::~MissingConfigurableElementsFilter()
{

}

//-----------------------------------------------------------------------------
// Function: MissingConfigurableElementsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool MissingConfigurableElementsFilter::filterAcceptsRow(int source_row, QModelIndex const& source_parent) const
{
    bool itemIsEditable = itemAtIndexIsEditable(source_row, source_parent);
    bool itemDefaultIsEmpty = isIndexedDefaultValueEmpty(source_row);

    return itemIsEditable && itemDefaultIsEmpty;
}
