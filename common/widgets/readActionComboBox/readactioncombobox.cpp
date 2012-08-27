/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: readactioncombobox.cpp
 *		Project: Kactus 2
 */

#include "readactioncombobox.h"

ReadActionComboBox::ReadActionComboBox(QWidget *parent):
QComboBox(parent) {
	setEditable(false);

	for (unsigned int i = 0; i <= General::READ_ACTION_COUNT; ++i) {
		addItem(General::READ_ACTION_STRINGS[i]);
	}
}

ReadActionComboBox::~ReadActionComboBox() {
}

General::ReadAction ReadActionComboBox::getCurrentValue() const {
	return static_cast<General::ReadAction>(currentIndex());
}

void ReadActionComboBox::setCurrentValue( General::ReadAction readAction ) {
	setCurrentIndex(readAction);
}
