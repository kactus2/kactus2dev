//-----------------------------------------------------------------------------
// File: WirePortEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.11.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "WirePortEditor.h"

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <KactusAPI/include/PortsInterface.h>
#include <editors/ComponentEditor/ports/MasterPortsEditor.h>
#include <editors/ComponentEditor/ports/WirePortsEditorConstructor.h>

#include <common/dialogs/NewBusDialog/NewBusDialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <wizards/BusInterfaceWizard/BusInterfaceWizard.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: WirePortEditor::WirePortEditor()
//-----------------------------------------------------------------------------
WirePortEditor::WirePortEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<PortValidator> portValidator, BusInterfaceInterface* busInterface,
    QWidget *parent):
ItemEditor(component, handler, parent),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder)),
portsInterface_(new PortsInterface(portValidator, expressionParser_, expressionFormatter)),
busInterface_(busInterface)
{
    portsInterface_->setPorts(component->getPorts());

    const QString defaultPath = QString("%1/wireListing.csv").arg(handler->getDirectoryPath(component->getVlnv()));

    QSharedPointer<PortAbstractionInterface> signalInterface(new PortAbstractionInterface());

    auto componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    WirePortsEditorConstructor wireFactory(component, componentParametersModel, parameterFinder, portValidator,
        portsInterface_, signalInterface, busInterface_, defaultPath);
    wireEditor_ = new MasterPortsEditor(component, handler, portsInterface_, signalInterface,
        &wireFactory, parameterFinder, busInterface, this);
    
    connect(wireEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(createNewInteface(QStringList const&)),
        this, SLOT(onCreateNewInteface(QStringList const&)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(createInterface(QStringList const&)),
        this, SLOT(onCreateInterface(QStringList const&)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(wireEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(wireEditor_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: WirePortEditor::isValid()
//-----------------------------------------------------------------------------
bool WirePortEditor::isValid() const
{
    return wireEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: portseditor::refresh()
//-----------------------------------------------------------------------------
void WirePortEditor::refresh()
{
    wireEditor_->refresh();
    busInterface_->setBusInterfaces(component());
}

//-----------------------------------------------------------------------------
// Function: WirePortEditor::showEvent()
//-----------------------------------------------------------------------------
void WirePortEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);

    emit helpUrlRequested("componenteditor/wirePorts.html");
}

//-----------------------------------------------------------------------------
// Function: portseditor::setAllowImportExport()
//-----------------------------------------------------------------------------
void WirePortEditor::setAllowImportExport(bool allow)
{
    wireEditor_->setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: WirePortEditor::onCreateNewInteface()
//-----------------------------------------------------------------------------
void WirePortEditor::onCreateNewInteface(QStringList const& selectedPorts)
{
    // Ask user for new bus definition VLNV.
    NewBusDialog dialog(handler(), this);
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

    if (handler()->contains(absVLNV))
    {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, handler(), absVLNV, newAbsDefVLNV, absDirectory,
            QStringLiteral("Set VLNV for abstraction definition")))
        {
                return; // if user canceled
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // Create a bus definition.
    QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, component()->getRevision()));

    // Create the file for the bus definition.
    if (!handler()->writeModelToFile(absDirectory, busDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef(new AbstractionDefinition(absVLNV, component()->getRevision()));
    
    // set reference from abstraction definition to bus definition
    absDef->setBusType(busVLNV);

    // create the file for the abstraction definition
    if (!handler()->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // Create interface with unique name within the component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    QString ifName = busVLNV.getName();
    int id = 1;
    while(component()->hasInterface(ifName))
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
    BusInterfaceWizard wizard(component(), busIf, handler(), selectedPorts, this, absVLNV,
        dialog.getSignalSelection() == NewBusDialog::USE_DESCRIPTION);

    openBusInterfaceWizard(busIf, wizard);
}

//-----------------------------------------------------------------------------
// Function: WirePortEditor::onCreateInterface()
//-----------------------------------------------------------------------------
void WirePortEditor::onCreateInterface(QStringList const& selectedPorts)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    
    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component(), busIf, handler(), selectedPorts, this);

    openBusInterfaceWizard(busIf, wizard);
}

//-----------------------------------------------------------------------------
// Function: portseditor::connectBusInterfaceWizard()
//-----------------------------------------------------------------------------
void WirePortEditor::openBusInterfaceWizard(QSharedPointer<BusInterface> busIf, BusInterfaceWizard& wizard)
{
    component()->getBusInterfaces()->append(busIf);

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
        component()->getBusInterfaces()->removeAll(busIf);
    }
}

//-----------------------------------------------------------------------------
// Function: portseditor::changeExtensionsEditorItem()
//-----------------------------------------------------------------------------
void WirePortEditor::changeExtensionsEditorItem(QModelIndex const& itemIndex)
{
    if (!itemIndex.isValid())
    {
        emit changeVendorExtensions(QStringLiteral("Component: ") + component()->getVlnv().toString(), component());
    }
    else if (QSharedPointer<Port> selectedPort = wireEditor_->getIndexedPort(itemIndex); selectedPort)
    {
        emit changeVendorExtensions(QStringLiteral("Port: ") + selectedPort->name(), selectedPort);
    }
}

//-----------------------------------------------------------------------------
// Function: portseditor::setupLayout()
//-----------------------------------------------------------------------------
void WirePortEditor::setupLayout()
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(new SummaryLabel(tr("Wire ports"), this), 0, Qt::AlignCenter);
    layout->addWidget(wireEditor_);
    layout->setContentsMargins(0, 0, 0, 0);
}