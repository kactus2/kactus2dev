/* 
 *  	Created on: 26.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfaceselector.cpp
 *		Project: Kactus 2
 */

#include "interfaceselector.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::InterfaceSelector()
//-----------------------------------------------------------------------------
InterfaceSelector::InterfaceSelector(QSharedPointer<Component> component,
    QWidget *parent,
    General::InterfaceMode mode):
QComboBox(parent),
    mode_(mode),
component_(component)
{
	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::~InterfaceSelector()
//-----------------------------------------------------------------------------
InterfaceSelector::~InterfaceSelector()
{
}

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::setInterfaceMode()
//-----------------------------------------------------------------------------
void InterfaceSelector::setInterfaceMode(General::InterfaceMode mode)
{
	mode_ = mode;
	refresh();
}

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::refresh()
//-----------------------------------------------------------------------------
void InterfaceSelector::refresh()
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// the list to add to combo box
	QStringList list;

	// if mode is specified then check that the modes match
	if (mode_ != General::INTERFACE_MODE_COUNT)
    {
		// check that each name matches the mode
		foreach (QSharedPointer<BusInterface> busInterface, *component_->getBusInterfaces())
        {			
			// if theres a match then add the name to the list
			if (busInterface->getInterfaceMode() == mode_)
            {
				list.append(busInterface->name());
			}
		}
	}
	// if mode is unspecified then add all
	else
    {
		list = component_->getBusInterfaceNames();
	}

	addItems(list);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::refresh()
//-----------------------------------------------------------------------------
void InterfaceSelector::selectInterface(QString const& interfaceName)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));
	setCurrentIndex(findText(interfaceName));
	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::onIndexChange()
//-----------------------------------------------------------------------------
void InterfaceSelector::onIndexChange(int newIndex)
{
	// find the text for the index
	emit interfaceSelected(itemText(newIndex));
}
