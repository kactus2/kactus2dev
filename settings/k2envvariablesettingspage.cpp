/* 
 *	Created on:	24.5.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvariablesettingspage.cpp
 *	Project:		Kactus 2
*/

#include "k2envvariablesettingspage.h"
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include "k2envvarmodel.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

K2EnvVariableSettingsPage::K2EnvVariableSettingsPage(QSettings& settings, PluginManager& pluginMgr):
PropertyPageView(),
settings_(settings),
view_(NULL),
model_(NULL) {

	// set up the view
	view_ = new EditableTableView(this);
	view_->setAllowImportExport(false);
	view_->setItemsDraggable(false);
	view_->setSortingEnabled(false);
	view_->setItemDelegate(new LineEditDelegate(this));

	// set up the model
	model_ = new K2EnvVarModel(settings_, pluginMgr, this);
	view_->setModel(model_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

K2EnvVariableSettingsPage::~K2EnvVariableSettingsPage() {
}

void K2EnvVariableSettingsPage::apply() {
	model_->apply(settings_);
}

bool K2EnvVariableSettingsPage::prevalidate() const {
	return !model_->containsEmptyVariables();
}

bool K2EnvVariableSettingsPage::validate() {
	if (model_->containsEmptyVariables()) {
		QMessageBox::critical(this, QCoreApplication::applicationName(), tr("All environment variables must have defined name."));
		return false;
	}
	return true;
}

bool K2EnvVariableSettingsPage::onPageChange() {
	return !model_->containsEmptyVariables();
}
