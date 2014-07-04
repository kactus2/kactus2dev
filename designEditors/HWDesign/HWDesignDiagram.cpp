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

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/portabstraction.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/PortRef.h>
#include <IPXACTmodels/Interconnection.h>
#include <IPXACTmodels/Interface.h>
#include <IPXACTmodels/HierConnection.h>
#include <IPXACTmodels/design.h>

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
// Function: HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::HWDesignDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, HWDesignWidget *parent)
    : ComponentDesignDiagram(lh, editProvider, parent),
      dragCompType_(CIT_NONE),
      dragBus_(false),
      dragEndPoint_(0)
{

}

//-----------------------------------------------------------------------------
// Function: ~HWDesignDiagram()
//-----------------------------------------------------------------------------
HWDesignDiagram::~HWDesignDiagram()
{

}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::loadDesign()
//-----------------------------------------------------------------------------
void HWDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    if (design->getColumns().isEmpty())
    {
        addColumn(ColumnDesc("IO", COLUMN_CONTENT_IO, 0, IO_COLUMN_WIDTH));
        addColumn(ColumnDesc("Buses", COLUMN_CONTENT_BUSES, 0, COMPONENT_COLUMN_WIDTH));
        addColumn(ColumnDesc("Components", COLUMN_CONTENT_COMPONENTS, 0, COMPONENT_COLUMN_WIDTH));
        addColumn(ColumnDesc("IO", COLUMN_CONTENT_IO, 0, IO_COLUMN_WIDTH));
    }
    else
    {
        foreach (ColumnDesc const& desc, design->getColumns())
        {
            HWColumn* column = new HWColumn(desc, getLayout().data());
            getLayout()->addColumn(column, true);
        }
    }

    // Create diagram interfaces for the top-level bus interfaces.
    foreach (QSharedPointer<BusInterface> busIf, getEditedComponent()->getBusInterfaces())
    {
        BusInterfaceItem* diagIf = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), busIf, 0);

        // Add the diagram interface to the first column where it is allowed to be placed.
        getLayout()->addItem(diagIf);
    }

    /* component instances */
    foreach (ComponentInstance const& instance, design->getComponentInstances())
    {        
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

        HWComponentItem* item = new HWComponentItem(getLibraryInterface(), component,
                                                    instance.getInstanceName(),
                                                    instance.getDisplayName(),
                                                    instance.getDescription(),
                                                    instance.getUuid(),
                                                    instance.getConfigurableElementValues(),
                                                    instance.getPortAdHocVisibilities());

        item->setBusInterfacePositions(instance.getBusInterfacePositions(), true);
        item->setAdHocPortPositions(instance.getAdHocPortPositions());
        item->setVendorExtensions(instance.getVendorExtensions());

        // Check if the position is not found.
        if (instance.getPosition().isNull())
        {
            // Migrate from the old layout to the column based layout.
            getLayout()->addItem(item);
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
                getLayout()->addItem(item);
            }
        }

		onComponentInstanceAdded(item);
    }

    /* interconnections */
    foreach(Interconnection interconnection, design->getInterconnections())
    {
        QPair<Interface, Interface> connInterfaces = interconnection.getInterfaces();

		// find the first referenced component
        HWComponentItem *comp1 = getComponent(connInterfaces.first.getComponentRef());

		if (!comp1) {
			emit errorMessage(tr("Component %1 was not found in the design").arg(
			connInterfaces.first.getComponentRef()));
			continue;
		}
		
		// find the second referenced component
        HWComponentItem *comp2 = getComponent(connInterfaces.second.getComponentRef());

		if (!comp2) {
			emit errorMessage(tr("Component %1 was not found in the design").arg(
			connInterfaces.second.getComponentRef()));
			continue;
		}

		// find the port of the first referenced component
        ConnectionEndpoint* port1 = comp1->getBusPort(connInterfaces.first.getBusRef());

		if (!port1)
        {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
			connInterfaces.first.getBusRef()).arg(connInterfaces.first.getComponentRef()));

			port1 = createMissingPort(connInterfaces.first.getBusRef(), comp1, design);
		}
		
		// find the port of the second referenced component
        ConnectionEndpoint* port2 = comp2->getBusPort(connInterfaces.second.getBusRef());

		if (!port2)
        {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
			connInterfaces.second.getBusRef()).arg(connInterfaces.second.getComponentRef()));
			
            port2 = createMissingPort(connInterfaces.second.getBusRef(), comp2, design);
		}

        if (interconnection.isOffPage())
        {
            port1 = port1->getOffPageConnector();
            port2 = port2->getOffPageConnector();
        }

		// if both components and their ports are found an interconnection can be
		// created
		if (comp1 && comp2 && port1 && port2)
        {
			HWConnection *diagramInterconnection =
				new HWConnection(port1, port2, true, QString(), interconnection.displayName(),
                                           interconnection.description(), this);
            diagramInterconnection->setRoute(interconnection.getRoute());
			diagramInterconnection->setName(interconnection.name());

            if (interconnection.isOffPage())
            {
                diagramInterconnection->hide();
            }
            diagramInterconnection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

            connect(diagramInterconnection, SIGNAL(errorMessage(QString const&)), this,
                    SIGNAL(errorMessage(QString const&)));

            addItem(diagramInterconnection);
            diagramInterconnection->updatePosition();
		}
    }

	// Create hierarchical connections.
    QList<QString> connectedHier;
    QList<HierConnection> hierConnections = design->getHierarchicalConnections();

    for (int i = 0; i < hierConnections.size(); ++i)
    {
        HierConnection hierConn = hierConnections.at(i);

		QSharedPointer<BusInterface> busIf = getEditedComponent()->getBusInterface(hierConn.getInterfaceRef());
        ConnectionEndpoint* diagIf = 0;

		// if the bus interface was not found
		if (busIf == 0)
        {
			emit errorMessage(tr("Bus interface %1 was not found in the top-component").arg(hierConn.getInterfaceRef()));

            // Create a dummy interface which is marked as invalid.
            busIf = QSharedPointer<BusInterface>(new BusInterface());
            busIf->setName(hierConn.getInterfaceRef());

            diagIf = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), busIf, 0);
            diagIf->setTemporary(true);
            diagIf->updateInterface();
        }
        else
        {			
            // Find the corresponding diagram interface.
            foreach (QGraphicsItem* item, items())
            {
                if (item->type() == BusInterfaceItem::Type &&
                    static_cast<BusInterfaceItem*>(item)->getBusInterface() == busIf)
                {
                    diagIf = static_cast<BusInterfaceItem*>(item);
                    break;
                }
            }
		}

        Q_ASSERT(diagIf != 0);

		// Check if the position is found.
        if (!hierConn.getPosition().isNull())
        {
            diagIf->setPos(hierConn.getPosition());
            diagIf->setDirection(hierConn.getDirection());

            GraphicsColumn* column = getLayout()->findColumnAt(hierConn.getPosition());
            
            if (column != 0)
            {
                column->addItem(diagIf);
            }
            else
            {
                getLayout()->addItem(diagIf);
            }
        }

		// find the component where the hierarchical connection is connected to
        HWComponentItem *comp = getComponent(hierConn.getInterface().getComponentRef());
		if (!comp) {
			emit errorMessage(tr("Component %1 was not found in the top-design").arg(
				hierConn.getInterface().getComponentRef()));
			continue;
		}

		// find the port of the component
        ConnectionEndpoint* compPort = comp->getBusPort(hierConn.getInterface().getBusRef());
		if (!compPort)
        {
			emit errorMessage(tr("Port %1 was not found in the component %2").arg(
				hierConn.getInterface().getBusRef()).arg(hierConn.getInterface().getComponentRef()));

            compPort = createMissingPort(hierConn.getInterface().getBusRef(), comp, design);
		}        

        if (hierConn.isOffPage())
        {
            compPort = compPort->getOffPageConnector();
            diagIf = diagIf->getOffPageConnector();
        }

        HWConnection* diagConn = new HWConnection(compPort, diagIf, true, QString(), QString(), QString(), this);
        diagConn->setRoute(hierConn.getRoute());

        if (hierConn.isOffPage())
        {
            diagConn->hide();
        }

        diagConn->setVendorExtensions(hierConn.getVendorExtensions());
        diagConn->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

        connect(diagConn, SIGNAL(errorMessage(QString const&)), this,
            SIGNAL(errorMessage(QString const&)));
        connectedHier.append(hierConn.getInterfaceRef());

        addItem(diagConn);
        diagConn->updatePosition();
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
                                                                 getEditedComponent()->getPort(name).data(),
                                                                 getLibraryInterface(), 0);
            if (adHocPortPositions.contains(name))
            {
                QPointF pos = adHocPortPositions[name];
                adHocIf->setPos(pos);
            }

            // Add the ad-hoc interface to the first column where it is allowed to be placed.
            GraphicsColumn* column = getLayout()->findColumnAt(adHocIf->scenePos());

            if (column != 0 && adHocPortPositions.contains(name))
            {
                column->addItem(adHocIf, true);
            }
            else
            {
                getLayout()->addItem(adHocIf);
            }
        }
    }

    // Create ad-hoc connections based on the design data.
    QList<AdHocConnection> adHocConnections = design->getAdHocConnections();

    foreach (AdHocConnection const& adHocConn, adHocConnections)
    {
        // Convert one multiple-port connection to two-port-only connections.
        if (adHocConn.externalPortReferences().empty() && !adHocConn.internalPortReferences().empty())
        {
            // Find the first referenced component.
            HWComponentItem* comp1 = getComponent(adHocConn.internalPortReferences().at(0).getComponentRef());

            if (comp1 == 0)
            {
                emit errorMessage(tr("Component %1 was not found in the design").arg(
                    adHocConn.internalPortReferences().at(0).getComponentRef()));
                continue;
            }

            // Find the corresponding port.
            ConnectionEndpoint* port1 = comp1->getAdHocPort(adHocConn.internalPortReferences().at(0).getPortRef());

            if (port1 == 0)
            {
                emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                    adHocConn.internalPortReferences().at(0).getPortRef()).arg(adHocConn.internalPortReferences().at(0).getComponentRef()));
            }

            if (adHocConn.isOffPage())
            {
                port1 = port1->getOffPageConnector();
            }

            for (int i = 1; i < adHocConn.internalPortReferences().size(); ++i)
            {
                // Find the second referenced component.
                HWComponentItem* comp2 = getComponent(adHocConn.internalPortReferences().at(i).getComponentRef());

                if (comp2 == 0)
                {
                    emit errorMessage(tr("Component %1 was not found in the design").arg(
                        adHocConn.internalPortReferences().at(i).getComponentRef()));
                    continue;
                }

                ConnectionEndpoint* port2 = comp2->getAdHocPort(adHocConn.internalPortReferences().at(i).getPortRef());

                if (port2 == 0)
                {
                    emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                        adHocConn.internalPortReferences().at(i).getPortRef()).arg(adHocConn.internalPortReferences().at(i).getComponentRef()));
                    continue;
                }

                if (adHocConn.isOffPage())
                {
                    port2 = port2->getOffPageConnector();
                }

                // Create the ad-hoc connection graphics item.
                HWConnection* conn = new HWConnection(port1, port2, true, adHocConn.name(),
                                                                          adHocConn.displayName(),
                                                                          adHocConn.description(), this);
                conn->setRoute(adHocConn.getRoute());
                
                conn->setAdHocLeftBound(0, adHocConn.internalPortReferences().at(0).getLeft());
                conn->setAdHocRightBound(0, adHocConn.internalPortReferences().at(0).getRight());

                conn->setAdHocLeftBound(1, adHocConn.internalPortReferences().at(i).getLeft());
                conn->setAdHocRightBound(1, adHocConn.internalPortReferences().at(i).getRight());

                if (adHocConn.isOffPage())
                {
                    conn->hide();
                }
                conn->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

                connect(conn, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

                addItem(conn);
                conn->updatePosition();
            }
        }

        for (int j = 0; j < adHocConn.externalPortReferences().size(); ++j)
        {
            ConnectionEndpoint* adHocIf =
                getDiagramAdHocPort(adHocConn.externalPortReferences().at(j).getPortRef());

            if (adHocIf == 0)
            {
                emit errorMessage(tr("Port %1 was not found in the top-level component").arg(
                                  adHocConn.externalPortReferences().at(j).getPortRef()));
                continue;
            }

            if (adHocConn.isOffPage())
            {
                adHocIf = adHocIf->getOffPageConnector();
            }

            for (int i = 0; i < adHocConn.internalPortReferences().size(); ++i)
            {
                // Find the referenced component.
                HWComponentItem* comp = getComponent(adHocConn.internalPortReferences().at(i).getComponentRef());

                if (comp == 0)
                {
                    emit errorMessage(tr("Component %1 was not found in the design").arg(
                        adHocConn.internalPortReferences().at(i).getComponentRef()));
                    continue;
                }

                ConnectionEndpoint* port = comp->getAdHocPort(adHocConn.internalPortReferences().at(i).getPortRef());

                if (port == 0)
                {
                    emit errorMessage(tr("Port %1 was not found in the component %2").arg(
                        adHocConn.internalPortReferences().at(i).getPortRef()).arg(adHocConn.internalPortReferences().at(i).getComponentRef()));
                    continue;
                }

                if (adHocConn.isOffPage())
                {
                    port = port->getOffPageConnector();
                }

                // Create the ad-hoc connection graphics item.
                HWConnection* conn = new HWConnection(port, adHocIf, true,
                                                                          adHocConn.name(),
                                                                          adHocConn.displayName(),
                                                                           adHocConn.description(), this);
                conn->setRoute(adHocConn.getRoute());

                conn->setAdHocLeftBound(0, adHocConn.internalPortReferences().at(i).getLeft());
                conn->setAdHocRightBound(0, adHocConn.internalPortReferences().at(i).getRight());

                conn->setAdHocLeftBound(1, adHocConn.externalPortReferences().at(j).getLeft());
                conn->setAdHocRightBound(1, adHocConn.externalPortReferences().at(j).getRight());

                if (adHocConn.isOffPage())
                {
                    conn->hide();
                }
                conn->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

                connect(conn, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
                addItem(conn);
                conn->updatePosition();
            }
        }
    }

    // Update the stacking of the columns.
    foreach (GraphicsColumn* column, getLayout()->getColumns())
    {
        column->updateItemPositions();
    }
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
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> HWDesignDiagram::createDesign(const VLNV &vlnv) const
{
	QSharedPointer<Design> design = DesignDiagram::createDesign(vlnv);

	QList<ComponentInstance> instances;
	QList<Interconnection> interconnections;
	QList<HierConnection> hierConnections;
    QList<AdHocConnection> adHocConnections;
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
                                       comp->scenePos(), comp->getUuid());

			// save the configurable element values to the design
			instance.setConfigurableElementValues(comp->getConfigurableElements());
            instance.setPortAdHocVisibilities(comp->getPortAdHocVisibilities());
            instance.setVendorExtensions(comp->getVendorExtensions());

            // Save the port positions.
            QListIterator<QSharedPointer<BusInterface> >
                itrBusIf(comp->componentModel()->getBusInterfaces());

            while (itrBusIf.hasNext())
            {
                QSharedPointer<BusInterface> busif = itrBusIf.next();
                instance.updateBusInterfacePosition(busif->getName(), comp->getBusPort(busif->getName())->pos());
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
				    Interface iface1(conn->endpoint1()->encompassingComp()->name(),
					    conn->endpoint1()->name());
				    Interface iface2(conn->endpoint2()->encompassingComp()->name(),
					    conn->endpoint2()->name());
				    interconnections.append(Interconnection(conn->name(), iface1, iface2,
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
                       HierConnection hierConnection = HierConnection(hierPort->name(),
                            Interface(compPort->encompassingComp()->name(),
                            compPort->name()),
                            hierPort->scenePos(), hierPort->getDirection(),
                            conn->route(),
                            conn->endpoint1()->type() == OffPageConnectorItem::Type);
                        hierConnection.setVendorExtensions(conn->getVendorExtensions());
                        hierConnections.append(hierConnection);
                    }
			    }
            }
            else
            {
                // Otherwise the connection is ad-hoc.
                QList<PortRef> internalPortRefs;
                QList<PortRef> externalPortRefs;

                if (conn->endpoint1()->isHierarchical())
                {
                    externalPortRefs.append(PortRef(conn->endpoint1()->name(), QString(),
                                                            conn->getAdHocLeftBound(0),
                                                            conn->getAdHocRightBound(0)));
                }
                else
                {
                    internalPortRefs.append(PortRef(conn->endpoint1()->name(),
                                                            conn->endpoint1()->encompassingComp()->name(),
                                                            conn->getAdHocLeftBound(0),
                                                            conn->getAdHocRightBound(0)));
                }

                if (conn->endpoint2()->isHierarchical())
                {
                    externalPortRefs.append(PortRef(conn->endpoint2()->name(), QString(),
                                                            conn->getAdHocLeftBound(1),
                                                            conn->getAdHocRightBound(1)));
                }
                else
                {
                    internalPortRefs.append(PortRef(conn->endpoint2()->name(),
                                                            conn->endpoint2()->encompassingComp()->name(),
                                                            conn->getAdHocLeftBound(1),
                                                            conn->getAdHocRightBound(1)));
                }

                adHocConnections.append(AdHocConnection(conn->name(), QString(),
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

    foreach(GraphicsColumn* column, getLayout()->getColumns())
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

    getEditedComponent()->setBusInterfaces(busIfs);
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addColumn(ColumnDesc const& desc)
{
    HWColumn* column = new HWColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(getLayout().data(), column));
    getEditProvider().addCommand(cmd);
    cmd->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onAdHocVisibilityChanged()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    if (visible)
    {
        AdHocInterfaceItem* adHocIf = new AdHocInterfaceItem(getEditedComponent(),
            getEditedComponent()->getPort(portName).data(),
            getLibraryInterface(), 0);

        // Add the ad-hoc interface to the first column where it is allowed to be placed.
        getLayout()->addItem(adHocIf);
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
void HWDesignDiagram::onPasteAction(){

    if (!isProtected())
    {
        QList<QGraphicsItem*> items = selectedItems();
        int type = getCommonItemType(items);

        if (type == HWComponentItem::Type)
        {
            HWComponentItem* targetComp = static_cast<HWComponentItem*>(items[0]);

            // Paste interfaces only to draft components.
            if ( !targetComp->componentModel()->getVlnv()->isValid() )
            {
                QMimeData const* mimedata = QApplication::clipboard()->mimeData();

                if ( mimedata->hasImage() && mimedata->imageData().canConvert<BusInterfaceCollectionCopyData>())
                {				
                    BusInterfaceCollectionCopyData collection = mimedata->imageData().value<BusInterfaceCollectionCopyData>();

                    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                    pasteInterfaces(collection, targetComp, cmd.data());
                    getEditProvider().addCommand(cmd);

                    // Update sidebar view.
                    emit componentSelected(targetComp);
                    emit helpUrlRequested("hwdesign/hwinstance.html");
                }
            }	
        }
        else
        {
            // Check the mime data.
            QMimeData const* mimeData = QApplication::clipboard()->mimeData();

            if (mimeData->hasImage())
            {
                // Paste components.
                if (mimeData->imageData().canConvert<ComponentCollectionCopyData>() && items.empty())
                {
                    GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());

                    if (column != 0)
                    {
                        ComponentCollectionCopyData collection = mimeData->imageData().value<ComponentCollectionCopyData>();

                        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                        pasteInstances(collection, column, cmd.data(), true);
                        getEditProvider().addCommand(cmd);
                    }
                }
                // Paste columns.
                else if (mimeData->imageData().canConvert<ColumnCollectionCopyData>() &&
                    (items.empty() || type == HWColumn::Type))
                {
                    ColumnCollectionCopyData collection = mimeData->imageData().value<ColumnCollectionCopyData>();
                    QSharedPointer<QUndoCommand> parentCmd(new QUndoCommand());

                    foreach (ColumnCopyData const& columnData, collection.columns)
                    {
                        HWColumn* column = new HWColumn(columnData.desc, getLayout().data());

                        QUndoCommand* columnCmd = new GraphicsColumnAddCommand(getLayout().data(), column, parentCmd.data());
                        columnCmd->redo();

                        pasteInstances(columnData.components, column, parentCmd.data(), false);
                        pasteInterfaces(columnData.interfaces, column, parentCmd.data(), false);
                    }

                    getEditProvider().addCommand(parentCmd);
                    // Update sidebar view.
                    emit clearItemSelection();            
                }
                // Paste top-level interfaces.
                else if (mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>() &&
                    items.empty()) 
                {
                    GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());

                    if (column != 0 )
                    {
                        BusInterfaceCollectionCopyData collection = mimeData->imageData().value<BusInterfaceCollectionCopyData>();

                        QSharedPointer<QUndoCommand> cmd(new QUndoCommand());
                        pasteInterfaces(collection, column, cmd.data(), true);
                        if (cmd->childCount() > 0)
                        {
                            getEditProvider().addCommand(cmd); 
                        }
                        // Update sidebar view .
                        emit clearItemSelection();       
                    }
                }                
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::onAddAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::onAddToLibraryAction()
{
    if (singleSelection())
    {
        QGraphicsItem *item = selectedItems().first();
        if (item != 0 && item->type() == HWComponentItem::Type)
        {
            item->setSelected(true);
            HWComponentItem *comp = qgraphicsitem_cast<HWComponentItem *>(item);

            // Set the instance name as default name suggestion.
            comp->componentModel()->getVlnv()->setName(comp->name());

            NewObjectDialog dialog(getLibraryInterface(), VLNV::COMPONENT, true, getParent());
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

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new ComponentPacketizeCommand(comp, vlnv));
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
    if (component->componentModel()->hasView(viewName)) {
        emit openDesign(*component->componentModel()->getVlnv(), viewName);
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

        if (getLibraryInterface()->contains(*comp->componentModel()->getVlnv()))
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

                if (dialog.getContentType() == COLUMN_CONTENT_IO)
                {
                    columnWidth = IO_COLUMN_WIDTH;
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

        BusInterfaceDialog modeDialog(editName, getParent());
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
        getEditProvider().addCommand(cmd);

        emit interfaceSelected(endpoint);
        emit openBus(busVLNV, absVLNV, true);
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
        else if (vlnv.getType() == VLNV::BUSDEFINITION || 
			     vlnv.getType() == VLNV::ABSTRACTIONDEFINITION)
        {
            dragBus_ = true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: dragLeaveEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dragLeaveEvent(QGraphicsSceneDragDropEvent*)
{
    dragCompType_ = CIT_NONE;
    dragBus_ = false;
    
    if (dragEndPoint_)
    {
        dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        dragEndPoint_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: dropEvent()
//-----------------------------------------------------------------------------
void HWDesignDiagram::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // Retrieve the vlnv.
    QVariant data = event->mimeData()->imageData();
    if (!data.canConvert<VLNV>()) {
        return;
    }

    VLNV droppedVLNV = data.value<VLNV>();
    updateDropAction(event);

    // Check if the dragged item was a valid one.
    if (dragCompType_ != CIT_NONE)
    {
        // Disallow self-instantiation.
        if (droppedVLNV == *getEditedComponent()->getVlnv())
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("Component cannot be instantiated to its own design."), QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }

        // Create the component model.
        QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
        QSharedPointer<Component> comp = libComp.staticCast<Component>();

        // Disallow instantiation of components marked as template.
        if (comp->getComponentFirmness() == KactusAttribute::KTS_TEMPLATE)
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Template component cannot be directly instantiated in a design. "
                                  "Save the template component as a new non-template component to enable instantiation."),
                               QMessageBox::Ok, getParent());
            msgBox.exec();
            return;
        }

        // Set the instance name for the new component instance.
        QString instanceName = createInstanceName(comp);

        // Act based on the selected drop action.
        if (event->dropAction() == Qt::CopyAction)
        {
            // Create the diagram component.
            HWComponentItem *newItem = new HWComponentItem(getLibraryInterface(), comp, instanceName);
            newItem->setPos(snapPointToGrid(event->scenePos()));

            GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));

            if (column != 0)
            {
                QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, newItem));

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
            HWComponentItem* oldCompItem = static_cast<HWComponentItem*>(getTopmostComponent(event->scenePos()));
            Q_ASSERT(oldCompItem != 0);

            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("Component instance '%1' is about to be replaced "
                                  "with an instance of %2. Continue and replace?").arg(oldCompItem->name(), droppedVLNV.toString()),
                               QMessageBox::Yes | QMessageBox::No, getParent());
            
            if (msgBox.exec() == QMessageBox::Yes)
            {
                // Create the component item.
                HWComponentItem *newCompItem = new HWComponentItem(getLibraryInterface(), comp, instanceName);

                // Perform the replacement.
                QSharedPointer<ReplaceComponentCommand> cmd(new ReplaceComponentCommand(oldCompItem, newCompItem, 
                    false, false));
                connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
                    this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
                connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
                    this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

                getEditProvider().addCommand(cmd);
                cmd->redo();
            }
        }
    }
    else if (dragBus_)
    {
        if (dragEndPoint_ != 0)
        {
            Q_ASSERT(getLibraryInterface()->contains(droppedVLNV));
			droppedVLNV.setType(getLibraryInterface()->getDocumentType(droppedVLNV));

			VLNV absdefVLNV;

			// if the dropped was an abstraction definition
			if (droppedVLNV.getType() == VLNV::ABSTRACTIONDEFINITION) {
				absdefVLNV = droppedVLNV;

				// parse the abstraction definition
				QSharedPointer<LibraryComponent> libComp = getLibraryInterface()->getModel(droppedVLNV);
				QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

				// get the bus definition referenced by the abstraction definition
				VLNV busdefVLNV = absDef->getBusType();

				// set the bus definition used
				droppedVLNV = busdefVLNV;

				Q_ASSERT(absdefVLNV.getType() == VLNV::ABSTRACTIONDEFINITION);
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

            if (dialog.exec() == QDialog::Rejected)
            {
                dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
                return;
            }

            dragEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);

            // Save old settings.
            VLNV oldBusType;
            VLNV oldAbsType;
            General::InterfaceMode oldMode = General::MASTER;
            QString oldName = "";

            if (dragEndPoint_->getBusInterface() != 0)
            {
                oldBusType = dragEndPoint_->getBusInterface()->getBusType();
                oldAbsType = dragEndPoint_->getBusInterface()->getAbstractionType();
                oldMode = dragEndPoint_->getBusInterface()->getInterfaceMode();
                oldName = dragEndPoint_->getBusInterface()->getName();
            }

            // Define the bus for the end point.
            dragEndPoint_->setTypes(droppedVLNV, absdefVLNV, dialog.getSelectedMode());
            
            if (editName)
            {
                dragEndPoint_->getBusInterface()->setName(dialog.getName());
            }

            dragEndPoint_->updateInterface();

            // Create an undo command.
            QSharedPointer<QUndoCommand> cmd(new EndPointTypesCommand(dragEndPoint_,
                                                                      oldBusType, oldAbsType,
                                                                      oldMode, oldName));
            getEditProvider().addCommand(cmd);

            dragEndPoint_ = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::updateDropAction()
//-----------------------------------------------------------------------------
void HWDesignDiagram::updateDropAction(QGraphicsSceneDragDropEvent* event)
{
    if (dragCompType_ != CIT_NONE)
    {
        // Find the topmost component under the cursor.
        ComponentItem* item = getTopmostComponent(event->scenePos());

        // If the item is a HW component, determine whether the component can be replaced with the dragged one.
        if (item != 0 && item->type() == HWComponentItem::Type)
        {
            event->setDropAction(Qt::MoveAction);
        }
        else
        {
            // Otherwise check whether the component can be placed to the underlying column.
            GraphicsColumn* column = getLayout()->findColumnAt(snapPointToGrid(event->scenePos()));

            if (column != 0 && column->getColumnDesc().getAllowedItems() & dragCompType_)
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
                (!dragEndPoint_->isConnected() && dragEndPoint_->encompassingComp() != 0 && !dragEndPoint_->encompassingComp()->componentModel()->getVlnv()->isValid())))
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

    QList<QGraphicsItem*> items = selectedItems();
    if (items.empty())
    {
        GraphicsColumn* column = getLayout()->findColumnAt(contextMenuPosition());

        pasteEnabled = (mimeData->imageData().canConvert<ColumnCollectionCopyData>() || 
            (column != 0 && mimeData->imageData().canConvert<ComponentCollectionCopyData>()) ||                                    
            (column != 0 && mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>()));
    }
    else
    {
        int type = getCommonItemType(items);

        if (type == HWColumn::Type)
        {
            pasteEnabled = mimeData->imageData().canConvert<ColumnCollectionCopyData>();
        }
        else
        {
            pasteEnabled = singleSelection() && draftSelected() &&
                mimeData->imageData().canConvert<BusInterfaceCollectionCopyData>();
        }
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
// Function: onSelected()
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
		else if (newSelection->type() == HWConnection::Type) {
			HWConnection* connection = qgraphicsitem_cast<HWConnection*>(newSelection);
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
    HWConnection* connection = new HWConnection(startPoint, endPoint, false, QString(), QString(), QString(), this);

    if (startPoint->isAdHoc())
    {
        connection->setLineWidth(1);
    }
    connection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

    return connection;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createConnection()
//-----------------------------------------------------------------------------
GraphicsConnection* HWDesignDiagram::createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint)
{
    HWConnection* connection = new HWConnection(startPoint->scenePos(),
        startPoint->getDirection(), endPoint,
        QVector2D(0.0f, 0.0f), QString(), QString(), this);

    if (startPoint->isAdHoc())
    {
        connection->setLineWidth(1);
    }
    connection->setBusWidthVisible(getParent()->getVisibilityControls().value("Bus Widths"));

    return connection;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::createAddCommandForConnection()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> HWDesignDiagram::createAddCommandForConnection(GraphicsConnection* connection)
{
    QSharedPointer<QUndoCommand> command;

    HWConnection* hwConnection = dynamic_cast<HWConnection*>(connection);
    if (hwConnection)
    {
        command = QSharedPointer<QUndoCommand>(new ConnectionAddCommand(this, hwConnection));
    }

    return command;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addTopLevelInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addTopLevelInterface(GraphicsColumn* column, QPointF const& pos)
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
        for (QMap<BusInterfaceItem*, QPointF>::iterator cur = oldPositions.begin(); cur != oldPositions.end(); 
            cur++)
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), column, cmd.data());
            }
        }
    }

    getEditProvider().addCommand(cmd);
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
            if (itemType == CIT_INTERFACE)
            {
                addTopLevelInterface(column, clickedPosition);
            }
            else if (itemType != CIT_NONE)
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
    unsigned int itemType = column->getColumnDesc().getAllowedItems();

    // Check if the item type is ambiguous (interface vs. the rest of the types).
    if (itemType != CIT_INTERFACE && itemType & CIT_INTERFACE)
    {
        // Open a dialog to determine which type of item to create.
        SelectItemTypeDialog dialog(column->getColumnDesc().getAllowedItems(), getParent());

        if (dialog.exec() == QDialog::Accepted)
        {
            itemType = dialog.getSelectedItemType();
        }
        else
        {
            itemType = CIT_NONE;
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
    comp->setComponentImplementation(getEditedComponent()->getComponentImplementation());

    // Create the corresponding diagram component.
    HWComponentItem* diagComp = new HWComponentItem(getLibraryInterface(), comp, name);
    diagComp->setPos(snapPointToGrid(position));

    QSharedPointer<ItemAddCommand> cmd(new ItemAddCommand(column, diagComp));

    connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
        this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
    connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

    getEditProvider().addCommand(cmd);
    cmd->redo();
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::addDraftInterface()
//-----------------------------------------------------------------------------
void HWDesignDiagram::addDraftComponentInterface(HWComponentItem* targetComponent, QPointF const& position)
{
    // The component is unpackaged if it has an invalid vlnv.
    if (!targetComponent->componentModel()->getVlnv()->isValid())
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
        getEditProvider().addCommand(cmd);
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
        // Perform the replacement.
        QSharedPointer<ReplaceComponentCommand> 
            cmd(new ReplaceComponentCommand(destHWComponent, sourceHWComponent, true, true));

        connect(cmd.data(), SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(cmd.data(), SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        getEditProvider().addCommand(cmd);
        cmd->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: createMissingPort()
//-----------------------------------------------------------------------------
BusPortItem* HWDesignDiagram::createMissingPort(QString const& portName, HWComponentItem* component,
                                                QSharedPointer<Design> design)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(portName);

    BusPortItem* port = new BusPortItem(busIf, getLibraryInterface(), false, component);
    component->addPort(port);

    foreach (ComponentInstance const& instance, design->getComponentInstances())
    {
        if (instance.getInstanceName() == component->name())
        {
            port->setPos(instance.getBusInterfacePositions().value(portName));
            component->onMovePort(port);
            return port;
        }
    }

    return port;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::copyInstances()
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
            instance.mode = busPort->getBusInterface()->getInterfaceMode();
            instance.instanceName = busPort->name();
            instance.description = busPort->description();
            instance.pos = busPort->pos();
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
            instance.instanceName = comp->name();
            instance.displayName = comp->displayName();
            instance.description = comp->description();
            instance.pos = comp->pos();
            instance.configurableElements = comp->getConfigurableElements();
            instance.busInterfacePositions = comp->getBusInterfacePositions();
            instance.adHocPortPositions = comp->getAdHocPortPositions();
            instance.adHocVisibilities = comp->getPortAdHocVisibilities();
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
        QString uniqueBusName = instance.busInterface->getName();        	
        unsigned int count =  0;	
        while( component->getBusPort( uniqueBusName ) != 0 )
        {
            count++;
            uniqueBusName = instance.busInterface->getName() + "_" + QString::number(count);			
        }

        // Create a copy of the busInterface and rename it.
        QSharedPointer<BusInterface> copyBusIf(new BusInterface(*instance.busInterface));
        copyBusIf->setName(uniqueBusName);
        copyBusIf->setDescription(instance.description);
        copyBusIf->setInterfaceMode(instance.mode);
        copyBusIf->setDefaultPos(instance.busInterface->getDefaultPos());

        // Create a bus port with the copied bus interface.
        BusPortItem* port = new BusPortItem(copyBusIf, getLibraryInterface(), false, component);        			
        port->setPos(snapPointToGrid(component->mapFromScene(contextMenuPosition())));
        
        // Lock the interface type for non-draft interfaces.
        if (copyBusIf->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
        {
            port->setTypeLocked(true);
        }

        QMap<QString, QPointF> oldLocations = component->getBusInterfacePositions();

        PortPasteCommand* pasteCmd = new PortPasteCommand(component, instance.srcComponent, instance.pos, port, cmd);
        pasteCmd->redo();

        QMap<QString, QPointF>::iterator cur = oldLocations.begin();

        while (cur != oldLocations.end())
        {
            if (component->getBusPort(cur.key())->pos() != cur.value())
            {
                new PortMoveCommand(component->getBusPort(cur.key()), cur.value(), pasteCmd);
            }
            ++cur;
        }  
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteInterfaces()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
                                      GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos)
{
    foreach(BusInterfaceCopyData const& instance, collection.instances)
    {
        QList<QString> existingNames;

        foreach (QGraphicsItem* item, items())
        {
            if (item->type() == BusInterfaceItem::Type)
            {
                BusInterfaceItem* interface = static_cast<BusInterfaceItem*>(item);
                existingNames.append(interface->name());
            }
        }

        // Bus interface must have a unique name within the component.
        QString uniqueBusName = instance.busInterface->getName();
        unsigned int count =  0;

        while (existingNames.contains(uniqueBusName))
        {
            count++;
            uniqueBusName = instance.busInterface->getName() + "_" + QString::number(count);			
        }

        GraphicsColumn* ioColumn = column;
        // Check if the column is not for IO.
        if (!ioColumn->getContentType() == COLUMN_CONTENT_IO)
        {
            ioColumn = 0;

            // Find the first column that is.
            foreach (GraphicsColumn* otherColumn, getLayout()->getColumns())
            {
                if (otherColumn->getContentType() == COLUMN_CONTENT_IO)
                {
                    ioColumn = otherColumn;
                    break;
                }
            }
        }

        if(ioColumn != 0)
        {          
            // Create a copy of the busInterface and rename it.
            QSharedPointer<BusInterface> copyBusIf(new BusInterface(*instance.busInterface));
            copyBusIf->setName(uniqueBusName);
            copyBusIf->setDescription(instance.description);

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
                if (dialog.exec() == QDialog::Rejected)
                {                   
                    continue;
                }
                copyBusIf->setPortMaps(dialog.getPortMaps());
                // Create a busPort with the copied bus interface.
                port = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), 
                    copyBusIf, ioColumn);
                pasteCmd = new BusInterfacePasteCommand(instance.srcComponent, 
                    getEditedComponent(), port, ioColumn, dialog.getPorts(), cmd);
            }
            else
            {
                // Create a busPort with the copied bus interface.
                port = new BusInterfaceItem(getLibraryInterface(), getEditedComponent(), 
                    copyBusIf, ioColumn);
                pasteCmd = new BusInterfacePasteCommand(instance.srcComponent, 
                    getEditedComponent(), port, ioColumn, cmd); 
            }

            if (useCursorPos)
            {
                port->setPos(contextMenuPosition());
            }
            else
            {
                port->setPos(instance.pos);
            }

            connect(port, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));

            // Store the positions of other interfaces.
            QMap<BusInterfaceItem*, QPointF> oldPositions;
            foreach (QGraphicsItem* item, ioColumn->childItems())
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
                    new ItemMoveCommand(cur.key(), cur.value(), ioColumn, pasteCmd);
                }
                ++cur;
            }  
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::pasteInstances()
//-----------------------------------------------------------------------------
void HWDesignDiagram::pasteInstances(ComponentCollectionCopyData const& collection,
                                     GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos)
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

        HWComponentItem* comp = new HWComponentItem(getLibraryInterface(),
                                                    component,
                                                    instanceName,
                                                    instance.displayName,
                                                    instance.description,
                                                    QString(),
                                                    instance.configurableElements,
                                                    instance.adHocVisibilities);

        if (useCursorPos)
        {
            comp->setPos(contextMenuPosition());
        }
        else
        {
            comp->setPos(instance.pos);
        }

        comp->setBusInterfacePositions(instance.busInterfacePositions, false);
        comp->setAdHocPortPositions(instance.adHocPortPositions);

        GraphicsColumn* compColumn = column;

        // Check if the column does not accept the given component.
        if (!column->isItemAllowed(comp))
        {
            compColumn = 0;

            // Find the first column that accepts it.
            foreach (GraphicsColumn* otherColumn, getLayout()->getColumns())
            {
                if (otherColumn->isItemAllowed(comp))
                {
                    compColumn = otherColumn;
                    break;
                }
            }
        }

        if (compColumn != 0)
        {
            ItemAddCommand* childCmd = new ItemAddCommand(compColumn, comp, cmd);

            connect(childCmd, SIGNAL(componentInstantiated(ComponentItem*)),
                this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
            connect(childCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
                this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

            childCmd->redo();
        }
    }
}
