/* 
 *	Created on:	24.5.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvariablesettingspage.cpp
 *	Project:    Kactus2
*/

#include "k2envvariablesettingspage.h"
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include "k2envvarmodel.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: K2EnvVariableSettingsPage::K2EnvVariableSettingsPage()
//-----------------------------------------------------------------------------
K2EnvVariableSettingsPage::K2EnvVariableSettingsPage(QSettings& settings, PluginManager& pluginMgr):
SettingsPage(settings),
    view_(new EditableTableView(this)),
    model_(new K2EnvVarModel(settings, pluginMgr, this)) 
{
	view_->setAllowImportExport(false);
	view_->setItemsDraggable(false);
	view_->setSortingEnabled(false);
	view_->setItemDelegate(new LineEditDelegate(this));

	view_->setModel(model_);

	QVBoxLayout* layout = new QVBoxLayout(this);    
	layout->addWidget(view_);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: K2EnvVariableSettingsPage::~K2EnvVariableSettingsPage()
//-----------------------------------------------------------------------------
K2EnvVariableSettingsPage::~K2EnvVariableSettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: K2EnvVariableSettingsPage::apply()
//-----------------------------------------------------------------------------
void K2EnvVariableSettingsPage::apply()
{
	model_->apply(settings());
}

//-----------------------------------------------------------------------------
// Function: K2EnvVariableSettingsPage::prevalidate()
//-----------------------------------------------------------------------------
bool K2EnvVariableSettingsPage::prevalidate() const
{
	return !model_->containsEmptyVariables();
}

//-----------------------------------------------------------------------------
// Function: K2EnvVariableSettingsPage::validate()
//-----------------------------------------------------------------------------
bool K2EnvVariableSettingsPage::validate()
{
	if (!prevalidate())
    {
		QMessageBox::critical(this, QCoreApplication::applicationName(),
            tr("All environment variables must have defined name."));
		return false;
	}

	return true;
}
