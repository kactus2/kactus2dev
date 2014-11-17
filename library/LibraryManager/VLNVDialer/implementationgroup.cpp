/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: firmnessgroup.cpp
 *		Project: Kactus 2
 */

#include "implementationgroup.h"

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::ImplementationGroup()
//-----------------------------------------------------------------------------
ImplementationGroup::ImplementationGroup(QWidget *parent): 
QGroupBox(tr("Implementation"), parent),
hwBox_(tr("HW"), this),
swBox_(tr("SW"), this),
systemBox_(tr("System"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&hwBox_, 0, 0, 1, 1);
	layout->addWidget(&swBox_, 0, 1, 1, 1);
	layout->addWidget(&systemBox_, 0, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	hwBox_.setChecked(true);
	swBox_.setChecked(true);
	systemBox_.setChecked(true);

	connect(&hwBox_, SIGNAL(clicked(bool)),
		this, SLOT(onHWChanged(bool)), Qt::UniqueConnection);
	connect(&swBox_, SIGNAL(clicked(bool)),
		this, SLOT(onSWChanged(bool)), Qt::UniqueConnection);
	connect(&systemBox_, SIGNAL(clicked(bool)),
		this, SLOT(onSystemChanged(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::~ImplementationGroup()
//-----------------------------------------------------------------------------
ImplementationGroup::~ImplementationGroup() {
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::setImplementation()
//-----------------------------------------------------------------------------
void ImplementationGroup::setImplementation(Utils::ImplementationOptions options)
{
    hwBox_.setChecked(options.hw_);
    swBox_.setChecked(options.sw_);
    systemBox_.setChecked(options.system_);    
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::getImplementation()
//-----------------------------------------------------------------------------
Utils::ImplementationOptions ImplementationGroup::getImplementation() const
{
    Utils::ImplementationOptions options;
    options.hw_ = hwBox_.isChecked();
    options.sw_ = swBox_.isChecked();
    options.system_ = systemBox_.isChecked();
    return options;
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onHWChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onHWChanged( bool checked ) {
	options_.hw_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onSWChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onSWChanged( bool checked ) {
	options_.sw_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onSystemChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onSystemChanged( bool checked ) {
	options_.system_ = checked;
	emit optionsChanged(options_);
}


