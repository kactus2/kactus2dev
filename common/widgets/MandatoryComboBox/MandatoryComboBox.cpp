//-----------------------------------------------------------------------------
// File: MandatoryComboBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.06.2026
//
// Description:
// Combobox for mandatory items. Implements correct item colors for dark mode.
//-----------------------------------------------------------------------------

#include "MandatoryComboBox.h"

#include <common/KactusUtils.h>
#include <KactusAPI/include/KactusColors.h>

MandatoryComboBox::MandatoryComboBox(QWidget* parent) : QComboBox(parent)
{
    setProperty("mandatoryField", true);
    isDarkMode_ = KactusUtils::darkThemeEnabled();
}

void MandatoryComboBox::showPopup()
{
    // Override color of text in combobox if dark mode is in use. Can't be set
	// in one style sheet, as it also affects the selected value shown when popup is closed.
	if (isDarkMode_)
	{
		auto textColorAsRGB = KactusUtils::colorToRgbString(KactusColors::REGULAR_TEXT);
		setStyleSheet("QComboBox { color: " % textColorAsRGB % "; selection-color: black; }");
    }
	QComboBox::showPopup();
}

void MandatoryComboBox::hidePopup()
{
    if (isDarkMode_)
	{
		setStyleSheet("QComboBox { color: black; }");
	}
	QComboBox::hidePopup();
}
