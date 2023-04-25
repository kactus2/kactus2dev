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

#include <QIcon>
#include <QGridLayout>

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
    setupLayout();

    hwBox_.setChecked(true);
	swBox_.setChecked(true);
	systemBox_.setChecked(true);

	connect(&hwBox_, SIGNAL(clicked(bool)),	this, SLOT(onHWChanged(bool)), Qt::UniqueConnection);
	connect(&swBox_, SIGNAL(clicked(bool)),	this, SLOT(onSWChanged(bool)), Qt::UniqueConnection);
	connect(&systemBox_, SIGNAL(clicked(bool)),	this, SLOT(onSystemChanged(bool)), Qt::UniqueConnection);
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
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(&hwBox_, 0, 0, 1, 1);
    layout->addWidget(&swBox_, 0, 1, 1, 1);
    layout->addWidget(&systemBox_, 0, 2, 1, 1);
    layout->setSpacing(0);
    layout->setColumnStretch(3, 1);
    layout->setContentsMargins(4, 4, 4, 4);
}

