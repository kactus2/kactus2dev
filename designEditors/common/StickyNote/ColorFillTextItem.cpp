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

#include <QObject>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::ColorFillTextItem()
//-----------------------------------------------------------------------------
ColorFillTextItem::ColorFillTextItem(QGraphicsItem* parent):
QGraphicsTextItem(parent),
fillColor_()
{ 
    setFlag(ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditorInteraction);

    connect(document(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);    
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

//-----------------------------------------------------------------------------
// Function: ColorFillTextItem::focusOutEvent()
//-----------------------------------------------------------------------------
void ColorFillTextItem::focusOutEvent(QFocusEvent *event)
{
    if (document()->isModified())
    {
        emit contentChanged();
        document()->setModified(false);
    }

    QGraphicsTextItem::focusOutEvent(event);
}
