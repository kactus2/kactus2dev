//-----------------------------------------------------------------------------
// File: usagecombobox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.08.2012
//
// Description:
// The combo box to select one of the defined usage types.
//-----------------------------------------------------------------------------

#include "usagecombobox.h"

//-----------------------------------------------------------------------------
// Function: usagecombobox::UsageComboBox()
//-----------------------------------------------------------------------------
UsageComboBox::UsageComboBox(QWidget *parent):
QComboBox(parent)
{
	// add items for all available usage-values
	for (int i = 0; i <= General::USAGE_COUNT; ++i)
    {
		QString text = General::usage2Str(static_cast<General::Usage>(i));
		addItem(text);
	}

	// only the available options can be selected
	setEditable(false);
}

//-----------------------------------------------------------------------------
// Function: usagecombobox::~UsageComboBox()
//-----------------------------------------------------------------------------
UsageComboBox::~UsageComboBox()
{

}

//-----------------------------------------------------------------------------
// Function: usagecombobox::getCurrentValue()
//-----------------------------------------------------------------------------
General::Usage UsageComboBox::getCurrentValue() const
{
	return static_cast<General::Usage>(currentIndex());
}

//-----------------------------------------------------------------------------
// Function: usagecombobox::setCurrentValue()
//-----------------------------------------------------------------------------
void UsageComboBox::setCurrentValue( General::Usage usage )
{
	setCurrentIndex(usage);
}
