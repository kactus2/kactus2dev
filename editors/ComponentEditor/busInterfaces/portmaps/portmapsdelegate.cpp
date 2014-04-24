/* 
 *
 *  Created on: 13.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsdelegate.cpp
 */

#include "portmapsdelegate.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QLineEdit>
#include <QRegExpValidator>
#include <QRegExp>

static const int LOGICAL_COLUMN = 2;
static const int PHYSICAL_COLUMN = 3;

PortMapsDelegate::PortMapsDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

PortMapsDelegate::~PortMapsDelegate() {
}

//-----------------------------------------------------------------------------
// Function: PortMapsDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize PortMapsDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}

