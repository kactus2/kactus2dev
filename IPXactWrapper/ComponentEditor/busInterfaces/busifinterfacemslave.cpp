/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.cpp
 */

#include "busifinterfacemslave.h"

#include <models/generaldeclarations.h>

#include <QLabel>
#include <QGridLayout>

BusIfInterfaceMSlave::BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif, 
										   QSharedPointer<Component> component,
										   QWidget *parent):
BusIfInterfaceModeEditor(busif, component, tr("Mirrored slave"), parent),
mirroredSlave_(QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface())),
remapEdit_(this),
rangeEdit_(this) {

	QLabel* remapLabel = new QLabel(tr("Remap address"), this);
	QLabel* rangeLabel = new QLabel(tr("Range"), this);

	QGridLayout* topLayout = new QGridLayout(this);
	topLayout->addWidget(remapLabel, 0, 0, Qt::AlignLeft);
	topLayout->addWidget(&remapEdit_, 0, 1, Qt::AlignLeft);
	topLayout->addWidget(rangeLabel, 1, 0, Qt::AlignLeft);
	topLayout->addWidget(&rangeEdit_, 1, 1, Qt::AlignLeft);
	topLayout->setColumnStretch(2, 1);

	connect(&remapEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onRemapChange(const QString&)), Qt::UniqueConnection);
	connect(&rangeEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onRangeChange(const QString&)), Qt::UniqueConnection);
}

BusIfInterfaceMSlave::~BusIfInterfaceMSlave() {
}

bool BusIfInterfaceMSlave::isValid() const {
	return true;
}

void BusIfInterfaceMSlave::refresh() {
	// if the model contains master-element
	if (busif_->getMirroredSlave()) {
		mirroredSlave_ = busif_->getMirroredSlave();
	}
	else {
		mirroredSlave_.clear();
		mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface());
	}

	remapEdit_.setText(mirroredSlave_->getRemapAddress());
	rangeEdit_.setText(mirroredSlave_->getRange());
}

General::InterfaceMode BusIfInterfaceMSlave::getInterfaceMode() const {
	return General::MIRROREDSLAVE;
}

void BusIfInterfaceMSlave::saveModeSpecific() {
	busif_->setMirroredSlave(mirroredSlave_);
}

void BusIfInterfaceMSlave::onRemapChange( const QString& newRemapAddress ) {
	mirroredSlave_->setRemapAddress(newRemapAddress);
	emit contentChanged();
}

void BusIfInterfaceMSlave::onRangeChange( const QString& newRange ) {
	mirroredSlave_->setRange(newRange);
	emit contentChanged();
}
