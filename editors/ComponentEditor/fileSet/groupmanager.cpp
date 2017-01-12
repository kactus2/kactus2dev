//-----------------------------------------------------------------------------
// File: groupmanager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.02.2011
//
// Description:
// GroupManager is a widget to manage group names within FileSet.
//-----------------------------------------------------------------------------

#include "groupmanager.h"

#include "groupmanagerdelegate.h"

#include <common/dialogs/comboSelector/comboselector.h>

//-----------------------------------------------------------------------------
// Function: GroupManager::GroupManager()
//-----------------------------------------------------------------------------
GroupManager::GroupManager(QString const& title, QWidget* parent):
ListManager(title, parent)
{

}

//-----------------------------------------------------------------------------
// Function: GroupManager::~GroupManager()
//-----------------------------------------------------------------------------
GroupManager::~GroupManager()
{

}

//-----------------------------------------------------------------------------
// Function: GroupManager::initialize()
//-----------------------------------------------------------------------------
void GroupManager::initialize(QStringList const& items)
{
	ListManager::initialize(items);

	view_->setItemDelegate(new GroupManagerDelegate(this));
}
