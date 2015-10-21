//-----------------------------------------------------------------------------
// File: booleancombobox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// Combo box that has "true", "false" or "unspecified" choices for user to select.
//-----------------------------------------------------------------------------

#include "booleancombobox.h"

//-----------------------------------------------------------------------------
// Function: booleancombobox::BooleanComboBox()
//-----------------------------------------------------------------------------
BooleanComboBox::BooleanComboBox(QWidget *parent):
QComboBox(parent)
{
	addItem(tr("true"));  // index 0
	addItem(tr("false")); // index 1
    addItem(tr(""));
}

//-----------------------------------------------------------------------------
// Function: booleancombobox::~BooleanComboBox()
//-----------------------------------------------------------------------------
BooleanComboBox::~BooleanComboBox()
{

}

//-----------------------------------------------------------------------------
// Function: booleancombobox::getCurrentValue()
//-----------------------------------------------------------------------------
QString BooleanComboBox::getCurrentValue() const
{
	if (currentIndex() == 0)
    {
        return QStringLiteral("true");
    }
	else if (currentIndex() == 1)
    {
        return QStringLiteral("false");
    }
    else
    {
        return QStringLiteral("");
    }
}

//-----------------------------------------------------------------------------
// Function: booleancombobox::setCurrentValue()
//-----------------------------------------------------------------------------
void BooleanComboBox::setCurrentValue(QString const& value)
{
	if (value == "true")
    {
		setCurrentIndex(0);
    }
	else if (value == "false")
    {
		setCurrentIndex(1);
    }
    else
    {
        setCurrentIndex(2);
    }
}
