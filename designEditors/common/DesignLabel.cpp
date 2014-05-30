//-----------------------------------------------------------------------------
// File: DesignLabel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "DesignLabel.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <common/KactusColors.h>
#include <QMargins>

#include "diagramgrid.h"
#include "DesignDiagram.h"
#include <common/graphicsItems/commands/FloatingItemMoveCommand.h>
#include <common/GenericEditProvider.h>

//-----------------------------------------------------------------------------
// Function: DesignLabel::DesignLabel()
//-----------------------------------------------------------------------------
DesignLabel::DesignLabel(QGraphicsItem* parent):
QGraphicsTextItem(parent),
oldPos_()
{ 
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditable);

    setTextWidth(GridSize * 8 * 2);    

    setOpacity(1.0);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::~DesignLabel()
//-----------------------------------------------------------------------------
DesignLabel::~DesignLabel()
{

}

//-----------------------------------------------------------------------------
// Function: DesignLabel::paint()
//-----------------------------------------------------------------------------
void DesignLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    
    painter->fillRect(option->rect, Qt::white);

    painter->setPen(KactusColors::HW_COMPONENT);
    painter->drawRect(boundingRect());

    painter->setPen(Qt::black);
    QGraphicsTextItem::paint(painter, option, widget);

    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::mousePressEvent()
//-----------------------------------------------------------------------------
void DesignLabel::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    oldPos_ = scenePos();

    QGraphicsTextItem::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DesignLabel::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    QGraphicsTextItem::mouseReleaseEvent(event);

    if (pos() != oldPos_)
    {
        QSharedPointer<FloatingItemMoveCommand> moveCommand(new FloatingItemMoveCommand(this, oldPos_));
        diagram->getEditProvider().addCommand(moveCommand, false);
    }
}
