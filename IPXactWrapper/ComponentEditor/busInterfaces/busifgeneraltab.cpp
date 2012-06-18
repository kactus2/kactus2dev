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

BusIfGeneralTab::BusIfGeneralTab( LibraryInterface* libHandler,
								 QSharedPointer<BusInterface> busif, 
								 QWidget* parent ):
QWidget(parent),
busif_(busif),
nameEditor_(busif->getNameGroup(), this, tr("Name and description")),
busType_(VLNV::BUSDEFINITION, libHandler, this, this),
absType_(VLNV::ABSTRACTIONDEFINITION, libHandler, this, this),
details_(busif, this),
parameters_(busif->getParameters(), this),
libHandler_(libHandler) {

	Q_ASSERT_X(libHandler, "BusIfGeneralTab constructor",
		"Null LibraryInterface-pointer given as parameter");
	Q_ASSERT(busif_);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameEditor_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&busType_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&busType_, SIGNAL(setAbsDef(const VLNV&)),
		&absType_, SLOT(setVLNV(const VLNV&)), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(setBusDef(const VLNV&)),
		&busType_, SLOT(setVLNV(const VLNV&)), Qt::UniqueConnection);

	busType_.setTitle(tr("Bus definition"));
	absType_.setTitle(tr("Abstraction definition"));

	absType_.setMandatory(false);

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
	layout->addWidget(&nameEditor_);
	layout->addWidget(&busType_);
	layout->addWidget(&absType_);
	layout->addWidget(&details_);
	layout->addWidget(&parameters_);
	layout->addStretch();

	scrollArea->setWidget(topWidget);

	refresh();
}

BusIfGeneralTab::~BusIfGeneralTab() {
}

bool BusIfGeneralTab::isValid() const {
	
	if (!nameEditor_.isValid())
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

	else if (!parameters_.isValid()) {
		return false;
	}

	return true;
}

void BusIfGeneralTab::refresh() {

	busType_.setVLNV(busif_->getBusType());
	absType_.setVLNV(busif_->getAbstractionType());

	details_.restoreChanges();

	parameters_.refresh();
}

void BusIfGeneralTab::applyChanges() {

	busif_->setBusType(busType_.getVLNV());
	busif_->setAbstractionType(absType_.getVLNV());

	details_.applyChanges();
}

VLNV BusIfGeneralTab::getBusType() const {
	return busType_.getVLNV();
}

VLNV BusIfGeneralTab::getAbsType() const {
	return absType_.getVLNV();
}
