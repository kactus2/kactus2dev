//-----------------------------------------------------------------------------
// File: HWDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// HWDesignDiagram is a graphical view to a design.
//-----------------------------------------------------------------------------

#include "HWDesignDiagram.h"

#include "AdHocPortItem.h"
#include "AdHocInterfaceItem.h"
#include "BusInterfaceDialog.h"
#include "BusInterfaceItem.h"
#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "AdHocConnectionItem.h"
#include "HWDesignWidget.h"
#include "HWAddCommands.h"
#include "HWMoveCommands.h"
#include "HWChangeCommands.h"
#include "OffPageConnectorItem.h"
#include "SelectItemTypeDialog.h"

#include "columnview/HWColumn.h"
#include "columnview/ColumnEditDialog.h"

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <designEditors/common/DiagramUtil.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagramResolver.h>

#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/undoCommands/AdHocConnectionAddCommand.h>
#include <designEditors/HWDesign/undoCommands/ComponentInstancePasteCommand.h>
#include <designEditors/HWDesign/undoCommands/PortPasteCommand.h>
#include <designEditors/HWDesign/undoCommands/ReplaceComponentCommand.h>
#include <designEditors/HWDesign/undoCommands/HWComponentAddCommand.h>

#include <library/LibraryManager/libraryhandler.h>

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

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

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
                                 DesignWidget* parent):
ComponentDesignDiagram(lh, editProvider, parent),
dragCompType_(ColumnTypes::NONE),
dragBus_(false),
dragEndPoint_(0),
diagramResolver_(new DesignDiagramResolver())
{

}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::~HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::~HWDesignDiagram()
{

}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::loadDesign()
//-----------------------------------------------------------------------------
void HWDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    QList<QSharedPointer<ColumnDesc> > designColumns = design->getColumns();
    if (designColumns.isEmpty())
    {
        addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("IO", ColumnTypes::IO, 0, IO_COLUMN_WIDTH)));
        addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Buses", ColumnTypes::BUSES, 0, COMPONENT_COLUMN_WIDTH)));
        addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Components", ColumnTypes::COMPONENTS, 0, COMPONENT_COLUMN_WIDTH)));
        addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("IO", ColumnTypes::IO, 0, IO_COLUMN_WIDTH)));
    }
    else
    {
        QMap<unsigned int, QSharedPointer<ColumnDesc> > orderedColumns;
        foreach (QSharedPointer<ColumnDesc> desc, designColumns)
        {
            orderedColumns.insertMulti(desc->getPosition(), desc);
        }

        foreach (QSharedPointer<ColumnDesc> desc, orderedColumns)
        {
            addColumn(desc);
        }
    }

    // Clear undo/redo stack to prevent undoing the column adds.
    getEditProvider()->clear();

    // Create diagram interfaces for the top-level bus interfaces.
    foreach (QSharedPointer<BusInterface> busIf, *getEditedComponent()->getBusInterfaces())
    {
        QSharedPointer<InterfaceGraphicsData> dataGroup =
            findOrCreateInterfaceExtensionGroup(design, busIf->name());

        BusInterfaceItem* topInterface = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), 
            busIf, dataGroup);

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

    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        createComponentItem(instance, design);
    }

    foreach(QSharedPointer<Interconnection> interconnection, *design->getInterconnections())
    {
        createInterconnection(interconnection, design);
    }

    // Set the ad-hoc data for the diagram.
    setAdHocData(getEditedComponent(), design->getPortAdHocVisibilities());

    // Create top-level ad-hoc interfaces and set their positions.
    createHierachicalAdHocPorts(design);

    // Create ad-hoc connections based on the design data.
    foreach (QSharedPointer<AdHocConnection> adHocConn, *design->getAdHocConnections())
    {
        createAdHocConnection(adHocConn);
    }

    // Update the stacking of the columns.
    foreach (GraphicsColumn* column, getLayout()->getColumns())
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
	foreach (QGraphicsItem *item, items())
    {
		// if the item is a component
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);
            if (comp->name() == instanceName)
            {
                return comp;
            }
        }
    }

	// if no component was found
	emit errorMessage(tr("Component %1 was not found within design").arg(instanceName));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::setVisibilityControlState()
//-----------------------------------------------------------------------------
void HWDesignDiagram::setVisibilityControlState(QString const& name, bool state)
{
    if (name == "Bus Widths")
    {
        foreach (QGraphicsItem* item, items())
        {
            HWConnection* conn = dynamic_cast<HWConnection*>(item);
            if (conn != 0)
            {
                conn->setBusWidthVisible(state);
            }
        }
    }

    DesignDiagram::setVisibilityControlState(name, state);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::updateHierComponent()
{
    // store all the bus interfaces to a map
    QList<QSharedPointer<BusInterface> > busIfs;

    // Search all graphics items in the scene.
    foreach (QGraphicsItem *item, items())
    {
        // Check if the item is a diagram interface and its bus interface is defined.
        BusInterfaceItem* diagIf = dynamic_cast<BusInterfaceItem*>(item);
        if (diagIf != 0 && diagIf->getBusInterface() != 0 && !diagIf->isInvalid())
        {
			busIfs.append(diagIf->getBusInterface());
        }
    }

    getEditedComponent()->getBusInterfaces()->clear();
    getEditedComponent()->getBusInterfaces()->append(busIfs);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addColumn(QSharedPointer<ColumnDesc> desc)
{
    HWColumn* column = new HWColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(getLayout().data(), column, getDesign()));
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

        AdHocInterfaceItem* adHocIf;

        QSharedPointer<Port> adhocPort = getEditedComponent()->getPort(portName);
        if (adhocPort)
        {
            adHocIf = new AdHocInterfaceItem(getEditedComponent(), adhocPort, adhocData, 0);
        }
        else
        {
            adHocIf = createMissingHierarchicalAdHocPort(portName, adhocData, 0);
        }

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        getLayout()->addItem(adHocIf);
    }
    else
    {
        // Search for the ad-hoc interface and delete it.
        HWConnectionEndpoint* found = getDiagramAdHocPort(portName);
        Q_ASSERT(found != 0);

        static_cast<GraphicsColumn*>(found->parentItem())->removeItem(found);
        delete found;
        found = 0;

        foreach(QSharedPointer<VendorExtension> extension, adhocGroup->getByType("kactus2:adHocVisible"))
        {
            QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (portExtension->getAttributeValue("portName") == portName)
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
// Function: HWDesignDiagram::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* HWDesignDiagram::getDiagramAdHocPort(QString const& portName)
{
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == AdHocInterfaceItem::Type && static_cast<AdHocInterfaceItem*>(item)->name() == portName)
        {
            return static_cast<HWConnectionEndpoint*>(item);
        }
    }

    return 0;
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

        if (type == BusInterfaceItem::Type || type == BusPortItem::Type)
        {
            BusInterfaceCollectionCopyData collection;
            copyInterfaces(items, collection);

            QMimeData* mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == HWComponentItem::Type)
        {
            ComponentCollectionCopyData collection;
            copyInstances(items, collection);

            QMimeData* mimeData = new QMimeData();
            mimeData->setImageData(QVariant::fromValue(collection));
            QApplication::clipboard()->setMimeData(mimeData);
        }
        else if (type == HWColumn::Type)
        {
            qSort(items.begin(), items.end(), &sortByX);

            ColumnCollectionCopyData collection;

            foreach (QGraphicsItem* item, items)
            {
                HWColumn* column = static_cast<HWColumn*>(item);

                collection.columns.append(ColumnCopyData());
                ColumnCopyData& columnData = collection.columns.back();
                columnData.desc = column->getColumnDesc();

                copyInstances(column->getItems(), columnData.components);
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
        QMimeData const* mimedata = QApplication::clipboard()->mimeData();
        if (mimedata->hasImage() && mimedata->imageData().canConvert<BusInterfaceCollectionCopyData>())
        {				
            BusInterfaceCollectionCopyData collection = mimedata->imageData().value<BusInterfaceCollectionCopyData>();

            QSharedPointer<QUndoCommand> pasteUndoCommand(new QUndoCommand());
            pasteInterfaces(collection, targetItem, pasteUndoCommand.data());
            getEditProvider()->addCommand(pasteUndoCommand);

            // Update sidebar view.
            emit componentSelected(targetItem);
            emit helpUrlRequested("hwdesign/hwinstance.html");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteComponentsToColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteComponentsToColumn()
{
    GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());
    if (column != 0)
    {
        QMimeData const* mimeData = QApplication::clipboard()->mimeData();
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
    QMimeData const* mimeData = QApplication::clipboard()->mimeData();
    ColumnCollectionCopyData collection = mimeData->imageData().value<ColumnCollectionCopyData>();

    QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());

    QPointF columnPosition = findCursorPositionMappedToScene();

    foreach (ColumnCopyData const& columnData, collection.columns)
    {
        QSharedPointer<ColumnDesc> columnCopy(new ColumnDesc(*columnData.desc));
        HWColumn* columnItem = new HWColumn(columnCopy, getLayout().data());
        columnItem->setPos(columnPosition);

        new GraphicsColumnAddCommand(getLayout().data(), columnItem, getDesign(), parentCommand.data());

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
    if (column != 0 )
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
    if (selection != 0 && selection->type() == HWComponentItem::Type)
    {
        selection->setSelected(true);
        HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(selection);

        // Set the instance name as default name suggestion.
        comp->componentModel()->getVlnv().setName(comp->name());

        NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, true, getParent());
        dialog.setVLNV(comp->componentModel()->getVlnv());
        dialog.setWindowTitle(tr("Add Component to Library"));

        if (dialog.exec() == QDialog::Accepted)
        {
            VLNV vlnv = dialog.getVLNV();
            comp->componentModel()->setVlnv(vlnv);
            comp->componentModel()->setHierarchy(dialog.getProductHierarchy());
            comp->componentModel()->setFirmness(dialog.getFirmness());

            comp->componentModel()->getViews()->append(QSharedPointer<View>(new View("flat")));

            // Write the model to file.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new ComponentPacketizeCommand(this, comp, vlnv));
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
                openInComponentEditor(comp);
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

    if (item == 0)
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
            // Request the user to set the vlnv.
           onAddToLibraryAction();
        }
    }
    else if (item->type() == HWColumn::Type)
    {
        if (!isProtected())
        {
            item->setSelected(true);
            HWColumn* column = qgraphicsitem_cast<HWColumn*>(item);

            ColumnEditDialog dialog(getParent(), false, column);

            if (dialog.exec() == QDialog::Accepted)
            {
                int columnWidth = COMPONENT_COLUMN_WIDTH;

                if (dialog.getContentType() == ColumnTypes::IO)
                {
                    columnWidth = IO_COLUMN_WIDTH;
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
    else if (item->type() == BusPortItem::Type || item->type() == BusInterfaceItem::Type)
    {
        HWConnectionEndpoint* endpoint = static_cast<HWConnectionEndpoint*>(item);
        addBusToLibrary(endpoint);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addBusToLibrary()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addBusToLibrary(HWConnectionEndpoint* endpoint)
{
    // Check if the bus is unpackaged.
    if (endpoint->isBus() &&
        (endpoint->getBusInterface() == 0 || !endpoint->getBusInterface()->getBusType().isValid()))
    {
        // Request the user to set the vlnv.
        NewObjectDialog dialog(getLibraryInterface(), VLNV::BUSDEFINITION, true, getParent());
        dialog.setWindowTitle(tr("Create New Bus"));

        if (dialog.exec() == QDialog::Rejected)
        {
            return;
        }

        VLNV busVLNV = dialog.getVLNV();
        VLNV absVLNV = busVLNV;
        absVLNV.setName(busVLNV.getName() + ".absDef");
        absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

        // Make sure that both VLNVs are not yet used.
        if (getLibraryInterface()->contains(busVLNV))
        {
            emit errorMessage(tr("Bus definition %1 was already found in the library").arg(busVLNV.toString()));
            return;
        }
        else if (getLibraryInterface()->contains(absVLNV)) {
            emit errorMessage(tr("Abstraction definition %1 was already found in the library").arg(
                absVLNV.toString()));
            return;
        }

        // Create a bus definition.
        QSharedPointer<BusDefinition> busDef = QSharedPointer<BusDefinition>(new BusDefinition());
        busDef->setVlnv(busVLNV);
        getLibraryInterface()->writeModelToFile(dialog.getPath(), busDef);

        // Create an abstraction definition.
        QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
        absDef->setVlnv(absVLNV);
        absDef->setBusType(busVLNV);
        getLibraryInterface()->writeModelToFile(dialog.getPath(), absDef);

        // Ask the user for the interface mode.
        endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

        bool nameIsEditable = endpoint->type() == BusInterfaceItem::Type && endpoint->getConnections().empty();

        BusInterfaceDialog modeDialog(nameIsEditable, getParent());
        modeDialog.addMode(General::MIRROREDMASTER);
        modeDialog.addMode(General::SLAVE);
        modeDialog.addMode(General::MIRROREDSLAVE);
        modeDialog.addMode(General::SYSTEM);
        modeDialog.addMode(General::MIRROREDSYSTEM);
        modeDialog.addMode(General::MASTER);

        if (modeDialog.exec() == QDialog::Rejected)
        {
            endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
            return;
        }

        endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);

        // Save old settings.
        VLNV oldBusType;
        VLNV oldAbsType;
        General::InterfaceMode oldMode = General::MASTER;
        QString oldName = "";

        if (endpoint->getBusInterface() != 0)
        {
            oldBusType = endpoint->getBusInterface()->getBusType();
            oldAbsType = *endpoint->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef();
            oldMode = endpoint->getBusInterface()->getInterfaceMode();
            oldName = endpoint->getBusInterface()->name();
        }

        // Set the types for the end point.
        endpoint->setTypes(busVLNV, absVLNV, modeDialog.getSelectedMode());

        if (nameIsEditable)
        {
            endpoint->getBusInterface()->setName(modeDialog.name());
        }

        endpoint->updateInterface();

        QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(endpoint, oldBusType,
            oldAbsType, oldMode, oldName));
        getEditProvider()->addCommand(cmd);

        emit interfaceSelected(endpoint);
        emit openBus(busVLNV, absVLNV, true);
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
        QSharedPointer<Document> libComp = getLibraryInterface()->getModel(draggedVLNV);
        QSharedPointer<Component> component = libComp.dynamicCast<Component>();

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
        dragEndPoint_ = 0;
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
        QSharedPointer<Component> droppedComponent = getLibraryInterface()->getModel(droppedVLNV).staticCast<Component>();

        // Disallow instantiation of components marked as template.
        if (droppedComponent->getFirmness() == KactusAttribute::TEMPLATE)
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Template component cannot be directly instantiated in a design. "
                "Save the template component as a new non-template component to enable instantiation."),
                QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }

        // Act based on the selected drop action.
        if (event->dropAction() == Qt::CopyAction)
        {
            createComponentItem(droppedComponent, event->scenePos());
        }
        else if (event->dropAction() == Qt::MoveAction)
        {
            replaceComponentItemAtPositionWith(event->scenePos(), droppedComponent);
        }
    }
    else if (dragBus_ && dragEndPoint_ != 0)
    {
        setInterfaceVLNVatEndpoint(droppedVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::setInterfaceVLNVatEndpoint()
//-----------------------------------------------------------------------------
void HWDesignDiagram::setInterfaceVLNVatEndpoint(VLNV &droppedVLNV)
{
    Q_ASSERT(getLibraryInterface()->contains(droppedVLNV));
    droppedVLNV.setType(getLibraryInterface()->getDocumentType(droppedVLNV));

    VLNV absdefVLNV;

    // if the dropped was an abstraction definition
    if (droppedVLNV.getType() == VLNV::ABSTRACTIONDEFINITION)
    {
        absdefVLNV = droppedVLNV;

        // parse the abstraction definition
        QSharedPointer<AbstractionDefinition> absDef = 
            getLibraryInterface()->getModel(droppedVLNV).staticCast<AbstractionDefinition>();

        // set the bus definition used
        droppedVLNV = absDef->getBusType();
    }
    Q_ASSERT(droppedVLNV.getType() == VLNV::BUSDEFINITION);

    bool editName = dragEndPoint_->type() == BusInterfaceItem::Type &&
        !dragEndPoint_->getBusInterface()->getBusType().isValid() &&
        dragEndPoint_->getConnections().empty();

    // Ask the user for the interface mode.
    BusInterfaceDialog dialog(editName, getParent());
    dialog.addMode(General::MIRROREDMASTER);
    dialog.addMode(General::SLAVE);
    dialog.addMode(General::MIRROREDSLAVE);
    dialog.addMode(General::SYSTEM);
    dialog.addMode(General::MIRROREDSYSTEM);
    dialog.addMode(General::MASTER);

    int result = dialog.exec();
    dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);

    if (result == QDialog::Accepted)
    {
        // Save old settings.
        VLNV oldBusType;
        VLNV oldAbsType;
        General::InterfaceMode oldMode = General::MASTER;
        QString oldName = "";

        if (dragEndPoint_->getBusInterface() != 0)
        {
            oldBusType = dragEndPoint_->getBusInterface()->getBusType();
            if (dragEndPoint_->getBusInterface()->getAbstractionTypes() && 
                !dragEndPoint_->getBusInterface()->getAbstractionTypes()->isEmpty())
            {
                oldAbsType = *dragEndPoint_->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef();
            }
            
            oldMode = dragEndPoint_->getBusInterface()->getInterfaceMode();
            oldName = dragEndPoint_->getBusInterface()->name();
        }

        // Define the bus for the end point.
        dragEndPoint_->setTypes(droppedVLNV, absdefVLNV, dialog.getSelectedMode());

        if (editName)
        {
            dragEndPoint_->getBusInterface()->setName(dialog.name());
        }

        dragEndPoint_->updateInterface();

        // Create an undo command.
        QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(dragEndPoint_, oldBusType, oldAbsType, oldMode, 
            oldName));
        getEditProvider()->addCommand(cmd);

        dragEndPoint_ = 0;
    }    
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::replaceComponentItemAt()
//-----------------------------------------------------------------------------
void HWDesignDiagram::replaceComponentItemAtPositionWith(QPointF position, QSharedPointer<Component> comp)
{
    // Replace the underlying component with the new one.
    HWComponentItem* oldCompItem = static_cast<HWComponentItem*>(getTopmostComponent(position));
    Q_ASSERT(oldCompItem != 0);

    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
        tr("Component instance '%1' is about to be replaced with an instance of %2. Continue and replace?").arg(
        oldCompItem->name(), comp->getVlnv().toString()), QMessageBox::Yes | QMessageBox::No, getParent());

    if (msgBox.exec() == QMessageBox::Yes)
    {
        // Create the component item.
        QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
        componentInstance->setInstanceName(createInstanceName(comp->getVlnv().getName()));

        QSharedPointer<ConfigurableVLNVReference> componentReference
            (new ConfigurableVLNVReference(comp->getVlnv()));
        componentInstance->setComponentRef(componentReference);

        GraphicsColumn* column = getLayout()->findColumnAt(oldCompItem->scenePos());
        HWComponentItem* newCompItem = new HWComponentItem(getLibraryInterface(), componentInstance, comp, column);

        // Perform the replacement.
        QSharedPointer<ReplaceComponentCommand> replaceCommand(new ReplaceComponentCommand(this, oldCompItem,
            newCompItem, false, false));

        connect(replaceCommand.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(replaceCommand.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider()->addCommand(replaceCommand);
        replaceCommand->redo();
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
        ComponentItem* item = getTopmostComponent(event->scenePos());

        GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));

        // If the item is a HW component, determine whether the component can be replaced with the dragged one.
        if (item != 0 && item->type() == HWComponentItem::Type && column != 0)
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
            if (column != 0 && column->getColumnDesc()->getAllowedItems() & dragCompType_)
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

        // Allow the drop event if the end point is undefined or there are no connections
        // and the encompassing component is unpackaged.
        if (dragEndPoint_ != 0 && dragEndPoint_->isBus() &&
            (dragEndPoint_->getBusInterface() == 0 || !dragEndPoint_->getBusInterface()->getBusType().isValid() || 
                (!dragEndPoint_->isConnected() && dragEndPoint_->encompassingComp() != 0 && 
                !dragEndPoint_->encompassingComp()->componentModel()->getVlnv().isValid())))
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
// Function: HWDesignDiagram::isHierarchicalComponent()
//-----------------------------------------------------------------------------
bool HWDesignDiagram::isHierarchicalComponent(QGraphicsItem* item) const
{
    HWComponentItem* component = qgraphicsitem_cast<HWComponentItem*>(item);
    if (component)
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
        (itemType == BusPortItem::Type || itemType == BusInterfaceItem::Type || 
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
        GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());

        pasteEnabled = (mimeData->imageData().canConvert<ColumnCollectionCopyData>() || 
            (column != 0 && mimeData->imageData().canConvert<ComponentCollectionCopyData>()) ||                                    
            (column != 0 && mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>()));
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
// Function: HWDesignDiagram::findOrCreateInterfaceExtensionGroup()
//-----------------------------------------------------------------------------
QSharedPointer<InterfaceGraphicsData> HWDesignDiagram::findOrCreateInterfaceExtensionGroup(
    QSharedPointer<Design> design, QString const& busInterfaceName)
{
    foreach (QSharedPointer<InterfaceGraphicsData> graphicsData, design->getInterfaceGraphicsData())
    {
        if (busInterfaceName == graphicsData->getName())
        {
            return graphicsData;
        }
    }

    QSharedPointer<InterfaceGraphicsData> dataGroup (new InterfaceGraphicsData(busInterfaceName));
    design->getVendorExtensions()->append(dataGroup);

    return dataGroup;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onSelected()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection != 0)
    {
        // Check if the selected item was a component.
        if (newSelection->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = qgraphicsitem_cast<HWComponentItem *>(newSelection);
            emit componentSelected(comp);
            emit helpUrlRequested("hwdesign/hwinstance.html");
        }
        // Check if the selected item was a port.
        else if (newSelection->type() == BusPortItem::Type)
        {
            BusPortItem* port = qgraphicsitem_cast<BusPortItem*>(newSelection);
            emit interfaceSelected(port);
            emit helpUrlRequested("hwdesign/busport.html");
        }
        // Check if the selected item was an interface.
        else if (newSelection->type() == BusInterfaceItem::Type)
        {
            BusInterfaceItem* interface = qgraphicsitem_cast<BusInterfaceItem*>(newSelection);
            emit interfaceSelected(interface);
            emit helpUrlRequested("hwdesign/businterface.html");
        }
        // Check if the selected item was an ad-hoc port.
        else if (newSelection->type() == AdHocPortItem::Type)
        {
            AdHocPortItem* adHocPort = static_cast<AdHocPortItem*>(newSelection);
            emit interfaceSelected(adHocPort);
            emit helpUrlRequested("hwdesign/adhocport.html");
        }
        // Check if the selected item was an ad-hoc interface.
        else if (newSelection->type() == AdHocInterfaceItem::Type)
        {
            AdHocInterfaceItem* adHocIf = static_cast<AdHocInterfaceItem*>(newSelection);
            emit interfaceSelected(adHocIf);
            emit helpUrlRequested("hwdesign/adhocinterface.html");
        }
		// check if the selected item was a connection
		else if (newSelection->type() == HWConnection::Type)
        {
			HWConnection* connection = qgraphicsitem_cast<HWConnection*>(newSelection);
			emit connectionSelected(connection);
            emit helpUrlRequested("hwdesign/connection.html");
		}
        else if (newSelection->type() == AdHocConnectionItem::Type)
        {
            AdHocConnectionItem* connection = qgraphicsitem_cast<AdHocConnectionItem*>(newSelection);
            emit connectionSelected(connection);
            emit helpUrlRequested("hwdesign/connection.html");
        }
        else
        {
            // inform others that nothing is currently selected
            emit clearItemSelection();
            emit helpUrlRequested("hwdesign/hwdesign.html");
        }
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();
        emit helpUrlRequested("hwdesign/hwdesign.html");
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
        QString connectionName = connection->createDefaultName();
        adHocConnection->setName(connectionName);
        route->setName(connectionName);
        connection->setLineWidth(1);
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

        HWConnection* hwConnection = new HWConnection(startPoint, endPoint, interconnection, route, this);    
        QString connectionName = hwConnection->createDefaultName();
        interconnection->setName(connectionName);
        route->setName(connectionName);
        hwConnection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));
        connection = hwConnection;
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
            QVector2D(0.0f, 0.0f), QString(), QString(), this);
    }
    else
    {
        HWConnection* connection = new HWConnection(startPoint->scenePos(), startPoint->getDirection(), endPoint,
            QVector2D(0.0f, 0.0f), QString(), QString(), this);
        connection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));
        return connection;
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAddCommandForConnection()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> HWDesignDiagram::createAddCommandForConnection(GraphicsConnection* connection)
{
    AdHocConnectionItem* adhocConnection = dynamic_cast<AdHocConnectionItem*>(connection);
    if (adhocConnection)
    {
        return QSharedPointer<QUndoCommand>(new AdHocConnectionAddCommand(this, adhocConnection, getDesign()));
    }

    HWConnection* hwConnection = dynamic_cast<HWConnection*>(connection);
    if (hwConnection)
    {
        return QSharedPointer<QUndoCommand>(new ConnectionAddCommand(this, hwConnection, getDesign()));
    }

    return QSharedPointer<QUndoCommand>();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addTopLevelInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addTopLevelInterface(GraphicsColumn* column, QPointF const& pos)
{
    QString draftInterfaceName = createNameForHierarchicalDraftInterface(QString("bus"));

    QSharedPointer<BusInterface> busif(new BusInterface());
    busif->setName(draftInterfaceName);

    QPointF newPosition = snapPointToGrid(pos);

    QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(busif->name()));
    dataGroup->setPosition(newPosition);
    getDesign()->getVendorExtensions()->append(dataGroup);

    BusInterfaceItem* newItem = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), busif, dataGroup);
    newItem->setPos(newPosition);

    connect(newItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

    // Save the positions of the other diagram interfaces.
    QMap<BusInterfaceItem*, QPointF> oldPositions;

    if (column->getContentType() == ColumnTypes::IO)
    {
        foreach (QGraphicsItem* item, column->childItems())
        {
            if (item->type() == BusInterfaceItem::Type)
            {
                BusInterfaceItem* interface = static_cast<BusInterfaceItem*>(item);
                oldPositions.insert(interface, interface->scenePos());
            }
        }
    }

    QSharedPointer<QUndoCommand> cmd(new ItemAddCommand(column, newItem));
    cmd->redo();

    // Determine if the other interfaces changed their position and create undo commands for them.
    if (column->getContentType() == ColumnTypes::IO)
    {
        for (QMap<BusInterfaceItem*, QPointF>::iterator cur = oldPositions.begin(); cur != oldPositions.end(); 
            cur++)
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), column, cmd.data());
            }
        }
    }

    getEditProvider()->addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createNameForHierarchicalDraftInterface()
//-----------------------------------------------------------------------------
QString HWDesignDiagram::createNameForHierarchicalDraftInterface(QString const& defaultName) const
{
    QString newName = defaultName;

    unsigned int busCount = 0;
    QStringList busNames;

    foreach (QGraphicsItem* graphicItem, items())
    {
        BusInterfaceItem* interfaceItem = dynamic_cast<BusInterfaceItem*>(graphicItem);
        if (interfaceItem)
        {
            busNames.append(interfaceItem->name());
        }
    }

    while (busNames.contains(newName))
    {
        busCount++;
        newName = "bus_" + QString::number(busCount);
    }

    return newName;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::draftAt()
//-----------------------------------------------------------------------------
void HWDesignDiagram::draftAt(QPointF const& clickedPosition)
{
    // Find the bottom-most item under the cursor.
    QGraphicsItem* clickedItem = 0;
    QList<QGraphicsItem*> itemList = items(clickedPosition);

    if (!itemList.empty())
    {
        clickedItem = itemList.back();
    }

    // If there was no item, then a new component/channel/bridge/interface should be added
    // to the column under cursor.
    if (clickedItem == 0)
    {
        GraphicsColumn* column = getLayout()->findColumnAt(clickedPosition);

        if (column != 0)
        {
            // Check what kind of item should be created.
            unsigned int itemType = findColumnItemType(column);

            // Create the item based on the selected/determined type.
            if (itemType == ColumnTypes::INTERFACE)
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
        HWComponentItem* component = static_cast<HWComponentItem*>(clickedItem);
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
    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
    comp->setVlnv(VLNV());
    comp->setImplementation(getEditedComponent()->getImplementation());

    // Create the corresponding diagram component.
    QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
    componentInstance->setInstanceName(name);

    HWComponentItem* diagComp = new HWComponentItem(getLibraryInterface(), componentInstance, comp, column);
    diagComp->setDraft();
    diagComp->setPos(snapPointToGrid(position));

    QSharedPointer<HWComponentAddCommand> addCommand(new HWComponentAddCommand(getDesign(), column, diagComp));

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
        QMap<BusPortItem*, QPointF> oldPositions;

        // Save old port positions.
        foreach (QGraphicsItem* item, targetComponent->childItems())
        {
            if (item->type() == BusPortItem::Type)
            {
                BusPortItem* port = static_cast<BusPortItem*>(item);
                oldPositions.insert(port, port->pos());
            }
        }

        QSharedPointer<QUndoCommand> cmd(new PortAddCommand(targetComponent, snapPointToGrid(position)));
        cmd->redo();

        // Create child undo commands for the ports with changed position.
        for (QMap<BusPortItem*, QPointF>::iterator current = oldPositions.begin(); 
            current != oldPositions.end(); current++)
        {
            if (current.key()->pos() != current.value())
            {
                new PortMoveCommand(current.key(), current.value(), cmd.data());
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
        QSharedPointer<ReplaceComponentCommand> cmd(new ReplaceComponentCommand(this,
            destHWComponent, sourceHWComponent, true, true));

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
        component = getLibraryInterface()->getModel(*instance->getComponentRef()).dynamicCast<Component>();

        if (!component && instance->getComponentRef()->isValid())
        {
            emit errorMessage(tr("Component %1 instantiated within design %2 was not found in the library").arg(
                instance->getComponentRef()->getName()).arg(design->getVlnv().getName()));
        }           
    }

    if(!component)
    {
        // Create an unpackaged component so that we can still visualize the component instance.
        component = QSharedPointer<Component>(new Component(*instance->getComponentRef()));
        component->setImplementation(KactusAttribute::HW);
    }
    
    HWComponentItem* item = new HWComponentItem(getLibraryInterface(), instance, component);

    // Check if the position is not found.
    // Migrate from the old layout to the column based layout.
    QPointF instancePosition = instance->getPosition();
    if (instancePosition.isNull())
    {
        getLayout()->addItem(item);
    }
    else
    {
        GraphicsColumn* column = getLayout()->findColumnAt(instancePosition);
        if (column != 0 && column->isItemAllowed(item))
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
void HWDesignDiagram::createComponentItem(QSharedPointer<Component> comp, QPointF position)
{
    GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(position));

    if (column != 0)
    {
        // Create the diagram component.                            
        QSharedPointer<ComponentInstance> componentInstance(new ComponentInstance());
        componentInstance->setComponentRef(
            QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(comp->getVlnv())));

        QString instanceName = createInstanceName(comp->getVlnv().getName());
        componentInstance->setInstanceName(instanceName);

        HWComponentItem *newItem = new HWComponentItem(getLibraryInterface(), componentInstance, comp);
        newItem->setPos(snapPointToGrid(position));

        QSharedPointer<HWComponentAddCommand> addCommand(new HWComponentAddCommand(getDesign(), column, newItem));

        connect(addCommand.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCommand.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider()->addCommand(addCommand);
        addCommand->redo();
    }
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

    HWConnection* connectionItem = new HWConnection(startPort, endPort, interconnection, route, this);
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
    connectionItem->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateRouteForInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> HWDesignDiagram::findOrCreateRouteForInterconnection(QString const& interconnectionName)
{
    foreach (QSharedPointer<ConnectionRoute> knownRoute, getDesign()->getRoutes())
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
        emit errorMessage(tr("Bus interface %1 was not found in the component %2").arg(busRef).arg(componentRef));
        interfaceItem = createMissingBusInterface(busRef, componentItem, design);
    }	

    return interfaceItem;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createMissingBusInterface()
//-----------------------------------------------------------------------------
BusPortItem* HWDesignDiagram::createMissingBusInterface(QString const& interfaceName,
    HWComponentItem* containingComponent, QSharedPointer<Design> design)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(interfaceName);

    BusPortItem* missingInterface = new BusPortItem(busIf, getLibraryInterface(), containingComponent);
    missingInterface->setTemporary(true);
    containingComponent->addPort(missingInterface);

    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
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
    HWComponentItem* startComponent = getComponentItem(startComponentRef);
    if (!startComponent)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(startComponentRef));
        return;
    }

    QSharedPointer<HierInterface> hierInterface = connection->getHierInterfaces()->first();
    ConnectionEndpoint* hierarchicalInterface = findOrCreateHierarchicalInterface(hierInterface->getBusReference());
    Q_ASSERT(hierarchicalInterface != 0);

    // Check if the interface item is already assigned to a column.
    GraphicsColumn* column = getLayout()->findColumnAt(hierarchicalInterface->scenePos());
    if (column != 0)
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
  
    HWConnection* connectionItem = new HWConnection(componentPort, hierarchicalInterface, connection, route, this);
    if (route->isOffpage())
    {
        connectionItem->hide();
    }

    componentPort->onConnect(hierarchicalInterface);
    hierarchicalInterface->onConnect(componentPort);

    connectionItem->validate();

    componentPort->addConnection(connectionItem);
    hierarchicalInterface->addConnection(connectionItem);

    connectionItem->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

    connect(connectionItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

    addItem(connectionItem);
    connectionItem->updatePosition();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::findOrCreateHierarchicalInterface()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWDesignDiagram::findOrCreateHierarchicalInterface(QString const& busRef)
{
    QSharedPointer<BusInterface> busIf = getEditedComponent()->getBusInterface(busRef);

    // if the bus interface was not found
    if (busIf != 0)
    {
        // Find the corresponding diagram interface.
        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == BusInterfaceItem::Type &&
                qgraphicsitem_cast<BusInterfaceItem*>(item)->getBusInterface() == busIf)
            {
                return qgraphicsitem_cast<BusInterfaceItem*>(item);
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

        ConnectionEndpoint* hierarchicalInterface = new BusInterfaceItem(getLibraryInterface(), 
            getEditedComponent(), busIf, dataGroup, 0);
        hierarchicalInterface->setTemporary(true);
        hierarchicalInterface->updateInterface();

        return hierarchicalInterface;
    }

    return 0;
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

    foreach (QSharedPointer<VendorExtension> positionExtension, adhocGroup->getByType("kactus2:adHocVisible"))
    {
        QSharedPointer<Kactus2Placeholder> adHocExtension = positionExtension.dynamicCast<Kactus2Placeholder>();
        QString portName = adHocExtension->getAttributeValue("portName");

        QSharedPointer<Port> adHocPort = getEditedComponent()->getPort(portName);

        AdHocInterfaceItem* adHocIf;

        if (adHocPort)
        {
            adHocIf = new AdHocInterfaceItem(getEditedComponent(), adHocPort, adHocExtension, 0);

            visiblePortNames.append(portName);
        }
        else
        {
            adHocIf = createMissingHierarchicalAdHocPort(portName, adHocExtension, 0);
        }

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        GraphicsColumn* column = getLayout()->findColumnAt(adHocIf->scenePos());
        if (column != 0 && column->isItemAllowed(adHocIf))
        {
            column->addItem(adHocIf, true);
        }
        else
        {
            getLayout()->addItem(adHocIf);
        }
    }

    foreach (QSharedPointer<Port> adhocPort, *getEditedComponent()->getPorts())
    {
        if (adhocPort->isAdHocVisible() && !visiblePortNames.contains(adhocPort->name()))
        {
            QSharedPointer<Kactus2Placeholder> positionPlaceHolder
                (new Kactus2Placeholder("kactus2:adHocVisible"));
            positionPlaceHolder->setAttribute("portName", adhocPort->name());

            adhocGroup->addToGroup(positionPlaceHolder);

            AdHocInterfaceItem* adhocInterface
                (new AdHocInterfaceItem(getEditedComponent(), adhocPort, positionPlaceHolder, 0));

            getLayout()->addItem(adhocInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createMissingHierarchicalAdHocPort()
//-----------------------------------------------------------------------------
AdHocInterfaceItem* HWDesignDiagram::createMissingHierarchicalAdHocPort(QString const& portName,
    QSharedPointer<Kactus2Placeholder> adHocExtension, QGraphicsItem* parentItem /*= (QGraphicsItem*)0*/)
{
    QSharedPointer<Port> missingPort (new Port(portName));

    AdHocInterfaceItem* missingInterface =
        new AdHocInterfaceItem(getEditedComponent(), missingPort, adHocExtension, parentItem);

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
    }

    else
    {
        // Convert one-to-many internal connections to one-to-one connections.
        if (adHocConnection->getExternalPortReferences()->empty() &&
            !adHocConnection->getInternalPortReferences()->empty())
        {
            QSharedPointer<PortReference> primaryPort = adHocConnection->getInternalPortReferences()->at(0);

            // Find the first referenced component.
            HWComponentItem* comp1 = getComponentItem(primaryPort->getComponentRef());

            if (comp1 == 0)
            {
                emit errorMessage(tr("Component %1 was not found in the design").
                    arg(primaryPort->getComponentRef()));
                return;
            }

            // Find the corresponding port.
            HWConnectionEndpoint* port1 = comp1->getAdHocPort(primaryPort->getPortRef());
            if (port1 == 0)
            {
                port1 = new AdHocPortItem(comp1->componentModel()->getPort(primaryPort->getPortRef()), comp1);
                comp1->addPort(port1);

                emit errorMessage(tr("Port %1 was not found in the component %2").arg(primaryPort->getPortRef(), 
                    primaryPort->getComponentRef()));

                comp1->setPortAdHocVisible(primaryPort->getPortRef(), true);
            }

            if (adHocConnection->isOffPage())
            {
                port1 = static_cast<HWConnectionEndpoint*>(port1->getOffPageConnector());
            }

            for (int i = 1; i < adHocConnection->getInternalPortReferences()->size(); ++i)
            {
                QSharedPointer<PortReference> secondaryPort = adHocConnection->getInternalPortReferences()->at(i);
                createConnectionForAdHocPorts(adHocConnection, secondaryPort, primaryPort, port1);
            }         
        }

        foreach (QSharedPointer<PortReference> externalPort, *adHocConnection->getExternalPortReferences())
        {
            ConnectionEndpoint* topAdHocPort = getDiagramAdHocPort(externalPort->getPortRef());
            if (topAdHocPort == 0)
            {
                if (!getEditedComponent()->hasPort(externalPort->getPortRef()))
                {
                    emit errorMessage(tr("Port %1 was not found in the top-level component").
                        arg(externalPort->getPortRef()));
                    return;
                }
                else
                {
                    setPortAdHocVisible(externalPort->getPortRef(), true);
                    topAdHocPort = getDiagramAdHocPort(externalPort->getPortRef());
                }
            }

            foreach (QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
            {
                createConnectionForAdHocPorts(adHocConnection, internalPort, externalPort, topAdHocPort);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAdHocTieOffConnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createAdHocTieOffConnection(QSharedPointer<AdHocConnection> connection)
{
    AdHocItem* tieOffPort;

    if (!connection->getInternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> internalPort, *connection->getInternalPortReferences())
        {
            HWComponentItem* comp1 = getComponentItem(internalPort->getComponentRef());
            HWConnectionEndpoint* connectionPort = comp1->getAdHocPort(internalPort->getPortRef());

            if (connectionPort)
            {
                tieOffPort = dynamic_cast<AdHocItem*>(connectionPort);
            }
            else
            {
                tieOffPort = comp1->createAdhocItem(internalPort->getPortRef());
            }

            diagramResolver_->resolveAdhocTieOff(connection->getTiedValue(), tieOffPort);
        }
    }

    if (!connection->getExternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> externalPort, *connection->getExternalPortReferences())
        {
            HWConnectionEndpoint* topAdHocPort = getDiagramAdHocPort(externalPort->getPortRef());

            if (topAdHocPort)
            {
                tieOffPort = dynamic_cast<AdHocItem*>(topAdHocPort);
            }
            else
            {
                tieOffPort = createAdhocItem(externalPort->getPortRef());
            }

            diagramResolver_->resolveAdhocTieOff(connection->getTiedValue(), tieOffPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnectionForAdHocPorts()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createConnectionForAdHocPorts(QSharedPointer<AdHocConnection> adHocConnection, 
    QSharedPointer<PortReference> internalPort, QSharedPointer<PortReference> primaryPort, 
    ConnectionEndpoint* primaryPortItem)
{
    // Find the referenced component.
    HWComponentItem* componentItem = getComponentItem(internalPort->getComponentRef());
    if (componentItem == 0)
    {
        emit errorMessage(tr("Component %1 was not found in the design").arg(internalPort->getComponentRef()));
        return;
    }

    HWConnectionEndpoint* adHocPort = componentItem->getAdHocPort(internalPort->getPortRef());

    if (adHocPort == 0)
    {
        adHocPort = new AdHocPortItem(componentItem->componentModel()->getPort(internalPort->getPortRef()), 
            componentItem);
        componentItem->addPort(adHocPort);
        componentItem->setPortAdHocVisible(internalPort->getPortRef(), true);
    }

    QSharedPointer<ConnectionRoute> route = findOrCreateRouteForInterconnection(adHocConnection->name());

    if (route->isOffpage())
    {
        adHocPort = static_cast<HWConnectionEndpoint*>(adHocPort->getOffPageConnector());

        primaryPortItem = primaryPortItem->getOffPageConnector();
    }

    // Create the ad-hoc connection graphics item.
    AdHocConnectionItem* connection =
        new AdHocConnectionItem(primaryPortItem, adHocPort, adHocConnection, route, this);

    connection->setRoute(adHocConnection->getRoute());

    QSharedPointer<PartSelect> internalPart = internalPort->getPartSelect();
    if (!internalPart.isNull())
    {
        connection->setAdHocLeftBound(0, internalPart->getLeftRange());
        connection->setAdHocRightBound(0, internalPart->getRightRange());
    }

    QSharedPointer<PartSelect> primaryPart = primaryPort->getPartSelect();
    if (!primaryPart.isNull())
    {
        connection->setAdHocLeftBound(1, primaryPart->getLeftRange());
        connection->setAdHocRightBound(1, primaryPart->getRightRange());
    }

    if (route->isOffpage())
    {
        connection->hide();
    }
    connection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

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
    foreach (QGraphicsItem* item, items)
    {
        if (item->type() == BusInterfaceItem::Type || item->type() == BusPortItem::Type)
        {
            BusInterfaceItem* busPort = static_cast<BusInterfaceItem*>(item);

            collection.instances.append(BusInterfaceCopyData());
            BusInterfaceCopyData& instance = collection.instances.back();

            instance.srcComponent = busPort->getOwnerComponent();
            instance.busInterface = busPort->getBusInterface();
            instance.position = busPort->pos();
            instance.topLevelIf = item->type() == BusInterfaceItem::Type;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyInstances()
//-----------------------------------------------------------------------------
void HWDesignDiagram::copyInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection)
{
    // Create instance copies.
    foreach (QGraphicsItem* item, items)
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
    foreach(BusInterfaceCopyData const& instance, collection.instances)
    {        
        // Bus interface must have a unique name within the component.
        QString uniqueBusName = instance.busInterface->name();        	
        unsigned int count =  0;	
        while (component->getBusPort(uniqueBusName) != 0)
        {
            count++;
            uniqueBusName = instance.busInterface->name() + "_" + QString::number(count);			
        }

        // Create a copy of the busInterface and rename it.
        QSharedPointer<BusInterface> copyBusIf(new BusInterface(*instance.busInterface));
        copyBusIf->setName(uniqueBusName);

        // Create a bus port with the copied bus interface.
        BusPortItem* port = new BusPortItem(copyBusIf, getLibraryInterface(), component);     
        port->setTemporary(true);

        port->setPos(snapPointToGrid(component->mapFromScene(contextMenuPosition())));
        
        // Lock the interface type for non-draft interfaces.
        if (copyBusIf->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
        {
            port->setTypeLocked(true);
        }

        QMap<QString, QPointF> oldLocations = component->getBusInterfacePositions();

        PortPasteCommand* pasteCmd = new PortPasteCommand(component, instance.srcComponent, port, cmd);
        pasteCmd->redo();

        QMap<QString, QPointF>::iterator cur = oldLocations.begin();

        while (cur != oldLocations.end())
        {
            if (component->getBusPort(cur.key())->pos() != cur.value())
            {
                new PortMoveCommand(component->getBusPort(cur.key()), cur.value(), pasteCmd);
            }
            cur++;
        }  
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteTopLevelInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteTopLevelInterfaces(BusInterfaceCollectionCopyData const& collection,
    GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos)
{
    if (collection.instances.isEmpty())
    {
        return;
    }

    QStringList existingNames;
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == BusInterfaceItem::Type)
        {
            BusInterfaceItem* interface = static_cast<BusInterfaceItem*>(item);
            existingNames.append(interface->name());
        }
    }

    foreach (BusInterfaceCopyData const& instance, collection.instances)
    {
        // Bus interface must have a unique name within the component.
        QString uniqueBusName = instance.busInterface->name();
        unsigned int count = 0;

        while (existingNames.contains(uniqueBusName))
        {
            count++;
            uniqueBusName = instance.busInterface->name() + "_" + QString::number(count);			
        }

        existingNames.append(uniqueBusName);

        GraphicsColumn* targetColumn = column;
        // Check if the column is not for IO.
        if (!targetColumn->getContentType() == ColumnTypes::IO)
        {
            targetColumn = 0;

            // Find the first column that is.
            foreach (GraphicsColumn* otherColumn, getLayout()->getColumns())
            {
                if (otherColumn->getContentType() == ColumnTypes::IO)
                {
                    targetColumn = otherColumn;
                    break;
                }
            }
        }

        if (targetColumn != 0)
        {          
            // Create a copy of the busInterface and rename it.
            QSharedPointer<BusInterface> copyBusIf(new BusInterface(*instance.busInterface));
            copyBusIf->setName(uniqueBusName);

            BusInterfaceItem* port = 0;

            // If interface is copied from an component instance to top-level, ask user for the port names in 
            // top-level.
            BusInterfacePasteCommand* pasteCmd = 0;            
            if (!instance.topLevelIf && instance.busInterface->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
            {
                BusInterfaceDialog dialog(false, getParent());
                dialog.addMode(instance.busInterface->getInterfaceMode());
                dialog.setBusInterfaces(instance.srcComponent, instance.busInterface, getEditedComponent(), 
                    getLibraryInterface());
                if (dialog.exec() == QDialog::Accepted)
                {                 
                    QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(copyBusIf->name()));

                    copyBusIf->getPortMaps()->append(dialog.getPortMaps());

                    // Create a busPort with the copied bus interface.
                    port = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), copyBusIf, 
                        dataGroup, targetColumn);
                    pasteCmd = new BusInterfacePasteCommand(instance.srcComponent, getEditedComponent(), port, 
                        targetColumn, this, dialog.getPorts(), cmd);
                }
                else
                {
                    return;
                }
            }
            else
            {
                QSharedPointer<InterfaceGraphicsData> dataGroup(new InterfaceGraphicsData(copyBusIf->name()));

                // Create a busPort with the copied bus interface.
                port = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), copyBusIf, 
                    dataGroup, targetColumn);
                pasteCmd = new BusInterfacePasteCommand(instance.srcComponent, getEditedComponent(), port,
                    targetColumn, this, cmd); 
            }

            if (useCursorPos)
            {
                port->setPos(findCursorPositionMappedToScene());
            }
            else
            {
                port->setPos(instance.position);
            }

            connect(port, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

            // Store the positions of other interfaces.
            QMap<BusInterfaceItem*, QPointF> oldPositions;
            foreach (QGraphicsItem* item, targetColumn->childItems())
            {
                if (item->type() == BusInterfaceItem::Type)
                {
                    BusInterfaceItem* interface = static_cast<BusInterfaceItem*>(item);
                    oldPositions.insert(interface, interface->scenePos());
                }
            }

            // Paste interface to component.
            pasteCmd->redo();

            // Determine if the other interfaces changed their position and create undo commands for them.
            QMap<BusInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

            while (cur != oldPositions.end())
            {
                if (cur.key()->scenePos() != cur.value())
                {
                    new ItemMoveCommand(cur.key(), cur.value(), targetColumn, pasteCmd);
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
    foreach (ComponentInstanceCopyData const& instance, collection.instances)
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

    AdHocInterfaceItem* adHocIf;

    QSharedPointer<Port> adhocPort = getEditedComponent()->getPort(portName);
    if (adhocPort)
    {
        adHocIf = new AdHocInterfaceItem(getEditedComponent(), adhocPort, adhocData, 0);
    }
    else
    {
        adHocIf = createMissingHierarchicalAdHocPort(portName, adhocData, 0);
    }

    return adHocIf;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::showAdhocPort()
//-----------------------------------------------------------------------------
void HWDesignDiagram::showAdhocPort(AdHocItem* portItem)
{
    AdHocInterfaceItem* interfaceItem = dynamic_cast<AdHocInterfaceItem*>(portItem);
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
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::hideAdhocPort()
//-----------------------------------------------------------------------------
void HWDesignDiagram::hideAdhocPort(AdHocItem* portItem)
{
    QSharedPointer<VendorExtension> adhocExtension = getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();

    AdHocInterfaceItem* interfaceItem = dynamic_cast<AdHocInterfaceItem*>(portItem);
    if (adhocGroup && interfaceItem)
    {
        static_cast<GraphicsColumn*>(interfaceItem->parentItem())->removeItem(interfaceItem);

        foreach(QSharedPointer<VendorExtension> extension, adhocGroup->getByType("kactus2:adHocVisible"))
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
