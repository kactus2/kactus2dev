/* 
 *
 *  Created on: 13.1.2011
 *      Author: kamppia
 */

#include "vlnvgenerator.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

VLNVGenerator::VLNVGenerator(QWidget* parent): QGroupBox(parent),
vendorEdit_(0), libraryEdit_(0), nameEdit_(0), versionEdit_(0) {

	// set title for the group box
	setTitle(tr("VLNV"));

	// set up the layout for the widget
	QGridLayout* layout = new QGridLayout(this);

	QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
	layout->addWidget(vendorLabel, 0, 0, 1, 1);

	vendorEdit_ = new QLineEdit(this);
	layout->addWidget(vendorEdit_, 0, 1, 1, 1);

	QLabel* libraryLabel = new QLabel(tr("Library:"), this);
	layout->addWidget(libraryLabel, 1, 0, 1, 1);

	libraryEdit_ = new QLineEdit(this);
	layout->addWidget(libraryEdit_, 1, 1, 1, 1);

	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	layout->addWidget(nameLabel, 2, 0, 1, 1);

	nameEdit_ = new QLineEdit(this);
	layout->addWidget(nameEdit_, 2, 1, 1, 1);

	QLabel* versionLabel = new QLabel(tr("Version:"), this);
	layout->addWidget(versionLabel, 3, 0, 1, 1);

	versionEdit_ = new QLineEdit(this);
	layout->addWidget(versionEdit_, 3, 1, 1, 1);

	// connect the signals informing of changes in items
	connect(vendorEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(libraryEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(nameEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(versionEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

VLNVGenerator::~VLNVGenerator() {
}

VLNV* VLNVGenerator::getVLNV(VLNV::IPXactType type) {

	if (!isValid()) {
		return 0;
	}

	return new VLNV(type,
			vendorEdit_->text(),
			libraryEdit_->text(),
			nameEdit_->text(),
			versionEdit_->text());
}

bool VLNVGenerator::isValid() {

	// all elements must contains text
	if (vendorEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Vendor-field in "
				"VLNV can not be empty"));
		return false;
	}
	else if (libraryEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Library-field in "
				"VLNV can not be empty"));
		return false;
	}

	else if (nameEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Name-field in "
				"VLNV can not be empty"));
		return false;
	}

	else if (versionEdit_->text().isEmpty()) {
		QMessageBox::critical(this, tr("Invalid field"), tr("Version-field in "
				"VLNV can not be empty"));
		return false;
	}
	// everything was fine
	else {
		return true;
	}
}

void VLNVGenerator::setVLNV(const VLNV* vlnv) {

	if (!vlnv) {
		return;
	}

	vendorEdit_->setText(vlnv->getVendor());
	libraryEdit_->setText(vlnv->getLibrary());
	nameEdit_->setText(vlnv->getName());
	versionEdit_->setText(vlnv->getVersion());
}

void VLNVGenerator::setVLNV( const VLNV& vlnv ) {
	setVLNV(&vlnv);
}
