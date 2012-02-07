/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanager.cpp
 */

#include "groupmanager.h"

#include "groupmanagerdelegate.h"

#include <common/dialogs/comboSelector/comboselector.h>

GroupManager::GroupManager(const QString title /*= tr("Group identifiers")*/, 
						   QWidget *parent /*= 0*/):
ListManager(title, parent) {
}

GroupManager::~GroupManager() {
}

void GroupManager::initialize( const QStringList& items /*= QStringList()*/ ) {
	ListManager::initialize(items);

	view_->setItemDelegate(new GroupManagerDelegate(this));
}
