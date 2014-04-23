/* 
 *
 *  Created on: 9.10.2010
 *      Author: kamppia
 */

#include "createvlnvdialog.h"
#include <library/LibraryManager/vlnv.h>

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QSizePolicy>
#include <QCoreApplication>

CreateVLNVDialog::CreateVLNVDialog(QWidget* parent, const VLNV* const vlnv):
QDialog(parent), type_(VLNV::INVALID),
vendorEdit_(0), libraryEdit_(0), nameEdit_(0), versionEdit_(0), layout_(0) {

	// the layout manager to control the layout of the dialog
	layout_ = new QGridLayout(this);
	layout_->setColumnStretch(1, 1);

	// the top text
	QLabel* topLabel = new QLabel(tr("Set the new vlnv information"), this);
	layout_->addWidget(topLabel, 0, 0, 1, 2, Qt::AlignTop | Qt::AlignLeft);

	// set the type if the vlnv pointer is valid
	if (vlnv) {
		type_ = vlnv->getType();
	}

	setTypeBox(vlnv);

	setVendor(vlnv);

	setLibrary(vlnv);

	setName(vlnv);

	setVersion(vlnv);


	// The ok and cancel buttons and their layout
	QPushButton* okButton = new QPushButton(tr("OK"), this);
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked(bool)),
			this, SLOT(accept()));
	layout_->addWidget(okButton, 6, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);

	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked(bool)),
			this, SLOT(reject()));
	layout_->addWidget(cancelButton, 6, 1, 1, 1, Qt::AlignLeft | Qt::AlignTop);

	return;
}

CreateVLNVDialog::~CreateVLNVDialog() {
}

void CreateVLNVDialog::accept() {
	done(QDialog::Accepted);
}

void CreateVLNVDialog::reject() {
	done(QDialog::Rejected);
}

void CreateVLNVDialog::onAbstractorSelected() {
	type_ = VLNV::ABSTRACTOR;
}

void CreateVLNVDialog::onAbstractionDefSelected() {
	type_ = VLNV::ABSTRACTIONDEFINITION;
}

void CreateVLNVDialog::onBusDefSelected() {
	type_ = VLNV::BUSDEFINITION;
}

void CreateVLNVDialog::onComponentSelected() {
	type_ = VLNV::COMPONENT;
}

void CreateVLNVDialog::onDesignSelected() {
	type_ = VLNV::DESIGN;
}

void CreateVLNVDialog::onDesignConfSelected() {
	type_ = VLNV::DESIGNCONFIGURATION;
}

void CreateVLNVDialog::onGeneratorChainSelected() {
	type_ = VLNV::GENERATORCHAIN;
}

VLNV* CreateVLNVDialog::execDialog() {

	// call the base class implementation of the exec() function

	// if user clicked ok
	if (QDialog::exec() == QDialog::Accepted) {
		return createVLNV();
	}
	// user clicked cancel
	else {
		return 0;
	}
}

VLNV* CreateVLNVDialog::createVLNV() {

	// get the info from the lineEdit widgets
	QString vendor(vendorEdit_->text());
	QString library(libraryEdit_->text());
	QString name(nameEdit_->text());
	QString version(versionEdit_->text());

	// if the type is not specified the creation of vlnv is not possible
	if (type_ == VLNV::INVALID) {
		QString title(QCoreApplication::applicationName());
		QString text(tr("The type of vlnv was not specified"));
		QMessageBox::critical(this, title, text);
		return 0;
	}
	// none of the vlnv text fields can't be empty
	if (vendor.isEmpty() ||
			library.isEmpty() ||
			name.isEmpty() ||
			version.isEmpty()) {

		QString title(QCoreApplication::applicationName());
		QString text(tr("None of the text fields can be empty"));
		QMessageBox::critical(this, title, text);
		return 0;
	}

	// create a vlnv instance and return the pointer to it
        VLNV* vlnv = new VLNV(type_, vendor, library, name, version);
	return vlnv;
}

void CreateVLNVDialog::setTypeBox(const VLNV* const vlnv) {

	QGroupBox* typeBox = new QGroupBox(tr("Select type of the vlnv"), this);
	QVBoxLayout* boxLayout = new QVBoxLayout(typeBox);

	// create the radio buttons and connect their clicked() signals to
	// event handlers

	QRadioButton* abstractorButton = new QRadioButton(tr("Abstractor"),
			typeBox);
	boxLayout->addWidget(abstractorButton, 0, Qt::AlignTop);
	connect(abstractorButton, SIGNAL(clicked()),
			this, SLOT(onAbstractorSelected()));

	QRadioButton* abstractionDefButton = new QRadioButton(
			tr("Abstraction definition"), typeBox);
	boxLayout->addWidget(abstractionDefButton, 0, Qt::AlignTop);
	connect(abstractionDefButton, SIGNAL(clicked()),
			this, SLOT(onAbstractionDefSelected()));

	QRadioButton* busDefButton = new QRadioButton(tr("Bus definition"),
			typeBox);
	boxLayout->addWidget(busDefButton, 0, Qt::AlignTop);
	connect(busDefButton, SIGNAL(clicked()),
			this, SLOT(onBusDefSelected()));

	QRadioButton* componentButton = new QRadioButton(tr("HW Component"), typeBox);
	boxLayout->addWidget(componentButton, 0, Qt::AlignTop);
	connect(componentButton, SIGNAL(clicked()),
			this, SLOT(onComponentSelected()));

	QRadioButton* designButton = new QRadioButton(tr("HW Design"), typeBox);
	boxLayout->addWidget(designButton, 0, Qt::AlignTop);
	connect(designButton, SIGNAL(clicked()),
			this, SLOT(onDesignSelected()));

	QRadioButton* designConfButton = new QRadioButton(
			tr("Design configuration"), typeBox);
	boxLayout->addWidget(designConfButton, 0, Qt::AlignTop);
	connect(designConfButton, SIGNAL(clicked()),
			this, SLOT(onDesignConfSelected()));

	QRadioButton* generatorButton = new QRadioButton(tr("Generator chain"),
			typeBox);
	boxLayout->addWidget(generatorButton, 0, Qt::AlignTop);
	connect(generatorButton, SIGNAL(clicked()),
			this, SLOT(onGeneratorChainSelected()));

	layout_->addWidget(typeBox, 1, 0, 1, 2, Qt::AlignTop | Qt::AlignLeft);

	// if the given pointer was empty then type is not preselected
	if (!vlnv) {
		return;
	}

	// use the type parameter to preselect a type if it has been defined
	switch (vlnv->getType()) {
	case VLNV::ABSTRACTOR: {
		abstractorButton->setChecked(true);
		return;
	}
	case VLNV::ABSTRACTIONDEFINITION: {
		abstractionDefButton->setChecked(true);
		return;
	}
	case VLNV::BUSDEFINITION: {
		busDefButton->setChecked(true);
		return;
	}
	case VLNV::COMPONENT: {
		componentButton->setChecked(true);
		return;
	}
	case VLNV::DESIGN: {
		designButton->setChecked(true);
		return;
	}
	case VLNV::DESIGNCONFIGURATION: {
		designConfButton->setChecked(true);
		return;
	}
	case VLNV::GENERATORCHAIN: {
		generatorButton->setChecked(true);
		return;
	}
	default: {
		return;
	}
	}
}

void CreateVLNVDialog::setVendor(const VLNV* const vlnv) {
	QString vendor(tr("Vendor:"));

	// if the vlnv pointer is valid use the previous vendor
	if (vlnv) {
		vendor = vlnv->getVendor();
	}

	// the vendor text label and line edit
	QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
	layout_->addWidget(vendorLabel, 2, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);

	vendorEdit_ = new QLineEdit(vendor, this);
	vendorEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
			QSizePolicy::Fixed));
	vendorLabel->setBuddy(vendorEdit_);
	layout_->addWidget(vendorEdit_, 2, 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
}

void CreateVLNVDialog::setLibrary(const VLNV* const vlnv) {
	QString library(tr("Library:"));

	// if the vlnv pointer is valid use the previous library
	if (vlnv) {
		library = vlnv->getLibrary();
	}

	// the library text label and line edit
	QLabel* libraryLabel = new QLabel(tr("Library:"), this);
	layout_->addWidget(libraryLabel, 3, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);

	libraryEdit_ = new QLineEdit(library, this);
	libraryEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
				QSizePolicy::Fixed));
	layout_->addWidget(libraryEdit_, 3, 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
	libraryLabel->setBuddy(libraryEdit_);
}

void CreateVLNVDialog::setName(const VLNV* const vlnv) {
	QString name(tr("Name:"));

	if (vlnv) {
		name = vlnv->getName();
	}

	// the name text label and edit
	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout_->addWidget(nameLabel, 4, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);

	nameEdit_ = new QLineEdit(name, this);
	nameEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
				QSizePolicy::Fixed));
	layout_->addWidget(nameEdit_, 4, 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
	nameLabel->setBuddy(nameEdit_);
}

void CreateVLNVDialog::setVersion(const VLNV* const vlnv) {
	QString version(tr("Version:"));

	if (vlnv) {
		version = vlnv->getVersion();
	}

	// the version text label and edit
	QLabel* versionLabel = new QLabel(tr("Version:"), this);
	layout_->addWidget(versionLabel, 5, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);

	versionEdit_ = new QLineEdit(version, this);
	versionEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
				QSizePolicy::Fixed));
	layout_->addWidget(versionEdit_, 5, 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
	versionLabel->setBuddy(versionEdit_);
}
