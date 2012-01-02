/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.cpp
 */

#include "busifgeneraltab.h"

#include <models/businterface.h>
#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

#include <QDebug>

BusIfGeneralTab::BusIfGeneralTab(LibraryInterface* libHandler, 
								 void* dataPointer, 
								 QWidget *parent): 
QWidget(parent), busif_(static_cast<BusInterface*>(dataPointer)),
nameGroup_(this, tr("Name and description")),
//attributes_(this),
busType_(VLNV::BUSDEFINITION, libHandler, this, this),
absType_(VLNV::ABSTRACTIONDEFINITION, libHandler, this, this),
details_(dataPointer, this),
libHandler_(libHandler) {

	Q_ASSERT_X(dataPointer, "BusIfGeneralTab constructor",
		"Null dataPointer given as parameter");
	Q_ASSERT_X(libHandler, "BusIfGeneralTab constructor",
		"Null LibraryInterface-pointer given as parameter");

	connect(&nameGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&busType_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	busType_.setTitle(tr("Bus definition"));
	absType_.setTitle(tr("Abstraction definition"));

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	QHBoxLayout* scrollLayout = new QHBoxLayout(this);
	scrollLayout->addWidget(scrollArea);

	// create the top widget and set it under the scroll area
	QWidget* topWidget = new QWidget(scrollArea);
	topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// create the layout of the general tab
	QVBoxLayout* layout = new QVBoxLayout(topWidget);
	layout->addWidget(&nameGroup_);
	layout->addWidget(&busType_);
	layout->addWidget(&absType_);
	layout->addWidget(&details_);
	layout->addStretch();

	scrollArea->setWidget(topWidget);

	restoreChanges();
}

BusIfGeneralTab::~BusIfGeneralTab() {
}

bool BusIfGeneralTab::isValid() const {
	
	if (!nameGroup_.isValid())
		return false;
	else if (!busType_.isValid())
		return false;
	
	// if specified bus type does not exist
	else if (!libHandler_->contains(busType_.getVLNV()))
		return false;

	// if abstraction type is not empty but is not valid
	else if (!absType_.isEmpty() && !absType_.isValid())
		return false;

	// if specified abstraction type does not exist
	else if (!absType_.isEmpty() && !libHandler_->contains(absType_.getVLNV()))
		return false;

	else if (!details_.isValid())
		return false;

	return true;
}

void BusIfGeneralTab::restoreChanges() {

	nameGroup_.setName(busif_->getName());
	nameGroup_.setDisplayName(busif_->getDisplayName());
	nameGroup_.setDescription(busif_->getDescription());

	//attributes_.setAttributes(busif_->getAttributes());

	if (busif_->getBusType().isValid())
		busType_.setVLNV(busif_->getBusType());
	
	if (busif_->getAbstractionType().isValid())
		absType_.setVLNV(busif_->getAbstractionType());

	details_.restoreChanges();
}

void BusIfGeneralTab::applyChanges() {

	if (!isValid())
		return;

	busif_->setName(nameGroup_.getName());
	busif_->setDisplayName(nameGroup_.getDisplayName());
	busif_->setDescription(nameGroup_.getDescription());

	//busif_->setAttributes(attributes_.getAttributes());

	busif_->setBusType(busType_.getVLNV());
	
	// abstraction type is optional element and is set only if its valid
	if (!absType_.isEmpty())
		busif_->setAbstractionType(absType_.getVLNV());

	details_.applyChanges();
}

VLNV BusIfGeneralTab::getBusType() const {
	return busType_.getVLNV();
}

VLNV BusIfGeneralTab::getAbsType() const {
	return absType_.getVLNV();
}
