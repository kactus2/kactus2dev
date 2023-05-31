//-----------------------------------------------------------------------------
// File: diagraminterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#include "HierarchicalBusInterfaceItem.h"

#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include "columnview/HWColumn.h"

#include <common/IEditProvider.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/GraphicsItemLabel.h>
#include <KactusAPI/include/BusInterfaceUtilities.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::HierarchicalBusInterfaceItem()
//-----------------------------------------------------------------------------
HierarchicalBusInterfaceItem::HierarchicalBusInterfaceItem(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, QSharedPointer<InterfaceGraphicsData> dataGroup, LibraryInterface* library,
    QGraphicsItem *parent):
BusInterfaceEndPoint(busIf, component, library, parent),
dataGroup_(dataGroup)
{
    HierarchicalBusInterfaceItem::setTemporary(busIf == 0);
    setTypeLocked(busIf != 0 && busIf->getInterfaceMode() != General::INTERFACE_MODE_COUNT);
    setPolygon(HierarchicalBusInterfaceItem:: getDirectionOutShape());

    dataGroup_->setName(busIf->name().toStdString());

    if (dataGroup_->hasPosition())
    {
        setPos(dataGroup_->getPosition());
    }

    if (dataGroup_->hasDirection())
    {
        HierarchicalBusInterfaceItem::setDirection(dataGroup_->getDirection());
    }
    else
    {
        dataGroup_->setDirection(HierarchicalBusInterfaceItem::getDirection());
    }

	getNameLabel()->setRotation(-rotation());

    HierarchicalBusInterfaceItem::updateInterface();
}


//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::updateName()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::updateName(QString const&, QString const& newName)
{
    dataGroup_->setName(newName.toStdString());
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::createMoveCommandForClashedItem()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
    DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand)
{
    if (endPoint && endPoint->isHierarchical() && endPoint->pos() != endPointPosition)
    {
        auto itemColumn = dynamic_cast<HWColumn*>(endPoint->parentItem());
        if (itemColumn)
        {
            new ItemMoveCommand(endPoint, endPointPosition, itemColumn, diagram, parentCommand.data());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::canConnectToInterface()
//-----------------------------------------------------------------------------
bool HierarchicalBusInterfaceItem::canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const
{
    QSharedPointer<BusInterface> bus = getBusInterface();
    QSharedPointer<const BusDefinition> busDefinition;
    QSharedPointer<BusInterface> otherInterface = otherEndPoint->getBusInterface();

    if (bus)
    {
        busDefinition = getLibraryAccess()->getModelReadOnly(bus->getBusType()).dynamicCast<const BusDefinition>();
    }

    return (otherInterface &&
            (getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
            !otherInterface->getBusType().isValid() ||
            (busDefinition && BusInterfaceUtilities::hasMatchingBusDefinitions(
                busDefinition, otherInterface->getBusType(), getLibraryAccess()))));
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::isHierarchical()
//-----------------------------------------------------------------------------
bool HierarchicalBusInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::setDirection()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::setDirection(QVector2D const& dir)
{
    BusInterfaceEndPoint::setDirection(dir);

    dataGroup_->setDirection(dir);

    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        getNameLabel()->setPos(-nameHeight / 2 + 2, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        getNameLabel()->setPos(nameHeight / 2 - 2, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::getDataExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> HierarchicalBusInterfaceItem::getDataExtension() const
{
    return dataGroup_;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant HierarchicalBusInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());

        dataGroup_->setDirection(getDirection());
    }
    else if (change == ItemScenePositionHasChanged)
    {   
        dataGroup_->setPosition(value.toPointF());

        updateConnectionPositions();
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());
    setOldPosition(scenePos());
    Q_ASSERT(oldColumn_ != 0);

    saveOldPortPositions(scene()->items());
    
    beginUpdateConnectionPositions();
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void HierarchicalBusInterfaceItem::moveItemByMouse()
{
    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    auto column = dynamic_cast<HWColumn*>(parentItem());
    if (column)
    {
        column->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::createMouseMoveCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> HierarchicalBusInterfaceItem::createMouseMoveCommand(DesignDiagram* diagram)
{
    if (!oldColumn_)
    {
        return QSharedPointer<QUndoCommand>();
    }

    auto column = dynamic_cast<HWColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onReleaseItem(this);

    QSharedPointer<QUndoCommand> cmd;

    // Check if the interface position was really changed.
    if (getOldPosition() != scenePos())
    {
        cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, getOldPosition(), oldColumn_, diagram));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    return cmd;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItem::getCurrentPosition()
//-----------------------------------------------------------------------------
QPointF HierarchicalBusInterfaceItem::getCurrentPosition() const
{
    return scenePos();
}
