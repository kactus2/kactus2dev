//-----------------------------------------------------------------------------
// File: ComponentEditorTreeSortProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2013
//
// Description:
// Sorting proxy model for Component Editor tree structure.
//-----------------------------------------------------------------------------

#include "ComponentEditorTreeSortProxyModel.h"
#include "componenteditorviewitem.h"

#include "componenteditoritem.h"

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeProxyModel()
//-----------------------------------------------------------------------------
ComponentEditorTreeProxyModel::ComponentEditorTreeProxyModel(QObject *parent) :
QSortFilterProxyModel(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentEditorTreeProxyModel()
//-----------------------------------------------------------------------------
ComponentEditorTreeProxyModel::~ComponentEditorTreeProxyModel()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeProxyModel::setRowVisibility()
//-----------------------------------------------------------------------------
void ComponentEditorTreeProxyModel::setRowVisibility(QStringList hiddenItemNames)
{
    hiddenItems_ = hiddenItemNames;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: lessThan()
//-----------------------------------------------------------------------------
bool ComponentEditorTreeProxyModel::lessThan(QModelIndex const& left, QModelIndex const& right) const
{
    ComponentEditorItem* leftItem = static_cast<ComponentEditorItem*>(left.internalPointer());
    ComponentEditorItem* rightItem = static_cast<ComponentEditorItem*>(right.internalPointer());

    // View item sorting.
    ComponentEditorViewItem* leftViewItem = dynamic_cast<ComponentEditorViewItem*>(leftItem);
    ComponentEditorViewItem* rightViewItem = dynamic_cast<ComponentEditorViewItem*>(rightItem);
    if (leftViewItem != 0 && rightViewItem != 0)
    {
        // Both hierarchical, sort alphabetically.
        if (leftViewItem->isHierarchical() && rightViewItem->isHierarchical())
        {
            return left.data().toString() < right.data().toString();
        }
        // Only left is hierarchical, precedes right.
        else if (leftViewItem->isHierarchical() )
        {
            return true;
        }
        // Only right is hierarchical, precedes left.
        else if (rightViewItem->isHierarchical())
        {
            return false;
        }
        // Both non-hierarchical, sort alphabetically.
        else
        {
            return left.data().toString() < right.data().toString();
        }
    }
    else
    {
        return left < right;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool ComponentEditorTreeProxyModel::filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const
{
	QModelIndex itemIndex = sourceModel()->index(sourceRow, 0, sourceParent);
	QString itemName = sourceModel()->data(itemIndex).toString();

	if (itemIsValidAndCanBeHidden(itemIndex) && hiddenItems_.contains( itemName ))
	{
		return false;
	}

	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeSortProxyModel::itemIsValidAndCanBeHidden()
//-----------------------------------------------------------------------------
bool ComponentEditorTreeProxyModel::itemIsValidAndCanBeHidden(QModelIndex const& index) const
{
	return !(index.isValid() && !static_cast<ComponentEditorItem*>(index.internalPointer())->isValid());
}
