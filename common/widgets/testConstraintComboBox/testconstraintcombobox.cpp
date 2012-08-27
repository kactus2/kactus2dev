/* 
 *  	Created on: 27.8.2012
 *      Author: Antti Kamppi
 * 		filename: testconstraintcombobox.cpp
 *		Project: Kactus 2
 */

#include "testconstraintcombobox.h"

TestConstraintComboBox::TestConstraintComboBox(QWidget *parent):
QComboBox(parent) {
	setEditable(false);

	for (unsigned int i = 0; i <= General::TESTCONSTRAINT_COUNT; ++i) {
		addItem(General::TEST_CONSTRAINT_STRINGS[i]);
	}
}

TestConstraintComboBox::~TestConstraintComboBox() {
}

General::TestConstraint TestConstraintComboBox::getCurrentValue() const {
	return static_cast<General::TestConstraint>(currentIndex());
}

void TestConstraintComboBox::setCurrentValue( General::TestConstraint testConstraint ) {
	setCurrentIndex(testConstraint);
}
