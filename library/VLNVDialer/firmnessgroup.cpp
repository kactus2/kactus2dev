//-----------------------------------------------------------------------------
// File: firmnessgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the check boxes to set the re-usability options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "firmnessgroup.h"

#include <KactusAPI/include/utils.h>

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::FirmnessGroup()
//-----------------------------------------------------------------------------
FirmnessGroup::FirmnessGroup(QWidget *parent):
FilterGroup(tr("Firmness"), parent),
templateBox_(QIcon(":/icons/common/graphics/template.png"), QString(), this),
mutableBox_(QIcon(":/icons/common/graphics/mutable.png"), QString(), this),
fixedBox_(QIcon(":/icons/common/graphics/fixed.png"), QString(), this),
options_()
{
    setupButton(&templateBox_, tr("Template"));
    setupButton(&mutableBox_, tr("Mutable"));
    setupButton(&fixedBox_, tr("Fixed"));

	templateBox_.setChecked(true);
	mutableBox_.setChecked(true);
	fixedBox_.setChecked(true);

    connect(&templateBox_, SIGNAL(clicked(bool)), this, SLOT(onTemplateChanged(bool)), Qt::UniqueConnection);
    connect(&mutableBox_, SIGNAL(clicked(bool)), this, SLOT(onMutableChanged(bool)), Qt::UniqueConnection);
    connect(&fixedBox_, SIGNAL(clicked(bool)), this, SLOT(onConfigurationChanged(bool)), Qt::UniqueConnection);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(&templateBox_, 0, 0, 1, 1);
    layout->addWidget(&mutableBox_, 0, 1, 1, 1);
    layout->addWidget(&fixedBox_, 0, 2, 1, 1);
    layout->setSpacing(0);
    layout->setColumnStretch(3, 1);
    layout->setContentsMargins(4, 4, 4, 4);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::setFirmness()
//-----------------------------------------------------------------------------
void FirmnessGroup::setFirmness(Utils::FirmnessOptions options)
{    
    templateBox_.setChecked(options.templates_);
    mutableBox_.setChecked(options.mutable_);
    fixedBox_.setChecked(options.fixed_);

    options_ = options;
    emit optionsChanged(options_);
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
void FirmnessGroup::onTemplateChanged(bool checked)
{
	options_.templates_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::onMutableChanged()
//-----------------------------------------------------------------------------
void FirmnessGroup::onMutableChanged(bool checked)
{
	options_.mutable_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: FirmnessGroup::onConfigurationChanged()
//-----------------------------------------------------------------------------
void FirmnessGroup::onConfigurationChanged(bool checked)
{
	options_.fixed_ = checked;
	emit optionsChanged(options_);
}
