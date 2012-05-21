/* 
 *  	Created on: 21.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgroupslot.cpp
 *		Project: Kactus 2
 */

#include "componenteditorgroupslot.h"

ComponentEditorGroupSlot::ComponentEditorGroupSlot( QWidget *parent, 
												   const QString& title /*= QString()*/ ):
QGroupBox(title, parent),
layout_(this),
currentWidget_(NULL) {

}

ComponentEditorGroupSlot::~ComponentEditorGroupSlot() {
}

void ComponentEditorGroupSlot::setWidget( QWidget* widget ) {

	// if there is a previous widget being displayed
	if (currentWidget_) {
		// remove the previous because only one widget can be displayed at a time.
		layout_.removeWidget(currentWidget_);
	}

	// update the current widget and display it
	currentWidget_ = widget;
	layout_.addWidget(currentWidget_);
}
