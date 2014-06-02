//-----------------------------------------------------------------------------
// File: ColorFillTextItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Graphic item for text editing with color filled background.
//-----------------------------------------------------------------------------

#include "ColorFillTextItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <common/KactusColors.h>
#include <QMargins>

#include "../diagramgrid.h"
#include "../DesignDiagram.h"
#include <common/graphicsItems/commands/FloatingItemMoveCommand.h>
#include <common/GenericEditProvider.h>

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::ColorFillTextItem()
//-----------------------------------------------------------------------------
ColorFillTextItem::ColorFillTextItem(QGraphicsItem* parent):
QGraphicsTextItem(parent),
fillColor_()
{ 
    setFlag(ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditable);
}

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::~ColorFillTextItem()
//-----------------------------------------------------------------------------
ColorFillTextItem::~ColorFillTextItem()
{

}

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::setFill()
//-----------------------------------------------------------------------------
void ColorFillTextItem::setFill(QColor const& color)
{
    fillColor_ = color;
}

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::paint()
//-----------------------------------------------------------------------------
void ColorFillTextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->fillRect(boundingRect(), QBrush(fillColor_)); 
    
    QGraphicsTextItem::paint(painter, option, widget);
}



