/* 
 *  	Created on: 10.10.2011
 *      Author: Antti Kamppi
 * 		filename: interfaceeditor.cpp
 *		Project: Kactus 2
 */

#include "interfaceeditor.h"

#include <models/businterface.h>
#include <models/generaldeclarations.h>

#include <common/GenericEditProvider.h>

#include <designwidget/DiagramConnectionEndpoint.h>
#include <designwidget/diagramport.h>
#include <designwidget/diagraminterface.h>
#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/blockdiagram.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>

InterfaceEditor::InterfaceEditor(QWidget *parent):
QWidget(parent),
busType_(this),
absType_(this),
nameEdit_(this),
nameLabel_(tr("Interface name"), this),
modeEdit_(this),
modeLabel_(tr("Interface mode"), this),
interface_(NULL),
mappings_(this) {

	Q_ASSERT(parent);

	busType_.setTitle(tr("Bus type"));
	busType_.setFlat(false);
	absType_.setTitle(tr("Abstraction type"));
	absType_.setFlat(false);

	// set the possible modes to the mode editor
	QStringList modes;
	modes.append("master");
	modes.append("slave");
	modes.append("system");
	modes.append("mirroredMaster");
	modes.append("mirroredSlave");
	modes.append("mirroredSystem");
	modes.append("monitor");
	modes.append("undefined");
	modeEdit_.addItems(modes);

	// There are always 2 columns.
	mappings_.setColumnCount(2);
	mappings_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Logical name")));
	mappings_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Physical name")));
	mappings_.horizontalHeader()->setStretchLastSection(true);
	mappings_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	mappings_.verticalHeader()->hide();
	mappings_.setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&busType_);
	layout->addWidget(&absType_);
	layout->addWidget(&nameLabel_);
	layout->addWidget(&nameEdit_);
	layout->addWidget(&modeLabel_);
	layout->addWidget(&modeEdit_);
	layout->addWidget(&mappings_, 1);

	clear();
}

InterfaceEditor::~InterfaceEditor() {
}

void InterfaceEditor::setInterface( DiagramConnectionEndPoint* interface ) {
	Q_ASSERT(interface);

	// disconnect the previous interface
	if (interface_) {
		disconnect(interface_, SIGNAL(destroyed(DiagramConnectionEndPoint*)),
			this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
	}

	interface_ = interface;

	busType_.setVLNV(interface->getBusInterface()->getBusType());
	absType_.setVLNV(interface->getBusInterface()->getAbstractionType());

	// set text for the name editor, signal must be disconnected when name is set 
	// to avoid loops 
	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onInterfaceNameChanged(const QString&)));
	nameEdit_.setText(interface->name());
	connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onInterfaceNameChanged(const QString&)), Qt::UniqueConnection);

	// set selection for mode editor, signal must be disconnected when mode is set 
	// to avoid loops 
	disconnect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onInterfaceModeChanged(const QString&)));
	// select the correct mode for mode editor
	int index = modeEdit_.findText(General::interfaceMode2Str(
		interface->getBusInterface()->getInterfaceMode()));
	modeEdit_.setCurrentIndex(index);
	connect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onInterfaceModeChanged(const QString&)), Qt::UniqueConnection);

	// set port maps to be displayed in the table widget.
	setPortMaps();

	connect(interface, SIGNAL(destroyed(DiagramConnectionEndPoint*)),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);

	if (interface->isHierarchical()) {
		nameEdit_.setEnabled(true);
		modeEdit_.setEnabled(true);
		mappings_.setEnabled(true);
	}
	else {
		nameEdit_.setDisabled(true);
		modeEdit_.setDisabled(true);
		mappings_.setDisabled(true);
	}

	// show the editors
	busType_.show();
	absType_.show();
	nameEdit_.show();
	nameLabel_.show();
	modeEdit_.show();
	modeLabel_.show();
	mappings_.show();
}

void InterfaceEditor::clear() {
	
	if (interface_) {
		disconnect(interface_, SIGNAL(destroyed(DiagramConnectionEndPoint*)),
			this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
		interface_ = 0;
	}

	busType_.setVLNV(VLNV());
	busType_.hide();

	absType_.setVLNV(VLNV());
	absType_.hide();

	nameEdit_.clear();
	nameEdit_.hide();
	nameLabel_.hide();

	modeEdit_.hide();
	modeLabel_.hide();

	mappings_.clearContents();
	mappings_.hide();
}

void InterfaceEditor::onInterfaceModeChanged( const QString& newMode ) {
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));
	
	//interface_->setInterfaceMode(General::str2Interfacemode(newMode, General::MONITOR));

	QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(
		interface_, nameEdit_.text(), General::str2Interfacemode(newMode, General::MONITOR)));
	static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onInterfaceNameChanged( const QString& newName ) {
	Q_ASSERT(interface_);
	
	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));
	//interface_->setName(newName);	

	QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(
		interface_, newName, 
		General::str2Interfacemode(modeEdit_.currentText(), General::MONITOR)));
	static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
	
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onPortMapChanged() {
	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	// save the bus interface
	QSharedPointer<BusInterface> busIf = interface_->getBusInterface();

	Q_ASSERT_X(busIf, "PortmapInterfaceTab::onSave()",
		"Null BusInterface pointer");

	QList<QSharedPointer<General::PortMap> > portMaps;

	for (int i = 0; i < mappings_.rowCount(); i++) {
		QString logical = mappings_.item(i, 0)->text();
		QString physical = mappings_.item(i, 1)->text();

		QSharedPointer<General::PortMap> portMap(new General::PortMap);
		portMap->logicalPort_ = logical;
		portMap->physicalPort_ = physical;

		portMaps.append(portMap);
	}

	QSharedPointer<QUndoCommand> cmd(new EndPointPortMapCommand(interface_, portMaps));
	static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::setPortMaps() {

	Q_ASSERT(interface_);

	// signal must be disconnected when changing items to avoid loops
	disconnect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(onPortMapChanged()));

	QSharedPointer<BusInterface> busIf = interface_->getBusInterface();
	QList<QSharedPointer<General::PortMap> > portMaps = busIf->getPortMaps();

	// as many rows as there are interface maps and always 2 columns
	mappings_.setRowCount(portMaps.size());

	// stop sorting when adding the ports to avoid sorting after each add
	mappings_.setSortingEnabled(false);
	int row = 0;
	foreach (QSharedPointer<General::PortMap> portMap, portMaps) {

		QTableWidgetItem* logicalItem = new QTableWidgetItem(portMap->logicalPort_);
		logicalItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mappings_.setItem(row, 0, logicalItem);

		QTableWidgetItem* physItem = new QTableWidgetItem(portMap->physicalPort_);
		physItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mappings_.setItem(row, 1, physItem);

		row++;
	}

	// set sorting back after all items are added
	mappings_.setSortingEnabled(true);

	connect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(onPortMapChanged()), Qt::UniqueConnection);
}

void InterfaceEditor::refresh() {
	Q_ASSERT(interface_);
	setInterface(interface_);
}
