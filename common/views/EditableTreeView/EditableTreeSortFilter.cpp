//-----------------------------------------------------------------------------
// File: EditableTreeSortfilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.12.2017
//
// Description:
// Sort filter model for editable tree view.
//-----------------------------------------------------------------------------

#include "EditableTreeSortFilter.h"

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::EditableTreeSortFilter()
//-----------------------------------------------------------------------------
EditableTreeSortFilter::EditableTreeSortFilter(QObject* parent):
QSortFilterProxyModel(parent)
{

}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::~EditableTreeSortFilter()
//-----------------------------------------------------------------------------
EditableTreeSortFilter::~EditableTreeSortFilter()
{

}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::onAddItem()
//-----------------------------------------------------------------------------
void EditableTreeSortFilter::onAddItem(QModelIndexList const& indexes)
{
    QModelIndex posToAdd;
    int rowCount = 1;

    if (!indexes.isEmpty())
    {
        QModelIndexList sortedIndexes = indexes;
        std::sort(sortedIndexes.begin(), sortedIndexes.end());
        posToAdd = mapToSource(indexes.first());

        int previousRow = indexes.first().row();
        for(QModelIndex const& index : sortedIndexes)
        {
            if (index.row() != previousRow)
            {
                ++rowCount;
            }

            previousRow = index.row();
        }
    }

    for (int i = 0; i < rowCount; ++i)
    {
        emit(addItem(posToAdd));
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::onAddSubItem()
//-----------------------------------------------------------------------------
void EditableTreeSortFilter::onAddSubItem(QModelIndexList const& parentIndexes)
{
    for(QModelIndex const& index : parentIndexes)
    {
        QModelIndex sourceParentIndex = mapToSource(index);
        emit addSubItem(sourceParentIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::onRemoveSelectedRows()
//-----------------------------------------------------------------------------
void EditableTreeSortFilter::onRemoveSelectedRows(QModelIndexList const& indexes)
{
    if (indexes.isEmpty())
    {
        return;
    }

    QModelIndexList removableIndexList = getRemovableIndexes(concatIndexesToOneColumn(indexes));
    std::sort(removableIndexList.begin(), removableIndexList.end());

    for (int i = removableIndexList.size() - 1; i >= 0; i--)
    {
        QModelIndex index = mapToSource(removableIndexList.at(i));
        emit removeItem(index);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::concatIndexesToOneColumn()
//-----------------------------------------------------------------------------
QModelIndexList EditableTreeSortFilter::concatIndexesToOneColumn(QModelIndexList const& indexes) const
{
    QModelIndexList firstColumnIndexes;

    for (auto const& index : indexes)
    {
        if (index.column() == 0)
        {
            firstColumnIndexes.append(index);
        }
        else
        {
            QModelIndex firstColumnIndex = index.sibling(index.row(), 0);
            if (!firstColumnIndexes.contains(firstColumnIndex))
            {
                firstColumnIndexes.append(firstColumnIndex);
            }
        }
    }

    return firstColumnIndexes;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::getRemovableIndexes()
//-----------------------------------------------------------------------------
QModelIndexList EditableTreeSortFilter::getRemovableIndexes(QModelIndexList const& indexes) const
{
    QModelIndexList removableIndexes;

    for (auto const& index : indexes)
    {
        if (indexIsRemovable(index, indexes))
        {
            removableIndexes.append(index);
        }
    }

    return removableIndexes;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::indexIsRemovable()
//-----------------------------------------------------------------------------
bool EditableTreeSortFilter::indexIsRemovable(QModelIndex const& index, QModelIndexList indexList) const
{
    if (index.parent().isValid())
    {
        QModelIndex const& parentIndex = index.parent();
        for (auto const& comparisonIndex : indexList)
        {
            if (parentIndex == comparisonIndex)
            {
                return false;
            }
        }

        return indexIsRemovable(parentIndex, indexList);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::onRemoveAllSubItemsFromIndexes()
//-----------------------------------------------------------------------------
void EditableTreeSortFilter::onRemoveAllSubItemsFromIndexes(QModelIndexList const& parentIndexes)
{
    if (!parentIndexes.isEmpty())
    {
        for(QModelIndex const& index : parentIndexes)
        {
            emit removeAllSubItemsFromIndex(mapToSource(index));
        }
    }
}
