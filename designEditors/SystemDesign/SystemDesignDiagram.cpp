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

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>
#include <QUuid>

#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <designEditors/SystemDesign/SystemDetailsEditor/SwitchHWDialog.h>
#include <designEditors/HWDesign/columnview/ColumnEditDialog.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/SWInstance.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/ApiInterface.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/SystemView.h>

Q_DECLARE_METATYPE(SystemDesignDiagram::PortCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ComponentCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ColumnCollectionCopyData)

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::SystemDesignDiagram(bool onlySW, LibraryInterface* lh,
                                         GenericEditProvider& editProvider,
                                         SystemDesignWidget* parent)
    : ComponentDesignDiagram(lh, editProvider, parent),
      onlySW_(onlySW),
      dragType_(DRAG_TYPE_NONE),
      dragEndPoint_(0)
{

}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::~SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::~SystemDesignDiagram()
{

}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createDesign()
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

            if (swCompItem->isDraft())
            {
                instance.setDraft();
            }

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

    foreach (GraphicsColumn* column, getLayout()->getColumns())
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
// Function: SystemDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::updateHierComponent()
{

}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::addColumn()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addColumn(ColumnDesc const& desc)
{
    SystemColumn* column = new SystemColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(getLayout().data(), column));
    getEditProvider().addCommand(cmd);
    cmd->redo();
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
                    IGraphicsItemStack* stack = getLayout()->findColumnAt(contextMenuPosition());

                    if (stack && stack->getContentType() != COLUMN_CONTENT_IO)
                    {
                        foreach (GraphicsColumn* col, getLayout()->getColumns())
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
                        stack = getLayout()->findColumnAt(contextMenuPosition());
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
                        SystemColumn* column = new SystemColumn(columnData.desc, getLayout().data());

                        new GraphicsColumnAddCommand(getLayout().data(), column, parentCmd.data());
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
void SystemDesignDiagram::onAddToLibraryAction()
{
    if (singleSelection())
    {
        QGraphicsItem *item = selectedItems().first();
        if (item != 0 && item->type() == SWComponentItem::Type)
        {
            SystemComponentItem* comp = static_cast<SystemComponentItem*>(item);
            // Request the user to set the vlnv.
            NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, false, getParent());
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
                QMessageBox::NoButton, getParent());
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
// Function: SystemDesignDiagram::openDesignForComponent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::openDesignForComponent(ComponentItem* component, QString const& viewName)
{
    if (component->componentModel()->hasSWView(viewName)) {
        emit openSWDesign(*component->componentModel()->getVlnv(), viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::mouseDoubleClickEvent()
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

    item = getBaseItemOf(item);

    if (dynamic_cast<SystemComponentItem*>(item) != 0)
    {
        item->setSelected(true);
        SystemComponentItem* comp = static_cast<SystemComponentItem*>(item);

        if (getLibraryInterface()->contains(*comp->componentModel()->getVlnv()))
        {
            openComponentItem(comp);
        }
        else if (!isProtected())
        {
            // Otherwise this is an unpackaged component.
            onAddToLibraryAction();
        }
    }
    else if (item->type() == SystemColumn::Type)
    {
        if (!isProtected())
        {
            item->setSelected(true);
            SystemColumn* column = qgraphicsitem_cast<SystemColumn*>(item);

            ColumnEditDialog dialog(getParent(), onlySW_, column);

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
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragType_ = DRAG_TYPE_NONE;

    if (dragEndPoint_)
    {
        dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        dragEndPoint_ = 0;
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
                QMessageBox::Ok, getParent());
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
                stack = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));
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

            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component instance '%1' is about to be replaced "
                "with an instance of %2. Continue and replace?").arg(oldCompItem->name(), droppedVLNV.toString()),
                QMessageBox::Yes | QMessageBox::No, getParent());

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
                               QMessageBox::Ok, getParent());
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
        SwitchHWDialog dialog(newComponent, viewName, getLibraryInterface(), getParent());
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
            newView->setHierarchyRef(getEditedComponent()->findSystemView(getParent()->getOpenViewName())->getHierarchyRef());

            getEditedComponent()->removeSystemView(getParent()->getOpenViewName());
            getLibraryInterface()->writeModelToFile(getEditedComponent());
        }

        // Add the system view to the new HW component and save.
        newComponent->addSystemView(newView);
        getLibraryInterface()->writeModelToFile(newComponent);

        // Refresh the design widget.
        getParent()->setDesign(droppedVLNV, dialog.getSystemViewName());
        getParent()->setProtection(false);
        getParent()->refresh();
    }
    else if (dragType_ == DRAG_TYPE_DEFINITION)
    {
        if (dragEndPoint_ != 0)
        {
            Q_ASSERT(getLibraryInterface()->contains(droppedVLNV));

            VLNV vlnv = droppedVLNV;
            vlnv.setType(getLibraryInterface()->getDocumentType(droppedVLNV));

            // Save old type and set the new one.
            VLNV oldType = dragEndPoint_->getTypeDefinition();
            dragEndPoint_->setTypeDefinition(vlnv);

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new TypeDefinitionChangeCommand(dragEndPoint_, oldType));
            getEditProvider().addCommand(cmd);

            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
            dragEndPoint_ = 0;
        }
    }
    else if (dragType_ == DRAG_TYPE_DESIGN)
    {
        // Retrieve the design.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Design> design = libComp.dynamicCast<Design>();

        // Import elements to the column under cursor.
        IGraphicsItemStack* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));
        importDesign(design, column, event->scenePos());
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
            GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));

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
        if (dragEndPoint_)
        {
            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
            dragEndPoint_ = 0;
        }

        // Check if there is an endpoint close enough the cursor.
        SWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<SWConnectionEndpoint>(event->scenePos(), this, GridSize);

        if (endpoint != 0 && !endpoint->isInvalid())
        {
            dragEndPoint_ = endpoint;
        }

        // Allow the drop event if the end point is undefined or there are no connections
        // and the encompassing component is unpackaged.
        if (dragEndPoint_ != 0 &&
            (dragEndPoint_->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED ||
            (!dragEndPoint_->isConnected() && dragEndPoint_->getOwnerComponent() != 0 &&
            !dragEndPoint_->getOwnerComponent()->isValid())))
        {
            event->setDropAction(Qt::CopyAction);
            dragEndPoint_->setHighlight(SWConnectionEndpoint::HIGHLIGHT_HOVER);
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
// Function: SystemDesignDiagram::openComponentActionEnabled()
//-----------------------------------------------------------------------------
bool SystemDesignDiagram::openComponentActionEnabled() const
{
    if (!singleSelection())
    {
        return false;
    }

    bool openEnabled = false;

    QGraphicsItem* selectedItem = selectedItems().first();
    if (selectedItem->type() == SWComponentItem::Type || selectedItem->type() == HWMappingItem::Type)
    {
        openEnabled = dynamic_cast<ComponentItem*>(selectedItem)->componentModel()->getVlnv()->isValid();
    }

    return openEnabled;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::isHierarchicalComponent()
//-----------------------------------------------------------------------------
bool SystemDesignDiagram::isHierarchicalComponent(QGraphicsItem* item) const
{
    SWComponentItem* component = qgraphicsitem_cast<SWComponentItem*>(item);
    if (component)
    {
        return component->componentModel()->isHierarchicalSW();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::copyActionEnabled()
//-----------------------------------------------------------------------------
bool SystemDesignDiagram::copyActionEnabled() const
{
    QList<QGraphicsItem*> items = selectedItems();    
    if (items.empty())
    {
        return false;
    }

    int itemType = getCommonItemType(items);
    return !isProtected() && (itemType == SWPortItem::Type || itemType == SWInterfaceItem::Type || 
        itemType == SWComponentItem::Type || itemType == SystemColumn::Type);     
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteActionEnabled()
//-----------------------------------------------------------------------------
bool SystemDesignDiagram::pasteActionEnabled() const
{
    QMimeData const* mimeData = QApplication::clipboard()->mimeData();
    
    if (isProtected() || !mimeData || !mimeData->hasImage())
    {
        return false;
    }

    bool pasteEnabled = false;

    QList<QGraphicsItem*> items = selectedItems();    
    if (items.empty())
    {
        pasteEnabled = mimeData->imageData().canConvert<ComponentCollectionCopyData>() ||
            mimeData->imageData().canConvert<ColumnCollectionCopyData>() ||
            (onlySW_ && mimeData->imageData().canConvert<PortCollectionCopyData>());
    }
    else
    {
        int type = getCommonItemType(items);

        if (type == SWComponentItem::Type)
        {
            pasteEnabled = singleSelection() && draftSelected() && 
                mimeData->imageData().canConvert<PortCollectionCopyData>();
        }
        else if (type == SystemColumn::Type)
        {
            pasteEnabled = mimeData->imageData().canConvert<ColumnCollectionCopyData>();
        }
        else if (type == HWMappingItem::Type)
        {
            pasteEnabled = singleSelection() && mimeData->imageData().canConvert<ComponentCollectionCopyData>();
        }
    }

    return pasteEnabled;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::openComponentByActiveView()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::openComponentByActiveView(ComponentItem* comp)
{
    QString viewName = getActiveViewOf(comp);

    // if view was found
    if (comp->componentModel()->hasSWView(viewName) && hierarchicalViewsOf(comp).contains(viewName))
    {
        openDesignForComponent(comp, viewName);
    }
    else
    {
        openInComponentEditor(comp);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::hierarchicalViewsOf()
//-----------------------------------------------------------------------------
QStringList SystemDesignDiagram::hierarchicalViewsOf(ComponentItem* component) const
{
    return component->componentModel()->getSWViewNames();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::componentType()
//-----------------------------------------------------------------------------
int SystemDesignDiagram::componentType() const
{
    return SWComponentItem::Type;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::offpageConnectorType()
//-----------------------------------------------------------------------------
int SystemDesignDiagram::offpageConnectorType() const
{
    return SWOffPageConnectorItem::Type;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::connectionType()
//-----------------------------------------------------------------------------
int SystemDesignDiagram::connectionType() const
{
    return GraphicsConnection::Type;
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
// Function: SystemDesignDiagram::loadDesign()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
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
        SystemColumn* column = new SystemColumn(desc, getLayout().data());
        getLayout()->addColumn(column, true);
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
            getLayout()->getColumns().at(colIndex)->addItem(item);
            colIndex = (colIndex + 1) % getLayout()->getColumns().size();
        }
        else
        {
            item->setPos(instance.getPosition());

            GraphicsColumn* column = getLayout()->findColumnAt(instance.getPosition());

            if (column != 0)
            {
                column->addItem(item, true);
            }
            else
            {
                getLayout()->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % getLayout()->getColumns().size();
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

        if (instance.isDraft())
        {
            item->setDraft();
        }

        if (instance.getMapping().isEmpty())
        {
            // Check if the position is not found.
            if (instance.getPosition().isNull())
            {
                getLayout()->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % getLayout()->getColumns().size();
            }
            else
            {
                GraphicsColumn* column = getLayout()->findColumnAt(instance.getPosition());

                if (column != 0)
                {
                    column->addItem(item, true);
                }
                else
                {
                    getLayout()->getColumns().at(colIndex)->addItem(item);
                    colIndex = (colIndex + 1) % getLayout()->getColumns().size();
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
                getLayout()->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % getLayout()->getColumns().size();
            }
        }

        onComponentInstanceAdded(item);
    }

    // Create SW interface items for the top-level API and COM interfaces.
    foreach (QSharedPointer<ApiInterface> apiIf, getEditedComponent()->getApiInterfaces())
    {
        SWInterfaceItem* item = new SWInterfaceItem(getEditedComponent(), apiIf);

        // Add the interface to the first column where it is allowed to be placed.
        getLayout()->addItem(item);
    }

    foreach (QSharedPointer<ComInterface> comIf, getEditedComponent()->getComInterfaces())
    {
        SWInterfaceItem* item = new SWInterfaceItem(getEditedComponent(), comIf);

        // Add the interface to the first column where it is allowed to be placed.
        getLayout()->addItem(item);
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
    getLayout()->updatePositions();
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

            GraphicsColumn* column = getLayout()->findColumnAt(hierConn.getPosition());

            if (column != 0)
            {
                column->addItem(interface);
            }
            else
            {
                getLayout()->addItem(interface);
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

            GraphicsColumn* column = getLayout()->findColumnAt(dependency.getPosition());

            if (column != 0)
            {
                column->addItem(interface);
            }
            else
            {
                getLayout()->addItem(interface);
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
// Function: SystemDesignDiagram::getComponent()
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
// Function: SystemDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* SystemDesignDiagram::createConnection(ConnectionEndpoint* startPoint, 
    ConnectionEndpoint* endPoint)
{
    return new GraphicsConnection(startPoint, endPoint, false, QString(), QString(), QString(), this);	
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* SystemDesignDiagram::createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint)
{
    return new GraphicsConnection(startPoint->scenePos(), startPoint->getDirection(), endPoint,
        QVector2D(0.0f, 0.0f), QString(), QString(), this);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createAddCommandForConnection()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> SystemDesignDiagram::createAddCommandForConnection(GraphicsConnection* connection)
{
    return QSharedPointer<QUndoCommand>(new SWConnectionAddCommand(this, connection));
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::addTopLevelInterface()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addTopLevelInterface(GraphicsColumn* column, QPointF const& pos)
{
    SWInterfaceItem* newItem = new SWInterfaceItem(getEditedComponent(), "", 0);
    newItem->setPos(snapPointToGrid(pos));

    // Save the positions of the other interfaces.
    QMap<SWInterfaceItem*, QPointF> oldPositions;

    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
            oldPositions.insert(interface, interface->scenePos());
        }
    }

    QSharedPointer<QUndoCommand> cmd(new SystemItemAddCommand(column, newItem));
    cmd->redo();

    // Determine if the other interfaces changed their position and create undo commands for them.
    QMap<SWInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

    while (cur != oldPositions.end())
    {
        if (cur.key()->scenePos() != cur.value())
        {
            new ItemMoveCommand(cur.key(), cur.value(), column, cmd.data());
        }

        ++cur;
    }

    getEditProvider().addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::draftAt()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::draftAt(QPointF const& clickedPosition)
{
    // Find the bottom-most item under the cursor.
    QGraphicsItem* item = 0;
    QList<QGraphicsItem*> itemList = items(clickedPosition);

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

            QSharedPointer<QUndoCommand> cmd(new SWPortAddCommand(comp, snapPointToGrid(clickedPosition)));
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
            stack = getLayout()->findColumnAt(clickedPosition);
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
                swCompItem->setDraft();
                swCompItem->setPos(snapPointToGrid(clickedPosition));

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
                GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(stack);
                if (column)
                {
                    addTopLevelInterface(column, clickedPosition);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::replace()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::replace(ComponentItem* destComp, ComponentItem* sourceComp)
{
    SystemComponentItem* destSystemComponent = dynamic_cast<SystemComponentItem*>(destComp);
    SystemComponentItem* sourceSystemComponent = dynamic_cast<SystemComponentItem*>(sourceComp);

    if (destSystemComponent && sourceSystemComponent)
    {
        // Perform the replacement. 
        QSharedPointer<ReplaceSystemComponentCommand>
            cmd(new ReplaceSystemComponentCommand(destSystemComponent, sourceSystemComponent, true, true));

        connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider().addCommand(cmd);
        cmd->redo();
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
            instance.isDraft = comp->isDraft();
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
            comp->setPos(contextMenuPosition());
        }
        else
        {
            comp->setPos(instance.pos);
        }

        comp->setApiInterfacePositions(instance.apiInterfacePositions, false);
        comp->setComInterfacePositions(instance.comInterfacePositions, false);

        if (instance.isDraft)
        {
            comp->setDraft();
        }

        IGraphicsItemStack* targetStack = stack;

        // Check if the stack does not accept the given component.
        if (!stack->isItemAllowed(comp))
        {
            targetStack = 0;

            // Find the first column that accepts the component.
            foreach (GraphicsColumn* otherColumn, getLayout()->getColumns())
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

        QPointF pos = snapPointToGrid(targetComp->mapFromScene(contextMenuPosition()));
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
            interface->setPos(contextMenuPosition());
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
