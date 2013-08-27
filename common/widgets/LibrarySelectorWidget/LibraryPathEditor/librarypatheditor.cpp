/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypatheditor.cpp
 *		Project: Kactus 2
 */

#include "librarypatheditor.h"

//-----------------------------------------------------------------------------
// Function: LibraryPathEditor()
//-----------------------------------------------------------------------------
LibraryPathEditor::LibraryPathEditor(QWidget *parent):
QLineEdit(parent),
unmodifiablePath_() {

}

//-----------------------------------------------------------------------------
// Function: ~LibraryPathEditor()
//-----------------------------------------------------------------------------
LibraryPathEditor::~LibraryPathEditor() {
}

//-----------------------------------------------------------------------------
// Function: setUnmodifiablePath()
//-----------------------------------------------------------------------------
void LibraryPathEditor::setUnmodifiablePath( const QString& path ) {
	unmodifiablePath_ = path;
}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void LibraryPathEditor::keyPressEvent( QKeyEvent* event ) {

	// if user clicks delete or back space
	if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {

		// if the current path is the minimum path
		if (text() == unmodifiablePath_) {
			
			// the minimum path is not changed
			event->accept();
			return;
		}
	}

	// normally use the base class implementation
	QLineEdit::keyPressEvent(event);
}
