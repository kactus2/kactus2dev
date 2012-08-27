/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: modwritecombobox.cpp
 *		Project: Kactus 2
 */

#include "modwritecombobox.h"

ModWriteComboBox::ModWriteComboBox(QWidget *parent):
QComboBox(parent) {

	setEditable(false);

	for (unsigned int i = 0; i <= General::MODIFIED_WRITE_COUNT; ++i) {
		addItem(General::MODIFIED_WRITE_STRINGS[i]);
	}
}

ModWriteComboBox::~ModWriteComboBox() {
}

General::ModifiedWrite ModWriteComboBox::getCurrentValue() const {
	return static_cast<General::ModifiedWrite>(currentIndex());
}

void ModWriteComboBox::setCurrentValue( General::ModifiedWrite modWriteValue ) {
	setCurrentIndex(modWriteValue);
}
