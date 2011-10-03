/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanager.cpp
 */

#include "dirlistmanager.h"

#include <QFileDialog>
#include <QDir>

DirListManager::DirListManager( const QString title, 
							   const QFileInfo& baseLocation, 
							   QWidget *parent /*= 0*/, 
							   const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items), baseDir_(baseLocation) {
}

DirListManager::~DirListManager() {
}

void DirListManager::onAdd() {
	
	// ask user to select the directory
	QString dir = QFileDialog::getExistingDirectory(this, 
		tr("Select a directory"), baseDir_.absolutePath());

	// if nothing was chosen or cancel was clicked
	if (dir.isEmpty()) {
		return;
	}

	// create QDir instance that refers to the base location.
	QDir baseDir(baseDir_.absoluteDir());

	// get the relative directory path
	QString relDir = QDir::cleanPath(baseDir.relativeFilePath(dir));

	// if the directory was the same as the base directory
	if (relDir.isEmpty()) {
		return;
	}

	// create a relative path from base to selected file and clean it 
	model_.appendItem(relDir);

	// inform that content of the widget has changed
	emit contentChanged();
}

void DirListManager::onEdit() {
	
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldDir = model_.data(index, Qt::DisplayRole).toString();

	QString newDir = QFileDialog::getExistingDirectory(this, 
		tr("Select a directory"), baseDir_.absolutePath());

	// create QDir instance that refers to the base location.
	QDir baseDir(baseDir_.absoluteDir());

	// create a new file path to selected file that is relative to the base
	QString newRelDir = QDir::cleanPath(baseDir.relativeFilePath(newDir));

	// if new relative dir was empty then dont change anything
	if (newRelDir.isEmpty()) {
		return;
	}
	// if the file path changed
	else if (oldDir != newRelDir) {
		model_.replace(index, newRelDir);

		// inform that content of the widget has changed
		emit contentChanged();
	}
}
