//-----------------------------------------------------------------------------
// File: WirePortEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.11.2023
//
// Description:
// Editor for wire/transactional/structured ports.
//-----------------------------------------------------------------------------

#include "TypedPortEditor.h"

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/PortsInterface.h>
#include <editors/common/ExpressionSet.h>

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
// Function: TypedPortEditor::TypedPortEditor()
//-----------------------------------------------------------------------------
TypedPortEditor::TypedPortEditor(QSharedPointer<Component> component, 
    LibraryInterface* handler,
    PortsEditorFactory const* editorFactory,
    QString const& portType,
    QSharedPointer<PortsInterface> portsInterface,
    BusInterfaceInterface* busInterface,
    QWidget *parent):
ItemEditor(component, handler, parent),
    busInterface_(busInterface),
    portEditor_(component, handler, portsInterface, editorFactory, busInterface, this),
    portType_(portType)
{
    connect(&portEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(&portEditor_, SIGNAL(createNewInteface(QStringList const&)),
        this, SLOT(onCreateNewInteface(QStringList const&)), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(createInterface(QStringList const&)),
        this, SLOT(onCreateInterface(QStringList const&)), Qt::UniqueConnection);

    connect(&portEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&portEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&portEditor_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::isValid()
//-----------------------------------------------------------------------------
bool TypedPortEditor::isValid() const
{
    return portEditor_.isValid();
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::refresh()
//-----------------------------------------------------------------------------
void TypedPortEditor::refresh()
{
    portEditor_.refresh();
    busInterface_->setBusInterfaces(component());
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::showEvent()
//-----------------------------------------------------------------------------
void TypedPortEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);

    emit helpUrlRequested(QString("componenteditor/%1Ports.html").arg(portType_));
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::setAllowImportExport()
//-----------------------------------------------------------------------------
void TypedPortEditor::setAllowImportExport(bool allow)
{
    portEditor_.setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::onCreateNewInteface()
//-----------------------------------------------------------------------------
void TypedPortEditor::onCreateNewInteface(QStringList const& selectedPorts)
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
// Function: TypedPortEditor::onCreateInterface()
//-----------------------------------------------------------------------------
void TypedPortEditor::onCreateInterface(QStringList const& selectedPorts)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    
    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component(), busIf, handler(), selectedPorts, this);

    openBusInterfaceWizard(busIf, wizard);
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::connectBusInterfaceWizard()
//-----------------------------------------------------------------------------
void TypedPortEditor::openBusInterfaceWizard(QSharedPointer<BusInterface> busIf, BusInterfaceWizard& wizard)
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
// Function: TypedPortEditor::changeExtensionsEditorItem()
//-----------------------------------------------------------------------------
void TypedPortEditor::changeExtensionsEditorItem(QModelIndex const& itemIndex)
{
    if (!itemIndex.isValid())
    {
        emit changeVendorExtensions(QStringLiteral("Component: ") + component()->getVlnv().toString(), component());
    }
    else if (QSharedPointer<Port> selectedPort = portEditor_.getIndexedPort(itemIndex); selectedPort)
    {
        emit changeVendorExtensions(QStringLiteral("Port: ") + selectedPort->name(), selectedPort);
    }
}

//-----------------------------------------------------------------------------
// Function: TypedPortEditor::setupLayout()
//-----------------------------------------------------------------------------
void TypedPortEditor::setupLayout()
{
    QString labelText = portType_ + " ports";
    labelText.replace(0, 1, portType_.front().toUpper());

    auto layout = new QVBoxLayout(this);
    layout->addWidget(new SummaryLabel(labelText, this), 0, Qt::AlignCenter);
    layout->addWidget(&portEditor_);
    layout->setContentsMargins(0, 0, 0, 0);
}