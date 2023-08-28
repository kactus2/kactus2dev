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

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <common/KactusColors.h>

#include <QCompleter>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::BusIfGeneralDetails()
//-----------------------------------------------------------------------------
BusIfGeneralDetails::BusIfGeneralDetails(BusInterfaceInterface* busInterface, std::string const& busName,
    Document::Revision docRevision, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent):
QGroupBox(tr("General") ,parent),
busInterface_(busInterface),
busName_(busName),
modeSelector_(docRevision, busInterface_->getMode(busName), true, this),
connectionRequired_(this),
bitsInLauEditor_(finder, this),
endiannessSelector_(this),
bitSteeringSelector_(this),
bitSteeringEditor_(finder, this)
{
    setFlat(true);
    
    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(finder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    auto bitInLauCompleter = new QCompleter(this);
    bitInLauCompleter->setModel(componentParameterModel);
    bitsInLauEditor_.setAppendingCompleter(bitInLauCompleter);
    bitsInLauEditor_.setFixedHeight(20);

    endiannessSelector_.addItem(QString("little"));
	endiannessSelector_.addItem(QString("big"));
    endiannessSelector_.addItem(QString(""));

	bitSteeringSelector_.addItem(QString("on"));
	bitSteeringSelector_.addItem(QString("off"));
    bitSteeringSelector_.addItem(QString(""));
    bitSteeringSelector_.setVisible(docRevision == Document::Revision::Std14);

    auto bitSteeringCompleter = new QCompleter(this);
    bitSteeringCompleter->setModel(componentParameterModel);
    bitSteeringEditor_.setAppendingCompleter(bitSteeringCompleter);

    bitSteeringEditor_.setFixedHeight(20);
    bitSteeringEditor_.setVisible(docRevision == Document::Revision::Std22);

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
    connect(&bitSteeringEditor_, SIGNAL(editingFinished()),
        this, SLOT(onBitSteeringExpressionChanged()), Qt::UniqueConnection);
    connect(&modeSelector_, SIGNAL(modeSelected(General::InterfaceMode)),
        this, SLOT(updateBitSteeringControls(General::InterfaceMode)), Qt::UniqueConnection);

    setupLayout(docRevision);
}

//-----------------------------------------------------------------------------
// Function: busifgeneraldetails::updateBitSteeringControls()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::updateBitSteeringControls(General::InterfaceMode mode)
{
    QPalette comboPalette = bitSteeringSelector_.palette();

    QColor colour = KactusColors::REGULAR_TEXT;
   
    if (mode == General::MIRRORED_MASTER || mode == General::MIRRORED_INITIATOR ||
        mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM)
    {
        if (!busInterface_->getBitSteering(busName_).empty())
        {
            colour = KactusColors::ERROR;
        }
        else
        {
            bitSteeringSelector_.setDisabled(true);
            bitSteeringEditor_.setDisabled(true);
        }
    }
    else
    {
        bitSteeringSelector_.setEnabled(true);
        bitSteeringEditor_.setEnabled(true);
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
    auto mode = busInterface_->getMode(busName_);
    modeSelector_.setMode(mode);

    QString connectionRequirement = QString::fromStdString(busInterface_->connectionIsRequired(busName_));
    connectionRequired_.setChecked(connectionRequirement == QLatin1String("true"));

    bitsInLauEditor_.setExpression(QString::fromStdString(busInterface_->getBitsInLau(busName_)));

    endiannessSelector_.setCurrentIndex(busInterface_->getEndianness(busName_));

    disconnect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),	this, SLOT(onBitSteeringChanged()));

    auto bitSteering = QString::fromStdString(busInterface_->getBitSteering(busName_));
    bitSteeringSelector_.setCurrentText(bitSteering);

    updateBitSteeringControls(mode);

    connect(&bitSteeringSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onBitSteeringChanged()), Qt::UniqueConnection);

    bitSteeringEditor_.setExpression(bitSteering);
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onAddressableUnitChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onAddressableUnitChanged()
{
    bitsInLauEditor_.finishEditingCurrentWord();

    busInterface_->setBitsInLau(busName_, bitsInLauEditor_.getExpression().toStdString());
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
    busInterface_->setBitSteering(busName_, bitSteeringSelector_.currentText().toStdString());
    
    updateBitSteeringControls(busInterface_->getMode(busName_));

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusIfGeneralDetails::onBitSteeringExpressionChanged()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::onBitSteeringExpressionChanged()
{
    bitSteeringEditor_.finishEditingCurrentWord();
    busInterface_->setBitSteering(busName_, bitSteeringEditor_.getExpression().toStdString());

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
void BusIfGeneralDetails::setupLayout(Document::Revision docRevision)
{
    QFormLayout* modeLayout = new QFormLayout(this);
    modeLayout->addRow(tr("Interface mode:"), &modeSelector_);
    modeLayout->addRow(tr("Addressable unit size, f(x):"), &bitsInLauEditor_);

    if (docRevision == Document::Revision::Std14)
    {
        modeLayout->addRow(tr("Bit steering:"), &bitSteeringSelector_);
    }
    if (docRevision == Document::Revision::Std22)
    {
        modeLayout->addRow(tr("Bit steering, f(x):"), &bitSteeringEditor_);
    }

    modeLayout->addRow(tr("Endianness:"), &endiannessSelector_);
    modeLayout->addRow(tr("Connection required:"), &connectionRequired_);
}

//-----------------------------------------------------------------------------
// Function: busifgeneraldetails::changeName()
//-----------------------------------------------------------------------------
void BusIfGeneralDetails::changeName(std::string const& newName)
{
    busName_ = newName;
}
