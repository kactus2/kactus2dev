/* 
 *
 *  Created on: 2.2.2011
 *      Author: Antti Kamppi
 * 		filename: itemeditor.cpp
 */

#include "itemeditor.h"
#include <models/component.h>

ItemEditor::ItemEditor( QSharedPointer<Component> component, QWidget *parent): 
QWidget(parent), 
component_(component) {
}

ItemEditor::~ItemEditor() {
}

QSharedPointer<Component> ItemEditor::component() const {
	return component_;
}

void ItemEditor::removeModel() {
	return;
}

//-----------------------------------------------------------------------------
// Function: confirmEditorChange()
//-----------------------------------------------------------------------------
bool ItemEditor::confirmEditorChange()
{
    // By default, we always allow the editor to be changed.
    return true;
}
