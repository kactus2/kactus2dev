//-----------------------------------------------------------------------------
// File: hierarchygroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the check boxes to set the hierarchy options for VLNVDialer.
//-----------------------------------------------------------------------------

#include "hierarchygroup.h"

#include <KactusAPI/include/utils.h>

#include <common/widgets/tagEditor/FlowLayout.h>

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::HierarchyGroup()
//-----------------------------------------------------------------------------
HierarchyGroup::HierarchyGroup(QWidget *parent):
	FilterGroup(tr("Product Hierarchy"), parent),
flatBox_(QIcon(":icons/common/graphics/square.png"), QString(), this),
productBox_(QIcon(":icons/common/graphics/box.png"), QString(), this),
boardBox_(QIcon(":icons/common/graphics/circuit.png"), QString(), this),
chipBox_(QIcon(":icons/common/graphics/chip.png"), QString(), this),
socBox_(QIcon(":icons/common/graphics/soc.png"), QString(), this),
ipBox_(QIcon(":icons/common/graphics/capacitor.png"), QString(), this),
options_()
{
    setupButton(&flatBox_, tr("Flat"));
    setupButton(&productBox_, tr("Product"));
    setupButton(&boardBox_, tr("Board"));
    setupButton(&chipBox_, tr("Chip"));
    setupButton(&socBox_, tr("SoC"));
	setupButton(&ipBox_, tr("IP"));

	auto layout = new FlowLayout(this, 2, 1, 1);
	layout->addWidget(&productBox_);
	layout->addWidget(&boardBox_);
	layout->addWidget(&chipBox_);
	layout->addWidget(&socBox_);
    layout->addWidget(&ipBox_);
    layout->addWidget(&flatBox_);

	flatBox_.setChecked(true);
	productBox_.setChecked(true);
	boardBox_.setChecked(true);
	chipBox_.setChecked(true);
	socBox_.setChecked(true);
	ipBox_.setChecked(true);

	connect(&flatBox_, SIGNAL(clicked(bool)), this, SLOT(onFlatChange(bool)), Qt::UniqueConnection);
	connect(&productBox_, SIGNAL(clicked(bool)), this, SLOT(onProductChange(bool)), Qt::UniqueConnection);
	connect(&boardBox_, SIGNAL(clicked(bool)), this, SLOT(onBoardChange(bool)), Qt::UniqueConnection);
	connect(&chipBox_, SIGNAL(clicked(bool)), this, SLOT(onChipChange(bool)), Qt::UniqueConnection);
	connect(&socBox_, SIGNAL(clicked(bool)), this, SLOT(onSocChange(bool)), Qt::UniqueConnection);
	connect(&ipBox_, SIGNAL(clicked(bool)),	this, SLOT(onIpChange(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::setHierarchy()
//-----------------------------------------------------------------------------
void HierarchyGroup::setHierarchy(Utils::HierarchyOptions options)
{
    flatBox_.setChecked(options.flat_);
    productBox_.setChecked(options.product_);
    boardBox_.setChecked(options.board_);    
    chipBox_.setChecked(options.chip_);
    socBox_.setChecked(options.soc_);
    ipBox_.setChecked(options.ip_);

    options_ = options;
    emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::getHierarchy()
//-----------------------------------------------------------------------------
Utils::HierarchyOptions HierarchyGroup::getHierarchy() const
{
    Utils::HierarchyOptions options;
    options.flat_ = flatBox_.isChecked();
    options.product_ = productBox_.isChecked();
    options.board_ = boardBox_.isChecked();
    options.chip_ = chipBox_.isChecked();
    options.soc_ = socBox_.isChecked();
    options.ip_ = ipBox_.isChecked();
    return options;
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::selectAll()
//-----------------------------------------------------------------------------
void HierarchyGroup::selectAll(bool select)
{
    Utils::HierarchyOptions options;
    options.flat_ = select;
    options.product_ = select;
    options.board_ = select;
    options.chip_ = select;
    options.soc_ = select;
    options.ip_ = select;

	setHierarchy(options);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onFlatChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onFlatChange(bool checked)
{
	options_.flat_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onProductChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onProductChange(bool checked)
{
	options_.product_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onBoardChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onBoardChange(bool checked)
{
	options_.board_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onChipChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onChipChange(bool checked)
{
	options_.chip_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onSocChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onSocChange(bool checked)
{
	options_.soc_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onIpChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onIpChange(bool checked)
{
	options_.ip_ = checked;
	emit optionsChanged(options_);
}