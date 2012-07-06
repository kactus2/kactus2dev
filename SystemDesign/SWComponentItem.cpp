//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include "HWMappingItem.h"
#include "SWPortItem.h"
#include "SystemMoveCommands.h"
#include "SystemDesignDiagram.h"

#include <designwidget/HWMoveCommands.h>

#include <models/component.h>
#include <models/model.h>

#include <LibraryManager/libraryinterface.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/GenericEditProvider.h>

#include <QBrush>
#include <QUndoCommand>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

//-----------------------------------------------------------------------------
// Function: SWComponentItem::SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(LibraryInterface* libInterface,
                       QSharedPointer<Component> component,
                       QString const& instanceName,
                       QString const& displayName,
                       QString const& description,
                       QMap<QString, QString> const& configurableElementValues)
    : SystemComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, MIN_HEIGHT), libInterface, component, instanceName,
                      displayName, description, configurableElementValues, 0),
      oldStack_(0),
      oldPos_(),
      hierIcon_(0),
      importedIcon_(0)
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
    setZValue(1001.0);
    ComponentItem::mousePressEvent(event);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IGraphicsItemStack*>(parentItem());
    oldStackPos_ = parentItem()->scenePos();

    // Begin the position update for the connections.
    foreach (QGraphicsItem* item, childItems())
    {
        if (item->type() == SWPortItem::Type)
        {
            SWPortItem* port = qgraphicsitem_cast<SWPortItem*>(item);

            foreach (GraphicsConnection* conn, port->getConnections())
            {
                conn->beginUpdatePosition();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<SystemDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    setConnectionUpdateDisabled(true);

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStackPos_ + pos()));

        IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onMoveItem(this);
    }

    setConnectionUpdateDisabled(false);

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != SWPortItem::Type)
        {
            continue;
        }

        SWPortItem* port = static_cast<SWPortItem*>(item);

        foreach (GraphicsConnection* conn, port->getConnections())
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
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldStack_ != 0)
    {
        IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldStack_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the connections and clear the list.
        foreach (QGraphicsItem* item, childItems())
        {
            if (item->type() == SWPortItem::Type)
            {
                SWPortItem* port = qgraphicsitem_cast<SWPortItem*>(item);

                foreach (GraphicsConnection* conn, port->getConnections())
                {
                    conn->endUpdatePosition(cmd.data());
                }
            }
        }
        
        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }

        oldStack_ = 0;
        revalidateConnections();
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        if (!getLibraryInterface()->contains(*vlnv))
        {
            setBrush(QBrush(QColor(0xe8, 0xc5, 0xc5)));
        }
        else
        {
            setBrush(QBrush(QColor(0xce,0xdf,0xff)));
        }
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (componentModel()->hasSWViews())
    {
        if (hierIcon_ == 0)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/hierarchy.png"), this);
            hierIcon_->setToolTip(tr("Hierarchical"));
            hierIcon_->setPos(58, 6);
        }
    }
    else if (hierIcon_ != 0)
    {
        delete hierIcon_;
        hierIcon_ = 0;
    }

    // Create imported icon if the component is an imported one.
    if (isImported())
    {
        if (importedIcon_ == 0)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/imported.png"), this);
            hierIcon_->setToolTip(tr("Auto-synced"));
            hierIcon_->setPos(-75, 6);
        }
    }
    else if (importedIcon_ != 0)
    {
        delete importedIcon_;
        importedIcon_ = 0;
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
