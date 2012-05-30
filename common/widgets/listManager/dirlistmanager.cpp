/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanager.cpp
 */

#include "dirlistmanager.h"
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QFileDialog>
#include <QDir>

DirListManager::DirListManager( const QString title, 
							   const QFileInfo& baseLocation, 
							   QWidget *parent /*= 0*/):
ListManager(title, parent), 
baseDir_(baseLocation) {
}

DirListManager::~DirListManager() {
}

void DirListManager::initialize( const QStringList& items /*= QStringList()*/ ) {
	ListManager::initialize(items);

	view_->setItemDelegate(new LineEditDelegate(this));
}

