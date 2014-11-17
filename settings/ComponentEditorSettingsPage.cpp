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

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>

#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::ComponentEditorSettingsPage()
//-----------------------------------------------------------------------------
ComponentEditorSettingsPage::ComponentEditorSettingsPage(QSettings &settings) : 
SettingsPage(settings),
hwCheckBoxes_(),
swCheckBoxes_(),
currentHierarchyIndex_(0)
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
	QString workSpace = SettingsPage::settings().value("Workspaces/CurrentWorkspace").toString();

	SettingsPage::settings().beginGroup("Workspaces/" + workSpace + "/ComponentEditorFilters");
	SettingsPage::settings().beginGroup("HW");
	
	for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);

		SettingsPage::settings().beginGroup(KactusAttribute::valueToString(val));

		foreach (QCheckBox* checkBox, hwCheckBoxes_.at(i))
		{
			SettingsPage::settings().setValue(checkBox->text().replace(" ", "_"), checkBox->isChecked());
		}

		// End hierarchy group.
		SettingsPage::settings().endGroup();

	}

	// End hardware group.
	settings().endGroup();

	settings().beginGroup("SW");
	foreach (QCheckBox *checkBox, swCheckBoxes_)
	{
		settings().setValue(checkBox->text().replace(" ", "_"), checkBox->isChecked());
	}

	// End software group.
	settings().endGroup();
	// End Workspaces/CurrentWorkspace/ComponentEditorFilters group.
	settings().endGroup();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::swSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::swSelectAll()
{
	foreach (QCheckBox* checkBox, swCheckBoxes_)
	{
		checkBox->setChecked(true);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::swDeselectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::swDelectAll()
{
	foreach (QCheckBox* checkBox, swCheckBoxes_)
	{
		checkBox->setChecked(false);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHWSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHWSelectAll()
{
	foreach (QCheckBox* checkBox, hwCheckBoxes_.at(currentHierarchyIndex_))
	{
		checkBox->setChecked(true);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHWDesignAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHwDeselectAll()
{
	foreach (QCheckBox* checkBox, hwCheckBoxes_.at(currentHierarchyIndex_))
	{
		checkBox->setChecked(false);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHierarchyChanged()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHierarchyChanged(int indexHierarchy)
{
	currentHierarchyIndex_ = indexHierarchy;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setupLayOut()
{
	QVBoxLayout* overLayout = new QVBoxLayout(this);
	QHBoxLayout* midLayout = new QHBoxLayout;
	QStackedWidget* hwStacked = new QStackedWidget;
	QComboBox* hierarchyCombo = new QComboBox;
	
	for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
		hierarchyCombo->addItem(KactusAttribute::valueToString(val));
	}

	connect(hierarchyCombo, SIGNAL(activated(int)), hwStacked, SLOT(setCurrentIndex(int)));
	connect(hierarchyCombo, SIGNAL(activated(int)), this, SLOT(onHierarchyChanged(int)));

	for (int i = 0; i < hwCheckBoxes_.size(); ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);
		QGroupBox* hwGroupBox = new QGroupBox(KactusAttribute::valueToString(val) + ":");
		QVBoxLayout* hwLayout = new QVBoxLayout;

		QHBoxLayout* hwLow = new QHBoxLayout;
		QPushButton* hwAllSelect = new QPushButton ("Select all");
		QPushButton* hwAllDeselect = new QPushButton ("Deselect all");

		foreach (QCheckBox* checkBox, hwCheckBoxes_.at(i))
		{
			hwLayout->addWidget(checkBox, 0);
		}

		connect(hwAllSelect, SIGNAL(clicked()), this, SLOT(onHWSelectAll()));
		connect(hwAllDeselect, SIGNAL(clicked()), this, SLOT(onHwDeselectAll()));

		hwLow->addWidget(hwAllSelect);
		hwLow->addWidget(hwAllDeselect);
		hwLow->addStretch(1);

		hwLayout->addStretch(1);
		hwLayout->addLayout(hwLow);

		hwGroupBox->setLayout(hwLayout);
		hwStacked->addWidget(hwGroupBox);
	}

	QGroupBox* swGroupBox = new QGroupBox(tr("Software editor visibilities:"));
	QVBoxLayout* swLayout = new QVBoxLayout(swGroupBox); 

	foreach (QCheckBox* checkBox, swCheckBoxes_)
	{
		swLayout->addWidget(checkBox, 0);
	}

	QHBoxLayout* swLow = new QHBoxLayout;
	QPushButton* swAllSelect = new QPushButton ("Select all");
	QPushButton* swAllDeselect = new QPushButton ("Deselect all");

	connect(swAllSelect, SIGNAL(clicked()), this, SLOT(swSelectAll()));
	connect(swAllDeselect, SIGNAL (clicked()), this, SLOT(swDelectAll()));

	swLow->addWidget(swAllSelect);
	swLow->addWidget(swAllDeselect);
	swLow->addStretch(1);

	swLayout->addStretch(1);

	swLayout->addLayout(swLow);

	swGroupBox->setLayout(swLayout);
	
	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->addWidget(hierarchyCombo);
	topLayout->addStretch(1);

	midLayout->addWidget(hwStacked);
	midLayout->addWidget(swGroupBox);

	overLayout->addLayout(topLayout);
	overLayout->addLayout(midLayout);	
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::loadSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::loadSettings()
{
	QString workSpace = SettingsPage::settings().value("Workspaces/CurrentWorkspace").toString();

	SettingsPage::settings().beginGroup("Workspaces/" + workSpace + "/ComponentEditorFilters");

	SettingsPage::settings().beginGroup("HW");

	for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(i);

		SettingsPage::settings().beginGroup(KactusAttribute::valueToString(val));
		
		QList <QCheckBox*> hierarchyBoxes;
		hierarchyBoxes.clear();

		hwCheckBoxes_.append(setCheckBoxes(hierarchyBoxes));

		// End hierarchy group.
		SettingsPage::settings().endGroup();

	}

	// End hardware group.
	settings().endGroup();

	SettingsPage::settings().beginGroup("SW");
	
	swCheckBoxes_ = setCheckBoxes(swCheckBoxes_);

	// End software group.
	settings().endGroup();
	// End Workspaces/CurrentWorkspace/ComponentEditorFilters group.
	settings().endGroup();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setCheckBoxes()
//-----------------------------------------------------------------------------
QList <QCheckBox*> ComponentEditorSettingsPage::setCheckBoxes(QList <QCheckBox*> checkBoxes)
{
	QStringList boxNames = SettingsPage::settings().childKeys();

	foreach (QString name, boxNames)
	{
		QString checkBoxName = name;

		QCheckBox* checkBox = new QCheckBox(checkBoxName.replace( "_", " "));
		checkBox->setChecked(settings().value(name, true).toBool());
		checkBoxes.append(checkBox);
	}

	return checkBoxes;
}
