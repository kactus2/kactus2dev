/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: accesscombobox.cpp
 *		Project: Kactus 2
 */

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
    for (int i = 0; i < General::ACCESS_COUNT; ++i)
    {
        QString text = General::access2Str(static_cast<General::Access>(i));
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
General::Access AccessComboBox::getCurrentValue() const
{
	return static_cast<General::Access>(currentIndex());
}

//-----------------------------------------------------------------------------
// Function: AccessComboBox::setCurrentValue()
//-----------------------------------------------------------------------------
void AccessComboBox::setCurrentValue(General::Access access)
{
	setCurrentIndex(access);
}	
