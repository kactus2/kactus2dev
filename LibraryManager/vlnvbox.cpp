/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "vlnvbox.h"

#include <QVBoxLayout>

VLNVBox::VLNVSearchSettings::VLNVSearchSettings(bool vendor, bool library,
		bool name, bool version): vendor_(vendor), library_(library),
		name_(name), version_(version) {
}

VLNVBox::VLNVSearchSettings::VLNVSearchSettings(): vendor_(true),
		library_(true),
		name_(true), version_(true) {
}

VLNVBox::VLNVBox(QWidget* parent): QGroupBox(tr("Search within:"), parent),
		searchVendors_(this), searchLibraries_(this), searchNames_(this),
		searchVersions_(this), settings_() {

	// set the texts for the check boxes
	searchVendors_.setText(tr("search vendors"));
	searchLibraries_.setText(tr("search libraries"));
	searchNames_.setText(tr("search names"));
	searchVersions_.setText(tr("search versions"));

	// set all checkBoxes to checked
	searchVendors_.setChecked(true);
	searchLibraries_.setChecked(true);
	searchNames_.setChecked(true);
	searchVersions_.setChecked(true);

	// connect all check boxes to inform if their state is changed
	connect(&searchVendors_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchLibraries_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchNames_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);
	connect(&searchVersions_, SIGNAL(stateChanged(int)),
			this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);

	// the layout to manage the layout of the group box
	QVBoxLayout* layout = new QVBoxLayout(this);

	// add widgets to layout and set stretch to the bottom so check boxes are
	// always aligned to the top of the group box
	layout->addWidget(&searchVendors_);
	layout->addWidget(&searchLibraries_);
	layout->addWidget(&searchNames_);
	layout->addWidget(&searchVersions_);
	layout->addStretch();
}

VLNVBox::~VLNVBox() {
}

void VLNVBox::onCheckStateChanged() {

	// get the current settings from the check boxes
	settings_.vendor_ = searchVendors_.isChecked();
	settings_.library_ = searchLibraries_.isChecked();
	settings_.name_ = searchNames_.isChecked();
	settings_.version_ = searchVersions_.isChecked();

	// inform of the change
	emit stateChanged(&settings_);
}
