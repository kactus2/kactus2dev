//-----------------------------------------------------------------------------
// File: ObjectSelectionListItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.07.2017
//
// Description:
// List item for VLNVs and files.
//-----------------------------------------------------------------------------

// #include "ObjectExportListItem.h"
#include "ObjectSelectionListItem.h"

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::ObjectSelectionListItem()
//-----------------------------------------------------------------------------
ObjectSelectionListItem::ObjectSelectionListItem(QIcon itemIcon, QString const& itemPath, bool isLocked,
    VLNV vlnv):
QListWidgetItem(itemIcon, itemPath),
itemVlnv_(vlnv),
itemPath_(itemPath),
type_(VLNVOJBECT),
locked_(isLocked)
{
    if (!vlnv.isValid())
    {
        type_ = ObjectSelectionListItem::FILE;
    }

    setCheckState(Qt::Checked);

    setFlags(Qt::NoItemFlags);
    if (!isLocked)
    {
        setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::~ObjectSelectionListItem()
//-----------------------------------------------------------------------------
ObjectSelectionListItem::~ObjectSelectionListItem()
{

}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::getVLNV()
//-----------------------------------------------------------------------------
VLNV ObjectSelectionListItem::getVLNV() const
{
    return itemVlnv_;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::getPath()
//-----------------------------------------------------------------------------
QString ObjectSelectionListItem::getPath() const
{
    return itemPath_;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::getType()
//-----------------------------------------------------------------------------
ObjectSelectionListItem::ItemType ObjectSelectionListItem::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ObjectSelectionListItem::itemIsLocked()
//-----------------------------------------------------------------------------
bool ObjectSelectionListItem::itemIsLocked() const
{
    return locked_;
}
