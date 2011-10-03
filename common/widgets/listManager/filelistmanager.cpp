/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: filelistmanager.cpp
 */

#include "filelistmanager.h"

#include <QFileDialog>
#include <QDir>

FileListManager::FileListManager( const QString title,
								 const QFileInfo& baseLocation, 
								 QWidget *parent /*= 0*/, 
								 const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items), baseLocation_(baseLocation) {
}

FileListManager::~FileListManager() {
}

void FileListManager::onAdd() {
	
	QStringList files = QFileDialog::getOpenFileNames(this, 
		tr("Select the files you want to add"), baseLocation_.absolutePath());

	// if nothing was chosen or cancel was clicked
	if (files.isEmpty()) {
		return;
	}

	// create QDir instance that refers to the base location.
	QDir baseDir(baseLocation_.absoluteDir());

	// add all paths to list
	foreach (QString item, files) {

		// if a file path is empty then dont append it
		if (item.isEmpty()) {
			continue;
		}

		// create a relative path from base to selected file and clean it 
		model_.appendItem(QDir::cleanPath(baseDir.relativeFilePath(item)));

		// inform that content of the widget has changed
		emit contentChanged();
	}
}

void FileListManager::onEdit() {
	
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldFile = model_.data(index, Qt::DisplayRole).toString();

	QString newFile = QFileDialog::getOpenFileName(this, 
		tr("Select a new file"), baseLocation_.absolutePath());

	// create QDir instance that refers to the base location.
	QDir baseDir(baseLocation_.absoluteDir());

	// create a new file path to selected file that is relative to the base
	QString newRelFile = QDir::cleanPath(baseDir.relativeFilePath(newFile));

	// if new relative path is empty then dont append it
	if (newRelFile.isEmpty()) {
		return;
	}

	// if the file path changed
	else if (oldFile != newRelFile) {
		model_.replace(index, newRelFile);

		// inform that content of the widget has changed
		emit contentChanged();
	}
}
