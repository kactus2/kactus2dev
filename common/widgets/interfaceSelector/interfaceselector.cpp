//-----------------------------------------------------------------------------
// File: interfaceSelector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.06.2012
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "interfaceselector.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: InterfaceSelector::InterfaceSelector()
//-----------------------------------------------------------------------------
InterfaceSelector::InterfaceSelector(BusInterfaceInterface* busInterface, QWidget* parent,
    General::InterfaceMode mode):
QComboBox(parent),
mode_(mode),
busInterface_(busInterface)
{
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
        for (auto busName : busInterface_->getItemNames())
        {
			// if theres a match then add the name to the list
            if (busInterface_->getMode(busName) == mode_)
            {
                list.append(QString::fromStdString(busName));
			}
		}
	}
	// if mode is unspecified then add all
	else
    {
        for (auto busName : busInterface_->getItemNames())
        {
            list.append(QString::fromStdString(busName));
        }
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
