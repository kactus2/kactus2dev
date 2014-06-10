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

#include "SystemChangeCommands.h"
#include "SystemMoveCommands.h"
#include "SystemAddCommands.h"
#include "SystemDeleteCommands.h"
#include "SWOffPageConnectorItem.h"
#include "SystemColumn.h"
#include "HWMappingItem.h"
#include "SWComponentItem.h"
#include "SystemDesignWidget.h"
#include "SWConnectionEndpoint.h"
#include "SWPortItem.h"
#include "SWInterfaceItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <designEditors/SystemDesign/SystemDetailsEditor/SwitchHWDialog.h>

#include <library/LibraryManager/libraryinterface.h>

#include <designEditors/HWDesign/columnview/ColumnEditDialog.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <IPXACTmodels/SWInstance.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/SWView.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/ApiInterface.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/SystemView.h>

#include <mainwindow/mainwindow.h>

#include <QUuid>

Q_DECLARE_METATYPE(SystemDesignDiagram::PortCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ComponentCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ColumnCollectionCopyData)

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::SystemDesignDiagram(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd,
                                         GenericEditProvider& editProvider,
                                         SystemDesignWidget* parent)
    : DesignDiagram(lh, mainWnd, editProvider, parent),
      onlySW_(onlySW),
      parent_(parent),
      layout_(),
      dragType_(DRAG_TYPE_NONE),
      tempConnection_(0),
      tempConnEndpoint_(0),
      tempPotentialEndingEndpoints_(),
      highlightedEndpoint_(0),
      offPageMode_(false),
      replaceMode_(false),
      sourceComp_(0),
      oldSelectedItems_(),
      selectAllAction_(tr("Select All"), this),
      copyAction_(tr("Copy"), this),
      pasteAction_(tr("Paste"), this),
      addAction_(tr("Add to Library"), this),
      openComponentAction_(tr("Open Component"), this),
      openDesignAction_(tr("Open SW Design"), this),
      showContextMenu_(true)
{
    setupActions();

    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(&editProvider, SIGNAL(modified()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::~SystemDesignDiagram()
{
    clearSelection();
    destroyConnections();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::clearScene()
{
    clearSelection();
    destroyConnections();
    layout_.clear();
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
            endConnect();
        }

        if (mode == MODE_SELECT)
        {
            hideOffPageConnections();
        }
    }

    DesignDiagram::setMode(mode);
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    // Create the column layout.
    layout_ = QSharedPointer<GraphicsColumnLayout>(new GraphicsColumnLayout(this));

    if (design->getColumns().isEmpty())
    {
        QList<ColumnDesc> columns;

        if (onlySW_)
        {
            columns.append(ColumnDesc("Low-level", COLUMN_CONTENT_COMPONENTS, 0, SW_COLUMN_WIDTH));
            columns.append(ColumnDesc("Middle-level", COLUMN_CONTENT_COMPONENTS, 0, SW_COLUMN_WIDTH));
            columns.append(ColumnDesc("High-level", COLUMN_CONTENT_COMPONENTS, 0, SW_COLUMN_WIDTH));
            columns.append(ColumnDesc("Out", COLUMN_CONTENT_IO, 0, IO_COLUMN_WIDTH));
        }
        else
        {
            columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SYSTEM_COLUMN_WIDTH));
            columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SYSTEM_COLUMN_WIDTH));
        }

        design->setColumns(columns);
    }

    foreach(ColumnDesc const& desc, design->getColumns())
    {
        SystemColumn* column = new SystemColumn(desc, layout_.data());
        layout_->addColumn(column, true);
    }

    unsigned int colIndex = 0;

    // Create (HW) component instances.
    foreach (ComponentInstance const& instance, design->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            emit errorMessage(tr("The component '%1' instantiated in the design '%2' "
                                 "was not found in the library").arg(
                instance.getComponentRef().getName()).arg(design->getVlnv()->getName()));

            // Create an unpackaged component so that we can still visualize the component instance.
            component = QSharedPointer<Component>(new Component(instance.getComponentRef()));
            component->setComponentImplementation(KactusAttribute::KTS_HW);
        }

        HWMappingItem* item = new HWMappingItem(getLibraryInterface(), component, instance.getInstanceName(),
                                                instance.getDisplayName(), instance.getDescription(),
												instance.getUuid(), instance.getConfigurableElementValues());
        item->setImported(instance.isImported());
        item->setImportRef(instance.getImportRef());
        item->setPropertyValues(instance.getPropertyValues());

        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

        // Setup custom port positions.
        {
            QMapIterator<QString, QPointF> itrPortPos(instance.getApiInterfacePositions());

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API);

                // If the port was not found, create it if the component is not packetized.
                if (port == 0)
                {
                    if (instance.getComponentRef().isValid())
                    {
                        continue;
                    }

                    port = new SWPortItem(itrPortPos.key(), item);
                    item->addPort(port);
                }

                port->setPos(itrPortPos.value());
                item->onMovePort(port);
            }
        }

        {
            QMapIterator<QString, QPointF> itrPortPos(instance.getComInterfacePositions());

            while (itrPortPos.hasNext())
            {
                itrPortPos.next();
                SWPortItem* port = item->getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM);

                // If the port was not found, create it.
                if (port == 0)
                {
                    if (instance.getComponentRef().isValid())
                    {
                        continue;
                    }

                    port = new SWPortItem(itrPortPos.key(), item);
                    item->addPort(port);
                }

                port->setPos(itrPortPos.value());
                item->onMovePort(port);
            }
        }

        // Check if the position is not found.
        if (instance.getPosition().isNull())
        {
            layout_->getColumns().at(colIndex)->addItem(item);
            colIndex = (colIndex + 1) % layout_->getColumns().size();
        }
        else
        {
            item->setPos(instance.getPosition());

            GraphicsColumn* column = layout_->findColumnAt(instance.getPosition());

            if (column != 0)
            {
                column->addItem(item, true);
            }
            else
            {
                layout_->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % layout_->getColumns().size();
            }
        }

        onComponentInstanceAdded(item);
    }
    
    // Create SW instances.
    foreach (SWInstance const& instance, design->getSWInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            if (instance.getComponentRef().isValid())
            {
                emit errorMessage(tr("The SW component '%1' instantiated in the design '%2' "
                                     "was not found in the library").arg(
                                  instance.getComponentRef().getName()).arg(design->getVlnv()->getName()));
            }

            // Create an unpackaged component so that we can still visualize the component instance.
            component = QSharedPointer<Component>(new Component(instance.getComponentRef()));
            component->setComponentImplementation(KactusAttribute::KTS_SW);
        }

        SWComponentItem* item = new SWComponentItem(getLibraryInterface(), component, instance.getInstanceName(),
                                                    instance.getDisplayName(), instance.getDescription());
        connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

        item->setImported(instance.isImported());
        item->setImportRef(instance.getImportRef());
        item->setPos(instance.getPosition());
        item->setPropertyValues(instance.getPropertyValues());
        item->setFileSetRef(instance.getFileSetRef());
        item->setApiInterfacePositions(instance.getApiInterfacePositions(), true);
        item->setComInterfacePositions(instance.getComInterfacePositions(), true);

        if (instance.getMapping().isEmpty())
        {
            // Check if the position is not found.
            if (instance.getPosition().isNull())
            {
                layout_->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % layout_->getColumns().size();
            }
            else
            {
                GraphicsColumn* column = layout_->findColumnAt(instance.getPosition());

                if (column != 0)
                {
                    column->addItem(item, true);
                }
                else
                {
                    layout_->getColumns().at(colIndex)->addItem(item);
                    colIndex = (colIndex + 1) % layout_->getColumns().size();
                }
            }
        }
        else
        {
            // Otherwise the mapping has been specified. Find the corresponding HW component instance.
            HWMappingItem* mappingItem = getHWComponentByUUID(instance.getMapping());

            if (mappingItem != 0)
            {
                mappingItem->addItem(item, true);
            }
            else
            {
                layout_->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % layout_->getColumns().size();
            }
        }

        onComponentInstanceAdded(item);
    }

    // Create SW interface items for the top-level API and COM interfaces.
    foreach (QSharedPointer<ApiInterface> apiIf, getEditedComponent()->getApiInterfaces())
    {
        SWInterfaceItem* item = new SWInterfaceItem(getEditedComponent(), apiIf);

        // Add the interface to the first column where it is allowed to be placed.
        layout_->addItem(item);
    }

    foreach (QSharedPointer<ComInterface> comIf, getEditedComponent()->getComInterfaces())
    {
        SWInterfaceItem* item = new SWInterfaceItem(getEditedComponent(), comIf);

        // Add the interface to the first column where it is allowed to be placed.
        layout_->addItem(item);
    }

    loadApiDependencies(design);
    loadComConnections(design);

    // Refresh the layout of all HW mapping items.
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem* mappingItem = static_cast<HWMappingItem*>(item);
            mappingItem->updateItemPositions();
        }
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    layout_->updatePositions();
}

//-----------------------------------------------------------------------------
// Function: dragEnterEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragType_ = DRAG_TYPE_NONE;

    if (!isProtected() && event->mimeData()->hasImage())
    {
        event->acceptProposedAction();

		QVariant data = event->mimeData()->imageData();
		if (!data.canConvert<VLNV>()) {
			return;
		}

		VLNV vlnv = data.value<VLNV>();

        if (vlnv.getType() == VLNV::COMPONENT)
        {
            // Determine the component type.
            QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(vlnv);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            // component with given vlnv was not found
            if (!comp) {
                emit errorMessage(tr("Component with the VLNV %1:%2:%3:%4 was not found in the library.").arg(
                    vlnv.getVendor()).arg(
                    vlnv.getLibrary()).arg(
                    vlnv.getName()).arg(
                    vlnv.getVersion()));
                return;
            }

            // Only SW and HW is allowed.
            if (comp->getComponentImplementation() == KactusAttribute::KTS_SW)
            {
                dragType_ = DRAG_TYPE_SW;
            }
            else if (comp->getComponentImplementation() == KactusAttribute::KTS_HW)
            {
                dragType_ = DRAG_TYPE_HW;
            }

        }
        else if (vlnv.getType() == VLNV::COMDEFINITION || vlnv.getType() == VLNV::APIDEFINITION)
        {
            dragType_ = DRAG_TYPE_DEFINITION;
        }
        else if (vlnv.getType() == VLNV::DESIGN)
        {
            dragType_ = DRAG_TYPE_DESIGN;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragMoveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    updateDropAction(event);
}

//-----------------------------------------------------------------------------
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragType_ = DRAG_TYPE_NONE;

    if (highlightedEndpoint_ != 0)
    {
        highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
        highlightedEndpoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (dragType_ == DRAG_TYPE_NONE)
    {
        return;
    }

    QVariant data = event->mimeData()->imageData();
	if (!data.canConvert<VLNV>()) {
		return;
	}

	VLNV droppedVLNV = data.value<VLNV>();
    updateDropAction(event);

    // Check if the dragged item was a valid one.
    if (dragType_ == DRAG_TYPE_SW)
    {
        // Disallow self-instantiation.
        if (droppedVLNV == *getEditedComponent()->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component cannot be instantiated to its own design."),
                QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }

        // Retrieve the component model.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        // Set the instance name for the new component instance.
        QString instanceName = createInstanceName(comp);

        // Act based on the selected drop action.
        if (event->dropAction() == Qt::CopyAction)
        {
            // Determine the component stack who gets the component (either HW mapping item or a system column).
            IGraphicsItemStack* stack = 0;

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
            
            if (stack != 0)
            {
                // Create the SW component item.
                SWComponentItem* item = new SWComponentItem(getLibraryInterface(), comp,
                                                            instanceName, QString(), QString(),
                                                            QUuid::createUuid().toString(),
                                                            QMap<QString, QString>());
                
                item->setPos(stack->mapStackFromScene(snapPointToGrid(event->scenePos())));
                connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
                connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                // Create the undo command and execute it.
                QSharedPointer<SystemItemAddCommand> cmd(new SystemItemAddCommand(stack, item));
                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                    this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                getEditProvider().addCommand(cmd);
                cmd->redo();
            }
        }
        else if (event->dropAction() == Qt::MoveAction)
        {
            // Replace the underlying component with the new one.

            // Retrieve the old component (top-most component under the cursor).
            SystemComponentItem* oldCompItem =
                dynamic_cast<SystemComponentItem*>(getTopmostComponent(event->scenePos()));

            Q_ASSERT(oldCompItem != 0);

            // TODO: Determine connectivity issues and ask confirmation from the user.
            //             QStringList detailList;
            // 
            //             foreach (ConnectionEndpoint* endpoint, oldCompItem->getEndpoints())
            //             {
            //                 if (endpoint->isConnected())
            //                 {
            //                     // Check if the endpoint is not found in the new component.
            //                     if (endpoint->getType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS)
            //                     {
            //                     }
            //                 }
            //             }

            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component instance '%1' is about to be replaced "
                "with an instance of %2. Continue and replace?").arg(oldCompItem->name(), droppedVLNV.toString()),
                QMessageBox::Yes | QMessageBox::No, (QWidget*)parent());

            if (msgBox.exec() == QMessageBox::No)
            {
                return;
            }

            // Create the SW component item.
            SWComponentItem* newCompItem = new SWComponentItem(getLibraryInterface(), comp,
                                                               instanceName, QString(), QString(),
                                                               QUuid::createUuid().toString(),
                                                               QMap<QString, QString>());

            // Perform the replacement.
            QSharedPointer<ReplaceSystemComponentCommand>
                cmd(new ReplaceSystemComponentCommand(oldCompItem, newCompItem, false,
                                                      oldCompItem->type() == HWMappingItem::Type));

            connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

            getEditProvider().addCommand(cmd);
            cmd->redo();
        }
    }
    else if (dragType_ == DRAG_TYPE_HW)
    {
        // Retrieve the component.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Component> newComponent = libComp.staticCast<Component>();

        // Check if the component does not have any hierarchical views.
        if (newComponent->getHierViews().isEmpty())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("HW component '%1' does not contain any hierarchical views.").arg(droppedVLNV.toString()),
                               QMessageBox::Ok, parent());
            msgBox.exec();
            return;
        }

        // Determine a default suggestion for the view name.
        QSettings settings;
        QStringList suggestions = settings.value("Policies/SysViewNames").toStringList();
        
        QString baseViewName = "";
        QString viewName = "";

        if (!suggestions.isEmpty())
        {
            baseViewName = suggestions.first();

            viewName = baseViewName;
            unsigned int runningNumber = 1;

            while (newComponent->findSystemView(viewName) != 0)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }

        // Ask the user whether to move or copy the design under the given HW.
        SwitchHWDialog dialog(newComponent, viewName, getLibraryInterface(), parent());
        dialog.setViewNameSuggestions(suggestions);
        dialog.showHWViewSelector();

        if (dialog.exec() == QDialog::Rejected)
        {
            return;
        }

        // Based on the action, either perform copy or move.
        SystemView* newView = new SystemView(dialog.getSystemViewName());
        newView->setHWViewRef(dialog.getHWViewRef());

        if (dialog.isCopyActionSelected())
        {
            // Save copies of the design and design configuration.
            VLNV vlnv = dialog.getVLNV();

            VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                vlnv.getName() + ".sysdesign", vlnv.getVersion());
            VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
                vlnv.getName() + ".sysdesigncfg", vlnv.getVersion());

            QSharedPointer<DesignConfiguration> desConf(new DesignConfiguration(*getDesignConfiguration()));
            desConf->setVlnv(desConfVLNV);
            desConf->setDesignRef(designVLNV);

            QSharedPointer<Design> design = createDesign(designVLNV);

            getLibraryInterface()->writeModelToFile(dialog.getPath(), design);
            getLibraryInterface()->writeModelToFile(dialog.getPath(), desConf);

            // Set the new system view to point to the newly-saved design configuration.
            newView->setHierarchyRef(desConfVLNV);
        }
        else
        {
            // Move is performed by removing the system view from the previous HW component and using
            // the existing VLNV for the hierarchy reference.
            newView->setHierarchyRef(getEditedComponent()->findSystemView(parent()->getOpenViewName())->getHierarchyRef());

            getEditedComponent()->removeSystemView(parent()->getOpenViewName());
            getLibraryInterface()->writeModelToFile(getEditedComponent());
        }

        // Add the system view to the new HW component and save.
        newComponent->addSystemView(newView);
        getLibraryInterface()->writeModelToFile(newComponent);

        // Refresh the design widget.
        parent()->setDesign(droppedVLNV, dialog.getSystemViewName());
        parent()->setProtection(false);
        parent()->refresh();
    }
    else if (dragType_ == DRAG_TYPE_DEFINITION)
    {
        if (highlightedEndpoint_ != 0)
        {
            Q_ASSERT(getLibraryInterface()->contains(droppedVLNV));

            VLNV vlnv = droppedVLNV;
            vlnv.setType(getLibraryInterface()->getDocumentType(droppedVLNV));

            // Save old type and set the new one.
            VLNV oldType = highlightedEndpoint_->getTypeDefinition();
            highlightedEndpoint_->setTypeDefinition(vlnv);

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new TypeDefinitionChangeCommand(highlightedEndpoint_, oldType));
            getEditProvider().addCommand(cmd);

            highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
            highlightedEndpoint_ = 0;
        }
    }
    else if (dragType_ == DRAG_TYPE_DESIGN)
    {
        // Retrieve the design.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Design> design = libComp.dynamicCast<Design>();

        // Import elements to the column under cursor.
        IGraphicsItemStack* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));
        importDesign(design, column, event->scenePos());
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    showContextMenu_ = (getMode() == MODE_SELECT && event->button() == Qt::RightButton);

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
        }
        // Otherwise choose a new start end point if in normal connection mode.
        else if (!offPageMode_)
        {
            offPageMode_ = (event->modifiers() & Qt::ShiftModifier);

            // Deselect all items.
            clearSelection();

            // Try to snap to a connection endpoint.
            SWConnectionEndpoint* endpoint = 
                DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

            if (endpoint == 0 || !endpoint->isVisible())
            {
                return;
            }

            if (offPageMode_)
            {
                if (highlightedEndpoint_ != 0)
                {
                    highlightedEndpoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
                    highlightedEndpoint_ = 0;
                }

                if (endpoint->type() != SWOffPageConnectorItem::Type)
                {
                    endpoint = static_cast<SWConnectionEndpoint*>(endpoint->getOffPageConnector());
                    endpoint->setVisible(true);
                }
            }

            tempConnEndpoint_ = endpoint;
        }

        if (offPageMode_ || !creating)
        {
            tempConnEndpoint_->onBeginConnect();
            
            // Create the connection.
            tempConnection_ = new GraphicsConnection(tempConnEndpoint_->scenePos(),
                                                     tempConnEndpoint_->getDirection(),
                                                     event->scenePos(),
                                                     QVector2D(0.0f, 0.0f), QString(), QString(), this);
            addItem(tempConnection_);

            // Determine all potential endpoints to which the starting endpoint could be connected
            // and highlight them.
            foreach (QGraphicsItem* item, items())
            {
                SWConnectionEndpoint* endpoint = dynamic_cast<SWConnectionEndpoint*>(item);

                if (endpoint != 0 && endpoint->isVisible() &&
                    endpoint != tempConnEndpoint_ &&
                    endpoint->getOffPageConnector() != tempConnEndpoint_ &&
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
            item = itemList.front();

            while (item->parentItem() != 0 && dynamic_cast<SystemComponentItem*>(item) == 0)
            {
                item = item->parentItem();
            }
        }

        // If the item was a SW component, add an undefined interface port to it.
        if (item != 0 && item->type() == SWComponentItem::Type)
        {
            SWComponentItem* comp = static_cast<SWComponentItem*>(item);

            // The component is unpackaged if it has an invalid vlnv.
            if (!comp->componentModel()->getVlnv()->isValid())
            {
                QMap<SWPortItem*, QPointF> oldPositions;

                // Save old port positions.
                foreach (QGraphicsItem* item, comp->childItems())
                {
                    if (item->type() == SWPortItem::Type)
                    {
                        SWPortItem* port = static_cast<SWPortItem*>(item);
                        oldPositions.insert(port, port->pos());
                    }
                }

                QSharedPointer<QUndoCommand> cmd(new SWPortAddCommand(comp, snapPointToGrid(event->scenePos())));
                cmd->redo();

                // Create child undo commands for the changed ports.
                QMap<SWPortItem*, QPointF>::iterator cur = oldPositions.begin();

                while (cur != oldPositions.end())
                {
                    if (cur.key()->pos() != cur.value())
                    {
                        new SWPortMoveCommand(cur.key(), cur.value(), cmd.data());
                    }

                    ++cur;
                }

                // Add the command to the edit stack.
                getEditProvider().addCommand(cmd);
            }
        }
        else if (item == 0 || item->type() == HWMappingItem::Type)
        {
            IGraphicsItemStack* stack = 0;

            if (item != 0)
            {
                stack = dynamic_cast<IGraphicsItemStack*>(item);
            }
            else
            {
                stack = layout_->findColumnAt(event->scenePos());
            }

            if (stack != 0)
            {
                if (stack->getContentType() == COLUMN_CONTENT_COMPONENTS)
                {
                    // Determine an unused name for the component instance.
                    QString name = createInstanceName("instance");

                    // Create a component model without a valid vlnv.
                    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
                    comp->setVlnv(VLNV());
                    comp->setComponentImplementation(KactusAttribute::KTS_SW);

                    // Create the corresponding SW component item.
                    SWComponentItem* swCompItem = new SWComponentItem(getLibraryInterface(), comp, name);
                    swCompItem->setPos(snapPointToGrid(event->scenePos()));

                    connect(swCompItem, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
                    connect(swCompItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                    QSharedPointer<SystemItemAddCommand> cmd(new SystemItemAddCommand(stack, swCompItem));

                    connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
                    connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                    getEditProvider().addCommand(cmd);
                    cmd->redo();
                }
                else if (stack->getContentType() == COLUMN_CONTENT_IO)
                {
                    SWInterfaceItem* newItem = new SWInterfaceItem(getEditedComponent(), "", 0);
                    newItem->setPos(snapPointToGrid(event->scenePos()));

                    // Save the positions of the other interfaces.
                    QMap<SWInterfaceItem*, QPointF> oldPositions;

                    foreach (QGraphicsItem* item, dynamic_cast<SystemColumn*>(stack)->childItems())
                    {
                        if (item->type() == SWInterfaceItem::Type)
                        {
                            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
                            oldPositions.insert(interface, interface->scenePos());
                        }
                    }

                    QSharedPointer<QUndoCommand> cmd(new SystemItemAddCommand(stack, newItem));
                    cmd->redo();

                    // Determine if the other interfaces changed their position and create undo commands for them.
                    QMap<SWInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

                    while (cur != oldPositions.end())
                    {
                        if (cur.key()->scenePos() != cur.value())
                        {
                            new ItemMoveCommand(cur.key(), cur.value(), stack, cmd.data());
                        }

                        ++cur;
                    }

                    getEditProvider().addCommand(cmd);
                }
            }
        }
    }
    else if (getMode() == MODE_TOGGLE_OFFPAGE)
    {
        // Try to snap to a connection end point.
        ConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<ConnectionEndpoint>(event->scenePos(), this, GridSize);

        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

        if (endpoint != 0 && endpoint->isVisible())
        {
            if (endpoint->getConnections().size() > 0)
            {
                hideOffPageConnections();

                QList<GraphicsConnection*> connections = endpoint->getConnections();

                foreach (GraphicsConnection* conn, connections)
                {
                    toggleConnectionStyle(conn, cmd.data());
                }
            }
        }
        else
        {
            QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

            if (item != 0 && item->type() == GraphicsConnection::Type)
            {
                toggleConnectionStyle(static_cast<GraphicsConnection*>(item), cmd.data());
            }
        }

        if (cmd->childCount() > 0)
        {
            getEditProvider().addCommand(cmd);
        }
    }
    else if (getMode() == MODE_SELECT)
    {
        // Check if the user pressed Alt over a component => replace component mode.
        if (!isProtected() && event->modifiers() & Qt::AltModifier)
        {
            // Find the top-most component at the cursor position.
            SystemComponentItem* sourceComp =
                dynamic_cast<SystemComponentItem*>(getTopmostComponent(event->scenePos()));

            if (sourceComp != 0)
            {
                sourceComp_ = sourceComp;
                QApplication::setOverrideCursor(Qt::ForbiddenCursor);
                replaceMode_ = true;
            }
        }
        else
        {
            // Handle the mouse press.
            QGraphicsScene::mousePressEvent(event);
        }
    }
    else if(getMode() == MODE_LABEL)
    {
        if (!isProtected())
        {
            createLabel(event->scenePos());
        }
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
            ComponentItem* item = static_cast<ComponentItem*>(newSelection);
            emit componentSelected(item);

            if (item->componentModel()->getComponentImplementation() == KactusAttribute::KTS_HW)
            {
                emit helpUrlRequested("swsysdesign/hwmappinginstance.html");
            }
            else
            {
                emit helpUrlRequested("swsysdesign/swinstance.html");
            }
        }
        else if (dynamic_cast<ConnectionEndpoint*>(newSelection) != 0)
        {
            emit interfaceSelected(static_cast<ConnectionEndpoint*>(newSelection));
        }
        else if (dynamic_cast<GraphicsConnection*>(newSelection) != 0)
        {
            GraphicsConnection* conn = static_cast<GraphicsConnection*>(newSelection);
            emit connectionSelected(conn);

            if (conn->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                emit helpUrlRequested("swsysdesign/apiconnection.html");
            }
            else if (conn->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                emit helpUrlRequested("swsysdesign/comconnection.html");
            }
            else
            {
                emit helpUrlRequested("swsysdesign/undefinedconnection.html");
            }
        }
        else
        {
            // Otherwise inform others that nothing is currently selected.
            emit clearItemSelection();

            if (onlySW_)
            {
                emit helpUrlRequested("swsysdesign/swdesign.html");
            }
            else
            {
                emit helpUrlRequested("swsysdesign/systemdesign.html");
            }
        }
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();

        if (onlySW_)
        {
            emit helpUrlRequested("swsysdesign/swdesign.html");
        }
        else
        {
            emit helpUrlRequested("swsysdesign/systemdesign.html");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    contextPos_ = QCursor::pos();

    // Check if the connect mode is active.
    if (getMode() == MODE_CONNECT || getMode() == MODE_TOGGLE_OFFPAGE)
    {
        // Find out if there is an endpoint currently under the cursor.
        SWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

        if (tempConnection_)
        {
            // Check if there was a valid endpoint close enough.
            if (endpoint != 0 && !endpoint->isInvalid() && tempPotentialEndingEndpoints_.contains(endpoint))
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

            GraphicsConnection* newTempConnection_ = 0;

            if (highlightedEndpoint_ != 0)
            {
                newTempConnection_ = new GraphicsConnection(tempConnEndpoint_, highlightedEndpoint_, false,
                                                      QString(), QString(), QString(), this);
            }
            else
            {
                newTempConnection_ = new GraphicsConnection(tempConnEndpoint_->scenePos(),
                                                      tempConnEndpoint_->getDirection(),
                                                      snapPointToGrid(event->scenePos()),
                                                      QVector2D(0.0f, 0.0f), QString(), QString(), this);
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

    if (replaceMode_)
    {
        SystemComponentItem* destComp =
            dynamic_cast<SystemComponentItem*>(getTopmostComponent(event->scenePos()));

        if (destComp != 0 && destComp != sourceComp_)
        {
            QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
        }
        else
        {
            QApplication::changeOverrideCursor(Qt::ForbiddenCursor);
        }
    }

    // Allow moving items only when a single item is selected.
    if (selectedItems().count() == 1)
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Check if we're replacing a component.
    if (replaceMode_)
    {
        replaceMode_ = false;
        QApplication::restoreOverrideCursor();

        SystemComponentItem* destComp =
            dynamic_cast<SystemComponentItem*>(getTopmostComponent(event->scenePos()));

        if (destComp == 0 || destComp == sourceComp_)
        {
            return;
        }

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("Component instance '%1' is about to be switched in place "
                              "with '%2'. Continue and replace?").arg(destComp->name(), sourceComp_->name()),
                           QMessageBox::Yes | QMessageBox::No, (QWidget*)parent());

        if (msgBox.exec() == QMessageBox::No)
        {
            return;
        }

        // Perform the replacement.
        QSharedPointer<ReplaceSystemComponentCommand>
            cmd(new ReplaceSystemComponentCommand(destComp, sourceComp_, true, true));

        connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider().addCommand(cmd);
        cmd->redo();
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
    QSharedPointer<Design> design = DesignDiagram::createDesign(vlnv);

    QList<ComponentInstance> instances;
    QList<SWInstance> swInstances;
    QList<ApiDependency> apiDependencies;
    QList<HierApiDependency> hierApiDependencies;
    QList<ComConnection> comConnections;
    QList<HierComConnection> hierComConnections;
    QList<ColumnDesc> columns;

    foreach (QGraphicsItem const* item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem const* mappingItem = static_cast<HWMappingItem const*>(item);

            ComponentInstance instance(mappingItem->name(), mappingItem->displayName(),
                                       mappingItem->description(),
                                       *mappingItem->componentModel()->getVlnv(),
                                       mappingItem->scenePos(), mappingItem->getUuid());
            instance.setConfigurableElementValues(mappingItem->getConfigurableElements());
            instance.setImported(mappingItem->isImported());
            instance.setImportRef(mappingItem->getImportRef());
            instance.setPropertyValues(mappingItem->getPropertyValues());

            // Save API and COM interface positions.
            QListIterator<QSharedPointer<ApiInterface> >
                itrApiIf(mappingItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                QSharedPointer<ApiInterface> apiIf = itrApiIf.next();
                instance.updateApiInterfacePosition(apiIf->getName(),
                    mappingItem->getSWPort(apiIf->getName(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos());
            }

            QListIterator<QSharedPointer<ComInterface> >
                itrComIf(mappingItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                QSharedPointer<ComInterface> comIf = itrComIf.next();
                instance.updateComInterfacePosition(comIf->getName(),
                    mappingItem->getSWPort(comIf->getName(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
            }

            instances.append(instance);
        }
        else if (item->type() == SWComponentItem::Type)
        {
            SWComponentItem const* swCompItem = static_cast<SWComponentItem const*>(item);

            SWInstance instance;
            instance.setInstanceName(swCompItem->name());
            instance.setDisplayName(swCompItem->displayName());
            instance.setDescription(swCompItem->description());
            instance.setComponentRef(*swCompItem->componentModel()->getVlnv());
            instance.setPropertyValues(swCompItem->getPropertyValues());
            instance.setFileSetRef(swCompItem->getFileSetRef());
            instance.setImported(swCompItem->isImported());
            instance.setImportRef(swCompItem->getImportRef());
                        
            if (swCompItem->parentItem()->type() == HWMappingItem::Type)
            {
                HWMappingItem const* parent = static_cast<HWMappingItem*>(swCompItem->parentItem());
                instance.setMapping(parent->getUuid());
                instance.setPosition(swCompItem->pos());
            }
            else
            {
                instance.setPosition(swCompItem->scenePos());
            }

            // Save API and COM interface positions.
            QListIterator<QSharedPointer<ApiInterface> >
                itrApiIf(swCompItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                QSharedPointer<ApiInterface> apiIf = itrApiIf.next();
                instance.updateApiInterfacePosition(apiIf->getName(),
                    swCompItem->getSWPort(apiIf->getName(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos());
            }

            QListIterator<QSharedPointer<ComInterface> >
                itrComIf(swCompItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                QSharedPointer<ComInterface> comIf = itrComIf.next();
                instance.updateComInterfacePosition(comIf->getName(),
                    swCompItem->getSWPort(comIf->getName(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
            }

            // Hack: Save undefined interfaces as COM interfaces.
            foreach (QGraphicsItem* childItem, item->childItems())
            {
                SWPortItem* portItem = dynamic_cast<SWPortItem*>(childItem);

                if (portItem != 0 && portItem->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
                {
                    instance.updateComInterfacePosition(portItem->name(),
                        swCompItem->getSWPort(portItem->name(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
                }
            }

            swInstances.append(instance);
        }
        else if (item->type() == GraphicsConnection::Type)
        {
            GraphicsConnection const* conn = static_cast<GraphicsConnection const*>(item);

            ConnectionEndpoint* endpoint1 = conn->endpoint1();
            ConnectionEndpoint* endpoint2 = conn->endpoint2();

            if (conn->getConnectionType() == SWConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                if (endpoint1->encompassingComp() != 0 && endpoint2->encompassingComp() != 0)
                {
                    ApiInterfaceRef providerRef(endpoint1->encompassingComp()->name(),
                                                endpoint1->name());
                    ApiInterfaceRef requesterRef(endpoint2->encompassingComp()->name(),
                                                 endpoint2->name());

                    ApiDependency dependency(conn->name(), QString(), conn->description(),
                                             providerRef, requesterRef, conn->route());
                    dependency.setImported(conn->isImported());
                    dependency.setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

                    apiDependencies.append(dependency);
                }
                else
                {
                    ConnectionEndpoint* compPort = endpoint1;
                    ConnectionEndpoint* hierPort = endpoint2;

                    if (compPort->encompassingComp() == 0)
                    {
                        std::swap(compPort, hierPort);
                    }

                    ApiInterfaceRef ref(compPort->encompassingComp()->name(), compPort->name());

                    HierApiDependency hierDependency(conn->name(), QString(),
                                                     conn->description(),
                                                     hierPort->name(), ref,
                                                     hierPort->scenePos(), hierPort->getDirection(),
                                                     conn->route());
                    hierDependency.setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);


                    hierApiDependencies.append(hierDependency);
                }
            }
            else// if (conn->getConnectionType() == SWConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                if (endpoint1->encompassingComp() != 0 && endpoint2->encompassingComp() != 0)
                {
                    ComInterfaceRef ref1(endpoint1->encompassingComp()->name(),
                                         endpoint1->name());
                    ComInterfaceRef ref2(endpoint2->encompassingComp()->name(),
                                         endpoint2->name());

                    ComConnection comConnection(conn->name(), QString(), conn->description(),
                                                ref1, ref2, conn->route());
                    comConnection.setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

                    comConnections.append(comConnection);
                }
                else
                {
                    ConnectionEndpoint* compPort = endpoint1;
                    ConnectionEndpoint* hierPort = endpoint2;

                    if (compPort->encompassingComp() == 0)
                    {
                        std::swap(compPort, hierPort);
                    }

                    if (endpoint2->getComInterface() != 0)
                    {
                        ComInterfaceRef ref(compPort->encompassingComp()->name(), compPort->name());

                        HierComConnection hierComConnection(conn->name(), QString(),
                                                            conn->description(),
                                                            hierPort->name(), ref,
                                                            hierPort->scenePos(), hierPort->getDirection(),
                                                            conn->route());
                        hierComConnection.setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

                        hierComConnections.append(hierComConnection);
                    }
                }
            }
        }
    }

    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        columns.append(column->getColumnDesc());
    }

    design->setComponentInstances(instances);
    design->setSWInstances(swInstances);
    design->setApiDependencies(apiDependencies);
    design->setHierApiDependencies(hierApiDependencies);
    design->setComConnections(comConnections);
    design->setHierComConnections(hierComConnections);
    design->setColumns(columns);   

    return design;
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addColumn(ColumnDesc const& desc)
{
    SystemColumn* column = new SystemColumn(desc, layout_.data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(layout_.data(), column));
    getEditProvider().addCommand(cmd);
    cmd->redo();
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
    QGraphicsItem *item = itemAt(snapPointToGrid(event->scenePos()), QTransform());

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

    if (dynamic_cast<SystemComponentItem*>(item) != 0)
    {
        item->setSelected(true);
        SystemComponentItem* comp = static_cast<SystemComponentItem*>(item);

        if (getLibraryInterface()->contains(*comp->componentModel()->getVlnv()))
        {
            QString viewName;
            QStringList hierViews = comp->componentModel()->getSWViewNames();

            // if configuration is used and it contains an active view for the instance
            if (getDesignConfiguration() && getDesignConfiguration()->hasActiveView(comp->name())) {
                viewName = getDesignConfiguration()->getActiveView(comp->name());

                QSharedPointer<SWView> view = comp->componentModel()->findSWView(viewName);

                // if view was found
                if (view)
                {
                    emit openSWDesign(*comp->componentModel()->getVlnv(), viewName);
                }
                else
                {
                    emit openComponent(*comp->componentModel()->getVlnv());
                }
                // if view was not found
//                 else {
//                     emit errorMessage(tr("The active view %1 was not found in "
//                         "instance %2").arg(viewName).arg(comp->name()));
//                 }
            }
            // If the component does not contain any hierarchical views, open the component editor.
            else if (hierViews.size() == 0)
            {
                if (comp->componentModel()->hasViews())
                {
                    emit noticeMessage(tr("No active view was selected for instance %1, "
                        "opening component editor.").arg(comp->name()));
                }

                emit openComponent(*comp->componentModel()->getVlnv());
            }
            // Open the first design if there is one or multiple hierarchical view.
            else
            {
                emit noticeMessage(tr("No active view was selected for instance %1, "
                    "opening the only hierarhical view of the component.").arg(comp->name()));
                emit openSWDesign(*comp->componentModel()->getVlnv(), hierViews.first());
            }
        }
        else if (!isProtected())
        {
            // Otherwise this is an unpackaged component.

            // Request the user to set the vlnv.
            NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, false, (QWidget*)parent());
            dialog.setVLNV(*comp->componentModel()->getVlnv());
            dialog.setWindowTitle(tr("Add Component to Library"));

            if (dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            VLNV vlnv = dialog.getVLNV();
            comp->componentModel()->setVlnv(vlnv);
            comp->componentModel()->setComponentHierarchy(dialog.getProductHierarchy());
            comp->componentModel()->setComponentFirmness(dialog.getFirmness());

            if (comp->type() == HWMappingItem::Type)
            {
                comp->componentModel()->createEmptyFlatView();
            }

            // Write the model to file.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Create an undo command.
            QSharedPointer<ComponentPacketizeCommand> cmd(new ComponentPacketizeCommand(comp, vlnv));
            getEditProvider().addCommand(cmd);
            cmd->redo();

            // Ask the user if he wants to complete the component.
            QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                               "Do you want to continue packaging the component completely?",
                               QMessageBox::NoButton, (QWidget*)parent());
            msgBox.setInformativeText("Pressing Continue opens up the component editor.");
            QPushButton* btnContinue = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
            msgBox.addButton(tr("Skip"), QMessageBox::RejectRole);

            msgBox.exec();

            if (msgBox.clickedButton() == btnContinue)
            {
                // Open up the component editor.
                emit openComponent(*comp->componentModel()->getVlnv());
            }
        }
    }
    else if (item->type() == SystemColumn::Type)
    {
        if (!isProtected())
        {
            item->setSelected(true);
            SystemColumn* column = qgraphicsitem_cast<SystemColumn*>(item);

            ColumnEditDialog dialog((QWidget*)parent(), onlySW_, column);

            // System design columns don't have adjustable content settings.
            if (!onlySW_)
            {
                dialog.hideContentSettings();
            }

            if (dialog.exec() == QDialog::Accepted)
            {
                int columnWidth = SYSTEM_COLUMN_WIDTH;

                if (onlySW_)
                {
                    columnWidth = SW_COLUMN_WIDTH;

                    if (dialog.getContentType() == COLUMN_CONTENT_IO)
                    {
                        columnWidth = IO_COLUMN_WIDTH;
                    }
                }

                ColumnDesc desc(dialog.getName(), dialog.getContentType(), dialog.getAllowedItems(), columnWidth);

                // Keep the previous width if the content type did not change.
                if (dialog.getContentType() == column->getColumnDesc().getContentType())
                {
                    desc.setWidth(column->getColumnDesc().getWidth());
                }

                QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc));
                getEditProvider().addCommand(cmd);
                cmd->redo();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout* SystemDesignDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onComponentInstanceAdded(ComponentItem* item)
{
    DesignDiagram::onComponentInstanceAdded(item);
}

//-----------------------------------------------------------------------------
// Function: onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    DesignDiagram::onComponentInstanceRemoved(item);
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
SystemComponentItem* SystemDesignDiagram::getComponent(QString const& instanceName)
{
    foreach (QGraphicsItem *item, items())
    {
        SystemComponentItem* comp = dynamic_cast<SystemComponentItem*>(item);

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
// Function: getHWComponent()
//-----------------------------------------------------------------------------
HWMappingItem* SystemDesignDiagram::getHWComponentByUUID(QString const& uuid)
{
    foreach (QGraphicsItem *item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem* comp = qgraphicsitem_cast<HWMappingItem*>(item);

            if (comp->getUuid() == uuid)
            {
                return comp;
            }
        }
    }

    emit errorMessage(tr("Component with UUID %1 was not found in the design").arg(uuid));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::createConnection(QGraphicsSceneMouseEvent* event)
{
    // Disable highlights from all potential endpoints.
    for (int i = 0 ; i < tempPotentialEndingEndpoints_.size(); ++i)
    {
        tempPotentialEndingEndpoints_.at(i)->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
        tempPotentialEndingEndpoints_.at(i)->onEndConnect();
    }

    tempPotentialEndingEndpoints_.clear();

    // Check if there is no endpoint close enough to the cursor or the endpoints cannot be connected together.
    SWConnectionEndpoint* endpoint =
        DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

    if (endpoint == 0 || endpoint == tempConnEndpoint_ || !endpoint->isVisible() ||
        !endpoint->canConnect(tempConnEndpoint_) || !tempConnEndpoint_->canConnect(endpoint))
    {
        if (!offPageMode_)
        {
            tempConnEndpoint_->onEndConnect();
        }

        discardConnection();
    }
    else 
    {
        // Check if the connection should be converted to an off-page connection.
        bool firstOffPage = tempConnEndpoint_->type() == SWOffPageConnectorItem::Type;
        bool secondOffPage = endpoint->type() == SWOffPageConnectorItem::Type;

        if (offPageMode_ ||
            ((firstOffPage || secondOffPage) && tempConnEndpoint_->type() != endpoint->type()))
        {
            delete tempConnection_;

            if (!firstOffPage)
            {
                tempConnEndpoint_ = static_cast<SWConnectionEndpoint*>(tempConnEndpoint_->getOffPageConnector());
            }

            if (!secondOffPage)
            {
                endpoint = static_cast<SWConnectionEndpoint*>(endpoint->getOffPageConnector());
            }

            tempConnection_ = new GraphicsConnection(tempConnEndpoint_, endpoint, false,
                                                     QString(), QString(), QString(), this);
            addItem(tempConnection_);
        }

        // Make the temporary connection a permanent one by connecting the ends.
        if (tempConnection_->connectEnds())
        {
            tempConnection_->fixOverlap();

            QSharedPointer<QUndoCommand> cmd(new SWConnectionAddCommand(this, tempConnection_));
            getEditProvider().addCommand(cmd);

            tempConnection_ = 0;
            
            if (!offPageMode_)
            {
                tempConnEndpoint_ = 0;
            }
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
        if (item->type() == GraphicsConnection::Type)
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
        if (tempConnEndpoint_ != 0 && highlightedEndpoint_ != tempConnEndpoint_ &&
            tempPotentialEndingEndpoints_.contains(highlightedEndpoint_))
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
        SystemComponentItem* comp1 = getComponent(dependency.getInterface1().componentRef);

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                dependency.getInterface1().componentRef));
            continue;
        }

        SystemComponentItem* comp2 = getComponent(dependency.getInterface2().componentRef);

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                dependency.getInterface2().componentRef));
            continue;
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = static_cast<SWPortItem*>(comp1->getSWPort(dependency.getInterface1().apiRef,
                                                                              SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface1().apiRef).arg(dependency.getInterface1().componentRef));
            
            port1 = createMissingPort(dependency.getInterface1().apiRef,
                                      SWConnectionEndpoint::ENDPOINT_TYPE_API, comp1, design);
        }

        ConnectionEndpoint* port2 = static_cast<SWPortItem*>(comp2->getSWPort(dependency.getInterface2().apiRef,
                                                                              SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface2().apiRef).arg(dependency.getInterface2().componentRef));
            
            port2 = createMissingPort(dependency.getInterface2().apiRef,
                                      SWConnectionEndpoint::ENDPOINT_TYPE_API, comp2, design);
        }

        if (dependency.isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
                                                                dependency.getName(),
                                                                dependency.getDisplayName(),
                                                                dependency.getDescription(), this);
        connection->setRoute(dependency.getRoute());
        connection->setImported(dependency.isImported());

        if (dependency.isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }

    // Load hierarchical dependencies.
    foreach (HierApiDependency const& dependency, design->getHierApiDependencies())
    {
        QSharedPointer<ApiInterface> apiIf =
            getEditedComponent()->getApiInterface(dependency.getInterfaceRef());
        ConnectionEndpoint* interface = 0;

        if (apiIf == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the top-component").arg(
                dependency.getInterfaceRef()));
            
            // Create a dummy interface which is marked as invalid.
            interface = new SWInterfaceItem(getEditedComponent(), dependency.getInterfaceRef(), 0);
            interface->setTemporary(true);
            interface->updateInterface();
        }
        else
        {
            // Find the corresponding SW interface item.
            foreach (QGraphicsItem* item, items())
            {
                if (item->type() == SWInterfaceItem::Type &&
                    static_cast<SWInterfaceItem*>(item)->getApiInterface() == apiIf)
                {
                    interface = static_cast<SWInterfaceItem*>(item);
                    break;
                }
            }
        }

        Q_ASSERT(interface != 0);

        // Check if the position is found.
        if (!dependency.getPosition().isNull())
        {
            interface->setPos(dependency.getPosition());
            interface->setDirection(dependency.getDirection());

            GraphicsColumn* column = layout_->findColumnAt(dependency.getPosition());

            if (column != 0)
            {
                column->addItem(interface);
            }
            else
            {
                layout_->addItem(interface);
            }
        }

        // Find the component where the hierarchical dependency is connected to.
        SystemComponentItem* componentItem = getComponent(dependency.getInterface().componentRef);

        if (componentItem == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the top-design").arg(
                dependency.getInterface().componentRef));
            continue;
        }

        // Find the port of the component.
        ConnectionEndpoint* port =
            componentItem->getSWPort(dependency.getInterface().apiRef, SWConnectionEndpoint::ENDPOINT_TYPE_API);

        if (port == 0)
        {
            emit errorMessage(tr("Port '%1' was not found in the component '%2'").arg(
                dependency.getInterface().apiRef, dependency.getInterface().componentRef));

            port = createMissingPort(dependency.getInterface().apiRef,
                                     SWConnectionEndpoint::ENDPOINT_TYPE_API, componentItem, design);
        }

        if (dependency.isOffPage())
        {
            port = port->getOffPageConnector();
            interface = interface->getOffPageConnector();
        }
        
        GraphicsConnection* connection = new GraphicsConnection(port, interface, true,
                                                                dependency.getName(),
                                                                dependency.getDisplayName(),
                                                                dependency.getDescription(), this);
        connection->setRoute(dependency.getRoute());

        if (dependency.isOffPage())
        {
            connection->setVisible(false);
        }

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
        SystemComponentItem* comp1 = getComponent(conn.getInterface1().componentRef);

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                conn.getInterface1().componentRef));
            continue;
        }

        SystemComponentItem* comp2 = getComponent(conn.getInterface2().componentRef);

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                conn.getInterface2().componentRef));
            continue;
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = comp1->getSWPort(conn.getInterface1().comRef,
                                                     SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port1 == 0)
        {
            emit errorMessage(tr("COM interface '%1' was not found in the component '%2'").arg(
                conn.getInterface1().comRef).arg(conn.getInterface1().componentRef));

            port1 = createMissingPort(conn.getInterface1().comRef, ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED,
                                      comp1, design);
        }

        ConnectionEndpoint* port2 = comp2->getSWPort(conn.getInterface2().comRef,
                                                     SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port2 == 0)
        {
            emit errorMessage(tr("COM interface '%1' was not found in the component '%2'").arg(
                conn.getInterface2().comRef).arg(conn.getInterface2().componentRef));
            
            port2 = createMissingPort(conn.getInterface2().comRef, ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED,
                                      comp2, design);
        }
        
        if (conn.isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
                                                                conn.getName(),
                                                                conn.getDisplayName(),
                                                                conn.getDescription(), this);
        connection->setRoute(conn.getRoute());

        if (conn.isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }

    // Load hierarchical COM connections.
    foreach (HierComConnection const& hierConn, design->getHierComConnections())
    {
        QSharedPointer<ComInterface> comIf =
            getEditedComponent()->getComInterface(hierConn.getInterfaceRef());
        ConnectionEndpoint* interface = 0;

        if (comIf == 0)
        {
            emit errorMessage(tr("COM interface '%1' was not found in the top-component").arg(
                hierConn.getInterfaceRef()));

            // Create a dummy interface which is marked as invalid.
            interface = new SWInterfaceItem(getEditedComponent(), hierConn.getInterfaceRef(), 0);
            interface->setTemporary(true);
            interface->updateInterface();
        }
        else
        {
            // Find the corresponding SW interface item.
            foreach (QGraphicsItem* item, items())
            {
                if (item->type() == SWInterfaceItem::Type &&
                    static_cast<SWInterfaceItem*>(item)->getComInterface() == comIf)
                {
                    interface = static_cast<SWInterfaceItem*>(item);
                    break;
                }
            }
        }

        Q_ASSERT(interface != 0);

        // Check if the position is found.
        if (!hierConn.getPosition().isNull())
        {
            interface->setPos(hierConn.getPosition());
            interface->setDirection(hierConn.getDirection());

            GraphicsColumn* column = layout_->findColumnAt(hierConn.getPosition());

            if (column != 0)
            {
                column->addItem(interface);
            }
            else
            {
                layout_->addItem(interface);
            }
        }

        // Find the component where the hierarchical hierConn is connected to.
        SystemComponentItem* componentItem = getComponent(hierConn.getInterface().componentRef);

        if (componentItem == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the top-design").arg(
                hierConn.getInterface().componentRef));
            continue;
        }

        // Find the port of the component.
        ConnectionEndpoint* port =
            componentItem->getSWPort(hierConn.getInterface().comRef, SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port == 0)
        {
            emit errorMessage(tr("Port '%1' was not found in the component '%2'").arg(
                hierConn.getInterface().comRef, hierConn.getInterface().componentRef));

            port = createMissingPort(hierConn.getInterface().comRef, ConnectionEndpoint::ENDPOINT_TYPE_COM,
                                     componentItem, design);
        }

        if (hierConn.isOffPage())
        {
            port = port->getOffPageConnector();
            interface = interface->getOffPageConnector();
        }
        
        GraphicsConnection* connection = new GraphicsConnection(port, interface, true,
                                                                hierConn.getName(),
                                                                hierConn.getDisplayName(),
                                                                hierConn.getDescription(), this);
        connection->setRoute(hierConn.getRoute());

        if (hierConn.isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::updateHierComponent()
{
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onSelectionChanged()
{
    // Disallow selecting elements of different type.
    if (selectedItems().size() > 1)
    {
        // Check if all selected items have the same type.
        int type = getCommonItemType(selectedItems());

        if (type == -1)
        {
            // If not, deselect those that are in the new selection but no in the old one.
            for (int i = 0; i < selectedItems().size(); ++i)
            {
                QGraphicsItem* item = selectedItems()[i];

                if (!oldSelectedItems_.contains(item))
                {
                    item->setSelected(false);
                }
            }
        }
    }

    // Retrieve the new selection.
    QGraphicsItem* newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    // If the old selection was an off-page connector, hide its connections.
    // Also hide the previously selected connection if it was an off-page connection.
    for (int i = 0; i < oldSelectedItems_.size(); ++i)
    {
        QGraphicsItem* oldSelection = oldSelectedItems_[i];

        if (oldSelection->type() == SWOffPageConnectorItem::Type)
        {
            SWOffPageConnectorItem* connector = static_cast<SWOffPageConnectorItem*>(oldSelection);

            foreach (GraphicsConnection* conn, connector->getConnections())
            {
                if (conn != newSelection)
                {
                    conn->setVisible(false);
                }
            }
        }
        else if (oldSelection->type() == GraphicsConnection::Type && !selectedItems().contains(oldSelection))
        {
            GraphicsConnection* conn = static_cast<GraphicsConnection*>(oldSelection);

            if (conn->endpoint1() != 0)
            {
                if (conn->endpoint1()->type() == SWOffPageConnectorItem::Type)
                {
                    oldSelection->setVisible(false);
                }
                else
                {
                    oldSelection->setZValue(-1000);
                }
            }
        }
    }

    // If the new selection is an off-page connector, show its connections.
    if (newSelection != 0 && newSelection->type() == SWOffPageConnectorItem::Type)
    {
        SWOffPageConnectorItem* connector = static_cast<SWOffPageConnectorItem*>(newSelection);

        foreach (GraphicsConnection* conn, connector->getConnections())
        {
            conn->setVisible(true);
        }
    }

    onSelected(newSelection);

    // Save the current selection as the old selection.
    oldSelectedItems_ = selectedItems();
    prepareContextMenuActions();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createMissingPort()
//-----------------------------------------------------------------------------
SWPortItem* SystemDesignDiagram::createMissingPort(QString const& portName, ConnectionEndpoint::EndpointType type,
                                                   SystemComponentItem* component,
                                                   QSharedPointer<Design> design)
{
    SWPortItem* port = new SWPortItem(portName, component);
    component->addPort(port);

    foreach (ComponentInstance const& instance, design->getComponentInstances())
    {
        if (instance.getInstanceName() == component->name())
        {
            if (type == ConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                port->setPos(instance.getApiInterfacePositions().value(portName));
            }
            else if (type == ConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                port->setPos(instance.getComInterfacePositions().value(portName));
            }

            component->onMovePort(port);
            return port;
        }
    }

    foreach (SWInstance const& instance, design->getSWInstances())
    {
        if (instance.getInstanceName() == component->name())
        {
            if (type == ConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                port->setPos(instance.getApiInterfacePositions().value(portName));
            }
            else if (type == ConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                port->setPos(instance.getComInterfacePositions().value(portName));
            }

            component->onMovePort(port);
            return port;
        }
    }

    return port;
}

//-----------------------------------------------------------------------------
// Function: hideOffPageConnections()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::hideOffPageConnections()
{
    foreach (QGraphicsItem* item, items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0 && conn->endpoint1()->type() == SWOffPageConnectorItem::Type)
        {
            conn->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: keyReleaseEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::keyReleaseEvent(QKeyEvent *event)
{
    // Check if the user ended the off-page connection mode.
    if ((event->key() == Qt::Key_Shift) && offPageMode_)
    {
        if (tempConnEndpoint_ != 0)
        {
            if (tempConnEndpoint_->getConnections().size() == 0)
            {
                tempConnEndpoint_->setVisible(false);
            }
        }

        endConnect();
    }
}

//-----------------------------------------------------------------------------
// Function: endConnect()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::endConnect()
{
    // Discard the connection that was being drawn.
    discardConnection();

    // Disable highlights from all end points.
    disableHighlights();
    
    // Disable off-page mode.
    offPageMode_ = false;
}

//-----------------------------------------------------------------------------
// Function: toggleConnectionStyle()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd)
{
    Q_ASSERT(parentCmd != 0);
    emit clearItemSelection();

    conn->toggleOffPage();
    new ConnectionToggleOffPageCommand(conn, parentCmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::importDesign()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::importDesign(QSharedPointer<Design> design, IGraphicsItemStack* stack,
                                       QPointF const& guidePos)
{
    // Map which hold name mappings for SW instances.
    QMap<QString, QString> nameMappings;

    // Import SW instances.
    foreach (SWInstance const& instance, design->getSWInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            emit errorMessage(tr("SW component %1 instantiated in the imported design was not "
                                 "found in the library").arg(instance.getComponentRef().toString()));

            // Create an unpackaged component so that we can still visualize the component instance.
            component = QSharedPointer<Component>(new Component(instance.getComponentRef()));
            component->setComponentImplementation(KactusAttribute::KTS_SW);
        }

        // Determine a unique name for the instance.
        QString instanceName = instance.getInstanceName();

        if (getComponent(instanceName) != 0)
        {
            instanceName = createInstanceName(instanceName);
        }

        nameMappings.insert(instance.getInstanceName(), instanceName);

        SWComponentItem* item = new SWComponentItem(getLibraryInterface(), component, instanceName,
                                                    instance.getDisplayName(), instance.getDescription());
        item->setImported(instance.isImported());
        item->setImportRef(instance.getImportRef());
        item->setPos(stack->mapStackFromScene(guidePos));
        item->setPropertyValues(instance.getPropertyValues());
        item->setFileSetRef(instance.getFileSetRef());
        item->setApiInterfacePositions(instance.getApiInterfacePositions(), true);
        item->setComInterfacePositions(instance.getComInterfacePositions(), true);

        connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
        
        // Add the instance to the specified stack.
        stack->addItem(item);
        addInstanceName(instance.getInstanceName());
    }

    // Import API dependencies.
    foreach (ApiDependency const& dependency, design->getApiDependencies())
    {
        // Find the referenced components.
        SystemComponentItem* comp1 = getComponent(nameMappings.value(dependency.getInterface1().componentRef));

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                nameMappings.value(dependency.getInterface1().componentRef)));
            continue;
        }

        SystemComponentItem* comp2 = getComponent(nameMappings.value(dependency.getInterface2().componentRef));

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                nameMappings.value(dependency.getInterface2().componentRef)));
            continue;
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = static_cast<SWPortItem*>(comp1->getSWPort(dependency.getInterface1().apiRef,
            SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface1().apiRef).arg(dependency.getInterface1().componentRef));

            port1 = createMissingPort(dependency.getInterface1().apiRef,
                SWConnectionEndpoint::ENDPOINT_TYPE_API, comp1, design);
        }

        ConnectionEndpoint* port2 = static_cast<SWPortItem*>(comp2->getSWPort(dependency.getInterface2().apiRef,
            SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                dependency.getInterface2().apiRef).arg(dependency.getInterface2().componentRef));

            port2 = createMissingPort(dependency.getInterface2().apiRef,
                SWConnectionEndpoint::ENDPOINT_TYPE_API, comp2, design);
        }

        if (dependency.isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
            dependency.getName(),
            dependency.getDisplayName(),
            dependency.getDescription(), this);
        connection->setImported(dependency.isImported());

        if (dependency.isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }

    // Import COM connections.
    foreach (ComConnection const& conn, design->getComConnections())
    {
        // Find the referenced components.
        SystemComponentItem* comp1 = getComponent(nameMappings.value(conn.getInterface1().componentRef));

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                nameMappings.value(conn.getInterface1().componentRef)));
            continue;
        }

        SystemComponentItem* comp2 = getComponent(nameMappings.value(conn.getInterface2().componentRef));

        if (comp2 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").arg(
                nameMappings.value(conn.getInterface2().componentRef)));
            continue;
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = comp1->getSWPort(conn.getInterface1().comRef,
            SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                conn.getInterface1().comRef).arg(conn.getInterface1().componentRef));
            continue;
        }

        ConnectionEndpoint* port2 = comp2->getSWPort(conn.getInterface2().comRef,
            SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").arg(
                conn.getInterface2().comRef).arg(conn.getInterface2().componentRef));
            continue;
        }

        if (conn.isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
                                                                conn.getName(),
                                                                conn.getDisplayName(),
                                                                conn.getDescription(), this);

        if (conn.isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::updateDropAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::updateDropAction(QGraphicsSceneDragDropEvent* event)
{
    if (dragType_ == DRAG_TYPE_SW)
    {
        // Find the top-most component under the cursor.
        ComponentItem* item = getTopmostComponent(event->scenePos());

        // If the underlying object is a HW mapping item, accept the drag here.
        // TODO: Alt modifier for MoveAction?
        if (item != 0 && item->type() == HWMappingItem::Type)
        {
            HWMappingItem* mappingItem = static_cast<HWMappingItem*>(item);

            if (mappingItem->componentModel()->isCpu())
            {
                event->setDropAction(Qt::CopyAction);
            }
            else
            {
                event->setDropAction(Qt::MoveAction);
            }
        }
        else if (item != 0 && item->type() == SWComponentItem::Type)
        {
            SWComponentItem* swCompItem = static_cast<SWComponentItem*>(item);

            if (!swCompItem->isImported())
            {
                event->setDropAction(Qt::MoveAction);
            }
            else
            {
                event->setDropAction(Qt::IgnoreAction);
            }
        }
        else
        {
            // Otherwise check which column should receive the SW component.
            GraphicsColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

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
    else if (dragType_ == DRAG_TYPE_HW)
    {
        QVariant data = event->mimeData()->imageData();

        if (!data.canConvert<VLNV>())
        {
            return;
        }

        VLNV vlnv = data.value<VLNV>();

        if (vlnv != *getEditedComponent()->getVlnv())
        {
            event->setDropAction(Qt::LinkAction);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }

        event->accept();
    }
    else if (dragType_ == DRAG_TYPE_DEFINITION)
    {
        // Check if there is an endpoint close enough the cursor.
        SWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

        if (highlightedEndpoint_ != 0)
        {
            highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_OFF);
            highlightedEndpoint_ = 0;
        }

        if (endpoint != 0 && !endpoint->isInvalid())
        {
            highlightedEndpoint_ = endpoint;
        }

        // Allow the drop event if the end point is undefined or there are no connections
        // and the encompassing component is unpackaged.
        if (highlightedEndpoint_ != 0 &&
            (highlightedEndpoint_->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED ||
            (!highlightedEndpoint_->isConnected() && highlightedEndpoint_->getOwnerComponent() != 0 &&
            !highlightedEndpoint_->getOwnerComponent()->isValid())))
        {
            event->setDropAction(Qt::CopyAction);
            highlightedEndpoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_HOVER);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }
    }
    else if (dragType_ == DRAG_TYPE_DESIGN)
    {
        event->setDropAction(Qt::CopyAction);
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::copySWInstances()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::copySWInstances(QList<QGraphicsItem*> const& items,
                                          ComponentCollectionCopyData &collection)
{
    // Create instance copies.
    foreach (QGraphicsItem* item, items)
    {
        if (item->type() == SWComponentItem::Type)
        {
            SWComponentItem* comp = static_cast<SWComponentItem*>(item);

            collection.instances.append(ComponentInstanceCopyData());
            ComponentInstanceCopyData& instance = collection.instances.back();

            // Take a copy of the component model so that we are not influenced by any changes to the
            // original model.
            instance.component = QSharedPointer<Component>(new Component(*comp->componentModel()));

            instance.instanceName = comp->name();
            instance.displayName = comp->displayName();
            instance.description = comp->description();
            instance.pos = comp->pos();
            instance.propertyValues = comp->getPropertyValues();
            instance.fileSetRef = comp->getFileSetRef();
            instance.apiInterfacePositions = comp->getApiInterfacePositions();
            instance.comInterfacePositions = comp->getComInterfacePositions();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteSWInstances()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::pasteSWInstances(ComponentCollectionCopyData const& collection, IGraphicsItemStack* stack,
                                           QUndoCommand* cmd, bool useCursorPos)
{
    foreach (ComponentInstanceCopyData const& instance, collection.instances)
    {
        // Create unique name for the component instance.
        QString instanceName = createInstanceName(instance.instanceName);

        // Take a copy of the component in case of a draft.
        QSharedPointer<Component> component = instance.component;

        if (!component->getVlnv()->isValid())
        {
            component = QSharedPointer<Component>(new Component(*instance.component));
        }

        SWComponentItem* comp = new SWComponentItem(getLibraryInterface(),
                                                    component,
                                                    instanceName,
                                                    instance.displayName,
                                                    instance.description);

        if (useCursorPos)
        {
            comp->setPos(parent()->mapFromGlobal(contextPos_));
        }
        else
        {
            comp->setPos(instance.pos);
        }

        comp->setApiInterfacePositions(instance.apiInterfacePositions, false);
        comp->setComInterfacePositions(instance.comInterfacePositions, false);

        IGraphicsItemStack* targetStack = stack;

        // Check if the stack does not accept the given component.
        if (!stack->isItemAllowed(comp))
        {
            targetStack = 0;

            // Find the first column that accepts the component.
            foreach (GraphicsColumn* otherColumn, layout_->getColumns())
            {
                if (otherColumn->isItemAllowed(comp))
                {
                    targetStack = otherColumn;
                    break;
                }
            }
        }

        if (targetStack != 0)
        {
            SystemItemAddCommand* childCmd = new SystemItemAddCommand(targetStack, comp, cmd);

            connect(childCmd, SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
            connect(childCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::onCopyAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onCopyAction()
{
    if (!isProtected())
    {
        QList<QGraphicsItem*> items = selectedItems();
        int type = getCommonItemType(items);

        if (type == SWPortItem::Type || type == SWInterfaceItem::Type)
        {
            PortCollectionCopyData collection;
            copyInterfaces(items, collection);
            
            QMimeData* mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == SWComponentItem::Type)
        {
            ComponentCollectionCopyData collection;
            copySWInstances(items, collection);

            QMimeData* mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == SystemColumn::Type)
        {
            qSort(items.begin(), items.end(), &sortByX);

            ColumnCollectionCopyData collection;

            foreach (QGraphicsItem* item, items)
            {
                SystemColumn* column = static_cast<SystemColumn*>(item);

                collection.columns.append(ColumnCopyData());
                ColumnCopyData& columnData = collection.columns.back();

                columnData.desc = column->getColumnDesc();

                copySWInstances(column->getItems(), columnData.components);
                copyInterfaces(column->getItems(), columnData.interfaces);
            }

            QMimeData* mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
    }

    prepareContextMenuActions();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::onPasteAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onPasteAction()
{
    if (!isProtected())
    {
        QList<QGraphicsItem*> items = selectedItems();
        int type = getCommonItemType(items);

        if (type == SWComponentItem::Type)
        {
            SWComponentItem* targetComp = static_cast<SWComponentItem*>(items[0]);

            // Paste only to draft components.
            if (!targetComp->componentModel()->getVlnv()->isValid())
            {
                QMimeData const* mimedata = QApplication::clipboard()->mimeData();

                if (mimedata->hasImage() && mimedata->imageData().canConvert<PortCollectionCopyData>())
                {				
                    PortCollectionCopyData collection = mimedata->imageData().value<PortCollectionCopyData>();

                    QSharedPointer<QUndoCommand> parentCmd(new QUndoCommand());
                    pasteInterfaces(collection, targetComp, parentCmd.data());
                    getEditProvider().addCommand(parentCmd);

                    // Update sidebar view.
                    emit componentSelected(targetComp);
                    emit helpUrlRequested("swsysdesign/swinstance.html");
                }
            }
        }
        else
        {
            // Check the mime data.
            QMimeData const* mimeData = QApplication::clipboard()->mimeData();

            if (mimeData->hasImage())
            {
                // Allow pasting interfaces to empty design space (columns).
                if (mimeData->imageData().canConvert<PortCollectionCopyData>() && items.empty())
                {
                    // Find a valid column for the interfaces.
                    IGraphicsItemStack* stack = layout_->findColumnAt(parent()->mapFromGlobal(contextPos_));

                    if (stack && stack->getContentType() != COLUMN_CONTENT_IO)
                    {
                        foreach (GraphicsColumn* col, layout_->getColumns())
                        {
                            if (col->getContentType() == COLUMN_CONTENT_IO)
                            {
                                stack = col;
                                break;
                            }
                        }
                    }

                    if (stack != 0)
                    {
                        PortCollectionCopyData collection = mimeData->imageData().value<PortCollectionCopyData>();

                        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                        pasteInterfaces(collection, stack, cmd.data(), true);
                        getEditProvider().addCommand(cmd);
                    }
                }
                // Allow pasting components to either empty design space (column) or to parent HW.
                else if (mimeData->imageData().canConvert<ComponentCollectionCopyData>() &&
                         (items.empty() || type == HWMappingItem::Type))
                {
                    IGraphicsItemStack* stack = 0;

                    if (type == HWMappingItem::Type)
                    {
                        stack = dynamic_cast<IGraphicsItemStack*>(items[0]);
                    }
                    else
                    {
                        stack = layout_->findColumnAt(parent()->mapFromGlobal(contextPos_));
                    }

                    if (stack != 0)
                    {
                        ComponentCollectionCopyData collection = mimeData->imageData().value<ComponentCollectionCopyData>();

                        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                        pasteSWInstances(collection, stack, cmd.data(), true);
                        getEditProvider().addCommand(cmd);
                        cmd->redo();
                    }
                }
                else if (mimeData->imageData().canConvert<ColumnCollectionCopyData>() &&
                         (items.empty() || type == SystemColumn::Type))
                {
                    ColumnCollectionCopyData collection = mimeData->imageData().value<ColumnCollectionCopyData>();
                    QSharedPointer<QUndoCommand> parentCmd(new QUndoCommand());

                    foreach (ColumnCopyData const& columnData, collection.columns)
                    {
                        SystemColumn* column = new SystemColumn(columnData.desc, layout_.data());

                        new GraphicsColumnAddCommand(layout_.data(), column, parentCmd.data());
                        pasteSWInstances(columnData.components, column, parentCmd.data(), false);
                        pasteInterfaces(columnData.interfaces, column, parentCmd.data(), false);
                    }

                    getEditProvider().addCommand(parentCmd);
                    parentCmd->redo();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::onAddAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onAddAction()
{
    if (selectedItems().size() == 1)
    {
        QGraphicsItem *item = selectedItems().first();
        if (item != 0 && item->type() == SWComponentItem::Type)
        {
            SystemComponentItem* comp = static_cast<SystemComponentItem*>(item);
            // Request the user to set the vlnv.
            NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, false, (QWidget*)parent());
            dialog.setVLNV(*comp->componentModel()->getVlnv());
            dialog.setWindowTitle(tr("Add Component to Library"));

            if (dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            VLNV vlnv = dialog.getVLNV();
            comp->componentModel()->setVlnv(vlnv);
            comp->componentModel()->setComponentHierarchy(dialog.getProductHierarchy());
            comp->componentModel()->setComponentFirmness(dialog.getFirmness());

            if (comp->type() == HWMappingItem::Type)
            {
                comp->componentModel()->createEmptyFlatView();
            }

            // Write the model to file.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Create an undo command.
            QSharedPointer<ComponentPacketizeCommand> cmd(new ComponentPacketizeCommand(comp, vlnv));
            getEditProvider().addCommand(cmd);
            cmd->redo();

            // Ask the user if he wants to complete the component.
            QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                "Do you want to continue packaging the component completely?",
                QMessageBox::NoButton, (QWidget*)parent());
            msgBox.setInformativeText("Pressing Continue opens up the component editor.");
            QPushButton* btnContinue = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
            msgBox.addButton(tr("Skip"), QMessageBox::RejectRole);

            msgBox.exec();

            if (msgBox.clickedButton() == btnContinue)
            {
                // Open up the component editor.
                emit openComponent(*comp->componentModel()->getVlnv());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::onOpenComponentAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onOpenComponentAction()
{
    if (selectedItems().size() == 1)
    {
        SystemComponentItem* component = dynamic_cast<SystemComponentItem*>(selectedItems().first());
        if (component)
        {
            emit openComponent(*component->componentModel()->getVlnv());
        }            
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::onOpenDesignAction()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::onOpenDesignAction()
{
    if (selectedItems().size() == 1)
    {
        SystemComponentItem* component = dynamic_cast<SystemComponentItem*>(selectedItems().first());
        if (component)
        {
            if (getLibraryInterface()->contains(*component->componentModel()->getVlnv()))
            {
                QString viewName;
                QStringList hierViews = component->componentModel()->getSWViewNames();

                // if configuration is used and it contains an active view for the instance
                if (getDesignConfiguration() && getDesignConfiguration()->hasActiveView(component->name())) {
                    viewName = getDesignConfiguration()->getActiveView(component->name());

                    QSharedPointer<SWView> view = component->componentModel()->findSWView(viewName);
                    // if view was found
                    if (view)
                    {
                        emit openSWDesign(*component->componentModel()->getVlnv(), viewName);
                    }
                }
                // Open the first design if there is one or multiple hierarchical view.
                else if (hierViews.size() != 0)
                {
                    emit openSWDesign(*component->componentModel()->getVlnv(), hierViews.first());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createContextMenu()
//-----------------------------------------------------------------------------
QMenu* SystemDesignDiagram::createContextMenu(QPointF const& pos)
{
    QMenu* menu = 0;

    if (getMode() == MODE_SELECT && showContextMenu_)
    {
        QGraphicsItem* item = itemAt(pos,QTransform());

        // Deselect the currently selected items if the user clicked above nothing.
        if (item == 0)
        {
            clearSelection();
        }
        // Or select new item if the clicked item does not belong into the current selection.
        else
        {
            // This fixes the problem when the user click above a text label or a pixmap but
            // actually wants to select the parent item (such as the actual component, not its label).
            if (item->parentItem() != 0 &&
                (item->type() == QGraphicsTextItem::Type || item->type() == QGraphicsPixmapItem::Type))
            {
                item = item->parentItem();
            }

            if (!selectedItems().contains(item))
            {
                clearSelection();
                item->setSelected(true);
            }
        }

        prepareContextMenuActions();

        menu = new QMenu(parent());
        menu->addAction(&selectAllAction_);
        menu->addSeparator();
        menu->addAction(&addAction_);
        menu->addAction(&openComponentAction_);
        menu->addAction(&openDesignAction_);
        menu->addSeparator();
        menu->addAction(&copyAction_);
        menu->addAction(&pasteAction_);
    }

    return menu;
}

//-----------------------------------------------------------------------------
// Function: SystemWDesignDiagram::setupActions()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::setupActions()
{
    parent()->addAction(&selectAllAction_);
    selectAllAction_.setShortcut(QKeySequence::SelectAll);
    connect(&selectAllAction_, SIGNAL(triggered()),this, SLOT(selectAll()), Qt::UniqueConnection);

    parent()->addAction(&copyAction_);
    copyAction_.setShortcut(QKeySequence::Copy);
    connect(&copyAction_, SIGNAL(triggered()),this, SLOT(onCopyAction()), Qt::UniqueConnection);

    parent()->addAction(&pasteAction_);
    pasteAction_.setShortcut(QKeySequence::Paste);
    connect(&pasteAction_, SIGNAL(triggered()),this, SLOT(onPasteAction()), Qt::UniqueConnection);	

    parent()->addAction(&addAction_);
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);

    parent()->addAction(&openComponentAction_);
    connect(&openComponentAction_, SIGNAL(triggered()), this, SLOT(onOpenComponentAction()), Qt::UniqueConnection);

    parent()->addAction(&openDesignAction_);
    connect(&openDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenDesignAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::copyInterfaces()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::copyInterfaces(QList<QGraphicsItem*> const& items, PortCollectionCopyData& collection)
{
    foreach (QGraphicsItem* item, items)
    {
        if (item->type() == SWPortItem::Type || item->type() == SWInterfaceItem::Type)
        {
            SWConnectionEndpoint* endpoint = static_cast<SWConnectionEndpoint*>(item);

            collection.ports.append(PortCopyData());
            PortCopyData& portData = collection.ports.back();
            portData.name = endpoint->name();
            portData.pos = endpoint->pos();

            if (endpoint->getApiInterface() != 0)
            {
                portData.apiInterface = QSharedPointer<ApiInterface>(new ApiInterface(*endpoint->getApiInterface()));
            }

            if (endpoint->getComInterface() != 0)
            {
                portData.comInterface = QSharedPointer<ComInterface>(new ComInterface(*endpoint->getComInterface()));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteInterfaces()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::pasteInterfaces(PortCollectionCopyData const& collection,
                                          SWComponentItem* targetComp, QUndoCommand* cmd)
{
    foreach (PortCopyData const& portData, collection.ports)
    {
        // Interface must have a unique name within the component.
        QString uniqueName = portData.name;	

        unsigned int count = 0;

        while(targetComp->getSWPort(uniqueName, SWConnectionEndpoint::ENDPOINT_TYPE_COM) != 0 ||
              targetComp->getSWPort(uniqueName, SWConnectionEndpoint::ENDPOINT_TYPE_API) != 0 )
        {
            ++count;
            uniqueName = portData.name + "_" + QString::number(count);			
        }

        SWPortItem* port = 0;

        if (portData.apiInterface != 0)
        {
            // Create a copy of the API interface.
            QSharedPointer<ApiInterface> apiIf(new ApiInterface(*portData.apiInterface));
            apiIf->setName(uniqueName);

            port = new SWPortItem(apiIf, targetComp);
        }
        else if (portData.comInterface != 0)
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(*portData.comInterface));
            comIf->setName(uniqueName);

            port = new SWPortItem(comIf, targetComp);
        }
        else
        {
            port = new SWPortItem(uniqueName, targetComp);
        }

        QPointF pos = snapPointToGrid(targetComp->mapFromScene(parent()->mapFromGlobal(contextPos_)));
        port->setPos(pos);

        // Save the positions of the other interfaces.
        QMap<SWPortItem*, QPointF> oldPositions;

        foreach (QGraphicsItem* item, targetComp->childItems())
        {
            if (item->type() == SWPortItem::Type)
            {
                SWPortItem* port = static_cast<SWPortItem*>(item);
                oldPositions.insert(port, port->pos());
            }
        }

        // Run the actual command for pasting the interface.
        QUndoCommand* pasteCmd = new SWPortAddCommand(targetComp, port, cmd);
        pasteCmd->redo();

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<SWPortItem*, QPointF>::iterator cur = oldPositions.begin();

        while (cur != oldPositions.end())
        {
            if (cur.key()->pos() != cur.value())
            {
                new SWPortMoveCommand(cur.key(), cur.value(), pasteCmd);
            }

            ++cur;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteInterfaces()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::pasteInterfaces(PortCollectionCopyData const& collection, IGraphicsItemStack* stack,
                                          QUndoCommand* cmd, bool useCursorPos)
{
    foreach (PortCopyData const& portData, collection.ports)
    {
        QList<QString> existingNames;

        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == SWInterfaceItem::Type)
            {
                SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
                existingNames.append(interface->name());
            }
        }
        
        // Interface must have a unique name within the component.
        QString uniqueName = portData.name;

        unsigned int count = 0;

        while (existingNames.contains(uniqueName))
        {
            ++count;
            uniqueName = portData.name + "_" + QString::number(count);
        }

        SWInterfaceItem* interface = 0;

        if (portData.apiInterface != 0)
        {
            // Create a copy of the API interface.
            QSharedPointer<ApiInterface> apiIf(new ApiInterface(*portData.apiInterface));
            apiIf->setName(uniqueName);

            interface = new SWInterfaceItem(getEditedComponent(), apiIf);
        }
        else if (portData.comInterface != 0)
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(*portData.comInterface));
            comIf->setName(uniqueName);

            interface = new SWInterfaceItem(getEditedComponent(), comIf);
        }
        else
        {
            interface = new SWInterfaceItem(getEditedComponent(), uniqueName, 0);
        }

        if (useCursorPos)
        {
            interface->setPos(parent()->mapFromGlobal(contextPos_));
        }
        else
        {
            interface->setPos(portData.pos);
        }

        // Save the positions of the other interfaces.
        QMap<SWInterfaceItem*, QPointF> oldPositions;

        foreach (QGraphicsItem* item, dynamic_cast<SystemColumn*>(stack)->childItems())
        {
            if (item->type() == SWInterfaceItem::Type)
            {
                SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
                oldPositions.insert(interface, interface->scenePos());
            }
        }

        QUndoCommand* pasteCmd = new SWInterfacePasteCommand(stack, interface, cmd);
        pasteCmd->redo();

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<SWInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

        while (cur != oldPositions.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), stack, pasteCmd);
            }

            ++cur;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::prepareContextMenuActions()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::prepareContextMenuActions()
{
    QList<QGraphicsItem*> items = selectedItems();

    // Disable all actions as default.
    addAction_.setEnabled(false);
    openComponentAction_.setEnabled(false);
    openDesignAction_.setEnabled(false);
    copyAction_.setEnabled(false);
    pasteAction_.setEnabled(false);

    if (items.empty())
    {
        // If the selection is empty, check if the clipboard contains components or a column.
        QMimeData const* mimeData = QApplication::clipboard()->mimeData();

        if (!isProtected() && mimeData != 0 && mimeData->hasImage() &&
            (mimeData->imageData().canConvert<ComponentCollectionCopyData>() ||
            mimeData->imageData().canConvert<ColumnCollectionCopyData>() ||
            mimeData->imageData().canConvert<PortCollectionCopyData>()))
        {
            pasteAction_.setEnabled(true);
        }
        else
        {
            pasteAction_.setEnabled(false);
        }
    }
    else
    {
        int type = getCommonItemType(items);

        if (type == SWPortItem::Type || type == SWInterfaceItem::Type)
        {
            // Allow copying API/COM ports (single or multiple).
            copyAction_.setEnabled(!isProtected());
        }
        else if (type == SWComponentItem::Type)
        {
            // Allow SW design opening, if component has at least one hierarchical view.
            SWComponentItem* swItem = qgraphicsitem_cast<SWComponentItem *>(items.back());
            Q_ASSERT(swItem);
            QStringList hierViews = swItem->componentModel()->getSWViewNames();          

             openDesignAction_.setEnabled(hierViews.size() != 0);

            // Allow copying components (single or multiple).
            copyAction_.setEnabled(!isProtected());
            
            // Enable paste action, if a draft component (no valid vlnv) and the clipboard
            // contains API/COM bus interfaces.
            bool draft = !qgraphicsitem_cast<SWComponentItem *>(items.back())->componentModel()->getVlnv()->isValid();
            addAction_.setEnabled(!isProtected() && draft && selectedItems().size() == 1);
            openComponentAction_.setEnabled(!draft && items.count() == 1);
            
            QMimeData const* mimedata = QApplication::clipboard()->mimeData();

            pasteAction_.setEnabled(!isProtected() && draft && mimedata != 0 && mimedata->hasImage() && 
                                    mimedata->imageData().canConvert<PortCollectionCopyData>());
        }
        else if (type == SystemColumn::Type)
        {
            copyAction_.setEnabled(!isProtected());

            QMimeData const* mimedata = QApplication::clipboard()->mimeData();
            pasteAction_.setEnabled(!isProtected() && mimedata != 0 && mimedata->hasImage() &&
                                    mimedata->imageData().canConvert<ColumnCollectionCopyData>());
        }
        else if (type == HWMappingItem::Type)
        {
            openComponentAction_.setEnabled(true);       
            
            QMimeData const* mimeData = QApplication::clipboard()->mimeData();
            pasteAction_.setEnabled(!isProtected() && mimeData != 0 && mimeData->hasImage() &&
                                    mimeData->imageData().canConvert<ComponentCollectionCopyData>());
        }        
    }
}
