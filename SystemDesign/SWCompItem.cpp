//-----------------------------------------------------------------------------
// File: SWCompItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#include "SWCompItem.h"

#include "IComponentStack.h"
#include "SystemDesignDiagram.h"

#include <models/component.h>

#include <QBrush>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
// Function: SWCompItem::SWCompItem()
//-----------------------------------------------------------------------------
SWCompItem::SWCompItem(QSharedPointer<Component> component,
                       QString const& instanceName,
                       QString const& displayName,
                       QString const& description,
                       QMap<QString, QString> const& configurableElementValues,
                       unsigned int id)
    : ComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, MIN_HEIGHT), component, instanceName,
                    displayName, description, configurableElementValues, 0),
      id_(id),
      oldStack_(0),
      oldPos_()
{
    setFlag(ItemIsMovable);
    setBrush(QBrush(QColor(0xce,0xdf,0xff)));

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: SWCompItem::~SWCompItem()
//-----------------------------------------------------------------------------
SWCompItem::~SWCompItem()
{
    // Remove this item from the stack where it resides.
    IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());

    if (stack != 0)
    {
        stack->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IComponentStack*>(parentItem());
    oldStackPos_ = parentItem()->scenePos();

    // TODO: Begin the position update for the connections.
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<SystemDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStackPos_ + pos()));

        IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldStack_ != 0)
    {
        IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onReleaseItem(this);

        oldStack_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        //         if (scenePos() != oldPos_)
        //         {
        //             cmd = QSharedPointer<QUndoCommand>(new MappingCompMoveCommand(this, oldPos_));
        //         }
        //         else
        //         {
        //             cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        //         }
        // 
        //         // TODO: End the position update for the connections and clear the list.
        //         
        //         // Add the undo command to the edit stack only if it has at least some real changes.
        //         if (cmd->childCount() > 0 || scenePos() != oldPos_)
        //         {
        //             static_cast<SystemDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        //         }
    }
}
