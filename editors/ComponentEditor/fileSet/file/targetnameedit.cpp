/* 
 *
 *  Created on: 8.2.2011
 *      Author: Antti Kamppi
 * 		filename: targetnameedit.cpp
 */

#include "targetnameedit.h"
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QFileDialog>
#include <QDir>
#include <QMouseEvent>

TargetNameEdit::TargetNameEdit(QWidget *parent,
							   LibraryInterface* handler,
							   QSharedPointer<Component> component,
							   const QString contents): 
QLineEdit(contents, parent), 
handler_(handler),
component_(component) {

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

	const QString baseLocation = handler_->getPath(*component_->getVlnv());

	QString file = QFileDialog::getSaveFileName(this, 
		tr("Set a target file"), baseLocation);

	// if no file was selected
	if (file.isEmpty()) {
		return;
	}

	// create the relative path
	QString relPath = General::getRelativeSavePath(baseLocation, file);

	// set the relative path to the lineEdit
	setText(relPath);
	emit contentChanged();
}
