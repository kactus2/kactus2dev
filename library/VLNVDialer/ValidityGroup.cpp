//-----------------------------------------------------------------------------
// File: ValidityGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.09.2023
//
// Description:
// Contains the check boxes to set the validity options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "ValidityGroup.h"

#include <KactusAPI/include/utils.h>

#include <common/widgets/tagEditor/FlowLayout.h>

//-----------------------------------------------------------------------------
// Function: ValidityGroup::ValidityGroup()
//-----------------------------------------------------------------------------
ValidityGroup::ValidityGroup(QWidget *parent):
FilterGroup(tr("Correctness"), parent),
validBox_(QIcon(":/icons/common/graphics/pass.png"), QString(), this),
nonvalidBox_(QIcon(":/icons/common/graphics/fail.png"), QString(), this),
options_()
{
    setupButton(&validBox_, tr("Valid"));
    setupButton(&nonvalidBox_, tr("Errors"));

    validBox_.setChecked(true);
    nonvalidBox_.setChecked(true);

    connect(&validBox_, SIGNAL(clicked(bool)), this, SLOT(onValidChanged(bool)), Qt::UniqueConnection);
    connect(&nonvalidBox_, SIGNAL(clicked(bool)), this, SLOT(onNonvalidChanged(bool)), Qt::UniqueConnection);

    auto layout = new FlowLayout(this, 2, 1, 1);
    layout->addWidget(&validBox_);
    layout->addWidget(&nonvalidBox_);
}

//-----------------------------------------------------------------------------
// Function: ValidityGroup::setFirmness()
//-----------------------------------------------------------------------------
void ValidityGroup::setValidity(Utils::ValidityOptions options)
{    
    validBox_.setChecked(options.valid_);
    nonvalidBox_.setChecked(options.invalid_);

    options_ = options;
    emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ValidityGroup::getFirmness()
//-----------------------------------------------------------------------------
Utils::ValidityOptions ValidityGroup::getValidity() const
{
    Utils::ValidityOptions options;
    options.valid_ = validBox_.isChecked();
    options.invalid_ = nonvalidBox_.isChecked();

    return options;
}

//-----------------------------------------------------------------------------
// Function: ValidityGroup::selectAll()
//-----------------------------------------------------------------------------
void ValidityGroup::selectAll(bool select)
{
    Utils::ValidityOptions options;
    options.valid_ = select;
    options.invalid_ = select;

    setValidity(options);
}

//-----------------------------------------------------------------------------
// Function: ValidityGroup::onValidChanged()
//-----------------------------------------------------------------------------
void ValidityGroup::onValidChanged(bool checked)
{
	options_.valid_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ValidityGroup::onNonvalidChanged()
//-----------------------------------------------------------------------------
void ValidityGroup::onNonvalidChanged(bool checked)
{
	options_.invalid_ = checked;
	emit optionsChanged(options_);
}
