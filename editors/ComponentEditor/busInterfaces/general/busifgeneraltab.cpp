//-----------------------------------------------------------------------------
// File: busifgeneraltab.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------

#include "busifgeneraltab.h"

#include <library/LibraryInterface.h>

#include <editors/ComponentEditor/busInterfaces/AbstractionTypesEditor.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QHBoxLayout>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::BusIfGeneralTab()
//-----------------------------------------------------------------------------
BusIfGeneralTab::BusIfGeneralTab(LibraryInterface* libHandler, QSharedPointer<BusInterface> busif,
    QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent, QWidget* parentWnd):
QWidget(parent),
busif_(busif),
nameEditor_(busif, this, tr("Name and description")),
busType_(VLNV::BUSDEFINITION, libHandler, parentWnd, this),
abstractionEditor_(new AbstractionTypesEditor(component, libHandler, parentWnd, this)),
modeStack_(busif, component, parameterFinder, libHandler, expressionParser, this),
details_(busif, this),
parameters_(busif->getParameters(), component->getChoices(), parameterFinder, expressionFormatter, this),
libHandler_(libHandler)
{
	Q_ASSERT_X(libHandler, "BusIfGeneralTab constructor", "Null LibraryInterface-pointer given as parameter");
	Q_ASSERT(busif_);

    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> >)),
        this ,SIGNAL(recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> >)),
        Qt::UniqueConnection);

    connect(&modeStack_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&modeStack_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&busType_, SIGNAL(vlnvEdited()), this, SLOT(onBusTypeChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(modeSelected(General::InterfaceMode)),
		this, SLOT(onModeChanged(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&modeStack_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&details_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&busType_, SIGNAL(setAbsDef(const VLNV&)), this, SLOT(onSetAbsType(const VLNV&)), Qt::UniqueConnection);
    connect(abstractionEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	busType_.setTitle(tr("Bus definition"));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::~BusIfGeneralTab()
//-----------------------------------------------------------------------------
BusIfGeneralTab::~BusIfGeneralTab()
{

}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::refresh()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::refresh()
{
	nameEditor_.refresh();
	busType_.setVLNV(busif_->getBusType());

    VLNV abstractionVLNV;
    if (!busif_->getAbstractionTypes()->isEmpty() && busif_->getAbstractionTypes()->first()->getAbstractionRef())
    {
        abstractionVLNV = *busif_->getAbstractionTypes()->first()->getAbstractionRef();
    }

    abstractionEditor_->setBusForModel(busif_);

	modeStack_.refresh();
	details_.refresh();
	parameters_.refresh();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::isValid()
//-----------------------------------------------------------------------------
VLNV BusIfGeneralTab::getBusType() const
{
	return busType_.getVLNV();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::getAbsType()
//-----------------------------------------------------------------------------
VLNV BusIfGeneralTab::getAbsType() const
{
    if (abstractionEditor_->getFirstAbstraction())
    {
        return *abstractionEditor_->getFirstAbstraction().data();
    }
    else
    {
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setAbsTypeMandatory()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setAbsTypeMandatory(bool isMandatory)
{
    abstractionEditor_->setProperty("mandatoryField", isMandatory);
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::onBusTypeChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::onBusTypeChanged()
{
	busif_->setBusType(busType_.getVLNV());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setBusTypesLocked()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setBusTypesLock(bool locked)
{
    busType_.setEnabled(!locked);
    abstractionEditor_->setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::onModeChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::onModeChanged(General::InterfaceMode mode)
{
	modeStack_.setMode(mode);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::showEvent()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/businterfacegeneral.html");
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::onSetBusType()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::onSetBusType(VLNV const& busDefVLNV)
{
	busif_->setBusType(busDefVLNV);
	busType_.setVLNV(busDefVLNV);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::onSetAbsType()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::onSetAbsType(VLNV const& absDefVLNV)
{
    if (busif_->getAbstractionTypes()->isEmpty())
    {
        abstractionEditor_->addNewAbstraction(absDefVLNV);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralTab::setupLayout()
//-----------------------------------------------------------------------------
void BusIfGeneralTab::setupLayout()
{
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
    topLayout->addWidget(&busType_, 0, 1, 1, 1);
    topLayout->addWidget(&details_, 1, 0, 1, 1);
    topLayout->addWidget(abstractionEditor_, 1, 1, 2, 1);
    topLayout->addWidget(&modeStack_, 2, 0, 1, 1);
    topLayout->addWidget(&parameters_, 3, 0, 1, 2);

    topLayout->setRowStretch(0, 5);
    topLayout->setRowStretch(1, 5);
    topLayout->setRowStretch(2, 5);
    topLayout->setRowStretch(3, 10);

    scrollArea->setWidget(topWidget);
}
