/* 
 *
 *  Created on: 13.1.2011
 *      Author: Antti Kamppi
 */

#include "libraryrefgenerator.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

LibraryRefGenerator::LibraryRefGenerator(const QString title,
		QWidget* parent, bool enableCheckBox): QGroupBox(parent),
		vendorEdit_(0), libraryEdit_(0), nameEdit_(0), versionEdit_(0),
		enableCheckBox_(0) {

	setTitle(title);

	// set up the layout for the widget
	QGridLayout* layout = new QGridLayout(this);

	QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
	layout->addWidget(vendorLabel, 0, 0, 1, 1);

	vendorEdit_ = new QLineEdit(this);
	vendorEdit_->setDisabled(true);
	layout->addWidget(vendorEdit_, 0, 1, 1, 1);

	QLabel* libraryLabel = new QLabel(tr("Library:"), this);
	layout->addWidget(libraryLabel, 1, 0, 1, 1);

	libraryEdit_ = new QLineEdit(this);
	libraryEdit_->setDisabled(true);
	layout->addWidget(libraryEdit_, 1, 1, 1, 1);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 2, 0, 1, 1);

	nameEdit_ = new QLineEdit(this);
	nameEdit_->setDisabled(true);
	layout->addWidget(nameEdit_, 2, 1, 1, 1);

	QLabel* versionLabel = new QLabel(tr("Version:"), this);
	layout->addWidget(versionLabel, 3, 0, 1, 1);

	versionEdit_ = new QLineEdit(this);
	versionEdit_->setDisabled(true);
	layout->addWidget(versionEdit_, 3, 1, 1, 1);

	// TODO the browsing must be added so user can select from existing busDefs
	QPushButton* browseButton = new QPushButton(tr("browse..."), this);
	layout->addWidget(browseButton, 4, 0, 1, 1);

	// if enableCheckBox was wanted to be constructed
	if (enableCheckBox) {
		enableCheckBox_ = new QCheckBox(tr("enabled"), this);
		layout->addWidget(enableCheckBox_, 4, 1, 1, 1);

		connect(enableCheckBox_, SIGNAL(toggled(bool)),
			this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	}

	connect(enableCheckBox_, SIGNAL(toggled(bool)),
			vendorEdit_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
	connect(enableCheckBox_, SIGNAL(toggled(bool)),
			libraryEdit_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
	connect(enableCheckBox_, SIGNAL(toggled(bool)),
			nameEdit_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
	connect(enableCheckBox_, SIGNAL(toggled(bool)),
			versionEdit_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

	// connect the signals used to inform of changes in the widget
	connect(vendorEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(libraryEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(nameEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(versionEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

LibraryRefGenerator::~LibraryRefGenerator() {

}

bool LibraryRefGenerator::isEnabled() const {

	// if enableCheckBox was constructed
	if (enableCheckBox_) {
		return enableCheckBox_->isChecked();
	}
	return true;
}

VLNV* LibraryRefGenerator::getVLNV(VLNV::IPXactType type) {

	// if the text-fields are not valid
	if (!isValid()) {
		return 0;
	}
	// if the generator is disabled
	else if (!isEnabled()) {
		return 0;
	}

	return new VLNV(type,
			vendorEdit_->text(),
			libraryEdit_->text(),
			nameEdit_->text(),
			versionEdit_->text());
}

bool LibraryRefGenerator::isValid() {
	// if this is not enabled then the contents of the text-fields does not
	// matter
	if (!isEnabled()) {
		return true;
	}

	// all elements must contains text
	else if (vendorEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Vendor-field in "
				"%1 element can not be empty").arg(title()));
		return false;
	}
	else if (libraryEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Library-field in "
				"%1 element can not be empty").arg(title()));
		return false;
	}
	else if (nameEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Name-field in "
				"%1 element can not be empty").arg(title()));
		return false;
	}
	else if (versionEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Version-field in "
				"%1 element can not be empty").arg(title()));
		return false;
	}
	else {
		return true;
	}
}

void LibraryRefGenerator::setVLNV(const VLNV* vlnv) {

	if (!vlnv) {
		return;
	}

	vendorEdit_->setText(vlnv->getVendor());
	libraryEdit_->setText(vlnv->getLibrary());
	nameEdit_->setText(vlnv->getName());
	versionEdit_->setText(vlnv->getVersion());
	setEnabled(true);

	// if enableCheckBox was constructed
	if (enableCheckBox_) {
		enableCheckBox_->setChecked(true);
	}
}

void LibraryRefGenerator::setVLNV( const VLNV& vlnv ) {
	setVLNV(&vlnv);
}
