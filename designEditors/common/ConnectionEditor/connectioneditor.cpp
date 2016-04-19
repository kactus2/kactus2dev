//-----------------------------------------------------------------------------
// File: connectioneditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.10.2011
//
// Description:
// Editor to display/edit details of a connection.
//-----------------------------------------------------------------------------

#include "connectioneditor.h"

#include "AdHocBoundsDelegate.h"
#include "AdHocBoundColumns.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/AdHocConnectionItem.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/validators/namevalidator.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QHeaderView>
#include <QList>
#include <QStringList>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::ConnectionEditor()
//-----------------------------------------------------------------------------
ConnectionEditor::ConnectionEditor(QWidget *parent, LibraryInterface* handler):
QWidget(parent),
    type_(this),
    absType_(this),
    instanceLabel_(tr("Connected interfaces:"), this),
    connectedInstances_(this),
    separator_(this),
    nameLabel_(tr("Connection name:"), this),
    nameEdit_(this),
    descriptionLabel_(tr("Description:"), this),
    descriptionEdit_(this),
    portsLabel_(tr("Connected physical ports:"), this),
    portWidget_(this),
    connection_(NULL),
    diagram_(0),
    handler_(handler),
    adHocBoundsTable_(this),
    adHocBoundsModel_(this)
{
	Q_ASSERT(parent);
	Q_ASSERT(handler);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	type_.setTitle(tr("Bus type VLNV"));
	type_.setFlat(false);
	absType_.setTitle(tr("Abstraction type VLNV"));
	absType_.setFlat(false);

	separator_.setFlat(true);

    nameEdit_.setValidator(new NameValidator(&nameEdit_));

	// There are always 2 columns.
	portWidget_.setColumnCount(2);
	portWidget_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("instance1")));
	portWidget_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("instance2")));
	portWidget_.horizontalHeader()->setStretchLastSection(true);
	portWidget_.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	portWidget_.verticalHeader()->hide();
	portWidget_.setEditTriggers(QAbstractItemView::NoEditTriggers);

	descriptionEdit_.setMaximumHeight(50);

    // Set settings for the table view.
    adHocBoundsTable_.setSortingEnabled(true);
    adHocBoundsTable_.setSelectionMode(QAbstractItemView::SingleSelection);
    adHocBoundsTable_.setItemDelegate(new AdHocBoundsDelegate(this));
    adHocBoundsTable_.verticalHeader()->hide();

    QSortFilterProxyModel* adHocPortProxy = new QSortFilterProxyModel(this);
    adHocPortProxy->setSourceModel(&adHocBoundsModel_);
    adHocBoundsTable_.setModel(adHocPortProxy);

    adHocBoundsTable_.setColumnWidth(AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT, 70);
    adHocBoundsTable_.setColumnWidth(AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT, 70);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT, 
        QHeaderView::Fixed);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT,
        QHeaderView::Fixed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&type_);
	layout->addWidget(&absType_);
	layout->addWidget(&instanceLabel_);
	layout->addWidget(&connectedInstances_);
	layout->addWidget(&separator_);
	layout->addWidget(&nameLabel_);
	layout->addWidget(&nameEdit_);
	layout->addWidget(&descriptionLabel_);
	layout->addWidget(&descriptionEdit_);
	layout->addWidget(&portsLabel_);
	layout->addWidget(&portWidget_, 1);
    layout->addWidget(&adHocBoundsTable_, 1);
    layout->addStretch(1);

	clear();
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::~ConnectionEditor()
//-----------------------------------------------------------------------------
ConnectionEditor::~ConnectionEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::clear()
//-----------------------------------------------------------------------------
void ConnectionEditor::clear()
{
	if (connection_)
    {
		disconnect(connection_, SIGNAL(destroyed(GraphicsConnection*)),	this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()), this, SLOT(refresh()));
		connection_ = 0;
	}

	disconnect(&nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNameOrDescriptionChanged()));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onNameOrDescriptionChanged()));

	// clear the contents of the editors
	type_.setVLNV(VLNV(), true);
	absType_.setVLNV(VLNV(), true);
	connectedInstances_.clear();
	nameEdit_.clear();
	descriptionEdit_.clear();
	portWidget_.clearContents();
    adHocBoundsModel_.setConnection(0, QSharedPointer<IEditProvider>());

	// set objects as hidden
	type_.hide();
	absType_.hide();
	instanceLabel_.hide();
	connectedInstances_.hide();
	separator_.hide();
	nameLabel_.hide();
	nameEdit_.hide();
	descriptionLabel_.hide();
	descriptionEdit_.hide();
	portsLabel_.hide();
	portWidget_.hide();
    adHocBoundsTable_.hide();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::refresh()
//-----------------------------------------------------------------------------
void ConnectionEditor::refresh()
{
	Q_ASSERT(connection_);
	setConnection(connection_, diagram_);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::setConnection()
//-----------------------------------------------------------------------------
void ConnectionEditor::setConnection(GraphicsConnection* connection, DesignDiagram* diagram)
{
	Q_ASSERT(connection);

	parentWidget()->raise();

	// disconnect the previous connection
	if (connection_)
    {
		disconnect(connection_, SIGNAL(destroyed(GraphicsConnection*)),	this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()), this, SLOT(refresh()));
	}

	connection_ = connection;
    diagram_ = diagram;

	ConnectionEndpoint* endpoint1 = connection->endpoint1();
    ConnectionEndpoint* endpoint2 = connection->endpoint2();
	Q_ASSERT(endpoint1);

    if (!endpoint1 || !endpoint2)
    {
        return;
    }

    if (endpoint1->isCom())
    {
        type_.setTitle(tr("COM type VLNV"));

        if (endpoint2->isCom())
        {
            type_.setVLNV(endpoint1->getComInterface()->getComType(), true);
        }
        else
        {
            type_.setVLNV(VLNV(), true);
        }
    }
    else if (endpoint1->isApi())
    {
        type_.setTitle(tr("API type VLNV"));

        if (endpoint2->isApi())
        {
            type_.setVLNV(endpoint1->getApiInterface()->getApiType(), true);
        }
        else
        {
            type_.setVLNV(VLNV(), true);
        }
    }
    else if (endpoint1->isBus())
    {
        type_.setTitle(tr("Bus type VLNV"));
	    type_.setVLNV(endpoint1->getBusInterface()->getBusType(), true);
        if (endpoint1->getBusInterface()->getAbstractionTypes() && 
            !endpoint1->getBusInterface()->getAbstractionTypes()->isEmpty() &&
            endpoint1->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef())
        {
            absType_.setVLNV(*endpoint1->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef(), 
                true);
        }	    
    }
    else if (endpoint1->isAdHoc())
    {
        adHocBoundsModel_.setConnection(static_cast<AdHocConnectionItem*>(connection_), diagram->getEditProvider());
        adHocBoundsTable_.resizeRowsToContents();
    }

	// set the names of the connected instances
	connectedInstances_.setText(QString("%1 - %2").arg(endpoint1->name()).arg(endpoint2->name()));

	// set text for the name editor, signal must be disconnected when name is set to avoid loops 
	disconnect(&nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNameOrDescriptionChanged()));
	nameEdit_.setText(connection->name());
	connect(&nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNameOrDescriptionChanged()), Qt::UniqueConnection);

	// display the current description of the interface.
	disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onNameOrDescriptionChanged()));
	descriptionEdit_.setPlainText(connection->description());
	connect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onNameOrDescriptionChanged()), Qt::UniqueConnection);

	connect(connection, SIGNAL(destroyed(GraphicsConnection*)),	this, SLOT(clear()), Qt::UniqueConnection);
	connect(connection, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);

    if (endpoint1->isBus())
    {
	    setPortMaps();
    }

    bool locked = diagram_->isProtected();
	
	// if either end point is hierarchical then there is no description to set
    if (connection_->getConnectionType() != ConnectionEndpoint::ENDPOINT_TYPE_ADHOC &&
        (endpoint1->isHierarchical() || connection->endpoint2()->isHierarchical()))
    {		
		// description exists only for normal interconnections
		descriptionEdit_.setDisabled(true);
		descriptionLabel_.hide();
		descriptionEdit_.hide();

        // name exists for only normal interconnections
        nameEdit_.setDisabled(true);
		nameLabel_.hide();
		nameEdit_.hide();
	}
	else
    {
		descriptionEdit_.setEnabled(!locked);
		descriptionLabel_.show();
		descriptionEdit_.show();

		nameEdit_.setEnabled(!locked);
        nameLabel_.show();
		nameEdit_.show();
	}

    adHocBoundsTable_.setEnabled(!locked);

	// set the objects visible
    instanceLabel_.show();
	connectedInstances_.show();
    separator_.show();

    type_.setVisible(endpoint1->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     endpoint2->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     !endpoint1->isAdHoc());
    absType_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);

    portsLabel_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);
    portWidget_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);
    adHocBoundsTable_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_ADHOC);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::onNameOrDescriptionChanged()
//-----------------------------------------------------------------------------
void ConnectionEditor::onNameOrDescriptionChanged()
{
	Q_ASSERT(connection_);

	disconnect(connection_, SIGNAL(contentChanged()), this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new ConnectionChangeCommand(connection_, nameEdit_.text(), 
        descriptionEdit_.toPlainText()));

	diagram_->getEditProvider()->addCommand(cmd);
    cmd->redo();

	connect(connection_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::setPortMaps()
//-----------------------------------------------------------------------------
void ConnectionEditor::setPortMaps()
{
	Q_ASSERT(connection_);
	
	portWidget_.clearContents();

	// get the interface and component for end point 1
	QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
	Q_ASSERT(busIf1);
	QList<QSharedPointer<PortMap> > portMaps1;
    if (busIf1->getPortMaps())
    {
        portMaps1 = *busIf1->getPortMaps();
    }

	QSharedPointer<Component> comp1 = connection_->endpoint1()->getOwnerComponent();
	Q_ASSERT(comp1);

	// get the interface and component for end point 2
	QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();
	Q_ASSERT(busIf2);
	QList<QSharedPointer<PortMap> > portMaps2;
    if (busIf2->getPortMaps())
    {
        portMaps2 = *busIf2->getPortMaps();
    }

	QSharedPointer<Component> comp2 = connection_->endpoint2()->getOwnerComponent();
	Q_ASSERT(comp2);

	// set the header for end point 1
	ComponentItem* diacomp1 = connection_->endpoint1()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp1)
    {
		portWidget_.horizontalHeaderItem(0)->setText(diacomp1->name());
	}
	else // if was the interface of a top component
    {
		portWidget_.horizontalHeaderItem(0)->setText(comp1->getVlnv().getName());
	}

	// set the header for end point 2
	ComponentItem* diacomp2 = connection_->endpoint2()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp2)
    {
		portWidget_.horizontalHeaderItem(1)->setText(diacomp2->name());
	}
	else // if was the interface of a top component
    {
		portWidget_.horizontalHeaderItem(1)->setText(comp2->getVlnv().getName());
	}

	// turn off sorting when adding items
	portWidget_.setSortingEnabled(false);

	// set the size to be the max value 
	portWidget_.setRowCount(0);

	// get list of all used logical ports
	QStringList logicalNames;
	foreach (QSharedPointer<PortMap> map, portMaps1)
    {
		if (!logicalNames.contains(map->getLogicalPort()->name_))
        {
			logicalNames.append(map->getLogicalPort()->name_);
		}
	}
	foreach (QSharedPointer<PortMap> map, portMaps2)
    {
		if (!logicalNames.contains(map->getLogicalPort()->name_))
        {
			logicalNames.append(map->getLogicalPort()->name_);
		}
	}

    // get the abstraction def for the interfaces
    VLNV absDefVLNV;
    if (busIf1->getAbstractionTypes() && !busIf1->getAbstractionTypes()->isEmpty() &&
        busIf1->getAbstractionTypes()->first()->getAbstractionRef())
    {
        absDefVLNV = *busIf1->getAbstractionTypes()->first()->getAbstractionRef();
    }
    
    QSharedPointer<AbstractionDefinition> absDef;
    if (handler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef = handler_->getModel(absDefVLNV).staticCast<AbstractionDefinition>();
    }

    General::InterfaceMode interfaceMode1 = busIf1->getInterfaceMode();
    General::InterfaceMode interfaceMode2 = busIf2->getInterfaceMode();

	int row = 0;
	// find the physical ports mapped to given logical port
	foreach (QString const& logicalPort, logicalNames)
    {
		bool invalid = false;

		// check that the logical signal is contained in both interface modes used
        if (absDef)
        {
            if (!absDef->hasPort(logicalPort, interfaceMode1) || !absDef->hasPort(logicalPort, interfaceMode2))
            {
                invalid = true;
            }
        }

		foreach (QSharedPointer<PortMap> map1, portMaps1)
        {
			if (map1->getLogicalPort()->name_ == logicalPort) 
            {
				foreach (QSharedPointer<PortMap> map2, portMaps2)
                {
					if (map2->getLogicalPort()->name_ == logicalPort)
                    {
						addMap(row, invalid, map1, comp1, map2, comp2);
					}
				}
			}
		}
	}

	// finally set sorting back on
	portWidget_.setSortingEnabled(true);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::addMap()
//-----------------------------------------------------------------------------
void ConnectionEditor::addMap(int& row, bool invalid, QSharedPointer<PortMap> portMap1,
    QSharedPointer<Component> component1, QSharedPointer<PortMap> portMap2, QSharedPointer<Component> component2)
{
    QSharedPointer<ComponentParameterFinder> firstFinder (new ComponentParameterFinder(component1));
    QSharedPointer<IPXactSystemVerilogParser> firstParser(new IPXactSystemVerilogParser(firstFinder));
    QSharedPointer<ComponentParameterFinder> secondFinder (new ComponentParameterFinder(component2));
    QSharedPointer<IPXactSystemVerilogParser> secondParser(new IPXactSystemVerilogParser(secondFinder));

    QPair<int, int> firstMappedBounds = calculateMappedPhysicalPortBounds(firstParser, portMap1, component1);
    int phys1Left = qMax(firstMappedBounds.first, firstMappedBounds.second);
    int phys1Right = qMin(firstMappedBounds.first, firstMappedBounds.second);
    bool phys1Invalid = isMappedPhysicalInvalid(invalid, portMap1, component1);

    QPair<int, int> secondMappedBounds = calculateMappedPhysicalPortBounds(secondParser, portMap2, component2);
    int phys2Left = qMax(secondMappedBounds.first, secondMappedBounds.second);
    int phys2Right = qMin(secondMappedBounds.first, secondMappedBounds.second);
    bool phys2Invalid = isMappedPhysicalInvalid(invalid, portMap2, component2);

    // check the sizes of the physical ports
	int size1 = phys1Left - phys1Right + 1;
	int size2 = phys2Left - phys2Right + 1;

	QTableWidgetItem* port1Item;
	QTableWidgetItem* port2Item;

    QSharedPointer<PortMap::LogicalPort> firstLogical = portMap1->getLogicalPort();
    QPair<int, int> firstLogicalBounds = calculateMappedLogicalPortBounds(firstParser, portMap1);
    int firstLogicalHigh = firstLogicalBounds.first;
    int firstLogicalLow = firstLogicalBounds.second;

    QSharedPointer<PortMap::LogicalPort> secondLogical = portMap2->getLogicalPort();
    QPair<int, int> secondLogicalBounds = calculateMappedLogicalPortBounds(secondParser, portMap2);
    int secondLogicalHigh = secondLogicalBounds.first;
    int secondLogicalLow = secondLogicalBounds.second;
    if (firstLogical && firstLogical->range_ && secondLogical && secondLogical->range_)
    {
        if (firstLogicalLow > secondLogicalHigh || firstLogicalHigh < secondLogicalLow)
        {
            return;
        }

        int logicalHigh = qMin(firstLogicalHigh, secondLogicalHigh);
        int logicalLow = qMax(firstLogicalLow, secondLogicalLow);

        int firstDownSize = abs(firstLogicalHigh - logicalHigh);
        int firstUpSize = abs(logicalLow - firstLogicalLow);

        size1 = (phys1Left - firstDownSize) - (phys1Right + firstUpSize) + 1;

        QString port1 = General::port2String(
            portMap1->getPhysicalPort()->name_, phys1Left - firstDownSize, phys1Right + firstUpSize);

        int secondDownSize = abs(secondLogicalHigh - logicalHigh);
        int secondUpSize = abs(logicalLow - secondLogicalLow);

        size2 = (phys2Left - secondDownSize) - (phys2Right + secondUpSize) + 1;

        QString port2 = General::port2String(
            portMap2->getPhysicalPort()->name_, phys2Left - secondDownSize, phys2Right + secondUpSize);

        if (size1 != size2)
        {
            phys1Invalid = true;
            phys2Invalid = true;
        }

        port1Item = new QTableWidgetItem(port1);
        port2Item = new QTableWidgetItem(port2);
    }
    else if (portMap1->getLogicalPort() && portMap1->getLogicalPort()->range_ && portMap2->getLogicalPort() &&
        !portMap2->getLogicalPort()->range_)
    {
        QString port1 = General::port2String(portMap1->getPhysicalPort()->name_, phys1Left, phys1Right);
        port1Item = new QTableWidgetItem(port1);

        QString port2 = General::port2String(portMap2->getPhysicalPort()->name_, firstLogicalHigh, firstLogicalLow);

        int firstLogicalSize = abs(firstLogicalHigh - firstLogicalLow) + 1;
        if (firstLogicalSize != size2)
        {
            phys1Invalid = true;
            phys2Invalid = true;
        }

        port2Item = new QTableWidgetItem(port2);
    }
    else if (portMap1->getLogicalPort() && !portMap1->getLogicalPort()->range_ && portMap2->getLogicalPort() &&
        !portMap2->getLogicalPort()->range_)
    {
        QString port1 =
            General::port2String(portMap1->getPhysicalPort()->name_, secondLogicalHigh, secondLogicalLow);

        int secondLogicalSize = abs(secondLogicalHigh - secondLogicalLow) + 1;
        if (secondLogicalSize != size1)
        {
            phys1Invalid = true;
            phys2Invalid = true;
        }
        
        port1Item = new QTableWidgetItem(port1);

        QString port2 = General::port2String(portMap2->getPhysicalPort()->name_, phys2Left, phys2Right);
        port2Item = new QTableWidgetItem(port2);
    }
    else
    {
        QString port1 = General::port2String(portMap1->getPhysicalPort()->name_, phys1Left, phys1Right);
        port1Item = new QTableWidgetItem(port1);

        QString port2 = General::port2String(portMap2->getPhysicalPort()->name_, phys2Left, phys2Right);
        port2Item = new QTableWidgetItem(port2);

        if (size1 != size2)
        {
            phys1Invalid = true;
            phys2Invalid = true;
        }
    }
    
    addPortItemsToPortWidget(port1Item, port2Item, phys1Invalid, phys2Invalid, row);
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::calculateMappedPortBounds()
//-----------------------------------------------------------------------------
QPair<int, int> ConnectionEditor::calculateMappedPhysicalPortBounds(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<PortMap> containingPortMap, QSharedPointer<Component> containingComponent)
{
    int physicalLeft = 0;
    int physicalRight = 0;

    QSharedPointer<PortMap::PhysicalPort> physicalPort = containingPortMap->getPhysicalPort();
    if (physicalPort && physicalPort->partSelect_)
    {
        physicalLeft = parser->parseExpression(physicalPort->partSelect_->getLeftRange()).toInt();
        physicalRight = parser->parseExpression(physicalPort->partSelect_->getRightRange()).toInt();
    }
    else if (containingComponent->hasPort(physicalPort->name_))
    {
        physicalLeft =
            parser->parseExpression(containingComponent->getPort(physicalPort->name_)->getLeftBound()).toInt();
        physicalRight =
            parser->parseExpression(containingComponent->getPort(physicalPort->name_)->getRightBound()).toInt();
    }

    QPair<int, int> portBounds(physicalLeft, physicalRight);
    return portBounds;
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::isPhysicalInvalid()
//-----------------------------------------------------------------------------
bool ConnectionEditor::isMappedPhysicalInvalid(bool invalid, QSharedPointer<PortMap> containingPortMap,
    QSharedPointer<Component> containingComponent)
{
    if (containingPortMap->getPhysicalPort() &&
        !containingComponent->hasPort(containingPortMap->getPhysicalPort()->name_))
    {
        return true;
    }

    return invalid;
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::calculateMappedLogicalPortBounds()
//-----------------------------------------------------------------------------
QPair<int, int> ConnectionEditor::calculateMappedLogicalPortBounds(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<PortMap> containingPortMap)
{
    int logicalHigh = 0;
    int logicalLow = 0;

    if (containingPortMap->getLogicalPort() && containingPortMap->getLogicalPort()->range_)
    {
        int logicalLeft = parser->parseExpression(containingPortMap->getLogicalPort()->range_->getLeft()).toInt();
        int logicalRight = parser->parseExpression(containingPortMap->getLogicalPort()->range_->getRight()).toInt();

        logicalHigh = qMax(logicalLeft, logicalRight);
        logicalLow = qMin(logicalLeft, logicalRight);
    }

    QPair<int, int> logicalBounds(logicalHigh, logicalLow);
    return logicalBounds;
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::addPortItemsToPortWidget()
//-----------------------------------------------------------------------------
void ConnectionEditor::addPortItemsToPortWidget(QTableWidgetItem* firstPortItem, QTableWidgetItem* secondPortItem,
    bool firstIsNotValid, bool secondIsNotValid, int& row)
{
    firstPortItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    secondPortItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (firstIsNotValid)
    {
        firstPortItem->setForeground(QBrush(Qt::red));
    }
    else
    {
        firstPortItem->setForeground(QBrush(Qt::black));
    }
    if (secondIsNotValid)
    {
        secondPortItem->setForeground(QBrush(Qt::red));
    }
    else
    {
        secondPortItem->setForeground(QBrush(Qt::black));
    }

    portWidget_.insertRow(row);
    portWidget_.setItem(row, 0, firstPortItem);
    portWidget_.setItem(row, 1, secondPortItem);
    ++row;
}
