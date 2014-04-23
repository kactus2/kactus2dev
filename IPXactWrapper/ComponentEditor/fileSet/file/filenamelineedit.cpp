/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenamelineedit.cpp
 */

#include "filenamelineedit.h"
#include <library/LibraryManager/libraryinterface.h>
#include <models/generaldeclarations.h>

#include <QFileDialog>
#include <QDir>

FileNameLineEdit::FileNameLineEdit(QWidget *parent, 
								   LibraryInterface* handler,
								   QSharedPointer<Component> component,
								   QSharedPointer<File> file):
QLineEdit(parent), 
handler_(handler),
component_(component),
file_(file) {

	setToolTip(tr("Click here to select a file to add"));

	setText(file_->getName());
}

FileNameLineEdit::~FileNameLineEdit() {

}

void FileNameLineEdit::mousePressEvent( QMouseEvent * e ) {

	// event is handled only if the left mouse button was pressed
	if (e->button() != Qt::LeftButton) {
		e->ignore();
		return;
	}

	// accept the event so it wont be passed to parent
	e->accept();

	const QString baseLocation = handler_->getPath(*component_->getVlnv());

	QString file = QFileDialog::getOpenFileName(this, 
		tr("Select a file"), baseLocation, QString(), 0,
		QFileDialog::ReadOnly);

	// if no file was selected
	if (file.isEmpty()) {
		return;
	}

	// get the relative path from xml file to the selected file
	QString relPath = General::getRelativePath(baseLocation, file);

	// set the relative path to the lineEdit
	setText(relPath);

	file_->setName(relPath);

	emit contentChanged();
}

bool FileNameLineEdit::isValid() const {
	// the name can't be empty, it must contain a path to a file.
	return !text().isEmpty();
}

void FileNameLineEdit::refresh() {
	setText(file_->getName());
}
