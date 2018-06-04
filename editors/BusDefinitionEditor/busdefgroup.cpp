//-----------------------------------------------------------------------------
// File: busdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#include "busdefgroup.h"

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusDefGroup::BusDefGroup()
//-----------------------------------------------------------------------------
BusDefGroup::BusDefGroup(LibraryInterface* libraryHandler, QWidget *parent):
QGroupBox(tr("General (Bus Definition)"), parent),
busDef_(),
directConnection_(tr("Allow non-mirrored connections"), this),
isBroadcast_(tr("Support broadcast"), this),
isAddressable_(tr("Addressable bus"), this),
maxMastersEditor_(this),
maxSlavesEditor_(this),
systemgroupEditor_(tr("System group names"), this),
descriptionEditor_(this),
vlnvDisplay_(new VLNVDisplayer(this, VLNV())),
extendDisplay_(new VLNVEditor(VLNV::ABSTRACTIONDEFINITION, libraryHandler, this, this))
{
    vlnvDisplay_->setTitle(QStringLiteral("Bus definition VLNV"));
    extendDisplay_->setTitle(QStringLiteral("Extended bus definition VLNV"));
    extendDisplay_->setDisabled(true);

    extendDisplay_->setToolTip(QStringLiteral("Extended bus definition is not currently supported in Kactus2"));

    QRegularExpression numberExpression(QString("[0-9]*"));
    QRegularExpressionValidator* numberValidator = new QRegularExpressionValidator(numberExpression, this);
    maxMastersEditor_.setValidator(numberValidator);
    maxSlavesEditor_.setValidator(numberValidator);

    maxMastersEditor_.setPlaceholderText(tr("unbound"));
    maxSlavesEditor_.setPlaceholderText(tr("unbound"));

    systemgroupEditor_.initialize();

    setupLayout();

	connect(&maxMastersEditor_, SIGNAL(editingFinished()), this, SLOT(onMastersChanged()), Qt::UniqueConnection);
	connect(&maxSlavesEditor_, SIGNAL(editingFinished()),	this, SLOT(onSlavesChanged()), Qt::UniqueConnection);
	 
	connect(&directConnection_, SIGNAL(toggled(bool)),
        this, SLOT(onDirectConnectionChanged(bool)), Qt::UniqueConnection);
    connect(&isBroadcast_, SIGNAL(toggled(bool)), this, SLOT(onIsBroadcastChanged(bool)), Qt::UniqueConnection);
	connect(&isAddressable_, SIGNAL(toggled(bool)), this, SLOT(onIsAddressableChanged(bool)), Qt::UniqueConnection);

    connect(&systemgroupEditor_, SIGNAL(contentChanged()), this, SLOT(onSystemNamesChanged()), Qt::UniqueConnection);

    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void BusDefGroup::setBusDef( QSharedPointer<BusDefinition> busDef )
{
	busDef_ = busDef;
    vlnvDisplay_->setVLNV(busDef_->getVlnv());

    if (busDef->getExtends().isValid())
    {
        extendDisplay_->setVLNV(busDef_->getExtends());
    }

    directConnection_.setChecked(busDef_->getDirectConnection());
    isBroadcast_.setChecked(busDef_->getBroadcast().toBool());
	isAddressable_.setChecked(busDef_->getIsAddressable());

    maxMastersEditor_.setText(busDef_->getMaxMasters());
	maxSlavesEditor_.setText(busDef_->getMaxSlaves());

    systemgroupEditor_.setItems(busDef_->getSystemGroupNames());

    descriptionEditor_.setPlainText(busDef_->getDescription());
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onDirectConnectionChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onDirectConnectionChanged(bool checked)
{
	busDef_->setDirectConnection(checked);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onIsBroadcastChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onIsBroadcastChanged(bool checked)
{
    busDef_->setBroadcast(checked);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onIsAddressableChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onIsAddressableChanged(bool checked)
{
	busDef_->setIsAddressable(checked);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onMastersChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onMastersChanged()
{
    busDef_->setMaxMasters(maxMastersEditor_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onSlavesChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onSlavesChanged()
{
	busDef_->setMaxSlaves(maxSlavesEditor_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onSystemNamesChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onSystemNamesChanged()
{
    QStringList systemGroupNames = systemgroupEditor_.items();
    systemGroupNames.removeDuplicates();

    busDef_->setSystemGroupNames(systemGroupNames);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onDescriptionChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onDescriptionChanged()
{
    busDef_->setDescription(descriptionEditor_.toPlainText());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::setupLayout()
//-----------------------------------------------------------------------------
void BusDefGroup::setupLayout()
{
    QFormLayout* masterSlaveLayout = new QFormLayout();
    masterSlaveLayout->addRow(tr("Max masters on bus:"), &maxMastersEditor_);
    masterSlaveLayout->addRow(tr("Max slaves on bus:"), &maxSlavesEditor_);

    QGroupBox* selectionGroup = new QGroupBox(tr("Constraints"), this);

    QVBoxLayout* selectionsLayout = new QVBoxLayout(selectionGroup);
    selectionsLayout->addWidget(&directConnection_);
    selectionsLayout->addWidget(&isBroadcast_);
    selectionsLayout->addWidget(&isAddressable_);
    selectionsLayout->addLayout(masterSlaveLayout);
    selectionsLayout->addStretch();    

    QGroupBox* descriptionGroup = new QGroupBox(tr("Description"), this);

    QVBoxLayout* descriptionLayout = new QVBoxLayout(descriptionGroup);
    descriptionLayout->addWidget(&descriptionEditor_);

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(vlnvDisplay_, 0, 0, 1, 1);
    topLayout->addWidget(extendDisplay_, 0, 1, 1, 1);
    topLayout->addWidget(selectionGroup, 1, 0, 1, 1);
    topLayout->addWidget(&systemgroupEditor_, 1, 1, 1, 1);
    topLayout->addWidget(descriptionGroup, 0, 2, 2, 2);

    topLayout->setColumnStretch(0, 25);
    topLayout->setColumnStretch(1, 25);
    topLayout->setColumnStretch(2, 50);

    maxMastersEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    maxSlavesEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
}
