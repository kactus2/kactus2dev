/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: typegroup.cpp
 *		Project: Kactus 2
 */

#include "typegroup.h"

#include <QGridLayout>

TypeGroup::TypeGroup(QWidget *parent):
QGroupBox(tr("Type"), parent),
componentBox_(tr("Component"), this),
busApiBox_(tr("Bus/API"), this),
advancedBox_(tr("Advanced"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&componentBox_, 0, 0, 1, 1);
	layout->addWidget(&busApiBox_, 0, 1, 1, 1);
	layout->addWidget(&advancedBox_, 0, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	componentBox_.setChecked(true);
	busApiBox_.setChecked(true);
	advancedBox_.setChecked(false);

	connect(&componentBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onComponentChange(int)), Qt::UniqueConnection);
	connect(&busApiBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onBusChange(int)), Qt::UniqueConnection);
	connect(&advancedBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onAdvancedChange(int)), Qt::UniqueConnection);
}

TypeGroup::~TypeGroup() {
}

void TypeGroup::onComponentChange( int state ) {
	options_.components_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void TypeGroup::onBusChange( int state ) {
	options_.buses_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void TypeGroup::onAdvancedChange( int state ) {
	options_.advanced_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}
