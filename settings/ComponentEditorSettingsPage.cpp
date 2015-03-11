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
#include <QLabel>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::ComponentEditorSettingsPage()
//-----------------------------------------------------------------------------
ComponentEditorSettingsPage::ComponentEditorSettingsPage(QSettings &settings) : 
SettingsPage(settings),
workspaceHwCheckBoxes_(),
workspaceSwCheckBoxes_(),
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

	for (int workspaceIndex = 0; workspaceIndex < workspaceNames.size(); ++workspaceIndex)
	{
		SettingsPage::settings().beginGroup(workspaceNames.at(workspaceIndex) + "/ComponentEditorFilters");

		applyHwSettings(workspaceIndex);

		applySwSettings(workspaceIndex);

		SettingsPage::settings().endGroup(); // workspace/ComponentEditorFilters.
	}

	SettingsPage::settings().endGroup(); // Workspaces.
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::applyHwSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::applyHwSettings(int workspaceIndex)
{
	SettingsPage::settings().beginGroup("HW");

	for (unsigned int hierarchyIndex = 0; hierarchyIndex < KactusAttribute::KTS_PRODHIER_COUNT; ++hierarchyIndex)
	{
    	KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(hierarchyIndex);
	    SettingsPage::settings().beginGroup(KactusAttribute::valueToString(val));

    	QStringList hwCheckboxNames = SettingsPage::settings().childKeys();
	    for (int checkBoxIndex = 0; checkBoxIndex < 
		    workspaceHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).size();  ++checkBoxIndex)
    	{
	    	// hwCheckboxNames contains the child keys of hierarchies.
		    // wsHwCheckBoxes_ has keys in the same order as hierachyKeys (see loadSettings function).
    		if (hwCheckboxNames.size() == workspaceHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).size())
	    	{
		    	SettingsPage::settings().setValue(hwCheckboxNames.at(checkBoxIndex), 
			    	workspaceHwCheckBoxes_.at(workspaceIndex).at(hierarchyIndex).at(checkBoxIndex)->isChecked());
    		}
	    }
    	SettingsPage::settings().endGroup(); // hierarchy group.
	}

	SettingsPage::settings().endGroup(); // HW.
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::applySwSettings()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::applySwSettings(int workspaceIndex)
{
	SettingsPage::settings().beginGroup("SW");

	QStringList swCheckboxNames = SettingsPage::settings().childKeys();

	for (int checkBoxIndex = 0; checkBoxIndex < workspaceSwCheckBoxes_.at(workspaceIndex).size(); ++checkBoxIndex)
	{
		if (swCheckboxNames.size() == workspaceSwCheckBoxes_.at(workspaceIndex).size())
		{
			SettingsPage::settings().setValue(swCheckboxNames.at(checkBoxIndex), 
				workspaceSwCheckBoxes_.at(workspaceIndex).at(checkBoxIndex)->isChecked());
		}
	}

	SettingsPage::settings().endGroup(); // SW
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHwHorizonSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHwHorizonSelectAll(int rowIndex)
{
	bool checkBoxChecked = false;

	for (int columnIndex = 0; columnIndex < workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).size(); 
		++columnIndex)
	{
		if (!workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->isChecked())
		{
			checkBoxChecked = true;
			break;
		}
	}

	for (int columnIndex = 0; columnIndex < workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).size(); 
		++columnIndex)
	{
		workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->
			setChecked(checkBoxChecked);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onHwVerticalSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onHwVerticalSelectAll(int columnIndex)
{
	bool checkBoxChecked = false;

	for (int rowIndex = 0; rowIndex < workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).size();
		++rowIndex)
	{
		if (!workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->isChecked())
		{
			checkBoxChecked = true;
			break;
		}
	}

	for (int rowIndex = 0; rowIndex < workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).size();
		++rowIndex)
	{
		workspaceHwCheckBoxes_.at(currentWorkspaceIndex_).at(columnIndex).at(rowIndex)->
			setChecked(checkBoxChecked);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onSwHorizonSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onSwHorizonSelectAll(int rowIndex)
{
	if (workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->isChecked())
	{
		workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->setChecked(false);
	}
	else
	{
		workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->setChecked(true);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onSwVerticalSelectAll()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onSwVerticalSelectAll(int columnIndex)
{
	columnIndex = 0;

	bool checkBoxChecked = false;

	for (int rowIndex = 0; rowIndex < workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++rowIndex)
	{
		if (!workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->isChecked())
		{
			checkBoxChecked = true;
			break;
		}
	}

	for (int rowIndex = 0; rowIndex < workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).size(); ++rowIndex)
	{
		workspaceSwCheckBoxes_.at(currentWorkspaceIndex_).at(rowIndex)->setChecked(checkBoxChecked);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::setupLayout()
{
	SettingsPage::settings().beginGroup("Workspaces");

	QString currentWorkspaceName = SettingsPage::settings().value("CurrentWorkspace").toString();
	QStringList workspaceNames = SettingsPage::settings().childGroups();

	SettingsPage::settings().endGroup(); // Workspaces

	QStackedWidget* workspaces = createWorkspacePages(currentWorkspaceName, workspaceNames);

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

	onWorkspaceChanged(workspaceNames.indexOf(currentWorkspaceName));
	workspaceCombo->setCurrentIndex(currentWorkspaceIndex_);
	workspaces->setCurrentIndex(currentWorkspaceIndex_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::createWorkspacePages()
//-----------------------------------------------------------------------------
QStackedWidget* ComponentEditorSettingsPage::createWorkspacePages(QString currentWorkspaceName, 
	QStringList workspaceNames)
{
	SettingsPage::settings().beginGroup("Workspaces");

	SettingsPage::settings().beginGroup(currentWorkspaceName + "/ComponentEditorFilters/HW/Flat");
	QStringList hwCheckBoxNames = SettingsPage::settings().childKeys();
	SettingsPage::settings().endGroup(); // workspaceName/ComponentEditorFilters/HW

	SettingsPage::settings().beginGroup(currentWorkspaceName + "/ComponentEditorFilters/SW");
	QStringList swCheckBoxNames = SettingsPage::settings().childKeys();
	SettingsPage::settings().endGroup(); // workspaceName/ComponentEditorFilters/SW

	SettingsPage::settings().endGroup(); // Workspaces
	
	QStringList hierarchyNames = getHierarchyNames();
	hwCheckBoxNames = changeNameSeparators(hwCheckBoxNames);
	swCheckBoxNames = changeNameSeparators(swCheckBoxNames);
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

	return (workspaces);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::getHierarchyNames()
//-----------------------------------------------------------------------------
QStringList ComponentEditorSettingsPage::getHierarchyNames()
{
	QStringList hierarchyNames;

	for (unsigned int hierarchyIndex = 0; hierarchyIndex < KactusAttribute::KTS_PRODHIER_COUNT; ++hierarchyIndex)
	{
		KactusAttribute::ProductHierarchy val = static_cast<KactusAttribute::ProductHierarchy>(hierarchyIndex);
		hierarchyNames.append(KactusAttribute::valueToString(val));
	}

	return hierarchyNames;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::changeNameSeparators()
//-----------------------------------------------------------------------------
QStringList ComponentEditorSettingsPage::changeNameSeparators(QStringList checkBoxNames)
{
	QStringList newNames;

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

	QList <QList <QCheckBox*> > hwCheckBoxes = workspaceHwCheckBoxes_.at(workspaceIndex);

	for (int columnIndex = 0; columnIndex <hwCheckBoxes.size(); ++columnIndex)
	{
		for (int rowIndex = 0; rowIndex < hwCheckBoxes.at(columnIndex).size(); ++rowIndex)
		{
			table->setCellWidget(rowIndex, columnIndex, 
				centeredCheckBox(hwCheckBoxes.at(columnIndex).at(rowIndex)));
		}
	}

	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setCornerButtonEnabled(false);

	table->setSelectionMode(QAbstractItemView::NoSelection);
	table->horizontalHeader()->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSelectionMode(QAbstractItemView::SingleSelection);

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

	QList <QCheckBox*> swCheckBoxes = workspaceSwCheckBoxes_.at(workspaceIndex);

	for (int rowIndex = 0; rowIndex < swCheckBoxes.size(); ++rowIndex)
	{
		table->setCellWidget(rowIndex, 0, centeredCheckBox(swCheckBoxes.at(rowIndex)));
	}

	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setCornerButtonEnabled(false);

	table->setSelectionMode(QAbstractItemView::NoSelection);
	table->horizontalHeader()->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSwVerticalSelectAll(int)));
	connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onSwHorizonSelectAll(int)));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::centeredCheckBox()
//-----------------------------------------------------------------------------
QWidget* ComponentEditorSettingsPage::centeredCheckBox(QCheckBox* checkBox)
{
	QWidget* checkboxWidget = new QWidget;
	QHBoxLayout* cellLayout = new QHBoxLayout(checkboxWidget);
	cellLayout->addWidget(checkBox);
	cellLayout->setAlignment(Qt::AlignCenter);
	cellLayout->setContentsMargins(0,0,0,0);
	checkboxWidget->setLayout(cellLayout);

	return checkboxWidget;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSettingsPage::onWorkspaceChanged()
//-----------------------------------------------------------------------------
void ComponentEditorSettingsPage::onWorkspaceChanged(int workspaceIndex)
{
	currentWorkspaceIndex_ = workspaceIndex;
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
		QList <QList <QCheckBox*> > hardwareCheckBoxes;

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

		workspaceHwCheckBoxes_.append(hardwareCheckBoxes);

		SettingsPage::settings().endGroup(); // HW.

		SettingsPage::settings().beginGroup("SW");

		workspaceSwCheckBoxes_.append(setCheckBoxes());

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
