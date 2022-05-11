//-----------------------------------------------------------------------------
// File: busifgeneraldetails.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Editor to edit the bus interface details.
//-----------------------------------------------------------------------------

#include "busifgeneraldetails.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <KactusAPI/include/BusInterfaceInterface.h>

#include <common/KactusColors.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::BusIfGeneralDetails(BusInterfaceInterface* busInterface, std::string const& busName,
    QWidget* parent):
QGroupBox(tr("General") ,parent),
busInterface_(busInterface),
busName_(busName),
modeSelector_(this, busInterface_->getMode(busName)),
connectionRequired_(this),
bitsInLauEditor_(this),
endiannessSelector_(this),
bitSteeringSelector_(this)
{
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
    connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
        this, SLOT(changeBitSteeringColour()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::~BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::~BusIfGeneralDetails()
{
}

//-----------------------------------------------------------------------------
// Function: busifgeneraldetails::changeBitSteeringColour()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::changeBitSteeringColour()
{
    QPalette comboPalette = bitSteeringSelector_.palette();

    QColor colour = KactusColors::REGULAR_TEXT;

    General::InterfaceMode interfaceMode = busInterface_->getMode(busName_);
    if (!bitSteeringSelector_.currentText().isEmpty() &&
        (interfaceMode == General::MIRROREDMASTER || interfaceMode == General::SYSTEM ||
        interfaceMode == General::MIRROREDSYSTEM))
    {
        colour = KactusColors::ERROR;
    }

    comboPalette.setColor(QPalette::Text, colour);
    bitSteeringSelector_.setPalette(comboPalette);
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
    modeSelector_.setMode(busInterface_->getMode(busName_));

    QString connectionRequirement = QString::fromStdString(busInterface_->connectionIsRequired(busName_));
    connectionRequired_.setChecked(connectionRequirement == QLatin1String("true"));

    bitsInLauEditor_.setText(QString::fromStdString(busInterface_->getBitsInLau(busName_)));

    endiannessSelector_.setCurrentIndex(busInterface_->getEndianness(busName_));

    disconnect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

    bitSteeringSelector_.setCurrentIndex(busInterface_->getBitSteering(busName_));

    changeBitSteeringColour();

    connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onAddressableUnitChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onAddressableUnitChanged()
{
    busInterface_->setBitsInLau(busName_, bitsInLauEditor_.text().toStdString());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onEndiannessChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onEndiannessChanged()
{
    busInterface_->setEndianness(busName_, endiannessSelector_.currentText().toStdString());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onBitSteeringChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onBitSteeringChanged()
{
	disconnect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

    changeBitSteeringColour();

    busInterface_->setBitSteering(busName_, bitSteeringSelector_.currentText().toStdString());

	connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onConnectionRequiredChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onConnectionRequiredChanged()
{
    busInterface_->setConnectionIsRequired(busName_, connectionRequired_.isChecked());
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

//-----------------------------------------------------------------------------
// Function: busifgeneraldetails::changeName()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::changeName(std::string const& newName)
{
    busName_ = newName;
}
