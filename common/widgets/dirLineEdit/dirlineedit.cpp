/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: dirlineedit.cpp
 *		Project: Kactus 2
 */

#include "dirlineedit.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <QFileDialog>

DirLineEdit::DirLineEdit(QWidget *parent, const QFileInfo& basePath):
QLineEdit(parent),
baseLocation_(basePath) {
}

DirLineEdit::~DirLineEdit() {
}

void DirLineEdit::mousePressEvent( QMouseEvent * e ) {
	// event is handled only if the left mouse button was pressed
	if (e->button() != Qt::LeftButton) {
		e->ignore();
		return;
	}

	// accept the event so it wont be passed to parent
	e->accept();

	// the current text
	QString dir = text();

	QString absDir = General::getAbsolutePath(baseLocation_.absoluteFilePath(), dir);
	QFileInfo absInfo(absDir);

	// the directory that is opened when the dialog is displayed
	QString defaultDir;

	// if the item was for a valid directory
	if (absInfo.exists()) {
		defaultDir = absDir;
	}
	// if no then use the base directory
	else {
		defaultDir = baseLocation_.absolutePath();
	}

	QString selectedDir = QFileDialog::getExistingDirectory(this, 
		tr("Select a directory"), defaultDir);

	// set the relative path to the lineEdit
	setText(selectedDir);
}
