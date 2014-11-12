//-----------------------------------------------------------------------------
// File: ComponentEditorSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.11.2014
//
// Description:
// Component editor property settings page.
//-----------------------------------------------------------------------------
#include "ComponentEditorSettingsPage.h"

#include <QVBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::ComponentEditorSettingsPage()
//-----------------------------------------------------------------------------
ComponentEditorSettingsPage::ComponentEditorSettingsPage(QSettings &settings) : SettingsPage(settings)
{
	loadSettings();
	setupLayOut();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::~ComponentEditorSettingsPage()
//-----------------------------------------------------------------------------
ComponentEditorSettingsPage::~ComponentEditorSettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::apply()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::apply()
{
	settings().beginGroup("ComponentEditorFilters");

	foreach (QCheckBox *checkBox, checkBoxes_)
	{
		settings().setValue(checkBox->text().replace(" ", "_"), checkBox->isChecked());
	}

	settings().endGroup();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setupLayOut()
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("Component editor visibility:"), this), 0);
	foreach (QCheckBox *checkBox, checkBoxes_)
	{
		layout->addWidget(checkBox,0);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::loadSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::loadSettings()
{

	SettingsPage::settings().beginGroup("ComponentEditorFilters");
	
	// A list of all the check box names that are defined.
	QStringList boxNames = SettingsPage::settings().childKeys();

	foreach (QString name, boxNames)
	{
		QString checkBoxName = name;

		QCheckBox *checkBox = new QCheckBox(checkBoxName.replace( "_", " "), this);
		checkBox->setChecked(settings().value(name, true).toBool());
		checkBoxes_.append(checkBox);
	}
	settings().endGroup();
}