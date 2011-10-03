/* 
 *  	Created on: 30.8.2011
 *      Author: Antti Kamppi
 * 		filename: createconfigurationdialog.cpp
 *		Project: Kactus 2
 */

#include "createconfigurationdialog.h"

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

#include <QDebug>

CreateConfigurationDialog::CreateConfigurationDialog(LibraryInterface* handler,
													 QSharedPointer<Component> component,
													 QWidget *parent,
													 Qt::WindowFlags f):
QDialog(parent, f),
handler_(handler),
component_(component),
useExistingRadio_(tr("Use current design"), this),
createNewRadio_(tr("Create new design"), this),
configNameEdit_(this),
vlnvEdit_(VLNV::DESIGNCONFIGURATION, handler, this, this, true),
implementationCombo_(this),
okButton_(QIcon(":/icons/graphics/ok.png"), tr("Ok"), this),
cancelButton_(QIcon(":/icons/graphics/cancel.png"), tr("Cancel"), this) {

	Q_ASSERT(component);
	Q_ASSERT(handler);

	setupLayout();
	setupConnections();

	// by default an existing design is used.
	useExistingRadio_.toggle();

	// set the views to the combo box
	implementationCombo_.addItem(QString(""));
	implementationCombo_.addItems(component->getViewNames());

	// set the title for vlnv editor
	vlnvEdit_.setTitle(tr("Configuration VLNV"));
	VLNV compVLNV = *component_->getVlnv();
	vlnvEdit_.setVendor(compVLNV.getVendor());
	vlnvEdit_.setLibrary(compVLNV.getLibrary());
	vlnvEdit_.setVersion(compVLNV.getVersion());

	setWindowTitle(tr("Create new configuration"));
}

CreateConfigurationDialog::~CreateConfigurationDialog() {
}

void CreateConfigurationDialog::setupLayout() {

	QHBoxLayout* radioLayout = new QHBoxLayout();
	radioLayout->addWidget(&useExistingRadio_);
	radioLayout->addWidget(&createNewRadio_);
	radioLayout->addStretch();

	QFormLayout* nameLayout = new QFormLayout();
	nameLayout->addRow(tr("Configuration name:"), &configNameEdit_);

	QFormLayout* comboLayout = new QFormLayout();
	comboLayout->addRow(tr("OPTIONAL reference to this component's\n"
		"top-level implementation view"), &implementationCombo_);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(&okButton_, 0, Qt::AlignRight);
	buttonLayout->addWidget(&cancelButton_, 0, Qt::AlignRight);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(radioLayout);
	topLayout->addLayout(nameLayout);
	topLayout->addWidget(&vlnvEdit_);
	topLayout->addLayout(comboLayout);
	topLayout->addLayout(buttonLayout);
}

void CreateConfigurationDialog::setupConnections() {

	connect(&okButton_, SIGNAL(clicked(bool)),
		this, SLOT(onOk()), Qt::UniqueConnection);
	connect(&cancelButton_, SIGNAL(clicked(bool)),
		this, SLOT(reject()), Qt::UniqueConnection);
	connect(&configNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onConfNameChanged(const QString&)), Qt::UniqueConnection);
}

void CreateConfigurationDialog::onOk() {
	QStringList errorList = validate();

	// if errors were found
	if (!errorList.isEmpty()) {
		QString errorText(tr("The following errors were found in the dialog:\n"));
		foreach (QString error, errorList) {
			errorText += ("- " + error + "\n");
		}

		QMessageBox::critical(this, tr("Invalid dialog input"), errorText);
	}
	// if no errors were found
	else 
		accept();
}

VLNV CreateConfigurationDialog::getConfigurationVLNV() const {
	VLNV configVLNV = vlnvEdit_.getVLNV();
	configVLNV.setType(VLNV::DESIGNCONFIGURATION);
	return configVLNV;
}

bool CreateConfigurationDialog::useExistingDesign() const {
	return useExistingRadio_.isChecked();
}

QString CreateConfigurationDialog::getConfigurationName() const {
	return configNameEdit_.text();
}

QString CreateConfigurationDialog::getImplementationViewName() const {
	return implementationCombo_.currentText();
}

QStringList CreateConfigurationDialog::validate() const {
	QStringList errors;

	if (configNameEdit_.text().isEmpty()) {
		errors.append(tr("A name for the configuration has to be specified."));
	}

	// if component already has a view with given name
	if (component_->findView(configNameEdit_.text())) {
		errors.append(tr("Component already has a configuration named %1.").arg(
			configNameEdit_.text()));
	}

	VLNV configVLNV = vlnvEdit_.getVLNV();
	// if configuration vlnv is not valid
	if (!configVLNV.isValid()) {
		errors.append(tr("The vlnv given for the configuration is not valid."));
	}
	// if vlnv was valid
	else {

		// if given VLNV is reserved
		if (handler_->contains(configVLNV)) {
			errors.append(tr("The vlnv given for the configuration is reserved."));
		}

		// is user wants to create a new design.
		if (!useExistingDesign()) {
			
			VLNV designVLNV = getDesignVLNV();
			if (handler_->contains(designVLNV)) {
				errors.append(tr("The vlnv for the new design is reserved."));
			}
		}

	}
	return errors;
}

VLNV CreateConfigurationDialog::getDesignVLNV() const {
	// first get the inputted values
	VLNV designVLNV = getConfigurationVLNV();
	designVLNV.setType(VLNV::DESIGN);

	// remove the possible ".designcfg" from the end
	QString name = designVLNV.getName().remove(QString(".designcfg"));

	// add .design to the end
	designVLNV.setName(name + ".design");
	return designVLNV;
}

void CreateConfigurationDialog::onConfNameChanged( const QString& newName ) {
	vlnvEdit_.setName(component_->getVlnv()->getName() + "." + newName + ".designcfg");
}
