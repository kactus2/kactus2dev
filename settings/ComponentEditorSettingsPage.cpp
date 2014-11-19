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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::ComponentEditorSettingsPage()
//-----------------------------------------------------------------------------
ComponentEditorSettingsPage::ComponentEditorSettingsPage(QSettings &settings) : 
SettingsPage(settings),
wsHwCheckBoxes_(),
wsSwCheckBoxes_(),
currentWorkspaceIndex_(0)
{
	loadSettings();
	setupLayout();
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
	SettingsPage::settings().beginGroup("Workspaces");

	QStringList workspaceNames = SettingsPage::settings().childGroups();

	//foreach (QString workspace, workspaceIDs)
	for (int workspaceIndex = 0; workspaceIndex < workspaceNames.size(); ++workspaceIndex)
	{
		SettingsPage::settings().beginGroup(workspaceNames.at(workspaceIndex) + "/ComponentEditorFilters");
		SettingsPage::settings().beginGroup("HW");

		for (unsigned int hierarchyIndex = 0; hierarchyIndex < KactusAttribute::KTS_PRODHIER_COUNT; ++hierarchyIndex)
		{
			KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(hierarchyIndex);
			SettingsPage::settings().beginGroup(KactusAttribute::valueToString(val));

			QStringList hwCheckboxNames = SettingsPage::settings().childKeys();
			for (int checkBoxIndex = 0; checkBoxIndex < 
				wsHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).size();  ++checkBoxIndex)
			{
				// hwCheckboxNames contains the child keys of hierarchies.
				// wsHwCheckBoxes_ has keys in the same order as hierachyKeys (see loadSettings function).
				if (hwCheckboxNames.size() == wsHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).size())
				{
					SettingsPage::settings().setValue(hwCheckboxNames.at(checkBoxIndex), 
						wsHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).at(checkBoxIndex)->isChecked());
				}
			}
			
			SettingsPage::settings().endGroup(); // hierarchy group.
		}

		SettingsPage::settings().endGroup(); // HW.

		SettingsPage::settings().beginGroup("SW");

		QStringList swCheckboxNames = SettingsPage::settings().childKeys();

		for (int checkBoxIndex = 0; checkBoxIndex < wsSwCheckBoxes_.at(workspaceIndex).size(); ++checkBoxIndex)
		{
			if (swCheckboxNames.size() == wsSwCheckBoxes_.at(workspaceIndex).size())
			{
				SettingsPage::settings().setValue(swCheckboxNames.at(checkBoxIndex), 
					wsSwCheckBoxes_.at(workspaceIndex).at(checkBoxIndex)->isChecked());
			}
		}

		SettingsPage::settings().endGroup(); // SW
		SettingsPage::settings().endGroup(); // workspace/ComponentEditorFilters.
	}

	SettingsPage::settings().endGroup(); // Workspaces.
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onWorkspaceChanged()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onWorkspaceChanged(int workspaceIndex)
{
	currentWorkspaceIndex_ = workspaceIndex;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHwHorizonSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHwHorizonSelectAll(int rowIndex)
{
	bool check = false;

	for (int columnIndex = 0; columnIndex < wsHwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++columnIndex)
	{
		if (!wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->isChecked())
		{
			check = true;
			break;
		}
	}

	for (int columnIndex = 0; columnIndex < wsHwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++columnIndex)
	{
		wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->setChecked(check);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHwVerticalSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHwVerticalSelectAll(int columnIndex)
{
	bool check = false;

	for (int rowIndex = 0; rowIndex < wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).size(); ++rowIndex)
	{
		if (!wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->isChecked())
		{
			check = true;
			break;
		}
	}

	for (int rowIndex = 0; rowIndex < wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).size(); ++rowIndex)
	{
		wsHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->setChecked(check);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onSwHorizonSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onSwHorizonSelectAll(int rowIndex)
{
	// If software side has multiple hierarchies, change 1 to the number of hierarchies.
	for (int i = 0; i < 1; ++i)
	{
		if (wsSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->isChecked())
		{
			wsSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->setChecked(false);
		}
		else
		{
			wsSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->setChecked(true);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onSwVerticalSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onSwVerticalSelectAll(int columnIndex)
{
	columnIndex = 0;

	bool check = false;

	for (int i = 0; i < wsSwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++i)
	{
		if (!wsSwCheckBoxes_.at(currentWorkspaceIndex_).at(i)->isChecked())
		{
			check = true;
			break;
		}
	}

	for (int i = 0; i < wsSwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++i)
	{
		wsSwCheckBoxes_.at(currentWorkspaceIndex_).at(i)->setChecked(check);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setCheckBoxNames()
//-----------------------------------------------------------------------------
QStringList ComponentEditorSettingsPage::setCheckBoxNames(QStringList checkBoxNames)
{
	QStringList newNames;
	newNames.clear();

	foreach (QString name, checkBoxNames)
	{
		name.replace("_", " ");
		newNames.append(name);
	}

	return newNames;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setHwTable()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setHwTable(QTableWidget* table, QStringList horizontalHeaders,
	                                         QStringList verticalHeaders, int workspaceIndex)
{
	table->setHorizontalHeaderLabels(horizontalHeaders);
	table->setVerticalHeaderLabels(verticalHeaders);

	QList <QList <QCheckBox*>> hwCheckBoxes = wsHwCheckBoxes_.at(workspaceIndex);

	for (int columnIndex = 0; columnIndex <hwCheckBoxes.size(); ++columnIndex)
	{
		for (int rowIndex = 0; rowIndex < hwCheckBoxes.at(columnIndex).size(); ++rowIndex)
		{
		    QWidget* hwWidget = new QWidget;
			QHBoxLayout* hwLayout = new QHBoxLayout(hwWidget);

			hwLayout->addWidget(hwCheckBoxes.at(columnIndex).at(rowIndex));
			hwLayout->setAlignment(Qt::AlignCenter);
			hwLayout->setContentsMargins(0,0,0,0);
			hwWidget->setLayout(hwLayout);

			table->setCellWidget(rowIndex, columnIndex, hwWidget);
		}
	}

	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setCornerButtonEnabled(false);

	connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHwVerticalSelectAll(int)));
	connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onHwHorizonSelectAll(int)));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setSwTable()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setSwTable(QTableWidget* table, QStringList horizontalHeaders, 
	                                         QStringList verticalHeaders, int workspaceIndex)
{
	table->setHorizontalHeaderLabels(horizontalHeaders);
	table->setVerticalHeaderLabels(verticalHeaders);

	QList <QCheckBox*> swCheckBoxes = wsSwCheckBoxes_.at(workspaceIndex);

	for (int rowIndex = 0; rowIndex < swCheckBoxes.size(); ++rowIndex)
	{
		QWidget* swWidget = new QWidget;
		QHBoxLayout* swLayout = new QHBoxLayout(swWidget);
		swLayout->addWidget(swCheckBoxes.at(rowIndex));
		swLayout->setAlignment(Qt::AlignCenter);
		swLayout->setContentsMargins(0,0,0,0);
		swWidget->setLayout(swLayout);

		table->setCellWidget(rowIndex, 0, swWidget);
	}

	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setCornerButtonEnabled(false);

	connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSwVerticalSelectAll(int)));
	connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSwHorizonSelectAll(int)));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setupLayout()
{
	SettingsPage::settings().beginGroup("Workspaces");
	QStringList workspaceNames = SettingsPage::settings().childGroups();
	QString currentWorkspaceName = SettingsPage::settings().value("CurrentWorkspace").toString();
	SettingsPage::settings().beginGroup(currentWorkspaceName + "/ComponentEditorFilters/HW/Global");
	QStringList hwCheckBoxNames = SettingsPage::settings().childKeys();
	SettingsPage::settings().endGroup(); // workspaceName/ComponentEditorFilters/HW
	SettingsPage::settings().beginGroup(currentWorkspaceName + "/ComponentEditorFilters/SW");
	QStringList swCheckBoxNames = SettingsPage::settings().childKeys();
	SettingsPage::settings().endGroup(); // workspaceName/ComponentEditorFilters/SW
	SettingsPage::settings().endGroup(); // Workspaces
	
	QStringList hierarchyNames;
	hierarchyNames.clear();
	for (unsigned int hierarchyIndex = 0; hierarchyIndex < KactusAttribute::KTS_PRODHIER_COUNT; ++hierarchyIndex)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(hierarchyIndex);
		hierarchyNames.append(KactusAttribute::valueToString(val));
	}

	hwCheckBoxNames = setCheckBoxNames(hwCheckBoxNames);
	swCheckBoxNames = setCheckBoxNames(swCheckBoxNames);
	QStackedWidget* workspaces = new QStackedWidget;

	for (int workspaceIndex = 0; workspaceIndex < workspaceNames.size(); ++workspaceIndex)
	{
	    QTableWidget* hwTable = new QTableWidget (hwCheckBoxNames.size(), hierarchyNames.size(), this);
		setHwTable(hwTable, hierarchyNames, hwCheckBoxNames, workspaceIndex);

		QTableWidget* swTable = new QTableWidget (swCheckBoxNames.size(), 1, this);
		QStringList swHeader("Global");
		setSwTable(swTable, swHeader, swCheckBoxNames, workspaceIndex);
		
		QTabWidget* wareTab = new QTabWidget;
	    wareTab->addTab(hwTable, "Hardware");
    	wareTab->addTab(swTable, "Software");

		workspaces->addWidget(wareTab);
	}

	assembleLayout( workspaces, workspaceNames, currentWorkspaceName);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::assembleLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::assembleLayout(QStackedWidget* workspaces, QStringList workspaceNames, 
	                                             QString currentWorkspaceName)
{
	QComboBox* workspaceCombo = new QComboBox;
	workspaceCombo->addItems(workspaceNames);

	connect(workspaceCombo, SIGNAL(activated(int)), workspaces, SLOT(setCurrentIndex(int)));
	connect(workspaceCombo, SIGNAL(activated(int)), this, SLOT(onWorkspaceChanged(int)));


	QHBoxLayout* topLayout = new QHBoxLayout;
	QLabel* workspaceLabel = new QLabel(tr("Select workspace:"));
	topLayout->addWidget(workspaceLabel);
	topLayout->addWidget(workspaceCombo);
	topLayout->addStretch(1);

	QVBoxLayout* workspaceLayout = new QVBoxLayout(this);
	workspaceLayout->addLayout(topLayout);
	workspaceLayout->addWidget(workspaces);

	for (int workspaceIndex = 0; workspaceIndex < workspaceNames.size(); ++workspaceIndex)
	{
		if (workspaceNames.at(workspaceIndex) == currentWorkspaceName)
		{
			workspaceCombo->setCurrentIndex(workspaceIndex);
			workspaces->setCurrentIndex(workspaceIndex);
			onWorkspaceChanged(workspaceIndex);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::loadSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::loadSettings()
{
	SettingsPage::settings().beginGroup("Workspaces");
	QStringList workspaceNames = SettingsPage::settings().childGroups();

	foreach (QString workspaceName, workspaceNames)
	{
		QList <QList <QCheckBox*>> hardwareCheckBoxes;

		SettingsPage::settings().beginGroup(workspaceName + "/ComponentEditorFilters");
		SettingsPage::settings().beginGroup("HW");

		for (unsigned int hierarchyIndex = 0; hierarchyIndex < KactusAttribute::KTS_PRODHIER_COUNT;
			 ++hierarchyIndex)
		{
			KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(hierarchyIndex);
			SettingsPage::settings().beginGroup(KactusAttribute::valueToString(val));

			hardwareCheckBoxes.append(setCheckBoxes());

			SettingsPage::settings().endGroup(); // Hierarchy group.
		}

		wsHwCheckBoxes_.append(hardwareCheckBoxes);

		SettingsPage::settings().endGroup(); // HW.

		SettingsPage::settings().beginGroup("SW");

		wsSwCheckBoxes_.append(setCheckBoxes());

		SettingsPage::settings().endGroup(); // SW
		SettingsPage::settings().endGroup(); // workspaceName/ComponentEditorFilters.
	}

	SettingsPage::settings().endGroup(); // Workspaces.
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setCheckBoxes()
//-----------------------------------------------------------------------------
QList <QCheckBox*> ComponentEditorSettingsPage::setCheckBoxes()
{
	QList <QCheckBox*> checkBoxes;

	QStringList boxNames = SettingsPage::settings().childKeys();
	foreach (QString name, boxNames)
	{
		QString checkBoxName = name;
		QCheckBox* checkBox = new QCheckBox;
		checkBox->setChecked(settings().value(name, true).toBool());
		checkBoxes.append(checkBox);
	}

	return checkBoxes;
}
