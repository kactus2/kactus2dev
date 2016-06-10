//-----------------------------------------------------------------------------
// File: busdefgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// BusDefGroup contains elements to set the properties of a Bus Definition.
//-----------------------------------------------------------------------------

#include "busdefgroup.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegExpValidator>
#include <QRegExp>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusDefGroup::BusDefGroup()
//-----------------------------------------------------------------------------
BusDefGroup::BusDefGroup(QWidget *parent): 
QGroupBox(tr("General (Bus Definition)"), parent),
    busDef_(),
    directConnection_(tr("Allow direct master-slave connection"), this),
    isBroadcast_(tr("Support broadcast"), this),
    isAddressable_(tr("Addressable bus"), this),
    maxMastersEditor_(this),
    maxSlavesEditor_(this),
    systemgroupEditor_(tr("System group names"), this),
    descriptionEditor_(this)
{    
    QRegExp numberExpression(QString("[0-9]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    QRegExpValidator* numberValidator = new QRegExpValidator(numberExpression, this);
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
// Function: BusDefGroup::~BusDefGroup()
//-----------------------------------------------------------------------------
BusDefGroup::~BusDefGroup()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::setBusDef()
//-----------------------------------------------------------------------------
void BusDefGroup::setBusDef( QSharedPointer<BusDefinition> busDef )
{
	busDef_ = busDef;

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
    descriptionLayout->addWidget(&descriptionEditor_, 1, Qt::AlignTop);
    
    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(selectionGroup);
    topLayout->addWidget(&systemgroupEditor_);
    topLayout->addWidget(descriptionGroup, 1);

    maxMastersEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    maxSlavesEditor_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    systemgroupEditor_.setMinimumHeight(100);
    descriptionEditor_.setMinimumHeight(100);
}
