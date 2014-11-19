//-----------------------------------------------------------------------------
// File: ComponentEditorSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.11.2014
//
// Description:
// Component editor property settings page.
//-----------------------------------------------------------------------------
#ifndef COMPONENTEDITORSETTINGSPAGE_H
#define COMPONENTEDITORSETTINGSPAGE_H

#include "SettingsPage.h"
#include "ComponentEditorSettingsPage.h"

#include <QCheckBox>
#include <QTableWidget>
#include <QStackedWidget>

class ComponentEditorSettingsPage : public SettingsPage
{
	Q_OBJECT

public:
	ComponentEditorSettingsPage(QSettings &settings);
	
	~ComponentEditorSettingsPage();

	/*!
	 *  Applies the changes that were done to the page.
	 */
	void apply();

public slots:

	/*!
	 *  Change the workspace.
	 *
	 *      @param [in] workspaceIndex   The index value of the current workspace.
	 */
	void onWorkspaceChanged(int workspaceIndex);

	/*!
	 *  Select all hardware check boxes from the selected row.
	 *
	 *      @param [in] rowIndex   The selected row.
	 */
	void onHwHorizonSelectAll(int rowIndex);

	/*!
	 *  Select all hardware check boxes from the selected column.
	 *
	 *      @param [in] columnIndex   The selected column.
	 */
	void onHwVerticalSelectAll(int columnIndex);

	/*!
	 *  Select all software check boxes from the selected row.
	 *
	 *      @param [in] rowIndex   The selected row.
	 */
	void onSwHorizonSelectAll(int rowIndex);

	/*!
	 *  Select all software check boxes from the selected column.
	 *
	 *      @param [in] columnIndex   If software would have hierarchy, this selects it.
	 */
	void onSwVerticalSelectAll(int columnIndex);

private:
	//! Disable copying.
	ComponentEditorSettingsPage(ComponentEditorSettingsPage const& rhs);
	ComponentEditorSettingsPage& operator=(ComponentEditorSettingsPage const& rhs);

	/*!
	 *  Set the check box names to correct format.
	 *
	 *      @param [in] checkBoxNames   List of names to be changed.
	 */
	QStringList setCheckBoxNames(QStringList checkBoxNames);

	/*!
	 *  Set the hardware table.
	 *
	 *      @param [in] table               The table.
	 *      @param [in] horizontalHeaders   Horizontal headers for the table.
	 *      @param [in] verticalHeaders     Vertical headers for the table.
	 *      @param [in] workspaceIndex      Current workspace index.
	 */
	void setHwTable(QTableWidget* table, QStringList horizontalHeaders, QStringList verticalHeaders, 
		int workspaceIndex);

	/*!
	 *  Set the software table.
	 *
	 *      @param [in] table               The table.
	 *      @param [in] horizontalHeaders   Horizontal headers for the table.
	 *      @param [in] verticalHeaders     Vertical headers for the table.
	 *      @param [in] workspaceIndex      Current workspace index.
	 */
	void setSwTable(QTableWidget* table, QStringList horizontalHeaders, QStringList verticalHeaders, 
		int workspaceIndex);

	/*!
	 *  Setup the table layout.
	 */
	void setupLayout();

	/*!
	 *  Assemble the layout.
	 *
	 *      @param [in] workspaces             StackedWidget of tables.
	 *      @param [in] workspaceNames         Names of the workspaces.
	 *      @param [in] currentWorkspaceName   Current workspace name.
	 */
	void assembleLayout(QStackedWidget* workspaces, QStringList workspaceNames, QString currentWorkspaceName);

	/*!
	 *  Loads the current settings.
	 */
	void loadSettings();

	/*!
	 *  Set the check boxes to their respective lists.
	 *
	 *      @param [in] checkBoxes   The current usable check box list.
	 */
	QList <QCheckBox*> setCheckBoxes();

	//! All the hardware check boxes used by all the workspaces. Structured as workspace.hierarchy.checkboxes.
	QList <QList <QList <QCheckBox*>>> wsHwCheckBoxes_;

	//! All the software check boxes used by all the workspaces. Structured as workspace.checkboxes.
	QList <QList <QCheckBox*>> wsSwCheckBoxes_;

	//! The current workspace index.
	int currentWorkspaceIndex_;
};

#endif // COMPONENTEDITORSETTINGSPAGE_H
