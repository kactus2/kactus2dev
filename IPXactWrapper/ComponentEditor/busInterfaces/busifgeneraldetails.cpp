/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraldetails.cpp
 */

#include "busifgeneraldetails.h"

#include <models/generaldeclarations.h>
#include <models/businterface.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

static const int MAX_BITS_IN_LAU = 2048;

BusIfGeneralDetails::BusIfGeneralDetails( QSharedPointer<BusInterface> busif, 
										 QWidget* parent ):
QGroupBox(tr("General") ,parent),
busif_(busif),
connRequired_(tr("Connection required"), this),
bitsInLau_(this),
endianness_(this),
bitSteering_(this),
bitSteeringEnabled_(tr("Enable"), this) {

	Q_ASSERT(busif_);

	bitsInLau_.setRange(0, MAX_BITS_IN_LAU);
	bitsInLau_.setSingleStep(1);
	bitsInLau_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	endianness_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	endianness_.addItem(QString("little"));
	endianness_.addItem(QString("big"));

	bitSteering_.addItem(QString("on"));
	bitSteering_.addItem(QString("off"));

	QLabel* bitSteeringLabel = new QLabel(tr("Bit steering"), this);
	QHBoxLayout* bitSteeringLayout = new QHBoxLayout();
	bitSteeringLayout->addWidget(bitSteeringLabel);
	bitSteeringLayout->addWidget(&bitSteering_);
	bitSteeringLayout->addWidget(&bitSteeringEnabled_, 0);
	bitSteeringLayout->addStretch();

	QFormLayout* lauLayout = new QFormLayout();
	lauLayout->addRow(tr("Number of bits in least addressable unit"), &bitsInLau_);

	QFormLayout* endiannessLayout = new QFormLayout();
	endiannessLayout->addRow(tr("Endianness"), &endianness_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(lauLayout);
	topLayout->addLayout(endiannessLayout);
	topLayout->addLayout(bitSteeringLayout);
	topLayout->addWidget(&connRequired_);
	topLayout->addStretch();

	connect(&connRequired_, SIGNAL(stateChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&bitsInLau_, SIGNAL(valueChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&endianness_, SIGNAL(currentIndexChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&bitSteering_, SIGNAL(currentIndexChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&bitSteeringEnabled_, SIGNAL(toggled(bool)),
		this, SLOT(onBitSteeringState(bool)), Qt::UniqueConnection);

	restoreChanges();
}

BusIfGeneralDetails::~BusIfGeneralDetails() {
}

bool BusIfGeneralDetails::isValid() const {
	return true;
}

void BusIfGeneralDetails::restoreChanges() {

	connRequired_.setChecked(busif_->getConnectionRequired());
	bitsInLau_.setValue(busif_->getBitsInLau());
	int endiannessIndex = endianness_.findText(
		General::endianness2Str(busif_->getEndianness()));
	endianness_.setCurrentIndex(endiannessIndex);

	// if bit steering is not specified
	if (busif_->getBitSteering() == General::BITSTEERING_UNSPECIFIED) {
		bitSteering_.setDisabled(true);
		bitSteering_.setCurrentIndex(-1);
		bitSteeringEnabled_.setChecked(false);
	}
	// if bitsteering is specified then set it to correct value
	else {
		bitSteeringEnabled_.setChecked(true);
		bitSteering_.setEnabled(true);
		int bitSteeringIndex = bitSteering_.findText(
			General::bitSteering2Str(busif_->getBitSteering()));
		bitSteering_.setCurrentIndex(bitSteeringIndex);
	}

}

void BusIfGeneralDetails::applyChanges() {

	busif_->setConnectionRequired(connRequired_.isChecked());
	busif_->setBitsInLau(bitsInLau_.value());
	busif_->setEndianness(General::str2Endianness(endianness_.currentText(),
		General::LITTLE));

	// if bit steering is enabled
	if (bitSteeringEnabled_.isChecked()) {
		busif_->setBitSteering(General::str2BitSteering(bitSteering_.currentText()));
	}
	// if it was disabled
	else {
		busif_->setBitSteering(General::BITSTEERING_UNSPECIFIED);
	}
}

void BusIfGeneralDetails::onBitSteeringState( bool checked ) {
	bitSteering_.setEnabled(checked);
	emit contentChanged();
}
