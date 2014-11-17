/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: firmnessgroup.cpp
 *		Project: Kactus 2
 */

#include "firmnessgroup.h"

#include <common/utils.h>

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::FirmnessGroup()
//-----------------------------------------------------------------------------
FirmnessGroup::FirmnessGroup(QWidget *parent):
QGroupBox(tr("Firmness"), parent),
templateBox_(tr("Template"), this),
mutableBox_(tr("Mutable"), this),
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
	fixedBox_.setChecked(true);

    connect(&templateBox_, SIGNAL(clicked(bool)),
        this, SLOT(onTemplateChanged(bool)), Qt::UniqueConnection);
    connect(&mutableBox_, SIGNAL(clicked(bool)),
        this, SLOT(onBlockChanged(bool)), Qt::UniqueConnection);
    connect(&fixedBox_, SIGNAL(clicked(bool)),
        this, SLOT(onConfigurationChanged(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::~FirmnessGroup()
//-----------------------------------------------------------------------------
FirmnessGroup::~FirmnessGroup() {
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::setFirmness()
//-----------------------------------------------------------------------------
void FirmnessGroup::setFirmness(Utils::FirmnessOptions options)
{    
    templateBox_.setChecked(options.templates_);
    mutableBox_.setChecked(options.mutable_);
    fixedBox_.setChecked(options.fixed_);    
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::getFirmness()
//-----------------------------------------------------------------------------
Utils::FirmnessOptions FirmnessGroup::getFirmness() const
{
    Utils::FirmnessOptions options;
    options.templates_ = templateBox_.isChecked();
    options.mutable_ = mutableBox_.isChecked();
    options.fixed_ = fixedBox_.isChecked();
    return options;
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::onTemplateChanged()
//-----------------------------------------------------------------------------
void FirmnessGroup::onTemplateChanged( bool checked ) {
	options_.templates_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::onBlockChanged()
//-----------------------------------------------------------------------------
void FirmnessGroup::onBlockChanged( bool checked ) {
	options_.mutable_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::onConfigurationChanged()
//-----------------------------------------------------------------------------
void FirmnessGroup::onConfigurationChanged( bool checked ) {
	options_.fixed_ = checked;
	emit optionsChanged(options_);
}
