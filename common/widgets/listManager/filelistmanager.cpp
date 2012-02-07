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
								 QWidget *parent /*= 0*/):
ListManager(title, parent),
baseLocation_(baseLocation) {
}

FileListManager::~FileListManager() {
}

void FileListManager::initialize( const QStringList& items /*= QStringList()*/ ) {
	ListManager::initialize(items);
}
