//-----------------------------------------------------------------------------
// File: EndpointDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Endpoint design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#include "EndpointDesignDiagram.h"

#include "SystemChangeCommands.h"
#include "SystemAddCommands.h"

#include "EndpointConnection.h"
#include "EndpointItem.h"
#include "EndpointDesignWidget.h"
#include "PlatformComponentItem.h"
#include "PlatformPlaceholderItem.h"
#include "AppPlaceholderItem.h"
#include "ApplicationItem.h"

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

#include <models/component.h>
#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/model.h>
#include <models/businterface.h>
#include <models/fileset.h>
#include <models/file.h>
#include <mainwindow/mainwindow.h>

#include <LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: EndpointDesignDiagram()
//-----------------------------------------------------------------------------
EndpointDesignDiagram::EndpointDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                                             GenericEditProvider& editProvider,
                                             QObject* parent /* = 0*/) : QGraphicsScene(parent), lh_(lh),
                                                                         mainWnd_(mainWnd),
                                                                         editProvider_(editProvider),
                                                                         system_(), designConf_(),
                                                                         nodeIDFactory_(),
                                                                         layout_(), mode_(MODE_SELECT),
                                                                         tempConnEndpoint_(0),
                                                                         tempConnection_(0),
                                                                         tempPotentialEndingEndPoints_(0),
                                                                         highlightedEndPoint_(0),
                                                                         instanceNames_()
                                                                         
{
    setSceneRect(0, 0, 100000, 100000);

    connect(this, SIGNAL(componentInstantiated(SWComponentItem*)),
            this, SLOT(onComponentInstanceAdded(SWComponentItem*)), Qt::UniqueConnection);
    connect(this, SIGNAL(componentInstanceRemoved(SWComponentItem*)),
            this, SLOT(onComponentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ~EndpointDesignDiagram()
//-----------------------------------------------------------------------------
EndpointDesignDiagram::~EndpointDesignDiagram()
{
    // Delete connections.
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == EndpointConnection::Type)
        {
            removeItem(item);
            delete item;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
bool EndpointDesignDiagram::setDesign(QSharedPointer<Component> system)
{
    Q_ASSERT(system != 0);
    system_ = system;

    VLNV designVLNV = system->getHierRef("kts_sys_ref");

    // The received type is always VLNV::DESIGN so it must be asked from the
    // library handler to make sure the type is correct.
    designVLNV.setType(lh_->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        emit errorMessage(tr("System %1 did not contain a hierarchical view").arg(
                          system->getVlnv()->getName()));
        return false;
    }

    QSharedPointer<Design> design;

    // Check if the component contains a direct reference to a design.
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
        design = libComp.staticCast<Design>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);
        designConf_ = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf_->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
            design = libComp.staticCast<Design>();
        }

        // If design configuration did not contain a reference to a design.
        if (!design)
        {
            emit errorMessage(tr("System %1 did not contain a hierarchical view").arg(
                              system->getVlnv()->getName()));
            return false;
        }
    }

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
        foreach(Design::ColumnDesc desc, design->getColumns())
        {
            layout_->addColumn(desc.name);
        }
    }

    unsigned int colIndex = 0;

    // Add component instances.
    foreach (Design::ComponentInstance const& instance, design->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = lh_->getModel(instance.componentRef);

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

        MappingComponentItem* item = new MappingComponentItem(this, lh_, component, instance.instanceName,
                                                              instance.displayName, instance.description,
                                                              instance.configurableElementValues, id);
        connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

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

        instanceNames_.append(instance.instanceName);
    }

    // Add interconnections.
    foreach (Design::Interconnection interconnection, design->getInterconnections())
    {
        // Find the referenced mapping components.
        MappingComponentItem* comp1 = getMappingComponent(interconnection.interface1.componentRef);
        
        if (comp1 == 0)
        {
            emit errorMessage(tr("Component %1 was not found within the design").arg(
                              interconnection.interface1.componentRef));
            continue;
        }

        MappingComponentItem* comp2 = getMappingComponent(interconnection.interface2.componentRef);

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component %1 was not found within the design").arg(
                interconnection.interface2.componentRef));
            continue;
        }

        // find the port of the first referenced component
        EndpointItem* endpoint1 = comp1->getEndpoint(interconnection.interface1.busRef);

        if (endpoint1 == 0)
        {
            emit errorMessage(tr("Endpoint %1 was not found within component %2").arg(
                interconnection.interface1.busRef).arg(interconnection.interface1.componentRef));
            continue;
        }

        // find the port of the second referenced component
        EndpointItem* endpoint2 = comp2->getEndpoint(interconnection.interface2.busRef);

        if (endpoint2 == 0)
        {
            emit errorMessage(tr("Endpoint %1 was not found within component %2").arg(
                interconnection.interface2.busRef).arg(interconnection.interface2.componentRef));
            continue;
        }

        // If both components and their ports are found an interconnection can be created.
        EndpointConnection* conn = new EndpointConnection(endpoint1, endpoint2, true);
        conn->setRoute(interconnection.route);
        connect(conn, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
        
        addItem(conn);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> EndpointDesignDiagram::getDesignConfiguration() const
{
    return designConf_;
}

//-----------------------------------------------------------------------------
// Function: removeInstanceName()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::removeInstanceName(QString const& name)
{
    instanceNames_.removeAll(name);
}

//-----------------------------------------------------------------------------
// Function: updateInstanceName()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::updateInstanceName(QString const& oldName, QString const& newName)
{
    instanceNames_.removeAll(oldName);
    instanceNames_.append(newName);
}

//-----------------------------------------------------------------------------
// Function: addMappingComponent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::addMappingComponent(SystemColumn* column, QPointF const& pos)
{
    Q_ASSERT(column != 0);

    // Create a component model without a valid vlnv.
    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
    comp->setVlnv(VLNV());
    
    // Create the mapping component graphics item.
    MappingComponentItem* item = new MappingComponentItem(this, lh_, comp, createInstanceName("unnamed"),
                                                          "", "", QMap<QString, QString>(),
                                                          nodeIDFactory_.getID());
    item->setPos(pos);
    connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Create an undo command and execute it.
    QSharedPointer<MappingCompAddCommand> cmd(new MappingCompAddCommand(column, item));
    connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
        this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
    connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: drawBackground
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::drawBackground(QPainter* painter, QRectF const& rect)
{
    painter->setWorldMatrixEnabled(true);
    painter->setBrush(Qt::lightGray);

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize ) {
        for (qreal y = top; y < rect.bottom(); y += GridSize )
            painter->drawPoint(x, y);
    }
}

//-----------------------------------------------------------------------------
// Function: getMainWindow()
//-----------------------------------------------------------------------------
MainWindow* EndpointDesignDiagram::getMainWindow() const
{
    return mainWnd_;
}

//-----------------------------------------------------------------------------
// Function: setMode()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::setMode(DrawMode mode)
{
    mode_ = mode;

    if (mode_ != MODE_SELECT)
    {
        emit clearItemSelection();
    }

    emit modeChanged(mode);
}

//-----------------------------------------------------------------------------
// Function: dragEnterEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragSW_ = false;

    if (event->mimeData()->hasFormat("data/vlnvptr"))
    {
        event->acceptProposedAction();

        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        if (vlnv->getType() == VLNV::COMPONENT)
        {
            // Determine the component type.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(*vlnv);
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

            dragSWType_ = comp->getComponentSWType();
            dragSW_ = true;
            comp.clear();
        }
        else if (vlnv->getType() == VLNV::BUSDEFINITION)
        {
            // TODO: Needed?
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragMoveEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
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

        if (item != 0 &&
            ((dragSWType_ == KactusAttribute::KTS_SW_ENDPOINTS && item->type() == MappingComponentItem::Type) ||
             (dragSWType_ == KactusAttribute::KTS_SW_PLATFORM && item->type() == PlatformPlaceholderItem::Type) ||
             (dragSWType_ == KactusAttribute::KTS_SW_APPLICATION && item->type() == AppPlaceholderItem::Type)))
        {
            event->setDropAction(Qt::CopyAction);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }

        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent * event)
{
    dragSW_ = false;
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Check if the dragged item was a valid one.
    if (dragSW_)
    {
        QList<QGraphicsItem*> itemList = items(event->scenePos());

        // Retrieve the vlnv.
        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        // Create the component model.
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(*vlnv);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        // Set the instance name for the new component instance.
        QString instanceName = createInstanceName(comp);

        if (dragSWType_ == KactusAttribute::KTS_SW_ENDPOINTS ||
            dragSWType_ == KactusAttribute::KTS_SW_PLATFORM)
        {
            // Find the bottom-most item under the cursor.
            MappingComponentItem* mappingCompItem = 0;
            
            if (!itemList.empty())
            {
                mappingCompItem = dynamic_cast<MappingComponentItem*>(itemList.back());
                Q_ASSERT(mappingCompItem != 0);
            }

            if (dragSWType_ == KactusAttribute::KTS_SW_ENDPOINTS)
            {
                // Create the application item.
                ProgramEntityItem* item = new ProgramEntityItem(comp, instanceName,
                                                                QString(), QString(),
                                                                QMap<QString, QString>());
                item->setPos(snapPointToGrid(event->scenePos()));
                connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

                // Create the undo command and execute it.
                QSharedPointer<ProgramEntityAddCommand> cmd(new ProgramEntityAddCommand(mappingCompItem, item));
                connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
                    this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

                editProvider_.addCommand(cmd);
            }
            else
            {
                // Create the platform component item.
                PlatformComponentItem* item = new PlatformComponentItem(comp, instanceName,
                                                                        QString(), QString(),
                                                                        QMap<QString, QString>(),
                                                                        mappingCompItem);
                connect(item, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

                // Create the undo command and execute it.
                QSharedPointer<PlatformCompAddCommand> cmd(new PlatformCompAddCommand(mappingCompItem, item));
                connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
                    this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

                editProvider_.addCommand(cmd);
            }
        }
        else if (dragSWType_ == KactusAttribute::KTS_SW_APPLICATION)
        {
            // Find the program entity item.
            ProgramEntityItem* progEntity = 0;

            for (int i = 0; i < itemList.size(); ++i)
            {
                if (itemList[i]->type() == ProgramEntityItem::Type)
                {
                    progEntity = static_cast<ProgramEntityItem*>(itemList[i]);
                    break;
                }
            }

            Q_ASSERT(progEntity != 0);

            // Create the application item.
            ApplicationItem* item = new ApplicationItem(comp, instanceName, QString(),
                                                        QString(), QMap<QString, QString>(),
                                                        progEntity);
            connect(item, SIGNAL(openSource(ProgramEntityItem*)),
                    this, SIGNAL(openSource(ProgramEntityItem*)), Qt::UniqueConnection);

            // Create the undo command and execute it.
            QSharedPointer<ApplicationAddCommand> cmd(new ApplicationAddCommand(progEntity, item));
            connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
                this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

            editProvider_.addCommand(cmd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // If other than left button was pressed return back to select mode.
    if (event->button() != Qt::LeftButton)
    {
        for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
        {
            tempPotentialEndingEndPoints_.at(i)->setHighlight(EndpointItem::HIGHLIGHT_OFF);
        }
        tempPotentialEndingEndPoints_.clear();

        // Destroy the connection that was being drawn.
        if (tempConnection_) {
            delete tempConnection_;
            tempConnection_ = 0;
            tempConnEndpoint_ = 0;
        }

        // Disable highlights from all endpoints.
        disableHighlight();

        setMode(MODE_SELECT);
        return;
    }

    if (mode_ == MODE_CONNECT)
    {
        // Deselect all items.
        clearSelection();

        // Try to snap to an MCAPI endpoint.
        tempConnEndpoint_ = DiagramUtil::snapToItem<EndpointItem>(event->scenePos(), this, GridSize);

        if (tempConnEndpoint_ == 0 || tempConnEndpoint_->isConnected())
        {
            return;
        }

        // Create a new temporary connection while the user is drawing.
        tempConnection_ = new EndpointConnection(tempConnEndpoint_->scenePos(),
                                                 tempConnEndpoint_->getDirectionVector(),
                                                 tempConnEndpoint_->scenePos(),
                                                 QVector2D(0.0f, 0.0f));

        addItem(tempConnection_);

        // Determine all potential endpoints to which the starting endpoint could be connected
        // and highlight them.
        foreach(QGraphicsItem* item, items())
        {
            EndpointItem* endpoint = dynamic_cast<EndpointItem*>(item);

            if (endpoint != 0 && endpoint != tempConnEndpoint_ &&
                endpoint->canConnect(tempConnEndpoint_) && tempConnEndpoint_->canConnect(endpoint))
            {
                tempPotentialEndingEndPoints_.append(endpoint);
                endpoint->setHighlight(EndpointItem::HIGHLIGHT_ALLOWED);
            }
        }
    }
    else if (mode_ == MODE_DRAFT)
    {
        // Find the bottom-most item under the cursor.
        QGraphicsItem* item = 0;
        QList<QGraphicsItem*> itemList = items(event->scenePos());

        if (!itemList.empty())
        {
            item = itemList.back();
        }

        // If there was no item, then a new mapping component should be added
        // to the column under the cursor.
        if (item == 0)
        {
            SystemColumn* column = layout_->findColumnAt(event->scenePos());

            if (column != 0)
            {
                // Determine a unique name for the mapping component.
                addMappingComponent(column, snapPointToGrid(event->scenePos()));
                emit contentChanged();
            }
        }
        // If there was an SW mapping component, then an application item is added.
        else if (item->type() == MappingComponentItem::Type)
        {
            MappingComponentItem* mappingComp = static_cast<MappingComponentItem*>(item);
            
            // Create a new MCAPI endpoint component.
            QSharedPointer<Component> component(new Component(VLNV()));
            component->setComponentImplementation(KactusAttribute::KTS_SW);
            component->setComponentSWType(KactusAttribute::KTS_SW_ENDPOINTS);

            // Add the fixed bus interface to the component.
            QSharedPointer<BusInterface> busIf(new BusInterface());
            busIf->setName("app_link");
            busIf->setInterfaceMode(General::SLAVE);
            busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "app_link", "1.0"));

            ProgramEntityItem* progEntity =
                new ProgramEntityItem(component, createInstanceName("unnamed_endpoints"), QString(),
                                      QString(), QMap<QString, QString>());
            progEntity->setPos(mappingComp->mapFromScene(event->scenePos()));

            QSharedPointer<ProgramEntityAddCommand> cmd(new ProgramEntityAddCommand(mappingComp, progEntity));
            connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
                this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

            editProvider_.addCommand(cmd);
        }
    }
    else if (mode_ == MODE_SELECT)
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
        //onSelected(newSelection);

        if (oldSelection && oldSelection->type() == EndpointConnection::Type)
            if (!selectedItems().size() || selectedItems().first() != oldSelection)
                oldSelection->setZValue(-1000);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Check if the connect mode is active.
    if (mode_ == MODE_CONNECT)
    {
        // Find out if there is an endpoint currently close to the cursor.
        EndpointItem* endpoint = DiagramUtil::snapToItem<EndpointItem>(event->scenePos(), this, GridSize);

        if (tempConnection_)
        {
            // Check if it was a valid endpoint.
            if (endpoint != 0 && tempPotentialEndingEndPoints_.contains(endpoint))
            {
                // Highlight the endpoint.
                disableHighlight();                
                highlightedEndPoint_ = endpoint;
                highlightedEndPoint_->setHighlight(EndpointItem::HIGHLIGHT_HOVER);

            }
            // Disable the highlight if there was no valid endpoint close enough.
            else
            {
                disableHighlight();
            }

            // Update the connection.
            Q_ASSERT(tempConnection_->getRoute().size() != 0);

            EndpointConnection* newTempConnection_ = 0;

            if (highlightedEndPoint_ != 0)
            {
                newTempConnection_ = new EndpointConnection(tempConnEndpoint_, highlightedEndPoint_, false);
            }
            else
            {
                if (event->scenePos().x() > tempConnEndpoint_->scenePos().x())
                {
                    tempConnEndpoint_->setDirection(EndpointItem::DIR_RIGHT);
                }
                else
                {
                    tempConnEndpoint_->setDirection(EndpointItem::DIR_LEFT);
                }

                newTempConnection_ = new EndpointConnection(tempConnEndpoint_->scenePos(),
                                                            tempConnEndpoint_->getDirectionVector(),
                                                            snapPointToGrid(event->scenePos()),
                                                            QVector2D(0.0f, 0.0f));
            }

            removeItem(tempConnection_);
            delete tempConnection_;
            tempConnection_ = newTempConnection_;

            addItem(tempConnection_);
        }
        else
        {
            if (highlightedEndPoint_ != 0)
            {
                highlightedEndPoint_->setHighlight(EndpointItem::HIGHLIGHT_OFF);
                highlightedEndPoint_ = 0;
            }

            if (endpoint != 0 && !endpoint->isConnected())
            {
                highlightedEndPoint_ = endpoint;
                highlightedEndPoint_->setHighlight(EndpointItem::HIGHLIGHT_HOVER);
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: void()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::disableHighlight()
{
    if (highlightedEndPoint_ != 0)
    {
        if (tempConnEndpoint_ != 0 && highlightedEndPoint_ != tempConnEndpoint_)
        {
            highlightedEndPoint_->setHighlight(EndpointItem::HIGHLIGHT_ALLOWED);
        }
        else
        {
            highlightedEndPoint_->setHighlight(EndpointItem::HIGHLIGHT_OFF);
        }

        highlightedEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // If there is a connection being drawn while in the connect mode,
    // finalize or discard the connection.
    if (mode_ == MODE_CONNECT && tempConnection_)
    {
        // Disable highlights from all potential endpoints.
        for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
        {
            tempPotentialEndingEndPoints_.at(i)->setHighlight(EndpointItem::HIGHLIGHT_OFF);
        }
        tempPotentialEndingEndPoints_.clear();

        EndpointItem* endpoint = DiagramUtil::snapToItem<EndpointItem>(event->scenePos(), this, GridSize);

        // Check if there is no endpoint close enough to the cursor or the
        // endpoints cannot be connected together.
        if (endpoint == 0 || endpoint == tempConnEndpoint_ ||
            !endpoint->canConnect(tempConnEndpoint_) ||
            !tempConnEndpoint_->canConnect(endpoint))
        {
            removeItem(tempConnection_);
            delete tempConnection_;
            tempConnection_ = 0;
            tempConnEndpoint_ = 0;
        }
        else 
        {
            if (tempConnection_->connectEnds())
            {
                connect(tempConnection_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

                QSharedPointer<QUndoCommand> cmd(new EndpointConnectionAddCommand(this, tempConnection_));
                editProvider_.addCommand(cmd, false);

                tempConnection_ = 0;
                tempConnEndpoint_ = 0;
                emit contentChanged();
            }
            else
            {
                delete tempConnection_;
                tempConnection_ = 0;
                tempConnEndpoint_ = 0;
            }
        }
    }

    // Process the normal mouse release event.
    QGraphicsScene::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::onVerticalScroll(qreal y)
{
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        EndpointDesignWidget* doc = static_cast<EndpointDesignWidget*>(parent());
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
// Function: saveHierarchy()
//-----------------------------------------------------------------------------
bool EndpointDesignDiagram::saveHierarchy() const
{
    // Check for unpackaged components.
    QList<QString> unpackagedNames;

    foreach (QGraphicsItem* item, items())
    {
        SWComponentItem* swCompItem = dynamic_cast<SWComponentItem*>(item);

        if (swCompItem != 0 && !swCompItem->componentModel()->getVlnv()->isValid()) 
        {
            unpackagedNames.append(swCompItem->name());
        }
    }

    // If there were unpacked components, show a message box and do not save.
    if (!unpackagedNames.empty())
    {
        QString detailMessage = tr("The following components must be packaged before the system can be saved:");

        foreach (QString const& name, unpackagedNames)
        {
            detailMessage += "\n * " + name;
        }

        TabDocument* doc = static_cast<TabDocument*>(parent());

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("System ") + doc->getDocumentName() + 
                           tr(" cannot be saved because some component(s) are unpackaged."),
                           QMessageBox::Ok, (QWidget*)parent());
        msgBox.setDetailedText(detailMessage);
        msgBox.exec();
        return false;
    }

    // Go through all graphics items and save the mapping components.
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == MappingComponentItem::Type)
        {
            MappingComponentItem* mappingCompItem = static_cast<MappingComponentItem*>(item);

            if (!mappingCompItem->save(lh_))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> EndpointDesignDiagram::createDesign(VLNV const& vlnv)
{
    QSharedPointer<Design> design(new Design(vlnv));

    QList<Design::ComponentInstance> instances;
    QList<Design::Interconnection> interconnections;
    QList<Design::ColumnDesc> columns;

    foreach (QGraphicsItem *item, items())
    {
        if (item->type() == MappingComponentItem::Type)
        {
            MappingComponentItem* mappingCompItem = qgraphicsitem_cast<MappingComponentItem*>(item);

            Design::ComponentInstance instance(mappingCompItem->name(), mappingCompItem->displayName(),
                                               mappingCompItem->description(),
                                               *mappingCompItem->componentModel()->getVlnv(),
                                               mappingCompItem->scenePos());
            instance.configurableElementValues = mappingCompItem->getConfigurableElements();

            instances.append(instance);
        }
        else if (item->type() == EndpointConnection::Type)
        {
            EndpointConnection* conn = static_cast<EndpointConnection*>(item);

            Design::Interface iface1(conn->getEndpoint1()->getParentMappingComp()->name(),
                                     conn->getEndpoint1()->getFullName());
            Design::Interface iface2(conn->getEndpoint2()->getParentMappingComp()->name(),
                                     conn->getEndpoint2()->getFullName());
            interconnections.append(Design::Interconnection(conn->getName(), iface1, iface2, conn->getRoute()));
        }
    }

    foreach (SystemColumn* column, layout_->getColumns())
    {
        columns.append(Design::ColumnDesc(column->getName(), COLUMN_CONTENT_CUSTOM, 0));
    }

    design->setComponentInstances(instances);
    design->setInterconnections(interconnections);
    design->setColumns(columns);   

    return design;
}

//-----------------------------------------------------------------------------
// Function: createInstanceName()
//-----------------------------------------------------------------------------
QString EndpointDesignDiagram::createInstanceName(QString const& baseName)
{
    // Determine a unique name by using a running number.
    int runningNumber = 0;
    QString name = baseName + "_" + QString::number(runningNumber);

    while (instanceNames_.contains(name))
    {
        ++runningNumber;
        name = baseName + "_" + QString::number(runningNumber);
    }

    instanceNames_.append(name);
    return name;
}

//-----------------------------------------------------------------------------
// Function: createInstanceName()
//-----------------------------------------------------------------------------
QString EndpointDesignDiagram::createInstanceName(QSharedPointer<Component> component)
{
    QString instanceName = component->getVlnv()->getName();
    instanceName.remove(QString(".comp"));
    return createInstanceName(instanceName);
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::addColumn(QString const& name)
{
    QSharedPointer<QUndoCommand> cmd(new SystemColumnAddCommand(layout_.data(), name));
    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: ()
//-----------------------------------------------------------------------------
MappingComponentItem* EndpointDesignDiagram::getMappingComponent(QString const& instanceName)
{
    // Search for a SW mapping component with the given instance name.
    foreach (QGraphicsItem *item, items())
    {
        if (item->type() == MappingComponentItem::Type)
        {
            MappingComponentItem* mappingCompItem = static_cast<MappingComponentItem*>(item);

            if (mappingCompItem->name() == instanceName)
            {
                return mappingCompItem;
            }
        }
    }

    // The component was not found.
    emit errorMessage(tr("Component %1 was not found within design").arg(instanceName));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Allow double click only when the mode is select.
    if (mode_ != MODE_SELECT)
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

    if (item->type() == AppPlaceholderItem::Type)
    {
        ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(item->parentItem());
        createApplication(progEntity);

    }
    else if (item->type() == PlatformPlaceholderItem::Type)
    {
        MappingComponentItem* mappingCompItem = static_cast<MappingComponentItem*>(item->parentItem());
        createPlatformComponent(mappingCompItem);
    }
    else if (item->type() == ApplicationItem::Type)
    {
        ApplicationItem* appItem = static_cast<ApplicationItem*>(item);

        if (!appItem->componentModel()->getVlnv()->isValid())
        {
            packetizeSWComponent(appItem, tr("Application"));
        }
    }
    else if (item->type() == PlatformComponentItem::Type)
    {
        PlatformComponentItem* platformCompItem = static_cast<PlatformComponentItem*>(item);

        if (!platformCompItem->componentModel()->getVlnv()->isValid())
        {
            packetizeSWComponent(platformCompItem, tr("SW Platform"));
        }
    }
    else if (item->type() == ProgramEntityItem::Type)
    {
        ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(item);

        if (!progEntity->componentModel()->getVlnv()->isValid())
        {
            packetizeSWComponent(progEntity, tr("Endpoints"));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: createApplication()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::createApplication(ProgramEntityItem* progEntity)
{
    // Create an application component without a valid vlnv.
    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
    comp->setVlnv(VLNV());
    comp->setComponentImplementation(KactusAttribute::KTS_SW);
    comp->setComponentSWType(KactusAttribute::KTS_SW_APPLICATION);

    // Add the fixed bus interface to the component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName("app_link");
    busIf->setInterfaceMode(General::MASTER);
    busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "app_link", "1.0"));

    comp->addBusInterface(busIf);

    // Ask the user if he/she wants to generate template code based on the endpoints.
    QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                       tr("Do you want to auto-generate template code based on the endpoints?"),
                       QMessageBox::Yes | QMessageBox::No, (QWidget*)parent());

    bool sourceCreated = false;

    if (msgBox.exec() == QMessageBox::Yes)
    {
        // Ask the user where to save the source file.
        QString filename = QFileDialog::getSaveFileName((QWidget*)parent(), tr("Save Source File"),
            QString(), tr("C Source Files (*.c)"));

        if (!filename.isEmpty())
        {
            progEntity->createSource(filename);

            FileSet* fileSet = new FileSet();
            fileSet->setName("cSources");
            File* file = new File(filename, fileSet);
            file->addFileType("cSource");
            fileSet->addFile(file);

            comp->addFileSet(fileSet);
            sourceCreated = true;
        }
    }

    ApplicationItem* app = new ApplicationItem(comp, createInstanceName("unnamed_app"),
                                               QString(), QString(), QMap<QString, QString>(), progEntity);
    connect(app, SIGNAL(openSource(ProgramEntityItem*)),
        this, SIGNAL(openSource(ProgramEntityItem*)), Qt::UniqueConnection);

    // Create the undo command and execute it.
    QSharedPointer<ApplicationAddCommand> cmd(new ApplicationAddCommand(progEntity, app));
    connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
        this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
    connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

    editProvider_.addCommand(cmd);

    if (sourceCreated)
    {
        emit openSource(progEntity);
    }
}

//-----------------------------------------------------------------------------
// Function: packetizeApplication()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::packetizeSWComponent(SWComponentItem* item, QString const& itemTypeName)
{
    // Request the user to set the vlnv.
    NewObjectDialog dialog(lh_, VLNV::COMPONENT, false, (QWidget*)parent());
    dialog.setWindowTitle(tr("Add ") + itemTypeName + tr(" to Library"));

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    VLNV vlnv = dialog.getVLNV();
    item->componentModel()->setVlnv(vlnv);

    // Write the model to file.
    lh_->writeModelToFile(dialog.getPath(), item->componentModel());

    // Update the diagram component.
    item->updateComponent();

    // Create an undo command.
    QSharedPointer<QUndoCommand> cmd(new SWComponentPacketizeCommand(item, vlnv));
    editProvider_.addCommand(cmd, false);

    // Ask the user if he wants to complete the component.
    QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
        "Do you want to continue packaging the " + itemTypeName.toLower() + " completely?",
        QMessageBox::NoButton, (QWidget*)parent());
    msgBox.setInformativeText("Pressing Continue opens up the component editor.");
    QPushButton* btnContinue = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Skip"), QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == btnContinue)
    {
        // Open up the component editor.
        emit openComponent(*item->componentModel()->getVlnv());
    }
}

//-----------------------------------------------------------------------------
// Function: createPlatformComponent()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::createPlatformComponent(MappingComponentItem* mappingCompItem)
{
    // Create a platform component without a valid vlnv.
    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
    comp->setVlnv(VLNV());
    comp->setComponentImplementation(KactusAttribute::KTS_SW);
    comp->setComponentSWType(KactusAttribute::KTS_SW_PLATFORM);

    PlatformComponentItem* platformComp = new PlatformComponentItem(comp,
                                                                    createInstanceName("unnamed_platform"),
                                                                    QString(), QString(),
                                                                    QMap<QString, QString>(),
                                                                    mappingCompItem);
    connect(platformComp, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Create the undo command and execute it.
    QSharedPointer<PlatformCompAddCommand> cmd(new PlatformCompAddCommand(mappingCompItem, platformComp));
    connect(cmd.data(), SIGNAL(componentInstantiated(SWComponentItem*)),
        this, SIGNAL(componentInstantiated(SWComponentItem*)), Qt::UniqueConnection);
    connect(cmd.data(), SIGNAL(componentInstanceRemoved(SWComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(SWComponentItem*)), Qt::UniqueConnection);

    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
SystemColumnLayout* EndpointDesignDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::onComponentInstanceAdded(SWComponentItem* item)
{
    instanceNames_.append(item->name());
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void EndpointDesignDiagram::onComponentInstanceRemoved(SWComponentItem* item)
{
    instanceNames_.removeAll(item->name());
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& EndpointDesignDiagram::getEditProvider()
{
    return editProvider_;
}
