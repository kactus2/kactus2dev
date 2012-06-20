/* 
 *
 * 		filename: blockdiagram.cpp
 */

#include "HWDesignDiagram.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "BusPortItem.h"
#include "AdHocPortItem.h"
#include "AdHocInterfaceItem.h"
#include "BusInterfaceItem.h"
#include "OffPageConnectorItem.h"
#include "SelectItemTypeDialog.h"
#include "HWDesignWidget.h"
#include "HWAddCommands.h"
#include "HWDeleteCommands.h"
#include "HWMoveCommands.h"
#include "HWChangeCommands.h"
#include "BusInterfaceDialog.h"

#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/DiagramUtil.h>
#include <common/diagramgrid.h>
#include <common/dialogs/comboSelector/comboselector.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/GenericEditProvider.h>

#include "columnview/HWColumn.h"

#include <LibraryManager/libraryhandler.h>

#include <AdHocEditor/AdHocEditor.h>

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

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::HWDesignDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, HWDesignWidget *parent)
    : DesignDiagram(lh, NULL, editProvider, parent),
      parent_(parent),
      tempConnection_(0),
      tempConnEndPoint_(0), 
      highlightedEndPoint_(0),
      dragCompType_(CIT_NONE),
      dragBus_(false),
      offPageMode_(false),
      oldSelection_(0)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(&editProvider, SIGNAL(modified()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void HWDesignDiagram::clearScene()
{
    destroyConnections();
    layout_.clear();
    DesignDiagram::clearScene();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::loadDesign()
//-----------------------------------------------------------------------------
void HWDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    // Create the column layout.
    layout_ = QSharedPointer<GraphicsColumnLayout>(new GraphicsColumnLayout(this));

    if (design->getColumns().isEmpty())
    {
        addColumn(ColumnDesc("IO", COLUMN_CONTENT_IO));
        addColumn(ColumnDesc("Buses", COLUMN_CONTENT_BUSES));
        addColumn(ColumnDesc("Components", COLUMN_CONTENT_COMPONENTS));
        addColumn(ColumnDesc("IO", COLUMN_CONTENT_IO));
    }
    else
    {
        foreach (ColumnDesc const& desc, design->getColumns())
        {
            HWColumn* column = new HWColumn(desc, layout_.data(), this);
            layout_->addColumn(column, true);
        }
    }

    // Create diagram interfaces for the top-level bus interfaces.
    foreach (QSharedPointer<BusInterface> busIf, getEditedComponent()->getBusInterfaces())
    {
        BusInterfaceItem* diagIf = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), busIf, 0);

        // Add the diagram interface to the first column where it is allowed to be placed.
        layout_->addItem(diagIf);
    }

    /* component instances */
    foreach (ComponentInstance const& instance,
             design->getComponentInstances()) {
        
		QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

		if (!component)
        {
			emit errorMessage(tr("The component %1 instantiated within design "
				                 "%2 was not found in the library").arg(
				              instance.getComponentRef().getName()).arg(design->getVlnv()->getName()));
			
            // Create an unpackaged component so that we can still visualize the component instance.
            component = QSharedPointer<Component>(new Component(instance.getComponentRef()));
            component->setComponentImplementation(getEditedComponent()->getComponentImplementation());
		}

        HWComponentItem* diagComp = new HWComponentItem(getLibraryInterface(), component,
                                                          instance.getInstanceName(),
                                                          instance.getDisplayName(),
                                                          instance.getDescription(),
                                                          instance.getConfigurableElementValues(),
                                                          instance.getPortAdHocVisibilities());

        // Setup the custom port positions.
        QMapIterator<QString, QPointF> itrPortPos(instance.getBusInterfacePositions());

        while (itrPortPos.hasNext())
        {
            itrPortPos.next();
            BusPortItem* port = diagComp->getBusPort(itrPortPos.key());

            if (port == 0)
            {
                port = diagComp->addPort(itrPortPos.value());
                port->setName(itrPortPos.key());
            }

            port->setPos(itrPortPos.value());
            diagComp->onMovePort(port);
        }

        // Setup the custom ad-hoc port positions for the component.
        itrPortPos = QMapIterator<QString, QPointF>(instance.getAdHocPortPositions());

        while (itrPortPos.hasNext())
        {
            itrPortPos.next();
            AdHocPortItem* port = diagComp->getAdHocPort(itrPortPos.key());

            if (port != 0)
            {
                port->setPos(itrPortPos.value());
                diagComp->onMovePort(port);
            }
        }

        // Check if the position is not found.
        if (instance.getPosition().isNull())
        {
            // Migrate from the old layout to the column based layout.
            layout_->addItem(diagComp);
        }
        else
        {
            diagComp->setPos(instance.getPosition());

            GraphicsColumn* column = layout_->findColumnAt(instance.getPosition());
            
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
		addInstanceName(instance.getInstanceName());
    }

    /* interconnections */
    foreach(Design::Interconnection interconnection, design->getInterconnections())
    {
		// find the first referenced component
        HWComponentItem *comp1 = getComponent(interconnection.interface1.componentRef);

		if (!comp1) {
			emit errorMessage(tr("Component %1 was not found in the design").arg(
			interconnection.interface1.componentRef));
			continue;
		}
		
		// find the second referenced component
        HWComponentItem *comp2 = getComponent(interconnection.interface2.componentRef);

		if (!comp2) {
			emit errorMessage(tr("Component %1 was not found in the design").arg(
			interconnection.interface2.componentRef));
			continue;
		}

		// find the port of the first referenced component
        ConnectionEndpoint* port1 = comp1->getBusPort(interconnection.interface1.busRef);

		if (!port1) {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
			interconnection.interface1.busRef).arg(interconnection.interface1.componentRef));
			continue;
		}
		
		// find the port of the second referenced component
        ConnectionEndpoint* port2 = comp2->getBusPort(interconnection.interface2.busRef);

		if (!port2) {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
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
		if (comp1 && comp2 && port1 && port2)
        {
			HWConnection *diagramInterconnection =
				new HWConnection(port1, port2, true, QString(), interconnection.displayName,
                                           interconnection.description, this);
            diagramInterconnection->setRoute(interconnection.route);
			diagramInterconnection->setName(interconnection.name);

            if (interconnection.offPage)
            {
                diagramInterconnection->setVisible(false);
            }

            connect(diagramInterconnection, SIGNAL(errorMessage(QString const&)), this,
                    SIGNAL(errorMessage(QString const&)));

            addItem(diagramInterconnection);
            diagramInterconnection->updatePosition();
		}
    }

	// Create hierarchical connections.
    QList<QString> connectedHier;
    QList<Design::HierConnection> hierConnections = design->getHierarchicalConnections();
    for (int i = 0; i < hierConnections.size(); ++i) {

        Design::HierConnection hierConn = hierConnections.at(i);

		QSharedPointer<BusInterface> busIf = getEditedComponent()->getBusInterfaces().value(hierConn.interfaceRef);

		// if the bus interface was not found
		if (!busIf) {
			emit errorMessage(tr("Bus interface %1 was not found in the top-component").arg(
				hierConn.interfaceRef));
			continue;
		}

        // Find the corresponding diagram interface.
        ConnectionEndpoint* diagIf = 0;

        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == BusInterfaceItem::Type &&
                static_cast<BusInterfaceItem*>(item)->getBusInterface() == busIf)
            {
                diagIf = static_cast<BusInterfaceItem*>(item);
                break;
            }
        }

        Q_ASSERT(diagIf != 0);

		// Check if the position is found.
        if (!hierConn.position.isNull())
        {
            diagIf->setPos(hierConn.position);
            diagIf->setDirection(hierConn.direction);

            GraphicsColumn* column = layout_->findColumnAt(hierConn.position);
            
            if (column != 0)
            {
                column->addItem(diagIf);
            }
            else
            {
                layout_->addItem(diagIf);
            }
        }

		// find the component where the hierarchical connection is connected to
        HWComponentItem *comp = getComponent(hierConn.interface_.componentRef);
		if (!comp) {
			emit errorMessage(tr("Component %1 was not found in the top-design").arg(
				hierConn.interface_.componentRef));
			continue;
		}

		// find the port of the component
        ConnectionEndpoint* compPort = comp->getBusPort(hierConn.interface_.busRef);
		if (!compPort)
        {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
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

            HWConnection* diagConn = new HWConnection(compPort, diagIf, true, QString(),
                                                                          QString(), QString(), this);
            diagConn->setRoute(hierConn.route);

            if (hierConn.offPage)
            {
                diagConn->setVisible(false);
            }

            connect(diagConn, SIGNAL(errorMessage(QString const&)), this,
                    SIGNAL(errorMessage(QString const&)));
			connectedHier.append(hierConn.interfaceRef);

            addItem(diagConn);
            diagConn->updatePosition();
		}
    }

    // Set the ad-hoc data for the diagram.
    setAdHocData(getEditedComponent(), design->getPortAdHocVisibilities());

    // Create top-level ad-hoc interfaces and set their positions.
    QMapIterator<QString, bool> itrCurPort(getPortAdHocVisibilities());
    QMap<QString, QPointF> const& adHocPortPositions = design->getAdHocPortPositions();

    while (itrCurPort.hasNext())
    {
        itrCurPort.next();

        if (itrCurPort.value())
        {
            QString const& name = itrCurPort.key();
            AdHocInterfaceItem* adHocIf = new AdHocInterfaceItem(getEditedComponent(),
                                                                       getEditedComponent()->getPort(name),
                                                                       getLibraryInterface(), 0);
            if (adHocPortPositions.contains(name))
            {
                QPointF pos = adHocPortPositions[name];
                adHocIf->setPos(pos);
            }

            //layout_->addItem(adHocIf);

            // Add the ad-hoc interface to the first column where it is allowed to be placed.
            GraphicsColumn* column = layout_->findColumnAt(adHocIf->scenePos());

            if (column != 0 && adHocPortPositions.contains(name))
            {
                column->addItem(adHocIf, true);
            }
            else
            {
                layout_->addItem(adHocIf);
            }
        }
    }

    // Create ad-hoc connections based on the design data.
    QList<Design::AdHocConnection> adHocConnections = design->getAdHocConnections();

    foreach (Design::AdHocConnection const& adHocConn, adHocConnections)
    {
        // Convert one multiple-port connection to two-port-only connections.
        if (adHocConn.externalPortReferences.empty() && !adHocConn.internalPortReferences.empty())
        {
            // Find the first referenced component.
            HWComponentItem* comp1 = getComponent(adHocConn.internalPortReferences.at(0).componentRef);

            if (comp1 == 0)
            {
                emit errorMessage(tr("Component %1 was not found in the design").arg(
                    adHocConn.internalPortReferences.at(0).componentRef));
                continue;
            }

            // Find the corresponding port.
            ConnectionEndpoint* port1 = comp1->getAdHocPort(adHocConn.internalPortReferences.at(0).portRef);

            if (port1 == 0)
            {
                emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                    adHocConn.internalPortReferences.at(0).portRef).arg(adHocConn.internalPortReferences.at(0).componentRef));
                continue;
            }

            if (adHocConn.offPage)
            {
                port1 = port1->getOffPageConnector();
            }

            for (int i = 1; i < adHocConn.internalPortReferences.size(); ++i)
            {
                // Find the second referenced component.
                HWComponentItem* comp2 = getComponent(adHocConn.internalPortReferences.at(i).componentRef);

                if (comp2 == 0)
                {
                    emit errorMessage(tr("Component %1 was not found in the design").arg(
                        adHocConn.internalPortReferences.at(i).componentRef));
                    continue;
                }

                ConnectionEndpoint* port2 = comp2->getAdHocPort(adHocConn.internalPortReferences.at(i).portRef);

                if (port2 == 0)
                {
                    emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                        adHocConn.internalPortReferences.at(i).portRef).arg(adHocConn.internalPortReferences.at(i).componentRef));
                    continue;
                }

                if (adHocConn.offPage)
                {
                    port2 = port2->getOffPageConnector();
                }

                // Create the ad-hoc connection graphics item.
                HWConnection* conn = new HWConnection(port1, port2, true, adHocConn.name,
                                                                          adHocConn.displayName,
                                                                          adHocConn.description, this);
                conn->setRoute(adHocConn.route);
                
                conn->setAdHocLeftBound(0, adHocConn.internalPortReferences.at(0).left);
                conn->setAdHocRightBound(0, adHocConn.internalPortReferences.at(0).right);

                conn->setAdHocLeftBound(1, adHocConn.internalPortReferences.at(i).left);
                conn->setAdHocRightBound(1, adHocConn.internalPortReferences.at(i).right);

                if (adHocConn.offPage)
                {
                    conn->setVisible(false);
                }

                connect(conn, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                addItem(conn);
                conn->updatePosition();
            }
        }

        for (int j = 0; j < adHocConn.externalPortReferences.size(); ++j)
        {
            ConnectionEndpoint* adHocIf =
                getDiagramAdHocPort(adHocConn.externalPortReferences.at(j).portRef);

            if (adHocIf == 0)
            {
                emit errorMessage(tr("Port %1 was not found in the top-level component").arg(
                                  adHocConn.externalPortReferences.at(j).portRef));
                continue;
            }

            if (adHocConn.offPage)
            {
                adHocIf = adHocIf->getOffPageConnector();
            }

            for (int i = 0; i < adHocConn.internalPortReferences.size(); ++i)
            {
                // Find the referenced component.
                HWComponentItem* comp = getComponent(adHocConn.internalPortReferences.at(i).componentRef);

                if (comp == 0)
                {
                    emit errorMessage(tr("Component %1 was not found in the design").arg(
                        adHocConn.internalPortReferences.at(i).componentRef));
                    continue;
                }

                ConnectionEndpoint* port = comp->getAdHocPort(adHocConn.internalPortReferences.at(i).portRef);

                if (port == 0)
                {
                    emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                        adHocConn.internalPortReferences.at(i).portRef).arg(adHocConn.internalPortReferences.at(i).componentRef));
                    continue;
                }

                if (adHocConn.offPage)
                {
                    port = port->getOffPageConnector();
                }

                // Create the ad-hoc connection graphics item.
                HWConnection* conn = new HWConnection(port, adHocIf, true,
                                                                          adHocConn.name,
                                                                          adHocConn.displayName,
                                                                          adHocConn.description, this);
                conn->setRoute(adHocConn.route);

                conn->setAdHocLeftBound(0, adHocConn.internalPortReferences.at(i).left);
                conn->setAdHocRightBound(0, adHocConn.internalPortReferences.at(i).right);

                conn->setAdHocLeftBound(1, adHocConn.externalPortReferences.at(j).left);
                conn->setAdHocRightBound(1, adHocConn.externalPortReferences.at(j).right);

                if (adHocConn.offPage)
                {
                    conn->setVisible(false);
                }

                connect(conn, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
                addItem(conn);
                conn->updatePosition();
            }
        }
    }

    // Update the stacking of the columns.
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: ~HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::~HWDesignDiagram()
{
    destroyConnections();
}

//-----------------------------------------------------------------------------
// Function: getComponent()
//-----------------------------------------------------------------------------
HWComponentItem *HWDesignDiagram::getComponent(const QString &instanceName)
{
	// search all graphicsitems in the scene
	foreach (QGraphicsItem *item, items()) {

		// if the item is a component
        if (item->type() == HWComponentItem::Type) {
            HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);

			// if component's name matches
            if (comp->name() == instanceName)
                return comp;
        }
    }

	// if no component was found
	emit errorMessage(tr("Component %1 was not found within design").arg(instanceName));
    return 0;
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> HWDesignDiagram::createDesign(const VLNV &vlnv) const
{
	QSharedPointer<Design> design(new Design(vlnv));

	QList<ComponentInstance> instances;
	QList<Design::Interconnection> interconnections;
	QList<Design::HierConnection> hierConnections;
    QList<Design::AdHocConnection> adHocConnections;
    QList<ColumnDesc> columns;

    QList<QString> unpackagedNames;
    QMap<QString, QPointF> adHocPortPositions;

	foreach (QGraphicsItem *item, items())
    {
		if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);

            ComponentInstance instance(comp->name(), comp->displayName(),
				                       comp->description(), *comp->componentModel()->getVlnv(),
                                       comp->scenePos());

			// save the configurable element values to the design
			instance.setConfigurableElementValues(comp->getConfigurableElements());
            instance.setPortAdHocVisibilities(comp->getPortAdHocVisibilities());

            // Save the port positions.
            QMapIterator< QString, QSharedPointer<BusInterface> >
                itrBusIf(comp->componentModel()->getBusInterfaces());

            while (itrBusIf.hasNext())
            {
                itrBusIf.next();
                instance.updateBusInterfacePosition(itrBusIf.key(), comp->getBusPort(itrBusIf.key())->pos());
            }

            QMapIterator<QString, bool> itrAdHoc(comp->getPortAdHocVisibilities());

            while (itrAdHoc.hasNext())
            {
                itrAdHoc.next();

                if (itrAdHoc.value())
                {
                    instance.updateAdHocPortPosition(itrAdHoc.key(), comp->getAdHocPort(itrAdHoc.key())->pos());
                }
            }

			instances.append(instance);
		}
        else if (item->type() == HWConnection::Type)
        {
            HWConnection *conn = qgraphicsitem_cast<HWConnection *>(item);

            // Save data based on the connection type.
            if (conn->isBus())
            {
			    if (conn->endpoint1()->encompassingComp() && conn->endpoint2()->encompassingComp())
                {
				    Design::Interface iface1(conn->endpoint1()->encompassingComp()->name(),
					    conn->endpoint1()->name());
				    Design::Interface iface2(conn->endpoint2()->encompassingComp()->name(),
					    conn->endpoint2()->name());
				    interconnections.append(Design::Interconnection(conn->name(), iface1, iface2,
                                                                    conn->route(),
                                                                    conn->endpoint1()->type() == OffPageConnectorItem::Type,
                                                                    QString(),
																    conn->description()));
			    }
                else
                {
				    ConnectionEndpoint *compPort;
				    ConnectionEndpoint *hierPort;

                    if (conn->endpoint1()->encompassingComp()) {
					    compPort = conn->endpoint1();
					    hierPort = conn->endpoint2();
				    } else {
					    compPort = conn->endpoint2();
					    hierPort = conn->endpoint1();
				    }

                    if (hierPort->getBusInterface() != 0)
                    {
                        hierConnections.append(
					        Design::HierConnection(hierPort->name(),
                                                   Design::Interface(compPort->encompassingComp()->name(),
					                                                 compPort->name()),
                                                   hierPort->scenePos(), hierPort->getDirection(),
                                                   conn->route(),
                                                   conn->endpoint1()->type() == OffPageConnectorItem::Type));
                    }
			    }
            }
            else
            {
                // Otherwise the connection is ad-hoc.
                QList<Design::PortRef> internalPortRefs;
                QList<Design::PortRef> externalPortRefs;

                if (conn->endpoint1()->isHierarchical())
                {
                    externalPortRefs.append(Design::PortRef(conn->endpoint1()->name(), QString(),
                                                            conn->getAdHocLeftBound(0),
                                                            conn->getAdHocRightBound(0)));
                }
                else
                {
                    internalPortRefs.append(Design::PortRef(conn->endpoint1()->name(),
                                                            conn->endpoint1()->encompassingComp()->name(),
                                                            conn->getAdHocLeftBound(0),
                                                            conn->getAdHocRightBound(0)));
                }

                if (conn->endpoint2()->isHierarchical())
                {
                    externalPortRefs.append(Design::PortRef(conn->endpoint2()->name(), QString(),
                                                            conn->getAdHocLeftBound(1),
                                                            conn->getAdHocRightBound(1)));
                }
                else
                {
                    internalPortRefs.append(Design::PortRef(conn->endpoint2()->name(),
                                                            conn->endpoint2()->encompassingComp()->name(),
                                                            conn->getAdHocLeftBound(1),
                                                            conn->getAdHocRightBound(1)));
                }

                adHocConnections.append(Design::AdHocConnection(conn->name(), QString(),
                                                                conn->description(), 0 /*TODO*/,
                                                                internalPortRefs, externalPortRefs,
                                                                conn->route(),
                                                                conn->endpoint1()->type() == OffPageConnectorItem::Type));
            }
		}
        else if (item->type() == AdHocInterfaceItem::Type)
        {
            AdHocInterfaceItem* adHocIf = static_cast<AdHocInterfaceItem*>(item);
            adHocPortPositions[adHocIf->name()] = adHocIf->scenePos();
        }
	}

    foreach(GraphicsColumn* column, layout_->getColumns())
    {
        columns.append(column->getColumnDesc());
    }

	design->setComponentInstances(instances);
	design->setInterconnections(interconnections);
    design->setHierarchicalConnections(hierConnections);
    design->setAdHocConnections(adHocConnections);
    design->setColumns(columns);   
    design->setPortAdHocVisibilities(getPortAdHocVisibilities());
    design->setAdHocPortPositions(adHocPortPositions);

	return design;
}

//-----------------------------------------------------------------------------
// Function: setMode()
//-----------------------------------------------------------------------------
void HWDesignDiagram::setMode(DrawMode mode)
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
// Function: HWDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::updateHierComponent()
{
    // store all the bus interfaces to a map
    QMap<QString, QSharedPointer<BusInterface> > busIfs;

    // Search all graphics items in the scene.
    foreach (QGraphicsItem *item, items())
    {
        // Check if the item is a diagram interface and its bus interface is defined.
        BusInterfaceItem* diagIf = dynamic_cast<BusInterfaceItem*>(item);

        if (diagIf != 0 && diagIf->getBusInterface() != 0)
        {
            busIfs[diagIf->name()] = diagIf->getBusInterface();

            // Set the bus interface non-temporary.
            //diagIf->setTemporary(false);
        }
    }

    getEditedComponent()->setBusInterfaces(busIfs);
}

//-----------------------------------------------------------------------------
// Function: selectionToFront()
//-----------------------------------------------------------------------------
void HWDesignDiagram::selectionToFront()
{
    /*if (oldSelection_ != 0 && oldSelection_->type() == HWConnection::Type)
    {
        oldSelection_->setZValue(-900);
    }*/

    if (selectedItems().isEmpty())
        return;

    QGraphicsItem* selectedItem = selectedItems().first();

    if (selectedItem->type() == HWConnection::Type)
    {
        selectedItem->setZValue(900);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // if other than left button was pressed return the mode back to select
	if (mouseEvent->button() != Qt::LeftButton)
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
            createConnection(mouseEvent);
        }
        // Otherwise choose a new start end point if in normal connection mode.
        else if (!offPageMode_)
        {
            offPageMode_ = (mouseEvent->modifiers() & Qt::ShiftModifier);

            // no items are selected if the mode is connect
            clearSelection();

            // Try to snap to a connection end point.
            ConnectionEndpoint* endpoint =
                DiagramUtil::snapToItem<ConnectionEndpoint>(mouseEvent->scenePos(), this, GridSize);

            if (endpoint == 0)
            {
                return;
            }

            if (offPageMode_)
            {
                if (highlightedEndPoint_ != 0)
                {
                    highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
                    highlightedEndPoint_ = 0;
                }

                if (endpoint->type() != OffPageConnectorItem::Type)
                {
                    endpoint = endpoint->getOffPageConnector();
                    endpoint->setVisible(true);
                }
            }

            tempConnEndPoint_ = endpoint;
        }

        // Start drawing a new connection if in off page mode or we were not creating anything yet.
        if (offPageMode_ || !creating)
        {
            // Create the connection.
            tempConnection_ = new HWConnection(tempConnEndPoint_->scenePos(),
                tempConnEndPoint_->getDirection(),
                mouseEvent->scenePos(),
                QVector2D(0.0f, 0.0f), QString(), QString(), this);

            if (tempConnEndPoint_->isAdHoc())
            {
                tempConnection_->setLineWidth(1);
            }

            addItem(tempConnection_);

            // Determine all potential end points to which the starting end point could be connected
            // and highlight them.
            foreach(QGraphicsItem* item, items())
            {
                ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

                if (endpoint != 0 && endpoint->isVisible() && endpoint != tempConnEndPoint_ &&
                    endpoint->getOffPageConnector() != tempConnEndPoint_ &&
                    endpoint->canConnect(tempConnEndPoint_) && tempConnEndPoint_->canConnect(endpoint))
                {
                    tempPotentialEndingEndPoints_.append(endpoint);
                    endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_ALLOWED);
                }
            }
        }
    }
    else if (getMode() == MODE_INTERFACE)
    {
        // Find the column under the cursor.
        GraphicsColumn* column = layout_->findColumnAt(mouseEvent->scenePos());

        // Add a new diagram interface to the column it it is allowed.
        if (column != 0 && column->getColumnDesc().getAllowedItems() & CIT_INTERFACE)
        {
            addInterface(column, mouseEvent->scenePos());
        }
    }
    else if (getMode() == MODE_DRAFT)
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
            GraphicsColumn* column = layout_->findColumnAt(mouseEvent->scenePos());
            
            if (column != 0)
            {
                // Check what kind of item should be created.
                unsigned int itemType = column->getColumnDesc().getAllowedItems();
                
                // Check if the item type is ambiguous (interface vs. the rest of the types).
                if (itemType != CIT_INTERFACE && itemType & CIT_INTERFACE)
                {
                    // Open a dialog to determine which type of item to create.
                    SelectItemTypeDialog dialog((QWidget*)parent(), column->getColumnDesc().getAllowedItems());

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
                    QString name = createInstanceName("unnamed");

                    // Create a component model without a valid vlnv.
                    QSharedPointer<Component> comp = QSharedPointer<Component>(new Component());
                    comp->setVlnv(VLNV());
                    comp->setComponentImplementation(getEditedComponent()->getComponentImplementation());

                    // Create the corresponding diagram component.
                    HWComponentItem* diagComp = new HWComponentItem(getLibraryInterface(), comp, name);
                    diagComp->setPos(snapPointToGrid(mouseEvent->scenePos()));

                    QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, diagComp));

					connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
						this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
					connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
						this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                    getEditProvider().addCommand(cmd);
                }
            }
        }
        // Otherwise check if the item is an unpackaged component in which case
        // we can add a new port (bus interface) to it.
        else if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(item);

            // The component is unpackaged if it has an invalid vlnv.
            if (!comp->componentModel()->getVlnv()->isValid())
            {
                QMap<BusPortItem*, QPointF> oldPositions;

                // Save old port positions.
                foreach (QGraphicsItem* item, comp->childItems())
                {
                    if (item->type() == BusPortItem::Type)
                    {
                        BusPortItem* port = static_cast<BusPortItem*>(item);
                        oldPositions.insert(port, port->pos());
                    }
                }

                QSharedPointer<QUndoCommand> cmd(new PortAddCommand(comp, snapPointToGrid(mouseEvent->scenePos())));
                cmd->redo();
                
                // Create child undo commands for the changed ports.
                QMap<BusPortItem*, QPointF>::iterator cur = oldPositions.begin();

                while (cur != oldPositions.end())
                {
                    if (cur.key()->pos() != cur.value())
                    {
                        QUndoCommand* childCmd = new PortMoveCommand(cur.key(), cur.value(), cmd.data());
                    }

                    ++cur;
                }

                // Add the command to the edit stack.
                getEditProvider().addCommand(cmd, false);
            }
        }
    }
    else if (getMode() == MODE_TOGGLE_OFFPAGE)
    {
        // Try to snap to a connection end point.
        ConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<ConnectionEndpoint>(mouseEvent->scenePos(), this, GridSize);

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
            QGraphicsItem* item = itemAt(mouseEvent->scenePos());

            if (item != 0 && item->type() == HWConnection::Type)
            {
                toggleConnectionStyle(static_cast<HWConnection*>(item), cmd.data());
            }
        }

        if (cmd->childCount() > 0)
        {
            getEditProvider().addCommand(cmd, false);
        }
    }
    else if (getMode() == MODE_SELECT)
    {
        // Handle the mouse press and bring the new selection to front.
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Check if the connect mode is active.
    if (getMode() == MODE_CONNECT || getMode() == MODE_TOGGLE_OFFPAGE)
    {
        // Find out if there is an end point currently under the cursor.
        HWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<HWConnectionEndpoint>(mouseEvent->scenePos(), this, GridSize);

        if (tempConnection_)
        {
            // Check if there was an valid endpoint close enough.
            if (endpoint != 0 && tempPotentialEndingEndPoints_.contains(endpoint))
            {
                // Highlight the end point.
                disableHighlight();                
                highlightedEndPoint_ = endpoint;
                highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

            }
            // Disable the highlight if there was no end point close enough.
            else
            {
                disableHighlight();
            }

            // Update the connection.
            Q_ASSERT(tempConnection_->route().size() != 0);

            HWConnection* newTempConnection_ = 0;

            if (highlightedEndPoint_ != 0)
            {
                 newTempConnection_ = new HWConnection(tempConnEndPoint_,
                                                                 highlightedEndPoint_, false,
                                                                 QString(), QString(), QString(), this);
            }
            else
            {
                newTempConnection_ = new HWConnection(tempConnEndPoint_->scenePos(),
                    tempConnEndPoint_->getDirection(), snapPointToGrid(mouseEvent->scenePos()),
                    QVector2D(0.0f, 0.0f), QString(), QString(), this);
            }

            if (tempConnEndPoint_->isAdHoc())
            {
                newTempConnection_->setLineWidth(1);
            }

            removeItem(tempConnection_);
            delete tempConnection_;

            addItem(newTempConnection_);
			tempConnection_ = newTempConnection_;
			return;
        }
        else
        {
            if (highlightedEndPoint_ != 0)
            {
                highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
                highlightedEndPoint_ = 0;
            }

            if (endpoint != 0 && endpoint->isVisible())
            {
                highlightedEndPoint_ = endpoint;
                highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

                if (highlightedEndPoint_->type() == OffPageConnectorItem::Type)
                {
                    hideOffPageConnections();

                    foreach (GraphicsConnection* conn, highlightedEndPoint_->getConnections())
                    {
                        conn->setVisible(true);
                    }
                }
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void HWDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	// process the normal mouse release event
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void HWDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
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

    //clearSelection();

    // This fixes the problem when the user click above a text label or a pixmap but
    // actually wants to select the parent item (such as the actual component, not its label).
    if (item->parentItem() != 0 && item->type() != HWComponentItem::Type &&
        item->type() != BusPortItem::Type && item->type() != BusInterfaceItem::Type &&
        item->type() != HWColumn::Type)
    {
        item = item->parentItem();
    }

    if (item->type() == HWComponentItem::Type)
    {
        item->setSelected(true);
        HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);

        if (getLibraryInterface()->contains(*comp->componentModel()->getVlnv()))
        {
			QString viewName;
			QStringList hierViews = comp->componentModel()->getHierViews();
			
			// if configuration is used and it contains an active view for the instance
			if (getDesignConfiguration() && getDesignConfiguration()->hasActiveView(comp->name())) {
				viewName = getDesignConfiguration()->getActiveView(comp->name());

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
        else if (!isProtected())
        {
            // Otherwise this is an unpackaged component. Check if the bus interfaces are valid.
//             foreach (QSharedPointer<BusInterface> busIf, comp->componentModel()->getBusInterfaces())
//             {
//                 if (!busIf->getBusType().isValid())
//                 {
//                     QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
//                                        tr("Component '%1' cannot be packetized because all bus interfaces "
//                                           "have not been defined.").arg(comp->name()),
//                                        QMessageBox::Ok, (QWidget*)parent());
//                     msgBox.setInformativeText(tr("Define first the buses by dragging from the library "
//                                                  "or double clicking the bus interface."));
//                     
//                     msgBox.exec();
//                     return;
//                 }
//             }
            
            // Request the user to set the vlnv.
            NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, true, (QWidget*)parent());
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
            comp->componentModel()->createEmptyFlatView();

            // Write the model to file.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), comp->componentModel());

            // Update the diagram component.
            comp->updateComponent();

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new ComponentPacketizeCommand(comp, vlnv));
            getEditProvider().addCommand(cmd, false);

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
    else if (item->type() == HWColumn::Type)
    {
        if (!isProtected())
        {
            item->setSelected(true);
            HWColumn* column = qgraphicsitem_cast<HWColumn*>(item);

            ColumnEditDialog dialog((QWidget*)parent(), false, column);

            if (dialog.exec() == QDialog::Accepted)
            {
                int columnWidth = 259;

                if (dialog.getContentType() == COLUMN_CONTENT_IO)
                {
                    columnWidth = 119;
                }

                ColumnDesc desc(dialog.getName(), dialog.getContentType(), dialog.getAllowedItems(), columnWidth);
                QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc));
                getEditProvider().addCommand(cmd);
            }
        }
    }
    else if (item->type() == BusPortItem::Type || item->type() == BusInterfaceItem::Type)
    {
        HWConnectionEndpoint* endpoint = static_cast<HWConnectionEndpoint*>(item);

        // Check if the bus is unpackaged.
        if (endpoint->isBus() &&
            (endpoint->getBusInterface() == 0 || !endpoint->getBusInterface()->getBusType().isValid()))
        {
            // Request the user to set the vlnv.
            NewObjectDialog dialog(getLibraryInterface(), VLNV::BUSDEFINITION, true, (QWidget*)parent());
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
                emit errorMessage(tr("Bus definition %1:%2:%3:%4 was already found in the library").arg(
                    busVLNV.getVendor()).arg(
                    busVLNV.getLibrary()).arg(
                    busVLNV.getName()).arg(
                    busVLNV.getVersion()));
                return;
            }
            else if (getLibraryInterface()->contains(absVLNV)) {
                emit errorMessage(tr("Abstraction definition %1:%2:%3:%4 was already found in the library").arg(
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
            getLibraryInterface()->writeModelToFile(dialog.getPath(), busDef);

            // Create an abstraction definition.
            QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
            absDef->setVlnv(absVLNV);

            // Set the reference from abstraction definition to bus definition.
            absDef->setBusType(busVLNV);

            // Create the file for the abstraction definition.
            getLibraryInterface()->writeModelToFile(dialog.getPath(), absDef);

            // Ask the user for the interface mode.
            endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

            bool editName = endpoint->type() == BusInterfaceItem::Type &&
                            endpoint->getConnections().empty();

            BusInterfaceDialog modeDialog(editName, (QWidget*)parent());
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
                oldAbsType = endpoint->getBusInterface()->getAbstractionType();
                oldMode = endpoint->getBusInterface()->getInterfaceMode();
                oldName = endpoint->getBusInterface()->getName();
            }

            // Set the types for the end point.
            endpoint->setTypes(busVLNV, absVLNV, modeDialog.getSelectedMode());

            if (editName)
            {
                endpoint->getBusInterface()->setName(modeDialog.getName());
            }

            endpoint->updateInterface();

            QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(endpoint, oldBusType,
                                                                      oldAbsType, oldMode, oldName));
            getEditProvider().addCommand(cmd, false);

            emit interfaceSelected(endpoint);
            emit openBus(busVLNV, absVLNV, true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragEnterEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    dragCompType_ = CIT_NONE;
    dragBus_ = false;

    // Allow drag only if the diagram is not locked and the dragged object is a vlnv.
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

            // Check if the implementation does not match with the edited component.
            if (comp->getComponentImplementation() != getEditedComponent()->getComponentImplementation())
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
            QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(*vlnv);
            QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();

            if ((getEditedComponent()->getComponentImplementation() == KactusAttribute::KTS_HW &&
                 busDef->getType() == KactusAttribute::KTS_BUSDEF_HW) ||
                (getEditedComponent()->getComponentImplementation() == KactusAttribute::KTS_SW &&
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
void HWDesignDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    if (dragCompType_ != CIT_NONE)
    {
        GraphicsColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

        if (column != 0 && column->getColumnDesc().getAllowedItems() & dragCompType_)
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
        HWConnectionEndpoint* endpoint =
            DiagramUtil::snapToItem<HWConnectionEndpoint>(event->scenePos(), this, GridSize);

        if (highlightedEndPoint_ != 0)
        {
            highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        }
        
        highlightedEndPoint_ = endpoint;

        // Allow the drop event if the end point is undefined or there are no connections
        // and the encompassing component is unpackaged.
        if (highlightedEndPoint_ != 0 && highlightedEndPoint_->isBus() &&
            (highlightedEndPoint_->getBusInterface() == 0 ||
             !highlightedEndPoint_->getBusInterface()->getBusType().isValid() ||
             (!highlightedEndPoint_->isConnected() &&
              highlightedEndPoint_->encompassingComp() != 0 &&
              !highlightedEndPoint_->encompassingComp()->componentModel()->getVlnv()->isValid())))
        {
            event->setDropAction(Qt::CopyAction);
            highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);
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
void HWDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragCompType_ = CIT_NONE;
    dragBus_ = false;
    
    if (highlightedEndPoint_ != 0)
    {
        highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        highlightedEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Check if the dragged item was a valid one.
    if (dragCompType_ != CIT_NONE)
    {
        GraphicsColumn* column = layout_->findColumnAt(snapPointToGrid(event->scenePos()));

        // Retrieve the vlnv.
        VLNV *vlnv;
        memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

        // Disallow self-instantiation.
        if (*vlnv == *getEditedComponent()->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Component cannot be instantiated to its own design."),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }

        // Create the component model.
		QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(*vlnv);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

		// Set the instance name for the new component instance.
		QString instanceName = createInstanceName(comp);

        // Create the diagram component.
        HWComponentItem *newItem = new HWComponentItem(getLibraryInterface(), comp, instanceName);
        newItem->setPos(snapPointToGrid(event->scenePos()));

        if (column != 0)
        {
            QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, newItem));

			connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
				this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
			connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
				this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

            getEditProvider().addCommand(cmd);
        }
    }
    else if (dragBus_)
    {
        if (highlightedEndPoint_ != 0)
        {
            // Retrieve the busdef VLNV and determine the absdef VLNV.
            VLNV *droppedVlnv;
            memcpy(&droppedVlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(droppedVlnv));

			Q_ASSERT(getLibraryInterface()->contains(*droppedVlnv));

            VLNV vlnv = *droppedVlnv;
			vlnv.setType(getLibraryInterface()->getDocumentType(*droppedVlnv));

			VLNV absdefVLNV;

			// if the dropped was an abstraction definition
			if (vlnv.getType() == VLNV::ABSTRACTIONDEFINITION) {
				absdefVLNV = vlnv;

				// parse the abstraction definition
				QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(vlnv);
				QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

				// get the bus definition referenced by the abstraction definition
				VLNV busdefVLNV = absDef->getBusType();

				// set the bus definition used
				vlnv = busdefVLNV;

				Q_ASSERT(absdefVLNV.getType() == VLNV::ABSTRACTIONDEFINITION);
			}
			Q_ASSERT(vlnv.getType() == VLNV::BUSDEFINITION);

            bool editName = highlightedEndPoint_->type() == BusInterfaceItem::Type &&
                            highlightedEndPoint_->getBusInterface() == 0 &&
                            highlightedEndPoint_->getConnections().empty();

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
                highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
                return;
            }

            highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);

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
            getEditProvider().addCommand(cmd, false);

            highlightedEndPoint_ = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onVerticalScroll(qreal y)
{
	Q_ASSERT(layout_);
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: disableHighlight()
//-----------------------------------------------------------------------------
void HWDesignDiagram::disableHighlight()
{
    if (highlightedEndPoint_ != 0)
    {
        if (tempConnEndPoint_ != 0 && highlightedEndPoint_ != tempConnEndPoint_)
        {
            highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_ALLOWED);
        }
        else
        {
            highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        }

        highlightedEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addColumn(ColumnDesc const& desc)
{
    HWColumn* column = new HWColumn(desc, layout_.data(), this);

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(layout_.data(), column));
    getEditProvider().addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: removeColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::removeColumn(GraphicsColumn* column)
{
    QSharedPointer<QUndoCommand> cmd(new ColumnDeleteCommand(layout_.data(), column));
    getEditProvider().addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        HWDesignWidget* doc = static_cast<HWDesignWidget*>(parent());
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
// Function: onSelected()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection)
    { 
        // Check if the selected item was a component.
        if (newSelection->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = qgraphicsitem_cast<HWComponentItem *>(newSelection);
            emit componentSelected(comp);
        }
        // Check if the selected item was a port.
        else if (newSelection->type() == BusPortItem::Type)
        {
            BusPortItem* port = qgraphicsitem_cast<BusPortItem*>(newSelection);
            emit interfaceSelected(port);
        }
        // Check if the selected item was an interface.
        else if (newSelection->type() == BusInterfaceItem::Type)
        {
            BusInterfaceItem* interface = qgraphicsitem_cast<BusInterfaceItem*>(newSelection);
            emit interfaceSelected(interface);
        }
        // Check if the selected item was an ad-hoc port.
        else if (newSelection->type() == AdHocPortItem::Type)
        {
            AdHocPortItem* adHocPort = static_cast<AdHocPortItem*>(newSelection);
            emit interfaceSelected(adHocPort);
        }
        // Check if the selected item was an ad-hoc interface.
        else if (newSelection->type() == AdHocInterfaceItem::Type)
        {
            AdHocInterfaceItem* adHocIf = static_cast<AdHocInterfaceItem*>(newSelection);
            emit interfaceSelected(adHocIf);
        }
		// check if the selected item was a connection
		else if (newSelection->type() == HWConnection::Type) {
			HWConnection* connection = qgraphicsitem_cast<HWConnection*>(newSelection);
			emit connectionSelected(connection);
		}
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
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout* HWDesignDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: addInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addInterface(GraphicsColumn* column, QPointF const& pos)
{
	QSharedPointer<BusInterface> busif(new BusInterface());
    BusInterfaceItem *newItem = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), busif, 0);
    newItem->setPos(snapPointToGrid(pos));

    connect(newItem, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

    // Save the positions of the other diagram interfaces.
    QMap<BusInterfaceItem*, QPointF> oldPositions;

    if (column->getContentType() == COLUMN_CONTENT_IO)
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
    if (column->getContentType() == COLUMN_CONTENT_IO)
    {
        QMap<BusInterfaceItem*, QPointF>::iterator cur = oldPositions.begin();

        while (cur != oldPositions.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                QUndoCommand* childCmd = new ItemMoveCommand(cur.key(), cur.value(), cmd.data());
            }

            ++cur;
        }
    }

    getEditProvider().addCommand(cmd, false);
}

HWDesignWidget* HWDesignDiagram::parent() const {
	return parent_;
}

//-----------------------------------------------------------------------------
// Function: createConnection()
//-----------------------------------------------------------------------------
void HWDesignDiagram::createConnection(QGraphicsSceneMouseEvent * mouseEvent)
{
    // Disable highlights from all potential end points.
    for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
    {
        tempPotentialEndingEndPoints_.at(i)->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
    }
    tempPotentialEndingEndPoints_.clear();

    ConnectionEndpoint* endpoint =
        DiagramUtil::snapToItem<ConnectionEndpoint>(mouseEvent->scenePos(), this, GridSize);

    // Check if there is no end point close enough to the cursor or the
    // end points cannot be connected together.
    if (endpoint == 0 || endpoint == tempConnEndPoint_ || !endpoint->isVisible() ||
        !endpoint->canConnect(tempConnEndPoint_) ||
        !tempConnEndPoint_->canConnect(endpoint)) {

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
        bool firstOffPage = tempConnEndPoint_->type() == OffPageConnectorItem::Type;
        bool secondOffPage = endpoint->type() == OffPageConnectorItem::Type;

        if (offPageMode_ ||
            ((firstOffPage || secondOffPage) && tempConnEndPoint_->type() != endpoint->type()))
        {
            delete tempConnection_;

            if (!firstOffPage)
            {
                tempConnEndPoint_ = tempConnEndPoint_->getOffPageConnector();
            }

            if (!secondOffPage)
            {
                endpoint = endpoint->getOffPageConnector();
            }

            tempConnection_ = new HWConnection(tempConnEndPoint_, endpoint, false,
                                                         QString(), QString(), QString(), this);
            addItem(tempConnection_);
        }

        if (tempConnection_->connectEnds())
        {
            QSharedPointer<QUndoCommand> cmd(new ConnectionAddCommand(this, tempConnection_));
            getEditProvider().addCommand(cmd, false);

            tempConnection_ = 0;

            if (!offPageMode_)
            {
                tempConnEndPoint_ = 0;
            }
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
void HWDesignDiagram::keyReleaseEvent(QKeyEvent *event)
{
    // Check if the user ended the off-page connection mode.
    if ((event->key() == Qt::Key_Shift) && offPageMode_)
    {
        if (tempConnEndPoint_ != 0)
        {
            if (tempConnEndPoint_->getConnections().size() == 0)
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
void HWDesignDiagram::endConnect()
{
    // Destroy the connection that was being drawn.
    if (tempConnection_) {
        delete tempConnection_;
        tempConnection_ = 0;
    }

    // Disable highlights from all end points.
    if (highlightedEndPoint_ != 0)
    {
        highlightedEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        highlightedEndPoint_ = 0;
    }

    for (int i = 0 ; i < tempPotentialEndingEndPoints_.size(); ++i)
    {
        tempPotentialEndingEndPoints_.at(i)->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
    }
    tempPotentialEndingEndPoints_.clear();

    offPageMode_ = false;
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onSelectionChanged()
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
        if (oldSelection_->type() == OffPageConnectorItem::Type)
        {
            OffPageConnectorItem* connector = static_cast<OffPageConnectorItem*>(oldSelection_);

            foreach (GraphicsConnection* conn, connector->getConnections())
            {
                if (conn != newSelection)
                {
                    conn->setVisible(false);
                }
            }
        }
        else if (oldSelection_->type() == HWConnection::Type && oldSelection_ != newSelection)
        {
            HWConnection* conn = static_cast<HWConnection*>(oldSelection_);

            if (conn->endpoint1() != 0)
            {
                if (conn->endpoint1()->type() == OffPageConnectorItem::Type)
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
    if (newSelection != 0 && newSelection->type() == OffPageConnectorItem::Type)
    {
        OffPageConnectorItem* connector = static_cast<OffPageConnectorItem*>(newSelection);

        foreach (GraphicsConnection* conn, connector->getConnections())
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
void HWDesignDiagram::toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd)
{
    Q_ASSERT(parentCmd != 0);
    emit clearItemSelection();

    // Determine the new end points for the connection.
    ConnectionEndpoint* endpoint1 = conn->endpoint1();
    ConnectionEndpoint* endpoint2 = conn->endpoint2();

    if (endpoint1->type() == OffPageConnectorItem::Type)
    {
        endpoint1 = static_cast<ConnectionEndpoint*>(endpoint1->parentItem());
        endpoint2 = static_cast<ConnectionEndpoint*>(endpoint2->parentItem());
    }
    else
    {
        endpoint1 = endpoint1->getOffPageConnector();
        endpoint2 = endpoint2->getOffPageConnector();
    }

    HWConnection* newConn = new HWConnection(endpoint1, endpoint2, false,
                                             conn->name(), QString(), conn->description(), this);

    // Recreate the connection by first deleting the old and then creating a new one.
    QUndoCommand* cmd = new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), parentCmd);
    cmd->redo();
    
    addItem(newConn);
    connect(newConn, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

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
void HWDesignDiagram::hideOffPageConnections()
{
    foreach (QGraphicsItem* item, items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0 && conn->endpoint1()->type() == OffPageConnectorItem::Type)
        {
            conn->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: destroyConnections()
//-----------------------------------------------------------------------------
void HWDesignDiagram::destroyConnections()
{
    QList<QGraphicsItem*> conns;

    foreach (QGraphicsItem* item, items()) {

        // if the item is an interconnection
        if (item->type() == HWConnection::Type) {
            conns.append(item);
        }
    }

    foreach (QGraphicsItem* item, conns)
    {
        removeItem(item);
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onAdHocVisibilityChanged()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    if (visible)
    {
        AdHocInterfaceItem* adHocIf = new AdHocInterfaceItem(getEditedComponent(),
                                                                   getEditedComponent()->getPort(portName),
                                                                   getLibraryInterface(), 0);

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        layout_->addItem(adHocIf);
    }
    else
    {
        // Search for the ad-hoc interface and delete it.
        ConnectionEndpoint* found = getDiagramAdHocPort(portName);
        Q_ASSERT(found != 0);

        static_cast<GraphicsColumn*>(found->parentItem())->removeItem(found);
        delete found;
        found = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* HWDesignDiagram::getDiagramAdHocPort(QString const& portName)
{
    foreach (QGraphicsItem* item, items())
    {
        if (item->type() == AdHocInterfaceItem::Type &&
            static_cast<AdHocInterfaceItem*>(item)->name() == portName)
        {
            return static_cast<HWConnectionEndpoint*>(item);
        }
    }

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
}

