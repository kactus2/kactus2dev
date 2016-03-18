//-----------------------------------------------------------------------------
// File: accesscombobox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The combo box to select one of the defined access types.
//-----------------------------------------------------------------------------

#include "accesscombobox.h"

//-----------------------------------------------------------------------------
// Function: AccessComboBox::AccessComboBox()
//-----------------------------------------------------------------------------
AccessComboBox::AccessComboBox(QWidget *parent):
QComboBox(parent) 
{  
	// only the available options can be selected
	setEditable(false);

    // add items for all available access-values
    for (int i = 0; i <= AccessTypes::ACCESS_COUNT; ++i)
    {
        QString text = AccessTypes::access2Str(static_cast<AccessTypes::Access>(i));
        addItem(text);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessComboBox::~AccessComboBox()
//-----------------------------------------------------------------------------
AccessComboBox::~AccessComboBox() 
{

}

//-----------------------------------------------------------------------------
// Function: AccessComboBox::getCurrentValue()
//-----------------------------------------------------------------------------
AccessTypes::Access AccessComboBox::getCurrentValue() const
{
	return static_cast<AccessTypes::Access>(currentIndex());
}

//-----------------------------------------------------------------------------
// Function: AccessComboBox::setCurrentValue()
//-----------------------------------------------------------------------------
void AccessComboBox::setCurrentValue(AccessTypes::Access access)
{
	setCurrentIndex(access);
}	
