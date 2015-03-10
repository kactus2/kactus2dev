/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchygroup.cpp
 *		Project: Kactus 2
 */

#include "hierarchygroup.h"

#include <common/utils.h>

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::HierarchyGroup()
//-----------------------------------------------------------------------------
HierarchyGroup::HierarchyGroup(QWidget *parent):
QGroupBox(tr("Product Hierarchy"), parent),
flatBox_(tr("Flat"), this),
productBox_(tr("Product"), this),
boardBox_(tr("Board"), this),
chipBox_(tr("Chip"), this),
socBox_(tr("SoC"), this),
ipBox_(tr("IP"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&flatBox_, 0, 0, 1, 1);
	layout->addWidget(&productBox_, 0, 1, 1, 1);
	layout->addWidget(&boardBox_, 0, 2, 1, 1);
	layout->addWidget(&chipBox_, 1, 0, 1, 1);
	layout->addWidget(&socBox_, 1, 1, 1, 1);
	layout->addWidget(&ipBox_, 1, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

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
// Function: HierarchyGroup::~HierarchyGroup()
//-----------------------------------------------------------------------------
HierarchyGroup::~HierarchyGroup() {
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
// Function: HierarchyGroup::onFlatChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onFlatChange( bool checked ) {
	options_.flat_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onProductChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onProductChange( bool checked ) {
	options_.product_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onBoardChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onBoardChange( bool checked ) {
	options_.board_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onChipChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onChipChange( bool checked ) {
	options_.chip_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onSocChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onSocChange( bool checked ) {
	options_.soc_ = checked;
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: HierarchyGroup::onIpChange()
//-----------------------------------------------------------------------------
void HierarchyGroup::onIpChange( bool checked ) {
	options_.ip_ = checked;
	emit optionsChanged(options_);
}