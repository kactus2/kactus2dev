//-----------------------------------------------------------------------------
// File: StickyNote.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Sticky note for designs.
//-----------------------------------------------------------------------------

#include "StickyNote.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <common/KactusColors.h>
#include <QMargins>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <common/graphicsItems/commands/FloatingItemMoveCommand.h>
#include <common/GenericEditProvider.h>

#include "ColorFillTextItem.h"

//-----------------------------------------------------------------------------
// Function: DesignLabel::DesignLabel()
//-----------------------------------------------------------------------------
StickyNote::StickyNote(QGraphicsItem* parent):
QGraphicsItemGroup(parent),
oldPos_(),
textArea_(0)
{ 
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);

    setHandlesChildEvents(false);

    QColor topColor = QColor("lemonChiffon").darker(103);

    const int TOP_OFFSET = 15;
    const int DEFAULT_WIDTH = GridSize * 8 * 2;

    QGraphicsRectItem* glueTop = new QGraphicsRectItem(0, 0 , DEFAULT_WIDTH, TOP_OFFSET);
    QPen outlinePen;
    outlinePen.setStyle(Qt::NoPen);
    glueTop->setPen(outlinePen);
    glueTop->setBrush(QBrush(topColor));

    addToGroup(glueTop);

    textArea_ = new ColorFillTextItem();
    textArea_->setFill(QColor("lemonChiffon"));
    textArea_->setTextWidth(DEFAULT_WIDTH);
    textArea_->setPos(0, TOP_OFFSET);
  
    addToGroup(textArea_);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::~DesignLabel()
//-----------------------------------------------------------------------------
StickyNote::~StickyNote()
{

}

//-----------------------------------------------------------------------------
// Function: DesignLabel::mousePressEvent()
//-----------------------------------------------------------------------------
void StickyNote::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    oldPos_ = scenePos();

    QGraphicsItem::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void StickyNote::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    QGraphicsItem::mouseReleaseEvent(event);

    if (pos() != oldPos_)
    {
        QSharedPointer<FloatingItemMoveCommand> moveCommand(new FloatingItemMoveCommand(this, oldPos_));
        diagram->getEditProvider().addCommand(moveCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: StickyNote::beginEditing()
//-----------------------------------------------------------------------------
void StickyNote::beginEditing()
{
    textArea_->setFocus();
}
