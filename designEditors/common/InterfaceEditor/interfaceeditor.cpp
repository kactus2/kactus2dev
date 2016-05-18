//-----------------------------------------------------------------------------
// File: interfaceeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.10.2011
//
// Description:
// Editor to display/edit the details of a bus interface.
//-----------------------------------------------------------------------------

#include "interfaceeditor.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Vector.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/InterfaceEditor/InterfacePortMapModel.h>
#include <designEditors/common/InterfaceEditor/InterfacePortMapDelegate.h>
#include <designEditors/common/InterfaceEditor/InterfacePortMapColumns.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/validators/namevalidator.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/Design.h>

#include <QVBoxLayout>
#include <QStringList>
#include <QBrush>
#include <QHeaderView>
#include <QDockWidget>

//! The maximum height for the description editor.
static const int MAX_DESC_HEIGHT = 50;

//-----------------------------------------------------------------------------
// Function: interfaceeditor::InterfaceEditor()
//-----------------------------------------------------------------------------
InterfaceEditor::InterfaceEditor(QWidget *parent, LibraryInterface* handler):
QWidget(parent),
type_(this),
absType_(this),
nameEdit_(this),
nameLabel_(tr("Interface name"), this),
modeEdit_(this),
modeLabel_(tr("Interface mode"), this),
dependencyDirLabel_(tr("Dependency direction"), this),
dependencyDirCombo_(this),
comDirectionLabel_(tr("Direction"), this),
comDirectionCombo_(this),
transferTypeLabel_(tr("Transfer type"), this),
transferTypeCombo_(this),
interface_(NULL),
comDef_(),
mappingsLabel_(tr("Port map"), this),
portMapsView_(this),
portMapsModel_(new InterfacePortMapModel(handler, this)),
descriptionLabel_(tr("Description"), this),
descriptionEdit_(this),
propertyValueLabel_(tr("Property values"), this),
propertyValueEditor_(this),
dummyWidget_(this),
handler_(handler),
containingDesign_()
{
	Q_ASSERT(parent);
	Q_ASSERT(handler);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	type_.setTitle(tr("Bus type VLNV"));
	type_.setFlat(false);
	absType_.setTitle(tr("Abstraction type VLNV"));
	absType_.setFlat(false);

	// set validator for interface name editor
    nameEdit_.setValidator(new NameValidator(&nameEdit_));

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

    portMapsView_.setSortingEnabled(true);
    portMapsView_.setSelectionMode(QAbstractItemView::SingleSelection);
    portMapsView_.verticalHeader()->hide();

    portMapsView_.setItemDelegate(new InterfacePortMapDelegate(this));

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(portMapsModel_);
    portMapsView_.setModel(proxy);

    portMapsView_.resizeColumnsToContents();

    portMapsView_.setContextMenuPolicy(Qt::NoContextMenu);
    portMapsView_.setAlternatingRowColors(false);
    portMapsView_.horizontalHeader()->setStretchLastSection(true);

	// set the maximum height for the description editor
	descriptionEdit_.setMaximumHeight(MAX_DESC_HEIGHT);

    connect(portMapsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

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
    layout->addWidget(&portMapsView_, 1);
    layout->addWidget(&propertyValueLabel_);
    layout->addWidget(&propertyValueEditor_, 1);
    layout->addWidget(&dummyWidget_, 1);

	clear();
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::~InterfaceEditor()
//-----------------------------------------------------------------------------
InterfaceEditor::~InterfaceEditor()
{

}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::setInterface()
//-----------------------------------------------------------------------------
void InterfaceEditor::setInterface(ConnectionEndpoint* interface, QSharedPointer<Design> containingDesign)
{
	Q_ASSERT(interface);

	parentWidget()->raise();

	// disconnect the previous interface
	if (interface_)
    {
		disconnect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)), this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

        comDef_.clear();
	}

	interface_ = interface;
    containingDesign_ = containingDesign;

    // set text for the name editor, signal must be disconnected when name is set to avoid loops 
    disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)), this, SLOT(onInterfaceNameChanged(const QString&)));
    nameEdit_.setText(interface->name());
    connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onInterfaceNameChanged(const QString&)), Qt::UniqueConnection);

    // display the current description of the interface.
    disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));
    descriptionEdit_.setPlainText(interface->description());
    connect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

    QList<QSharedPointer<ActiveInterface> > activeInterfaces = getActiveInterfaces(interface_, containingDesign_);

    portMapsModel_->setInterfaceData(interface_, activeInterfaces);

    // Fill the rest of the editors based on the interface type.
    if (interface->isBus())
    {
        Q_ASSERT(interface->getBusInterface());
        type_.setTitle(tr("Bus type VLNV"));
	    type_.setVLNV(interface->getBusInterface()->getBusType(), true);
        if (interface->getBusInterface()->getAbstractionTypes() && 
            !interface->getBusInterface()->getAbstractionTypes()->isEmpty() &&
            interface->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef())
        {
            absType_.setVLNV(*interface->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef(),
                true);
        }

	    // set selection for mode editor, signal must be disconnected when mode is set to avoid loops 
	    disconnect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		    this, SLOT(onInterfaceModeChanged(const QString&)));
	    // select the correct mode for mode editor
	    int index = modeEdit_.findText(General::interfaceMode2Str(
            interface->getBusInterface()->getInterfaceMode()));

	    modeEdit_.setCurrentIndex(index);
	    connect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		    this, SLOT(onInterfaceModeChanged(const QString&)), Qt::UniqueConnection);

	    // set port maps to be displayed in the table widget.
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
            QSharedPointer<Document const> libComp =
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
            transferTypeCombo_.addItems(*comDef_->getTransferTypes());

            // Set selection for the data type.
            QString const& transferType = interface->getComInterface()->getTransferType();

            if (comDef_->getTransferTypes()->contains(transferType))
            {
                transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(transferType));
            }
        }

        connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(onComTransferTypeChanged(QString const&)), Qt::UniqueConnection);

        // Set selection for COM direction.
        disconnect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onComDirectionChanged(QString const&)));

        int index = comDirectionCombo_.findText(DirectionTypes::direction2Str(
            interface->getComInterface()->getDirection()));
        comDirectionCombo_.setCurrentIndex(index);

        connect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
                this, SLOT(onComDirectionChanged(QString const&)), Qt::UniqueConnection);

        // Set property values and try to read the allowed properties from the COM definition if valid.
        disconnect(&propertyValueEditor_, SIGNAL(contentChanged()), this, SLOT(onComPropertyValuesChanged()));

        propertyValueEditor_.setData(interface->getComInterface()->getPropertyValues());

        if (comDef_ != 0)
        {
            propertyValueEditor_.setAllowedProperties(*comDef_->getProperties());
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

    portMapsView_.setEnabled(!locked);

    // Allow editing of values only in unlocked mode when the interface is a hierarchical one
    // or unpackaged.
	if (!locked && (interface->isHierarchical() ||
                    (interface->encompassingComp() != 0 &&
                     !interface->encompassingComp()->componentModel()->getVlnv().isValid())))
    {
		nameEdit_.setEnabled(true);
		modeEdit_.setEnabled(true);
        dependencyDirCombo_.setEnabled(true);
        comDirectionCombo_.setEnabled(true);
		descriptionEdit_.setEnabled(true);
        propertyValueEditor_.setEnabled(true);
        transferTypeCombo_.setEnabled(true);
	}
	else
    {
		nameEdit_.setDisabled(true);
		modeEdit_.setDisabled(true);
        dependencyDirCombo_.setDisabled(true);
        comDirectionCombo_.setDisabled(true);
		descriptionEdit_.setDisabled(true);
        propertyValueEditor_.setDisabled(true);
        transferTypeCombo_.setDisabled(true);
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
    portMapsView_.setVisible(interface->isBus());

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

//-----------------------------------------------------------------------------
// Function: interfaceeditor::getActiveInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ActiveInterface> > InterfaceEditor::getActiveInterfaces(ConnectionEndpoint* endPoint,
    QSharedPointer<Design> design)
{
    QList<QSharedPointer<ActiveInterface> > newInterfaces;

    if (endPoint->encompassingComp() != 0)
    {
        foreach (QSharedPointer<Interconnection> connection, *design->getInterconnections())
        {
            QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();

            if (activeInterfaceReferencesBusInterface(startInterface, endPoint))
            {
                newInterfaces.append(startInterface);
            }
            else
            {
                foreach (QSharedPointer<ActiveInterface> currentInterface, *connection->getActiveInterfaces())
                {
                    if (activeInterfaceReferencesBusInterface(currentInterface, endPoint))
                    {
                        newInterfaces.append(currentInterface);
                    }
                }
            }
        }
    }

    return newInterfaces;
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::activeInterfaceReferencesBusInterface()
//-----------------------------------------------------------------------------
bool InterfaceEditor::activeInterfaceReferencesBusInterface(QSharedPointer<ActiveInterface> currentInterface,
    ConnectionEndpoint* endPoint) const
{
    return currentInterface->getComponentReference() == endPoint->encompassingComp()->name() &&
        currentInterface->getBusReference() == endPoint->name();
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::clear()
//-----------------------------------------------------------------------------
void InterfaceEditor::clear()
{
	if (interface_)
    {
		disconnect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)), this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));
		interface_ = 0;
        comDef_.clear();
	}

	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)), this, SLOT(onInterfaceNameChanged(const QString&)));
	disconnect(&modeEdit_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onInterfaceModeChanged(const QString&)));
    disconnect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onDependencyDirectionChanged(QString const&)));
    disconnect(&comDirectionCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onComDirectionChanged(QString const&)));
    disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(onComTransferTypeChanged(QString const&)));
    disconnect(&propertyValueEditor_, SIGNAL(contentChanged()), this, SLOT(onComPropertyValuesChanged()));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));

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

    portMapsView_.hide();

	mappingsLabel_.hide();

	parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onInterfaceModeChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onInterfaceModeChanged( const QString& newMode )
{
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointChangeCommand(
		static_cast<HWConnectionEndpoint*>(interface_), nameEdit_.text(),
        General::str2Interfacemode(newMode, General::MONITOR),
		descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onInterfaceNameChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onInterfaceNameChanged( const QString& newName )
{
	Q_ASSERT(interface_);
	
	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new EndpointNameChangeCommand(interface_, newName));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();
	
	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onDescriptionChanged()
{
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointDescChangeCommand(interface_, descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onPortMapChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onPortMapChanged()
{
	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

	// save the bus interface
	QSharedPointer<BusInterface> busIf = interface_->getBusInterface();

	Q_ASSERT_X(busIf, "PortmapInterfaceTab::onSave()", "Null BusInterface pointer");

	QList<QSharedPointer<PortMap> > portMaps;

    /*
	for (int i = 0; i < mappings_.rowCount(); i++)
    {
		QString logicalPortName = mappings_.item(i, 0)->text();
		QString physicalPortName = mappings_.item(i, 1)->text();

        QSharedPointer<PortMap::LogicalPort> logicalPort (new PortMap::LogicalPort(logicalPortName));
        QSharedPointer<PortMap::PhysicalPort> physicalPort (new PortMap::PhysicalPort(physicalPortName));

		QSharedPointer<PortMap> portMap(new PortMap);
        portMap->setLogicalPort(logicalPort);
        portMap->setPhysicalPort(physicalPort);

		portMaps.append(portMap);
	}*/

	QSharedPointer<QUndoCommand> cmd(
        new EndPointPortMapCommand(static_cast<HWConnectionEndpoint*>(interface_), portMaps));
	static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::refresh()
//-----------------------------------------------------------------------------
void InterfaceEditor::refresh()
{
	Q_ASSERT(interface_);
	setInterface(interface_, containingDesign_);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onDependencyDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onDependencyDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointDependencyDirectionChangeCommand(
        interface_, str2DependencyDirection(newDir, DEPENDENCY_PROVIDER)));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointComDirectionChangeCommand(
        interface_, DirectionTypes::str2Direction(newDir, DirectionTypes::IN)));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComTransferTypeChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComTransferTypeChanged(QString const& newTransferType)
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointTransferTypeChangeCommand(interface_, newTransferType));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComPropertyValuesChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComPropertyValuesChanged()
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointPropertyValuesChangeCommand(interface_,
                                                                             propertyValueEditor_.getData()));
    static_cast<DesignDiagram*>(interface_->scene())->getEditProvider()->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}
