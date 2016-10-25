//-----------------------------------------------------------------------------
// File: busifgeneraldetails.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Editor to edit the bus interface details.
//-----------------------------------------------------------------------------

#include "busifgeneraldetails.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::BusIfGeneralDetails(QSharedPointer<BusInterface> busif, QWidget* parent ):
QGroupBox(tr("General") ,parent),
    busif_(busif),
    modeSelector_(this, busif),
    connectionRequired_(this),
    bitsInLauEditor_(this),
    endiannessSelector_(this),
    bitSteeringSelector_(this)
{
	Q_ASSERT(busif_);

    endiannessSelector_.addItem(QString("little"));
	endiannessSelector_.addItem(QString("big"));
    endiannessSelector_.addItem(QString(""));

	bitSteeringSelector_.addItem(QString("on"));
	bitSteeringSelector_.addItem(QString("off"));
    bitSteeringSelector_.addItem(QString(""));

    connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
        this, SIGNAL(modeSelected(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&connectionRequired_, SIGNAL(stateChanged(int)),
        this, SLOT(onConnectionRequiredChanged()), Qt::UniqueConnection);
	connect(&bitsInLauEditor_, SIGNAL(editingFinished()),
		this, SLOT(onAddressableUnitChanged()), Qt::UniqueConnection);
	connect(&endiannessSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onEndiannessChanged()), Qt::UniqueConnection);
	connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::~BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::~BusIfGeneralDetails()
{
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::isValid()
//-----------------------------------------------------------------------------
bool BusIfGeneralDetails::isValid() const
{
    return modeSelector_.currentIndex() != -1;
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::refresh()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::refresh()
{
    modeSelector_.setMode(busif_->getInterfaceMode());

	connectionRequired_.setChecked(busif_->getConnectionRequired() == "1");

	bitsInLauEditor_.setText(busif_->getBitsInLau());

    endiannessSelector_.setCurrentIndex(busif_->getEndianness());

    disconnect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

    bitSteeringSelector_.setCurrentIndex(busif_->getBitSteering());

    connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onAddressableUnitChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onAddressableUnitChanged()
{
	busif_->setBitsInLau(bitsInLauEditor_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onEndiannessChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onEndiannessChanged()
{
    if (endiannessSelector_.currentText() == QLatin1String("little"))
    {
        busif_->setEndianness(BusInterface::LITTLE);
    }
    else if (endiannessSelector_.currentText() == QLatin1String("big"))
    {
        busif_->setEndianness(BusInterface::BIG);
    }
    else
    {
        busif_->setEndianness(BusInterface::ENDIANNESS_UNSPECIFIED);
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onBitSteeringChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onBitSteeringChanged()
{
	disconnect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

	// if bit steering is enabled
	if (bitSteeringSelector_.currentText() == QLatin1String("on"))
    {
		busif_->setBitSteering(BusInterface::BITSTEERING_ON);
	}
	// if it was disabled
	else if (bitSteeringSelector_.currentText() == QLatin1String("off"))
	{
        busif_->setBitSteering(BusInterface::BITSTEERING_OFF);
	}
    else
    {
		busif_->setBitSteering(BusInterface::BITSTEERING_UNSPECIFIED);
	}

	connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onConnectionRequiredChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onConnectionRequiredChanged()
{
	busif_->setConnectionRequired(connectionRequired_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::setupLayout()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::setupLayout()
{
    QFormLayout* modeLayout = new QFormLayout(this);
    modeLayout->addRow(tr("Interface mode:"), &modeSelector_);
    modeLayout->addRow(tr("Addressable unit size:"), &bitsInLauEditor_);
    modeLayout->addRow(tr("Endianness:"), &endiannessSelector_);
    modeLayout->addRow(tr("Bit steering:"), &bitSteeringSelector_);
    modeLayout->addRow(tr("Connection required:"), &connectionRequired_);
}
