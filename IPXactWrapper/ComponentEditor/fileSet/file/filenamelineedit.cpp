/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenamelineedit.cpp
 */

#include "filenamelineedit.h"
#include <LibraryManager/libraryinterface.h>

#include <QFileDialog>
#include <QDir>

FileNameLineEdit::FileNameLineEdit(QWidget *parent, 
								   LibraryInterface* handler,
								   QSharedPointer<Component> component,
								   const QString& contents ):
QLineEdit(contents, parent), 
handler_(handler),
component_(component) {

	setToolTip(tr("Click here to select a file to add"));
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

	// create QDir instance to match the base location
	QDir baseDir(baseLocation);

	// create the relative path
	QString relPath = QDir::cleanPath(baseDir.relativeFilePath(file));

	// set the relative path to the lineEdit
	setText(relPath);
}

bool FileNameLineEdit::isValid() const {
	// the name can't be empty, it must contain a path to a file.
	return !text().isEmpty();
}
