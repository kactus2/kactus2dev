//-----------------------------------------------------------------------------
// File: interfaceeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 10.10.2011
//
// Description:
// Editor to display/edit the details of a bus/api/com interface.
//-----------------------------------------------------------------------------

#include "interfaceeditor.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <designEditors/common/InterfaceEditor/InterfacePortMapModel.h>
#include <designEditors/common/InterfaceEditor/InterfacePortMapDelegate.h>
#include <designEditors/common/InterfaceEditor/InterfacePortMapColumns.h>

#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/HierInterface.h>
#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/validators/namevalidator.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>

#include <QVBoxLayout>
#include <QHeaderView>

namespace
{
    enum INTERFACE_TYPE {
        BUS = 0,
        COM, 
        API,
        NO_TYPE
    };
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::InterfaceEditor()
//-----------------------------------------------------------------------------
InterfaceEditor::InterfaceEditor(QWidget *parent, LibraryInterface* handler):
QStackedWidget(parent),
    library_(handler),
    busType_(this),
    absType_(this),
    busNameEditor_(this),
    modeEdit_(this),
    portMapsView_(this),
    portMapsModel_(new InterfacePortMapModel(handler, this)),
    busDescriptionEditor_(this),
    comType_(this),
    comNameEditor_(this),
    transferTypeCombo_(this),
    comDirectionCombo_(this),
    comDescriptionEditor_(this),
    apiType_(this),
    apiNameEditor_(this),   
    dependencyDirCombo_(this), 
    apiDescriptionEditor_(this),
    propertyValueEditor_(this),
    interface_(0),
    containingDesign_(),
    editProvider_()
{
    Q_ASSERT(parent);
    Q_ASSERT(handler);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);       

    setupBusEditor();
    setupComEditor();
    setupApiEditor();
    insertWidget(NO_TYPE, new QWidget(this));

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
void InterfaceEditor::setInterface(ConnectionEndpoint* interface, QSharedPointer<Design> containingDesign, 
    QSharedPointer<IEditProvider> editProvider, bool locked)
{
	Q_ASSERT(interface);

	parentWidget()->raise();

	// disconnect the previous interface
	if (interface_)
    {
		disconnect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)), this, SLOT(clear()));
		disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));
	}

	interface_ = interface;
    containingDesign_ = containingDesign;
    editProvider_ = editProvider;
    locked_ = locked;

    // Fill the rest of the editors based on the interface type.
    if (interface_->isBus())
    {
        setBusInterface();
    }
    else if (interface_->isApi())
    {
        setApiInterface();
    }
    else if (interface_->isCom())
    {
        setComInterface();
    }
    else
    {
        setCurrentIndex(NO_TYPE);
    }

	connect(interface_, SIGNAL(destroyed(ConnectionEndpoint*)), this, SLOT(clear()), Qt::UniqueConnection);
	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);

    bool editingEnabled = !locked_ && (interface_->isTemporary() || !interface_->isTypeLocked());

    busNameEditor_.setEnabled(editingEnabled);
    busDescriptionEditor_.setEnabled(editingEnabled);
    modeEdit_.setEnabled(editingEnabled);
    portMapsModel_->setLock(locked_);

    apiNameEditor_.setEnabled(editingEnabled);
    dependencyDirCombo_.setEnabled(editingEnabled);
    transferTypeCombo_.setEnabled(editingEnabled);
    apiDescriptionEditor_.setEnabled(editingEnabled);

    comNameEditor_.setEnabled(editingEnabled);
    comDirectionCombo_.setEnabled(editingEnabled);
    comDescriptionEditor_.setEnabled(editingEnabled);
    propertyValueEditor_.setLock(locked_);    

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::refresh()
//-----------------------------------------------------------------------------
void InterfaceEditor::refresh()
{
    Q_ASSERT(interface_);
    setInterface(interface_, containingDesign_, editProvider_, locked_);
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
        editProvider_.clear();
    }

    setCurrentIndex(NO_TYPE);

    parentWidget()->setMaximumHeight(20);    
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onInterfaceModeChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onInterfaceModeChanged(QString const& newMode)
{
	Q_ASSERT(interface_);

	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointChangeCommand(static_cast<HWConnectionEndpoint*>(interface_), 
        General::str2Interfacemode(newMode, General::INTERFACE_MODE_COUNT)));

	editProvider_->addCommand(cmd);
    cmd->redo();

	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onInterfaceNameChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onInterfaceNameChanged(QString const& newName)
{
	Q_ASSERT(interface_);
	
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new EndpointNameChangeCommand(interface_, newName, getDesignInterfaces()));
	editProvider_->addCommand(cmd);
    cmd->redo();
	
	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onDescriptionChanged()
{    
	Q_ASSERT(interface_);

    QString interfaceDescription;
    if (interface_->isBus())
    {
        interfaceDescription = busDescriptionEditor_.toPlainText();
    }
    else if (interface_->isCom())
    {
        interfaceDescription = comDescriptionEditor_.toPlainText();
    }
    else if (interface_->isApi())
    {
        interfaceDescription = apiDescriptionEditor_.toPlainText();
    }

	disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new EndpointDescChangeCommand(interface_, interfaceDescription));
	editProvider_->addCommand(cmd);
    cmd->redo();

	connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onDependencyDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onDependencyDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointDependencyDirectionChangeCommand(interface_,
        str2DependencyDirection(newDir, DEPENDENCY_PROVIDER)));

    editProvider_->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::onComDirectionChanged()
//-----------------------------------------------------------------------------
void InterfaceEditor::onComDirectionChanged(QString const& newDir)
{
    disconnect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()));

    QSharedPointer<QUndoCommand> cmd(new EndpointComDirectionChangeCommand(interface_,
        DirectionTypes::str2Direction(newDir, DirectionTypes::IN)));

    editProvider_->addCommand(cmd);
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
    editProvider_->addCommand(cmd);
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
    editProvider_->addCommand(cmd);
    cmd->redo();

    connect(interface_, SIGNAL(contentChanged()), this, SLOT(refresh()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setBusInterface()
//-----------------------------------------------------------------------------
void InterfaceEditor::setBusInterface()
{
    Q_ASSERT(interface_->getBusInterface());
    
    busType_.setVLNV(interface_->getBusInterface()->getBusType(), true);

    setNameAndDescription(&busNameEditor_, &busDescriptionEditor_);

    if (interface_->getBusInterface()->getAbstractionTypes() && 
        !interface_->getBusInterface()->getAbstractionTypes()->isEmpty() &&
        interface_->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef())
    {
        absType_.setVLNV(*interface_->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef(), true);
    }
    else
    {
        absType_.setVLNV(VLNV(), true);
    }

    // Set selection for mode editor, signal must be disconnected when mode is set to avoid loops.
    disconnect(&modeEdit_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onInterfaceModeChanged(QString const&)));

    int index = modeEdit_.findText(General::interfaceMode2Str(interface_->getBusInterface()->getInterfaceMode()));
    modeEdit_.setCurrentIndex(index);

    connect(&modeEdit_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onInterfaceModeChanged(QString const&)), Qt::UniqueConnection);

    portMapsModel_->setInterfaceData(interface_, getActiveInterfaces());

    setCurrentIndex(BUS);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setNameAndDescription()
//-----------------------------------------------------------------------------
void InterfaceEditor::setNameAndDescription(QLineEdit* nameEditor, QPlainTextEdit* descriptionEditor)
{
    // Set text for the name editor, signal must be disconnected when name is set to avoid loops.
    disconnect(nameEditor, SIGNAL(textEdited(QString const&)), this, SLOT(onInterfaceNameChanged(QString const&)));
    nameEditor->setText(interface_->name());
    connect(nameEditor, SIGNAL(textEdited(QString const&)), this, SLOT(onInterfaceNameChanged(QString const&)),
        Qt::UniqueConnection);

    disconnect(descriptionEditor, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));
    descriptionEditor->setPlainText(interface_->description());
    connect(descriptionEditor, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::getActiveInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ActiveInterface> > InterfaceEditor::getActiveInterfaces() const
{
    if (interface_->encompassingComp() == 0)
    {
        return QList<QSharedPointer<ActiveInterface> >();
    }

    QList<QSharedPointer<ActiveInterface> > endPointInterfaces;

    foreach (QSharedPointer<Interconnection> connection, *containingDesign_->getInterconnections())
    {
        QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();

        if (activeInterfaceReferencesBusInterface(startInterface))
        {
            endPointInterfaces.append(startInterface);
        }

        foreach (QSharedPointer<ActiveInterface> currentInterface, *connection->getActiveInterfaces())
        {
            if (activeInterfaceReferencesBusInterface(currentInterface))
            {
                endPointInterfaces.append(currentInterface);
            }
        }
    }

    return endPointInterfaces;
}

//-----------------------------------------------------------------------------
// Function: interfaceeditor::activeInterfaceReferencesBusInterface()
//-----------------------------------------------------------------------------
bool InterfaceEditor::activeInterfaceReferencesBusInterface(QSharedPointer<ActiveInterface> currentInterface) const
{
    return currentInterface->getComponentReference().compare(interface_->encompassingComp()->name()) == 0 &&
        currentInterface->getBusReference().compare(interface_->name()) == 0;
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::getDesignInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<HierInterface> > InterfaceEditor::getDesignInterfaces() const
{
    QList<QSharedPointer<HierInterface> > interfaces;

    if (interface_->isHierarchical())
    {
        foreach (QSharedPointer<Interconnection> connection, *containingDesign_->getInterconnections())
        {
            QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();

            foreach (QSharedPointer<HierInterface> currentHierInterface, *connection->getHierInterfaces())
            {
                if (currentHierInterface->getBusReference().compare(interface_->name()) == 0)
                {
                    interfaces.append(currentHierInterface);
                }
            }
        }
    }
    else
    {
        foreach (QSharedPointer<ActiveInterface> activeInteface, getActiveInterfaces())
        {
            interfaces.append(activeInteface);
        }
    }

    return interfaces;
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setComInterface()
//-----------------------------------------------------------------------------
void InterfaceEditor::setComInterface()
{
    Q_ASSERT(interface_->getComInterface());

    VLNV comType = interface_->getComInterface()->getComType();

    comType_.setVLNV(comType, true);

    // set text for the name editor, signal must be disconnected when name is set to avoid loops 
    setNameAndDescription(&comNameEditor_, &comDescriptionEditor_);

    // Fill in the possible values of the data type.
    disconnect(&transferTypeCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onComTransferTypeChanged(QString const&)));

    transferTypeCombo_.clear();
    transferTypeCombo_.addItem("");

    QSharedPointer<ComDefinition const> comDefinition;
    if (comType.isValid())
    {
        comDefinition = library_->getModelReadOnly(comType).dynamicCast<ComDefinition const>();
    }

    if (comDefinition != 0)
    {
        transferTypeCombo_.addItems(*comDefinition->getTransferTypes());
        transferTypeCombo_.setCurrentIndex(transferTypeCombo_.findText(interface_->getComInterface()->getTransferType()));
    }

    connect(&transferTypeCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onComTransferTypeChanged(QString const&)), Qt::UniqueConnection);

    // Set selection for COM direction.
    disconnect(&comDirectionCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onComDirectionChanged(QString const&)));

    comDirectionCombo_.setCurrentIndex(comDirectionCombo_.findText(DirectionTypes::direction2Str(
        interface_->getComInterface()->getDirection())));

    connect(&comDirectionCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onComDirectionChanged(QString const&)), Qt::UniqueConnection);

    // Set property values and try to read the allowed properties from the COM definition if valid.
    disconnect(&propertyValueEditor_, SIGNAL(contentChanged()), this, SLOT(onComPropertyValuesChanged()));

    propertyValueEditor_.setData(interface_->getComInterface()->getPropertyValues());

    if (comDefinition != 0)
    {
        propertyValueEditor_.setAllowedProperties(*comDefinition->getProperties());
    }

    connect(&propertyValueEditor_, SIGNAL(contentChanged()),
        this, SLOT(onComPropertyValuesChanged()), Qt::UniqueConnection);

    setCurrentIndex(COM);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setApiInterface()
//-----------------------------------------------------------------------------
void InterfaceEditor::setApiInterface()
{
    Q_ASSERT(interface_->getApiInterface());

    apiType_.setVLNV(interface_->getApiInterface()->getApiType(), true);

    setNameAndDescription(&apiNameEditor_, &apiDescriptionEditor_);

    disconnect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onDependencyDirectionChanged(QString const&)));

    dependencyDirCombo_.setCurrentIndex(dependencyDirCombo_.findText(
        dependencyDirection2Str(interface_->getApiInterface()->getDependencyDirection())));

    connect(&dependencyDirCombo_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onDependencyDirectionChanged(QString const&)), Qt::UniqueConnection);

    setCurrentIndex(API);    
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setupBusEditor()
//-----------------------------------------------------------------------------
void InterfaceEditor::setupBusEditor()
{
    busNameEditor_.setValidator(new NameValidator(&busNameEditor_));

    busType_.setTitle(tr("Bus type VLNV"));
    busType_.setFlat(false);

    absType_.setTitle(tr("Abstraction type VLNV"));
    absType_.setFlat(false);

    // set the possible modes to the mode editor
    for (int i = 0; i <= General::INTERFACE_MODE_COUNT; i++)
    {
        modeEdit_.addItem(General::INTERFACE_MODE_NAMES[i]);
    }

    portMapsView_.setSortingEnabled(true);
    portMapsView_.setSelectionMode(QAbstractItemView::SingleSelection);
    portMapsView_.verticalHeader()->hide();

    portMapsView_.resizeColumnsToContents();

    portMapsView_.setContextMenuPolicy(Qt::NoContextMenu);
    portMapsView_.setAlternatingRowColors(false);
    portMapsView_.horizontalHeader()->setStretchLastSection(true);

    portMapsView_.setItemDelegate(new InterfacePortMapDelegate(this));

    QSortFilterProxyModel* sortProxy = new QSortFilterProxyModel(this);
    sortProxy->setSourceModel(portMapsModel_);
    portMapsView_.setModel(sortProxy);

    connect(portMapsModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    QWidget* busTypeEditor(new QWidget(this));
    QVBoxLayout* busLayout(new QVBoxLayout(busTypeEditor));
    busLayout->addWidget(&busType_);
    busLayout->addWidget(&absType_);
    busLayout->addWidget(new QLabel(tr("Bus interface name:"), this));
    busLayout->addWidget(&busNameEditor_);
    busLayout->addWidget(new QLabel(tr("Interface mode:"), this));
    busLayout->addWidget(&modeEdit_);
    busLayout->addWidget(new QLabel(tr("Description:"), this));
    busLayout->addWidget(&busDescriptionEditor_, 1);    
    busLayout->addWidget(new QLabel(tr("Port maps:"), this));
    busLayout->addWidget(&portMapsView_, 1, Qt::AlignTop);

    insertWidget(BUS, busTypeEditor);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setupComEditor()
//-----------------------------------------------------------------------------
void InterfaceEditor::setupComEditor()
{
    comType_.setTitle(tr("COM type VLNV"));
    comType_.setFlat(false);

    comNameEditor_.setValidator(new NameValidator(&comNameEditor_));

    comDirectionCombo_.addItem("in");
    comDirectionCombo_.addItem("out");
    comDirectionCombo_.addItem("inout");

    QWidget* comTypeEditor = new QWidget(this);
    QVBoxLayout* comLayout = new QVBoxLayout(comTypeEditor);
    comLayout->addWidget(&comType_);
    comLayout->addWidget(new QLabel(tr("COM interface name:"), this));
    comLayout->addWidget(&comNameEditor_);
    comLayout->addWidget(new QLabel(tr("Transfer type:"), this));
    comLayout->addWidget(&transferTypeCombo_);
    comLayout->addWidget(new QLabel(tr("Direction:"), this));
    comLayout->addWidget(&comDirectionCombo_);
    comLayout->addWidget(new QLabel(tr("Description:"), this));
    comLayout->addWidget(&comDescriptionEditor_);    
    comLayout->addWidget(&propertyValueEditor_, 1);

    insertWidget(COM, comTypeEditor);
}

//-----------------------------------------------------------------------------
// Function: InterfaceEditor::setupApiEditor()
//-----------------------------------------------------------------------------
void InterfaceEditor::setupApiEditor()
{
    apiType_.setTitle(tr("API type VLNV"));
    apiType_.setFlat(false);

    apiNameEditor_.setValidator(new NameValidator(&apiNameEditor_));

    dependencyDirCombo_.addItem("requester");
    dependencyDirCombo_.addItem("provider");

    QWidget* apiTypeEditor = new QWidget(this);
    QVBoxLayout* apiLayout = new QVBoxLayout(apiTypeEditor);
    apiLayout->addWidget(&apiType_);
    apiLayout->addWidget(new QLabel(tr("API interface name:"), this));
    apiLayout->addWidget(&apiNameEditor_);
    apiLayout->addWidget(new QLabel(tr("Dependency direction:"), this));
    apiLayout->addWidget(&dependencyDirCombo_);
    apiLayout->addWidget(new QLabel(tr("Description:"), this));
    apiLayout->addWidget(&apiDescriptionEditor_, 1);   
    apiLayout->addStretch(1);

    insertWidget(API, apiTypeEditor);
}
