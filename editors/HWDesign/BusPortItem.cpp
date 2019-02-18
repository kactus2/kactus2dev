//-----------------------------------------------------------------------------
// File: BusPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------

#include "BusPortItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/NamelabelWidth.h>
#include <editors/common/GraphicsItemLabel.h>
#include <editors/common/BusInterfaceCompatibility.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QFont>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusPortItem::BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent):
BusInterfaceEndPoint(busIf, parent->componentModel(), parent),
library_(library)
{
    Q_ASSERT_X(busIf, "BusPortItem constructor", "Null BusInterface pointer given as parameter");

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::~BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::~BusPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::updateName()
//-----------------------------------------------------------------------------
void BusPortItem::updateName(QString const& previousName, QString const& newName)
{
    if (previousName.compare(newName) != 0)
    {
        encompassingComp()->getComponentInstance()->removeBusInterfacePosition(previousName);
        encompassingComp()->getComponentInstance()->updateBusInterfacePosition(newName, pos());
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::createMoveCommandForClashedItem()
//-----------------------------------------------------------------------------
void BusPortItem::createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
    DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand)
{
    if (endPoint)
    {
        HWConnectionEndpoint* hwEndPoint = dynamic_cast<HWConnectionEndpoint*>(endPoint);
        if (hwEndPoint && endPoint->pos() != endPointPosition)
        {
            new PortMoveCommand(hwEndPoint, endPointPosition, diagram, parentCommand.data());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::canConnectToInterface()
//-----------------------------------------------------------------------------
bool BusPortItem::canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const
{
    QSharedPointer<BusInterface> otherInterface = otherEndPoint->getBusInterface();
    if (otherInterface)
    {
        if (otherEndPoint->isHierarchical())
        {
            return (otherInterface->getInterfaceMode() == getBusInterface()->getInterfaceMode() ||
                otherInterface->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
                !getBusInterface()->getBusType().isValid());
        }

        // If only one port has a bus definition defined at most, then the end points can be connected.
        else if (!(getBusInterface()->getBusType().isValid() && otherInterface->getBusType().isValid()))
        {
            return true;
        }

        // Otherwise make sure that the bus and abstraction definitions are of the same type.
        QSharedPointer<const BusDefinition> busDefinition =
            library_->getModelReadOnly(getBusInterface()->getBusType()).dynamicCast<const BusDefinition>();
        if (BusInterfaceUtilities::hasMatchingBusDefinitions(busDefinition, otherInterface->getBusType(), library_))
        {
            QList<General::InterfaceMode> compatibleModes = getOpposingModes(getBusInterface());
            compatibleModes.append(General::SYSTEM);

            General::InterfaceMode otherMode = otherInterface->getInterfaceMode();

            return getBusInterface()->getInterfaceMode() !=
                General::MONITOR && compatibleModes.contains(otherMode);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool BusPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setTemporary()
//-----------------------------------------------------------------------------
void BusPortItem::setTemporary(bool temp)
{
    setTypeLocked(!temp);
    HWConnectionEndpoint::setTemporary(temp);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool BusPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void BusPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

    // Check if the port is directed to the left.
    if (pos().x() < 0)
    {
        getNameLabel()->setPos(nameHeight/2, GridSize);
    }
    else // The port is directed to the right.
    {
        getNameLabel()->setPos(-nameHeight/2, GridSize + nameWidth);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::checkDirection()
//-----------------------------------------------------------------------------
void BusPortItem::checkDirection()
{
    // Check if the port is directed to the left
    if (pos().x() < 0)
    {
        setDirection(QVector2D(-1.0f, 0.0f));
    }
    else // The port is directed to the right.
    {
        setDirection(QVector2D(1.0f, 0.0f));
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getNameLength()
//-----------------------------------------------------------------------------
qreal BusPortItem::getNameLength()
{
    QFont font = getNameLabel()->font();
    return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void BusPortItem::shortenNameLabel(qreal width)
{
    QString nameLabelText = NamelabelWidth::setNameLabel(name(), getNameLabel()->font(), width);
    getNameLabel()->setText(nameLabelText);

    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant BusPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        QPointF pos = value.toPointF();

        if (parentItem())
        {
            QRectF parentRect = encompassingComp()->rect();

            if (pos.x() < 0)
            {
                pos.setX(parentRect.left());
            }
            else
            {
                pos.setX(parentRect.right());
            }
        }

        return snapPointToGrid(pos);
    }
    else if (change == ItemPositionHasChanged && parentItem())
    {
        checkDirection();
        setLabelPosition();
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::mousePressEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);  

    if (sceneIsLocked())
    {
        return;
    }

    setOldPosition(pos());
    saveOldPortPositions(parentItem()->childItems());

    beginUpdateConnectionPositions();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void BusPortItem::moveItemByMouse()
{
    encompassingComp()->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::createMouseMoveCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> BusPortItem::createMouseMoveCommand(DesignDiagram* diagram)
{
    QSharedPointer<QUndoCommand> moveUndoCommand;

    // Check if the port position was really changed.
    if (getOldPosition() != pos())
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, getOldPosition(), diagram));
    }
    else
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    return moveUndoCommand;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOpposingModes()
//-----------------------------------------------------------------------------
QList<General::InterfaceMode> BusPortItem::getOpposingModes(QSharedPointer<BusInterface> busIf) const
{
    QList<General::InterfaceMode> possibleModes;

    General::InterfaceMode sourceMode = busIf->getInterfaceMode();

    if (sourceMode == General::INTERFACE_MODE_COUNT)
    {
        return possibleModes;
    }

    possibleModes.append(General::getCompatibleInterfaceMode(sourceMode));

    if (sourceMode == General::MASTER)
    {
        QSharedPointer<BusDefinition const> busDef = 
            library_->getModelReadOnly(busIf->getBusType()).dynamicCast<BusDefinition const>();
        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::SLAVE);      
        }
    }
    else if (sourceMode == General::SLAVE)
    {
        QSharedPointer<BusDefinition const> busDef = 
            library_->getModelReadOnly(busIf->getBusType()).dynamicCast<BusDefinition const>();
        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::MASTER);      
        }
    }

    return possibleModes;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getCurrentPosition()
//-----------------------------------------------------------------------------
QPointF BusPortItem::getCurrentPosition() const
{
    return pos();
}
