//-----------------------------------------------------------------------------
// File: implementationgroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains check boxes to set the implementation options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "implementationgroup.h"

#include <common/widgets/tagEditor/FlowLayout.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::ImplementationGroup()
//-----------------------------------------------------------------------------
ImplementationGroup::ImplementationGroup(QWidget* parent) :
    FilterGroup(tr("Implementation"), parent),
    hwBox_(QIcon(":icons/common/graphics/transistor.png"), QString(), this),
    swBox_(QIcon(":icons/common/graphics/sw-component48x48.png"), QString(), this),
    systemBox_(QIcon(":icons/common/graphics/system-component.png"), QString(), this),
    options_()
{
    setupButton(&hwBox_, tr("HW"));
    setupButton(&swBox_, tr("SW"));
    setupButton(&systemBox_, tr("System"));

    hwBox_.setChecked(true);
	swBox_.setChecked(true);
	systemBox_.setChecked(true);

	connect(&hwBox_, SIGNAL(clicked(bool)),	this, SLOT(onHWChanged(bool)), Qt::UniqueConnection);
	connect(&swBox_, SIGNAL(clicked(bool)),	this, SLOT(onSWChanged(bool)), Qt::UniqueConnection);
	connect(&systemBox_, SIGNAL(clicked(bool)),	this, SLOT(onSystemChanged(bool)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::setImplementation()
//-----------------------------------------------------------------------------
void ImplementationGroup::setImplementation(Utils::ImplementationOptions options)
{
    hwBox_.setChecked(options.hw_);
    swBox_.setChecked(options.sw_);
    systemBox_.setChecked(options.system_);

    options_ = options;
    emit optionsChanged(options_);
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
// Function: ImplementationGroup::selectAll()
//-----------------------------------------------------------------------------
void ImplementationGroup::selectAll(bool select)
{
    Utils::ImplementationOptions options;
    options.hw_ = select;
    options.sw_ = select;
    options.system_ = select;
    
    setImplementation(options);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onHWChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onHWChanged(bool checked)
{
	options_.hw_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onSWChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onSWChanged(bool checked)
{
	options_.sw_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: ImplementationGroup::onSystemChanged()
//-----------------------------------------------------------------------------
void ImplementationGroup::onSystemChanged(bool checked)
{
	options_.system_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::setupLayout()
//-----------------------------------------------------------------------------
void ImplementationGroup::setupLayout()
{
    auto layout = new FlowLayout(this, 2, 1, 1);
    layout->addWidget(&hwBox_);
    layout->addWidget(&swBox_);
    layout->addWidget(&systemBox_);
    //layout->setSpacing(0);
    //layout->setColumnStretch(3, 1);
    //layout->setContentsMargins(4, 4, 4, 4);
}

