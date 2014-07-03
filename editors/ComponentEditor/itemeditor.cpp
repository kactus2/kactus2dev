/* 
 *
 *  Created on: 2.2.2011
 *      Author: Antti Kamppi
 * 		filename: itemeditor.cpp
 */

#include "itemeditor.h"
#include <IPXACTmodels/component.h>
#include <library/LibraryManager/libraryinterface.h>

ItemEditor::ItemEditor( QSharedPointer<Component> component,
	LibraryInterface* handler,
	QWidget *parent): 
QWidget(parent), 
component_(component),
handler_(handler) {
}

ItemEditor::~ItemEditor() {
}

QSharedPointer<Component> ItemEditor::component() const {
	return component_;
}

//-----------------------------------------------------------------------------
// Function: confirmEditorChange()
//-----------------------------------------------------------------------------
bool ItemEditor::confirmEditorChange()
{
    // By default, we always allow the editor to be changed.
    return true;
}

//-----------------------------------------------------------------------------
// Function: ItemEditor::setProtection()
//-----------------------------------------------------------------------------
void ItemEditor::setProtection(bool locked)
{
    setDisabled(locked);
}

LibraryInterface* ItemEditor::handler() const {
	return handler_;
}
