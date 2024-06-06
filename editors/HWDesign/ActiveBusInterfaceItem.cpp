//-----------------------------------------------------------------------------
// File: ActiveBusInterfaceItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------

#include "ActiveBusInterfaceItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/NamelabelWidth.h>
#include <editors/common/GraphicsItemLabel.h>
#include <KactusAPI/include/BusInterfaceUtilities.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QFont>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::ActiveBusInterfaceItem()
//-----------------------------------------------------------------------------
ActiveBusInterfaceItem::ActiveBusInterfaceItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library,
    HWComponentItem* parent):
BusInterfaceEndPoint(busIf, parent->componentModel(), library, parent)
{
    Q_ASSERT_X(busIf, "ActiveBusInterfaceItem constructor", "Null BusInterface pointer given as parameter");

    setIsDraft(parent->isDraft());

    ActiveBusInterfaceItem::updateInterface();

}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::updateName()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::updateName(QString const& previousName, QString const& newName)
{
    if (previousName.compare(newName) != 0)
    {
        encompassingComp()->getComponentInstance()->removeBusInterfacePosition(previousName);
        encompassingComp()->getComponentInstance()->updateBusInterfacePosition(newName, pos());
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::createMoveCommandForClashedItem()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
    DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand)
{
    if (endPoint)
    {
        auto hwEndPoint = dynamic_cast<HWConnectionEndpoint*>(endPoint);
        if (hwEndPoint && endPoint->pos() != endPointPosition)
        {
            new PortMoveCommand(hwEndPoint, endPointPosition, diagram, parentCommand.data());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::canConnectToInterface()
//-----------------------------------------------------------------------------
bool ActiveBusInterfaceItem::canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const
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
        QSharedPointer<const BusDefinition> busDefinition = getLibraryAccess()->getModelReadOnly(
            getBusInterface()->getBusType()).dynamicCast<const BusDefinition>();
        if (busDefinition)
        {
            if (BusInterfaceUtilities::hasMatchingBusDefinitions(
                busDefinition, otherInterface->getBusType(), getLibraryAccess()))
            {
                QList<General::InterfaceMode> compatibleModes = getOpposingModes(getBusInterface());
                compatibleModes.append(General::SYSTEM);

                General::InterfaceMode otherMode = otherInterface->getInterfaceMode();

                return getBusInterface()->getInterfaceMode() !=
                    General::MONITOR && compatibleModes.contains(otherMode);
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool ActiveBusInterfaceItem::isHierarchical() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::setTemporary()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::setTemporary(bool temp)
{
    setTypeLocked(!temp);
    HWConnectionEndpoint::setTemporary(temp);
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool ActiveBusInterfaceItem::isDirectionFixed() const noexcept
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::setLabelPosition()
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
// Function: ActiveBusInterfaceItem::checkDirection()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::checkDirection()
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
// Function: ActiveBusInterfaceItem::getNameLength()
//-----------------------------------------------------------------------------
qreal ActiveBusInterfaceItem::getNameLength()
{
    QFont font = getNameLabel()->font();
    return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::shortenNameLabel(qreal width)
{
    QString nameLabelText = NamelabelWidth::setNameLabel(name(), getNameLabel()->font(), width);
    getNameLabel()->setText(nameLabelText);

    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant ActiveBusInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
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
        updateConnectionPositions();
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
// Function: ActiveBusInterfaceItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void ActiveBusInterfaceItem::moveItemByMouse()
{
    encompassingComp()->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::createMouseMoveCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> ActiveBusInterfaceItem::createMouseMoveCommand(DesignDiagram* diagram)
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
// Function: ActiveBusInterfaceItem::getOpposingModes()
//-----------------------------------------------------------------------------
QList<General::InterfaceMode> ActiveBusInterfaceItem::getOpposingModes(QSharedPointer<BusInterface> busIf) const
{
    QList<General::InterfaceMode> possibleModes;

    General::InterfaceMode sourceMode = busIf->getInterfaceMode();

    if (sourceMode == General::INTERFACE_MODE_COUNT)
    {
        return possibleModes;
    }

    auto busDef = getLibraryAccess()->getModelReadOnly<BusDefinition>(busIf->getBusType());
    if (busDef != nullptr && busDef->getDirectConnection())
    {
        possibleModes.append(General::getCompatibleInterfaceModesForActiveInterface(sourceMode));
    }
    else
    {
        possibleModes.append(General::getCompatibleInterfaceMode(sourceMode));
    }

    return possibleModes;
}

//-----------------------------------------------------------------------------
// Function: ActiveBusInterfaceItem::getCurrentPosition()
//-----------------------------------------------------------------------------
QPointF ActiveBusInterfaceItem::getCurrentPosition() const
{
    return pos();
}
