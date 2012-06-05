/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenamelineedit.cpp
 */

#include "filenamelineedit.h"

#include <QFileDialog>
#include <QDir>

FileNameLineEdit::FileNameLineEdit(QWidget *parent, 
								   const QFileInfo& baseLocation, 
								   const QString& contents ):
QLineEdit(contents, parent), baseLocation_(baseLocation) {

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

	QString file = QFileDialog::getOpenFileName(this, 
		tr("Select a file"), baseLocation_.absolutePath(), QString(), 0,
		QFileDialog::ReadOnly);

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

bool FileNameLineEdit::isValid() const {
	// the name can't be empty, it must contain a path to a file.
	return !text().isEmpty();
}
