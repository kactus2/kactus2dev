/* 
 *
 *  Created on: 8.2.2011
 *      Author: Antti Kamppi
 * 		filename: targetnameedit.cpp
 */

#include "targetnameedit.h"

#include <QFileDialog>
#include <QDir>
#include <QMouseEvent>

TargetNameEdit::TargetNameEdit(QWidget *parent,
							   const QFileInfo& baseLocation,
							   const QString contents): 
QLineEdit(contents, parent), baseLocation_(baseLocation) {

}

TargetNameEdit::~TargetNameEdit() {
}

void TargetNameEdit::mousePressEvent( QMouseEvent * e ) {
	
	// event is handled only if the left mouse button was pressed
	if (e->button() != Qt::LeftButton) {
		e->ignore();
		return;
	}

	// accept the event so it wont be passed to parent
	e->accept();

	QString file = QFileDialog::getSaveFileName(this, 
		tr("Set a target file"), baseLocation_.absolutePath());

	// if no file was selected
	if (file.isEmpty()) {
		return;
	}

	// create QDir instance to match the base location
	QDir baseDir(baseLocation_.absoluteDir());

	// create the relative path
	QString relPath = QDir::cleanPath(baseDir.relativeFilePath(file));

	// set the relative path to the lineEdit
	setText(relPath);
}
