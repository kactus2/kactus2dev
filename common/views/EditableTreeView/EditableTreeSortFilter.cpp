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
        qSort(sortedIndexes);
        posToAdd = mapToSource(indexes.first());

        int previousRow = indexes.first().row();
        foreach (QModelIndex index, sortedIndexes)
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
    foreach (QModelIndex index, parentIndexes)
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

    QModelIndexList sortedIndexList = indexes;
    qSort(sortedIndexList);

    int previousRow = sortedIndexList.first().row();
    int rowCount = 1;
    foreach (QModelIndex index, sortedIndexList)
    {
        if (previousRow != index.row())
        {
            ++rowCount;
        }
        previousRow = index.row();
    }

    for (int i = 0; i < rowCount; ++i)
    {
        QModelIndex index = sortedIndexList.first();
        index = mapToSource(index);

        emit removeItem(index);
    }
}

//-----------------------------------------------------------------------------
// Function: EditableTreeSortFilter::onRemoveAllSubItemsFromIndexes()
//-----------------------------------------------------------------------------
void EditableTreeSortFilter::onRemoveAllSubItemsFromIndexes(QModelIndexList const& parentIndexes)
{
    if (!parentIndexes.isEmpty())
    {
        foreach (QModelIndex index, parentIndexes)
        {
            if (!index.parent().isValid())
            {
                emit removeAllSubItemsFromIndex(mapToSource(index));
            }
        }
    }
}
