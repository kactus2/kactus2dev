//-----------------------------------------------------------------------------
// File: SystemDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// System design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#include "SystemDesignDiagram.h"

#include "../EndpointDesign/SystemChangeCommands.h"
#include "../EndpointDesign/SystemAddCommands.h"

#include "HWMappingItem.h"
#include "SWCompItem.h"
#include "SystemDesignWidget.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>

#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/ComponentItem.h>

#include <models/SWInstance.h>
#include <models/component.h>
#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/model.h>
#include <models/businterface.h>
#include <models/fileset.h>
#include <models/file.h>
#include <mainwindow/mainwindow.h>

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/LibraryUtils.h>

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::SystemDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                                         GenericEditProvider& editProvider,
                                         SystemDesignWidget* parent)
    : DesignDiagram(lh, mainWnd, editProvider, parent),
      parent_(parent),
      nodeIDFactory_(),
      layout_(),
      dragSW_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::~SystemDesignDiagram()
{
    // TODO: Delete connections.
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::openDesign(QSharedPointer<Design> design)
{
    // Update the design.
    updateSystemDesign(getLibraryInterface(),
                       QFileInfo(getLibraryInterface()->getPath(*design->getVlnv())).path(),
                       getEditedComponent()->getHierRef("kts_hw_ref"), *design);

    // Create the column layout.
    layout_ = QSharedPointer<SystemColumnLayout>(new SystemColumnLayout(this));
    connect(layout_.data(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    if (design->getColumns().isEmpty())
    {
        layout_->addColumn("SW Components");
        layout_->addColumn("SW Components");
    }
    else
    {
        foreach(ColumnDesc desc, design->getColumns())
        {
            layout_->addColumn(desc.name);
        }
    }

    unsigned int colIndex = 0;

    // Create (HW) component instances.
    foreach (Design::ComponentInstance const& instance, design->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.componentRef);

        if (!libComponent)
        {
            emit errorMessage(tr("The component %1 was not found in the library").arg(
                instance.componentRef.getName()).arg(design->getVlnv()->getName()));
            continue;
        }

        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        int id = instance.mcapiNodeID;

        // If the id is not set, generate a new ID. TODO: The id should be set in the generation phase.
        if (id == -1)
        {
            id = nodeIDFactory_.getID();
        }
        else
        {
            nodeIDFactory_.usedID(id);
        }

        HWMappingItem* item = new HWMappingItem(component, instance.instanceName,
                                                instance.displayName, instance.description,
                                                instance.configurableElementValues, id);
        item->setImported(instance.imported);

        connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

        // Check if the position is not found.
        if (instance.position.isNull())
        {
            layout_->getColumns().at(colIndex)->addItem(item);
            colIndex = 1 - colIndex;
        }
        else
        {
            item->setPos(instance.position);

            SystemColumn* column = layout_->findColumnAt(instance.position);

            if (column != 0)
            {
                column->addItem(item, true);
            }
            else
            {
                layout_->getColumns().at(colIndex)->addItem(item);
                colIndex = 1 - colIndex;
            }
        }

        addInstanceName(instance.instanceName);
    }
    
    // Create SW instances.
    foreach (SWInstance const& instance, design->getSWInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());

        if (!libComponent)
        {
            emit errorMessage(tr("The SW component %1 was not found in the library").arg(
                                    instance.getComponentRef().getName()).arg(design->getVlnv()->getName()));
            continue;
        }

        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        // TODO: Determine ID.
        int id = 0;

        SWCompItem* item = new SWCompItem(component, instance.getInstanceName(),
                                          instance.getDisplayName(), instance.getDescription(),
                                          QMap<QString, QString>(), id);
        //item->setImported(instance.imported);
        item->setPos(instance.getPosition());

        connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

        if (instance.getMapping().isEmpty())
        {
            // Check if the position is not found.
            if (instance.getPosition().isNull())
            {
                layout_->getColumns().at(colIndex)->addItem(item);
                colIndex = 1 - colIndex;
            }
            else
            {
                SystemColumn* column = layout_->findColumnAt(instance.getPosition());

                if (column != 0)
                {
                    column->addItem(item, true);
                }
                else
                {
                    layout_->getColumns().at(colIndex)->addItem(item);
                    colIndex = 1 - colIndex;
                }
            }
        }
        else
        {
            // Otherwise the mapping has been specified. Find the corresponding HW component instance.
            HWMappingItem* mappingItem = getHWComponent(instance.getMapping());

            if (mappingItem != 0)
            {
                mappingItem->addItem(item, true);
            }
        }

        addInstanceName(instance.getInstanceName());
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    layout_->updatePositions();
}

//-----------------------------------------------------------------------------
// Function: dragEnterEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragSW_ = false;

    if (!isProtected() && event->mimeData()->hasFormat("data/vlnvptr"))
    {
        event->acceptProposedAction();

        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        if (vlnv->getType() == VLNV::COMPONENT)
        {
            // Determine the component type.
            QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(*vlnv);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            // component with given vlnv was not found
            if (!comp) {
                emit errorMessage(tr("Component with the VLNV %1:%2:%3:%4 was not found in the library.").arg(
                    vlnv->getVendor()).arg(
                    vlnv->getLibrary()).arg(
                    vlnv->getName()).arg(
                    vlnv->getVersion()));
                return;
            }

            // Check if the firmness does not match with the edited component.
            if (comp->getComponentImplementation() != KactusAttribute::KTS_SW)
            {
                return;
            }

            dragSW_ = true;
            comp.clear();
        }
        // TODO:
        else if (vlnv->getType() == VLNV::COMDEFINITION)
        {
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragMoveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    if (dragSW_)
    {
        // Find the top-most item under the cursor.
        QGraphicsItem* item = 0;
        QList<QGraphicsItem*> itemList = items(event->scenePos());

        if (!itemList.empty())
        {
            item = itemList.front();

            if (item->type() == QGraphicsTextItem::Type)
            {
                item = item->parentItem();
            }
        }

        // If the underlying object is a HW mapping item, accept the drag here.
        if (item != 0 && item->type() == HWMappingItem::Type)
        {
            event->setDropAction(Qt::CopyAction);
        }
        else
        {
            // Otherwise check which column should receive the SW component.
            SystemColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

            if (column != 0)
            {
                event->setDropAction(Qt::CopyAction);
            }
            else
            {
                event->setDropAction(Qt::IgnoreAction);
            }
        }

        event->accept();
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

//-----------------------------------------------------------------------------
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragSW_ = false;
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Check if the dragged item was a valid one.
    if (dragSW_)
    {
        // Determine the component stack who gets the component (either HW mapping item or a system column).
        IComponentStack* stack = 0;

        QList<QGraphicsItem*> itemList = items(event->scenePos());
        
        if (!itemList.empty())
        {
            QGraphicsItem* item = itemList.back();

            if (item != 0 && item->type() == HWMappingItem::Type)
            {
                stack = static_cast<HWMappingItem*>(item);
            }
        }

        if (stack == 0)
        {
            stack = layout_->findColumnAt(snapPointToGrid(event->scenePos()));
        }

        // Retrieve the vlnv.
        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        // Create the component model.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(*vlnv);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        // Set the instance name for the new component instance.
        QString instanceName = createInstanceName(comp);

        if (stack != 0)
        {
            // Create the SW component item.
            SWCompItem* item = new SWCompItem(comp, instanceName, QString(), QString(),
                QMap<QString, QString>(), nodeIDFactory_.getID());
            
            item->setPos(stack->mapStackFromScene(snapPointToGrid(event->scenePos())));
            connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

            // Create the undo command and execute it.
            QSharedPointer<SystemItemAddCommand> cmd(new SystemItemAddCommand(stack, item));
            connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

            getEditProvider().addCommand(cmd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // If other than left button was pressed return back to select mode.
    if (event->button() != Qt::LeftButton)
    {
        // TODO: Disable highlights.
        setMode(MODE_SELECT);
        return;
    }

    if (getMode() == MODE_CONNECT)
    {
        // Deselect all items.
        clearSelection();

        // TODO: Creating connections.
    }
    else if (getMode() == MODE_DRAFT)
    {
        // Find the bottom-most item under the cursor.
        QGraphicsItem* item = 0;
        QList<QGraphicsItem*> itemList = items(event->scenePos());

        if (!itemList.empty())
        {
            item = itemList.back();
        }

        // TODO:
    }
    else if (getMode() == MODE_SELECT)
    {
        // Save the old selection.
        QGraphicsItem *oldSelection = 0;

        if (!selectedItems().isEmpty())
        {
            oldSelection = selectedItems().front();
        }

        // Handle the mouse press and bring the new selection to front.
        QGraphicsScene::mousePressEvent(event);
        //selectionToFront();

        // Retrieve the new selection.
        QGraphicsItem *newSelection = 0;

        if (!selectedItems().isEmpty())
        {
            newSelection = selectedItems().front();
        }

        // TODO:
        onSelected(newSelection);

//         if (oldSelection && oldSelection->type() == EndpointConnection::Type)
//             if (!selectedItems().size() || selectedItems().first() != oldSelection)
//                 oldSelection->setZValue(-1000);
    }
}

//-----------------------------------------------------------------------------
// Function: onSelected()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection)
    { 
        // Check if the selected item was a component.
        if (dynamic_cast<ComponentItem*>(newSelection) != 0)
        {
            emit componentSelected(dynamic_cast<ComponentItem*>(newSelection));
        }
        else
        {
            // Otherwise inform others that nothing is currently selected.
            emit clearItemSelection();
        }
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Check if the connect mode is active.
    if (getMode() == MODE_CONNECT)
    {
        // TODO:
    }

    QGraphicsScene::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // If there is a connection being drawn while in the connect mode,
    // finalize or discard the connection.
    if (getMode() == MODE_CONNECT /*&& tempConnection_*/)
    {
    }

    // Process the normal mouse release event.
    QGraphicsScene::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onVerticalScroll(qreal y)
{
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        SystemDesignWidget* doc = static_cast<SystemDesignWidget*>(parent());
        QGraphicsView* view = doc->getView();

        // Retrieve the center point in scene coordinates.
        //QPointF origCenterPos = view->mapToScene(view->rect().center());
        QPointF centerPos = itemsBoundingRect().center();

        // Set the zoom level and center the view.
        doc->setZoomLevel(doc->getZoomLevel() + event->delta() / 12);

        view->centerOn(centerPos);
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> SystemDesignDiagram::createDesign(VLNV const& vlnv) const
{
    QSharedPointer<Design> design(new Design(vlnv));

    QList<Design::ComponentInstance> instances;
    QList<SWInstance> swInstances;
    QList<Design::Interconnection> interconnections;
    QList<ColumnDesc> columns;

    foreach (QGraphicsItem const* item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem const* mappingItem = static_cast<HWMappingItem const*>(item);

            Design::ComponentInstance instance(mappingItem->name(), mappingItem->displayName(),
                                               mappingItem->description(),
                                               *mappingItem->componentModel()->getVlnv(),
                                               mappingItem->scenePos());
            instance.configurableElementValues = mappingItem->getConfigurableElements();
            instance.imported = mappingItem->isImported();
            //instance.mcapiNodeID = 

            instances.append(instance);
        }
        else if (item->type() == SWCompItem::Type)
        {
            SWCompItem const* swCompItem = static_cast<SWCompItem const*>(item);

            SWInstance instance;
            instance.setInstanceName(swCompItem->name());
            instance.setDisplayName(swCompItem->displayName());
            instance.setDescription(swCompItem->description());
            instance.setComponentRef(*swCompItem->componentModel()->getVlnv());
                        
            if (swCompItem->parentItem()->type() == HWMappingItem::Type)
            {
                HWMappingItem const* parent = static_cast<HWMappingItem*>(swCompItem->parentItem());
                instance.setMapping(parent->name());
                instance.setPosition(swCompItem->pos());
            }
            else
            {
                instance.setPosition(swCompItem->scenePos());
            }

            swInstances.append(instance);
        }
    }

    foreach (SystemColumn* column, layout_->getColumns())
    {
        columns.append(ColumnDesc(column->getName(), COLUMN_CONTENT_CUSTOM, 0));
    }

    design->setComponentInstances(instances);
    design->setSWInstances(swInstances);
    design->setInterconnections(interconnections);
    design->setColumns(columns);   

    return design;
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addColumn(QString const& name)
{
    QSharedPointer<QUndoCommand> cmd(new SystemColumnAddCommand(layout_.data(), name));
    getEditProvider().addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Allow double click only when the mode is select.
    if (getMode() != MODE_SELECT)
    {
        return;
    }

    // Find the item under the cursor.
    QGraphicsItem *item = itemAt(snapPointToGrid(mouseEvent->scenePos()));

    if (item == 0)
    {
        return;
    }

    // This fixes the problem when the user click above a text label or a pixmap but
    // actually wants to select the parent item (such as the actual component, not its label).
    while (item->parentItem() != 0 &&
           (item->type() == QGraphicsTextItem::Type || item->type() == QGraphicsPixmapItem::Type))
    {
        item = item->parentItem();
    }

    // TODO:
}

//-----------------------------------------------------------------------------
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
SystemColumnLayout* SystemDesignDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onComponentInstanceAdded(ComponentItem* item)
{
    DesignDiagram::onComponentInstanceAdded(item);

//     if (item->type() == MappingComponentItem::Type)
//     {
//         nodeIDFactory_.usedID(static_cast<MappingComponentItem*>(item)->getID());
//     }
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    DesignDiagram::onComponentInstanceRemoved(item);

//     if (item->type() == MappingComponentItem::Type)
//     {
//         nodeIDFactory_.freeID(static_cast<MappingComponentItem*>(item)->getID());
//     }
}

//-----------------------------------------------------------------------------
// Function: parent()
//-----------------------------------------------------------------------------
SystemDesignWidget* SystemDesignDiagram::parent() const
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: getHWComponent()
//-----------------------------------------------------------------------------
HWMappingItem* SystemDesignDiagram::getHWComponent(QString const& instanceName)
{
    // search all graphicsitems in the scene
    foreach (QGraphicsItem *item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem* comp = qgraphicsitem_cast<HWMappingItem*>(item);

            if (comp->name() == instanceName)
            {
                return comp;
            }
        }
    }

    // if no component was found
    emit errorMessage(tr("Component %1 was not found in the design").arg(instanceName));
    return 0;
}
