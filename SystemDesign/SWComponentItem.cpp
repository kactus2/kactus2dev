//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all SW components visualized in a system design.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include "SWPortItem.h"
#include "IComponentStack.h"

#include <common/layouts/VCollisionLayout.h>

#include <models/component.h>
#include <models/ComInterface.h>
#include <models/ApiInterface.h>

//-----------------------------------------------------------------------------
// Function: SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(QRectF const& size, QSharedPointer<Component> component,
                                 QString const& instanceName, QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent)
    : ComponentItem(size, component, instanceName, displayName, description, configurableElementValues, parent),
      imported_(false)
{
    int portSpacing = 3 * GridSize;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;
    bool right = false;

    // Create graphics items for API interfaces.
    foreach (QSharedPointer<ApiInterface> apiIf, component->getApiInterfaces())
    {
        SWPortItem* port = new SWPortItem(apiIf, this);
        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right)
        {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        }
        else
        {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }

    // Create graphics items for COM interfaces.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        SWPortItem* port = new SWPortItem(comIf, this);
        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right)
        {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        }
        else
        {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }
}

//-----------------------------------------------------------------------------
// Function: ~SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::~SWComponentItem()
{

}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();
}


//-----------------------------------------------------------------------------
// Function: setImported()
//-----------------------------------------------------------------------------
void SWComponentItem::setImported(bool imported)
{
    imported_ = imported;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: isImported()
//-----------------------------------------------------------------------------
bool SWComponentItem::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void SWComponentItem::onAddPort(SWPortItem* port, bool right)
{
    if (right)
    {
        rightPorts_.append(port);
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        leftPorts_.append(port);
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void SWComponentItem::onMovePort(SWPortItem* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), std::max(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));

    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0.0)
    {
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::updateSize()
//-----------------------------------------------------------------------------
void SWComponentItem::updateSize()
{
    QRectF oldRect = rect();
    oldRect.setHeight(getHeight());

    setRect(oldRect);

    IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());

    if (stack != 0)
    {
        stack->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal SWComponentItem::getHeight() const
{
    // Update the component's size based on the port that is positioned at
    // the lowest level of them all.
    qreal maxY = 4 * GridSize;

    if (!leftPorts_.empty())
    {
        maxY = leftPorts_.back()->y();
    }

    if (!rightPorts_.empty())
    {
        maxY = std::max(maxY, rightPorts_.back()->y());
    }

    return (maxY + BOTTOM_MARGIN);
}
