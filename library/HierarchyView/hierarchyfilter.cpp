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

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: HierarchyFilter::HierarchyFilter()
//-----------------------------------------------------------------------------
HierarchyFilter::HierarchyFilter(LibraryInterface* libraryAccess, QObject *parent):
LibraryFilter(libraryAccess, parent)
{
    setRecursiveFilteringEnabled(false);
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

	auto item = static_cast<HierarchyItem const*>(itemIndex.internalPointer());

    if (checkValidity(item->isValid()) == false)
    {
        return false;
    }

    // Display only non-duplicate children of root.
    if (item->parentIsRoot() && item->isDuplicate())
    {
        return false;
    }
    
    if (auto document = getLibraryInterface()->getModelReadOnly(item->getVLNV()); 
        !checkTags(document))
    {
        return false;
    }

    if (HierarchyItem::ObjectType itemType = item->type(); 
        itemType == HierarchyItem::COMPONENT && !type().components_)
    {
        return false;
    }

    else if (itemType == HierarchyItem::CATALOG && !type().catalogs_)
    {
        return false;
    }

    else if (itemType == HierarchyItem::APIDEFINITION || itemType == HierarchyItem::COMDEFINITION)
    {
        if (!type().apis_ || !implementation().sw_)
        {
            return false;
        }

    }

    else if (itemType == HierarchyItem::BUSDEFINITION || itemType == HierarchyItem::ABSDEFINITION)
    {
        if (!type().buses_ || !implementation().hw_)
        {
            return false;
        }
    }

    if (QSharedPointer<Component const> component = item->component(); 
        component && (!checkFirmness(component) || !checkImplementation(component) || !checkHierarchy(component)))
    {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
