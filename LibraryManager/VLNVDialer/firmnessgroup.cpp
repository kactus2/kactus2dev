/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: firmnessgroup.cpp
 *		Project: Kactus 2
 */

#include "firmnessgroup.h"

#include <common/utils.h>

#include <QGridLayout>

FirmnessGroup::FirmnessGroup(QWidget *parent):
QGroupBox(tr("Firmness"), parent),
templateBox_(tr("Template"), this),
mutableBox_(tr("Mutable"), this),
//parameterizableBox_(tr("Parameterizable"), this),
fixedBox_(tr("Fixed"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&templateBox_, 0, 0, 1, 1);
	layout->addWidget(&mutableBox_, 0, 1, 1, 1);
	layout->addWidget(&fixedBox_, 0, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	templateBox_.setChecked(true);
	mutableBox_.setChecked(true);
	//parameterizableBox_.setChecked(true);
	fixedBox_.setChecked(true);

	connect(&templateBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onTemplateChanged(int)), Qt::UniqueConnection);
	connect(&mutableBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onBlockChanged(int)), Qt::UniqueConnection);
	//connect(&parameterizableBox_, SIGNAL(stateChanged(int)),
	//	this, SLOT(onIntegrationChanged(int)), Qt::UniqueConnection);
	connect(&fixedBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onConfigurationChanged(int)), Qt::UniqueConnection);
}

FirmnessGroup::~FirmnessGroup() {
}

void FirmnessGroup::onTemplateChanged( int state ) {
	options_.templates_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void FirmnessGroup::onBlockChanged( int state ) {
	options_.mutable_ = Utils::checkBoxState2Bool(state);
	options_.parameterizable_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void FirmnessGroup::onIntegrationChanged( int state ) {
	options_.parameterizable_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void FirmnessGroup::onConfigurationChanged( int state ) {
	options_.fixed_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}