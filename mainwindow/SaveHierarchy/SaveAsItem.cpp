//-----------------------------------------------------------------------------
// File: SaveAsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.05.2015
//
// Description:
// Tree item for representing a hierarchical save row.
//-----------------------------------------------------------------------------

#include "SaveAsItem.h"

//-----------------------------------------------------------------------------
// Function: SaveAsItem::SaveAsItem()
//-----------------------------------------------------------------------------
SaveAsItem::SaveAsItem(QTreeWidgetItem *parent): QTreeWidgetItem(parent)
{

}

//-----------------------------------------------------------------------------
// Function: SaveAsItem::~SaveAsItem()
//-----------------------------------------------------------------------------
SaveAsItem::~SaveAsItem()
{

}

//-----------------------------------------------------------------------------
// Function: SaveAsItem::setData()
//-----------------------------------------------------------------------------
void SaveAsItem::setData(int column, int role, QVariant const& value)
{
    if (column == 1 && role == Qt::CheckStateRole)
    {
        bool unchecked = (value.toBool() == false);

        if (unchecked || !parent() || parent()->checkState(column) == Qt::Checked) 
        {
            QTreeWidgetItem::setData(column, role, value);
        }

        for (int i = 0; i < childCount(); i++)
        {
            child(i)->setData(column, role, value);
            child(i)->setDisabled(unchecked);
        }
    }
    else
    {
        QTreeWidgetItem::setData(column, role, value);
    }
}
