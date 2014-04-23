/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portseditor.cpp
 */

#include "portseditor.h"

#include "portsdelegate.h"
#include <common/dialogs/NewBusDialog/NewBusDialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>
#include <models/component.h>

#include <models/businterface.h>
#include <IPXactWrapper/ComponentEditor/BusInterfaceWizard/BusInterfaceWizard.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>

PortsEditor::PortsEditor(QSharedPointer<Component> component,
						 LibraryInterface* handler,
						 bool showButtons,
						 QWidget *parent):
ItemEditor(component, handler, parent),
importButton_(QIcon(":/icons/graphics/import.png"), tr("Import CSV-file"), this),
exportButton_(QIcon(":/icons/graphics/export.png"), tr("Export CSV-file"), this),
view_(this), 
model_(component, this),
proxy_(this),
component_(component),
handler_(handler)
 {

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/portListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	connect(&importButton_, SIGNAL(clicked(bool)),
		&view_, SLOT(onCSVImport()), Qt::UniqueConnection);
	connect(&exportButton_, SIGNAL(clicked(bool)),
		&view_, SLOT(onCSVExport()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)),
		this, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
  
    connect(&view_, SIGNAL(createBus(QStringList const& )),
            this, SLOT(onCreateNewInteface(QStringList const& )));
    connect(&view_, SIGNAL(createExistingBus(QStringList const& )),
        this, SLOT(onCreateInterface(QStringList const& )));

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new PortsDelegate(this));

	view_.verticalHeader()->show();

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

    // Set case-insensitive sorting.
    proxy_.setSortCaseSensitivity(Qt::CaseInsensitive);

	// sort the view
	// view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&importButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&exportButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// if buttons are not displayed
	if (!showButtons) {
		importButton_.hide();
		exportButton_.hide();
	}

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Ports"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_, 1);
	layout->addLayout(buttonLayout);
	layout->setContentsMargins(0, 0, 0, 0);
}

PortsEditor::~PortsEditor() {
}

bool PortsEditor::isValid() const {
	return model_.isValid();
}

void PortsEditor::refresh() {
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::showEvent()
//-----------------------------------------------------------------------------
void PortsEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    emit helpUrlRequested("componenteditor/ports.html");
}

void PortsEditor::setAllowImportExport( bool allow ) {
	view_.setAllowImportExport(allow);
}

void PortsEditor::addPort( QSharedPointer<Port> port ) {
	model_.addPort(port);
}

void PortsEditor::removePort( QSharedPointer<Port> port ) {
	// find the index for the model parameter
	QModelIndex portIndex = model_.index(port);

	// if the model parameter was found
	if (portIndex.isValid()) {
		model_.onRemoveItem(portIndex);
	}
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

    if (handler_->contains(absVLNV)) {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, handler_, absVLNV, newAbsDefVLNV, absDirectory,
            "Set VLNV for abstraction definition")) {
                // if user canceled
                return;
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // Create a bus definition.
    QSharedPointer<BusDefinition> busDef(new BusDefinition());
    busDef->setVlnv(busVLNV);

    // Create the file for the bus definition.
    if (!handler_->writeModelToFile(absDirectory, busDef))
    {
        emit errorMessage(tr("Could not write model to %1").arg(absDirectory));
        return;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef(new AbstractionDefinition());
    absDef->setVlnv(absVLNV);

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
    busIf->setAbstractionType(absVLNV);
    busIf->setBusType(busVLNV);

    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this, absVLNV, 
        dialog.getSignalSelection() == NewBusDialog::USE_DESCRIPTION);
    component_->addBusInterface(busIf);   

    if (wizard.exec() == QWizard::Accepted)
    {
        emit createInterface();
        emit contentChanged();         
    }
    else
    {
        component_->removeBusInterface(busIf->getName());
    }
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::onCreateInterface()
//-----------------------------------------------------------------------------
void PortsEditor::onCreateInterface(QStringList const& selectedPorts)
{
    QSharedPointer<BusInterface> busIf(new BusInterface());
    
    // Open the bus interface wizard.
    BusInterfaceWizard wizard(component_, busIf, handler_, selectedPorts, this);
    component_->addBusInterface(busIf);   

    if (wizard.exec() == QWizard::Accepted)
    {
        emit createInterface();
        emit contentChanged();         
    }
    else
    {
        component_->removeBusInterface(busIf->getName());
    }
}
