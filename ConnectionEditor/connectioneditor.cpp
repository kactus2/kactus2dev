/* 
 *  	Created on: 14.10.2011
 *      Author: Antti Kamppi
 * 		filename: connectioneditor.cpp
 *		Project: Kactus 2
 */

#include "connectioneditor.h"

#include <designwidget/diagraminterconnection.h>
#include <designwidget/DiagramConnectionEndpoint.h>
#include <designwidget/blockdiagram.h>
#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/diagramcomponent.h>

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>

#include <models/businterface.h>
#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <models/generaldeclarations.h>

#include <common/GenericEditProvider.h>
#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QHeaderView>
#include <QList>
#include <QStringList>
#include <QBrush>
#include <QDockWidget>

#include <QDebug>

//! \brief The maximum height for the description editor.
static const int MAX_DESC_HEIGHT = 50;

ConnectionEditor::ConnectionEditor(QWidget *parent, LibraryInterface* handler):
QWidget(parent),
busType_(this),
absType_(this),
instanceLabel_(tr("Connected interfaces"), this),
connectedInstances_(this),
separator_(this),
nameLabel_(tr("Connection name"), this),
nameEdit_(this),
descriptionLabel_(tr("Description"), this),
descriptionEdit_(this),
portsLabel_(tr("Connected physical ports"), this),
portWidget_(this),
connection_(NULL),
handler_(handler) {

	Q_ASSERT(parent);
	Q_ASSERT(handler);

	busType_.setTitle(tr("Bus type"));
	busType_.setFlat(false);
	absType_.setTitle(tr("Abstraction type"));
	absType_.setFlat(false);

	separator_.setFlat(true);

	// set validator for name edit
	nameEdit_.setValidator(new VhdlNameValidator(&nameEdit_));

	// There are always 2 columns.
	portWidget_.setColumnCount(2);
	portWidget_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("instance1")));
	portWidget_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("instance2")));
	portWidget_.horizontalHeader()->setStretchLastSection(true);
	portWidget_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	portWidget_.verticalHeader()->hide();
	portWidget_.setEditTriggers(QAbstractItemView::NoEditTriggers);

	// set the maximum height for the description editor
	descriptionEdit_.setMaximumHeight(MAX_DESC_HEIGHT);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&busType_);
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

	clear();
}

ConnectionEditor::~ConnectionEditor() {
}

void ConnectionEditor::clear() {
	if (connection_) {
		disconnect(connection_, SIGNAL(destroyed(DiagramInterconnection*)),
			this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
		connection_ = 0;
	}

	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));

	// clear the contents of the editors
	busType_.setVLNV(VLNV());
	absType_.setVLNV(VLNV());
	connectedInstances_.clear();
	nameEdit_.clear();
	descriptionEdit_.clear();
	portWidget_.clearContents();

	// set objects as hidden
	busType_.hide();
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

	qobject_cast<QDockWidget*>(parentWidget())->hide();
}

void ConnectionEditor::refresh() {
	Q_ASSERT(connection_);
	setConnection(connection_);
}

void ConnectionEditor::setConnection( DiagramInterconnection* connection ) {
	Q_ASSERT(connection);

	qobject_cast<QDockWidget*>(parentWidget())->show();

	// disconnect the previous connection
	if (connection_) {
		disconnect(connection_, SIGNAL(destroyed(DiagramInterconnection*)),
			this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
	}

	connection_ = connection;

	DiagramConnectionEndPoint* endPoint1 = connection->endPoint1();
	Q_ASSERT(endPoint1);

	busType_.setVLNV(endPoint1->getBusInterface()->getBusType());
	absType_.setVLNV(endPoint1->getBusInterface()->getAbstractionType());

	QString endPoint1Name = endPoint1->name();
	QString endPoint2Name = connection->endPoint2()->name();

	// set the names of the connected instances
	connectedInstances_.setText(QString("%1 - %2").arg(endPoint1Name).arg(endPoint2Name));

	// set text for the name editor, signal must be disconnected when name is set 
	// to avoid loops 
	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)));
	nameEdit_.setText(connection->name());
	connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);

	// display the current description of the interface.
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	descriptionEdit_.setPlainText(connection->description());
	connect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	connect(connection, SIGNAL(destroyed(DiagramInterconnection*)),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(connection, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);

	setPortMaps();
	
	// if either end point is hierarchical then there is no description to set
	if (endPoint1->isHierarchical() || connection->endPoint2()->isHierarchical()) {
		
		// description exists only for normal interconnections
		descriptionEdit_.setDisabled(true);
		descriptionLabel_.hide();
		descriptionEdit_.hide();

		// name exists for only normal interconnections
		nameLabel_.hide();
		nameEdit_.hide();
		nameEdit_.setDisabled(true);
	}
	else {
		descriptionEdit_.setEnabled(true);
		descriptionLabel_.show();
		descriptionEdit_.show();

		nameLabel_.show();
		nameEdit_.show();
		nameEdit_.setEnabled(true);
	}

	// set the objects visible
	busType_.show();
	absType_.show();
	instanceLabel_.show();
	connectedInstances_.show();
	separator_.show();
	portsLabel_.show();
	portWidget_.show();
}

void ConnectionEditor::onNameChanged( const QString& name ) {
	Q_ASSERT(connection_);

	disconnect(connection_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new ConnectionChangeCommand(
		connection_, name, descriptionEdit_.toPlainText()));
	static_cast<BlockDiagram*>(connection_->scene())->getEditProvider().addCommand(cmd);

	connect(connection_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void ConnectionEditor::onDescriptionChanged() {

	Q_ASSERT(connection_);

	disconnect(connection_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new ConnectionChangeCommand(
		connection_, nameEdit_.text(), descriptionEdit_.toPlainText()));
	static_cast<BlockDiagram*>(connection_->scene())->getEditProvider().addCommand(cmd);

	connect(connection_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void ConnectionEditor::setPortMaps() {
	Q_ASSERT(connection_);
	
	portWidget_.clearContents();

	// get the interface and component for end point 1
	QSharedPointer<BusInterface> busIf1 = connection_->endPoint1()->getBusInterface();
	Q_ASSERT(busIf1);
	QList<QSharedPointer<General::PortMap> > portMaps1 = busIf1->getPortMaps();
	QSharedPointer<Component> comp1 = connection_->endPoint1()->ownerComponent();
	Q_ASSERT(comp1);

	// get the interface and component for end point 2
	QSharedPointer<BusInterface> busIf2 = connection_->endPoint2()->getBusInterface();
	Q_ASSERT(busIf2);
	QList<QSharedPointer<General::PortMap> > portMaps2 = busIf2->getPortMaps();
	QSharedPointer<Component> comp2 = connection_->endPoint2()->ownerComponent();
	Q_ASSERT(comp2);

	// set the header for end point 1
	DiagramComponent* diacomp1 = connection_->endPoint1()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp1) {
		portWidget_.horizontalHeaderItem(0)->setText(diacomp1->name());
	}
	// if was the interface of a top component
	else {
		portWidget_.horizontalHeaderItem(0)->setText(comp1->getVlnv()->getName());
	}

	// set the header for end point 2
	DiagramComponent* diacomp2 = connection_->endPoint2()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp2) {
		portWidget_.horizontalHeaderItem(1)->setText(diacomp2->name());
	}
	// if was the interface of a top component
	else {
		portWidget_.horizontalHeaderItem(1)->setText(comp2->getVlnv()->getName());
	}

	// get the abstraction def for the interfaces
	VLNV absDefVLNV = busIf1->getAbstractionType();
	QSharedPointer<AbstractionDefinition> absDef;
	if (handler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION) {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(absDefVLNV);
		absDef = libComp.staticCast<AbstractionDefinition>();
	}

	General::InterfaceMode interfaceMode1 = busIf1->getInterfaceMode();
	General::InterfaceMode interfaceMode2 = busIf2->getInterfaceMode();

	// turn off sorting when adding items
	portWidget_.setSortingEnabled(false);

	// set the size to be the max value 
	portWidget_.setRowCount(0);

	// get list of all used logical ports
	QStringList logicalNames;
	foreach (QSharedPointer<General::PortMap> map, portMaps1) {
		if (!logicalNames.contains(map->logicalPort_)) {
			logicalNames.append(map->logicalPort_);
		}
	}
	foreach (QSharedPointer<General::PortMap> map, portMaps2) {
		if (!logicalNames.contains(map->logicalPort_)) {
			logicalNames.append(map->logicalPort_);
		}
	}

	int row = 0;
	// find the physical ports mapped to given logical port
	foreach (QString logicalPort, logicalNames) {

		bool invalid = false;

		// check that the logical signal is contained in both interface modes used
		if (absDef) {
			if (!absDef->hasPort(logicalPort, interfaceMode1) ||
				!absDef->hasPort(logicalPort, interfaceMode2)) {
					invalid = true;
			}
		}

		foreach (QSharedPointer<General::PortMap> map1, portMaps1) {
			if (map1->logicalPort_ == logicalPort) {

				foreach (QSharedPointer<General::PortMap> map2, portMaps2) {
					if (map2->logicalPort_ == logicalPort) {
						addMap(row, invalid, map1, comp1, map2, comp2);
					}
				}
			}
		}
	}

	// finally set sorting back on
	portWidget_.setSortingEnabled(true);
}

void ConnectionEditor::addMap( int& row, bool invalid,
							  QSharedPointer<General::PortMap> portMap1,
							  QSharedPointer<Component> component1,
							  QSharedPointer<General::PortMap> portMap2,
							  QSharedPointer<Component> component2) {
	
	int phys1Left = 0;
	int phys1Right = 0;
	bool phys1Invalid = invalid;

	// if port is vectored on the port map
	if (portMap1->physicalVector_) {
		phys1Left = portMap1->physicalVector_->getLeft();
		phys1Right = portMap1->physicalVector_->getRight();

		// if the port is not found on the component
		if (!component1->hasPort(portMap1->physicalPort_)) {
			phys1Invalid = true;
		}
	}
	// if port is found on the component then use the port bounds
	else if (component1->hasPort(portMap1->physicalPort_)) {
		phys1Left = component1->getPortLeftBound(portMap1->physicalPort_);
		phys1Right = component1->getPortRightBound(portMap1->physicalPort_);
	}
	// port was not found on the component
	else {
		phys1Invalid = true;
	}


	int phys2Left = 0;
	int phys2Right = 0;
	bool phys2Invalid = invalid;
	// if port is vectored on the port map
	if (portMap2->physicalVector_) {
		phys2Left = portMap2->physicalVector_->getLeft();
		phys2Right = portMap2->physicalVector_->getRight();

		// if the port is not found on the component
		if (!component2->hasPort(portMap2->physicalPort_)) {
			phys2Invalid = true;
		}
	}
	// if port is found on the component then use the port bounds
	else if (component2->hasPort(portMap2->physicalPort_)) {
		phys2Left = component2->getPortLeftBound(portMap2->physicalPort_);
		phys2Right = component2->getPortRightBound(portMap2->physicalPort_);
	}
	// port was not found on the component
	else {
		phys2Invalid = true;
	}

	// check the sizes of the physical ports
	int size1 = phys1Left - phys1Right + 1;
	int size2 = phys2Left - phys2Right + 1;

	QTableWidgetItem* port1Item;
	QTableWidgetItem* port2Item;

	// if both have vectored logical signals
	if (portMap1->logicalVector_ && portMap2->logicalVector_) {
		
		// if the vectored ports don't have any common bounds
		if (portMap1->logicalVector_->getRight() > portMap2->logicalVector_->getLeft() ||
			portMap1->logicalVector_->getLeft() < portMap2->logicalVector_->getRight()) {
				return;
		}

		int logicalLeft = qMin(portMap1->logicalVector_->getLeft(), 
			portMap2->logicalVector_->getLeft());
		int logicalRight = qMax(portMap1->logicalVector_->getRight(),
			portMap2->logicalVector_->getRight());

		QString port1;
		QString port2;
		
		{
			// count how much the left bound of port 1 has to be adjusted down
			int downSize = abs(portMap1->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 1 has to be adjusted up
			int upSize = abs(logicalRight - portMap1->logicalVector_->getRight());

			// the actual size of the connected parts of the ports
			size1 = (phys1Left - downSize) - (phys1Right + upSize) + 1; 

			port1 = General::port2String(portMap1->physicalPort_,
				phys1Left - downSize, phys1Right + upSize);
		}
		{
			// count how much the left bound of port 2 has to be adjusted down
			int downSize = abs(portMap2->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 2 has to be adjusted up
			int upSize = abs(logicalRight - portMap2->logicalVector_->getRight());

			// the actual size of the connected parts of the ports
			size2 = (phys2Left - downSize) - (phys2Right + upSize) + 1;

			port2 = General::port2String(portMap2->physicalPort_,
				phys2Left - downSize, phys2Right + upSize);
		}

		// if the connected sizes of the ports don't match
		if (size1 != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}

		port1Item = new QTableWidgetItem(port1);

		port2Item = new QTableWidgetItem(port2);
	}
	// if port map1 has vectored logical signal
	else if (portMap1->logicalVector_ && !portMap2->logicalVector_) {

		// port 1 uses the original physical bounds
		QString port1 = General::port2String(portMap1->physicalPort_, phys1Left, phys1Right);
		port1Item = new QTableWidgetItem(port1);

		// port 2 uses the bounds of the logical port of port 1
		QString port2 = General::port2String(portMap2->physicalPort_, 
			portMap1->logicalVector_->getLeft(),
			portMap1->logicalVector_->getRight());
		
		// if the logical port and port 2 sizes don't match
		if (portMap1->logicalVector_->getSize() != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
		port2Item = new QTableWidgetItem(port2);
	}
	// if port map2 has vectored logical signal
	else if (!portMap1->logicalVector_ && portMap2->logicalVector_) {
		
		// port 1 uses the bounds of the logical port of port 2
		QString port1 = General::port2String(portMap1->physicalPort_, 
			portMap2->logicalVector_->getLeft(),
			portMap2->logicalVector_->getRight());

		// if the logical port and port 2 sizes don't match
		if (portMap2->logicalVector_->getSize() != size1) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
		port1Item = new QTableWidgetItem(port1);

		// port 2 uses the original physical bounds
		QString port2 = General::port2String(portMap2->physicalPort_, phys2Left, phys2Right);
		port2Item = new QTableWidgetItem(port2);
	}
	// if neither has vectored logical signal
	else {
		QString port1 = General::port2String(portMap1->physicalPort_, phys1Left, phys1Right);
		port1Item = new QTableWidgetItem(port1);
			
		QString port2 = General::port2String(portMap2->physicalPort_, phys2Left, phys2Right);
		port2Item = new QTableWidgetItem(port2);

		// if sizes don't match then both must be marked as invalid
		if (size1 != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
	}

	// set the flags for the items
	port1Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	port2Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	// set the colors according to validity of items
	if (phys1Invalid) {
		port1Item->setForeground(QBrush(Qt::red));
	}
	else {
		port1Item->setForeground(QBrush(Qt::black));
	}
	if (phys2Invalid) {
		port2Item->setForeground(QBrush(Qt::red));
	}
	else {
		port2Item->setForeground(QBrush(Qt::black));
	}
	
	// add items to the port widget
	portWidget_.insertRow(row);
	portWidget_.setItem(row, 0, port1Item);
	portWidget_.setItem(row, 1, port2Item);
	++row;
}
