//-----------------------------------------------------------------------------
// File: hierarchyfilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 1.7.2011
//
// Description:
// The class that does the filtering and sorting of items for the library hierarchy view.
//-----------------------------------------------------------------------------

#include "hierarchyfilter.h"

#include "../LibraryFilter.h"
#include "hierarchyitem.h"

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::HierarchyFilter()
//-----------------------------------------------------------------------------
HierarchyFilter::HierarchyFilter(QObject *parent):
LibraryFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::~HierarchyFilter()
//-----------------------------------------------------------------------------
HierarchyFilter::~HierarchyFilter()
{

}

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool HierarchyFilter::filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const
{
	// find the hierarchy item
	QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);

	// root item is always ok
	if (!itemIndex.isValid())
    {
		return true;
    }

	HierarchyItem* item = static_cast<HierarchyItem*>(itemIndex.internalPointer());

    // If no search rules for vlnv are defined then display only non-duplicate children of root.
    if (hasEmptyVLNVfilter() && item->parentIsRoot() && item->isDuplicate())
    {
        return false;
    }

    HierarchyItem::ObjectType itemType = item->type();
    if (itemType == HierarchyItem::COMPONENT)
    {
        if (!type().components_)
        {
            return false;
        }
    }

    else if (itemType == HierarchyItem::APIDEFINITION || itemType == HierarchyItem::COMDEFINITION)
    {
        if (!type().buses_)
        {
            return false;
        }

        // if SW should not be displayed then this is not shown
        if (!implementation().sw_)
        {
            return false;
        }

    }

    else if (itemType == HierarchyItem::BUSDEFINITION || itemType == HierarchyItem::ABSDEFINITION)
    {
        if (!type().buses_)
        {
            return false;
        }
        // if hw should not be displayed then this is not shown
        if (!implementation().hw_)
        {
            return false;
        }
    }

    QSharedPointer<Component const> component = item->component();
    if (component)
    {
        // check the filters
        if (!checkFirmness(component) || !checkImplementation(component) || !checkHierarchy(component))
        {
            return false;
        }
    }

    return checkVLNVs(item->getVLNVs());
}
