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
#include "SWConnection.h"
#include "SWConnectionEndpoint.h"

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
#include <models/ApiInterface.h>
#include <models/ComInterface.h>

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
      dragSW_(false),
      tempConnection_(0),
      tempConnEndpoint_(0),
      tempPotentialEndingEndpoints_(),
      highlightedEndpoint_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::~SystemDesignDiagram()
{
    destroyConnections();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::clearScene()
{
    destroyConnections();
    DesignDiagram::clearScene();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::setMode()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::setMode(DrawMode mode)
{
    if (getMode() != mode)
    {
        if (getMode() == MODE_CONNECT)
        {
            discardConnection();
            disableHighlights();
        }
    }

    DesignDiagram::setMode(mode);
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

        // Setup custom port positions.
        {
            QMapIterator<QString, QPointF> itrPortPos(instance.apiInterfacePositions);

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API);

                if (port != 0)
                {
                    port->setPos(itrPortPos.value());
                    item->onMovePort(port);
                }
            }
        }

        {
            QMapIterator<QString, QPointF> itrPortPos(instance.comInterfacePositions);

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM);

                if (port != 0)
                {
                    port->setPos(itrPortPos.value());
                    item->onMovePort(port);
                }
            }
        }

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

        // Setup custom port positions.
        {
            QMapIterator<QString, QPointF> itrPortPos(instance.getApiInterfacePositions());

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API);

                if (port != 0)
                {
                    port->setPos(itrPortPos.value());
                    item->onMovePort(port);
                }
            }
        }

        {
            QMapIterator<QString, QPointF> itrPortPos(instance.getComInterfacePositions());

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM);

                if (port != 0)
                {
                    port->setPos(itrPortPos.value());
                    item->onMovePort(port);
                }
            }
        }

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

    loadApiDependencies(design);


    loadComConnections(design);


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
void SystemDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // If other than left button was pressed return back to select mode.
    if (event->button() != Qt::LeftButton)
    {
        setMode(MODE_SELECT);
        return;
    }

    if (getMode() == MODE_CONNECT)
    {
        bool creating = tempConnection_ != 0;

        // Check if we need to change the temporary connection into a persistent one.
        if (creating)
        {
            createConnection(event);
            disableHighlights();
        }
        // Otherwise choose a new start endpoint.
        else
        {
            // Deselect all items.
            clearSelection();

            // Try to snap to a connection endpoint.
            SWConnectionEndpoint* endpoint =
                DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

            if (endpoint == 0)
            {
                return;
            }

            tempConnEndpoint_ = endpoint;
            tempConnEndpoint_->onBeginConnect();
            
            // Create the connection.
            tempConnection_ = new SWConnection(tempConnEndpoint_->scenePos(),
                                               tempConnEndpoint_->getDirection(),
                                               event->scenePos(),
                                               QVector2D(0.0f, 0.0f), QString(), QString(), this);

            if (tempConnEndpoint_->isApi())
            {
                tempConnection_->setLineWidth(2);
            }

            addItem(tempConnection_);

            // Determine all potential endpoints to which the starting endpoint could be connected
            // and highlight them.
            foreach (QGraphicsItem* item, items())
            {
                SWConnectionEndpoint* endpoint = dynamic_cast<SWConnectionEndpoint*>(item);

                if (endpoint != 0 && endpoint->isVisible() &&
                    endpoint != tempConnEndpoint_ &&
                    endpoint->canConnect(tempConnEndpoint_) && tempConnEndpoint_->canConnect(endpoint))
                {
                    tempPotentialEndingEndpoints_.append(endpoint);
                    endpoint->setHighlight(SWConnectionEndpoint::HIGHLIGHT_ALLOWED);
                }
            }
        }
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
        // Find out if there is an endpoint currently under the cursor.
        SWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

        if (tempConnection_)
        {
            // Check if there was a valid endpoint close enough.
            if (endpoint != 0 && tempPotentialEndingEndpoints_.contains(endpoint))
            {
                // Highlight the endpoint.
                disableCurrentHighlight();                
                highlightedEndpoint_ = endpoint;
                highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_HOVER);
                highlightedEndpoint_->onBeginConnect();

            }
            // Disable the highlight if there was no endpoint close enough.
            else
            {
                disableCurrentHighlight();
            }

            // Update the connection.
            Q_ASSERT(tempConnection_->route().size() != 0);

            SWConnection* newTempConnection_ = 0;

            if (highlightedEndpoint_ != 0)
            {
                newTempConnection_ = new SWConnection(tempConnEndpoint_, highlightedEndpoint_, false,
                                                      QString(), QString(), QString(), this);
            }
            else
            {
                newTempConnection_ = new SWConnection(tempConnEndpoint_->scenePos(),
                                                      tempConnEndpoint_->getDirection(),
                                                      snapPointToGrid(event->scenePos()),
                                                      QVector2D(0.0f, 0.0f), QString(), QString(), this);
            }

            if (tempConnEndpoint_->isApi())
            {
                newTempConnection_->setLineWidth(2);
            }

            discardConnection();
            
            addItem(newTempConnection_);
            tempConnection_ = newTempConnection_;
            return;
        }
        else
        {
            if (highlightedEndpoint_ != 0)
            {
                highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
                highlightedEndpoint_ = 0;
            }

            if (endpoint != 0 && endpoint->isVisible())
            {
                highlightedEndpoint_ = endpoint;
                highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_HOVER);
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
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
    QList<ApiDependency> apiDependencies;
    QList<ComConnection> comConnections;
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

            // Save API and COM interface positions.
            QMapIterator< QString, QSharedPointer<ApiInterface> >
                itrApiIf(mappingItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                itrApiIf.next();
                instance.apiInterfacePositions[itrApiIf.key()] =
                    mappingItem->getSWPort(itrApiIf.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos();
            }

            QMapIterator< QString, QSharedPointer<ComInterface> >
                itrComIf(mappingItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                itrComIf.next();
                instance.apiInterfacePositions[itrComIf.key()] =
                    mappingItem->getSWPort(itrComIf.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos();
            }

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

            // Save API and COM interface positions.
            QMapIterator< QString, QSharedPointer<ApiInterface> >
                itrApiIf(swCompItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                itrApiIf.next();
                instance.updateApiInterfacePosition(itrApiIf.key(),
                    swCompItem->getSWPort(itrApiIf.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos());
            }

            QMapIterator< QString, QSharedPointer<ComInterface> >
                itrComIf(swCompItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                itrComIf.next();
                instance.updateComInterfacePosition(itrComIf.key(),
                    swCompItem->getSWPort(itrComIf.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
            }

            swInstances.append(instance);
        }
        else if (item->type() == SWConnection::Type)
        {
            SWConnection const* conn = static_cast<SWConnection const*>(item);

            if (conn->getConnectionType() == SWConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                SWConnectionEndpoint* endpoint1 = conn->endpoint1();
                SWConnectionEndpoint* endpoint2 = conn->endpoint2();

                if (endpoint1->getApiInterface()->getDependencyDirection() == DEPENDENCY_REQUESTER)
                {
                    std::swap(endpoint1, endpoint2);
                }

                ApiInterfaceRef providerRef(conn->endpoint1()->encompassingComp()->name(),
                                            conn->endpoint1()->name());
                ApiInterfaceRef requesterRef(conn->endpoint2()->encompassingComp()->name(),
                                             conn->endpoint2()->name());

                ApiDependency dependency(conn->name(), QString(), conn->description(),
                                         providerRef, requesterRef, conn->route());
                apiDependencies.append(dependency);
            }
            else if (conn->getConnectionType() == SWConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                ComInterfaceRef ref1(conn->endpoint1()->encompassingComp()->name(),
                                     conn->endpoint1()->name());
                ComInterfaceRef ref2(conn->endpoint2()->encompassingComp()->name(),
                                     conn->endpoint2()->name());

                ComConnection comConnection(conn->name(), QString(), conn->description(),
                                            ref1, ref2, conn->route());
                comConnections.append(comConnection);
            }
        }
    }

    foreach (SystemColumn* column, layout_->getColumns())
    {
        columns.append(ColumnDesc(column->getName(), COLUMN_CONTENT_CUSTOM, 0));
    }

    design->setComponentInstances(instances);
    design->setSWInstances(swInstances);
    design->setApiDependencies(apiDependencies);
    design->setComConnections(comConnections);
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
void SystemDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    // Allow double click only when the mode is select.
    if (getMode() != MODE_SELECT)
    {
        return;
    }

    // Find the item under the cursor.
    QGraphicsItem *item = itemAt(snapPointToGrid(event->scenePos()));

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
SWComponentItem* SystemDesignDiagram::getComponent(QString const& instanceName)
{
    foreach (QGraphicsItem *item, items())
    {
        SWComponentItem* comp = dynamic_cast<SWComponentItem*>(item);

        if (comp != 0 && comp->name() == instanceName)
        {
            return comp;
        }
    }

    emit errorMessage(tr("Component %1 was not found in the design").arg(instanceName));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: getHWComponent()
//-----------------------------------------------------------------------------
HWMappingItem* SystemDesignDiagram::getHWComponent(QString const& instanceName)
{
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

    emit errorMessage(tr("Component %1 was not found in the design").arg(instanceName));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::createConnection(QGraphicsSceneMouseEvent* event)
{
    // Check if there is no endpoint close enough to the cursor or the endpoints cannot be connected together.
    SWConnectionEndpoint* endpoint =
        DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

    if (endpoint == 0 || endpoint == tempConnEndpoint_ || !endpoint->isVisible() ||
        !endpoint->canConnect(tempConnEndpoint_) || !tempConnEndpoint_->canConnect(endpoint))
    {
        discardConnection();
    }
    else 
    {
        // Otherwise make the temporary connection a permanent one by connecting the ends.
        connect(tempConnection_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (tempConnection_->connectEnds())
        {
            QSharedPointer<QUndoCommand> cmd(new SWConnectionAddCommand(this, tempConnection_));
            getEditProvider().addCommand(cmd, false);

            tempConnection_ = 0;
            tempConnEndpoint_ = 0;
            emit contentChanged();
        }
        else
        {
            // Discard the connection if connecting ends failed.
            discardConnection();
            tempConnEndpoint_ = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::destroyConnections()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::destroyConnections()
{
    QList<QGraphicsItem*> conns;

    // Search all SW connections.
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == SWConnection::Type)
        {
            conns.append(item);
        }
    }

    // And destroy them.
    foreach (QGraphicsItem* item, conns)
    {
        removeItem(item);
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::discardConnection()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::discardConnection()
{
    if (tempConnection_ != 0)
    {
        removeItem(tempConnection_);
        delete tempConnection_;

        tempConnection_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::disableHighlights()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::disableHighlights()
{
    // Disable highlights from all potential endpoints.
    for (int i = 0 ; i < tempPotentialEndingEndpoints_.size(); ++i)
    {
        tempPotentialEndingEndpoints_.at(i)->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
        tempPotentialEndingEndpoints_.at(i)->onEndConnect();
    }

    tempPotentialEndingEndpoints_.clear();

    // If the starting point is set, disable highlight from it too.
    if (tempConnEndpoint_ != 0)
    {
        tempConnEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
        tempConnEndpoint_->onEndConnect();
        tempConnEndpoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::disableCurrentHighlight()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::disableCurrentHighlight()
{
    if (highlightedEndpoint_ != 0)
    {
        if (tempConnEndpoint_ != 0 && highlightedEndpoint_ != tempConnEndpoint_)
        {
            highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_ALLOWED);
            highlightedEndpoint_->onEndConnect();
        }
        else
        {
            highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
        }

        highlightedEndpoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::loadApiDependencies()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadApiDependencies(QSharedPointer<Design> design)
{
    foreach (ApiDependency const& dependency, design->getApiDependencies())
    {
        // Find the referenced components.
        SWComponentItem* comp1 = getComponent(dependency.getInterface1().componentRef);

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                dependency.getInterface1().componentRef));
            continue;
        }

        SWComponentItem* comp2 = getComponent(dependency.getInterface2().componentRef);

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                dependency.getInterface2().componentRef));
            continue;
        }

        // Find the connected ports in the components.
        SWConnectionEndpoint* port1 = comp1->getSWPort(dependency.getInterface1().apiRef,
                                                       SWConnectionEndpoint::ENDPOINT_TYPE_API);

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface1().apiRef).arg(dependency.getInterface1().componentRef));
            continue;
        }

        SWConnectionEndpoint* port2 = comp2->getSWPort(dependency.getInterface2().apiRef,
                                                       SWConnectionEndpoint::ENDPOINT_TYPE_API);

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface2().apiRef).arg(dependency.getInterface2().componentRef));
            continue;
        }


        SWConnection* connection = new SWConnection(port1, port2, true,
            dependency.getName(),
            dependency.getDisplayName(),
            dependency.getDescription(), this);
        connection->setRoute(dependency.getRoute());

        connect(connection, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::loadComConnections()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadComConnections(QSharedPointer<Design> design)
{
    foreach (ComConnection const& conn, design->getComConnections())
    {
        // Find the referenced components.
        SWComponentItem* comp1 = getComponent(conn.getInterface1().componentRef);

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                conn.getInterface1().componentRef));
            continue;
        }

        SWComponentItem* comp2 = getComponent(conn.getInterface2().componentRef);

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                conn.getInterface2().componentRef));
            continue;
        }

        // Find the connected ports in the components.
        SWConnectionEndpoint* port1 = comp1->getSWPort(conn.getInterface1().comRef,
                                                       SWConnectionEndpoint::ENDPOINT_TYPE_API);

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                conn.getInterface1().comRef).arg(conn.getInterface1().componentRef));
            continue;
        }

        SWConnectionEndpoint* port2 = comp2->getSWPort(conn.getInterface2().comRef,
                                                       SWConnectionEndpoint::ENDPOINT_TYPE_API);

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                conn.getInterface2().comRef).arg(conn.getInterface2().componentRef));
            continue;
        }


        SWConnection* connection = new SWConnection(port1, port2, true,
            conn.getName(),
            conn.getDisplayName(),
            conn.getDescription(), this);
        connection->setRoute(conn.getRoute());

        connect(connection, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }
}
