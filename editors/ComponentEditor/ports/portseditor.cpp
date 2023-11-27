//-----------------------------------------------------------------------------
// File: portseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.03.2011
//
// Description:
// Editor to edit the ports of a component.
//-----------------------------------------------------------------------------

#include "portseditor.h"

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <KactusAPI/include/PortsInterface.h>
#include <editors/ComponentEditor/ports/MasterPortsEditor.h>
#include <editors/ComponentEditor/ports/WirePortsEditorFactory.h>
#include <editors/ComponentEditor/ports/TransactionalPortsEditorFactory.h>

#include <common/dialogs/NewBusDialog/NewBusDialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <wizards/BusInterfaceWizard/BusInterfaceWizard.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QCompleter>
#include <QVBoxLayout>

namespace
{
    QString const WIREPORTS = QLatin1String("Wire ports");
    QString const TRANSACTIONALPORTS = QLatin1String("Transactional ports");
};

//-----------------------------------------------------------------------------
// Function: PortsEditor::PortsEditor()
//-----------------------------------------------------------------------------
PortsEditor::PortsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    ExpressionSet expressions,
    QSharedPointer<PortValidator> portValidator, BusInterfaceInterface* busInterface,
    QWidget *parent):
ItemEditor(component, handler, parent),
component_(component),
handler_(handler),
wireEditor_(0),
transactionalEditor_(0),
portTabs_(new QTabWidget(this)),
portsInterface_(new PortsInterface(portValidator, expressions.parser, expressions.formatter)),
busInterface_(busInterface)
{
    portsInterface_->setPorts(component->getPorts());

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/portListing.csv").arg(componentPath);

    QSharedPointer<PortAbstractionInterface> signalInterface(new PortAbstractionInterface());

    WirePortsEditorFactory wireFactory(component, expressions, portValidator,
        portsInterface_, signalInterface, busInterface_, defaultPath);
    wireEditor_ = new MasterPortsEditor(component, handler, portsInterface_,  &wireFactory,  busInterface, this);

    TransactionalPortsEditorFactory transactionalFactory(component, expressions, portValidator,
        portsInterface_, signalInterface, busInterface_, defaultPath);
    transactionalEditor_ = new MasterPortsEditor(component, handler, portsInterface_, 
        &transactionalFactory, busInterface, this);

    connectSignals();

	auto summaryLabel = new SummaryLabel(tr("Ports summary"), this);

    portTabs_->addTab(wireEditor_, getTabNameWithPortCount(WIREPORTS, wireEditor_->getAmountOfPorts()));
    portTabs_->addTab(transactionalEditor_,
        getTabNameWithPortCount(TRANSACTIONALPORTS, transactionalEditor_->getAmountOfPorts()));

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
 	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(portTabs_);
	layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: portseditor::connectSignals()
//-----------------------------------------------------------------------------
void PortsEditor::connectSignals()
{
    connect(wireEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(createNewInteface(QStringList const&)),
        this, SLOT(onCreateNewInteface(QStringList const&)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(createInterface(QStringList const&)),
        this, SLOT(onCreateInterface(QStringList const&)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(createNewInteface(QStringList const&)),
        this, SLOT(onCreateNewInteface(QStringList const&)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(createInterface(QStringList const&)),
        this, SLOT(onCreateInterface(QStringList const&)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(invalidateOtherFilter()),
        transactionalEditor_, SIGNAL(ivalidateThisFilter()), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(invalidateOtherFilter()),
        wireEditor_, SIGNAL(ivalidateThisFilter()), Qt::UniqueConnection);

    connect(portTabs_, SIGNAL(currentChanged(int)), this, SLOT(changeHelpFile()), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(portCountChanged()), this, SLOT(redefineTabText()), Qt::UniqueConnection);
    connect(transactionalEditor_, SIGNAL(portCountChanged()), this, SLOT(redefineTabText()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: portseditor::getTabNameWithPortCount()
//-----------------------------------------------------------------------------
QString PortsEditor::getTabNameWithPortCount(QString const& tabName, int const& portCount) const
{
    QString tabText = tabName + QLatin1String(" (") + QString::number(portCount) + QLatin1Char(')');
    return tabText;
}

//-----------------------------------------------------------------------------
// Function: portseditor::redefineTabText()
//-----------------------------------------------------------------------------
void PortsEditor::redefineTabText()
{
    for (int i = 0; i < portTabs_->count(); ++i)
    {
        if (portTabs_->widget(i) == wireEditor_)
        {
            int portCount = wireEditor_->getAmountOfPorts();
            portTabs_->setTabText(i, getTabNameWithPortCount(WIREPORTS, portCount));
        }
        else if (portTabs_->widget(i) == transactionalEditor_)
        {
            int portCount = transactionalEditor_->getAmountOfPorts();
            portTabs_->setTabText(i, getTabNameWithPortCount(TRANSACTIONALPORTS, portCount));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::isValid()
//-----------------------------------------------------------------------------
bool PortsEditor::isValid() const
{
    return wireEditor_->isValid() && transactionalEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: portseditor::refresh()
//-----------------------------------------------------------------------------
void PortsEditor::refresh()
{
    wireEditor_->refresh();
    transactionalEditor_->refresh();
    busInterface_->setBusInterfaces(component_);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::showEvent()
//-----------------------------------------------------------------------------
void PortsEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    changeHelpFile();
}

//-----------------------------------------------------------------------------
// Function: portseditor::changeHelpFile()
//-----------------------------------------------------------------------------
void PortsEditor::changeHelpFile()
{
    if (portTabs_->currentWidget() == wireEditor_)
    {
        emit helpUrlRequested("componenteditor/wirePorts.html");
    }
    else
    {
        emit helpUrlRequested("componenteditor/transactionalPorts.html");
    }
}

//-----------------------------------------------------------------------------
// Function: portseditor::setAllowImportExport()
//-----------------------------------------------------------------------------
void PortsEditor::setAllowImportExport(bool allow)
{
    wireEditor_->setAllowImportExport(allow);
    transactionalEditor_->setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::setComponent()
//-----------------------------------------------------------------------------
void PortsEditor::setComponent(QSharedPointer<Component> component)
{
    component_ = component;

    portsInterface_->setPorts(component_->getPorts());
    wireEditor_->setComponent(component);
    transactionalEditor_->setComponent(component);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::onCreateNewInteface()
//-----------------------------------------------------------------------------
void PortsEditor::onCreateNewInteface(QStringList const& selectedPorts)
{
    // Ask user for new bus definition VLNV.
    NewBusDialog dialog(handler_, this);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // create the vlnvs that identify the bus definition and abstraction definition
    VLNV busVLNV = dialog.getVLNV();
    busVLNV.setType(VLNV::BUSDEFINITION);
    
    // remove the possible .busDef from the end of the name field
    VLNV absVLNV = busVLNV;
    QString absDefName = busVLNV.getName();
    absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);
    absVLNV.setName(absDefName + ".absDef");
    absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

    // by default the abs def and bus def are saved to same directory
    QString absDirectory = dialog.getPath();

    if (handler_->contains(absVLNV))
    {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, handler_, absVLNV, newAbsDefVLNV, absDirectory,
            QStringLiteral("Set VLNV for abstraction definition")))
        {
                return; // if user canceled
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // Create a bus definition.
    QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, component_->getRevision()));

    // Create the file for the bus definition.
    if (!handler_->writeModelToFile(absDirectory, busDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef(new AbstractionDefinition(absVLNV, component_->getRevision()));
    
    // set reference from abstraction definition to bus definition
    absDef->setBusType(busVLNV);

    // create the file for the abstraction definition
    if (!handler_->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // Create interface with unique name within the component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    QString ifName = busVLNV.getName();
    int id = 1;
    while(component_->hasInterface(ifName))
    {
        ifName = busVLNV.getName() + "_" + QString::number(id);
        id++;
    }

    busIf->setName(ifName);
    QSharedPointer<AbstractionType> abstractionVLNV(new AbstractionType());
    abstractionVLNV->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(absVLNV)));
    busIf->getAbstractionTypes()->append(abstractionVLNV);
    busIf->setBusType(busVLNV);

    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this, absVLNV,
        dialog.getSignalSelection() == NewBusDialog::USE_DESCRIPTION);

    openBusInterfaceWizard(busIf, wizard);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::onCreateInterface()
//-----------------------------------------------------------------------------
void PortsEditor::onCreateInterface(QStringList const& selectedPorts)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    
    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this);

    openBusInterfaceWizard(busIf, wizard);
}

//-----------------------------------------------------------------------------
// Function: portseditor::connectBusInterfaceWizard()
//-----------------------------------------------------------------------------
void PortsEditor::openBusInterfaceWizard(QSharedPointer<BusInterface> busIf, BusInterfaceWizard& wizard)
{
    component_->getBusInterfaces()->append(busIf);

    connect(&wizard, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&wizard, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    if (wizard.exec() == QWizard::Accepted)
    {
        emit createInterface();
        emit contentChanged();
    }
    else
    {
        component_->getBusInterfaces()->removeAll(busIf);
    }
}

//-----------------------------------------------------------------------------
// Function: portseditor::changeExtensionsEditorItem()
//-----------------------------------------------------------------------------
void PortsEditor::changeExtensionsEditorItem(QModelIndex const& itemIndex)
{
    QSharedPointer<Extendable> extensionItem;
    QString extensionID;

    if (!itemIndex.isValid())
    {
        extensionItem = component_;
        extensionID = QLatin1String("Component: ") + component_->getVlnv().toString();
    }
    else
    {
        QSharedPointer<Port> selectedPort;
        if (portTabs_->currentWidget() == wireEditor_)
        {
            selectedPort = wireEditor_->getIndexedPort(itemIndex);
        }
        else
        {
            selectedPort = transactionalEditor_->getIndexedPort(itemIndex);
        }

        if (!selectedPort)
        {
            return;
        }

        extensionItem = selectedPort;
        extensionID = QLatin1String("Port: ") + selectedPort->name();
    }

    emit changeVendorExtensions(extensionID, extensionItem);
}
