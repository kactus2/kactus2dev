/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanager.cpp
 */

#include "dirlistmanager.h"

#include "dirlistmanagerdelegate.h"

#include <QFileDialog>
#include <QDir>

DirListManager::DirListManager( const QString title, 
							   const QFileInfo& baseLocation, 
							   QWidget *parent /*= 0*/, 
							   const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items), 
baseDir_(baseLocation) {

	view_.setItemDelegate(new DirListManagerDelegate(this, baseDir_));
}

DirListManager::~DirListManager() {
}

