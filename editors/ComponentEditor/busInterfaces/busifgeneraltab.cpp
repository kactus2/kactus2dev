/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.cpp
 */

#include "busifgeneraltab.h"

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/vlnv.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QApplication>

BusIfGeneralTab::BusIfGeneralTab(LibraryInterface* libHandler,
                                  QSharedPointer<BusInterface> busif,
                                  QSharedPointer<Component> component,
                                  QSharedPointer<ParameterFinder> parameterFinder,
                                  QSharedPointer<ExpressionFormatter> expressionFormatter,
                                  QWidget* parent,
                                  QWidget* parentWnd):
QWidget(parent),
busif_(busif),
nameEditor_(busif->getNameGroup(), this, tr("Name and description")),
busType_(VLNV::BUSDEFINITION, libHandler, parentWnd, this),
absType_(VLNV::ABSTRACTIONDEFINITION, libHandler, parentWnd, this),
modeStack_(busif, component, parameterFinder, libHandler, this),
details_(busif, this),
parameters_(busif->getParameters(), component, parameterFinder, expressionFormatter, this),
libHandler_(libHandler) {

	Q_ASSERT_X(libHandler, "BusIfGeneralTab constructor",
		"Null LibraryInterface-pointer given as parameter");
	Q_ASSERT(busif_);

    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    connect(&modeStack_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&modeStack_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&busType_, SIGNAL(vlnvEdited()),
		this, SLOT(onBusTypeChanged()), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(vlnvEdited()),
		this, SLOT(onAbsTypeChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(modeSelected(General::InterfaceMode)),
		this, SLOT(onModeChanged(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&modeStack_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(contentChanged()),
        &modeStack_, SIGNAL(busIfParametersChanged()), Qt::UniqueConnection);

	connect(&busType_, SIGNAL(setAbsDef(const VLNV&)),
		this, SLOT(onSetAbsType(const VLNV&)), Qt::UniqueConnection);
	connect(&absType_, SIGNAL(setBusDef(const VLNV&)),
		this, SLOT(onSetBusType(const VLNV&)), Qt::UniqueConnection);

	busType_.setTitle(tr("Bus definition"));
	absType_.setTitle(tr("Abstraction definition"));

	absType_.setMandatory(false);

    setupLayout();
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

    return valid;
}

void BusIfGeneralTab::refresh()
{
	nameEditor_.refresh();
	busType_.setVLNV(busif_->getBusType());
	absType_.setVLNV(busif_->getAbstractionType());
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

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setupLayout()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setupLayout()
{
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

    QGridLayout* topLayout = new QGridLayout(topWidget);

    topLayout->addWidget(&nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(&modeStack_, 0, 1, 2, 1);

    topLayout->addWidget(&details_, 1, 0, 1, 1);

    QHBoxLayout* vlnvLayout = new QHBoxLayout();
    vlnvLayout->addWidget(&busType_);
    vlnvLayout->addWidget(&absType_);
    topLayout->addLayout(vlnvLayout, 2, 0, 1, 2);
    topLayout->setRowStretch(2, 10);

    topLayout->addWidget(&parameters_, 3, 0, 1, 2);

    topLayout->setRowStretch(0, 5);
    topLayout->setRowStretch(1, 5);
    topLayout->setRowStretch(3, 80);

    scrollArea->setWidget(topWidget);
}
