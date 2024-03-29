//-----------------------------------------------------------------------------
// File: connectioneditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.10.2011
//
// Description:
// Editor to display/edit details of a connection.
//-----------------------------------------------------------------------------

#include "connectioneditor.h"

#include "AdHocBoundColumns.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/DesignDiagram.h>

#include <editors/HWDesign/AdHocConnectionItem.h>
#include <editors/HWDesign/HWConnectionEndpoint.h>
#include <editors/HWDesign/HWChangeCommands.h>

#include <KactusAPI/include/ParameterCache.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/common/validators/namevalidator.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QSharedPointer>
#include <QHeaderView>
#include <QList>
#include <QStringList>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::ConnectionEditor()
//-----------------------------------------------------------------------------
ConnectionEditor::ConnectionEditor(LibraryInterface* library, QWidget *parent):
QWidget(parent),
type_(this, VLNV(), true),
absType_(this, VLNV(), true),
instanceLabel_(tr("Connected interfaces:"), this),
connectedInstances_(this),
nameGroup_(this),
nameLabel_(tr("Name:"), this),
nameEdit_(this),
descriptionLabel_(tr("Description:"), this),
descriptionEdit_(this),
portsLabel_(tr("Connected physical ports:"), this),
portWidget_(this),
connection_(nullptr),
diagram_(nullptr),
library_(library),
adHocBoundsTable_(this),
adHocBoundsModel_(this),
connectionTypeTable_(new QStackedWidget())
{
	Q_ASSERT(parent);
	Q_ASSERT(library_);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	type_.setTitle(tr("Bus type VLNV"));
	type_.setFlat(true);
	absType_.setTitle(tr("Abstraction type VLNV"));
	absType_.setFlat(true);

	nameGroup_.setTitle(tr("Connection name"));
    nameGroup_.setFlat(true);

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
    adHocBoundsTable_.verticalHeader()->hide();

    QSortFilterProxyModel* adHocPortProxy = new QSortFilterProxyModel(this);
    adHocPortProxy->setSourceModel(&adHocBoundsModel_);
    adHocBoundsTable_.setModel(adHocPortProxy);

    adHocBoundsTable_.setColumnWidth(AdHocBoundColumns::LEFT_BOUND, 70);
    adHocBoundsTable_.setColumnWidth(AdHocBoundColumns::RIGHT_BOUND, 70);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(AdHocBoundColumns::LEFT_BOUND, QHeaderView::Fixed);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(AdHocBoundColumns::RIGHT_BOUND, QHeaderView::Fixed);

    connectionTypeTable_->addWidget(&portWidget_);
    connectionTypeTable_->addWidget(&adHocBoundsTable_);

    setupLayout();

	clear();
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
		connection_ = nullptr;
	}

	disconnect(&nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNameOrDescriptionChanged()));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onNameOrDescriptionChanged()));

	// clear the contents of the editors
    type_.setVLNV(VLNV());
    absType_.setVLNV(VLNV());
	connectedInstances_.clear();
	nameEdit_.clear();
	descriptionEdit_.clear();
	portWidget_.clearContents();
    adHocBoundsModel_.setConnection(QSharedPointer<AdHocConnection>(), QSharedPointer<IEditProvider>());

	// set objects as hidden
	type_.hide();
	absType_.hide();
	instanceLabel_.hide();
	connectedInstances_.hide();
	nameGroup_.hide();
	portsLabel_.hide();
    connectionTypeTable_->hide();

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

	//parentWidget()->raise();

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
            type_.setVLNV(endpoint1->getComInterface()->getComType());
        }
        else
        {
            type_.setVLNV(VLNV());
        }
    }
    else if (endpoint1->isApi())
    {
        type_.setTitle(tr("API type VLNV"));

        if (endpoint2->isApi())
        {
            type_.setVLNV(endpoint1->getApiInterface()->getApiType());
        }
        else
        {
            type_.setVLNV(VLNV());
        }
    }
    else if (endpoint1->isBus())
    {
        type_.setTitle(tr("Bus type VLNV"));
        type_.setVLNV(endpoint1->getBusInterface()->getBusType());
        absType_.setVLNV(findAbstractionVLNV(endpoint1, endpoint1->getBusInterface()));

        setPortMaps();
        portWidget_.scrollToTop();
    }
    else if (endpoint1->isAdHoc())
    {
        AdHocConnectionItem* adHoc = static_cast<AdHocConnectionItem*>(connection_);
        adHocBoundsModel_.setConnection(adHoc->getInterconnection(), diagram->getEditProvider());
        adHocBoundsTable_.resizeRowsToContents();
    }

	// set the names of the connected instances
	connectedInstances_.setText(QString("%1 - %2").arg(endpoint1->name(), endpoint2->name()));

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

    bool locked = diagram_->isProtected();
	         
    nameEdit_.setDisabled(locked);
    descriptionEdit_.setDisabled(locked);    
    adHocBoundsTable_.setEnabled(!locked);

    bool isAdHocConnection = connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_ADHOC;
    bool isBus = connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS;
       	
    nameGroup_.show();    
    instanceLabel_.show();
	connectedInstances_.show();

    type_.setVisible(endpoint1->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     endpoint2->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     !isAdHocConnection);

    absType_.setVisible(isBus);

    portsLabel_.setVisible(isBus || isAdHocConnection);

    connectionTypeTable_->setVisible(isBus || isAdHocConnection);
    if (isBus)
    {
        connectionTypeTable_->setCurrentIndex(0);
    }
    else if (isAdHocConnection)
    {
        connectionTypeTable_->setCurrentIndex(1);
    }

    portWidget_.resizeColumnsToContents();

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
// Function: ConnectionEditor::findAbstractionVLNV()
//-----------------------------------------------------------------------------
VLNV ConnectionEditor::findAbstractionVLNV(ConnectionEndpoint* endPoint,
    QSharedPointer<BusInterface> busInterface) const
{
    QString activeView = getActiveViewForEndPoint(endPoint);
    QSharedPointer<AbstractionType> abstraction = busInterface->getAbstractionContainingView(activeView);

    VLNV definition;
    if (abstraction && abstraction->getAbstractionRef())
    {
        definition = VLNV(*abstraction->getAbstractionRef());
    }

    return definition;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::setPortMaps()
//-----------------------------------------------------------------------------
void ConnectionEditor::setPortMaps()
{
	portWidget_.clearContents();
    portWidget_.setSortingEnabled(false);
    portWidget_.setRowCount(0);

    Q_ASSERT(connection_);
    if (!connection_)
    {
        return;
    }

    ConnectionEndpoint* endPoint1 = connection_->endpoint1();
    ConnectionEndpoint* endPoint2 = connection_->endpoint2();

    QSharedPointer<BusInterface> busIf1 = endPoint1->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = endPoint2->getBusInterface();

    QSharedPointer<Component> component1 = endPoint1->getOwnerComponent();
    QSharedPointer<Component> component2 = endPoint2->getOwnerComponent();

    Q_ASSERT(busIf1);
    Q_ASSERT(component1);
    Q_ASSERT(busIf2);
    Q_ASSERT(component2);

    setTableHeaders();

    QSharedPointer<ComponentParameterFinder> firstFinder(new ParameterCache(component1));
    QSharedPointer<ComponentParameterFinder> secondFinder(new ParameterCache(component2));

    QSharedPointer<MultipleParameterFinder> parameterFinder(new MultipleParameterFinder());
    parameterFinder->addFinder(firstFinder);
    parameterFinder->addFinder(secondFinder);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    QList<QSharedPointer<PortMap> > portMaps1 = getPortMapsForEndPoint(endPoint1, busIf1);
    QList<QSharedPointer<PortMap> > portMaps2 = getPortMapsForEndPoint(endPoint2, busIf2);

    // Get the abstraction def for the interfaces.
    VLNV absDefVLNV = findAbstractionVLNV(endPoint1, busIf1);
    
    QSharedPointer<AbstractionDefinition> absDef;
    if (library_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef = library_->getModel(absDefVLNV).staticCast<AbstractionDefinition>();
    }

    General::InterfaceMode mode1 = busIf1->getInterfaceMode();
    General::InterfaceMode mode2 = busIf2->getInterfaceMode();

	// find the physical ports mapped to given logical port
	for (QString const& logicalPort : getAllLogicalPorts(portMaps1, portMaps2))
    {
		bool validAbstraction = absDef && absDef->hasPort(logicalPort, mode1) && absDef->hasPort(logicalPort, mode2);

		for (QSharedPointer<PortMap> map1 : portMaps1)
        {
			if (map1->getLogicalPort()->name_.compare(logicalPort) == 0)
            {
				for (QSharedPointer<PortMap> map2 : portMaps2)
                {
					if (map2->getLogicalPort()->name_.compare(logicalPort) == 0)
                    {
                        //! TODO: add checking for mapped tied values.
                        if (map1->getLogicalPort() && map1->getPhysicalPort() &&
                            map2->getLogicalPort() && map2->getPhysicalPort())
                        {
                            addMap(map1, component1, map2, component2, expressionParser, validAbstraction);
                        }
					}
				}
			}
		}
	}

	// Finally, set sorting back on.
	portWidget_.setSortingEnabled(true);
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::getPortMapsForEndPoint()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortMap> > ConnectionEditor::getPortMapsForEndPoint(ConnectionEndpoint* endPoint,
    QSharedPointer<BusInterface> busInterface) const
{
    QString activeView;
    if (endPoint->encompassingComp() && endPoint->encompassingComp()->getComponentInstance())
    {
        activeView = getActiveViewForEndPoint(endPoint);
    }

    return busInterface->getPortMapsForView(activeView);
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::getActiveViewForComponentInstance()
//-----------------------------------------------------------------------------
QString ConnectionEditor::getActiveViewForEndPoint(ConnectionEndpoint* endPoint) const
{
    QString activeView;

    if (diagram_)
    {
        if (!endPoint->encompassingComp())
        {
            activeView = diagram_->getTopView();
        }

        else if (endPoint->encompassingComp() && endPoint->encompassingComp()->getComponentInstance() &&
            diagram_->getDesignConfiguration())
        {
            QSharedPointer<DesignConfiguration> configuration = diagram_->getDesignConfiguration();
            QSharedPointer<ComponentInstance> instance = endPoint->encompassingComp()->getComponentInstance();
            activeView = configuration->getActiveView(instance->getInstanceName());
        }
    }

    return activeView;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::setTableHeaders()
//-----------------------------------------------------------------------------
void ConnectionEditor::setTableHeaders()
{
    ComponentItem* componentItem1 = connection_->endpoint1()->encompassingComp();
    if (componentItem1)
    {
        portWidget_.horizontalHeaderItem(0)->setText(componentItem1->name());
    }
    else // if was the interface of a top component
    {
        QSharedPointer<Component> component1 = connection_->endpoint1()->getOwnerComponent();
        portWidget_.horizontalHeaderItem(0)->setText(component1->getVlnv().getName());
    }

    ComponentItem* componentItem2 = connection_->endpoint2()->encompassingComp();
    if (componentItem2)
    {
        portWidget_.horizontalHeaderItem(1)->setText(componentItem2->name());
    }
    else // if was the interface of a top component
    {
        QSharedPointer<Component> component2 = connection_->endpoint2()->getOwnerComponent();
        portWidget_.horizontalHeaderItem(1)->setText(component2->getVlnv().getName());
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::getAllLogicalPorts()
//-----------------------------------------------------------------------------
QStringList ConnectionEditor::getAllLogicalPorts(QList<QSharedPointer<PortMap> > const& portMaps1,
    QList<QSharedPointer<PortMap> > const& portMaps2) const
{
    QStringList logicalNames;

    for (QSharedPointer<PortMap> map : portMaps1)
    {
        logicalNames.append(map->getLogicalPort()->name_);
    }
    for (QSharedPointer<PortMap> map : portMaps2)
    {
        logicalNames.append(map->getLogicalPort()->name_);
    }
    logicalNames.removeDuplicates();
    
    return logicalNames;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::addMap()
//-----------------------------------------------------------------------------
void ConnectionEditor::addMap(QSharedPointer<PortMap> portMap1,
    QSharedPointer<Component> component1, QSharedPointer<PortMap> portMap2, QSharedPointer<Component> component2, 
    QSharedPointer<ExpressionParser> expressionParser, bool validAbstraction)
{
    QPair<int, int> firstMappedBounds = calculateMappedPhysicalPortBounds(expressionParser, portMap1, component1);
    int left1 = qMax(firstMappedBounds.first, firstMappedBounds.second);
    int right1 = qMin(firstMappedBounds.first, firstMappedBounds.second);

    QPair<int, int> secondMappedBounds = calculateMappedPhysicalPortBounds(expressionParser, portMap2, component2);
    int left2 = qMax(secondMappedBounds.first, secondMappedBounds.second);
    int right2 = qMin(secondMappedBounds.first, secondMappedBounds.second);
    
    if (portMap1->getLogicalPort()->range_ && portMap2->getLogicalPort()->range_)
    {
        QPair<int, int> firstLogicalBounds = calculateMappedLogicalPortBounds(expressionParser, portMap1);
        int firstLogicalHigh = firstLogicalBounds.first;
        int firstLogicalLow = firstLogicalBounds.second;

        QPair<int, int> secondLogicalBounds = calculateMappedLogicalPortBounds(expressionParser, portMap2);
        int secondLogicalHigh = secondLogicalBounds.first;
        int secondLogicalLow = secondLogicalBounds.second;

        if (firstLogicalLow > secondLogicalHigh || firstLogicalHigh < secondLogicalLow)
        {
            // If the maps have 0 common logical bits, do not create a row.
            return;
        }

        // Find the matching ranges for the physical ports based on the logical ranges.
        int logicalHigh = qMin(firstLogicalHigh, secondLogicalHigh);
        int logicalLow = qMax(firstLogicalLow, secondLogicalLow);

        int firstDownSize = abs(firstLogicalHigh - logicalHigh);
        int firstUpSize = abs(logicalLow - firstLogicalLow);

        left1 = (left1 - firstDownSize);
        right1 = (right1 + firstUpSize);

        int secondDownSize = abs(secondLogicalHigh - logicalHigh);
        int secondUpSize = abs(logicalLow - secondLogicalLow);

        left2 = (left2 - secondDownSize);
        right2 = (right2 + secondUpSize);
    }

    int size1 = abs(left1 - right1) + 1;
    int size2 = abs(left2 - right2) + 1;

    bool hasMatchingSizes = (size1 == size2);
    bool phys1IsValid = validAbstraction && hasMatchingSizes && isValidPhysicalPort(portMap1, component1);
    bool phys2IsValid = validAbstraction && hasMatchingSizes && isValidPhysicalPort(portMap2, component2);

    QTableWidgetItem* port1Item = createPortItem(portMap1->getPhysicalPort()->name_, left1, right1, phys1IsValid);
    QTableWidgetItem* port2Item = createPortItem(portMap2->getPhysicalPort()->name_, left2, right2, phys2IsValid);

    int row = portWidget_.rowCount();
    portWidget_.insertRow(row);
    portWidget_.setItem(row, 0, port1Item);
    portWidget_.setItem(row, 1, port2Item);   
}

//-----------------------------------------------------------------------------
// Function: ConnectionEditor::createPortItem()
//-----------------------------------------------------------------------------
QTableWidgetItem* ConnectionEditor::createPortItem(QString const& portName, int left, int right, bool isValid)
{
    QString itemText = portName + QStringLiteral("[%1..%2]").arg(left).arg(right);

    QTableWidgetItem* portItem = new QTableWidgetItem(itemText);

    portItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (isValid)
    {
        portItem->setForeground(QBrush(Qt::black));
    }
    else 
    {
        portItem->setForeground(QBrush(Qt::red));
    }

    return portItem;
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
        QSharedPointer<Port> componentPort = containingComponent->getPort(physicalPort->name_);
        physicalLeft =  parser->parseExpression(componentPort->getLeftBound()).toInt();
        physicalRight = parser->parseExpression(componentPort->getRightBound()).toInt();
    }

    QPair<int, int> portBounds(physicalLeft, physicalRight);
    return portBounds;
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::isValidPhysicalPort()
//-----------------------------------------------------------------------------
bool ConnectionEditor::isValidPhysicalPort(QSharedPointer<PortMap> containingPortMap,
    QSharedPointer<Component> containingComponent)
{
    return containingComponent->hasPort(containingPortMap->getPhysicalPort()->name_);
}

//-----------------------------------------------------------------------------
// Function: connectioneditor::calculateMappedLogicalPortBounds()
//-----------------------------------------------------------------------------
QPair<int, int> ConnectionEditor::calculateMappedLogicalPortBounds(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<PortMap> containingPortMap)
{
    int logicalHigh = 0;
    int logicalLow = 0;

    if (containingPortMap->getLogicalPort()->range_)
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
// Function: ConnectionEditor::setupLayout()
//-----------------------------------------------------------------------------
void ConnectionEditor::setupLayout()
{
    auto nameLayout = new QGridLayout(&nameGroup_);
    nameLayout->addWidget(&nameLabel_, 0, 0);
    nameLayout->addWidget(&nameEdit_, 0, 1);
    nameLayout->addWidget(&descriptionLabel_, 1, 0);
    nameLayout->addWidget(&descriptionEdit_, 1, 1);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(&instanceLabel_, 0, 0, 1, 1);
    layout->addWidget(&connectedInstances_, 0, 1, 1, 1);
    layout->addWidget(&type_, 1, 0, 1, 2);
    layout->addWidget(&absType_, 2, 0, 1, 2);

    layout->addWidget(&nameGroup_, 3, 0, 1, 2);
    layout->addWidget(&portsLabel_, 4, 0, 1, 2);
    layout->addWidget(connectionTypeTable_, 5, 0, 1, 2);

    layout->setColumnStretch(1, 1);
    layout->setRowStretch(5, 1);
}
