/* 
 *	Created on:	24.5.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvariablesettingspage.cpp
 *	Project:		Kactus 2
*/

#include "k2envvariablesettingspage.h"

#include <QVBoxLayout>

K2EnvVariableSettingsPage::K2EnvVariableSettingsPage(QSettings& settings):
PropertyPageView(),
settings_(settings),
view_(NULL) {

	view_ = new EditableTableView(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);
}

K2EnvVariableSettingsPage::~K2EnvVariableSettingsPage() {
}

void K2EnvVariableSettingsPage::apply() {

}
