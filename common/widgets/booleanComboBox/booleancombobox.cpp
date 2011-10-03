/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: booleancombobox.cpp
 */

#include "booleancombobox.h"

BooleanComboBox::BooleanComboBox(QWidget *parent): QComboBox(parent) {

	addItem(tr("true"));  // index 0
	addItem(tr("false")); // index 1
}

BooleanComboBox::~BooleanComboBox() {
}

bool BooleanComboBox::getCurrentValue() const {

	if (currentIndex() == 0)
		return true;
	else
		return false;
}

void BooleanComboBox::setCurrentValue( bool value ) {
	if (value)
		setCurrentIndex(0);
	else
		setCurrentIndex(1);
}
