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

BusDefGroup::BusDefGroup(QWidget *parent): 
QGroupBox(tr("General (Bus Definition)"), parent),
busDef_(),
directConnection_(tr("Used only between regular and bus components"), this),
isAddressable_(tr("Does not include addressing"), this),
maxMasters_(this),
maxSlaves_(this) {

	QFormLayout* masterLayout = new QFormLayout();
	masterLayout->addRow(tr("Max masters:"), &maxMasters_);

	QFormLayout* slaveLayout = new QFormLayout();
	slaveLayout->addRow(tr("Max slaves:"), &maxSlaves_);

// 	QFormLayout* lineLayout = new QFormLayout();
// 	lineLayout->addRow(tr("Max masters"), &maxMasters_);
// 	lineLayout->addRow(tr("Max slaves"), &maxSlaves_);

	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addWidget(&directConnection_);
	topLayout->addWidget(&isAddressable_);
	topLayout->addLayout(masterLayout);
	topLayout->addLayout(slaveLayout);
	topLayout->addStretch();

	QRegExp regExp(QString("[0-9]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
	QRegExpValidator* validator = new QRegExpValidator(regExp, this);
	maxMasters_.setValidator(validator);
	maxSlaves_.setValidator(validator);

	maxMasters_.setMaximumWidth(45);
	maxMasters_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	maxSlaves_.setMaximumWidth(45);
	maxSlaves_.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	connect(&maxMasters_, SIGNAL(editingFinished()),
		this, SLOT(onMastersChanged()), Qt::UniqueConnection);
	connect(&maxSlaves_, SIGNAL(editingFinished()),
		this, SLOT(onSlavesChanged()), Qt::UniqueConnection);
	 
	connect(&directConnection_, SIGNAL(stateChanged(int)),
		this, SLOT(onDirectConnectionChanged(int)), Qt::UniqueConnection);
	connect(&isAddressable_, SIGNAL(stateChanged(int)),
		this, SLOT(onIsAddressableChanged(int)), Qt::UniqueConnection);
}

BusDefGroup::~BusDefGroup() {
}

void BusDefGroup::setBusDef( QSharedPointer<BusDefinition> busDef ) {
	busDef_ = busDef;

	// set value for direct connection check box
	if (busDef_->getDirectConnection())
		directConnection_.setCheckState(Qt::Unchecked);
	else
		directConnection_.setCheckState(Qt::Checked);

	// set value for is addressable check box
	if (busDef_->getIsAddressable())
		isAddressable_.setCheckState(Qt::Unchecked);
	else
		isAddressable_.setCheckState(Qt::Checked);

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
}

void BusDefGroup::onDirectConnectionChanged( int state ) {

	// if user unchecked the box
	if (state == Qt::Unchecked) 
		busDef_->setDirectConnection(true);
	else 
		busDef_->setDirectConnection(false);

	emit contentChanged();
}

void BusDefGroup::onIsAddressableChanged( int state ) {
	
	// if user unchecked the box
	if (state == Qt::Unchecked) 
		busDef_->setIsAddressable(true);
	else 
		busDef_->setIsAddressable(false);

	emit contentChanged();
}

void BusDefGroup::onMastersChanged() {
	QString text = maxMasters_.text();

	// if was not set
	if (text.isEmpty()) 
		busDef_->setMaxMasters(-1);

	// convert text to number
	else
		busDef_->setMaxMasters(text.toInt());

	emit contentChanged();
}

void BusDefGroup::onSlavesChanged() {
	QString text = maxSlaves_.text();

	// if was not set
	if (text.isEmpty())
		busDef_->setMaxSlaves(-1);

	// convert text to number
	else
		busDef_->setMaxSlaves(text.toInt());

	emit contentChanged();
}
