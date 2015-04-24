/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraldetails.cpp
 */

#include "busifgeneraldetails.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/businterface.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

static const int MAX_BITS_IN_LAU = 2048;

BusIfGeneralDetails::BusIfGeneralDetails(QSharedPointer<BusInterface> busif, QWidget* parent ):
QGroupBox(tr("General") ,parent),
    busif_(busif),
    modeSelector_(this, busif),
    connRequired_(tr("Connection required"), this),
    bitsInLau_(this),
    endianness_(this),
    bitSteering_(this),
    bitSteeringEnabled_(tr("Enable"), this)
{
	Q_ASSERT(busif_);

	bitsInLau_.setRange(0, MAX_BITS_IN_LAU);
	bitsInLau_.setSingleStep(1);
	bitsInLau_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	endianness_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	endianness_.addItem(QString("little"));
	endianness_.addItem(QString("big"));

	bitSteering_.addItem(QString("on"));
	bitSteering_.addItem(QString("off"));

    connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
        this, SIGNAL(modeSelected(General::InterfaceMode)), Qt::UniqueConnection);
	connect(&connRequired_, SIGNAL(stateChanged(int)),
        this, SLOT(onConnectionRequiredChanged()), Qt::UniqueConnection);
	connect(&bitsInLau_, SIGNAL(valueChanged(int)),
		this, SLOT(onAddressableUnitChanged(int)), Qt::UniqueConnection);
	connect(&endianness_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onEndiannessChanged()), Qt::UniqueConnection);
	connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
	connect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

    setupLayout();
}

BusIfGeneralDetails::~BusIfGeneralDetails()
{
}

bool BusIfGeneralDetails::isValid() const
{
    if (modeSelector_.currentIndex() == -1)
    {
        return false;
    }

    return true;
}

void BusIfGeneralDetails::refresh()
{
    modeSelector_.setMode(busif_->getInterfaceMode());

	connRequired_.setChecked(busif_->getConnectionRequired());

	bitsInLau_.setValue(busif_->getBitsInLau());

	int endiannessIndex = endianness_.findText(General::endianness2Str(busif_->getEndianness()));
	endianness_.setCurrentIndex(endiannessIndex);

	// if bit steering is not specified
	if (busif_->getBitSteering() == General::BITSTEERING_UNSPECIFIED)
    {
		bitSteering_.setDisabled(true);
		bitSteering_.setCurrentIndex(-1);
		bitSteeringEnabled_.setChecked(false);
	}
	// if bit steering is specified then set it to correct value
	else
    {
		disconnect(&bitSteering_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));
		disconnect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),	this, SLOT(onBitSteeringChanged()));

		bitSteeringEnabled_.setChecked(true);
		bitSteering_.setEnabled(true);
		int bitSteeringIndex = bitSteering_.findText(General::bitSteering2Str(busif_->getBitSteering()));
		bitSteering_.setCurrentIndex(bitSteeringIndex);

		connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
		connect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),
			this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
	}
}

void BusIfGeneralDetails::onAddressableUnitChanged(int newValue)
{
	busif_->setBitsInLau(newValue);
	emit contentChanged();
}

void BusIfGeneralDetails::onEndiannessChanged()
{
	busif_->setEndianness(General::str2Endianness(endianness_.currentText(), General::LITTLE));
	emit contentChanged();
}

void BusIfGeneralDetails::onBitSteeringChanged()
{
	disconnect(&bitSteering_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));
	disconnect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),	this, SLOT(onBitSteeringChanged()));
	
	// if bit steering is checked then enable the combo box
	bitSteering_.setEnabled(bitSteeringEnabled_.isChecked());

	// if bit steering is enabled
	if (bitSteeringEnabled_.isChecked()) {
		busif_->setBitSteering(General::str2BitSteering(bitSteering_.currentText()));
	}
	// if it was disabled
	else {
		busif_->setBitSteering(General::BITSTEERING_UNSPECIFIED);
	}

	connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
	connect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),
		this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

	emit contentChanged();
}

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
    bitSteeringLayout->addWidget(&bitSteeringEnabled_, 0);
    bitSteeringLayout->addStretch();

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(modeLayout);
    topLayout->addLayout(lauLayout);
    topLayout->addLayout(endiannessLayout);
    topLayout->addLayout(bitSteeringLayout);
    topLayout->addWidget(&connRequired_);
    topLayout->addStretch();
}
