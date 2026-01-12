//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include "HWMappingItem.h"
#include "SWPortItem.h"
#include "SystemDesignDiagram.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>


#include <QBrush>
#include <QUndoCommand>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

//-----------------------------------------------------------------------------
// Function: SWComponentItem::SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
                                 QSharedPointer<ComponentInstance> instance):
SystemComponentItem(QRectF(-COMPONENTWIDTH / 2, 0, COMPONENTWIDTH, MIN_HEIGHT), libInterface, instance,
    component, nullptr)
{
    setFlag(ItemIsMovable);

    updateComponent();

    updateSize();

}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::~SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::~SWComponentItem()
{
    // Remove this item from the stack where it resides.
    IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (stack != 0)
    {
        stack->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SWComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    setZValue(1001.0);
    ComponentItem::mousePressEvent(event);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IGraphicsItemStack*>(parentItem());
    oldStackPos_ = parentItem()->scenePos();

    // Begin the position update for all connections.
    for (QGraphicsItem *item : scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != nullptr)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0 || diagram->isProtected())
    {
        return;
    }

    setConnectionUpdateDisabled(true);

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStackPos_ + pos()));

        auto stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(stack != nullptr);
        stack->onMoveItem(this);
    }

    setConnectionUpdateDisabled(false);

    // Update the port connections manually.
    for (QGraphicsItem *item : childItems())
    {
        if (item->type() != SWPortItem::Type)
        {
            continue;
        }

        auto port = static_cast<SWPortItem*>(item);

        for (GraphicsConnection* conn : port->getConnections())
        {
            conn->updatePosition();
        }

        for (GraphicsConnection* conn : port->getOffPageConnector()->getConnections())
        {
            conn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldStack_ != nullptr)
    {
        auto stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(stack != nullptr);
        stack->onReleaseItem(this);

        if (scenePos() != oldPos_)
        {
            auto moveCommand = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldStack_, diagram));

            diagram->getEditProvider()->addCommand(moveCommand);

            moveCommand->redo();
        }

        oldStack_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged or not.
    if (isDraft())
    {
        setBrush(QBrush(KactusColors::DRAFT_COMPONENT));
    }
    else if (getLibraryInterface()->contains(vlnv))
    {
        setBrush(QBrush(KactusColors::SW_COMPONENT));
    }
    else
    {
        setBrush(QBrush(KactusColors::MISSING_COMPONENT));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (componentModel()->isHierarchical())
    {
        if (hierIcon_ == nullptr)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/hierarchy.png"), this);
            hierIcon_->setToolTip(tr("Hierarchical"));
            hierIcon_->setPos(58, 6);
        }
    }
    else if (hierIcon_ != 0)
    {
        delete hierIcon_;
        hierIcon_ = nullptr;
    }

    // Create imported icon if the component is an imported one.
    if (isImported())
    {
        if (importedIcon_ == nullptr)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/imported.png"), this);
            hierIcon_->setToolTip(tr("Auto-synced"));
            hierIcon_->setPos(-75, 6);
        }
    }
    else if (importedIcon_ != nullptr)
    {
        delete importedIcon_;
        importedIcon_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void SWComponentItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem::contextMenuEvent(event);

    QMenu menu;
    menu.addAction("Open Source...", this, SLOT(openCSource()));

    menu.exec(event->screenPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: openCSource()
//-----------------------------------------------------------------------------
void SWComponentItem::openCSource()
{
    // TODO: Auto-generate code.
    emit openCSource(this);
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::setFileSetRef()
//-----------------------------------------------------------------------------
void SWComponentItem::setFileSetRef(QString const& fileSetName)
{
    if (fileSetRef_ != fileSetName)
    {
        fileSetRef_ = fileSetName;
		getComponentInstance()->setFileSetRef( fileSetRef_ );
        emit fileSetRefChanged(fileSetRef_);
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getFileSetRef()
//-----------------------------------------------------------------------------
QString const& SWComponentItem::getFileSetRef() const
{
    return fileSetRef_;
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getParentHW()
//-----------------------------------------------------------------------------
HWMappingItem const* SWComponentItem::getLinkedHW() const
{
    return dynamic_cast<HWMappingItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::setPackaged()
//-----------------------------------------------------------------------------
void SWComponentItem::setPackaged()
{
    isDraft_ = false;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::setDraft()
//-----------------------------------------------------------------------------
void SWComponentItem::setDraft()
{
    isDraft_ = true;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::isDraft()
//-----------------------------------------------------------------------------
bool SWComponentItem::isDraft() const
{
    return isDraft_;
}

