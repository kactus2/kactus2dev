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

static const int MAX_BITS_IN_LAU = 2048;

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::BusIfGeneralDetails(QSharedPointer<BusInterface> busif, QWidget* parent ):
QGroupBox(tr("General") ,parent),
    busif_(busif),
    modeSelector_(this, busif),
    connRequired_(tr("Connection required"), this),
    bitsInLau_(this),
    endianness_(this),
    bitSteering_(this)
{
	Q_ASSERT(busif_);

	bitsInLau_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	endianness_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    endianness_.addItem(QString("little"));
	endianness_.addItem(QString("big"));
    	
	bitSteering_.addItem(QString("on"));
	bitSteering_.addItem(QString("off"));
    bitSteering_.addItem(QString(""));

    connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
        this, SIGNAL(modeSelected(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&connRequired_, SIGNAL(stateChanged(int)),
        this, SLOT(onConnectionRequiredChanged()), Qt::UniqueConnection);
	connect(&bitsInLau_, SIGNAL(editingFinished()),
		this, SLOT(onAddressableUnitChanged()), Qt::UniqueConnection);
	connect(&endianness_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onEndiannessChanged()), Qt::UniqueConnection);
	connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
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

	connRequired_.setChecked(busif_->getConnectionRequired() == "1");

	bitsInLau_.setText(busif_->getBitsInLau());

    endianness_.setCurrentIndex(busif_->getEndianness());

    disconnect(&bitSteering_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

    bitSteering_.setCurrentIndex(busif_->getBitSteering());

    connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onAddressableUnitChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onAddressableUnitChanged()
{
	busif_->setBitsInLau(bitsInLau_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onEndiannessChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onEndiannessChanged()
{
    if (endianness_.currentText() == QLatin1String("little"))
    {
        busif_->setEndianness(BusInterface::LITTLE_ENDIAN);
    }
    else
    {
        busif_->setEndianness(BusInterface::BIG_ENDIAN);
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onBitSteeringChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onBitSteeringChanged()
{
	disconnect(&bitSteering_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

	// if bit steering is enabled
	if (bitSteering_.currentText() == QLatin1String("on"))
    {
		busif_->setBitSteering(BusInterface::BITSTEERING_ON);
	}
	// if it was disabled
	else if (bitSteering_.currentText() == QLatin1String("off"))
	{
        busif_->setBitSteering(BusInterface::BITSTEERING_OFF);
	}
    else
    {
		busif_->setBitSteering(BusInterface::BITSTEERING_UNSPECIFIED);
	}

	connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onConnectionRequiredChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onConnectionRequiredChanged()
{
	busif_->setConnectionRequired(connRequired_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::setupLayout()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::setupLayout()
{
    QFormLayout* modeLayout = new QFormLayout();
    modeLayout->addRow(tr("Interface mode"), &modeSelector_);
    modeLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    QFormLayout* lauLayout = new QFormLayout();
    lauLayout->addRow(tr("Number of bits in least addressable unit"), &bitsInLau_);

    QFormLayout* endiannessLayout = new QFormLayout();
    endiannessLayout->addRow(tr("Endianness"), &endianness_);

    QLabel* bitSteeringLabel = new QLabel(tr("Bit steering"), this);
    QHBoxLayout* bitSteeringLayout = new QHBoxLayout();
    bitSteeringLayout->addWidget(bitSteeringLabel);
    bitSteeringLayout->addWidget(&bitSteering_);
    bitSteeringLayout->addStretch();

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(modeLayout);
    topLayout->addLayout(lauLayout);
    topLayout->addLayout(endiannessLayout);
    topLayout->addLayout(bitSteeringLayout);
    topLayout->addWidget(&connRequired_);
    topLayout->addStretch();
}
