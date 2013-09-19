/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: typegroup.cpp
 *		Project: Kactus 2
 */

#include "typegroup.h"

#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: TypeGroup::TypeGroup()
//-----------------------------------------------------------------------------
TypeGroup::TypeGroup(QWidget *parent):
QGroupBox(tr("Item Type"), parent),
componentBox_(tr("Component"), this),
busApiComBox_(tr("Bus/API/COM"), this),
advancedBox_(tr("Advanced"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&componentBox_, 0, 0, 1, 1);
	layout->addWidget(&busApiComBox_, 0, 1, 1, 1);
	layout->addWidget(&advancedBox_, 0, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	componentBox_.setChecked(true);
	busApiComBox_.setChecked(true);
	advancedBox_.setChecked(false);

	connect(&componentBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onComponentChange(int)), Qt::UniqueConnection);
	connect(&busApiComBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onBusChange(int)), Qt::UniqueConnection);
	connect(&advancedBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onAdvancedChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::~TypeGroup()
//-----------------------------------------------------------------------------
TypeGroup::~TypeGroup() {
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::setType()
//-----------------------------------------------------------------------------
void TypeGroup::setTypes(Utils::TypeOptions options)
{
    componentBox_.setChecked(options.components_);
    busApiComBox_.setChecked(options.buses_);
    advancedBox_.setChecked(options.advanced_);    
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::getType()
//-----------------------------------------------------------------------------
Utils::TypeOptions TypeGroup::getTypes() const
{
    Utils::TypeOptions options;
    options.components_ = componentBox_.isChecked();
    options.buses_ = busApiComBox_.isChecked();
    options.advanced_ = advancedBox_.isChecked();
    return options;
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::onComponentChange()
//-----------------------------------------------------------------------------
void TypeGroup::onComponentChange( int state ) {
	options_.components_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::onBusChange()
//-----------------------------------------------------------------------------
void TypeGroup::onBusChange( int state ) {
	options_.buses_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

//-----------------------------------------------------------------------------
// Function: TypeGroup::onAdvancedChange()
//-----------------------------------------------------------------------------
void TypeGroup::onAdvancedChange( int state ) {
	options_.advanced_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}
