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
#include <QHBoxLayout>
#include <QGroupBox>
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

	settings().beginGroup("HW");
	foreach (QCheckBox *checkBox, hwCheckBoxes_)
	{
		settings().setValue(checkBox->text().replace(" ", "_"), checkBox->isChecked());
	}
	settings().endGroup();

	settings().beginGroup("SW");
	foreach (QCheckBox *checkBox, swCheckBoxes_)
	{
		settings().setValue(checkBox->text().replace(" ", "_"), checkBox->isChecked());
	}

	settings().endGroup();

	settings().endGroup();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setupLayOut()
{
	QVBoxLayout* overLayout = new QVBoxLayout(this);

	overLayout->addWidget(new QLabel(tr("Component editor visibility:"), this), 0);

	QHBoxLayout* midLayout = new QHBoxLayout(this);

	QGroupBox* hwGroupBox = new QGroupBox(tr("Hardware:"), this);
	QVBoxLayout* hwLayout = new QVBoxLayout(hwGroupBox);

	foreach (QCheckBox *checkBox, hwCheckBoxes_)
	{
		hwLayout->addWidget(checkBox,0);
	}

	QGroupBox* swGroupBox = new QGroupBox(tr("Software:"), this);
	QVBoxLayout* swLayout = new QVBoxLayout(swGroupBox); 

	foreach (QCheckBox *checkBox, swCheckBoxes_)
	{
		swLayout->addWidget(checkBox, 0);
	}

	swLayout->insertStretch(swCheckBoxes_.size() + 1 ,1);

	swGroupBox->setLayout(swLayout);
	hwGroupBox->setLayout(hwLayout);
	midLayout->addWidget(hwGroupBox);
	midLayout->addWidget(swGroupBox);
	overLayout->addLayout(midLayout);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::loadSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::loadSettings()
{

	SettingsPage::settings().beginGroup("ComponentEditorFilters");
	
	SettingsPage::settings().beginGroup("HW");

	QStringList boxNames = SettingsPage::settings().childKeys();

	foreach (QString name, boxNames)
	{
		QString checkBoxName = name;

		QCheckBox *checkBox = new QCheckBox(checkBoxName.replace( "_", " "), this);
		checkBox->setChecked(settings().value(name, true).toBool());
		hwCheckBoxes_.append(checkBox);
	}
	
	settings().endGroup();

	SettingsPage::settings().beginGroup("SW");
	
	boxNames = SettingsPage::settings().childKeys();

	foreach (QString name, boxNames)
	{
		QString checkBoxName = name;

		QCheckBox *checkBox = new QCheckBox(checkBoxName.replace( "_", " "), this);
		checkBox->setChecked(settings().value(name, true).toBool());
		swCheckBoxes_.append(checkBox);
	}
	
	settings().endGroup();
	settings().endGroup();
}