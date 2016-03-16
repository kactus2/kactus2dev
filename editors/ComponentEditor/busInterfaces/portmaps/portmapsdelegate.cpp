//-----------------------------------------------------------------------------
// File: portmapsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 13.04.2011
//
// Description:
//  Delegate to provide editors to edit port maps.
//-----------------------------------------------------------------------------

#include "portmapsdelegate.h"

//-----------------------------------------------------------------------------
// Function: PortMapsDelegate::PortMapsDelegate()
//-----------------------------------------------------------------------------
PortMapsDelegate::PortMapsDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsDelegate::~PortMapsDelegate()
//-----------------------------------------------------------------------------
PortMapsDelegate::~PortMapsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize PortMapsDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}
