/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busdefgroup.cpp
 */

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
    isAddressable_(tr("Addressable bus"), this),
    maxMasters_(this),
    maxSlaves_(this),
    descriptionEditor_(this)
{    
    QRegExp regExp(QString("[0-9]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    QRegExpValidator* validator = new QRegExpValidator(regExp, this);
    maxMasters_.setValidator(validator);
    maxSlaves_.setValidator(validator);
	
    maxMasters_.setPlaceholderText(tr("unbound"));
    maxSlaves_.setPlaceholderText(tr("unbound"));

    setupLayout();

	connect(&maxMasters_, SIGNAL(editingFinished()), this, SLOT(onMastersChanged()), Qt::UniqueConnection);
	connect(&maxSlaves_, SIGNAL(editingFinished()),	this, SLOT(onSlavesChanged()), Qt::UniqueConnection);
	 
	connect(&directConnection_, SIGNAL(toggled(bool)),
        this, SLOT(onDirectConnectionChanged(bool)), Qt::UniqueConnection);
	connect(&isAddressable_, SIGNAL(toggled(bool)),
		this, SLOT(onIsAddressableChanged(bool)), Qt::UniqueConnection);

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

	// set value for direct connection check box
    directConnection_.setChecked(busDef_->getDirectConnection());

	// set value for is addressable check box
	isAddressable_.setChecked(busDef_->getIsAddressable());

	// set value for the max masters line edit
	if (busDef_->getMaxMasters() < 0)
		maxMasters_.setText(QString());
	else 
		maxMasters_.setText(QString::number(busDef_->getMaxMasters()));

	// set value for the max slaves line edit
	if (busDef_->getMaxSlaves() < 0)
		maxSlaves_.setText(QString());
	else
		maxSlaves_.setText(QString::number(busDef_->getMaxSlaves()));

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
	QString text = maxMasters_.text();

	// if was not set
	if (text.isEmpty()) 
		busDef_->setMaxMasters(-1);

	// convert text to number
	else
		busDef_->setMaxMasters(text.toInt());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusDefGroup::onSlavesChanged()
//-----------------------------------------------------------------------------
void BusDefGroup::onSlavesChanged()
{
	QString text = maxSlaves_.text();

	// if was not set
	if (text.isEmpty())
		busDef_->setMaxSlaves(-1);

	// convert text to number
	else
		busDef_->setMaxSlaves(text.toInt());

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
    masterSlaveLayout->addRow(tr("Max masters on bus:"), &maxMasters_);
    masterSlaveLayout->addRow(tr("Max slaves on bus:"), &maxSlaves_);

    QVBoxLayout* selectionsLayout = new QVBoxLayout();
    selectionsLayout->addWidget(&directConnection_);
    selectionsLayout->addWidget(&isAddressable_);
    selectionsLayout->addLayout(masterSlaveLayout);
    selectionsLayout->addStretch();    

    QVBoxLayout* descriptionLayout = new QVBoxLayout();
    descriptionLayout->addWidget(new QLabel(tr("Description:"), this), 0, Qt::AlignTop);
    descriptionLayout->addWidget(&descriptionEditor_, 1, Qt::AlignTop);
    
    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(selectionsLayout);
    topLayout->addLayout(descriptionLayout);

    maxMasters_.setMaximumWidth(55);
    maxMasters_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    maxSlaves_.setMaximumWidth(55);
    maxSlaves_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    descriptionEditor_.setMaximumHeight(75);
}
