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
#include <models/ApiInterface.h>
#include <models/ComInterface.h>
#include <models/ComDefinition.h>
#include <models/ComProperty.h>

#include <common/GenericEditProvider.h>

#include <designwidget/HWConnectionEndpoint.h>
#include <designwidget/BusPortItem.h>
#include <designwidget/BusInterfaceItem.h>
#include <designwidget/HWChangeCommands.h>
#include <designwidget/HWDesignDiagram.h>
#include <designwidget/HWComponentItem.h>

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QBrush>
#include <QHeaderView>
#include <QDockWidget>

//! \brief The maximum height for the description editor.
static const int MAX_DESC_HEIGHT = 50;

InterfaceEditor::InterfaceEditor(QWidget *parent, LibraryInterface* handler)
    : QWidget(parent),
      type_(this),
      absType_(this),
      nameEdit_(this),
      nameLabel_(tr("Interface name"), this),
      modeEdit_(this),
      modeLabel_(tr("Interface mode"), this),
      dependencyDirLabel_(tr("Dependency direction"), this),
      dependencyDirCombo_(this),
      transferTypeLabel_(tr("Transfer type"), this),
      transferTypeCombo_(this),
      comDirectionLabel_(tr("Direction"), this),
      comDirectionCombo_(this),
      interface_(NULL),
      mappingsLabel_(tr("Port map"), this),
      mappings_(this),
      descriptionLabel_(tr("Description"), this),
      descriptionEdit_(this),
      propertyValueEditor_(this),
      propertyValueLabel_(tr("Property values"), this),
      dummyWidget_(this),
      handler_(handler)
{
	Q_ASSERT(parent);
	Q_ASSERT(handler);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	type_.setTitle(tr("Bus type VLNV"));
	type_.setFlat(false);
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

    // Set possible modes to the direction editors.
    dependencyDirCombo_.addItem("requester");
    dependencyDirCombo_.addItem("provider");

    comDirectionCombo_.addItem("in");
    comDirectionCombo_.addItem("out");
    comDirectionCombo_.addItem("inout");

	// There are always 2 columns.
	mappings_.setColumnCount(2);
	mappings_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Logical name")));
	mappings_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Physical name")));
	mappings_.horizontalHeader()->setStretchLastSection(true);
	mappings_.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	mappings_.verticalHeader()->hide();
	mappings_.setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed);

	// set the maximum height for the description editor
	descriptionEdit_.setMaximumHeight(MAX_DESC_HEIGHT);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&type_);
	layout->addWidget(&absType_);
	layout->addWidget(&nameLabel_);
	layout->addWidget(&nameEdit_);
	layout->addWidget(&modeLabel_);
	layout->addWidget(&modeEdit_);
    layout->addWidget(&dependencyDirLabel_);
    layout->addWidget(&dependencyDirCombo_);
    layout->addWidget(&transferTypeLabel_);
    layout->addWidget(&transferTypeCombo_);
    layout->addWidget(&comDirectionLabel_);
    layout->addWidget(&comDirectionCombo_);
	layout->addWidget(&descriptionLabel_);
	layout->addWidget(&descriptionEdit_);
	layout->addWidget(&mappingsLabel_);
    layout->addWidget(&mappings_, 1);
    layout->addWidget(&propertyValueLabel_);
    layout->addWidget(&propertyValueEditor_, 1);
    layout->addWidget(&dummyWidget_, 1);

	clear();
}

InterfaceEditor::~InterfaceEditor() {
}

void InterfaceEditor::setInterface( ConnectionEndpoint* interface ) {
	Q_ASSERT(interface);

	parentWidget()->raise();

	// disconnect the previous interface
	if (interface_) {
		disconnect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)),
			       this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()),
			       this, SLOT(refresh()));

        comDef_.clear();
	}

	interface_ = interface;

    // set text for the name editor, signal must be disconnected when name is set 
    // to avoid loops 
    disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onInterfaceNameChanged(const QString&)));
    nameEdit_.setText(interface->name());
    connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onInterfaceNameChanged(const QString&)), Qt::UniqueConnection);

    // display the current description of the interface.
    disconnect(&descriptionEdit_, SIGNAL(textChanged()),
        this, SLOT(onDescriptionChanged()));
    descriptionEdit_.setPlainText(interface->description());
    connect(&descriptionEdit_, SIGNAL(textChanged()),
        this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

    // Fill the rest of the editors based on the interface type.
    if (interface->isBus())
    {
        Q_ASSERT(interface->getBusInterface());
        type_.setTitle(tr("Bus type VLNV"));
	    type_.setVLNV(interface->getBusInterface()->getBusType(), true);
        absType_.setVLNV(interface->getBusInterface()->getAbstractionType(), true);

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
    }
    else if (interface->isApi())
    {
        Q_ASSERT(interface->getApiInterface());
        type_.setTitle(tr("API type VLNV"));
        type_.setVLNV(interface->getApiInterface()->getApiType(), true);

        // Set selection for dependency direction.
        disconnect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onDependencyDirectionChanged(QString const&)));

        int index = dependencyDirCombo_.findText(dependencyDirection2Str(
            interface->getApiInterface()->getDependencyDirection()));
        dependencyDirCombo_.setCurrentIndex(index);

        connect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(onDependencyDirectionChanged(QString const&)), Qt::UniqueConnection);
    }
    else if (interface->isCom())
    {
        Q_ASSERT(interface->getComInterface());
        type_.setTitle(tr("COM type VLNV"));
        type_.setVLNV(interface->getComInterface()->getComType(), true);

        // Retrieve the COM definition.
        if (interface->getComInterface()->getComType().isValid())
        {
            QSharedPointer<LibraryComponent const> libComp =
                handler_->getModelReadOnly(interface->getComInterface()->getComType());
            comDef_ = libComp.dynamicCast<ComDefinition const>();
        }

        // Fill in the possible values of the data type.
        disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onComTransferTypeChanged(QString const&)));

        transferTypeCombo_.clear();
        transferTypeCombo_.addItem("");

        if (comDef_ != 0)
        {
            transferTypeCombo_.addItems(comDef_->getTransferTypes());

            // Set selection for the data type.
            QString const& transferType = interface->getComInterface()->getTransferType();

            if (comDef_->getTransferTypes().contains(transferType))
            {
                transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(transferType));
            }
        }

        connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(onComTransferTypeChanged(QString const&)), Qt::UniqueConnection);

        // Set selection for COM direction.
        disconnect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onComDirectionChanged(QString const&)));

        int index = comDirectionCombo_.findText(General::direction2Str(
            interface->getComInterface()->getDirection()));
        comDirectionCombo_.setCurrentIndex(index);

        connect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(onComDirectionChanged(QString const&)), Qt::UniqueConnection);

        // Set property values and try to read the allowed properties from the COM definition if valid.
        disconnect(&propertyValueEditor_, SIGNAL(contentChanged()),
                   this, SLOT(onComPropertyValuesChanged()));

        propertyValueEditor_.setData(interface->getComInterface()->getPropertyValues());

        if (comDef_ != 0)
        {
            propertyValueEditor_.setAllowedProperties(&comDef_->getProperties());
        }

        connect(&propertyValueEditor_, SIGNAL(contentChanged()),
                this, SLOT(onComPropertyValuesChanged()), Qt::UniqueConnection);
    }
    else
    {
        type_.setTitle(tr("Type VLNV"));
    }

	connect(interface, SIGNAL(destroyed(ConnectionEndpoint*)),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);

    bool locked = static_cast<DesignDiagram*>(interface->scene())->isProtected();

    // Allow editing of values only in unlocked mode when the interface is a hierarchical one
    // or unpackaged.
	if (!locked && (interface->isHierarchical() ||
                    (interface->encompassingComp() != 0 &&
                     !interface->encompassingComp()->componentModel()->getVlnv()->isValid())))
    {
		nameEdit_.setEnabled(true);
		modeEdit_.setEnabled(true);
        dependencyDirCombo_.setEnabled(true);
        comDirectionCombo_.setEnabled(true);
		mappings_.setEnabled(true);
		mappings_.setEditTriggers(QAbstractItemView::NoEditTriggers);
		descriptionEdit_.setEnabled(true);
        propertyValueEditor_.setEnabled(true);
        transferTypeCombo_.setEnabled(true);
	}
	else {
		nameEdit_.setDisabled(true);
		modeEdit_.setDisabled(true);
        dependencyDirCombo_.setDisabled(true);
        comDirectionCombo_.setDisabled(true);
		descriptionEdit_.setDisabled(true);
        propertyValueEditor_.setDisabled(true);
        transferTypeCombo_.setDisabled(true);

		mappings_.setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	// show the editors
    type_.show();
	nameEdit_.show();
	nameLabel_.show();
	descriptionLabel_.show();
	descriptionEdit_.show();

    absType_.setVisible(interface->isBus());
    modeEdit_.setVisible(interface->isBus());
    modeLabel_.setVisible(interface->isBus());
    mappingsLabel_.setVisible(interface->isBus());
    mappings_.setVisible(interface->isBus());

    dependencyDirCombo_.setVisible(interface->isApi());
    dependencyDirLabel_.setVisible(interface->isApi());
    dummyWidget_.setVisible(!interface->isCom() && !interface->isBus());

    transferTypeCombo_.setVisible(interface->isCom());
    transferTypeLabel_.setVisible(interface->isCom());
    comDirectionCombo_.setVisible(interface->isCom());
    comDirectionLabel_.setVisible(interface->isCom());
    propertyValueLabel_.setVisible(interface->isCom());
    propertyValueEditor_.setVisible(interface->isCom());

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

void InterfaceEditor::clear() {
	
	if (interface_) {
		disconnect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)),
			this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
		interface_ = 0;
        comDef_.clear();
	}

	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onInterfaceNameChanged(const QString&)));
	disconnect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onInterfaceModeChanged(const QString&)));
    disconnect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onDependencyDirectionChanged(QString const&)));
    disconnect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onComDirectionChanged(QString const&)));
    disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onComTransferTypeChanged(QString const&)));
    disconnect(&propertyValueEditor_, SIGNAL(contentChanged()),
        this, SLOT(onComPropertyValuesChanged()));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	disconnect(&mappings_, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(onPortMapChanged()));

	type_.setVLNV(VLNV(), true);
	type_.hide();

	absType_.setVLNV(VLNV(), true);
	absType_.hide();

	nameEdit_.clear();
	nameEdit_.hide();
	nameLabel_.hide();

	modeEdit_.hide();
	modeLabel_.hide();

    dependencyDirCombo_.hide();
    dependencyDirLabel_.hide();

    comDirectionCombo_.hide();
    comDirectionLabel_.hide();

	descriptionLabel_.hide();
	descriptionEdit_.clear();
	descriptionEdit_.hide();

    propertyValueLabel_.hide();
    propertyValueEditor_.hide();
    dummyWidget_.hide();

    transferTypeCombo_.clear();
    transferTypeCombo_.hide();
    transferTypeLabel_.hide();

	mappings_.clearContents();
	mappings_.hide();
	mappingsLabel_.hide();

	parentWidget()->setMaximumHeight(20);
}

void InterfaceEditor::onInterfaceModeChanged( const QString& newMode ) {
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointChangeCommand(
		static_cast<HWConnectionEndpoint*>(interface_), nameEdit_.text(),
        General::str2Interfacemode(newMode, General::MONITOR),
		descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onInterfaceNameChanged( const QString& newName ) {
	Q_ASSERT(interface_);
	
	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new EndpointNameChangeCommand(interface_, newName));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
	
	connect(interface_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void InterfaceEditor::onDescriptionChanged() {
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointDescChangeCommand(interface_, descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

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

	QSharedPointer<QUndoCommand> cmd(new EndPointPortMapCommand(static_cast<HWConnectionEndpoint*>(interface_), portMaps));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);
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
	QSharedPointer<Component> component = interface_->getOwnerComponent();
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

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onDependencyDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onDependencyDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()),
               this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointDependencyDirectionChangeCommand(
        interface_, str2DependencyDirection(newDir, DEPENDENCY_PROVIDER)));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

    connect(interface_, SIGNAL(contentChanged()), 
        this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()),
               this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointComDirectionChangeCommand(
        interface_, General::str2Direction(newDir, General::IN)));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

    connect(interface_, SIGNAL(contentChanged()), 
            this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComTransferTypeChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComTransferTypeChanged(QString const& newTransferType)
{
    disconnect(interface_, SIGNAL(contentChanged()),
               this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointTransferTypeChangeCommand(interface_, newTransferType));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

    connect(interface_, SIGNAL(contentChanged()), 
            this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComPropertyValuesChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComPropertyValuesChanged()
{
    disconnect(interface_, SIGNAL(contentChanged()),
               this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointPropertyValuesChangeCommand(interface_,
                                                                             propertyValueEditor_.getData()));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider().addCommand(cmd);

    connect(interface_, SIGNAL(contentChanged()), 
            this, SLOT(refresh()), Qt::UniqueConnection);
}
