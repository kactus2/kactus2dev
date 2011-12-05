/* 
 *  	Created on: 10.10.2011
 *      Author: Antti Kamppi
 * 		filename: interfaceeditor.cpp
 *		Project: Kactus 2
 */

#include "interfaceeditor.h"

#include <models/businterface.h>
#include <models/generaldeclarations.h>
#include <models/abstractiondefinition.h>

#include <common/GenericEditProvider.h>

#include <designwidget/DiagramConnectionEndpoint.h>
#include <designwidget/diagramport.h>
#include <designwidget/diagraminterface.h>
#include <designwidget/DiagramChangeCommands.h>
#include <designwidget/blockdiagram.h>
#include <designwidget/diagramcomponent.h>

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QBrush>
#include <QHeaderView>
#include <QDockWidget>

//! \brief The maximum height for the description editor.
static const int MAX_DESC_HEIGHT = 50;

InterfaceEditor::InterfaceEditor(QWidget *parent, LibraryInterface* handler):
QWidget(parent),
busType_(this),
absType_(this),
nameEdit_(this),
nameLabel_(tr("Interface name"), this),
modeEdit_(this),
modeLabel_(tr("Interface mode"), this),
interface_(NULL),
mappingsLabel_(tr("Port map"), this),
mappings_(this),
descriptionLabel_(tr("Description"), this),
descriptionEdit_(this),
handler_(handler) {

	Q_ASSERT(parent);
	Q_ASSERT(handler);

	busType_.setTitle(tr("Bus type VLNV"));
	busType_.setFlat(false);
	absType_.setTitle(tr("Abstraction type VLNV"));
	absType_.setFlat(false);

	// set validator for interface name editor
	nameEdit_.setValidator(new VhdlNameValidator(&nameEdit_));

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

	// set the maximum height for the description editor
	descriptionEdit_.setMaximumHeight(MAX_DESC_HEIGHT);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&busType_);
	layout->addWidget(&absType_);
	layout->addWidget(&nameLabel_);
	layout->addWidget(&nameEdit_);
	layout->addWidget(&modeLabel_);
	layout->addWidget(&modeEdit_);
	layout->addWidget(&descriptionLabel_);
	layout->addWidget(&descriptionEdit_);
	layout->addWidget(&mappingsLabel_);
	layout->addWidget(&mappings_, 1);

	clear();
}

InterfaceEditor::~InterfaceEditor() {
}

void InterfaceEditor::setInterface( DiagramConnectionEndPoint* interface ) {
	Q_ASSERT(interface);

	qobject_cast<QDockWidget*>(parentWidget())->show();

	// disconnect the previous interface
	if (interface_) {
		disconnect(interface_, SIGNAL(destroyed(DiagramConnectionEndPoint*)),
			this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
	}

	interface_ = interface;

    Q_ASSERT(interface->getBusInterface());
	busType_.setVLNV(interface->getBusInterface()->getBusType(), true);
    absType_.setVLNV(interface->getBusInterface()->getAbstractionType(), true);

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

	// display the current description of the interface.
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	descriptionEdit_.setPlainText(interface->description());
	connect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	// set port maps to be displayed in the table widget.
	setPortMaps();

	connect(interface, SIGNAL(destroyed(DiagramConnectionEndPoint*)),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);

    bool locked = static_cast<BlockDiagram*>(interface->scene())->isProtected();

	if (!locked && (interface->isHierarchical() ||
                    (interface->encompassingComp() != 0 &&
                     !interface->encompassingComp()->componentModel()->getVlnv()->isValid()))) {
		nameEdit_.setEnabled(true);
		modeEdit_.setEnabled(true);
		mappings_.setEnabled(true);
		descriptionEdit_.setEnabled(true);
	}
	else {
		nameEdit_.setDisabled(true);
		modeEdit_.setDisabled(true);
		mappings_.setDisabled(true);
		descriptionEdit_.setDisabled(true);
	}

	// show the editors
	busType_.show();
	absType_.show();
	nameEdit_.show();
	nameLabel_.show();
	modeEdit_.show();
	modeLabel_.show();
	descriptionLabel_.show();
	descriptionEdit_.show();
	mappingsLabel_.show();
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

	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onInterfaceNameChanged(const QString&)));
	disconnect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onInterfaceModeChanged(const QString&)));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	disconnect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(onPortMapChanged()));

	busType_.setVLNV(VLNV(), true);
	busType_.hide();

	absType_.setVLNV(VLNV(), true);
	absType_.hide();

	nameEdit_.clear();
	nameEdit_.hide();
	nameLabel_.hide();

	modeEdit_.hide();
	modeLabel_.hide();

	descriptionLabel_.hide();
	descriptionEdit_.clear();
	descriptionEdit_.hide();

	mappings_.clearContents();
	mappings_.hide();
	mappingsLabel_.hide();

	qobject_cast<QDockWidget*>(parentWidget())->hide();
}

void InterfaceEditor::onInterfaceModeChanged( const QString& newMode ) {
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(
		interface_, nameEdit_.text(), General::str2Interfacemode(newMode, General::MONITOR),
		descriptionEdit_.toPlainText()));
	static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onInterfaceNameChanged( const QString& newName ) {
	Q_ASSERT(interface_);
	
	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(
		interface_, newName, 
		General::str2Interfacemode(modeEdit_.currentText(), General::MONITOR),
		descriptionEdit_.toPlainText()));
	static_cast<BlockDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
	
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onDescriptionChanged() {
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndPointChangeCommand(
		interface_, nameEdit_.text(), 
		General::str2Interfacemode(modeEdit_.currentText(), General::MONITOR),
		descriptionEdit_.toPlainText()));
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
	Q_ASSERT(busIf);
	QList<QSharedPointer<General::PortMap> > portMaps = busIf->getPortMaps();

	// get the abstraction def for the interface
	VLNV absDefVLNV = busIf->getAbstractionType();
	QSharedPointer<AbstractionDefinition> absDef;
	if (handler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION) {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(absDefVLNV);
		absDef = libComp.staticCast<AbstractionDefinition>();
	}

	// get the component that contains the selected interface
	QSharedPointer<Component> component = interface_->ownerComponent();
	Q_ASSERT(component);

	// get the interface mode of the bus interface
	General::InterfaceMode interfaceMode = busIf->getInterfaceMode();

	// as many rows as there are interface maps and always 2 columns
	mappings_.setRowCount(portMaps.size());

	// stop sorting when adding the ports to avoid sorting after each add
	mappings_.setSortingEnabled(false);
	int row = 0;
	foreach (QSharedPointer<General::PortMap> portMap, portMaps) {

		QString logicalPort = General::toLogicalString(*portMap);
		QTableWidgetItem* logicalItem = new QTableWidgetItem(logicalPort);
		logicalItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		// if no abs type is specified
		if (!absDef) {
			logicalItem->setForeground(QBrush(Qt::red));
		}
		// if the logical port does not belong to the abs def
		else if (!absDef->hasPort(portMap->logicalPort_, interfaceMode)) {
			logicalItem->setForeground(QBrush(Qt::red));
		}
		else {
			logicalItem->setForeground(QBrush(Qt::black));
		}

		// get size of the logical port
		int logicalSize = 1;
		if (portMap->logicalVector_) {
			logicalSize = portMap->logicalVector_->getSize();
		}
		
		// display at least the name of physical port
		QString physicalPort = portMap->physicalPort_;
		// if port map contains vectored physical port.
		if (portMap->physicalVector_) {
			physicalPort = General::toPhysString(*portMap);
		}
		// if port map does not contain physical vector but port is found on the component
		else if (component->hasPort(physicalPort)) {
			physicalPort = General::port2String(physicalPort, 
				component->getPortLeftBound(physicalPort),
				component->getPortRightBound(physicalPort));
		}
		QTableWidgetItem* physItem = new QTableWidgetItem(physicalPort);
		physItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		// if the port is not contained in the component
		if (!component->hasPort(portMap->physicalPort_)) {
			physItem->setForeground(QBrush(Qt::red));
		}
		else {
			physItem->setForeground(QBrush(Qt::black));
		}

		// get size of the physical port
		int physicalSize = 1;
		if (portMap->physicalVector_) {
			physicalSize = portMap->physicalVector_->getSize();
		}

		// if the sizes of the ports don't match
		if (logicalSize != physicalSize) {
			logicalItem->setForeground(QBrush(Qt::red));
			physItem->setForeground(QBrush(Qt::red));
		}
		
		mappings_.setItem(row, 0, logicalItem);
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
