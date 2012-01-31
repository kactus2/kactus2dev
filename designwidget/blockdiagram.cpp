/* 
 *
 * 		filename: blockdiagram.cpp
 */

#include "blockdiagram.h"
#include "diagramcomponent.h"
#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagraminterface.h"
#include "DiagramOffPageConnector.h"
#include "SelectItemTypeDialog.h"
#include "designwidget.h"
#include "DiagramAddCommands.h"
#include "DiagramDeleteCommands.h"
#include "DiagramMoveCommands.h"
#include "DiagramChangeCommands.h"
#include "BusInterfaceDialog.h"

#include <common/DiagramUtil.h>
#include <common/diagramgrid.h>
#include <common/dialogs/comboSelector/comboselector.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/GenericEditProvider.h>

#include "columnview/DiagramColumnLayout.h"
#include "columnview/DiagramColumn.h"

#include <LibraryManager/libraryhandler.h>

#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/portabstraction.h>
#include <models/generaldeclarations.h>
#include <models/model.h>
#include <models/modelparameter.h>
#include <models/channel.h>
#include <models/designconfiguration.h>
#include <models/busdefinition.h>

#include <models/design.h>
#include <models/port.h>
#include <models/view.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QUuid>
#include <QMimeData>
#include <QString>
#include <QMessageBox>
#include <QCoreApplication>
#include <QPrinter>

#include <QDebug>
#include "columnview/ColumnEditDialog.h"

BlockDiagram::BlockDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, DesignWidget *parent) : 
    QGraphicsScene(parent), 
    lh_(lh),
    parent_(parent),
    mode_(MODE_SELECT),
    tempConnection_(0),
    tempConnEndPoint_(0), 
    highlightedEndPoint_(0),
    instanceNames_(),
    component_(),
    designConf_(),
    dragCompType_(CIT_NONE),
    dragBus_(false),
    editProvider_(editProvider),
    locked_(false),
    offPageMode_(false),
    oldSelection_(0)
{
    setSceneRect(0, 0, 100000, 100000);

    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

    //connect(this, SIGNAL(sceneRectChanged(const QRectF&)),
	//	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

bool BlockDiagram::setDesign(QSharedPointer<Component> hierComp, const QString& viewName)
{
    Q_ASSERT_X(hierComp, "BlockDiagram::setDesign()",
		"Null component pointer given as parameter");

    // Clear the edit provider.
	editProvider_.clear();

	// clear the previous design configuration
	designConf_.clear();

	foreach (QGraphicsItem* item, items()) {

		// if the item is an interconnection
		if (item->type() == DiagramInterconnection::Type) {
			removeItem(item);
			delete item;
		}
	}

    clear();
    component_ = hierComp;

    VLNV designVLNV = hierComp->getHierRef(viewName);

	// the received type is always VLNV::DESIGN so it must be asked from the
	// library handler to make sure the type is correct.
	designVLNV.setType(lh_->getDocumentType(designVLNV));

	if (!designVLNV.isValid()) {
		emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(
		hierComp->getVlnv()->getName()));
		return false;
	}

	QSharedPointer<Design> design;

	// if the component contains a direct reference to a design
	if (designVLNV.getType() == VLNV::DESIGN) {
		QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
		design = libComp.staticCast<Design>();
	}
	
	// if component had reference to a design configuration
	else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION) {
		QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);
		designConf_ = libComp.staticCast<DesignConfiguration>();

		designVLNV = designConf_->getDesignRef();

		if (designVLNV.isValid()) {
			QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
			design = libComp.staticCast<Design>();
		}
		
		// if design configuration did not contain a reference to a design.
		if (!design) {
			emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(
				hierComp->getVlnv()->getName()));
			return false;
		}
	}

	// if referenced view was not found
	else {
		emit errorMessage(tr("The hierarchical design %1 referenced within "
		"component %2 was not found in the library").arg(designVLNV.getName()).arg(
		hierComp->getVlnv()->getName()));
		return false;
	}

    // Create the column layout.
    layout_ = QSharedPointer<DiagramColumnLayout>(new DiagramColumnLayout(editProvider_, this));
    connect(layout_.data(), SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    if (design->getColumns().isEmpty())
    {
        if (component_->getComponentImplementation() == KactusAttribute::KTS_SW &&
            component_->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM)
        {
            layout_->addColumn("Low-level", COLUMN_CONTENT_COMPONENTS);
            layout_->addColumn("Middle-level", COLUMN_CONTENT_COMPONENTS);
            layout_->addColumn("High-level", COLUMN_CONTENT_COMPONENTS);
            layout_->addColumn("Out", COLUMN_CONTENT_IO);
        }
        else
        {
            layout_->addColumn("IO", COLUMN_CONTENT_IO);
            layout_->addColumn("Buses", COLUMN_CONTENT_BUSES);
            layout_->addColumn("Components", COLUMN_CONTENT_COMPONENTS);
            layout_->addColumn("IO", COLUMN_CONTENT_IO);
        }
    }
    else
    {
        foreach(Design::ColumnDesc desc, design->getColumns())
        {
            layout_->addColumn(desc.name, desc.contentType, desc.allowedItems);
        }
    }

    // Create diagram interfaces for the top-level bus interfaces.
    foreach (QSharedPointer<BusInterface> busIf, hierComp->getBusInterfaces())
    {
        DiagramInterface* diagIf = new DiagramInterface(lh_, component_, busIf, 0);
        connect(diagIf, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        // Add the diagram interface to the first column where it is allowed to be placed.
        layout_->addItem(diagIf);
    }

    /* component instances */
    foreach (Design::ComponentInstance instance,
             design->getComponentInstances()) {
        
		QSharedPointer<LibraryComponent> libComponent = lh_->getModel(instance.componentRef);

		if (!libComponent) {
			emit errorMessage(tr("The component %1 instantiated within design "
				"%2 was not found within library").arg(
				instance.componentRef.getName()).arg(design->getVlnv()->getName()));
			continue;
		}

        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        DiagramComponent* diagComp = new DiagramComponent(lh_, component, instance.instanceName,
                                                          instance.displayName, instance.description,
                                                          instance.configurableElementValues);

        connect(diagComp, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        // Setup the custom port positions.
        QMapIterator<QString, QPointF> itrPortPos(instance.portPositions);

        while (itrPortPos.hasNext())
        {
            itrPortPos.next();
            DiagramPort* port = diagComp->getBusPort(itrPortPos.key());

            if (port != 0)
            {
                port->setPos(itrPortPos.value());
                diagComp->onMovePort(port);
            }
        }

        // Check if the position is not found.
        if (instance.position.isNull())
        {
            // Migrate from the old layout to the column based layout.
            layout_->addItem(diagComp);
        }
        else
        {
            diagComp->setPos(instance.position);

            DiagramColumn* column = layout_->findColumnAt(instance.position);
            
            if (column != 0)
            {
                column->addItem(diagComp, true);
            }
            else
            {
                layout_->addItem(diagComp);
            }
        }

		// add the name to the list of instance names so no items with same 
		// names are added
		instanceNames_.append(instance.instanceName);
    }

    /* interconnections */
    foreach(Design::Interconnection interconnection,
            design->getInterconnections()) {

		// find the first referenced component
        DiagramComponent *comp1 = getComponent(interconnection.interface1.componentRef);

		if (!comp1) {
			emit errorMessage(tr("Component %1 was not found within the design").arg(
			interconnection.interface1.componentRef));
			continue;
		}
		
		// find the second referenced component
        DiagramComponent *comp2 = getComponent(interconnection.interface2.componentRef);

		if (!comp2) {
			emit errorMessage(tr("Component %1 was not found within the design").arg(
			interconnection.interface2.componentRef));
			continue;
		}

		// find the port of the first referenced component
        DiagramConnectionEndPoint* port1 = comp1->getBusPort(interconnection.interface1.busRef);

		if (!port1) {
			emit errorMessage(tr("Port %1 was not found within component %2").arg(
			interconnection.interface1.busRef).arg(interconnection.interface1.componentRef));
			continue;
		}
		
		// find the port of the second referenced component
        DiagramConnectionEndPoint* port2 = comp2->getBusPort(interconnection.interface2.busRef);

		if (!port2) {
			emit errorMessage(tr("Port %1 was not found within component %2").arg(
			interconnection.interface2.busRef).arg(interconnection.interface2.componentRef));
			continue;
		}

        if (interconnection.offPage)
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

		// if both components and their ports are found an interconnection can be
		// created
		if (comp1 && comp2 && port1 && port2) {
			DiagramInterconnection *diagramInterconnection =
				new DiagramInterconnection(port1, port2, true, interconnection.displayName,
                                           interconnection.description, this);
            diagramInterconnection->setRoute(interconnection.route);
			diagramInterconnection->setName(interconnection.name);

            if (interconnection.offPage)
            {
                diagramInterconnection->setVisible(false);
            }

            connect(diagramInterconnection, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
		}
    }

	// the hierarchical connections
    QList<QString> connectedHier;
    QList<Design::HierConnection> hierConnections = design->getHierarchicalConnections();
    for (int i = 0; i < hierConnections.size(); ++i) {

        Design::HierConnection hierConn = hierConnections.at(i);

		QSharedPointer<BusInterface> busIf = hierComp->getBusInterfaces().value(hierConn.interfaceRef);

		// if the bus interface was not found
		if (!busIf) {
			emit errorMessage(tr("Bus interface %1 was not found in top-component").arg(
				hierConn.interfaceRef));
			continue;
		}

        // Find the corresponding diagram interface.
        DiagramConnectionEndPoint* diagIf = 0;

        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == DiagramInterface::Type &&
                static_cast<DiagramInterface*>(item)->getBusInterface() == busIf)
            {
                diagIf = static_cast<DiagramInterface*>(item);
                break;
            }
        }

        Q_ASSERT(diagIf != 0);

		// Check if the position is found.
        if (!hierConn.position.isNull())
        {
            diagIf->setPos(hierConn.position);
            diagIf->setDirection(hierConn.direction);

            DiagramColumn* column = layout_->findColumnAt(hierConn.position);
            
            if (column != 0)
            {
                column->addItem(diagIf, true);
            }
            else
            {
                layout_->addItem(diagIf);
            }
        }

		// find the component where the hierarchical connection is connected to
        DiagramComponent *comp = getComponent(hierConn.interface_.componentRef);
		if (!comp) {
			emit errorMessage(tr("Component %1 was not found within top-design").arg(
				hierConn.interface_.componentRef));
			continue;
		}

		// find the port of the component
        DiagramConnectionEndPoint* compPort = comp->getBusPort(hierConn.interface_.busRef);
		if (!compPort)
        {
			emit errorMessage(tr("Port %1 was not found within component %2").arg(
				hierConn.interface_.busRef).arg(hierConn.interface_.componentRef));
		}
        // if both the component and it's port are found the connection can be made
		else
        {
            if (hierConn.offPage)
            {
                compPort = compPort->getOffPageConnector();
                diagIf = diagIf->getOffPageConnector();
            }

            DiagramInterconnection* diagConn = new DiagramInterconnection(compPort, diagIf, true,
                                                                          QString(), QString(), this);
            diagConn->setRoute(hierConn.route);

            if (hierConn.offPage)
            {
                diagConn->setVisible(false);
            }

            connect(diagConn, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
			connectedHier.append(hierConn.interfaceRef);
		}
    }

    return true;
}

BlockDiagram::~BlockDiagram() {
    
	foreach (QGraphicsItem* item, items()) {

		// if the item is an interconnection
		if (item->type() == DiagramInterconnection::Type) {
			removeItem(item);
			delete item;
		}
	}
}

DiagramComponent *BlockDiagram::getComponent(const QString &instanceName) {

	// search all graphicsitems in the scene
	foreach (QGraphicsItem *item, items()) {

		// if the item is a component
        if (item->type() == DiagramComponent::Type) {
            DiagramComponent *comp = qgraphicsitem_cast<DiagramComponent *>(item);

			// if component's name matches
            if (comp->name() == instanceName)
                return comp;
        }
    }

	// if no component was found
	emit errorMessage(tr("Component %1 was not found within design").arg(instanceName));
    return 0;
}

void BlockDiagram::setMode(DrawMode mode)
{
    if (mode_ != mode)
    {
        if (mode_ == MODE_CONNECT)
        {
            endConnect();
        }

        mode_ = mode;

        if (mode_ != MODE_SELECT)
        {
            clearSelection();
        }
        else
        {
            hideOffPageConnections();
        }

        emit modeChanged(mode);
    }
}

QSharedPointer<Design> BlockDiagram::createDesign(const VLNV &vlnv)
{
	QSharedPointer<Design> design(new Design(vlnv));

	QList<Design::ComponentInstance> instances;
	QList<Design::Interconnection> interconnections;
	QList<Design::HierConnection> hierConnections;
    QList<Design::ColumnDesc> columns;

    QList<QString> unpackagedNames;

	foreach (QGraphicsItem *item, items())
    {
		if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent *comp = qgraphicsitem_cast<DiagramComponent *>(item);

            // Check for unpackaged components.
            if (!comp->componentModel()->getVlnv()->isValid())
            {
                unpackagedNames.append(comp->name());
                continue;
            }

			Design::ComponentInstance instance(comp->name(), comp->displayName(),
				comp->description(), *comp->componentModel()->getVlnv(), comp->scenePos());

			// save the configurable element values to the design
			instance.configurableElementValues = comp->getConfigurableElements();

            // Save the port positions.
            QMapIterator< QString, QSharedPointer<BusInterface> >
                itrBusIf(comp->componentModel()->getBusInterfaces());

            while (itrBusIf.hasNext())
            {
                itrBusIf.next();
                instance.portPositions[itrBusIf.key()] = comp->getBusPort(itrBusIf.key())->pos();
            }

			instances.append(instance);
		} else if (item->type() == DiagramInterconnection::Type) {
			DiagramInterconnection *conn = qgraphicsitem_cast<DiagramInterconnection *>(item);

			if (conn->endPoint1()->encompassingComp() && conn->endPoint2()->encompassingComp())
            {
				Design::Interface iface1(conn->endPoint1()->encompassingComp()->name(),
					conn->endPoint1()->name());
				Design::Interface iface2(conn->endPoint2()->encompassingComp()->name(),
					conn->endPoint2()->name());
				interconnections.append(Design::Interconnection(conn->name(), iface1, iface2,
                                                                conn->route(),
                                                                conn->endPoint1()->type() == DiagramOffPageConnector::Type,
                                                                QString(),
																conn->description()));
			}
            else
            {
				DiagramConnectionEndPoint *compPort;
				DiagramConnectionEndPoint *hierPort;

                if (conn->endPoint1()->encompassingComp()) {
					compPort = conn->endPoint1();
					hierPort = conn->endPoint2();
				} else {
					compPort = conn->endPoint2();
					hierPort = conn->endPoint1();
				}

                if (hierPort->getBusInterface() != 0)
                {
                    hierConnections.append(
					    Design::HierConnection(hierPort->name(),
                                               Design::Interface(compPort->encompassingComp()->name(),
					                                             compPort->name()),
                                               hierPort->scenePos(), hierPort->getDirection(),
                                               conn->route(),
                                               conn->endPoint1()->type() == DiagramOffPageConnector::Type));
                }
			}
		}
	}

    // If there were unpacked components, show a message box and do not save.
    if (!unpackagedNames.empty())
    {
        QString detailMessage = tr("The following components must be packaged before the design can be saved:");

        foreach (QString const& name, unpackagedNames)
        {
            detailMessage += "\n * " + name;
        }

        DesignWidget* designWidget = static_cast<DesignWidget*>(parent());

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("Design ") + designWidget->getDocumentName() + 
                           tr(" cannot be saved because some component(s) are unpackaged."),
                           QMessageBox::Ok, (QWidget*)parent());
        msgBox.setDetailedText(detailMessage);
        msgBox.exec();
        return QSharedPointer<Design>();
    }

//     if (layout_ == 0)
//     {
//         layout_ = QSharedPointer<DiagramColumnLayout>(new DiagramColumnLayout(editProvider_, this));
//         layout_->addColumn("IO", COLUMN_CONTENT_IO);
//         layout_->addColumn("Buses", COLUMN_CONTENT_BUSES);
//         layout_->addColumn("Components", COLUMN_CONTENT_COMPONENTS);
//         layout_->addColumn("IO", COLUMN_CONTENT_IO);
//     }

    foreach(DiagramColumn* column, layout_->getColumns())
    {
        columns.append(Design::ColumnDesc(column->getName(), column->getContentType(),
                                          column->getAllowedItems()));
    }

	design->setComponentInstances(instances);
	design->setInterconnections(interconnections);
	design->setHierarchicalConnections(hierConnections);
    design->setColumns(columns);   

	return design;
}

void BlockDiagram::updateHierComponent(QSharedPointer<Component> comp) {

	// store all the bus interfaces to a map
    QMap<QString, QSharedPointer<BusInterface> > busIfs;

	// Search all graphics items in the scene.
    foreach (QGraphicsItem *item, items())
    {
		// Check if the item is a diagram interface and its bus interface is defined.
        DiagramInterface* diagIf = dynamic_cast<DiagramInterface*>(item);

        if (diagIf != 0 && diagIf->getBusInterface() != 0)
        {
            busIfs[diagIf->name()] = diagIf->getBusInterface();

            // Set the bus interface non-temporary.
            //diagIf->setTemporary(false);
        }
    }

    comp->setBusInterfaces(busIfs);
}

void BlockDiagram::selectionToFront()
{
    if (selectedItems().isEmpty())
        return;

    QGraphicsItem* selectedItem = selectedItems().first();

    if (selectedItem->type() == DiagramInterconnection::Type)
    {
        selectedItem->setZValue(-900);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // if other than left button was pressed return the mode back to select
	if (mouseEvent->button() != Qt::LeftButton)
    {
        setMode(MODE_SELECT);
        return;
	}

    if (mode_ == MODE_CONNECT)
    {
        bool creating = tempConnection_ != 0;

        // Check if we need to change the temporary connection into a persistent one.
        if (creating)
        {
            createConnection(mouseEvent);
        }
        // Otherwise choose a new start end point if in normal connection mode.
        else if (!offPageMode_)
        {
            offPageMode_ = (mouseEvent->modifiers() & Qt::ShiftModifier);

            // no items are selected if the mode is connect
            clearSelection();

            // Try to snap to a connection end point.
            DiagramConnectionEndPoint* endPoint =
                DiagramUtil::snapToItem<DiagramConnectionEndPoint>(mouseEvent->scenePos(), this, GridSize);

            if (endPoint == 0)
            {
                return;
            }

            if (offPageMode_)
            {
                if (highlightedEndPoint_ != 0)
                {
                    highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
                    highlightedEndPoint_ = 0;
                }

                if (endPoint->type() != DiagramOffPageConnector::Type)
                {
                    endPoint = endPoint->getOffPageConnector();
                    endPoint->setVisible(true);
                }
            }

            tempConnEndPoint_ = endPoint;
        }

        // Start drawing a new connection if in off page mode or we were not creating anything yet.
        if (offPageMode_ || !creating)
        {
            // Create the connection.
            tempConnection_ = new DiagramInterconnection(tempConnEndPoint_->scenePos(),
                tempConnEndPoint_->getDirection(),
                mouseEvent->scenePos(),
                QVector2D(0.0f, 0.0f), QString(), QString(), this);

            // Determine all potential end points to which the starting end point could be connected
            // and highlight them.
            foreach(QGraphicsItem* item, items())
            {
                DiagramConnectionEndPoint* endPoint = dynamic_cast<DiagramConnectionEndPoint*>(item);

                if (endPoint != 0 && endPoint->isVisible() && endPoint != tempConnEndPoint_ &&
                    endPoint->getOffPageConnector() != tempConnEndPoint_ &&
                    endPoint->canConnect(tempConnEndPoint_) && tempConnEndPoint_->canConnect(endPoint))
                {
                    tempPotentialEndingEndPoints_.append(endPoint);
                    endPoint->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_ALLOWED);
                }
            }
        }
    }
    else if (mode_ == MODE_INTERFACE)
    {
        // Find the column under the cursor.
        DiagramColumn* column = layout_->findColumnAt(mouseEvent->scenePos());

        // Add a new diagram interface to the column it it is allowed.
        if (column != 0 && column->getAllowedItems() & CIT_INTERFACE)
        {
            addInterface(column, mouseEvent->scenePos());
        }
    }
    else if (mode_ == MODE_DRAFT)
    {
        // Find the bottom-most item under the cursor.
        QGraphicsItem* item = 0;
        QList<QGraphicsItem*> itemList = items(mouseEvent->scenePos());
        
        if (!itemList.empty())
        {
            item = itemList.back();
        }

        // If there was no item, then a new component/channel/bridge/interface should be added
        // to the column under cursor.
        if (item == 0)
        {
            DiagramColumn* column = layout_->findColumnAt(mouseEvent->scenePos());
            
            if (column != 0)
            {
                // Check what kind of item should be created.
                unsigned int itemType = column->getAllowedItems();
                
                // Check if the item type is ambiguous (interface vs. the rest of the types).
                if (itemType != CIT_INTERFACE && itemType & CIT_INTERFACE)
                {
                    // Open a dialog to determine which type of item to create.
                    SelectItemTypeDialog dialog((QWidget*)parent(), column->getAllowedItems());

                    if (dialog.exec() != QDialog::Accepted)
                    {
                        return;
                    }

                    itemType = dialog.getSelectedItemType();
                }

                // Create the item based on the selected/determined type.
                if (itemType == CIT_INTERFACE)
                {
                    addInterface(column, mouseEvent->scenePos());
                }
                else
                {
                    // Determine an unused name for the component instance.
                    QString name = "unnamed";
                    int runningNumber = 0;
                    
                    while (instanceNames_.contains(name))
                    {
                        ++runningNumber;
                        name = "unnamed_" + QString::number(runningNumber);
                    }

                    instanceNames_.append(name);

                    // Create a component model without a valid vlnv.
                    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
                    comp->setVlnv(VLNV());
                    comp->setComponentImplementation(component_->getComponentImplementation());

                    if (component_->getComponentImplementation() == KactusAttribute::KTS_SW)
                    {
                        comp->setComponentSWType(component_->getComponentSWType());
                    }

                    // Create the corresponding diagram component.
                    DiagramComponent* diagComp = new DiagramComponent(lh_, comp, name);
                    diagComp->setPos(snapPointToGrid(mouseEvent->scenePos()));
                    connect(diagComp, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

                    QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, diagComp));

					connect(cmd.data(), SIGNAL(componentInstantiated(DiagramComponent*)),
						this, SIGNAL(componentInstantiated(DiagramComponent*)), Qt::UniqueConnection);
					connect(cmd.data(), SIGNAL(componentInstanceRemoved(DiagramComponent*)),
						this, SIGNAL(componentInstanceRemoved(DiagramComponent*)), Qt::UniqueConnection);

                    editProvider_.addCommand(cmd);
                }

                emit contentChanged();
            }
        }
        // Otherwise check if the item is an unpackaged component in which case
        // we can add a new port (bus interface) to it.
        else if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(item);

            // The component is unpackaged if it has an invalid vlnv.
            if (!comp->componentModel()->getVlnv()->isValid())
            {
                QMap<DiagramPort*, QPointF> oldPositions;

                // Save old port positions.
                foreach (QGraphicsItem* item, comp->childItems())
                {
                    if (item->type() == DiagramPort::Type)
                    {
                        DiagramPort* port = static_cast<DiagramPort*>(item);
                        oldPositions.insert(port, port->pos());
                    }
                }

                QSharedPointer<QUndoCommand> cmd(new PortAddCommand(comp, snapPointToGrid(mouseEvent->scenePos())));
                cmd->redo();
                
                // Create child undo commands for the changed ports.
                QMap<DiagramPort*, QPointF>::iterator cur = oldPositions.begin();

                while (cur != oldPositions.end())
                {
                    if (cur.key()->pos() != cur.value())
                    {
                        QUndoCommand* childCmd = new PortMoveCommand(cur.key(), cur.value(), cmd.data());
                    }

                    ++cur;
                }

                // Add the command to the edit stack.
                editProvider_.addCommand(cmd, false);
            }
        }
    }
    else if (mode_ == MODE_TOGGLE_OFFPAGE)
    {
        // Try to snap to a connection end point.
        DiagramConnectionEndPoint* endPoint =
            DiagramUtil::snapToItem<DiagramConnectionEndPoint>(mouseEvent->scenePos(), this, GridSize);

        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

        if (endPoint != 0 && endPoint->isVisible())
        {
            if (endPoint->getInterconnections().size() > 0)
            {
                hideOffPageConnections();

                foreach (DiagramInterconnection* conn, endPoint->getInterconnections())
                {
                    toggleConnectionStyle(conn, cmd.data());
                }
            }
        }
        else
        {
            QGraphicsItem* item = itemAt(mouseEvent->scenePos());

            if (item != 0 && item->type() == DiagramInterconnection::Type)
            {
                toggleConnectionStyle(static_cast<DiagramInterconnection*>(item), cmd.data());
            }
        }

        if (cmd->childCount() > 0)
        {
            editProvider_.addCommand(cmd, false);
        }
    }
    else if (mode_ == MODE_SELECT)
    {
        // Handle the mouse press and bring the new selection to front.
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Check if the connect mode is active.
    if (mode_ == MODE_CONNECT || mode_ == MODE_TOGGLE_OFFPAGE)
    {
        // Find out if there is an end point currently under the cursor.
        DiagramConnectionEndPoint* endPoint =
            DiagramUtil::snapToItem<DiagramConnectionEndPoint>(mouseEvent->scenePos(), this, GridSize);

        if (tempConnection_)
        {
            // Check if there was an valid endpoint close enough.
            if (endPoint != 0 && tempPotentialEndingEndPoints_.contains(endPoint))
            {
                // Highlight the end point.
                disableHighlight();                
                highlightedEndPoint_ = endPoint;
                highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_HOVER);

            }
            // Disable the highlight if there was no end point close enough.
            else
            {
                disableHighlight();
            }

            // Update the connection.
            Q_ASSERT(tempConnection_->route().size() != 0);

            DiagramInterconnection* newTempConnection_ = 0;

            if (highlightedEndPoint_ != 0)
            {
                 newTempConnection_ = new DiagramInterconnection(tempConnEndPoint_,
                                                                 highlightedEndPoint_, false,
                                                                 QString(), QString(), this);
            }
            else
            {
                newTempConnection_ = new DiagramInterconnection(tempConnEndPoint_->scenePos(),
                    tempConnEndPoint_->getDirection(), snapPointToGrid(mouseEvent->scenePos()),
                    QVector2D(0.0f, 0.0f), QString(), QString(), this);
            }

            removeItem(tempConnection_);
			delete tempConnection_;
			tempConnection_ = newTempConnection_;
			return;
        }
        else
        {
            if (highlightedEndPoint_ != 0)
            {
                highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
                highlightedEndPoint_ = 0;
            }

            if (endPoint != 0 && endPoint->isVisible())
            {
                highlightedEndPoint_ = endPoint;
                highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_HOVER);

                if (highlightedEndPoint_->type() == DiagramOffPageConnector::Type)
                {
                    hideOffPageConnections();

                    foreach (DiagramInterconnection* conn, highlightedEndPoint_->getInterconnections())
                    {
                        conn->setVisible(true);
                    }
                }
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void BlockDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	// Try to create a connection if in connection mode.
//     if (mode_ == MODE_CONNECT && tempConnection_ && !offPageMode_)
//     {
//         createConnection(mouseEvent);
//     }

	// process the normal mouse release event
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void BlockDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
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

    //clearSelection();

    // This fixes the problem when the user click above a text label or a pixmap but
    // actually wants to select the parent item (such as the actual component, not its label).
    if (item->parentItem() != 0 && item->type() != DiagramComponent::Type &&
        item->type() != DiagramPort::Type && item->type() != DiagramInterface::Type &&
        item->type() != DiagramColumn::Type)
    {
        item = item->parentItem();
    }

    if (item->type() == DiagramComponent::Type)
    {
        item->setSelected(true);
        DiagramComponent *comp = qgraphicsitem_cast<DiagramComponent *>(item);

        if (comp->componentModel()->getVlnv()->isValid())
        {
			QString viewName;
			QStringList hierViews = comp->componentModel()->getHierViews();
			
			// if configuration is used and it contains an active view for the instance
			if (designConf_ && designConf_->hasActiveView(comp->name())) {
				viewName = designConf_->getActiveView(comp->name());

				View* view = comp->componentModel()->findView(viewName);

				// if view was found and it is hierarchical
				if (view && view->isHierarchical()) {
					emit openDesign(*comp->componentModel()->getVlnv(), viewName);
				}
				// if view was found but it is not hierarchical
				else if (view && !view->isHierarchical()) {
					emit openComponent(*comp->componentModel()->getVlnv());
				}
				// if view was not found
				else {
					emit errorMessage(tr("The active view %1 was not found in "
						"instance %2").arg(viewName).arg(comp->name()));
				}
			}
			// if component does not contain any hierarchical views or contains
			// more than one hierarchical view then it is not known which view to open
			else if (hierViews.size() != 1) {

				emit noticeMessage(tr("No active view was selected for instance %1, "
					"opening component editor.").arg(comp->name()));
				// just open the component because view is ambiguous
				emit openComponent(*comp->componentModel()->getVlnv());
			}
			// if there is exactly one hierarchical view then open it
			else {
				emit noticeMessage(tr("No active view was selected for instance %1, "
					"opening the only hierarhical view of the component.").arg(comp->name()));
				emit openDesign(*comp->componentModel()->getVlnv(), hierViews.first());
			}
        }
        else
        {
            // Otherwise this is an unpackaged component. Check if the bus interfaces are valid.
            foreach (QSharedPointer<BusInterface> busIf, comp->componentModel()->getBusInterfaces())
            {
                if (!busIf->getBusType().isValid())
                {
                    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                       tr("Component '%1' cannot be packetized because all bus interfaces "
                                          "have not been defined.").arg(comp->name()),
                                       QMessageBox::Ok, (QWidget*)parent());
                    msgBox.setInformativeText(tr("Define first the buses by dragging from the library "
                                                 "or double clicking the bus interface."));
                    
                    msgBox.exec();
                    return;
                }
            }
            
            // Request the user to set the vlnv.
            NewObjectDialog dialog(lh_, VLNV::COMPONENT, true, (QWidget*)parent());
            dialog.setWindowTitle(tr("Add Component to Library"));

            if (dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            VLNV vlnv = dialog.getVLNV();
            comp->componentModel()->setVlnv(vlnv);
            comp->componentModel()->setComponentHierarchy(dialog.getProductHierarchy());
            comp->componentModel()->setComponentFirmness(dialog.getFirmness());

            // Write the model to file.
            lh_->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Update the diagram component.
            comp->updateComponent();

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new ComponentPacketizeCommand(comp, vlnv));
            editProvider_.addCommand(cmd, false);

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
    else if (item->type() == DiagramColumn::Type)
    {
        item->setSelected(true);
        DiagramColumn* column = qgraphicsitem_cast<DiagramColumn*>(item);

        ColumnEditDialog dialog((QWidget*)parent(), column);

        if (dialog.exec() == QDialog::Accepted)
        {
            QSharedPointer<QUndoCommand> cmd(new ColumnChangeCommand(column, dialog.getName(),
                                                                     dialog.getContentType(),
                                                                     dialog.getAllowedItems()));
            editProvider_.addCommand(cmd);

        }
    }
    else if (item->type() == DiagramPort::Type || item->type() == DiagramInterface::Type)
    {
        DiagramConnectionEndPoint* endPoint = static_cast<DiagramConnectionEndPoint*>(item);

        // Check if the bus is unpackaged.
        if (endPoint->getBusInterface() == 0 || !endPoint->getBusInterface()->getBusType().isValid())
        {
            // Request the user to set the vlnv.
            NewObjectDialog dialog(lh_, VLNV::BUSDEFINITION, true, (QWidget*)parent());
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
            if (lh_->contains(busVLNV))
            {
                emit errorMessage(tr("Bus definition %1:%2:%3:%4 was already found in library").arg(
                    busVLNV.getVendor()).arg(
                    busVLNV.getLibrary()).arg(
                    busVLNV.getName()).arg(
                    busVLNV.getVersion()));
                return;
            }
            else if (lh_->contains(absVLNV)) {
                emit errorMessage(tr("Abstraction definition %1:%2:%3:%4 was already found in library").arg(
                    absVLNV.getVendor()).arg(
                    absVLNV.getLibrary()).arg(
                    absVLNV.getName()).arg(
                    absVLNV.getVersion()));
                return;
            }

            // Create a bus definition.
            QSharedPointer<BusDefinition> busDef = QSharedPointer<BusDefinition>(new BusDefinition());
            busDef->setVlnv(busVLNV);

            // Create the file for the bus definition.
            lh_->writeModelToFile(dialog.getPath(), busDef);

            // Create an abstraction definition.
            QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
            absDef->setVlnv(absVLNV);

            // Set the reference from abstraction definition to bus definition.
            absDef->setBusType(busVLNV);

            // Create the file for the abstraction definition.
            lh_->writeModelToFile(dialog.getPath(), absDef);

            // Ask the user for the interface mode.
            endPoint->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_HOVER);

            bool editName = endPoint->type() == DiagramInterface::Type &&
                            endPoint->getInterconnections().empty();

            BusInterfaceDialog modeDialog(editName, (QWidget*)parent());
            modeDialog.addMode(General::MIRROREDMASTER);
            modeDialog.addMode(General::SLAVE);
            modeDialog.addMode(General::MIRROREDSLAVE);
            modeDialog.addMode(General::SYSTEM);
            modeDialog.addMode(General::MIRROREDSYSTEM);
            modeDialog.addMode(General::MASTER);

            if (modeDialog.exec() == QDialog::Rejected)
            {
                endPoint->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
                return;
            }

            endPoint->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);

            // Save old settings.
            VLNV oldBusType;
            VLNV oldAbsType;
            General::InterfaceMode oldMode = General::MASTER;
            QString oldName = "";

            if (endPoint->getBusInterface() != 0)
            {
                oldBusType = endPoint->getBusInterface()->getBusType();
                oldAbsType = endPoint->getBusInterface()->getAbstractionType();
                oldMode = endPoint->getBusInterface()->getInterfaceMode();
                oldName = endPoint->getBusInterface()->getName();
            }

            // Set the types for the end point.
            endPoint->setTypes(busVLNV, absVLNV, modeDialog.getSelectedMode());

            if (editName)
            {
                endPoint->getBusInterface()->setName(modeDialog.getName());
            }

            endPoint->updateInterface();

            QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(endPoint, oldBusType,
                                                                      oldAbsType, oldMode, oldName));
            editProvider_.addCommand(cmd, false);

            // Select the end point (to update the property widget contents).
            if (endPoint->type() == DiagramPort::Type)
            {
                emit portSelected(static_cast<DiagramPort*>(endPoint));
            }
            else
            {
                emit interfaceSelected(static_cast<DiagramInterface*>(endPoint));
            }

            QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                               "Do you want to continue defining the bus?",
                               QMessageBox::NoButton, (QWidget*)parent());
            msgBox.setInformativeText("Pressing Continue opens up the bus editor.");
            QPushButton* btnContinue = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
            msgBox.addButton(tr("Skip"), QMessageBox::RejectRole);

            msgBox.exec();

            // Open the user clicked continue, open up the bus editor.
            if (msgBox.clickedButton() == btnContinue)
            {
                emit openBus(busVLNV, absVLNV, false);
            }
        }
    }
}

void BlockDiagram::drawBackground(QPainter* painter, const QRectF& rect)
{
    if (dynamic_cast<QPrinter*>(painter->device()) == 0)
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
}

//-----------------------------------------------------------------------------
// Function: dragEnterEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragCompType_ = CIT_NONE;
    dragBus_ = false;

    // Allow drag only if the diagram is not locked and the dragged object is a vlnv.
    if (!locked_ && event->mimeData()->hasFormat("data/vlnvptr"))
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

            // Check if the implementation does not match with the edited component.
            if (comp->getComponentImplementation() != component_->getComponentImplementation())
            {
                return;
            }

            if (comp->isBridge())
            {
                dragCompType_ = CIT_BRIDGE;
            }
            else if (comp->isChannel())
            {
                dragCompType_ = CIT_CHANNEL;
            }
            else
            {
                dragCompType_ = CIT_COMPONENT;
            }
            comp.clear();
        }
        else if (vlnv->getType() == VLNV::BUSDEFINITION || 
			vlnv->getType() == VLNV::ABSTRACTIONDEFINITION)
        {
            // Check that the bus definition is compatible with the edited component.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(*vlnv);
            QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();

            if ((component_->getComponentImplementation() == KactusAttribute::KTS_HW &&
                 busDef->getType() == KactusAttribute::KTS_BUSDEF_HW) ||
                (component_->getComponentImplementation() == KactusAttribute::KTS_SW &&
                 busDef->getType() == KactusAttribute::KTS_BUSDEF_API))
            {
                dragBus_ = true;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragMoveEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    if (dragCompType_ != CIT_NONE)
    {
        DiagramColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

        if (column != 0 && column->getAllowedItems() & dragCompType_)
        {
            event->setDropAction(Qt::CopyAction);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }

        event->accept();
    }
    else if (dragBus_)
    {
        // Check if there is a connection endpoint close enough the cursor.
        DiagramConnectionEndPoint* endPoint =
            DiagramUtil::snapToItem<DiagramConnectionEndPoint>(event->scenePos(), this, GridSize);

        if (highlightedEndPoint_ != 0)
        {
            highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
        }
        
        highlightedEndPoint_ = endPoint;

        // Allow the drop event if the end point is undefined or there are no connections
        // and the encompassing component is unpackaged.
        if (highlightedEndPoint_ != 0 &&
            (highlightedEndPoint_->getBusInterface() == 0 ||
             !highlightedEndPoint_->getBusInterface()->getBusType().isValid() ||
             (!highlightedEndPoint_->isConnected() &&
              highlightedEndPoint_->encompassingComp() != 0 &&
              !highlightedEndPoint_->encompassingComp()->componentModel()->getVlnv()->isValid())))
        {
            event->setDropAction(Qt::CopyAction);
            highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_HOVER);
        }
        else
        {
            event->setDropAction(Qt::IgnoreAction);
        }
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

//-----------------------------------------------------------------------------
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragCompType_ = CIT_NONE;
    dragBus_ = false;
    
    if (highlightedEndPoint_ != 0)
    {
        highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
        highlightedEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Check if the dragged item was a valid one.
    if (dragCompType_ != CIT_NONE)
    {
        DiagramColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

        // Retrieve the vlnv.
        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        // Disallow self-instantiation.
        if (*vlnv == *component_->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Component cannot be instantiated to its own design."),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }

        // Create the component model.
		QSharedPointer<LibraryComponent> libComp = lh_->getModel(*vlnv);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

		// Set the instance name for the new component instance.
		QString instanceName = createInstanceName(comp);

        // Create the diagram component.
        DiagramComponent *newItem = new DiagramComponent(lh_, comp, instanceName);
        newItem->setPos(snapPointToGrid(event->scenePos()));
        connect(newItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (column != 0)
        {
            QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, newItem));

			connect(cmd.data(), SIGNAL(componentInstantiated(DiagramComponent*)),
				this, SIGNAL(componentInstantiated(DiagramComponent*)), Qt::UniqueConnection);
			connect(cmd.data(), SIGNAL(componentInstanceRemoved(DiagramComponent*)),
				this, SIGNAL(componentInstanceRemoved(DiagramComponent*)), Qt::UniqueConnection);

            editProvider_.addCommand(cmd);
        }
    }
    else if (dragBus_)
    {
        if (highlightedEndPoint_ != 0)
        {
            // Retrieve the busdef VLNV and determine the absdef VLNV.
            VLNV *droppedVlnv;
            memcpy(&droppedVlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(droppedVlnv));

			Q_ASSERT(lh_->contains(*droppedVlnv));

            VLNV vlnv = *droppedVlnv;
			vlnv.setType(lh_->getDocumentType(*droppedVlnv));

			VLNV absdefVLNV;

			// if the dropped was an abstraction definition
			if (vlnv.getType() == VLNV::ABSTRACTIONDEFINITION) {
				absdefVLNV = vlnv;

				// parse the abstraction definition
				QSharedPointer<LibraryComponent> libComp = lh_->getModel(vlnv);
				QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

				// get the bus definition referenced by the abstraction definition
				VLNV busdefVLNV = absDef->getBusType();

				// set the bus definition used
				vlnv = busdefVLNV;

				Q_ASSERT(absdefVLNV.getType() == VLNV::ABSTRACTIONDEFINITION);
			}
			Q_ASSERT(vlnv.getType() == VLNV::BUSDEFINITION);

            bool editName = highlightedEndPoint_->type() == DiagramInterface::Type &&
                            highlightedEndPoint_->getBusInterface() == 0 &&
                            highlightedEndPoint_->getInterconnections().empty();

            // Ask the user for the interface mode.
            BusInterfaceDialog dialog(editName, (QWidget*)parent());
            dialog.addMode(General::MIRROREDMASTER);
            dialog.addMode(General::SLAVE);
            dialog.addMode(General::MIRROREDSLAVE);
            dialog.addMode(General::SYSTEM);
            dialog.addMode(General::MIRROREDSYSTEM);
            dialog.addMode(General::MASTER);

            if (dialog.exec() == QDialog::Rejected)
            {
                highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
                return;
            }

            highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);

            // Save old settings.
            VLNV oldBusType;
            VLNV oldAbsType;
            General::InterfaceMode oldMode = General::MASTER;
            QString oldName = "";

            if (highlightedEndPoint_->getBusInterface() != 0)
            {
                oldBusType = highlightedEndPoint_->getBusInterface()->getBusType();
                oldAbsType = highlightedEndPoint_->getBusInterface()->getAbstractionType();
                oldMode = highlightedEndPoint_->getBusInterface()->getInterfaceMode();
                oldName = highlightedEndPoint_->getBusInterface()->getName();
            }

            // Define the bus for the end point.
            highlightedEndPoint_->setTypes(vlnv, absdefVLNV, dialog.getSelectedMode());
            
            if (editName)
            {
                highlightedEndPoint_->getBusInterface()->setName(dialog.getName());
            }

            highlightedEndPoint_->updateInterface();

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(highlightedEndPoint_,
                                                                      oldBusType, oldAbsType,
                                                                      oldMode, oldName));
            editProvider_.addCommand(cmd, false);

            highlightedEndPoint_ = 0;
        }
    }
}

QString BlockDiagram::createInstanceName( QSharedPointer<Component> component ) {
	int runningNumber = 1;
	QString instanceName = component->getVlnv()->getName();
	instanceName.remove(QString(".comp"));
	QString tempName = instanceName;
	tempName += "_";
	tempName += QString::number(runningNumber);

	// increase the number as long as there is another instance with same name
	while (instanceNames_.contains(tempName)) {
		++runningNumber;
		tempName = instanceName;
		tempName += "_";
		tempName += QString::number(runningNumber);
	}

	// when temp name is unique
	instanceNames_.append(tempName);
	return tempName;
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void BlockDiagram::onVerticalScroll(qreal y)
{
	Q_ASSERT(layout_);
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: disableHighlight()
//-----------------------------------------------------------------------------
void BlockDiagram::disableHighlight()
{
    if (highlightedEndPoint_ != 0)
    {
        if (tempConnEndPoint_ != 0 && highlightedEndPoint_ != tempConnEndPoint_)
        {
            highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_ALLOWED);
        }
        else
        {
            highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
        }

        highlightedEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void BlockDiagram::addColumn(QString const& name, ColumnContentType contentType,
                             unsigned int allowedItems)
{
    QSharedPointer<QUndoCommand> cmd(new ColumnAddCommand(layout_.data(), name, contentType, allowedItems));
    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: removeColumn()
//-----------------------------------------------------------------------------
void BlockDiagram::removeColumn(DiagramColumn* column)
{
    QSharedPointer<QUndoCommand> cmd(new ColumnDeleteCommand(layout_.data(), column));
    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        DesignWidget* doc = static_cast<DesignWidget*>(parent());
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

QSharedPointer<DesignConfiguration> BlockDiagram::getDesignConfiguration() const {
	return designConf_;
}

void BlockDiagram::removeInstanceName( const QString& name ) {
	instanceNames_.removeAll(name);
}

void BlockDiagram::updateInstanceName( const QString& oldName, const QString& newName ) {
	instanceNames_.removeAll(oldName);
	instanceNames_.append(newName);
}

//-----------------------------------------------------------------------------
// Function: onSelected()
//-----------------------------------------------------------------------------
void BlockDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection)
    { 
        // Check if the selected item was a component.
        if (newSelection->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = qgraphicsitem_cast<DiagramComponent *>(newSelection);
            emit componentSelected(comp);
        }
        // Check if the selected item was a port.
        else if (newSelection->type() == DiagramPort::Type)
        {
            DiagramPort* port = qgraphicsitem_cast<DiagramPort*>(newSelection);
            emit portSelected(port);
        }
        // Check if the selected item was an interface.
        else if (newSelection->type() == DiagramInterface::Type)
        {
            DiagramInterface* interface = qgraphicsitem_cast<DiagramInterface*>(newSelection);
            emit interfaceSelected(interface);
        }
		// check if the selected item was a connection
		else if (newSelection->type() == DiagramInterconnection::Type) {
			DiagramInterconnection* connection = qgraphicsitem_cast<DiagramInterconnection*>(newSelection);
			emit connectionSelected(connection);
		}
        // Otherwise the selection was an interconnection
        else
        {
            // inform others that nothing is currently selected
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
// Function: onShow()
//-----------------------------------------------------------------------------
void BlockDiagram::onShow()
{
    // Retrieve the new selection.
    QGraphicsItem *newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    onSelected(newSelection);
}

//-----------------------------------------------------------------------------
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
DiagramColumnLayout* BlockDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: ()
//-----------------------------------------------------------------------------
GenericEditProvider& BlockDiagram::getEditProvider()
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: addInterface()
//-----------------------------------------------------------------------------
void BlockDiagram::addInterface(DiagramColumn* column, QPointF const& pos)
{
	QSharedPointer<BusInterface> busif(new BusInterface());
    DiagramInterface *newItem = new DiagramInterface(lh_, component_, busif, 0);
    newItem->setPos(snapPointToGrid(pos));

    connect(newItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
    connect(newItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Save the positions of the other diagram interfaces.
    QMap<DiagramInterface*, QPointF> oldPositions;

    if (column->getContentType() == COLUMN_CONTENT_IO)
    {
        foreach (QGraphicsItem* item, column->childItems())
        {
            if (item->type() == DiagramInterface::Type)
            {
                DiagramInterface* interface = static_cast<DiagramInterface*>(item);
                oldPositions.insert(interface, interface->scenePos());
            }
        }
    }

    QSharedPointer<QUndoCommand> cmd(new ItemAddCommand(column, newItem));
    cmd->redo();

    // Determine if the other interfaces changed their position and create undo commands for them.
    if (column->getContentType() == COLUMN_CONTENT_IO)
    {
        QMap<DiagramInterface*, QPointF>::iterator cur = oldPositions.begin();

        while (cur != oldPositions.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                QUndoCommand* childCmd = new ItemMoveCommand(cur.key(), cur.value(), cmd.data());
            }

            ++cur;
        }
    }

    editProvider_.addCommand(cmd, false);
}

QList<DiagramComponent*> BlockDiagram::getInstances() const {

	// the list to store the diagram components to
	QList<DiagramComponent*> instances;

	// ask for all graphics items.
	QList<QGraphicsItem*> graphItems = items();
	foreach (QGraphicsItem* graphItem, graphItems) {

		// make dynamic type conversion
		DiagramComponent* diagComp = dynamic_cast<DiagramComponent*>(graphItem);
		// if the item was a diagram component then add it to the list.
		if (diagComp) 
			instances.append(diagComp);
	}
	return instances;
}

DesignWidget* BlockDiagram::parent() const {
	return parent_;
}

//-----------------------------------------------------------------------------
// Function: setProtection()
//-----------------------------------------------------------------------------
void BlockDiagram::setProtection(bool locked)
{
    locked_ = locked;
    clearSelection();
    emit clearItemSelection();
}

//-----------------------------------------------------------------------------
// Function: isProtected()
//-----------------------------------------------------------------------------
bool BlockDiagram::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: createConnection()
//-----------------------------------------------------------------------------
void BlockDiagram::createConnection(QGraphicsSceneMouseEvent * mouseEvent)
{
    // Disable highlights from all potential end points.
    for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
    {
        tempPotentialEndingEndPoints_.at(i)->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
    }
    tempPotentialEndingEndPoints_.clear();

    DiagramConnectionEndPoint* endPoint =
        DiagramUtil::snapToItem<DiagramConnectionEndPoint>(mouseEvent->scenePos(), this, GridSize);

    // Check if there is no end point close enough to the cursor or the
    // end points cannot be connected together.
    if (endPoint == 0 || endPoint == tempConnEndPoint_ ||
        !endPoint->canConnect(tempConnEndPoint_) ||
        !tempConnEndPoint_->canConnect(endPoint)) {

            // if tempConnection has been created then delete it
            if (tempConnection_) {
                removeItem(tempConnection_);
                delete tempConnection_;
                tempConnection_ = 0;
            }
    }
    else 
    {
        // Check if the connection should be converted to an off-page connection.
        bool firstOffPage = tempConnEndPoint_->type() == DiagramOffPageConnector::Type;
        bool secondOffPage = endPoint->type() == DiagramOffPageConnector::Type;

        if (offPageMode_ ||
            ((firstOffPage || secondOffPage) && tempConnEndPoint_->type() != endPoint->type()))
        {
            delete tempConnection_;

            if (!firstOffPage)
            {
                tempConnEndPoint_ = tempConnEndPoint_->getOffPageConnector();
            }

            if (!secondOffPage)
            {
                endPoint = endPoint->getOffPageConnector();
            }

            tempConnection_ = new DiagramInterconnection(tempConnEndPoint_, endPoint, false,
                                                         QString(), QString(), this);
        }

        connect(tempConnection_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (tempConnection_->connectEnds())
        {
            QSharedPointer<QUndoCommand> cmd(new ConnectionAddCommand(this, tempConnection_));
            editProvider_.addCommand(cmd, false);

            tempConnection_ = 0;

            if (!offPageMode_)
            {
                tempConnEndPoint_ = 0;
            }

            emit contentChanged();
        }
        else
        {
            delete tempConnection_;
            tempConnection_ = 0;
            tempConnEndPoint_ = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: keyReleaseEvent()
//-----------------------------------------------------------------------------
void BlockDiagram::keyReleaseEvent(QKeyEvent *event)
{
    // Check if the user ended the off-page connection mode.
    if ((event->key() == Qt::Key_Shift) && offPageMode_)
    {
        if (tempConnEndPoint_ != 0)
        {
            if (tempConnEndPoint_->getInterconnections().size() == 0)
            {
                tempConnEndPoint_->setVisible(false);
            }
        }
       
        endConnect();
    }
}

//-----------------------------------------------------------------------------
// Function: endConnect()
//-----------------------------------------------------------------------------
void BlockDiagram::endConnect()
{
    // Destroy the connection that was being drawn.
    if (tempConnection_) {
        delete tempConnection_;
        tempConnection_ = 0;
    }

    // Disable highlights from all end points.
    if (highlightedEndPoint_ != 0)
    {
        highlightedEndPoint_->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
        highlightedEndPoint_ = 0;
    }

    for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
    {
        tempPotentialEndingEndPoints_.at(i)->setHighlight(DiagramConnectionEndPoint::HIGHLIGHT_OFF);
    }
    tempPotentialEndingEndPoints_.clear();

    offPageMode_ = false;
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void BlockDiagram::onSelectionChanged()
{
    selectionToFront();

    // Retrieve the new selection.
    QGraphicsItem* newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    // If the old selection was an off-page connector, hide its connections.
    // Also hide the previously selected connection if it was an off-page connection.
    if (oldSelection_ != 0)
    {
        if (oldSelection_->type() == DiagramOffPageConnector::Type)
        {
            DiagramOffPageConnector* connector = static_cast<DiagramOffPageConnector*>(oldSelection_);

            foreach (DiagramInterconnection* conn, connector->getInterconnections())
            {
                if (conn != newSelection)
                {
                    conn->setVisible(false);
                }
            }
        }
        else if (oldSelection_->type() == DiagramInterconnection::Type && oldSelection_ != newSelection)
        {
            DiagramInterconnection* conn = static_cast<DiagramInterconnection*>(oldSelection_);

            if (conn->endPoint1() != 0)
            {
                if (conn->endPoint1()->type() == DiagramOffPageConnector::Type)
                {
                    oldSelection_->setVisible(false);
                }
                else
                {
                    oldSelection_->setZValue(-1000);
                }
            }
        }
    }

    // If the new selection is an off-page connector, show its connections.
    if (newSelection != 0 && newSelection->type() == DiagramOffPageConnector::Type)
    {
        DiagramOffPageConnector* connector = static_cast<DiagramOffPageConnector*>(newSelection);

        foreach (DiagramInterconnection* conn, connector->getInterconnections())
        {
            conn->setVisible(true);
        }
    }

    onSelected(newSelection);

    // Save the current selection as the old selection.
    oldSelection_ = newSelection;
}

//-----------------------------------------------------------------------------
// Function: toggleConnectionStyle()
//-----------------------------------------------------------------------------
void BlockDiagram::toggleConnectionStyle(DiagramInterconnection* conn, QUndoCommand* parentCmd)
{
    Q_ASSERT(parentCmd != 0);
    emit clearItemSelection();

    // Determine the new end points for the connection.
    DiagramConnectionEndPoint* endPoint1 = conn->endPoint1();
    DiagramConnectionEndPoint* endPoint2 = conn->endPoint2();

    if (endPoint1->type() == DiagramOffPageConnector::Type)
    {
        endPoint1 = static_cast<DiagramConnectionEndPoint*>(endPoint1->parentItem());
        endPoint2 = static_cast<DiagramConnectionEndPoint*>(endPoint2->parentItem());
    }
    else
    {
        endPoint1 = endPoint1->getOffPageConnector();
        endPoint2 = endPoint2->getOffPageConnector();
    }

    // Recreate the connection by first deleting the old and then creating a new one.
    QUndoCommand* cmd = new ConnectionDeleteCommand(static_cast<DiagramInterconnection*>(conn), parentCmd);
    cmd->redo();

    DiagramInterconnection* newConn = new DiagramInterconnection(endPoint1, endPoint2, false,
                                                                 QString(), QString(), this);

    connect(newConn, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    if (newConn->connectEnds())
    {
        QUndoCommand* cmd = new ConnectionAddCommand(this, newConn, parentCmd);
    }
    else
    {
        delete newConn;
        newConn = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: hideOffPageConnections()
//-----------------------------------------------------------------------------
void BlockDiagram::hideOffPageConnections()
{
    foreach (QGraphicsItem* item, items())
    {
        DiagramInterconnection* conn = dynamic_cast<DiagramInterconnection*>(item);

        if (conn != 0 && conn->endPoint1()->type() == DiagramOffPageConnector::Type)
        {
            conn->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setBusWidthsVisible()
//-----------------------------------------------------------------------------
void BlockDiagram::setBusWidthsVisible(bool visible)
{
    foreach (QGraphicsItem* item, items())
    {
        DiagramInterconnection* conn = dynamic_cast<DiagramInterconnection*>(item);

        if (conn != 0)
        {
            conn->setBusWidthVisible(visible);
        }
    }
}
