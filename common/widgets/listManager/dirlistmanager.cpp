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
							   LibraryInterface* handler,
							   QSharedPointer<Component> component,
							   QWidget *parent /*= 0*/):
ListManager(title, parent), 
handler_(handler), 
component_(component) {
}

DirListManager::~DirListManager() {
}

void DirListManager::initialize( const QStringList& items /*= QStringList()*/ ) {
	ListManager::initialize(items);

	view_->setItemDelegate(new LineEditDelegate(this));
}

