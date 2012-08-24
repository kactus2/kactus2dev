/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: usagecombobox.cpp
 *		Project: Kactus 2
 */

#include "usagecombobox.h"

UsageComboBox::UsageComboBox(QWidget *parent):
QComboBox(parent) {

	// add items for all available usage-values
	for (int i = 0; i < General::USAGE_COUNT; ++i) {
		QString text = General::usage2Str(static_cast<General::Usage>(i));
		addItem(text);
	}

	// only the available options can be selected
	setEditable(false);
}

UsageComboBox::~UsageComboBox() {
}

General::Usage UsageComboBox::getCurrentValue() const {
	return static_cast<General::Usage>(currentIndex());
}

void UsageComboBox::setCurrentValue( General::Usage usage ) {
	setCurrentIndex(usage);
}
