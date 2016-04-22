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

#include "SWOffPageConnectorItem.h"
#include "SystemColumn.h"
#include "HWMappingItem.h"
#include "SWComponentItem.h"
#include "SystemDesignWidget.h"
#include "SWPortItem.h"
#include "SWInterfaceItem.h"

#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <designEditors/common/DiagramUtil.h>
#include <designEditors/HWDesign/columnview/ColumnEditDialog.h>
#include <designEditors/HWDesign/HWChangeCommands.h>
#include <designEditors/SystemDesign/UndoCommands/SystemMoveCommands.h>
#include <designEditors/SystemDesign/UndoCommands/SystemAddCommands.h>
#include <designEditors/SystemDesign/UndoCommands/SystemComponentAddCommand.h>
#include <designEditors/SystemDesign/SystemDetailsEditor/SwitchHWDialog.h>
#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/UndoCommands/ComConnectionAddCommand.h>
#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/UndoCommands/ApiConnectionAddCommand.h>
#include <designEditors/SystemDesign/UndoCommands/SWInterfaceAddCommand.h>
#include <designEditors/SystemDesign/UndoCommands/SystemChangeCommands.h>
#include <designEditors/SystemDesign/UndoCommands/ReplaceSystemComponentCommand.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/kactusExtensions/SWInstance.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>

#include <IPXACTmodels/Design/Design.h>

#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>
#include <QUuid>

Q_DECLARE_METATYPE(SystemDesignDiagram::PortCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ComponentCollectionCopyData)
Q_DECLARE_METATYPE(SystemDesignDiagram::ColumnCollectionCopyData)

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::SystemDesignDiagram()
//-----------------------------------------------------------------------------
SystemDesignDiagram::SystemDesignDiagram(bool onlySW, LibraryInterface* lh,
                                         QSharedPointer<IEditProvider> editProvider, SystemDesignWidget* parent):
ComponentDesignDiagram(lh, editProvider, parent),
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
/*QSharedPointer<Design> SystemDesignDiagram::createDesign(VLNV const& vlnv) const
{
    QSharedPointer<Design> design = DesignDiagram::createDesign(vlnv);
    
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances(new QList<QSharedPointer<ComponentInstance> >());
    QList<QSharedPointer<SWInstance> > swInstances;
    QList<QSharedPointer<ApiInterconnection> > apiDependencies;
    QList<QSharedPointer<HierApiInterconnection> > hierApiDependencies;
    QList<QSharedPointer<ComInterconnection> > comConnections;
    QList<QSharedPointer<HierComInterconnection> > hierComConnections;
    QList<QSharedPointer<ColumnDesc> > columns;

    foreach (QGraphicsItem const* item, items())
    {
        if (item->type() == HWMappingItem::Type)
        {
            HWMappingItem const* mappingItem = static_cast<HWMappingItem const*>(item);

            QSharedPointer<ConfigurableVLNVReference> vlnvReference 
                (new ConfigurableVLNVReference(mappingItem->componentModel()->getVlnv()));

            QSharedPointer<ComponentInstance> instance (new ComponentInstance(mappingItem->name(),
                mappingItem->displayName(), mappingItem->description(), vlnvReference, mappingItem->scenePos(),
                mappingItem->getUuid()));

            QMap<QString, QString> mappedElements = mappingItem->getConfigurableElements();
            QMapIterator<QString, QString> iterator(mappedElements);
            while (iterator.hasNext())
            {
                iterator.next();
                QSharedPointer<ConfigurableElementValue> element (new ConfigurableElementValue());
                element->setReferenceId(iterator.key());
                element->setConfigurableValue(iterator.value());

                instance->getConfigurableElementValues()->append(element);
            }

            instance->setImported(mappingItem->isImported());
            instance->setImportRef(mappingItem->getImportRef());
            instance->setPropertyValues(mappingItem->getPropertyValues());

            // Save API and COM interface positions.
            QListIterator<QSharedPointer<ApiInterface> >
                itrApiIf(mappingItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                QSharedPointer<ApiInterface> apiIf = itrApiIf.next();
                instance->updateApiInterfacePosition(apiIf->name(),
                    mappingItem->getSWPort(apiIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos());
            }

            QListIterator<QSharedPointer<ComInterface> >
                itrComIf(mappingItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                QSharedPointer<ComInterface> comIf = itrComIf.next();
                instance->updateComInterfacePosition(comIf->name(),
                    mappingItem->getSWPort(comIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
            }

            instances->append(instance);
        }
        else if (item->type() == SWComponentItem::Type)
        {
            SWComponentItem const* swCompItem = static_cast<SWComponentItem const*>(item);

            QSharedPointer<SWInstance> instance(new SWInstance());
            instance->setInstanceName(swCompItem->name());
            instance->setDisplayName(swCompItem->displayName());
            instance->setDescription(swCompItem->description());

            QSharedPointer<ConfigurableVLNVReference> vlnvReference
                (new ConfigurableVLNVReference(swCompItem->componentModel()->getVlnv()));

            instance->setComponentRef(vlnvReference);
            instance->setPropertyValues(swCompItem->getPropertyValues());
            instance->setFileSetRef(swCompItem->getFileSetRef());
            instance->setImported(swCompItem->isImported());
            instance->setImportRef(swCompItem->getImportRef());

            if (swCompItem->isDraft())
            {
                instance->setDraft(true);
            }

            if (swCompItem->parentItem()->type() == HWMappingItem::Type)
            {
                HWMappingItem const* parent = static_cast<HWMappingItem*>(swCompItem->parentItem());
                instance->setMapping(parent->getUuid());
                instance->setPosition(swCompItem->pos());
            }
            else
            {
                instance->setPosition(swCompItem->scenePos());
            }

            // Save API and COM interface positions.
            QListIterator<QSharedPointer<ApiInterface> >
                itrApiIf(swCompItem->componentModel()->getApiInterfaces());

            while (itrApiIf.hasNext())
            {
                QSharedPointer<ApiInterface> apiIf = itrApiIf.next();
                instance->updateApiInterfacePosition(apiIf->name(),
                    swCompItem->getSWPort(apiIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos());
            }

            QListIterator<QSharedPointer<ComInterface> >
                itrComIf(swCompItem->componentModel()->getComInterfaces());

            while (itrComIf.hasNext())
            {
                QSharedPointer<ComInterface> comIf = itrComIf.next();
                instance->updateComInterfacePosition(comIf->name(),
                    swCompItem->getSWPort(comIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos());
            }

            // Hack: Save undefined interfaces as COM interfaces.
            foreach (QGraphicsItem* childItem, item->childItems())
            {
                SWPortItem* portItem = dynamic_cast<SWPortItem*>(childItem);

                if (portItem != 0 && portItem->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
                {
                    instance->updateComInterfacePosition(portItem->name(),
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
                    QSharedPointer<ActiveInterface> providerRef(new ActiveInterface(endpoint1->encompassingComp()->name(), endpoint1->name()));
                    QSharedPointer<ActiveInterface> requesterRef(new ActiveInterface(endpoint2->encompassingComp()->name(), endpoint2->name()));

                    QSharedPointer<ApiInterconnection> dependency(new ApiInterconnection(
                        conn->name(), QString(), conn->description(),
                        providerRef, requesterRef, conn->route()));
                    dependency->setImported(conn->isImported());
                    dependency->setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

                    apiDependencies.append(dependency);
                }
                else
                {
                   //TODO: ConnectionEndpoint* compPort = endpoint1;
                    ConnectionEndpoint* compPort = endpoint1;
                    ConnectionEndpoint* hierPort = endpoint2;

                    if (compPort->encompassingComp() == 0)
                    {
                        std::swap(compPort, hierPort);
                    }

                    QSharedPointer<ActiveInterface> ref
                        (new ActiveInterface(compPort->encompassingComp()->name(), compPort->name()));

                    QSharedPointer<HierApiInterconnection> hierDependency(new HierApiInterconnection(
                        conn->name(), QString(), conn->description(), hierPort->name(), ref, hierPort->scenePos(),
                        hierPort->getDirection(), conn->route()));
                    hierDependency->setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);


                    hierApiDependencies.append(hierDependency);
                }
            }
            else// if (conn->getConnectionType() == SWConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
               
                if (endpoint1->encompassingComp() != 0 && endpoint2->encompassingComp() != 0)
                {
                    QSharedPointer<ActiveInterface> ref1(
                        new ActiveInterface(endpoint1->encompassingComp()->name(), endpoint1->name()));
                    QSharedPointer<ActiveInterface> ref2(
                        new ActiveInterface(endpoint2->encompassingComp()->name(), endpoint2->name()));

                    QSharedPointer<ComInterconnection> comConnection(
                        new ComInterconnection(conn->name(), QString(), conn->description(), ref1, ref2, conn->route()));
                    comConnection->setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

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
                        QSharedPointer<ActiveInterface> ref(
                            new ActiveInterface(compPort->encompassingComp()->name(), compPort->name()));

                        QSharedPointer<HierComInterconnection> hierComConnection(
                            new HierComInterconnection(conn->name(), QString(), conn->description(),
                            hierPort->name(), ref, hierPort->scenePos(), hierPort->getDirection(), conn->route()));

                        hierComConnection->setOffPage(conn->endpoint1()->type() == SWOffPageConnectorItem::Type);

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
    design->setApiConnections(apiDependencies);
    design->setHierApiDependencies(hierApiDependencies);
    design->setComConnections(comConnections);
    design->setHierComConnections(hierComConnections);
    design->setColumns(columns);

    return design;
}*/

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::updateHierComponent()
{

}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::addColumn()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addColumn(QSharedPointer<ColumnDesc> desc)
{
    SystemColumn* column = new SystemColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(getLayout().data(), column, getDesign()));
    getEditProvider()->addCommand(cmd);
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
            if (!targetComp->componentModel()->getVlnv().isValid())
            {
                QMimeData const* mimedata = QApplication::clipboard()->mimeData();

                if (mimedata->hasImage() && mimedata->imageData().canConvert<PortCollectionCopyData>())
                {				
                    PortCollectionCopyData collection = mimedata->imageData().value<PortCollectionCopyData>();

                    QSharedPointer<QUndoCommand> parentCmd(new QUndoCommand());
                    pasteInterfaces(collection, targetComp, parentCmd.data());
                    getEditProvider()->addCommand(parentCmd);

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

                    if (stack && stack->getContentType() != ColumnTypes::IO)
                    {
                        foreach (GraphicsColumn* col, getLayout()->getColumns())
                        {
                            if (col->getContentType() == ColumnTypes::IO)
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
                        getEditProvider()->addCommand(cmd);
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
                        stack = getLayout()->findColumnAt(findCursorPositionMappedToScene());
                    }

                    if (stack != 0)
                    {
                        ComponentCollectionCopyData collection = mimeData->imageData().value<ComponentCollectionCopyData>();

                        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                        pasteSWInstances(collection, stack, cmd.data(), true);
                        getEditProvider()->addCommand(cmd);
                        cmd->redo();
                    }
                }
                else if (mimeData->imageData().canConvert<ColumnCollectionCopyData>() &&
                    (items.empty() || type == SystemColumn::Type))
                {
                    ColumnCollectionCopyData collection = mimeData->imageData().value<ColumnCollectionCopyData>();

                    pasteColumns(collection);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteColumns()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::pasteColumns(ColumnCollectionCopyData const collection)
{
    QSharedPointer<QUndoCommand> parentCmd(new QUndoCommand());

    foreach (ColumnCopyData const& columnData, collection.columns)
    {
        QSharedPointer<ColumnDesc> copiedColumnDescription (new ColumnDesc(*columnData.desc.data()));
        QString newName = createColumnName(columnData.desc->name());
        copiedColumnDescription->setName(newName);

        SystemColumn* column = new SystemColumn(copiedColumnDescription, getLayout().data());

        GraphicsColumnAddCommand* addCommand =
            new GraphicsColumnAddCommand(getLayout().data(), column, getDesign(), parentCmd.data());
        pasteSWInstances(columnData.components, column, parentCmd.data(), false);
        pasteInterfaces(columnData.interfaces, column, parentCmd.data(), false);

        QPointF columnOldPosition (copiedColumnDescription->getPosition(), 0);
        column->setPos(findCursorPositionMappedToScene());

        new GraphicsColumnMoveCommand(getLayout().data(), column, columnOldPosition, addCommand);
    }

    getEditProvider()->addCommand(parentCmd);
    parentCmd->redo();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createColumnName()
//-----------------------------------------------------------------------------
QString SystemDesignDiagram::createColumnName(QString const& baseName)
{
    // Determine a unique name by using a running number.
    int runningNumber = 0;

    QStringList columnNames;
    foreach (QSharedPointer<ColumnDesc> column, getDesign()->getColumns())
    {
        columnNames.append(column->name());
    }

    QString format = "$ComponentName$_$InstanceNumber$";
    QString name = baseName;

    while (columnNames.contains(name))
    {
        name = format;
        name.replace("$ComponentName$", baseName);
        name.replace("$InstanceNumber$", QString::number(runningNumber));

        runningNumber++;
    }

    return name;
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
            dialog.setVLNV(comp->componentModel()->getVlnv());
            dialog.setWindowTitle(tr("Add Component to Library"));

            if (dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            VLNV vlnv = dialog.getVLNV();
            comp->componentModel()->setVlnv(vlnv);
            comp->componentModel()->setHierarchy(dialog.getProductHierarchy());
            comp->componentModel()->setFirmness(dialog.getFirmness());

            if (comp->type() == HWMappingItem::Type)
            {
				// create new view
				QSharedPointer<View> newView( new View() );

				// depending on the hierarchy level select the name for the view
				KactusAttribute::ProductHierarchy hier = comp->componentModel()->getHierarchy();

				switch (hier) 
				{
					case KactusAttribute::IP:
					case KactusAttribute::SOC:
					{
						// set the name
						newView->setName("rtl");
						break;
					}
					default:
					{
						newView->setName("flat");
						break;
					}
				}

				newView->addEnvIdentifier(QString("::"));

				comp->componentModel()->getModel()->getViews()->append(newView);
            }

            // Write the model to file.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Create an undo command.
            QSharedPointer<ComponentPacketizeCommand> cmd(new ComponentPacketizeCommand(this, comp, vlnv));
            getEditProvider()->addCommand(cmd);
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
                emit openComponent(comp->componentModel()->getVlnv());
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::openDesignForComponent()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::openDesignForComponent(ComponentItem* component, QString const& viewName)
{
    if (component->componentModel()->hasSWView(viewName))
    {
        emit openSWDesign(component->componentModel()->getVlnv(), viewName);
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

        if (getLibraryInterface()->contains(comp->componentModel()->getVlnv()))
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

                    if (dialog.getContentType() == ColumnTypes::IO)
                    {
                        columnWidth = IO_COLUMN_WIDTH;
                    }
                }

                QSharedPointer<ColumnDesc> desc(new ColumnDesc(dialog.name(), dialog.getContentType(),
                    dialog.getAllowedItems(), columnWidth));

                // Keep the previous width if the content type did not change.
                if (dialog.getContentType() == column->getColumnDesc()->getContentType())
                {
                    desc->setWidth(column->getColumnDesc()->getWidth());
                }

                QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc, getDesign()));
                getEditProvider()->addCommand(cmd);
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
            QSharedPointer<Document> libComp = getLibraryInterface()->getModel(vlnv);
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
            if (comp->getImplementation() == KactusAttribute::SW)
            {
                dragType_ = DRAG_TYPE_SW;
            }
            else if (comp->getImplementation() == KactusAttribute::HW)
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
	if (!data.canConvert<VLNV>())
    {
		return;
	}

	VLNV droppedVLNV = data.value<VLNV>();
    updateDropAction(event);

    // Check if the dragged item was a valid one.
    if (dragType_ == DRAG_TYPE_SW)
    {
        // Disallow self-instantiation.
        if (droppedVLNV == getEditedComponent()->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component cannot be instantiated to its own design."),
                QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }

        // Retrieve the component model.
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        // Set the instance name for the new component instance.
        QString instanceName = createInstanceName(comp->getVlnv().getName());

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
				// Create the diagram component.
				QSharedPointer<SWInstance> swInstance = createSWInstance(comp);

                // Create the SW component item.
                SWComponentItem* item = new SWComponentItem(getLibraryInterface(), comp, swInstance);
                
                item->setPos(stack->mapStackFromScene(snapPointToGrid(event->scenePos())));
                connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
                connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                // Create the undo command and execute it.
                QSharedPointer<SystemComponentAddCommand> cmd(
                    new SystemComponentAddCommand(stack, item, getDesign()));

                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                    this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                getEditProvider()->addCommand(cmd);
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

			// Create the diagram component.
			QSharedPointer<SWInstance> swInstance = createSWInstance(comp);

			// Create the SW component item.
			SWComponentItem* newCompItem = new SWComponentItem(getLibraryInterface(), comp, swInstance);

            // Perform the replacement.
            QSharedPointer<ReplaceSystemComponentCommand> cmd(new ReplaceSystemComponentCommand(
                oldCompItem, newCompItem, false, oldCompItem->type() == HWMappingItem::Type, getDesign()));

            connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
            connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

            getEditProvider()->addCommand(cmd);
            cmd->redo();
        }
    }
    else if (dragType_ == DRAG_TYPE_HW)
    {
        // Retrieve the component.
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Component> newComponent = libComp.staticCast<Component>();

        // Check if the component does not have any hierarchical views.
        if (newComponent->getHierViews().isEmpty())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("HW component '%1' does not contain any hierarchical views.\n"
                               "In order to add new HW components to the system design, the HW components must "
                               "be added to the HW design.").arg(droppedVLNV.toString()),
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
        QSharedPointer<SystemView> newView( new SystemView(dialog.getSystemViewName()) );
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

            QSharedPointer<Design> design = getDesign();
            design->setVlnv(designVLNV);

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

			foreach ( QSharedPointer<VendorExtension> extension, *getEditedComponent()->getVendorExtensions() )
			{
				QSharedPointer<SystemView> currentView = extension.dynamicCast<SystemView>();

				if ( currentView && currentView->name() == getParent()->getOpenViewName() )
				{
					getEditedComponent()->getVendorExtensions()->removeOne(currentView);
					break;
				}
			}

            getLibraryInterface()->writeModelToFile(getEditedComponent());
        }

        // Add the system view to the new HW component and save.
        newComponent->getVendorExtensions()->append(newView);
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
            getEditProvider()->addCommand(cmd);

            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
            dragEndPoint_ = 0;
        }
    }
    else if (dragType_ == DRAG_TYPE_DESIGN)
    {
        // Retrieve the design.
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Design> design = libComp.dynamicCast<Design>();

        // Import elements to the column under cursor.
        IGraphicsItemStack* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));
        importDesign(design, column, event->scenePos());
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createSWInstance()
//-----------------------------------------------------------------------------
QSharedPointer<SWInstance> SystemDesignDiagram::createSWInstance(QSharedPointer<Component> comp)
{
	QSharedPointer<SWInstance> swInstance(new SWInstance());
	QString instanceName = createInstanceName(comp->getVlnv().getName());
	swInstance->setInstanceName(instanceName);
	swInstance->setComponentRef( QSharedPointer<ConfigurableVLNVReference>(
		new ConfigurableVLNVReference(comp->getVlnv()) ) );

	return swInstance;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::getUsedInstanceNames()
//-----------------------------------------------------------------------------
QStringList SystemDesignDiagram::getUsedInstanceNames() const
{
    QStringList usedNames;

    foreach (QSharedPointer<ComponentInstance> instance, *getDesign()->getComponentInstances())
    {
        usedNames.append(instance->getInstanceName());
    }
    foreach (QSharedPointer<SWInstance> instance, getDesign()->getSWInstances())
    {
        usedNames.append(instance->getInstanceName());
    }

    return usedNames;
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

        if (vlnv != getEditedComponent()->getVlnv())
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
            (!dragEndPoint_->isConnected() && dragEndPoint_->getOwnerComponent() != 0)))
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
        openEnabled = dynamic_cast<ComponentItem*>(selectedItem)->componentModel()->getVlnv().isValid();
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
    QStringList hierarchicalViews;

    foreach( QSharedPointer<SWView> view, component->componentModel()->getSWViews() )
    {
        if ( view->getHierarchyRef().isValid() )
        {
            hierarchicalViews.append( view->name() );
        }
    }

    return hierarchicalViews;
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

            if (item->componentModel()->getImplementation() == KactusAttribute::HW)
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
        if (onlySW_)
        {
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("Low-level", ColumnTypes::COMPONENTS, 0, SW_COLUMN_WIDTH)));
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("Middle-level", ColumnTypes::COMPONENTS, 0, SW_COLUMN_WIDTH)));
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("High-level", ColumnTypes::COMPONENTS, 0, SW_COLUMN_WIDTH)));
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("Out", ColumnTypes::IO, 0, IO_COLUMN_WIDTH)));
        }
        else
        {
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0, SYSTEM_COLUMN_WIDTH)));
            design->addColumn(QSharedPointer<ColumnDesc>(
                new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0, SYSTEM_COLUMN_WIDTH)));
        }
    }

    QMap<unsigned int, QSharedPointer<ColumnDesc> > orderedColumns;
    foreach(QSharedPointer<ColumnDesc> desc, design->getColumns())
    {
        orderedColumns.insertMulti(desc->getPosition(), desc);
    }

    foreach (QSharedPointer<ColumnDesc> containedColumn, orderedColumns)
    {
        SystemColumn* column = new SystemColumn(containedColumn, getLayout().data());
        getLayout()->addColumn(column, true);
    }

    unsigned int colIndex = 0;

    // Create (HW) component instances.
    if (!onlySW_)
    {
        foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
        {
            QSharedPointer<Document> libComponent = getLibraryInterface()->getModel(*instance->getComponentRef());
            QSharedPointer<Component> component = libComponent.staticCast<Component>();

            if (!component)
            {
                emit errorMessage(tr("The component '%1' instantiated in the design '%2' was not found in " 
                    "the library").arg(instance->getComponentRef()->getName(), design->getVlnv().getName()));

                // Create an unpackaged component so that we can still visualize the component instance->
                component = QSharedPointer<Component>(new Component(*instance->getComponentRef()));
                component->setImplementation(KactusAttribute::HW);
            }

            HWMappingItem* item = new HWMappingItem(getLibraryInterface(), component, instance); //instance->getConfigurableElementValues());
            item->setImported(instance->isImported());
            item->setImportRef(instance->getImportRef());
            item->setPropertyValues(instance->getPropertyValues());

            connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

            // Check if the position is not found.
            if (instance->getPosition().isNull())
            {
                getLayout()->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % getLayout()->getColumns().size();
            }
            else
            {
                item->setPos(instance->getPosition());

                GraphicsColumn* column = getLayout()->findColumnAt(instance->getPosition());

                if (column != 0 && column->isItemAllowed(item))
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
    }

    // Create SW instances.
    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        QSharedPointer<Component> component;

        if (!instance->getComponentRef()->isEmpty())
        {
             component = getLibraryInterface()->getModel(*instance->getComponentRef()).dynamicCast<Component>();

             if (!component && instance->getComponentRef()->isValid())
             {
                 emit errorMessage(tr("The SW component '%1' instantiated in the design '%2' was not found in "
                     "the library").arg(instance->getComponentRef()->getName(), design->getVlnv().getName()));
             }           
        }
        
        if(!component)
        {
            // Create an unpackaged component so that we can still visualize the component instance->
            component = QSharedPointer<Component>(new Component(*instance->getComponentRef()));
            component->setImplementation(KactusAttribute::SW);
        }

        SWComponentItem* item = new SWComponentItem(getLibraryInterface(), component, instance);
        connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

        item->setImported(instance->isImported());
        item->setImportRef(instance->getImportRef());
        item->setPos(instance->getPosition());
        item->setPropertyValues(instance->getPropertyValues());
        item->setFileSetRef(instance->getFileSetRef());

        if (instance->isDraft())
        {
            item->setDraft();
        }

        if (instance->getMapping().isEmpty())
        {
            // Check if the position is not found.
            if (instance->getPosition().isNull())
            {
                getLayout()->getColumns().at(colIndex)->addItem(item);
                colIndex = (colIndex + 1) % getLayout()->getColumns().size();
            }
            else
            {
                GraphicsColumn* column = getLayout()->findColumnAt(instance->getPosition());

                if (column != 0 && column->isItemAllowed(item))
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
            // Otherwise the mapping has been specified. Find the corresponding HW component instance->
            HWMappingItem* mappingItem = getHWComponentByUUID(instance->getMapping());

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

    if (onlySW_)
    {
        loadInterfaces(design);
    }

    loadApiDependencies(design);
    loadComConnections(design);

    // Refresh the layout of all HW mapping items.
    if (!onlySW_)
    {
        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == HWMappingItem::Type)
            {
                HWMappingItem* mappingItem = static_cast<HWMappingItem*>(item);
                mappingItem->updateItemPositions();
            }
        }
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    getLayout()->updatePositions();
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::loadInterfaces()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadInterfaces(QSharedPointer<Design> design)
{
    QStringList componentInterfaceNames;

    // Create SW interface items for the top-level API and COM interfaces.
    foreach (QSharedPointer<ApiInterface> apiIf, getEditedComponent()->getApiInterfaces())
    {
        QSharedPointer<InterfaceGraphicsData> graphicsData = findOrCreateInterfaceGraphicsData(design, apiIf->name());

        SWInterfaceItem* item (new SWInterfaceItem(getEditedComponent(), apiIf, graphicsData));

        // Add the interface to the first column where it is allowed to be placed.
        addInterfaceItemToLayout(item);

        componentInterfaceNames.append(apiIf->name());
    }

    foreach (QSharedPointer<ComInterface> comIf, getEditedComponent()->getComInterfaces())
    {
        QSharedPointer<InterfaceGraphicsData> graphicsData = findOrCreateInterfaceGraphicsData(design, comIf->name());

        SWInterfaceItem* item (new SWInterfaceItem(getEditedComponent(), comIf, graphicsData));

        // Add the interface to the first column where it is allowed to be placed.
        addInterfaceItemToLayout(item);

        componentInterfaceNames.append(comIf->name());
    }

    foreach (QSharedPointer<InterfaceGraphicsData> graphicsData, design->getInterfaceGraphicsData())
    {
        if (!componentInterfaceNames.contains(graphicsData->getName()))
        {
            SWInterfaceItem* draftInterfaceItem (
                new SWInterfaceItem(getEditedComponent(), graphicsData->getName(), graphicsData, 0));

            addInterfaceItemToLayout(draftInterfaceItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::findOrCreateInterfaceGraphicsData()
//-----------------------------------------------------------------------------
QSharedPointer<InterfaceGraphicsData> SystemDesignDiagram::findOrCreateInterfaceGraphicsData(
    QSharedPointer<Design> design, QString const& interfaceName)
{
    foreach (QSharedPointer<InterfaceGraphicsData> graphicsData, design->getInterfaceGraphicsData())
    {
        if (interfaceName == graphicsData->getName())
        {
            return graphicsData;
        }
    }

    QSharedPointer<InterfaceGraphicsData> newGraphicsData (new InterfaceGraphicsData(interfaceName));
    design->getVendorExtensions()->append(newGraphicsData);
    return newGraphicsData;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::addInterfaceItemToLayout()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addInterfaceItemToLayout(SWInterfaceItem* item)
{
    GraphicsColumn* targetColumn = getLayout()->findColumnAt(item->scenePos());
    if (targetColumn && targetColumn->isItemAllowed(item))
    {
        targetColumn->addItem(item);
    }
    else
    {
        getLayout()->addItem(item);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::loadComConnections()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadComConnections(QSharedPointer<Design> design)
{
    foreach (QSharedPointer<ComInterconnection> conn, design->getComConnections())
    {
        ConnectionEndpoint* startPoint = findOrCreateEndpointItem(
            conn->getStartInterface(), ConnectionEndpoint::ENDPOINT_TYPE_COM, conn->name(), design);
        ConnectionEndpoint* endPoint = findOrCreateEndpointItem(
            conn->getEndInterface(), ConnectionEndpoint::ENDPOINT_TYPE_COM, conn->name(), design);

        if (startPoint && endPoint)
        {
            if (conn->isOffPage())
            {
                startPoint = startPoint->getOffPageConnector();
                endPoint = endPoint->getOffPageConnector();
            }

            QSharedPointer<ConnectionRoute> comRoute = getInterconnectionRoute(conn->name());

            ComGraphicsConnection* connection =
                new ComGraphicsConnection(startPoint, endPoint, conn, comRoute, true, this);

            if (comRoute->isOffpage())
            {
                connection->setVisible(false);
            }

            connect(connection, SIGNAL(errorMessage(QString const&)),
                this, SIGNAL(errorMessage(QString const&)));

            addItem(connection);
            connection->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::findOrCreateInterfaceItem()
//-----------------------------------------------------------------------------
ConnectionEndpoint* SystemDesignDiagram::findOrCreateEndpointItem(QSharedPointer<HierInterface> endpointInterface,
    SWConnectionEndpoint::EndpointType type, QString const& connectionName,
    QSharedPointer<Design> containingDesign)
{
    QString interfaceReference = endpointInterface->getBusReference();
    QSharedPointer<ActiveInterface> activeReference = endpointInterface.dynamicCast<ActiveInterface>();
    if (activeReference)
    {
        QString componentReference = activeReference->getComponentReference();
        SystemComponentItem* componentItem = getComponent(componentReference);
        if (!componentItem)
        {
            emit errorMessage(
                tr("Component '%1' containing interface '%2' in connection '%3' was not found in the design.")
                .arg(componentReference).arg(endpointInterface->getBusReference()).arg(connectionName));
        }
        else
        {
            return findOrCreateSWPortItem(componentItem, interfaceReference, type, containingDesign);
        }
    }
    else
    {
        SWInterfaceItem* interfaceEndpoint = getSWInterfaceItem(interfaceReference);
        if (interfaceEndpoint->getType() == ConnectionEndpoint::ENDPOINT_TYPE_COM)
        {
            QSharedPointer<ComInterface> componentCom = getEditedComponent()->getComInterface(interfaceReference);
            if (!componentCom)
            {
                return createDummyInterface("COM", interfaceReference);
            }
        }
        else if (interfaceEndpoint->getType() == ConnectionEndpoint::ENDPOINT_TYPE_API)
        {
            QSharedPointer<ApiInterface> componentApi = getEditedComponent()->getApiInterface(interfaceReference);
            if (!componentApi)
            {
                return createDummyInterface("API", interfaceReference);
            }
        }

        return interfaceEndpoint;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::getSWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem* SystemDesignDiagram::getSWInterfaceItem(QString const& interfaceName) const
{
    SWInterfaceItem* endPoint = 0;

    foreach (QGraphicsItem* graphicsItem, items())
    {
        if (graphicsItem->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interfaceItem = qgraphicsitem_cast<SWInterfaceItem*>(graphicsItem);
            if (interfaceItem && interfaceName == interfaceItem->name())
            {
                endPoint = interfaceItem;
                break;
            }
        }
    }

    return endPoint;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createDummyInterface()
//-----------------------------------------------------------------------------
ConnectionEndpoint* SystemDesignDiagram::createDummyInterface(QString const& itemType,
    QString const& interfaceReference)
{
    emit errorMessage(tr("%1 interface '%2' was not found within the top component").
        arg(itemType).arg(interfaceReference));

    QSharedPointer<InterfaceGraphicsData> graphicsData (new InterfaceGraphicsData(interfaceReference));
    getDesign()->getVendorExtensions()->append(graphicsData);

    ConnectionEndpoint* hierarchicalInterface =
        new SWInterfaceItem(getEditedComponent(), interfaceReference, graphicsData, 0);
    hierarchicalInterface->setTemporary(true);
    hierarchicalInterface->updateInterface();

    return hierarchicalInterface;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::findOrCreatePortItem()
//-----------------------------------------------------------------------------
ConnectionEndpoint* SystemDesignDiagram::findOrCreateSWPortItem(SystemComponentItem* containingItem,
    QString const& interfaceReference, SWConnectionEndpoint::EndpointType type,
    QSharedPointer<Design> containingDesign)
{
    ConnectionEndpoint* portEndpoint = containingItem->getSWPort(interfaceReference, type);

    if (!portEndpoint)
    {
        QString interfaceType = "API";
        if (type == SWConnectionEndpoint::ENDPOINT_TYPE_COM)
        {
            interfaceType = "COM";
        }

        emit errorMessage(tr("%1 interface '%2' was not found in the component '%3'").
            arg(interfaceType).arg(interfaceReference).arg(containingItem->name()));

        portEndpoint = createMissingPort(interfaceReference, ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED,
            containingItem, containingDesign);
    }

    return portEndpoint;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::getInterconnectionRoute()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> SystemDesignDiagram::getInterconnectionRoute(QString const& interconnectionName)
    const
{
    foreach (QSharedPointer<ConnectionRoute> route, getDesign()->getRoutes())
    {
        if (route->name() == interconnectionName)
        {
            return route;
        }
    }

    return QSharedPointer<ConnectionRoute> (new ConnectionRoute(interconnectionName));
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::loadApiDependencies()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::loadApiDependencies(QSharedPointer<Design> design)
{
    foreach (QSharedPointer<ApiInterconnection> dependency, design->getApiConnections())
    {
        ConnectionEndpoint* startPoint = findOrCreateEndpointItem(
            dependency->getStartInterface(), ConnectionEndpoint::ENDPOINT_TYPE_API, dependency->name(), design);
        ConnectionEndpoint* endPoint = findOrCreateEndpointItem(
            dependency->getEndInterface(), ConnectionEndpoint::ENDPOINT_TYPE_API, dependency->name(), design);

        if (dependency->isOffPage())
        {
            startPoint = startPoint->getOffPageConnector();
            endPoint = endPoint->getOffPageConnector();
        }

        QSharedPointer<ConnectionRoute> apiRoute = getInterconnectionRoute(dependency->name());
        ApiGraphicsConnection* connection =
            new ApiGraphicsConnection(startPoint, endPoint, dependency, apiRoute, true, this);

        connection->setImported(dependency->isImported());

        if (apiRoute->isOffpage())
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
                                                   SystemComponentItem* component, QSharedPointer<Design> design)
{
    SWPortItem* port = new SWPortItem(portName, component);
    component->addPort(port);

    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        if (instance->getInstanceName() == component->name())
        {
            if (type == ConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                port->setPos(instance->getApiInterfacePositions().value(portName));
            }
            else if (type == ConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                port->setPos(instance->getComInterfacePositions().value(portName));
            }

            component->onMovePort(port);
            return port;
        }
    }

    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        if (instance->getInstanceName() == component->name())
        {
            if (type == ConnectionEndpoint::ENDPOINT_TYPE_API)
            {
                port->setPos(instance->getApiInterfacePositions().value(portName));
            }
            else if (type == ConnectionEndpoint::ENDPOINT_TYPE_COM)
            {
                port->setPos(instance->getComInterfacePositions().value(portName));
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
    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        QSharedPointer<Document> libComponent = getLibraryInterface()->getModel(*instance->getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            emit errorMessage(tr("SW component %1 instantiated in the imported design was not "
                                 "found in the library").arg(instance->getComponentRef()->toString()));

            // Create an unpackaged component so that we can still visualize the component instance->
            component = QSharedPointer<Component>(new Component(*instance->getComponentRef()));
            component->setImplementation(KactusAttribute::SW);
        }

        // Determine a unique name for the instance->
        QString instanceName = instance->getInstanceName();

        if (getComponent(instanceName) != 0)
        {
            instanceName = createInstanceName(instanceName);
        }

        nameMappings.insert(instance->getInstanceName(), instanceName);

        SWComponentItem* item = new SWComponentItem(getLibraryInterface(), component, instance);
        item->setImported(instance->isImported());
        item->setImportRef(instance->getImportRef());
        item->setPos(stack->mapStackFromScene(guidePos));
        item->setPropertyValues(instance->getPropertyValues());
        item->setFileSetRef(instance->getFileSetRef());

        connect(item, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
        connect(item, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
        
        // Add the instance to the specified stack.
        stack->addItem(item);
    }

    // Import API dependencies.
    foreach (QSharedPointer<ApiInterconnection> dependency, design->getApiConnections())
    {
        // Find the referenced components.
        QString startComponentReference = dependency->getStartInterface()->getComponentReference();
        QString startApiReference = dependency->getStartInterface()->getBusReference();

        SystemComponentItem* comp1 = getComponent(nameMappings.value(startComponentReference));

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").
                arg(nameMappings.value(startComponentReference)));
            continue;
        }

        QString endComponentReference = "";
        QString endApiReference = dependency->getEndInterface()->getBusReference();

        SystemComponentItem* comp2;

        QSharedPointer<ActiveInterface> activeEndInterface =
            dependency->getEndInterface().dynamicCast<ActiveInterface>();
        if (activeEndInterface)
        {
            endComponentReference = activeEndInterface->getComponentReference();

            comp2 = getComponent(nameMappings.value(endComponentReference));
            if (comp2 == 0)
            {
                emit errorMessage(tr("Component '%1' was not found in the design").
                    arg(nameMappings.value(endComponentReference)));
                continue;
            }
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = static_cast<SWPortItem*>(comp1->getSWPort(startApiReference,
            SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port1 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").
                arg(startApiReference).arg(startComponentReference));

            port1 = createMissingPort(startApiReference, SWConnectionEndpoint::ENDPOINT_TYPE_API, comp1, design);
        }

        ConnectionEndpoint* port2 = static_cast<SWPortItem*>(comp2->getSWPort(endApiReference,
            SWConnectionEndpoint::ENDPOINT_TYPE_API));

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").
                arg(endApiReference).arg(endComponentReference));

            port2 = createMissingPort(endApiReference, SWConnectionEndpoint::ENDPOINT_TYPE_API, comp2, design);
        }

        if (dependency->isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
            dependency->name(), dependency->displayName(), dependency->description(), this);
        connection->setImported(dependency->isImported());

        if (dependency->isOffPage())
        {
            connection->setVisible(false);
        }

        connect(connection, SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(QString const&)));

        addItem(connection);
        connection->updatePosition();
    }

    // Import COM connections.
    foreach (QSharedPointer<ComInterconnection> conn, design->getComConnections())
    {
        // Find the referenced components.
        QString startComponentReference = conn->getStartInterface()->getComponentReference();
        QString startComReference = conn->getStartInterface()->getBusReference();

        SystemComponentItem* comp1 = getComponent(nameMappings.value(startComponentReference));

        if (comp1 == 0)
        {
            emit errorMessage(tr("Component '%1' was not found in the design").
                arg(nameMappings.value(startComponentReference)));
            continue;
        }

        SystemComponentItem* comp2;

        QString endComponentReference ="";
        QString endcomReference = conn->getEndInterface()->getBusReference();
        QSharedPointer<ActiveInterface> activeEndInterface = conn->getEndInterface().dynamicCast<ActiveInterface>();
        if (activeEndInterface)
        {
            endComponentReference = activeEndInterface->getComponentReference();
            comp2 = getComponent(nameMappings.value(endComponentReference));

            if (comp2 == 0)
            {
                emit errorMessage(tr("Component '%1' was not found in the design").arg(
                    nameMappings.value(endComponentReference)));
                continue;
            }
        }

        // Find the connected ports in the components.
        ConnectionEndpoint* port1 = comp1->getSWPort(startComReference, SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port1 == 0)
        {
            emit errorMessage(tr("COM interface '%1' was not found in the component '%2'").
                arg(startComReference).arg(startComponentReference));
            continue;
        }

        ConnectionEndpoint* port2 = comp2->getSWPort(endcomReference, SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        if (port2 == 0)
        {
            emit errorMessage(tr("API interface '%1' was not found in the component '%2'").
                arg(endcomReference).arg(endComponentReference));
            continue;
        }

        if (conn->isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

        GraphicsConnection* connection = new GraphicsConnection(port1, port2, true,
                                                                conn->name(),
                                                                conn->displayName(),
                                                                conn->description(), this);

        if (conn->isOffPage())
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
    QSharedPointer<ConnectionRoute> route(new ConnectionRoute(""));

    GraphicsConnection* connection;

    if (startPoint->isCom() || endPoint->isCom())
    {
        QSharedPointer<ComInterconnection> interconnection(new ComInterconnection());

        QSharedPointer<HierInterface> startInterface = createEndpointInterface(startPoint);
        interconnection->setInterface(startInterface);

        QSharedPointer<HierInterface> endInterface = createEndpointInterface(endPoint);
        interconnection->setInterface(endInterface);

        ComGraphicsConnection* comGraphicsConnection =
            new ComGraphicsConnection(startPoint, endPoint, interconnection, route, false, this);

        QString connectionName = comGraphicsConnection->createDefaultName();
        interconnection->setName(connectionName);
        route->setName(connectionName);

        connection = comGraphicsConnection;
    }
    else
    {
        QSharedPointer<ApiInterconnection> interconnection (new ApiInterconnection());

        QSharedPointer<HierInterface> startInterface = createEndpointInterface(startPoint);
        interconnection->setInterface(startInterface);

        QSharedPointer<HierInterface> endInterface = createEndpointInterface(endPoint);
        interconnection->setInterface(endInterface);

        ApiGraphicsConnection* apiGraphicsConnection =
            new ApiGraphicsConnection(startPoint, endPoint, interconnection, route, false, this);

        QString connectionName = apiGraphicsConnection->createDefaultName();
        interconnection->setName(connectionName);
        route->setName(connectionName);

        connection = apiGraphicsConnection;
    }

    return connection;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createEndpointInterface()
//-----------------------------------------------------------------------------
QSharedPointer<HierInterface> SystemDesignDiagram::createEndpointInterface(ConnectionEndpoint* connectionPoint)
{
    QString componentReference ("");
    QString endPointName = connectionPoint->name();

    SWPortItem* portItem = dynamic_cast<SWPortItem*>(connectionPoint);
    if (portItem)
    {
        componentReference = connectionPoint->encompassingComp()->name();
    }
    else
    {
        SWInterfaceItem* interfaceItem = dynamic_cast<SWInterfaceItem*>(connectionPoint);
        if (interfaceItem)
        {
            componentReference = interfaceItem->getOwnerComponent()->getVlnv().getName();
        }
    }

    QSharedPointer<HierInterface> endPointInterface;

    QString editedComponentName = getEditedComponent()->getVlnv().getName();
    if (componentReference == editedComponentName)
    {
        endPointInterface = QSharedPointer<HierInterface> (new HierInterface(endPointName));
    }
    else
    {
        endPointInterface =
            QSharedPointer<ActiveInterface> (new ActiveInterface(componentReference, endPointName));
    }

    return endPointInterface;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* SystemDesignDiagram::createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint)
{
    GraphicsConnection* connection;

    if (startPoint->isCom())
    {
        connection = new ComGraphicsConnection(startPoint->scenePos(), startPoint->getDirection(), endPoint,
            QVector2D(0.0f, 0.0f), QString(), QString(), this);
    }
    else
    {
        connection = new ApiGraphicsConnection(startPoint->scenePos(), startPoint->getDirection(), endPoint,
            QVector2D(0.0f, 0.0f), QString(), QString(), this);
    }

    connection->setZValue(1000);

    return connection;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createAddCommandForConnection()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> SystemDesignDiagram::createAddCommandForConnection(GraphicsConnection* connection)
{
    ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
    if (apiConnection)
    {
        return QSharedPointer<QUndoCommand>(new ApiConnectionAddCommand(this, apiConnection, getDesign()));
    }
    else
    {
        ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
        if (comConnection)
        {
            return QSharedPointer<QUndoCommand>(new ComConnectionAddCommand(this, comConnection, getDesign()));
        }
    }

    return QSharedPointer<QUndoCommand>(new QUndoCommand());
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::addTopLevelInterface()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::addTopLevelInterface(GraphicsColumn* column, QPointF const& pos)
{
    QString interfaceName = createDraftInterfaceName("interface");
    QSharedPointer<InterfaceGraphicsData> graphicsData (new InterfaceGraphicsData(interfaceName));

    SWInterfaceItem* newItem = new SWInterfaceItem(getEditedComponent(), interfaceName, graphicsData, 0);

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

    QSharedPointer<QUndoCommand> cmd(new SWInterfaceAddCommand(column, newItem, getDesign()));
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

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::createDraftInterfaceName()
//-----------------------------------------------------------------------------
QString SystemDesignDiagram::createDraftInterfaceName(QString const& baseName) const
{
    // Determine a unique name by using a running number.
    int runningNumber = 0;

    QStringList interfaceNames;
    foreach (QSharedPointer<ApiInterface> apiInterface, getEditedComponent()->getApiInterfaces())
    {
        interfaceNames.append(apiInterface->name());
    }
    foreach (QSharedPointer<ComInterface> comInterface, getEditedComponent()->getComInterfaces())
    {
        interfaceNames.append(comInterface->name());
    }
    foreach (QSharedPointer<InterfaceGraphicsData> graphicsData, getDesign()->getInterfaceGraphicsData())
    {
        if (!interfaceNames.contains(graphicsData->getName()))
        {
            interfaceNames.append(graphicsData->getName());
        }
    }


    QString format = "$InterfaceName$_$InterfaceNumber$";
    QString name = baseName;

    while (interfaceNames.contains(name))
    {
        name = format;
        name.replace("$InterfaceName$", baseName);
        name.replace("$InterfaceNumber$", QString::number(runningNumber));

        runningNumber++;
    }

    return name;
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
        if (!comp->componentModel()->getVlnv().isValid())
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
            getEditProvider()->addCommand(cmd);
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
            if (stack->getContentType() == ColumnTypes::COMPONENTS)
            {
                // Determine an unused name for the component instance.
                QString name = createInstanceName("instance");

                // Create a component model without a valid vlnv.
                QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
                comp->setVlnv(VLNV());
                comp->setImplementation(KactusAttribute::SW);

				// Create the corresponding diagram component.
				QSharedPointer<SWInstance> swInstance(new SWInstance());
				swInstance->setInstanceName(name);
                swInstance->setDraft(true);

                // Create the corresponding SW component item.
                SWComponentItem* swCompItem = new SWComponentItem(getLibraryInterface(), comp, swInstance);
                swCompItem->setDraft();
                swCompItem->setPos(snapPointToGrid(clickedPosition));

                // Add to the design.

                connect(swCompItem, SIGNAL(openCSource(ComponentItem*)), this, SIGNAL(openCSource(ComponentItem*)));
                connect(swCompItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                QSharedPointer<SystemComponentAddCommand> cmd(
                    new SystemComponentAddCommand(stack, swCompItem, getDesign()));

                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                    this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                getEditProvider()->addCommand(cmd);
                cmd->redo();
            }
            else if (stack->getContentType() == ColumnTypes::IO)
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
        QSharedPointer<ReplaceSystemComponentCommand> cmd(new ReplaceSystemComponentCommand(
            destSystemComponent, sourceSystemComponent, true, true, getDesign()));

        connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider()->addCommand(cmd);
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
            SWComponentItem* comp = dynamic_cast<SWComponentItem*>(item);

            collection.instances.append(ComponentInstanceCopyData());
            ComponentInstanceCopyData& instance = collection.instances.back();

            // Take a copy of the component model so that we are not influenced by any changes to the original.
            instance.component = QSharedPointer<Component>(new Component(*comp->componentModel()));

            QSharedPointer<SWInstance> copiedSWInstance = comp->getComponentInstance().dynamicCast<SWInstance>();
            instance.swInstance = QSharedPointer<SWInstance>(new SWInstance(*copiedSWInstance));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::pasteSWInstances()
//-----------------------------------------------------------------------------
void SystemDesignDiagram::pasteSWInstances(ComponentCollectionCopyData const collection, IGraphicsItemStack* stack,
                                           QUndoCommand* cmd, bool useCursorPos)
{
    foreach (ComponentInstanceCopyData const instanceCopy, collection.instances)
    {
        // Create unique name for the component instance.
        QSharedPointer<SWInstance> swInstanceCopy (new SWInstance(*instanceCopy.swInstance.data()));

        QString instanceName = createInstanceName(instanceCopy.swInstance->getInstanceName());
        swInstanceCopy->setInstanceName(instanceName);

        // Take a copy of the component in case of a draft.
        QSharedPointer<Component> component = instanceCopy.component;
        if (!component->getVlnv().isValid())
        {
            component = QSharedPointer<Component>(new Component(*instanceCopy.component));
        }

        SWComponentItem* comp = new SWComponentItem(getLibraryInterface(), component, swInstanceCopy);

        if (useCursorPos)
        {
            comp->setPos(findCursorPositionMappedToScene());
        }
        else
        {
            comp->setPos(swInstanceCopy->getPosition());
        }

        if (swInstanceCopy->isDraft())
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
            SystemComponentAddCommand* childCmd =
                new SystemComponentAddCommand(targetStack, comp,  getDesign(), cmd);

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

        QPointF pos = snapPointToGrid(targetComp->mapFromScene(findCursorPositionMappedToScene()));
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
        QString uniqueName = createDraftInterfaceName(portData.name);

        SWInterfaceItem* interface = 0;

        QSharedPointer<InterfaceGraphicsData> graphicsData(new InterfaceGraphicsData(uniqueName));

        if (portData.apiInterface != 0)
        {
            // Create a copy of the API interface.
            QSharedPointer<ApiInterface> apiIf(new ApiInterface(*portData.apiInterface));
            apiIf->setName(uniqueName);

            interface = new SWInterfaceItem(getEditedComponent(), apiIf, graphicsData);
        }
        else if (portData.comInterface != 0)
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(*portData.comInterface));
            comIf->setName(uniqueName);

            interface = new SWInterfaceItem(getEditedComponent(), comIf, graphicsData);
        }
        else
        {
            interface = new SWInterfaceItem(getEditedComponent(), uniqueName, graphicsData, 0);
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

        QUndoCommand* addCommand = new SWInterfaceAddCommand(stack, interface, getDesign(), cmd);
        addCommand->redo();

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<SWInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

        while (cur != oldPositions.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), stack, addCommand);
            }

            ++cur;
        }
    }
}
