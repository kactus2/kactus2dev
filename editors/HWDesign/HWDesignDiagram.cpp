//-----------------------------------------------------------------------------
// File: HWDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------

#include "HWDesignDiagram.h"

#include "BusInterfaceDialog.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "AdHocConnectionItem.h"
#include "HWDesignWidget.h"
#include "HWAddCommands.h"
#include "HWMoveCommands.h"
#include "HWChangeCommands.h"
#include "OffPageConnectorItem.h"
#include "SelectItemTypeDialog.h"

#include "PortmapDialog.h"

#include "columnview/HWColumn.h"
#include "columnview/ColumnEditDialog.h"

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnConstants.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>

#include <editors/common/DiagramUtil.h>
#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagramResolver.h>
#include <editors/common/StickyNote/StickyNote.h>
#include <editors/common/Association/Association.h>
#include <editors/common/ComponentItemAutoConnector/ComponentItemAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/AutoConnectorItem.h>
#include <editors/common/ComponentItemAutoConnector/HierarchicalPortItemMatcher.h>
#include <editors/common/ComponentItemAutoConnector/PortItemMatcher.h>
#include <editors/common/ComponentItemAutoConnector/HierarchicalPortTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/PortTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/HierarchicalBusInterfaceItemMatcher.h>
#include <editors/common/ComponentItemAutoConnector/BusInterfaceItemMatcher.h>
#include <editors/common/ComponentItemAutoConnector/HierarchicalBusInterfaceTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/BusInterfaceTableAutoConnector.h>
#include <editors/common/ComponentItemAutoConnector/BusInterfaceListFiller.h>
#include <editors/common/ComponentItemAutoConnector/PortListFiller.h>

#include <editors/HWDesign/HierarchicalBusInterfaceItem.h>
#include <editors/HWDesign/ActiveBusInterfaceItem.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/undoCommands/AdHocConnectionAddCommand.h>
#include <editors/HWDesign/undoCommands/ComponentDeleteCommand.h>
#include <editors/HWDesign/undoCommands/ComponentInstancePasteCommand.h>
#include <editors/HWDesign/undoCommands/PortPasteCommand.h>
#include <editors/HWDesign/undoCommands/ReplaceComponentCommand.h>
#include <editors/HWDesign/undoCommands/HWComponentAddCommand.h>
#include <editors/HWDesign/undoCommands/HWColumnAddCommand.h>
#include <editors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>
#include <editors/HWDesign/undoCommands/TopAdHocVisibilityChangeCommand.h>

#include <KactusAPI/include/LibraryHandler.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PartSelect.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/PortReference.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <QPair>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QString>
#include <QMessageBox>
#include <QCoreApplication>
#include <QListIterator>
#include <QApplication>
#include <QMenu>
#include <QClipboard>

Q_DECLARE_METATYPE(HWDesignDiagram::BusInterfaceCollectionCopyData)
Q_DECLARE_METATYPE(HWDesignDiagram::ComponentCollectionCopyData)
Q_DECLARE_METATYPE(HWDesignDiagram::ColumnCollectionCopyData)

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::HWDesignDiagram(LibraryInterface *lh, QSharedPointer<IEditProvider> editProvider,
    QSharedPointer<MultipleParameterFinder> designandInstancesParameterFinder,
    QSharedPointer<ListParameterFinder> designParameterFinder, DesignWidget* parent):
ComponentDesignDiagram(lh, editProvider, parent),
diagramResolver_(new DesignDiagramResolver(designParameterFinder)),
designAndInstancesParameterFinder_(designandInstancesParameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::loadDesign()
//-----------------------------------------------------------------------------
void HWDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    
    if (QList<QSharedPointer<ColumnDesc> > designColumns = design->getColumns(); 
        designColumns.isEmpty())
    {
        loadColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "IO", ColumnTypes::IO, 0, GraphicsColumnConstants::IO_COLUMN_WIDTH)));
        loadColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Buses", ColumnTypes::BUSES, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        loadColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Components", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        loadColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "IO", ColumnTypes::IO, 0, GraphicsColumnConstants::IO_COLUMN_WIDTH)));
    }
    else
    {
        QMultiMap<unsigned int, QSharedPointer<ColumnDesc> > orderedColumns;
        for (QSharedPointer<ColumnDesc> desc : designColumns)
        {
            orderedColumns.insert(desc->getPosition(), desc);
        }

        for (QSharedPointer<ColumnDesc> desc : orderedColumns)
        {
            loadColumn(desc);
        }
    }

    // Clear undo/redo stack to prevent undoing the column adds.
    getEditProvider()->clear();

    // Create diagram interfaces for the top-level bus interfaces.
    for (QSharedPointer<BusInterface> busIf : *getEditedComponent()->getBusInterfaces())
    {
        QSharedPointer<InterfaceGraphicsData> dataGroup =
            findOrCreateInterfaceExtensionGroup(design, busIf->name());

        auto topInterface =
            new HierarchicalBusInterfaceItem(getEditedComponent(), busIf, dataGroup, getLibraryInterface());

        GraphicsColumn* targetColumn = getLayout()->findColumnAt(topInterface->scenePos());
        if (targetColumn && targetColumn->isItemAllowed(topInterface))
        {
            targetColumn->addItem(topInterface);
        }
        else
        {
            getLayout()->addItem(topInterface);
        }        
    }

    for (QSharedPointer<ComponentInstance> instance : *design->getComponentInstances())
    {
        createComponentItem(instance, design);
    }

    for (QSharedPointer<Interconnection> interconnection : *design->getInterconnections())
    {
        createInterconnection(interconnection, design);
    }

    // Set the ad-hoc data for the diagram.
    setAdHocData(getEditedComponent(), design->getPortAdHocVisibilities());

    // Create top-level ad-hoc interfaces and set their positions.
    createHierachicalAdHocPorts(design);

    // Create ad-hoc connections based on the design data.
    for (QSharedPointer<AdHocConnection> adHocConn : *design->getAdHocConnections())
    {
        createAdHocConnection(adHocConn);
    }

    // Update the stacking of the columns.
    for (GraphicsColumn* column : getLayout()->getColumns())
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem* HWDesignDiagram::getComponentItem(QString const& instanceName)
{
	// Search all items in the scene.
	for (QGraphicsItem* item : items())
    {
		// if the item is a component
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);
            if (comp->name().compare(instanceName) == 0)
            {
                return comp;
            }
        }
    }

	// if no component was found
	emit errorMessage(tr("Component %1 was not found within design").arg(instanceName));
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::updateHierComponent()
{
    // Store all the bus interfaces.
    QList<QSharedPointer<BusInterface> > busIfs;

    // Search all graphics items in the scene.
    for (QGraphicsItem *item : items())
    {
        // Check if the item is a diagram interface and its bus interface is defined.
        auto diagIf = dynamic_cast<HierarchicalBusInterfaceItem*>(item);
        if (diagIf != nullptr && diagIf->getBusInterface() != nullptr && !diagIf->isInvalid())
        {
			busIfs.append(diagIf->getBusInterface());
        }
    }

    auto componentInterfaces = getEditedComponent()->getBusInterfaces();

    // Add any new interfaces in the component.
    for (auto diagramInterface : busIfs)
    {
        if (componentInterfaces->contains(diagramInterface) == false)
        {
            componentInterfaces->append(diagramInterface);
        }
    }

    // Remove interfaces deleted in the design from the component.
    for (auto componentInterface : *componentInterfaces)
    {
        if (busIfs.contains(componentInterface) == false)
        {
            componentInterfaces->removeAll(componentInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::loadColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::loadColumn(QSharedPointer<ColumnDesc> description)
{
    auto column = new HWColumn(description, getLayout().data());

    getLayout()->appendColumn(column);
    getDesign()->addColumn(description);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addColumn(QSharedPointer<ColumnDesc> desc)
{
    auto column = new HWColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new HWColumnAddCommand(getLayout().data(), column, this));
    getEditProvider()->addCommand(cmd);
    cmd->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onAdHocVisibilityChanged()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    QSharedPointer<VendorExtension> adhocExtension = getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();

    if (!adhocGroup)
    {
        adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
        getDesign()->getVendorExtensions()->append(adhocGroup);
    }

    if (visible)
    {
        QSharedPointer<Kactus2Placeholder> adhocData(new Kactus2Placeholder("kactus2:adHocVisible"));
        adhocGroup->addToGroup(adhocData);

        HierarchicalPortItem* adHocIf;

        
        if (QSharedPointer<Port> adhocPort = getEditedComponent()->getPort(portName); adhocPort)
        {
            adHocIf = new HierarchicalPortItem(getEditedComponent(), adhocPort, adhocData, 0);
        }
        else
        {
            adHocIf = createMissingHierarchicalAdHocPort(portName, adhocData, nullptr);
        }

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        getLayout()->addItem(adHocIf);
    }
    else
    {
        // Search for the ad-hoc interface and delete it.
        HWConnectionEndpoint* found = getDiagramAdHocPort(portName);
        Q_ASSERT(found != nullptr);

        static_cast<GraphicsColumn*>(found->parentItem())->removeItem(found);
        delete found;
        found = nullptr;

        for (QSharedPointer<VendorExtension> extension : adhocGroup->getByType("kactus2:adHocVisible"))
        {
            QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (portExtension->getAttributeValue(QStringLiteral("portName")) == portName)
            {
                adhocGroup->removeFromGroup(portExtension);
            }
        }

        if (adhocGroup->getByType(QStringLiteral("kactus2:adHocVisible")).isEmpty())
        {
            getDesign()->getVendorExtensions()->removeAll(adhocExtension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* HWDesignDiagram::getDiagramAdHocPort(QString const& portName)
{
    for (QGraphicsItem* item : items())
    {
        if (item->type() == HierarchicalPortItem::Type &&
            static_cast<HierarchicalPortItem*>(item)->name().compare(portName) == 0)
        {
            return static_cast<HWConnectionEndpoint*>(item);
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onCopyAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onCopyAction()
{
    if (!isProtected())
    {
        QList<QGraphicsItem*> items = selectedItems();
        int type = getCommonItemType(items);

        if (type == HierarchicalBusInterfaceItem::Type || type == ActiveBusInterfaceItem::Type)
        {
            BusInterfaceCollectionCopyData collection;
            copyInterfaces(items, collection);

            auto mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == HWComponentItem::Type)
        {
            ComponentCollectionCopyData collection;
            copyInstances(items, collection);

            auto mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == HWColumn::Type)
        {
            std::sort(items.begin(), items.end(), &sortByX);

            ColumnCollectionCopyData collection;

            for (QGraphicsItem* item : items)
            {
                auto column = static_cast<HWColumn*>(item);

                collection.columns.append(ColumnCopyData());
                ColumnCopyData& columnData = collection.columns.back();
                columnData.desc = column->getColumnDesc();

                copyInstances(column->getItems(), columnData.components);
                copyInterfaces(column->getItems(), columnData.interfaces);
            }

            auto mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
    }

    prepareContextMenuActions();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onPasteAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onPasteAction()
{
    if (isProtected())
    {
        return;
    }

    QList<QGraphicsItem*> selection = selectedItems();
    int selectionType = getCommonItemType(selection);

    QMimeData const* mimeData = QApplication::clipboard()->mimeData();

    if (selectionType == HWComponentItem::Type)
    {
        pasteInterfacesToDraftComponent(static_cast<HWComponentItem*>(selection.first()));
    }
    else if (mimeData->imageData().canConvert<ComponentCollectionCopyData>() && selection.empty())
    {
        pasteComponentsToColumn();
    }
    else if (mimeData->imageData().canConvert<ColumnCollectionCopyData>() &&
        (selection.empty() || selectionType == HWColumn::Type))
    {
        pasteColumns();
    }
    else if (mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>() && selection.empty()) 
    {
        pasteHierarchicalInterfaces(mimeData);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteInterfacesToDraftComponent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteInterfacesToDraftComponent(HWComponentItem* targetItem)
{
    // Paste interfaces only to draft components.
    if (targetItem->isDraft())
    {
        auto const* mimedata = QApplication::clipboard()->mimeData();
        if (mimedata->hasImage() && mimedata->imageData().canConvert<BusInterfaceCollectionCopyData>())
        {				
            BusInterfaceCollectionCopyData collection = mimedata->imageData().value<BusInterfaceCollectionCopyData>();

            QSharedPointer<QUndoCommand> pasteUndoCommand(new QUndoCommand());
            pasteInterfaces(collection, targetItem, pasteUndoCommand.data());
            getEditProvider()->addCommand(pasteUndoCommand);

            // Update sidebar view.
            emit componentSelected(targetItem);
            emit helpUrlRequested(QStringLiteral("hwdesign/hwinstance.html"));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteComponentsToColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteComponentsToColumn()
{
    GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());
    if (column != nullptr)
    {
        auto const* mimeData = QApplication::clipboard()->mimeData();
        ComponentCollectionCopyData collection = mimeData->imageData().value<ComponentCollectionCopyData>();

        QSharedPointer<QUndoCommand> pasteUndoCommand(new QUndoCommand());
        createComponentPasteCommand(collection, column, pasteUndoCommand.data(), true);
        
        getEditProvider()->addCommand(pasteUndoCommand);
        pasteUndoCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteColumns()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteColumns()
{
    auto const* mimeData = QApplication::clipboard()->mimeData();
    ColumnCollectionCopyData collection = mimeData->imageData().value<ColumnCollectionCopyData>();

    QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());

    QPointF columnPosition = findCursorPositionMappedToScene();

    for (ColumnCopyData const& columnData : collection.columns)
    {
        QSharedPointer<ColumnDesc> columnCopy(new ColumnDesc(*columnData.desc));
        auto columnItem = new HWColumn(columnCopy, getLayout().data());
        columnItem->setPos(columnPosition);

        new HWColumnAddCommand(getLayout().data(), columnItem, this, parentCommand.data());

        createComponentPasteCommand(columnData.components, columnItem, parentCommand.data(), false);
        
        pasteTopLevelInterfaces(columnData.interfaces, columnItem, parentCommand.data(), false);

        columnPosition += QPointF(columnCopy->getWidth(), 0);
    }

    getEditProvider()->addCommand(parentCommand);
    parentCommand->redo();

    emit clearItemSelection();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteHierarchicalInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteHierarchicalInterfaces(QMimeData const* mimeData)
{
    GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());
    if (column != nullptr)
    {
        BusInterfaceCollectionCopyData collection = mimeData->imageData().value<BusInterfaceCollectionCopyData>();

        QSharedPointer<QUndoCommand> pasteUndoCommand(new QUndoCommand());
        pasteTopLevelInterfaces(collection, column, pasteUndoCommand.data(), true);

        if (pasteUndoCommand->childCount() > 0)
        {
            getEditProvider()->addCommand(pasteUndoCommand); 
        }

        emit clearItemSelection();       
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onAddToLibraryAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onAddToLibraryAction()
{
    if (!singleSelection())
    {
        return;
    }

    QGraphicsItem* selection = selectedItems().first();
    if (selection != nullptr && selection->type() == HWComponentItem::Type)
    {
        selection->setSelected(true);
        HWComponentItem* componentItem = qgraphicsitem_cast<HWComponentItem *>(selection);

        QSharedPointer<Component> targetComponent = componentItem->componentModel();

        // Set the instance name as default name suggestion.
        VLNV componentVLNV = targetComponent->getVlnv();
        componentVLNV.setName(componentItem->name());

        NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, true, getParent());
        dialog.setVLNV(componentVLNV);
        dialog.setWindowTitle(tr("Add Component to Library"));

        if (dialog.exec() == QDialog::Accepted)
        {
            VLNV vlnv = dialog.getVLNV();
                
            QSharedPointer<QUndoCommand> cmd(new ComponentPacketizeCommand(this, componentItem, vlnv));

            targetComponent->setVlnv(vlnv);
            targetComponent->setHierarchy(dialog.getProductHierarchy());
            targetComponent->setFirmness(dialog.getFirmness());
            targetComponent->setTags(dialog.getTags());

            for (ConnectionEndpoint* interfacePoint : componentItem->getEndpoints())
            {
                interfacePoint->setSelectionHighlight(true);
                interfacePoint->setTemporary(false);

                ConnectionEndpoint* definedPoint(nullptr);

                for (auto const connection : interfacePoint->getConnections())
                {
                    if (connection->endpoint1() != interfacePoint && !connection->endpoint1()->isTemporary())
                    {
                        definedPoint = connection->endpoint1();
                        break;
                    }
                    else if (connection->endpoint2() != interfacePoint && !connection->endpoint2()->isTemporary())
                    {
                        definedPoint = connection->endpoint2();
                        break;
                    }
                }

                if (definedPoint)
                {
                    copyMode(definedPoint, interfacePoint, cmd.data());
                    copyType(definedPoint, interfacePoint, cmd.data());                    
                    copyPortMapsAndPhysicalPorts(definedPoint->getOwnerComponent(),
                        definedPoint->getBusInterface(), definedPoint->encompassingComp(), interfacePoint,
                        cmd.data());
                }

                interfacePoint->setSelectionHighlight(false);
                interfacePoint->updateInterface();
            }

            getLibraryInterface()->writeModelToFile(dialog.getPath(), targetComponent);

            getEditProvider()->addCommand(cmd);
            cmd->redo();

            QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                tr("Do you want to continue packaging the component completely?"),
                QMessageBox::NoButton, getParent());
            msgBox.setInformativeText(tr("Pressing Continue opens up the component editor."));

            QPushButton* btnContinue = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
            msgBox.addButton(tr("Skip"), QMessageBox::RejectRole);

            msgBox.exec();

            if (msgBox.clickedButton() == btnContinue)
            {
                openInComponentEditor(componentItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::openDesignForComponent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::openDesignForComponent(ComponentItem* component, QString const& viewName)
{
    if (component->componentModel()->hasView(viewName))
    {
        emit openDesign(component->componentModel()->getVlnv(), viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Allow double click only when the mode is select.
    if (getMode() != MODE_SELECT)
    {
        return;
    }

    // Find the item under the cursor.
    QGraphicsItem *item = itemAt(snapPointToGrid(mouseEvent->scenePos()), QTransform());

    if (item == nullptr)
    {
        return;
    }

    item = getBaseItemOf(item);

    if (item->type() == HWComponentItem::Type)
    {
        item->setSelected(true);
        HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);

        if (getLibraryInterface()->contains(comp->componentModel()->getVlnv()))
        {
            openComponentItem(comp);
        }
        else if (!isProtected())
        {           
           onAddToLibraryAction();
        }
    }
    else if (item->type() == HWColumn::Type && !isProtected())
    {
        item->setSelected(true);
        HWColumn* column = qgraphicsitem_cast<HWColumn*>(item);

        ColumnEditDialog dialog(getParent(), false, column);

        if (dialog.exec() == QDialog::Accepted)
        {
            int columnWidth = GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH;

            if (dialog.getContentType() == ColumnTypes::IO)
            {
                columnWidth = GraphicsColumnConstants::IO_COLUMN_WIDTH;
            }

            QSharedPointer<ColumnDesc> desc(new ColumnDesc(dialog.name(), dialog.getContentType(),
                dialog.getAllowedItems(), columnWidth));

            // Keep the previous width if the content type did not change.
            if (dialog.getContentType() == column->getColumnDesc()->getContentType())
            {
                desc->setWidth(column->getColumnDesc()->getWidth());
            }

            QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc, this));
            getEditProvider()->addCommand(cmd);
            cmd->redo();

        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::dragEnterEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragCompType_ = ColumnTypes::NONE;
    dragBus_ = false;

    // Allow drag only if the diagram is not locked and the dragged object is a vlnv.
    if (isProtected() || !event->mimeData()->hasImage() || !event->mimeData()->imageData().canConvert<VLNV>())
    {
        return;
    }

    event->acceptProposedAction();
    VLNV draggedVLNV = event->mimeData()->imageData().value<VLNV>();

    if (draggedVLNV.getType() == VLNV::BUSDEFINITION || draggedVLNV.getType() == VLNV::ABSTRACTIONDEFINITION)
    {
        dragBus_ = true;
    }
    else if (draggedVLNV.getType() == VLNV::COMPONENT)
    {
        // Determine the component type.
        QSharedPointer<Document const> libComp = getLibraryInterface()->getModelReadOnly(draggedVLNV);
        QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

        // component with given vlnv was not found
        if (!component) 
        {
            emit errorMessage(tr("Component with the VLNV %1 was not found in the library.").arg(
                draggedVLNV.toString()));
            return;
        }

        // Check if the implementation does not match with the edited component.
        if (component->getImplementation() != getEditedComponent()->getImplementation())
        {
            return;
        }

        if (component->isBridge())
        {
            dragCompType_ = ColumnTypes::BRIDGE;
        }
        else if (component->isChannel())
        {
            dragCompType_ = ColumnTypes::CHANNEL;
        }
        else
        {
            dragCompType_ = ColumnTypes::COMPONENT;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::dragLeaveEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragCompType_ = ColumnTypes::NONE;
    dragBus_ = false;
    
    if (dragEndPoint_)
    {
        dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        dragEndPoint_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::dropEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Retrieve the vlnv.
    QVariant data = event->mimeData()->imageData();
    if (!data.canConvert<VLNV>())
    {
        return;
    }

    VLNV droppedVLNV = data.value<VLNV>();
    updateDropAction(event);

    // Check if the dragged item was a valid one.
    if (dragCompType_ != ColumnTypes::NONE)
    {
        // Do not allow self-instantiation.
        if (droppedVLNV == getEditedComponent()->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component cannot be instantiated to its own design."), QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }

        // Create the component model.
        QSharedPointer<Component> dropComponent = getLibraryInterface()->getModel(droppedVLNV).staticCast<Component>();

        // Disallow instantiation of components marked as template.
        if (dropComponent->getFirmness() == KactusAttribute::TEMPLATE)
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Template component cannot be directly instantiated in a design. "
                "Save the template component as a new non-template component to enable instantiation."),
                QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }
        
        // Display error if user is trying to instantiate component of different standard revision.
        if (dropComponent->getRevision() != getEditedComponent()->getRevision())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component %1 using IP-XACT %2 cannot be instantiated in the design using IP-XACT %3")
                .arg(dropComponent->getVlnv().toString())
                .arg(Document::toString(dropComponent->getRevision()))
                .arg(Document::toString(getEditedComponent()->getRevision())),
                QMessageBox::Ok, getParent());

            msgBox.exec();
            return;
        }

        // Act based on the selected drop action.
        if (event->dropAction() == Qt::CopyAction)
        {
            QSharedPointer<QUndoCommand> addCommand(new QUndoCommand());
            QString instanceName = createInstanceName(dropComponent->getVlnv().getName());

            auto createdItem = createComponentItem(dropComponent, instanceName,
                event->scenePos(), addCommand.data());

            if (createdItem)
            {
                getEditProvider()->addCommand(addCommand);
            }
        }
        else if (event->dropAction() == Qt::MoveAction)
        {
            replaceComponentItemAtPositionWith(event->scenePos(), dropComponent);
        }
    }
    else if (dragBus_ && dragEndPoint_ != nullptr)
    {
        setInterfaceVLNVatEndpoint(droppedVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::setInterfaceVLNVatEndpoint()
//-----------------------------------------------------------------------------
void HWDesignDiagram::setInterfaceVLNVatEndpoint(VLNV const& droppedVLNV)
{
    Q_ASSERT(getLibraryInterface()->contains(droppedVLNV));

    VLNV busVLNV;
    VLNV absdefVLNV;

    // if the dropped was an abstraction definition
    if (getLibraryInterface()->getDocumentType(droppedVLNV) == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<AbstractionDefinition const> absDef = 
            getLibraryInterface()->getModelReadOnly(droppedVLNV).staticCast<AbstractionDefinition const>();

        busVLNV = absDef->getBusType();
        absdefVLNV = droppedVLNV;
    }
    else
    {
        busVLNV = droppedVLNV;
    }

    Q_ASSERT(busVLNV.getType() == VLNV::BUSDEFINITION);

    // Ask the user for the interface mode.
    BusInterfaceDialog dialog(getParent());
    dialog.addMode(General::MIRRORED_MASTER);
    dialog.addMode(General::SLAVE);
    dialog.addMode(General::MIRRORED_SLAVE);
    dialog.addMode(General::SYSTEM);
    dialog.addMode(General::MIRRORED_SYSTEM);
    dialog.addMode(General::MASTER);

    int result = dialog.exec();
    dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);

    if (result == QDialog::Accepted)
    {
        QString activeView = getActiveViewForEndPoint(dragEndPoint_);

        QSharedPointer<QUndoCommand> typeCommand(
            new EndPointTypesCommand(dragEndPoint_, busVLNV, absdefVLNV, activeView));
        new EndpointChangeCommand(dragEndPoint_, dialog.getSelectedMode(), typeCommand.data());

        getEditProvider()->addCommand(typeCommand);
        typeCommand->redo();

        dragEndPoint_ = nullptr;
    }    
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::replaceComponentItemAt()
//-----------------------------------------------------------------------------
void HWDesignDiagram::replaceComponentItemAtPositionWith(QPointF position, QSharedPointer<Component> component)
{
    // Replace the underlying component with the new one.
    auto previousItem = static_cast<HWComponentItem*>(getTopmostComponent(position));
    Q_ASSERT(previousItem != nullptr);

    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
        tr("Component instance '%1' is about to be replaced with an instance of %2. Continue and replace?").arg(
        previousItem->name(), component->getVlnv().toString()), QMessageBox::Yes | QMessageBox::No, getParent());

    if (msgBox.exec() == QMessageBox::Yes)
    {
        QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());

        // Create instance of the dropped component.
        HWComponentItem* newCompItem = createComponentItem(component, previousItem->name(), previousItem->scenePos(),
            parentCommand.data());

        // Perform the replacement.
        auto replaceCommand(new ReplaceComponentCommand(previousItem, newCompItem, this, parentCommand.data()));

        connect(replaceCommand, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(replaceCommand, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        replaceCommand->redo();

        // Remove the old component.
        auto deleteCmd = new ComponentDeleteCommand(this, 
            getLayout()->findColumnAt(previousItem->scenePos()), previousItem, parentCommand.data());

        connect(deleteCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(deleteCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        deleteCmd->redo();

        getEditProvider()->addCommand(parentCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::updateDropAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::updateDropAction(QGraphicsSceneDragDropEvent* event)
{
    if (dragCompType_ != ColumnTypes::NONE)
    {
        // Find the topmost component under the cursor.
        auto const item = getTopmostComponent(event->scenePos());

        auto const column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));

        // If the item is a HW component, determine whether the component can be replaced with the dragged one.
        if (item != nullptr && item->type() == HWComponentItem::Type && column != nullptr)
        {
            if (column->getColumnDesc()->getAllowedItems() & dragCompType_)
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
            // Otherwise check whether the component can be placed to the underlying column.
            if (column != nullptr && column->getColumnDesc()->getAllowedItems() & dragCompType_)
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

    else if (dragBus_)
    {        
        if (dragEndPoint_)
        {
            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        }
        
        dragEndPoint_ =  DiagramUtil::snapToItem<HWConnectionEndpoint>(event->scenePos(), this, GridSize);     

        // Allow the drop event if the end point is undefined.
        if (dragEndPoint_ != nullptr && dragEndPoint_->isBus() && dragEndPoint_->isHierarchical() &&
            (dragEndPoint_->getBusInterface() == nullptr || !dragEndPoint_->getBusInterface()->getBusType().isValid()))
        {
            event->setDropAction(Qt::CopyAction);
            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }

        event->accept();
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::openComponentActionEnabled()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::openComponentActionEnabled() const
{
    return singleSelection() && selectedItems().first()->type() == componentType() && !draftSelected();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::itemIsCorrectType()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::selectedItemIsCorrectType() const
{
    int type = selectedItems().first()->type();

    return type == componentType() || type == HierarchicalBusInterfaceItem::Type ||
        type == ActiveBusInterfaceItem::Type || type == HWConnection::Type || type == AdHocConnectionItem::Type ||
        type == HWColumn::Type || type == HierarchicalPortItem::Type || type == ActivePortItem::Type ||
        type == StickyNote::Type || type == Association::Type;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::isHierarchicalComponent()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::isHierarchicalComponent(QGraphicsItem* item) const
{
    if (auto const component = qgraphicsitem_cast<HWComponentItem*>(item); component)
    {
        return component->componentModel()->isHierarchical();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyActionEnabled()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::copyActionEnabled() const
{
    if (selectedItems().empty())
    {
        return false;
    }

    int itemType = getCommonItemType(selectedItems());
    return (!isProtected() && 
        (itemType == ActiveBusInterfaceItem::Type || itemType == HierarchicalBusInterfaceItem::Type ||
         itemType == HWComponentItem::Type || itemType == HWColumn::Type));  
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteActionEnabled()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::pasteActionEnabled() const
{
    // If the selection is empty, check if the clipboard contains components or a column.
    QMimeData const* mimeData = QApplication::clipboard()->mimeData();

    if (isProtected() || !mimeData || !mimeData->hasImage())
    {
        return false;
    }

    bool pasteEnabled = false;        

    QList<QGraphicsItem*> selection = selectedItems();
    int commonType = getCommonItemType(selection);

    if (selection.empty())
    {
        auto const column = getLayout()->findColumnAt(contextMenuPosition());

        pasteEnabled = (mimeData->imageData().canConvert<ColumnCollectionCopyData>() || 
            (column != nullptr && mimeData->imageData().canConvert<ComponentCollectionCopyData>()) ||
            (column != nullptr && mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>()));
    }
    else if (commonType == HWColumn::Type)
    {
        pasteEnabled = mimeData->imageData().canConvert<ColumnCollectionCopyData>();
    }
    else
    {
        pasteEnabled = singleSelection() && draftSelected() && 
            mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>();
    }

    return pasteEnabled;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::openComponentByActiveView()
//-----------------------------------------------------------------------------
void HWDesignDiagram::openComponentByActiveView(ComponentItem* comp)
{
    QString viewName = getActiveViewOf(comp);

    // if view was found
    if (comp->componentModel()->hasView(viewName) && hierarchicalViewsOf(comp).contains(viewName))
    {
        openDesignForComponent(comp, viewName);
    }
    else
    {
        openInComponentEditor(comp);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::hierarchicalViewsOf()
//-----------------------------------------------------------------------------
QStringList HWDesignDiagram::hierarchicalViewsOf(ComponentItem* component) const
{
    return component->componentModel()->getHierViews();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::componentType()
//-----------------------------------------------------------------------------
int HWDesignDiagram::componentType() const
{
    return HWComponentItem::Type;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::offpageConnectorType()
//-----------------------------------------------------------------------------
int HWDesignDiagram::offpageConnectorType() const
{
    return OffPageConnectorItem::Type;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::connectionType()
//-----------------------------------------------------------------------------
int HWDesignDiagram::connectionType() const
{
    return HWConnection::Type;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateInterfaceExtensionGroup()getCo
//-----------------------------------------------------------------------------
QSharedPointer<InterfaceGraphicsData> HWDesignDiagram::findOrCreateInterfaceExtensionGroup(
    QSharedPointer<Design> design, QString const& busInterfaceName)
{
    for (QSharedPointer<InterfaceGraphicsData> graphicsData : design->getInterfaceGraphicsData())
    {
        if (busInterfaceName == graphicsData->getName())
        {
            return graphicsData;
        }
    }

    QSharedPointer<InterfaceGraphicsData> dataGroup (new InterfaceGraphicsData(busInterfaceName));
    dataGroup->setDirection(QVector2D(1, 0));
    design->getVendorExtensions()->append(dataGroup);

    return dataGroup;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onSelected()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection != nullptr)
    {
        QString revisionSuffix;

        if (getDesign()->getRevision() == Document::Revision::Std22)
        {
            revisionSuffix = QStringLiteral("2022");
        }

        // Check if the selected item was a component.
        if (newSelection->type() == HWComponentItem::Type)
        {
            auto comp = qgraphicsitem_cast<HWComponentItem *>(newSelection);
            emit componentSelected(comp);
            emit helpUrlRequested(tr("hwdesign/hwinstance%1.html").arg(revisionSuffix));
        }
        // Check if the selected item was a port.
        else if (newSelection->type() == ActiveBusInterfaceItem::Type)
        {
            auto port = qgraphicsitem_cast<ActiveBusInterfaceItem*>(newSelection);
            emit interfaceSelected(port);
            emit helpUrlRequested(tr("hwdesign/busport%1.html").arg(revisionSuffix));
        }
        // Check if the selected item was an interface.
        else if (newSelection->type() == HierarchicalBusInterfaceItem::Type)
        {
            auto interface = qgraphicsitem_cast<HierarchicalBusInterfaceItem*>(newSelection);
            emit interfaceSelected(interface);
            emit helpUrlRequested(tr("hwdesign/busport%1.html").arg(revisionSuffix));
        }
        // Check if the selected item was an ad-hoc port.
        else if (newSelection->type() == ActivePortItem::Type)
        {
            auto adHocPort = static_cast<ActivePortItem*>(newSelection);
            emit interfaceSelected(adHocPort);
            emit helpUrlRequested(QStringLiteral("hwdesign/adhocport.html"));
        }
        // Check if the selected item was an ad-hoc interface.
        else if (newSelection->type() == HierarchicalPortItem::Type)
        {
            auto adHocIf = static_cast<HierarchicalPortItem*>(newSelection);
            emit interfaceSelected(adHocIf);
            emit helpUrlRequested(QStringLiteral("hwdesign/adhocport.html"));
        }
		// check if the selected item was a connection
		else if (newSelection->type() == HWConnection::Type)
        {
            auto connection = qgraphicsitem_cast<HWConnection*>(newSelection);
			emit connectionSelected(connection);
            emit helpUrlRequested(QStringLiteral("hwdesign/connection.html"));
		}
        else if (newSelection->type() == AdHocConnectionItem::Type)
        {
            auto connection = qgraphicsitem_cast<AdHocConnectionItem*>(newSelection);
            emit connectionSelected(connection);
            emit helpUrlRequested(QStringLiteral("hwdesign/adhocconnection.html"));
        }
        else
        {
            // inform others that nothing is currently selected
            emit clearItemSelection();
            emit helpUrlRequested(QStringLiteral("hwdesign/hwdesign.html"));
        }
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();
        emit helpUrlRequested(QStringLiteral("hwdesign/hwdesign.html"));
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* HWDesignDiagram::createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint)
{    
    QSharedPointer<ConnectionRoute> route(new ConnectionRoute(""));
    
    GraphicsConnection* connection;

    if (startPoint->isAdHoc())
    {
        QSharedPointer<AdHocConnection> adHocConnection(new AdHocConnection(""));
        
        if (startPoint->isHierarchical())
        {
            QSharedPointer<PortReference> startReference(new PortReference(startPoint->getPort()->name()));
            adHocConnection->getExternalPortReferences()->append(startReference);
        }
        else
        {
            QSharedPointer<PortReference> startReference(new PortReference(startPoint->getPort()->name(),
                startPoint->encompassingComp()->name()));
            adHocConnection->getInternalPortReferences()->append(startReference);
        }
        
        if (endPoint->isHierarchical())
        {
            QSharedPointer<PortReference> endReference(new PortReference(endPoint->name()));
            adHocConnection->getExternalPortReferences()->append(endReference);
        }
        else
        {
            QSharedPointer<PortReference> endReference(new PortReference(endPoint->name(),
                endPoint->encompassingComp()->name()));
            adHocConnection->getInternalPortReferences()->append(endReference);
        }

        connection = new AdHocConnectionItem(startPoint, endPoint, adHocConnection, route, this);   

        auto connectionName = connection->createDefaultName();
        connection->setName(connectionName);
    }
    else
    {
        QSharedPointer<Interconnection> interconnection(new Interconnection());

        if (startPoint->isHierarchical())
        {
            QSharedPointer<HierInterface> startInterface(new HierInterface(startPoint->name()));
            interconnection->getHierInterfaces()->append(startInterface);
        }
        else
        {
            QSharedPointer<ActiveInterface> startInterface(
                new ActiveInterface(startPoint->encompassingComp()->name(), startPoint->name()));
            interconnection->setStartInterface(startInterface);
        }

        if (endPoint->isHierarchical())
        {
            QSharedPointer<HierInterface> endInterface(new HierInterface(endPoint->name()));
            interconnection->getHierInterfaces()->append(endInterface);
        }
        else
        {
            QSharedPointer<ActiveInterface> endInterface(
                new ActiveInterface(endPoint->encompassingComp()->name(), endPoint->name()));
            if (startPoint->isHierarchical())
            {
                interconnection->setStartInterface(endInterface);
            }
            else
            {
                interconnection->getActiveInterfaces()->append(endInterface);
            }            
        }

        connection = new HWConnection(startPoint, endPoint, interconnection, route, this);  

        auto connectionName = connection->createDefaultName();
        connection->setName(connectionName);
    }
    
    return connection;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* HWDesignDiagram::createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint)
{
    if (startPoint->isAdHoc())
    {
        return new AdHocConnectionItem(startPoint->scenePos(), startPoint->getDirection(), endPoint,
            QVector2D(0.0f, 0.0f), this);
    }
    else
    {
        return new HWConnection(startPoint->scenePos(), startPoint->getDirection(), endPoint,
            QVector2D(0.0f, 0.0f), this);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAddCommandForConnection()
//-----------------------------------------------------------------------------
QUndoCommand* HWDesignDiagram::createAddCommandForConnection(GraphicsConnection* connection, QUndoCommand* parentCommand /*= nullptr*/)
{
    if (auto const adhocConnection = dynamic_cast<AdHocConnectionItem*>(connection); adhocConnection)
    {
        return new AdHocConnectionAddCommand(this, adhocConnection, getDesign(), parentCommand);
    }

    if (auto const hwConnection = dynamic_cast<HWConnection*>(connection); hwConnection)
    {
        auto connectionCommand(new ConnectionAddCommand(this, hwConnection, getDesign(), parentCommand));

        ConnectionEndpoint* startPoint = connection->endpoint1();
        ConnectionEndpoint* endPoint = connection->endpoint2();

        ConnectionEndpoint* definedPoint = nullptr;
        ConnectionEndpoint* draftPoint = nullptr;
        
        if (!startPoint->isTypeLocked())
        {
            draftPoint = startPoint;
        }
        else
        {
            definedPoint = startPoint;
        }

        if (!endPoint->isTypeLocked())
        {
            draftPoint = endPoint;
        }
        else
        {
            definedPoint = endPoint;
        }

        if (definedPoint && draftPoint && draftPoint->isHierarchical())
        {
            if (!copyDefinitions(definedPoint, draftPoint, hwConnection, connectionCommand))
            {
                connectionCommand->undo();
                return nullptr;
            }
        }
                        
        return connectionCommand;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyDefinitions()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::copyDefinitions(ConnectionEndpoint* definedPoint, ConnectionEndpoint* undefinedPoint,
    HWConnection* definingConnection, QUndoCommand* parentCommand)
{    
    if (undefinedPoint->isHierarchical() && hasDefaultName(undefinedPoint))
    {
        copyInterfaceName(definedPoint, undefinedPoint, definingConnection, parentCommand);
    }
    
    copyMode(definedPoint, undefinedPoint, parentCommand);

    copyType(definedPoint, undefinedPoint, parentCommand);
   
    return copyPortsAndMapsForHierarchicalPoint(definedPoint, undefinedPoint, parentCommand);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::hasDefaultName()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::hasDefaultName(ConnectionEndpoint const* connectionPoint) const
{
    QRegularExpression defaultInterfaceName("bus(_\\d+)?");
    return defaultInterfaceName.match(connectionPoint->name()).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyInterfaceName()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyInterfaceName(ConnectionEndpoint* sourceInterface,
    ConnectionEndpoint* targetInterface, HWConnection* definingConnection, QUndoCommand* parentCommand)
{
    auto previousName = targetInterface->name();
    
    QStringList reservedNames = getTopLevelInterfaceNames();
    auto interfaceName = generateUniqueName(sourceInterface->name(), reservedNames);
       
    QList<QSharedPointer<HierInterface> > affectedInterfaces = 
        findInterfacesByName(previousName, definingConnection->getInterconnection());

    QUndoCommand* nameCommand = new EndpointNameChangeCommand(targetInterface, interfaceName, affectedInterfaces,
        parentCommand);
    nameCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findInterfacesByName()
//-----------------------------------------------------------------------------
QList<QSharedPointer<HierInterface> > HWDesignDiagram::findInterfacesByName(QString const& previousName,
     QSharedPointer<Interconnection> connection) const
{
    QList<QSharedPointer<HierInterface> > foundInterfaces;

    for (QSharedPointer<HierInterface> designInterface : *connection->getHierInterfaces())
    {
        if (designInterface->getBusReference().compare(previousName) == 0)
        {
            foundInterfaces.append(designInterface);
        }
    }

    return foundInterfaces;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyMode()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyMode(ConnectionEndpoint* sourceInterface, ConnectionEndpoint* targetInterface,
    QUndoCommand* parentCommand)
{
    General::InterfaceMode selectedMode = targetInterface->getBusInterface()->getInterfaceMode();

    if (selectedMode == General::INTERFACE_MODE_COUNT)
    {
        General::InterfaceMode sourceMode = sourceInterface->getBusInterface()->getInterfaceMode();

        if (targetInterface->isHierarchical() || sourceInterface->isHierarchical())
        {
            selectedMode = sourceMode;
        }
        else
        {
            selectedMode = General::getCompatibleInterfaceMode(sourceMode);
        }
    }    

    QUndoCommand* modeChangeCommand = new EndpointChangeCommand(targetInterface, selectedMode, parentCommand);
    modeChangeCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyType()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyType(ConnectionEndpoint* definedPoint, ConnectionEndpoint* draftPoint,
    QUndoCommand* parentCommand)
{
    VLNV busVLNV = definedPoint->getBusInterface()->getBusType();

    QString activeView = getActiveViewForEndPoint(definedPoint);

    VLNV absVLNV;
    if (definedPoint && definedPoint->getBusInterface() &&
        definedPoint->getBusInterface()->getAbstractionContainingView(activeView))
    {
        QSharedPointer<AbstractionType> sourceAbstraction =
            definedPoint->getBusInterface()->getAbstractionContainingView(activeView);
        if (sourceAbstraction)
        {
            absVLNV = *sourceAbstraction->getAbstractionRef().data();
        }
    }

    QUndoCommand* typeChangeCommand =
        new EndPointTypesCommand(draftPoint, busVLNV, absVLNV, activeView, parentCommand);
    typeChangeCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyPortsAndMapsForHierarchicalPoint()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::copyPortsAndMapsForHierarchicalPoint(ConnectionEndpoint* sourcePoint,
    ConnectionEndpoint* targetPoint, QUndoCommand* parentCommand)
{
    QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
        tr("Do you want to auto-copy the port maps or perform a manual setup using the existing ports instead?"),
        QMessageBox::NoButton, getParent());

    QPushButton* copyButton = msgBox.addButton(tr("Copy"), QMessageBox::YesRole);
    QPushButton* manualSetupButton = msgBox.addButton(tr("Manual Setup"), QMessageBox::NoRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

    targetPoint->setSelectionHighlight(true);
    msgBox.exec();
    targetPoint->setSelectionHighlight(false);
    
    bool setupDone = false;
    if (msgBox.clickedButton() == copyButton)
    {
        copyPortMapsAndPhysicalPorts(sourcePoint->getOwnerComponent(), sourcePoint->getBusInterface(),
            sourcePoint->encompassingComp(), targetPoint, parentCommand);
        setupDone = true;
    }
    else if (msgBox.clickedButton() == manualSetupButton)
    {
        setupDone = createPortMapsManually(sourcePoint, targetPoint, parentCommand);
    }

    return setupDone;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::copyPortMapsAndPhysicalPorts()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyPortMapsAndPhysicalPorts(QSharedPointer<Component> sourceComponent,
    QSharedPointer<BusInterface> sourceInterface, ComponentItem* sourceInstanceItem, ConnectionEndpoint* target,
    QUndoCommand* parentCommand)
{
    QString sourceView;
    if (!sourceInstanceItem)
    {
        sourceView = getTopView();
    }
    else
    {
        sourceView = getActiveViewOf(sourceInstanceItem);
    }

    QSharedPointer<AbstractionType> sourceAbstraction = sourceInterface->getAbstractionContainingView(sourceView);

    if (!sourceAbstraction || (sourceAbstraction && !sourceAbstraction->getAbstractionRef()))
    {
       return;
    }

    VLNV abstractionVLNV = *sourceAbstraction->getAbstractionRef();
    QSharedPointer<AbstractionDefinition const> absDef =
        getLibraryInterface()->getModelReadOnly(abstractionVLNV).staticCast<AbstractionDefinition const>();

    diagramResolver_->setContext(sourceComponent);

    General::InterfaceMode mode = target->getBusInterface()->getInterfaceMode();
    QStringList reservedNames = target->getOwnerComponent()->getPortNames();

    auto copyCommand = new QUndoCommand(parentCommand);

    QMap<QString, QString> nameTranslations;
    QList< QSharedPointer<PortMap> > newPortMaps;   
    for (QSharedPointer<PortMap> portMap : *sourceAbstraction->getPortMaps())
    {
        if (portMap->getPhysicalPort() && portMap->getLogicalPort())
        {
            QString logicalName = portMap->getLogicalPort()->name_;
            QString physicalName = portMap->getPhysicalPort()->name_;

            if (!nameTranslations.contains(physicalName) && sourceComponent->hasPort(physicalName))
            { 
                QSharedPointer<Port> sourcePort = sourceComponent->getPort(physicalName);

                DirectionTypes::Direction directionOverride = sourcePort->getDirection();
                if (absDef && absDef->hasPort(logicalName, mode))
                { 
                    directionOverride = absDef->getPortDirection(logicalName, mode, target->getBusInterface()->getSystem());
                }

                QSharedPointer<Port> clonePort = createConnectingPhysicalPort(sourcePort, directionOverride, reservedNames);
                nameTranslations.insert(physicalName, clonePort->name());

                QUndoCommand* portCopyCommand = new AddPhysicalPortCommand(target->getOwnerComponent(), clonePort, copyCommand);
                portCopyCommand->redo();
            }

            QSharedPointer<PortMap> cloneMap(new PortMap(*portMap));
            cloneMap->getPhysicalPort()->name_ = nameTranslations.value(physicalName);   

            if (QSharedPointer<PartSelect> clonePart = cloneMap->getPhysicalPort()->partSelect_; 
                clonePart)
            {
                clonePart->setLeftRange(diagramResolver_->parseToConstant(clonePart->getLeftRange()));
                clonePart->setRightRange(diagramResolver_->parseToConstant(clonePart->getRightRange()));
            }
            
            if (QSharedPointer<Range> logicalRange = cloneMap->getLogicalPort()->range_; 
                logicalRange)
            {
                logicalRange->setLeft(diagramResolver_->parseToConstant(logicalRange->getLeft()));
                logicalRange->setRight(diagramResolver_->parseToConstant(logicalRange->getRight()));
            }

            cloneMap->setLogicalTieOff(diagramResolver_->parseToConstant(portMap->getLogicalTieOff()));

            newPortMaps.append(cloneMap);
        }
    }

    QUndoCommand* portMapCreationCommand = new EndPointPortMapCommand(target, newPortMaps, copyCommand);
    portMapCreationCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnectingPhysicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> HWDesignDiagram::createConnectingPhysicalPort(QSharedPointer<Port> sourcePort, 
    DirectionTypes::Direction portDirection, QStringList const& reservedNames)
{
    QSharedPointer<Port> connectingPort(new Port(*sourcePort));

    QString newPortName = sourcePort->name();
    QRegularExpression directionalEnd(QStringLiteral("_((i)|(in)|(o)|(out)|(inout))$"));
    if (sourcePort->getDirection() != portDirection && directionalEnd.match(newPortName).hasMatch())
    {
        newPortName.remove(directionalEnd);
        newPortName.append('_');
        newPortName.append(DirectionTypes::direction2Str(portDirection));
    }

    newPortName = generateUniqueName(newPortName, reservedNames);
    connectingPort->setName(newPortName);

    connectingPort->setDirection(portDirection);

    connectingPort->setLeftBound(diagramResolver_->parseToConstant(sourcePort->getLeftBound()));
    connectingPort->setRightBound(diagramResolver_->parseToConstant(sourcePort->getRightBound()));

    connectingPort->setDefaultValue(diagramResolver_->parseToConstant(sourcePort->getDefaultValue()));

    connectingPort->setArrayLeft(diagramResolver_->parseToConstant(sourcePort->getArrayLeft()));
    connectingPort->setArrayRight(diagramResolver_->parseToConstant(sourcePort->getArrayRight()));

    return connectingPort;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createPortMapsManually()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::createPortMapsManually(ConnectionEndpoint* sourcePoint, ConnectionEndpoint* targetPoint,
    QUndoCommand* parentCommand)
{
    PortmapDialog dialog(getLibraryInterface(), targetPoint->getOwnerComponent(), 
        targetPoint->getBusInterface(), sourcePoint->getBusInterface(), getParent());

    int accepted = dialog.exec() == QDialog::Accepted;

    QList<QSharedPointer<PortMap> > newMaps = *targetPoint->getBusInterface()->getAllPortMaps();
    sourcePoint->getBusInterface()->clearAllPortMaps();

    new EndPointPortMapCommand(targetPoint, newMaps, parentCommand);

    return accepted;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addTopLevelInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addTopLevelInterface(GraphicsColumn* column, QPointF const& pos)
{
    QStringList reservedNames = getTopLevelInterfaceNames();
    QString draftInterfaceName = generateUniqueName("bus", reservedNames);

    QSharedPointer<BusInterface> busif(new BusInterface());
    busif->setName(draftInterfaceName);

    QPointF newPosition = snapPointToGrid(pos);

    QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(busif->name()));
    dataGroup->setPosition(newPosition);
    getDesign()->getVendorExtensions()->append(dataGroup);

    auto newItem = new HierarchicalBusInterfaceItem(getEditedComponent(), busif, dataGroup, getLibraryInterface());
    newItem->setPos(newPosition);

    // Save the positions of the other diagram interfaces.
    QMap<HierarchicalBusInterfaceItem*, QPointF> oldPositions;

    if (column->getContentType() == ColumnTypes::IO)
    {
        for (QGraphicsItem* item : column->childItems())
        {
            if (item->type() == HierarchicalBusInterfaceItem::Type)
            {
                auto interface = static_cast<HierarchicalBusInterfaceItem*>(item);
                oldPositions.insert(interface, interface->scenePos());
            }
        }
    }

    QSharedPointer<QUndoCommand> cmd(new ItemAddCommand(column, newItem, this));
    cmd->redo();

    // Determine if the other interfaces changed their position and create undo commands for them.
    if (column->getContentType() == ColumnTypes::IO)
    {
        for (auto cur = oldPositions.cbegin(); cur != oldPositions.cend(); ++cur)
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), column, this, cmd.data());
            }
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::draftAt()
//-----------------------------------------------------------------------------
void HWDesignDiagram::draftAt(QPointF const& clickedPosition)
{
    // Find the bottom-most item under the cursor.
    QList<QGraphicsItem*> itemList = items(clickedPosition);
    QGraphicsItem* clickedItem = nullptr;
    if (!itemList.empty())
    {
        clickedItem = itemList.back();
    }

    // If there was no item, then the new item should be added to the column under cursor.
    if (clickedItem == nullptr)
    {
        GraphicsColumn* column = getLayout()->findColumnAt(clickedPosition);

        if (column != nullptr)
        {
            // Check what kind of item should be created.
            if (unsigned int itemType = findColumnItemType(column); 
                itemType == ColumnTypes::INTERFACE)
            {
                addTopLevelInterface(column, clickedPosition);
            }
            else if (itemType != ColumnTypes::NONE)
            {
                addDraftComponentInstance(column, clickedPosition);
            }
        }
    }
    // Otherwise check if the item is an unpackaged component in which case
    // we can add a new port (bus interface) to it.
    else if (clickedItem->type() == HWComponentItem::Type)
    {
        auto component = static_cast<HWComponentItem*>(clickedItem);
        addDraftComponentInterface(component, clickedPosition);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findItemTypeForColumn()
//-----------------------------------------------------------------------------
unsigned int HWDesignDiagram::findColumnItemType(GraphicsColumn* column) const
{
    unsigned int itemType = column->getColumnDesc()->getAllowedItems();

    // Check if the item type is ambiguous (interface vs. the rest of the types).
    if (itemType != ColumnTypes::INTERFACE && itemType & ColumnTypes::INTERFACE)
    {
        // Open a dialog to determine which type of item to create.
        SelectItemTypeDialog dialog(column->getColumnDesc()->getAllowedItems(), getParent());

        if (dialog.exec() == QDialog::Accepted)
        {
            itemType = dialog.getSelectedItemType();
        }
        else
        {
            itemType = ColumnTypes::NONE;
        }
    }

    return itemType;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createDraftInnstance()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addDraftComponentInstance(GraphicsColumn* column, QPointF const& position)
{
    // Determine an unused name for the component instance.
    QString name = createInstanceName("instance");

    // Create a component model without a valid vlnv.
    auto comp = QSharedPointer<Component>(new Component(VLNV(), getEditedComponent()->getRevision()));
    comp->setImplementation(getEditedComponent()->getImplementation());

    // Create the corresponding diagram component.
    QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
    componentInstance->setInstanceName(name);

    auto diagComp = new HWComponentItem(getLibraryInterface(), componentInstance, comp, column);
    diagComp->setDraft();
    diagComp->setPos(snapPointToGrid(position));

    QSharedPointer<HWComponentAddCommand> addCommand(new HWComponentAddCommand(this, column, diagComp));

    connect(addCommand.data(), SIGNAL(componentInstantiated(ComponentItem*)),
        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
    connect(addCommand.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

    getEditProvider()->addCommand(addCommand);
    addCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addDraftInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addDraftComponentInterface(HWComponentItem* targetComponent, QPointF const& position)
{
    // The component is unpackaged if it has an invalid vlnv.
    if (!targetComponent->componentModel()->getVlnv().isValid())
    {
        QMap<ActiveBusInterfaceItem*, QPointF> oldPositions;

        // Save old port positions.
        for (QGraphicsItem* item : targetComponent->childItems())
        {
            if (item->type() == ActiveBusInterfaceItem::Type)
            {
                auto port = static_cast<ActiveBusInterfaceItem*>(item);
                oldPositions.insert(port, port->pos());
            }
        }

        QSharedPointer<QUndoCommand> cmd(new PortAddCommand(targetComponent, snapPointToGrid(position), this));
        cmd->redo();

        // Create child undo commands for the ports with changed position.
        for (auto current = oldPositions.cbegin(); current != oldPositions.cend(); ++current)
        {
            if (current.key()->pos() != current.value())
            {
                new PortMoveCommand(current.key(), current.value(), this, cmd.data());
            }
        }

        // Add the command to the edit stack.
        getEditProvider()->addCommand(cmd);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::replace()
//-----------------------------------------------------------------------------
void HWDesignDiagram::replace(ComponentItem* destComp, ComponentItem* sourceComp)
{
    HWComponentItem* destHWComponent = qgraphicsitem_cast<HWComponentItem*>(destComp);
    HWComponentItem* sourceHWComponent = qgraphicsitem_cast<HWComponentItem*>(sourceComp);

    if (destHWComponent && sourceHWComponent)
    {
        QSharedPointer<ReplaceComponentCommand> cmd(
            new ReplaceComponentCommand(destHWComponent, sourceHWComponent, this));

        connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider()->addCommand(cmd);
        cmd->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createComponentItem()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createComponentItem(QSharedPointer<ComponentInstance> instance, QSharedPointer<Design> design)
{
    QSharedPointer<Component> component;

    if (!instance->isDraft())
    {
        component = getLibraryInterface()->getModel<Component>(*instance->getComponentRef());

        if (!component && instance->getComponentRef()->isValid())
        {
            emit errorMessage(tr("Component %1 instantiated within design %2 was not found in the library").arg(
                instance->getComponentRef()->getName(), design->getVlnv().getName()));
        }           
    }

    if(!component)
    {
        // Create an unpackaged component so that we can still visualize the component instance.
        component = QSharedPointer<Component>(new Component(*instance->getComponentRef(), getEditedComponent()->getRevision()));
        component->setImplementation(KactusAttribute::HW);
    }
    
    auto item = new HWComponentItem(getLibraryInterface(), instance, component);

    // Check if the position is not found.
    // Migrate from the old layout to the column based layout.
    
    if (QPointF instancePosition = instance->getPosition(); 
        instancePosition.isNull())
    {
        getLayout()->addItem(item);
    }
    else
    {
        GraphicsColumn* column = getLayout()->findColumnAt(instancePosition);
        if (column != nullptr && column->isItemAllowed(item))
        {
            column->addItem(item, true);
        }
        else
        {
            getLayout()->addItem(item);
        }
    }

    onComponentInstanceAdded(item);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem* HWDesignDiagram::createComponentItem(QSharedPointer<Component> comp, QString const& instanceName,
    QPointF position, QUndoCommand* parentCommand)
{
    if (GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(position)); 
        column != nullptr)
    {
        if (!referenceFinderContainsComponent(comp))
        {
            QSharedPointer<ComponentParameterFinder> componentFinder(new ComponentParameterFinder(comp));
            designAndInstancesParameterFinder_->addFinder(componentFinder);
        }

        // Create the diagram component.                            
        QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
        componentInstance->setComponentRef(QSharedPointer<ConfigurableVLNVReference>(
            new ConfigurableVLNVReference(comp->getVlnv())));

        componentInstance->setInstanceName(instanceName);

        auto newItem = new HWComponentItem(getLibraryInterface(), componentInstance, comp);
        newItem->setPos(snapPointToGrid(position));

        auto addCommand(new HWComponentAddCommand(this, column, newItem, parentCommand));

        connect(addCommand, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCommand, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        addCommand->redo();

        return newItem;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::referenceFinderContainsComponent()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::referenceFinderContainsComponent(QSharedPointer<Component> component)
{
    for (QSharedPointer<ComponentInstance> instance : *getDesign()->getComponentInstances())
    {
        if (instance->getComponentRef())
        {
            QSharedPointer<Document> componentDocument =
                getLibraryInterface()->getModel(*instance->getComponentRef().data());
            if (componentDocument)
            {
                QSharedPointer<Component> instancedComponent = componentDocument.dynamicCast<Component>();
                if (instancedComponent && instancedComponent->getVlnv() == component->getVlnv())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createInterconnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createInterconnection(QSharedPointer<Interconnection> interconnection,
    QSharedPointer<Design> design)
{
    if (!interconnection->getHierInterfaces()->isEmpty())
    {
        createHierarchicalConnection(interconnection, design);
    }
    
    if (!interconnection->getActiveInterfaces()->isEmpty())
    {
        createInterconnectionBetweenComponents(interconnection, design);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createInterconnectionBetweenComponents()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createInterconnectionBetweenComponents(QSharedPointer<Interconnection> interconnection,
    QSharedPointer<Design> design)
{
    QSharedPointer<ActiveInterface> startInterface = interconnection->getStartInterface();
    QString startComponentRef = startInterface->getComponentReference();
    HWComponentItem* startComponent = getComponentItem(startComponentRef);
    if (!startComponent)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(startComponentRef));
        return;
    }

    QSharedPointer<ActiveInterface> endInterface = interconnection->getActiveInterfaces()->first();
    QString endComponentRef = endInterface->getComponentReference();
    HWComponentItem* endComponent = getComponentItem(endComponentRef);
    if (!endComponent)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(endComponentRef));
        return;
    }

    // Find the referenced ports.
    ConnectionEndpoint* startPort = findOrCreateMissingInterface(startComponent, 
        startInterface->getComponentReference(), startInterface->getBusReference(), design);
    ConnectionEndpoint* endPort = findOrCreateMissingInterface(endComponent, 
        endComponentRef, endInterface->getBusReference(), design);

    QSharedPointer<ConnectionRoute> route = findOrCreateRouteForInterconnection(interconnection->name());

    if (route->isOffpage())
    {
        startPort = startPort->getOffPageConnector();
        endPort = endPort->getOffPageConnector();
    }

    auto connectionItem = new HWConnection(startPort, endPort, interconnection, route, this);
    addItem(connectionItem);

    startPort->onConnect(endPort);
    endPort->onConnect(startPort);

    connectionItem->validate();

    startPort->addConnection(connectionItem);
    endPort->addConnection(connectionItem);

    if (route->isOffpage())
    {
        connectionItem->hide();
    }

    connectionItem->updatePosition();

    connect(connectionItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateRouteForInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> HWDesignDiagram::findOrCreateRouteForInterconnection(QString const& interconnectionName)
{
    for (QSharedPointer<ConnectionRoute> knownRoute : getDesign()->getRoutes())
    {
        if (knownRoute->name() == interconnectionName)
        {
            return knownRoute;
        }
    }

    QSharedPointer<ConnectionRoute> route(new ConnectionRoute(interconnectionName));
    getDesign()->addRoute(route);
    return route;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateMissingInterface()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::findOrCreateMissingInterface(HWComponentItem* componentItem, 
    QString const& componentRef, QString const& busRef, QSharedPointer<Design> design)
{
    ConnectionEndpoint* interfaceItem = componentItem->getBusPort(busRef);

    if (!interfaceItem)
    {
        emit errorMessage(tr("Bus interface %1 was not found in the component %2").arg(busRef, componentRef));
        interfaceItem = createMissingBusInterface(busRef, componentItem, design);
    }	

    return interfaceItem;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createMissingBusInterface()
//-----------------------------------------------------------------------------
ActiveBusInterfaceItem* HWDesignDiagram::createMissingBusInterface(QString const& interfaceName,
    HWComponentItem* containingComponent, QSharedPointer<Design> design)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(interfaceName);

    auto missingInterface = new ActiveBusInterfaceItem(busIf, getLibraryInterface(), containingComponent);
    missingInterface->setTemporary(true);
    containingComponent->addPort(missingInterface);

    for (QSharedPointer<ComponentInstance> instance : *design->getComponentInstances())
    {
        if (instance->getInstanceName() == containingComponent->name())
        {
            missingInterface->setPos(instance->getBusInterfacePositions().value(interfaceName));
            containingComponent->onMovePort(missingInterface);
            return missingInterface;
        }
    }

    return missingInterface;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createHierarchicalConnection(QSharedPointer<Interconnection> connection,
    QSharedPointer<Design> design)
{
    QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();

    QString startComponentRef = startInterface->getComponentReference();
    
    if (auto startComponent = getComponentItem(startComponentRef); 
        !startComponent)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(startComponentRef));
        return;
    }

    QSharedPointer<HierInterface> hierInterface = connection->getHierInterfaces()->first();
    ConnectionEndpoint* hierarchicalInterface = findOrCreateHierarchicalInterface(hierInterface->getBusReference());
    Q_ASSERT(hierarchicalInterface != nullptr);

    // Check if the interface item is already assigned to a column.
    
    if (GraphicsColumn* column = getLayout()->findColumnAt(hierarchicalInterface->scenePos()); 
        column != nullptr)
    {
        column->addItem(hierarchicalInterface);
    }
    else
    {
        getLayout()->addItem(hierarchicalInterface);
    }

    // Find the component where the hierarchical connection is connected.
    HWComponentItem *comp = getComponentItem(startInterface->getComponentReference());
    if (!comp)
    {
        emit errorMessage(tr("Component %1 was not found in the top-design").arg(
            startInterface->getComponentReference()));
        return;
    }

    // Find the port of the component.
    ConnectionEndpoint* componentPort = findOrCreateMissingInterface(comp, startInterface->getComponentReference(), 
        startInterface->getBusReference(), design);
    
    QSharedPointer<ConnectionRoute> route = findOrCreateRouteForInterconnection(connection->name());

    if (route->isOffpage())
    {
        componentPort = componentPort->getOffPageConnector();
        hierarchicalInterface = hierarchicalInterface->getOffPageConnector();
    }
  
    auto connectionItem = new HWConnection(componentPort, hierarchicalInterface, connection, route, this);
    if (route->isOffpage())
    {
        connectionItem->hide();
    }

    componentPort->onConnect(hierarchicalInterface);
    hierarchicalInterface->onConnect(componentPort);

    connectionItem->validate();

    componentPort->addConnection(connectionItem);
    hierarchicalInterface->addConnection(connectionItem);

    connect(connectionItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

    addItem(connectionItem);
    connectionItem->updatePosition();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateHierarchicalInterface()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::findOrCreateHierarchicalInterface(QString const& busRef)
{
    // if the bus interface was not found
    if (QSharedPointer<BusInterface> busIf = getEditedComponent()->getBusInterface(busRef); 
        busIf != nullptr)
    {
        // Find the corresponding diagram interface.
        for (QGraphicsItem* item : items())
        {
            if (item->type() == HierarchicalBusInterfaceItem::Type &&
                qgraphicsitem_cast<HierarchicalBusInterfaceItem*>(item)->getBusInterface() == busIf)
            {
                return qgraphicsitem_cast<HierarchicalBusInterfaceItem*>(item);
            }
        }
    }
    else
    {			
        emit errorMessage(tr("Bus interface %1 was not found in the top-component").arg(busRef));

        // Create a dummy interface which is marked as invalid.
        busIf = QSharedPointer<BusInterface>(new BusInterface());
        busIf->setName(busRef);

        QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(busIf->name()));
        getDesign()->getVendorExtensions()->append(dataGroup);

        auto hierarchicalInterface = new HierarchicalBusInterfaceItem(getEditedComponent(), busIf, dataGroup, nullptr);
        hierarchicalInterface->setTemporary(true);
        hierarchicalInterface->updateInterface();

        return hierarchicalInterface;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createHierachicalAdHocPorts()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createHierachicalAdHocPorts(QSharedPointer<Design> design)
{
    QSharedPointer<VendorExtension> adHocPortPositions = design->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adHocPortPositions.dynamicCast<Kactus2Group>();

    QStringList visiblePortNames;

    if (adhocGroup.isNull())
	{
		adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
		design->getVendorExtensions()->append(adhocGroup);
	}

    for (QSharedPointer<VendorExtension> positionExtension : adhocGroup->getByType("kactus2:adHocVisible"))
    {
        QSharedPointer<Kactus2Placeholder> adHocExtension = positionExtension.dynamicCast<Kactus2Placeholder>();
        QString portName = adHocExtension->getAttributeValue("portName");

        QSharedPointer<Port> adHocPort = getEditedComponent()->getPort(portName);

        HierarchicalPortItem* adHocIf;

        if (adHocPort)
        {
            adHocIf = new HierarchicalPortItem(getEditedComponent(), adHocPort, adHocExtension, 0);
            visiblePortNames.append(portName);
        }
        else
        {
            adHocIf = createMissingHierarchicalAdHocPort(portName, adHocExtension, 0);
        }

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        GraphicsColumn* column = getLayout()->findColumnAt(adHocIf->scenePos());
        if (column != nullptr && column->isItemAllowed(adHocIf))
        {
            column->addItem(adHocIf, true);
        }
        else
        {
            getLayout()->addItem(adHocIf);
        }
    }

    for (QSharedPointer<Port> adhocPort : *getEditedComponent()->getPorts())
    {
        if (adhocPort->isAdHocVisible() && !visiblePortNames.contains(adhocPort->name()))
        {
            QSharedPointer<Kactus2Placeholder> positionPlaceHolder(new Kactus2Placeholder("kactus2:adHocVisible"));
            positionPlaceHolder->setAttribute("portName", adhocPort->name());

            adhocGroup->addToGroup(positionPlaceHolder);

            auto adhocInterface(new HierarchicalPortItem(getEditedComponent(), adhocPort, positionPlaceHolder, nullptr));

            getLayout()->addItem(adhocInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createMissingHierarchicalAdHocPort()
//-----------------------------------------------------------------------------
HierarchicalPortItem* HWDesignDiagram::createMissingHierarchicalAdHocPort(QString const& portName,
    QSharedPointer<Kactus2Placeholder> adHocExtension, QGraphicsItem* parentItem)
{
    QSharedPointer<Port> missingPort (new Port());
    missingPort->setName(portName);

    auto missingInterface = new HierarchicalPortItem(getEditedComponent(), missingPort, adHocExtension, parentItem);

    return missingInterface;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAdHocConnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection)
{
    if (!adHocConnection->getTiedValue().isEmpty())
    {
        createAdHocTieOffConnection(adHocConnection);
        return;
    }

    // Convert one-to-many internal connections to one-to-one connections.
    if (adHocConnection->getExternalPortReferences()->isEmpty() &&
        !adHocConnection->getInternalPortReferences()->isEmpty())
    {
        QSharedPointer<PortReference> primaryPort = adHocConnection->getInternalPortReferences()->at(0);


        HWConnectionEndpoint* primaryPortItem = findAdhocPort(primaryPort);        

        if (primaryPortItem == nullptr)
        {
            return;
        }

        if (adHocConnection->isOffPage())
        {
            primaryPortItem = static_cast<HWConnectionEndpoint*>(primaryPortItem->getOffPageConnector());
        }

        for (int i = 1; i < adHocConnection->getInternalPortReferences()->size(); ++i)
        {
            QSharedPointer<PortReference> secondaryPort = adHocConnection->getInternalPortReferences()->at(i);
            createConnectionForAdHocPorts(adHocConnection, secondaryPort, primaryPortItem);
        }    
    }

    const int EXTERNAL_PORT_COUNT = adHocConnection->getExternalPortReferences()->count();
    for (int i = 0; i < EXTERNAL_PORT_COUNT; i++)
    {
        QSharedPointer<PortReference> externalPort = adHocConnection->getExternalPortReferences()->at(i);

        if (!getEditedComponent()->hasPort(externalPort->getPortRef()))
        {
            emit errorMessage(tr("Port %1 was not found in the top-level component").arg(
                externalPort->getPortRef()));
            continue;
        }

        setPortAdHocVisible(externalPort->getPortRef(), true);
        ConnectionEndpoint* topPortItem = getDiagramAdHocPort(externalPort->getPortRef());

        for (int j = i + 1; j < EXTERNAL_PORT_COUNT; j++)
        {
            QSharedPointer<PortReference> connectedExternalPort = 
                adHocConnection->getExternalPortReferences()->at(j);
            createConnectionForAdHocPorts(adHocConnection, connectedExternalPort, topPortItem);
        }

        for (QSharedPointer<PortReference> internalPort : *adHocConnection->getInternalPortReferences())
        {
            createConnectionForAdHocPorts(adHocConnection, internalPort, topPortItem);
        }

    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findAdhocPort()
//-----------------------------------------------------------------------------
ActivePortItem* HWDesignDiagram::findAdhocPort(QSharedPointer<PortReference> primaryPort)
{
    ActivePortItem* portItem(nullptr);
 
    if (HWComponentItem* componentItem = getComponentItem(primaryPort->getComponentRef()); 
        componentItem == nullptr)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(primaryPort->getComponentRef()));
    }
    else
    {
        portItem = componentItem->getAdHocPort(primaryPort->getPortRef());
        
        if (portItem == nullptr)
        {
            QSharedPointer<Port> physicalPort = componentItem->componentModel()->getPort(primaryPort->getPortRef());
            if (!physicalPort)
            {
                emit errorMessage(tr("Port %1 was not found in the component %2").arg(primaryPort->getPortRef(), 
                    primaryPort->getComponentRef()));
            }
            else
            {
                portItem = new ActivePortItem(physicalPort, componentItem);
                componentItem->addPort(portItem);
                componentItem->setPortAdHocVisible(primaryPort->getPortRef(), true);
            }
        }
    }    

    return portItem;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAdHocTieOffConnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createAdHocTieOffConnection(QSharedPointer<AdHocConnection> connection)
{
    for (QSharedPointer<PortReference> internalPort : *connection->getInternalPortReferences())
    {
        HWComponentItem* comp1 = getComponentItem(internalPort->getComponentRef());

        if (!comp1)
        {
            continue;
        }

        AdHocItem* componentPort = comp1->getAdHocPort(internalPort->getPortRef());

        if (!componentPort)
        {
            componentPort = comp1->createAdhocItem(internalPort->getPortRef());
        }

        QSharedPointer<Component> referencedComponent = componentPort->getOwnerComponent();
        diagramResolver_->setContext(referencedComponent);

        diagramResolver_->resolveAdhocTieOff(connection->getTiedValue(), componentPort);
    }

    for (QSharedPointer<PortReference> externalPort : *connection->getExternalPortReferences())
    {
        HWConnectionEndpoint* topAdHocPort = getDiagramAdHocPort(externalPort->getPortRef());

        AdHocItem* topPort;
        if (topAdHocPort)
        {
            topPort = dynamic_cast<AdHocItem*>(topAdHocPort);
        }
        else
        {
            topPort = createAdhocItem(externalPort->getPortRef());
        }

        QSharedPointer<Component> referencedComponent = topPort->getOwnerComponent();
        diagramResolver_->setContext(referencedComponent);

        diagramResolver_->resolveAdhocTieOff(connection->getTiedValue(), topPort);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnectionForAdHocPorts()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createConnectionForAdHocPorts(QSharedPointer<AdHocConnection> adHocConnection, 
    QSharedPointer<PortReference> secondaryPort, 
    ConnectionEndpoint* primaryPortItem)
{ 
    HWConnectionEndpoint* secondaryPortItem(0);

    if (secondaryPort->getComponentRef().isEmpty())
    {
        secondaryPortItem = getDiagramAdHocPort(secondaryPort->getPortRef());
    }
    else
    {
        secondaryPortItem = findAdhocPort(secondaryPort); 
    }

    if (secondaryPortItem == nullptr)
    {
        return;
    }

    QSharedPointer<ConnectionRoute> route = findOrCreateRouteForInterconnection(adHocConnection->name());

    if (route->isOffpage())
    {
        secondaryPortItem = static_cast<HWConnectionEndpoint*>(secondaryPortItem->getOffPageConnector());

        primaryPortItem = primaryPortItem->getOffPageConnector();
    }

    // Create the ad-hoc connection graphics item.
    auto connection = new AdHocConnectionItem(primaryPortItem, secondaryPortItem, adHocConnection, route, this);

    primaryPortItem->onConnect(secondaryPortItem);
    secondaryPortItem->onConnect(primaryPortItem);

    connection->validate();

    primaryPortItem->addConnection(connection);
    secondaryPortItem->addConnection(connection);

    connection->setRoute(adHocConnection->getRoute());

    if (route->isOffpage())
    {
        connection->hide();
    }

    connect(connection, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

    addItem(connection);
    connection->updatePosition();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyInterfaces(QList<QGraphicsItem*> const& items, BusInterfaceCollectionCopyData &collection)
{
    // Create instance copies.
    for (QGraphicsItem* item : items)
    {
        if (item->type() == HierarchicalBusInterfaceItem::Type || item->type() == ActiveBusInterfaceItem::Type)
        {
            HierarchicalBusInterfaceItem* busPort = static_cast<HierarchicalBusInterfaceItem*>(item);

            collection.instances.append(BusInterfaceCopyData());
            BusInterfaceCopyData& instance = collection.instances.back();

            instance.srcComponent = busPort->getOwnerComponent();
            instance.busInterface = busPort->getBusInterface();
            instance.containingItem = busPort->encompassingComp();
            instance.position = busPort->pos();
            instance.topLevelIf = item->type() == HierarchicalBusInterfaceItem::Type;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyInstances()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection)
{
    // Create instance copies.
    for (QGraphicsItem* item : items)
    {
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(item);

            collection.instances.append(ComponentInstanceCopyData());
            ComponentInstanceCopyData& instance = collection.instances.back();

            instance.component = comp->componentModel(); 
            instance.instance = comp->getComponentInstance();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
    HWComponentItem* component, QUndoCommand* cmd)
{
    QStringList reservedNames = component->componentModel()->getBusInterfaceNames();

    for (BusInterfaceCopyData const& instance : collection.instances)
    {        
        QString uniqueBusName = generateUniqueName(instance.busInterface->name(), reservedNames);        	      
        reservedNames.append(uniqueBusName);

        QSharedPointer<BusInterface> interfaceCopy;
        if (!component->isDraft())
        {
            interfaceCopy = QSharedPointer<BusInterface>(new BusInterface(*instance.busInterface));            
        }
        else
        {
            interfaceCopy = QSharedPointer<BusInterface>(new BusInterface());
            interfaceCopy->setInterfaceMode(instance.busInterface->getInterfaceMode());
        }

        interfaceCopy->setName(uniqueBusName);

        ActiveBusInterfaceItem* interfaceItem =
            new ActiveBusInterfaceItem(interfaceCopy, getLibraryInterface(), component);
        interfaceItem->setPos(snapPointToGrid(component->mapFromScene(contextMenuPosition())));
        
        // Lock the interface type for non-draft interfaces.
        interfaceItem->setTemporary(true);
        interfaceItem->setTypeLocked(interfaceCopy->getInterfaceMode() != General::INTERFACE_MODE_COUNT);

        QMap<QString, QPointF> oldLocations = component->getBusInterfacePositions();

        PortPasteCommand* pasteCmd = new PortPasteCommand(component, interfaceItem, this, cmd);

        if (!component->isDraft())
        {
            copyPortMapsAndPhysicalPorts(instance.srcComponent, instance.busInterface, instance.containingItem,
                interfaceItem, pasteCmd);
        }
        
        pasteCmd->redo();

        QMap<QString, QPointF>::iterator current = oldLocations.begin();
        while (current != oldLocations.end())
        {
            if (component->getBusPort(current.key())->pos() != current.value())
            {
                new PortMoveCommand(component->getBusPort(current.key()), current.value(), this, pasteCmd);
            }
            ++current;
        }  
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteTopLevelInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteTopLevelInterfaces(BusInterfaceCollectionCopyData const& collection,
    GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos)
{
    QStringList existingNames = getTopLevelInterfaceNames();

    for (BusInterfaceCopyData const& instance : collection.instances)
    {
        GraphicsColumn* targetColumn = column;
        if (!targetColumn->getContentType() == ColumnTypes::IO)
        {
            targetColumn = nullptr;

            for (GraphicsColumn* otherColumn : getLayout()->getColumns())
            {
                if (otherColumn->getContentType() == ColumnTypes::IO)
                {
                    targetColumn = otherColumn;
                    break;
                }
            }
        }
        
        if (targetColumn != nullptr)
        {           
            // Bus interface must have a unique name within the component.
            QString uniqueBusName = generateUniqueName(instance.busInterface->name(), existingNames);
            existingNames.append(uniqueBusName);

            QSharedPointer<BusInterface> copyBusIf(new BusInterface(*instance.busInterface));
            copyBusIf->setName(uniqueBusName);

            QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(copyBusIf->name()));

            HierarchicalBusInterfaceItem* pastedItem = new HierarchicalBusInterfaceItem(
                getEditedComponent(), copyBusIf, dataGroup, getLibraryInterface(), targetColumn);

            BusInterfacePasteCommand* pasteCmd = new BusInterfacePasteCommand(getEditedComponent(), pastedItem,
                targetColumn, this, cmd);    

            copyPortMapsAndPhysicalPorts(instance.srcComponent, instance.busInterface, instance.containingItem,
                pastedItem, pasteCmd);

            if (useCursorPos)
            {
                pastedItem->setPos(findCursorPositionMappedToScene());
            }
            else
            {
                pastedItem->setPos(instance.position);
            }

            // Store the positions of other interfaces.
            QMap<HierarchicalBusInterfaceItem*, QPointF> oldPositions;
            for (QGraphicsItem* item : targetColumn->childItems())
            {
                if (item->type() == HierarchicalBusInterfaceItem::Type)
                {
                    HierarchicalBusInterfaceItem* interface = static_cast<HierarchicalBusInterfaceItem*>(item);
                    oldPositions.insert(interface, interface->scenePos());
                }
            }

            // Paste interface to component.
            pasteCmd->redo();

            // Determine if the other interfaces changed their position and create undo commands for them.
            QMap<HierarchicalBusInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

            while (cur != oldPositions.end())
            {
                if (cur.key()->scenePos() != cur.value())
                {
                    new ItemMoveCommand(cur.key(), cur.value(), targetColumn, this, pasteCmd);
                }
                ++cur;
            }  
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createComponentPasteCommand()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createComponentPasteCommand(ComponentCollectionCopyData const& collection, GraphicsColumn* column,
    QUndoCommand* parentCommand, bool useCursorPos)
{
    for (ComponentInstanceCopyData const& instance : collection.instances)
    {
        QPointF position;
        if (useCursorPos)
        {
            position = findCursorPositionMappedToScene();
        }
        else
        {
            position = instance.instance->getPosition();
        }

        new ComponentInstancePasteCommand(instance.component, instance.instance, position, column, this,
            parentCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAdhocItem()
//-----------------------------------------------------------------------------
AdHocItem* HWDesignDiagram::createAdhocItem(QString const& portName)
{
    QSharedPointer<VendorExtension> adhocExtension = getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();

    if (!adhocGroup)
    {
        adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
        getDesign()->getVendorExtensions()->append(adhocGroup);
    }

    QSharedPointer<Kactus2Placeholder> adhocData(new Kactus2Placeholder("kactus2:adHocVisible"));
    adhocGroup->addToGroup(adhocData);

    HierarchicalPortItem* adHocIf;

    QSharedPointer<Port> adhocPort = getEditedComponent()->getPort(portName);
    if (adhocPort)
    {
        adHocIf = new HierarchicalPortItem(getEditedComponent(), adhocPort, adhocData, nullptr);
    }
    else
    {
        adHocIf = createMissingHierarchicalAdHocPort(portName, adhocData, nullptr);
    }

    return adHocIf;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::showAdhocPort()
//-----------------------------------------------------------------------------
void HWDesignDiagram::showAdhocPort(AdHocItem* portItem)
{
    HierarchicalPortItem* interfaceItem = dynamic_cast<HierarchicalPortItem*>(portItem);
    if (interfaceItem)
    {
        QPointF interfacePosition = interfaceItem->scenePos();

        GraphicsColumn* targetColumn = getLayout()->findColumnAt(interfacePosition);

        if (targetColumn->isItemAllowed(interfaceItem))
        {
            targetColumn->addItem(interfaceItem);
        }
        else
        {
            getLayout()->addItem(interfaceItem);
        }
    }

    QSharedPointer<VendorExtension> adhocExtension = getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();
    if (!adhocGroup)
    {
        adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
        getDesign()->getVendorExtensions()->append(adhocGroup);
    }

    for (QSharedPointer<VendorExtension> extension : adhocGroup->getByType("kactus2:adHocVisible"))
    {
        QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
        if (portExtension->getAttributeValue("portName") == portItem->name())
        {
            return;
        }
    }

    QSharedPointer<Kactus2Placeholder> newPortExtension = interfaceItem->getDataGroup();
    if (newPortExtension.isNull())
    {
        newPortExtension = QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:adHocVisible"));
        newPortExtension->setAttribute("portName", portItem->name());
        newPortExtension->setAttribute("x", QString::number(portItem->scenePos().x()));
        newPortExtension->setAttribute("y", QString::number(portItem->scenePos().y()));

        interfaceItem->setDataGroup(newPortExtension);
    }

    adhocGroup->addToGroup(newPortExtension);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::hideAdhocPort()
//-----------------------------------------------------------------------------
void HWDesignDiagram::hideAdhocPort(AdHocItem* portItem)
{
    QSharedPointer<VendorExtension> adhocExtension = getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();

    HierarchicalPortItem* interfaceItem = dynamic_cast<HierarchicalPortItem*>(portItem);
    if (adhocGroup && interfaceItem && interfaceItem->parentItem())
    {
        static_cast<GraphicsColumn*>(interfaceItem->parentItem())->removeItem(interfaceItem);

        for (QSharedPointer<VendorExtension> extension : adhocGroup->getByType("kactus2:adHocVisible"))
        {
            QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (portExtension->getAttributeValue("portName") == interfaceItem->name())
            {
                adhocGroup->removeFromGroup(portExtension);
            }
        }

        if (adhocGroup->getByType("kactus2:adHocVisible").isEmpty())
        {
            getDesign()->getVendorExtensions()->removeAll(adhocExtension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createUniqueName()
//-----------------------------------------------------------------------------
QString HWDesignDiagram::generateUniqueName(QString const& name, QStringList const& reservedNames) const
{
    QString uniqueName = name;
    QRegularExpression trailingCount("_\\d+$");

    int count = 0;

    while (reservedNames.contains(uniqueName))
    {
        uniqueName.remove(trailingCount);
        count++;        
        uniqueName.append("_");
        uniqueName.append(QString::number(count));
    }    

    return uniqueName;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getTopLevelInterfaceNames()
//-----------------------------------------------------------------------------
QStringList HWDesignDiagram::getTopLevelInterfaceNames() const
{
    QStringList existingNames;
    for (QGraphicsItem* item : items())
    {
        if (item->type() == HierarchicalBusInterfaceItem::Type)
        {
            HierarchicalBusInterfaceItem* interface = static_cast<HierarchicalBusInterfaceItem*>(item);
            existingNames.append(interface->name());
        }
    }

    return existingNames;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAutoConnector()
//-----------------------------------------------------------------------------
ComponentItemAutoConnector* HWDesignDiagram::createAutoConnector(ComponentItem* firstItem) const
{
    QString firstItemName = firstItem->name();
    QString firstItemVisibleName = getVisibleNameForComponentItem(firstItem);
    QString secondItemName = "";
    QString secondItemVisibleName = "";
    QSharedPointer<Component> firstComponent = firstItem->componentModel();
    QSharedPointer<Component> secondComponent;

    QSharedPointer<TableItemMatcher> busItemMatcher;
    QSharedPointer<TableAutoConnector> busAutoConnector;
    QSharedPointer<TableItemMatcher> portItemMatcher;
    QSharedPointer<TableAutoConnector> portAutoConnector;

    AutoConnectorItem::ContainerType secondContainerType;

    if (selectedItems().count() == 1)
    {
        secondComponent = getEditedComponent();
        secondItemName = secondComponent->getVlnv().getName();
        secondItemVisibleName = secondItemName;

        busItemMatcher = QSharedPointer<HierarchicalBusInterfaceItemMatcher>(
            new HierarchicalBusInterfaceItemMatcher(getLibraryInterface()));
        busAutoConnector = QSharedPointer< HierarchicalBusInterfaceTableAutoConnector>(
            new HierarchicalBusInterfaceTableAutoConnector(getLibraryInterface()));

        portItemMatcher = QSharedPointer<HierarchicalPortItemMatcher>(new HierarchicalPortItemMatcher());
        portAutoConnector = QSharedPointer< HierarchicalPortTableAutoConnector>(
            new HierarchicalPortTableAutoConnector());

        secondContainerType = AutoConnectorItem::TOP_COMPONENT;
    }
    else
    {
        QGraphicsItem* secondItem = selectedItems().first();
        if (secondItem == firstItem)
        {
            secondItem = selectedItems().last();
        }

        ComponentItem* secondComponentItem = dynamic_cast<ComponentItem*>(secondItem);

        secondItemName = secondComponentItem->name();
        secondItemVisibleName = getVisibleNameForComponentItem(secondComponentItem);
        secondComponent = secondComponentItem->componentModel();

        busItemMatcher = QSharedPointer<BusInterfaceItemMatcher>(
            new BusInterfaceItemMatcher(getLibraryInterface()));
        busAutoConnector = QSharedPointer<BusInterfaceTableAutoConnector>(
            new BusInterfaceTableAutoConnector(getLibraryInterface()));

        portItemMatcher = QSharedPointer<PortItemMatcher>(new PortItemMatcher());
        portAutoConnector = QSharedPointer<PortTableAutoConnector>(
            new PortTableAutoConnector());

        secondContainerType = AutoConnectorItem::COMPONENT_ITEM;
    }

    ComponentItemAutoConnector::AutoContainer firstContainer;
    firstContainer.component_ = firstComponent;
    firstContainer.name_ = firstItemName;
    firstContainer.visibleName_ = firstItemVisibleName;
    ComponentItemAutoConnector::AutoContainer secondContainer;
    secondContainer.component_ = secondComponent;
    secondContainer.name_ = secondItemName;
    secondContainer.visibleName_ = secondItemVisibleName;

    ComponentItemAutoConnector::TableTools busTools;
    busTools.itemMatcher_ = busItemMatcher;
    busTools.tableConnector_ = busAutoConnector;
    busTools.listFiller_ = QSharedPointer<BusInterfaceListFiller>(new BusInterfaceListFiller());

    ComponentItemAutoConnector::TableTools portTools;
    portTools.itemMatcher_ = portItemMatcher;
    portTools.tableConnector_ = portAutoConnector;
    portTools.listFiller_ = QSharedPointer<PortListFiller>(new PortListFiller());

    ComponentItemAutoConnector* autoConnector = new ComponentItemAutoConnector(
        firstContainer, secondContainer, busTools, portTools, secondContainerType, getDesign(), getParent());

    return autoConnector;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getEndPointForItem()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::getEndPointForItem(AutoConnectorItem* connectorItem,
    QUndoCommand* parentUndoCommand /*= nullptr*/)
{
    if (connectorItem->getContainterType() == AutoConnectorItem::COMPONENT_ITEM)
    {
        return getEndPointFromComponentItem(connectorItem, parentUndoCommand);
    }
    else
    {
        return getEndPointForTopComponentItem(connectorItem);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getEndPointFromComponentItem()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::getEndPointFromComponentItem(AutoConnectorItem* connectorItem,
    QUndoCommand* parentUndoCommand /*= nullptr*/)
{
    HWComponentItem* containingItem = getComponentItem(connectorItem->getContainingItem());
    auto itemName = connectorItem->getName();
    if (connectorItem->getItemType() == AutoConnectorItem::PORT)
    {
        for (auto const& endpoint : containingItem->getEndpoints())
        {
            if (endpoint->isAdHoc() && endpoint->name() == itemName)
            {
                return endpoint;
            }
        }

        QString name = connectorItem->getName();
        for (auto const& port : *containingItem->componentModel()->getPorts())
        {
            if (port->name() == name)
            {
                AdHocVisibilityChangeCommand* newVisibilityCommand =
                    new AdHocVisibilityChangeCommand(containingItem, itemName, true, parentUndoCommand);
                newVisibilityCommand->redo();

                for (auto const& endpoint : containingItem->getEndpoints())
                {
                    if (endpoint->isAdHoc() && endpoint->getPort()->name() == name)
                    {
                        return endpoint;
                    }
                }
            }
        }
    }
    else if (connectorItem->getItemType() == AutoConnectorItem::BUS_INTERFACE)
    {
        for (auto const& endPoint : containingItem->getEndpoints())
        {
            if (endPoint->isBus() && endPoint->name() == itemName)
            {
                return endPoint;
            }
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getEndPointForTopComponentItem()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::getEndPointForTopComponentItem(AutoConnectorItem* connectorItem,
    QUndoCommand* parentUndoCommand /*= nullptr*/)
{
    auto itemName = connectorItem->getName();
    if (connectorItem->getItemType() == AutoConnectorItem::PORT)
    {
        HWConnectionEndpoint* portEndPoint = getDiagramAdHocPort(itemName);
        if (!portEndPoint && getEditedComponent()->hasPort(connectorItem->getName()))
        {
            TopAdHocVisibilityChangeCommand* adHocCommand =
                new TopAdHocVisibilityChangeCommand(this, itemName, true, parentUndoCommand);
            adHocCommand->redo();
            portEndPoint = getDiagramAdHocPort(itemName);
        }

        return portEndPoint;
    }
    else
    {
        return findOrCreateHierarchicalInterface(itemName);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnectionBetweenEndPoints()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createConnectionBetweenEndPoints(ConnectionEndpoint* startPoint,
    ConnectionEndpoint* endPoint)
{
    if (startPoint->isAdHoc() || startPoint->isBus())
    {
        GraphicsConnection* newConnection = createConnection(startPoint, endPoint);

        startPoint->onConnect(endPoint);
        startPoint->onConnect(endPoint);

        startPoint->addConnection(newConnection);
        endPoint->addConnection(newConnection);

        if (startPoint->isAdHoc() && endPoint->isAdHoc())
        {
            AdHocConnectionItem* adhocItem = dynamic_cast<AdHocConnectionItem*>(newConnection);
            addItem(adhocItem);

            getDesign()->getAdHocConnections()->append(adhocItem->getInterconnection());
            getDesign()->addRoute(adhocItem->getRouteExtension());
        }
        else if (startPoint->isBus() && endPoint->isBus())
        {
            HWConnection* hwConnectionItem = dynamic_cast<HWConnection*>(newConnection);

            addItem(hwConnectionItem);

            getDesign()->getInterconnections()->append(hwConnectionItem->getInterconnection());
            getDesign()->addRoute(hwConnectionItem->getRouteExtension());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addAutoConnectorActionToContextMenu()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::addAutoConnectorActionToContextMenu() const
{
    return true;
}
