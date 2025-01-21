//-----------------------------------------------------------------------------
// File: Workspace.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.04.2023
//
// Description:
// Stores and loads all workspace related settings for the application.
//-----------------------------------------------------------------------------

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QAction>
#include <QMainWindow>

#include "DockWidgetHandler.h"

//-----------------------------------------------------------------------------
//! Workspace class
//-----------------------------------------------------------------------------
class WorkspaceSettings : public QObject
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] mainWindow	The mainwindow of the application.
     *    @param [in] docks		The DockWidgetHandler which holds the visibility and filter settings.
	 * 
	 */
	WorkspaceSettings(QMainWindow* mainWindow, DockWidgetHandler* docks);

	//! The destructor.
	virtual ~WorkspaceSettings() = default;

    // Disable copying.
	WorkspaceSettings(WorkspaceSettings const& rhs) = delete;
	WorkspaceSettings& operator=(WorkspaceSettings const& rhs) = delete;

	//! Gets the currently active workspace name.
    QString getCurrentWorkspace() const;

	//! Gets the list of available workspace names.
    QStringList getWorkspaces() const;

	/*!
	 *  Stores the workspace settings.
	 */
	void saveSettings() const;

	/*!
	 *  Restores the workspace settings.
	*/
	void restoreSettings();

	/*!
	 *  Saves the settings for the given workspace.
	 *
	 *    @param [in] workspaceName The name of the workspace.
	 */
	void saveWorkspace(QString const& workspaceName) const;

	/*!
	 *  Loads the settings for the given workspace and sets it as active workspace.
	 *
	 *    @param [in] workspaceName The name of the workspace.
	 */
	void loadWorkspace(QString const& workspaceName);

	/*!
	 *  Creates a new workspace.
	 *
	 *    @param [in] workspaceName   The name of the new workspace.
	 */
	void createNewWorkspace(QString const& workspaceName) const;

    void updateWorkspaceMenu(QMenu* workspaceMenu) const;

signals:

	//! Emitted when workspace menu needs to be updated.
	void requestMenuUpdate();

	//! Emitted when workspace has been changed.
	void workspaceChanged(QString const& workspace);

private slots:

	/*!
	 *  Changes the active workspace.
	 */
    void onWorkspaceChanged(QAction* action);

	/*!
	 *  Creates a new workspace, requesting a name for the workspace from the user using a dialog.
	 */
	void onNewWorkspace();

	/*!
	 *  Deletes a workspace, asking the user which workspace to delete using a dialog.
	 */
	void onDeleteWorkspace();

private:

	/*!
	 *  Copy the component editor settings from the current workspace.
	 *
	 *    @param [in] workspaceName   The name of the new workspace.
	 */
	void copyComponentEditorSettings(QString const& workspaceName) const;

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	//! The main window of the application.
	QMainWindow* mainWindow_;

	//! DockWidgetHandler which holds the visibility and filter settings.
	DockWidgetHandler* docks_;

	//! The name of the currently active workspace.
	QString activeWorkspace_ = QString("Default");

};

//-----------------------------------------------------------------------------

#endif // WORKSPACE_H
