/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: signalsgroup.cpp
 */

#include "signalsgroup.h"

#include <QHBoxLayout>

SignalsGroup::SignalsGroup(QWidget *parent): 
QGroupBox(tr("Signals"), parent),
newSignalButton_(tr("Add new signal"), this),
newSignalOptionsButton_(tr("Add new signal options"), this) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&newSignalButton_);
	layout->addWidget(&newSignalOptionsButton_);

	connect(&newSignalButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(addNewSignal()), Qt::UniqueConnection);
	connect(&newSignalOptionsButton_, SIGNAL(clicked(bool)),
		this, SIGNAL(addNewSignalOptions()), Qt::UniqueConnection);
}

SignalsGroup::~SignalsGroup() {
}
