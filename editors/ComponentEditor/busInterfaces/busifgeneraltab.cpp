/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.cpp
 */

#include "busifgeneraltab.h"

#include <models/businterface.h>
#include <library/LibraryManager/vlnv.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QApplication>

BusIfGeneralTab::BusIfGeneralTab( LibraryInterface* libHandler,
								 QSharedPointer<BusInterface> busif, 
								 QSharedPointer<Component> component,
								 QWidget* parent,
                                 QWidget* parentWnd):
QWidget(parent),
busif_(busif),
nameEditor_(busif->getNameGroup(), this, tr("Name and description")),
busType_(VLNV::BUSDEFINITION, libHandler, parentWnd, this),
absType_(VLNV::ABSTRACTIONDEFINITION, libHandler, parentWnd, this),
modeSelector_(this, busif),
modeStack_(busif, component, libHandler, this),
details_(busif, this),
parameters_(busif->getParameters(), this),
libHandler_(libHandler) {

	Q_ASSERT_X(libHandler, "BusIfGeneralTab constructor",
		"Null LibraryInterface-pointer given as parameter");
	Q_ASSERT(busif_);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&busType_, SIGNAL(vlnvEdited()),
		this, SLOT(onBusTypeChanged()), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(vlnvEdited()),
		this, SLOT(onAbsTypeChanged()), Qt::UniqueConnection);
	connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
		this, SLOT(onModeChanged(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&modeStack_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&busType_, SIGNAL(setAbsDef(const VLNV&)),
		this, SLOT(onSetAbsType(const VLNV&)), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(setBusDef(const VLNV&)),
		this, SLOT(onSetBusType(const VLNV&)), Qt::UniqueConnection);

	busType_.setTitle(tr("Bus definition"));
	absType_.setTitle(tr("Abstraction definition"));

	absType_.setMandatory(false);

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);

	QHBoxLayout* scrollLayout = new QHBoxLayout(this);
	scrollLayout->addWidget(scrollArea);
	scrollLayout->setContentsMargins(0, 0, 0, 0);

	// create the top widget and set it under the scroll area
	QWidget* topWidget = new QWidget(scrollArea);
	topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QHBoxLayout* vlnvLayout = new QHBoxLayout();
	vlnvLayout->addWidget(&busType_);
	vlnvLayout->addWidget(&absType_);

	QFormLayout* modeLayout = new QFormLayout();
	modeLayout->addRow(tr("Interface mode"), &modeSelector_);

	// create the layout of the general tab
	QVBoxLayout* layout = new QVBoxLayout(topWidget);
	layout->addWidget(&nameEditor_);
	layout->addLayout(vlnvLayout);
	layout->addLayout(modeLayout);
	layout->addWidget(&modeStack_);
	layout->addWidget(&details_);
	layout->addWidget(&parameters_);
	layout->addStretch();

	scrollArea->setWidget(topWidget);
}

BusIfGeneralTab::~BusIfGeneralTab() {
}

bool BusIfGeneralTab::isValid() const {
	
	if (!nameEditor_.isValid()) {
		return false;
	}
	else if (!busType_.isValid()) {
		return false;
	}
	
	// if specified bus type does not exist
	else if (!libHandler_->contains(busType_.getVLNV())) {
		return false;
	}

	// if abstraction type is not empty but is not valid
	else if (!absType_.isEmpty() && !absType_.isValid()) {
		return false;
	}

	// if specified abstraction type does not exist
	else if (!absType_.isEmpty() && !libHandler_->contains(absType_.getVLNV())) {
        return false;
    }

    else if (!details_.isValid()) {
        return false;
    }

    else if (!parameters_.isValid()) {
        return false;
    }

    else if (modeSelector_.currentIndex() == -1)
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::isValid()
//-----------------------------------------------------------------------------
bool BusIfGeneralTab::isValid(QStringList& errorList) const
{
    bool valid = true;
    if (!nameEditor_.isValid()) {
        errorList.append(tr("No name defined for bus interface."));
        valid = false;
    }
    if (!busType_.isValid()) {
        errorList.append(tr("No valid VLNV set for bus definition."));
        valid = false;
    }

    // if specified bus type does not exist
    else if (!libHandler_->contains(busType_.getVLNV())) {
        errorList.append(tr("No item found in library with VLNV %1.").arg(busType_.getVLNV().toString()));
        valid = false;
    }

    // if abstraction type is not empty but is not valid
    if (!absType_.isEmpty() && !absType_.isValid()) {
        errorList.append(tr("No valid VLNV set for abstraction definition."));
        valid = false;
    }

    // if specified abstraction type does not exist
    else if (!absType_.isEmpty() && !libHandler_->contains(absType_.getVLNV())) {
        errorList.append(tr("No item found in library with VLNV %1.").arg(absType_.getVLNV().toString()));
        valid = false;
    }

    if (!details_.isValid()) {
        //TODO: check validity in details. Always returns true.
        errorList.append(tr("Not all details are valid."));
        valid = false;
    }

    QString thisIdentifier(tr("bus interface %1").arg(busif_->getName()));

    if (!parameters_.isValid(errorList, thisIdentifier)) {        
        valid = false;
    }

    if (modeSelector_.currentIndex() == -1)
    {
        errorList.append(tr("No mode selected for %1.").arg(thisIdentifier));
        valid = false;
    }

    return valid;
}

void BusIfGeneralTab::refresh() {
	nameEditor_.refresh();
	busType_.setVLNV(busif_->getBusType());
	absType_.setVLNV(busif_->getAbstractionType());
	modeSelector_.setMode(busif_->getInterfaceMode());
	modeStack_.refresh();
	details_.refresh();
	parameters_.refresh();
}

VLNV BusIfGeneralTab::getBusType() const {
	return busType_.getVLNV();
}

VLNV BusIfGeneralTab::getAbsType() const {
	return absType_.getVLNV();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setAbsTypeMandatory()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setAbsTypeMandatory(bool isMandatory)
{
	absType_.setMandatory(isMandatory);
}

void BusIfGeneralTab::onBusTypeChanged() {
	busif_->setBusType(busType_.getVLNV());

    // If only one possible absDef, set it automatically.
    if (busType_.getVLNV().isValid())
    {
        QList<VLNV> absDefVLNVs;
        if (libHandler_->getChildren(absDefVLNVs, busType_.getVLNV()) > 0) 
        {
            absType_.setVLNV(absDefVLNVs.first());
            onAbsTypeChanged();
            return;
        }
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setBusTypesLocked()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setBusTypesLock(bool locked)
{
    busType_.setEnabled(!locked);
    absType_.setEnabled(!locked);
}

void BusIfGeneralTab::onAbsTypeChanged() {
	busif_->setAbstractionType(absType_.getVLNV());
	emit contentChanged();
}

void BusIfGeneralTab::onModeChanged( General::InterfaceMode mode ) {
	modeStack_.setMode(mode);
	emit contentChanged();
}

void BusIfGeneralTab::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/businterfacegeneral.html");
}

void BusIfGeneralTab::onSetBusType( const VLNV& busDefVLNV ) {
	busif_->setBusType(busDefVLNV);
	busType_.setVLNV(busDefVLNV);
	emit contentChanged();
}

void BusIfGeneralTab::onSetAbsType( const VLNV& absDefVLNV ) {
	busif_->setAbstractionType(absDefVLNV);
	absType_.setVLNV(absDefVLNV);
	emit contentChanged();
}