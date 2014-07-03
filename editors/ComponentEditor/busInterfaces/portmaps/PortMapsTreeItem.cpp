//-----------------------------------------------------------------------------
// File: PortMapsTreeItem
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "PortMapsTreeItem.h"


#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::PortMapsTreeItem()
//-----------------------------------------------------------------------------
PortMapsTreeItem::PortMapsTreeItem() : 
    component_(0),
    parent_(0),    
    type_(ITEM_TYPE_ROOT),
    name_("root"),
    children_()
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::PortMapsTreeItem()
//-----------------------------------------------------------------------------
PortMapsTreeItem::PortMapsTreeItem(PortMapsTreeItem* parent, 
QSharedPointer<Component> component, 
QString const& name, ItemType type)
: parent_(parent),
component_(component),
type_(type),
name_(name),
children_()
{
}


//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::~PortMapsTreeItem()
//-----------------------------------------------------------------------------
PortMapsTreeItem::~PortMapsTreeItem()
{
    foreach (PortMapsTreeItem* item, children_)
    {
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::refresh()
//-----------------------------------------------------------------------------
void PortMapsTreeItem::refresh()
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::data()
//-----------------------------------------------------------------------------
QVariant PortMapsTreeItem::data(int) const
{
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsTreeItem::isValid() const
{    
    // Item is valid, if all childs are valid.
    for(int i = 0; i < getChildCount(); i++)
    {
        if (!getChild(i)->isValid())
        {
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsTreeItem::isValid(QStringList& errorList) const
{
    bool valid = true;
    // Item is valid, if all childs are valid.
    for(int i = 0; i < getChildCount(); i++)
    {
        if (!getChild(i)->isValid(errorList))
        {
            valid = false;
        }
    }
    return valid;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getChild()
//-----------------------------------------------------------------------------
PortMapsTreeItem* PortMapsTreeItem::getChild(int index)
{
    if (index < 0 || index >= getChildCount())
    {
        return 0;
    }

    return children_[index];
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getChild()
//-----------------------------------------------------------------------------
PortMapsTreeItem const* PortMapsTreeItem::getChild(int index) const
{
    if (index < 0 || index >= getChildCount())
    {
        return 0;
    }

    return children_[index];
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getChildCount()
//-----------------------------------------------------------------------------
int PortMapsTreeItem::getChildCount() const
{
    return children_.size();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getParent()
//-----------------------------------------------------------------------------
PortMapsTreeItem* PortMapsTreeItem::getParent() const
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getIndex()
//-----------------------------------------------------------------------------
int PortMapsTreeItem::getIndex() const
{
    if (parent_)
    {
        for (int i = 0; i < parent_->getChildCount(); i++)
        {
            if (parent_->getChild(i) == this)
            {
                return i;
            }
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getType()
//-----------------------------------------------------------------------------
PortMapsTreeItem::ItemType PortMapsTreeItem::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::getName()
//-----------------------------------------------------------------------------
QString PortMapsTreeItem::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::addChild()
//-----------------------------------------------------------------------------
void PortMapsTreeItem::addChild(PortMapsTreeItem* item)
{    
    children_.append(item);
    item->parent_ = this;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeItem::removeChild()
//-----------------------------------------------------------------------------
void PortMapsTreeItem::removeChild(PortMapsTreeItem* item)
{
    children_.removeOne(item);
    item->parent_ = 0;
}