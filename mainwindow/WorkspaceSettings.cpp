//-----------------------------------------------------------------------------
// File: Workspace.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.04.2023
//
// Description:
// Stores and loads all workspace related settings for the application.
//-----------------------------------------------------------------------------

#include "WorkspaceSettings.h"

#include "NewWorkspaceDialog.h"
#include "DeleteWorkspaceDialog.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <editors/ComponentEditor/componenteditor.h>

#include <QActionGroup>
#include <QSettings>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: Workspace::saveSettings()
//-----------------------------------------------------------------------------
WorkspaceSettings::WorkspaceSettings(QMainWindow* mainWindow, DockWidgetHandler* docks): 
    QObject(mainWindow),
    mainWindow_(mainWindow),
    docks_(docks)
{

}

//-----------------------------------------------------------------------------
// Function: Workspace::getCurrentWorkspace()
//-----------------------------------------------------------------------------
QString WorkspaceSettings::getCurrentWorkspace() const
{
    return activeWorkspace_;
}

//-----------------------------------------------------------------------------
// Function: Workspace::onNewWorkspace()
//-----------------------------------------------------------------------------
QStringList WorkspaceSettings::getWorkspaces() const
{
    QSettings settings;
    settings.beginGroup("Workspaces");
    QStringList workspaceIDs = settings.childGroups();
    settings.endGroup();

    return workspaceIDs;
}

//-----------------------------------------------------------------------------
// Function: Workspace::saveSettings()
//-----------------------------------------------------------------------------
void WorkspaceSettings::saveSettings() const
{
    QSettings settings;

    // Save the active workspace.
    settings.setValue("Workspaces/CurrentWorkspace", activeWorkspace_);
    saveWorkspace(activeWorkspace_);
}

//-----------------------------------------------------------------------------
// Function: Workspace::restoreSettings()
//-----------------------------------------------------------------------------
void WorkspaceSettings::restoreSettings()
{
    QSettings settings;

    // Create default workspaces if the workspaces registry group is not found.
    settings.beginGroup("Workspaces");
    bool createDefaultWorkspaces = settings.childGroups().isEmpty();
    settings.endGroup();

    if (createDefaultWorkspaces)
    {
        docks_->loadFilterSettings(settings);

        createNewWorkspace("Default");
        createNewWorkspace("Design");
    }

    // Load the active workspace.
    activeWorkspace_ = settings.value("Workspaces/CurrentWorkspace", QString("Default")).toString();
    loadWorkspace(activeWorkspace_);
}

//-----------------------------------------------------------------------------
// Function: Workspace::saveWorkspace()
//-----------------------------------------------------------------------------
void WorkspaceSettings::saveWorkspace(QString const& workspaceName) const
{
    QSettings settings;

    // Create the registry group name.
    QString keyName = "Workspaces/" + workspaceName;

    // Save the geometry and state of windows.
    settings.beginGroup(keyName);

    settings.setValue("WindowState", mainWindow_->saveState());
    settings.setValue("Geometry", mainWindow_->saveGeometry());
    settings.setValue("WindowPosition", mainWindow_->pos());

    docks_->saveFilterSettings(settings);
    docks_->saveVisibilitySettings(settings);

    settings.endGroup(); // Workspaces/workspaceName
}

//-----------------------------------------------------------------------------
// Function: Workspace::loadWorkspace()
//-----------------------------------------------------------------------------
void WorkspaceSettings::loadWorkspace(QString const& workspaceName)
{
    QSettings settings;

    // Create the registry key name.
    QString keyName = "Workspaces/" + workspaceName;
    settings.beginGroup(keyName);

    // Set the window to normal state (this fixed a weird error with window state).
    mainWindow_->setWindowState(Qt::WindowNoState);

    if (settings.contains("WindowPosition"))
    {
        mainWindow_->move(settings.value("WindowPosition").toPoint());
    }

    if (settings.contains("Geometry"))
    {
        mainWindow_->restoreGeometry(settings.value("Geometry").toByteArray());
    }

    if (settings.contains("WindowState"))
    {
        mainWindow_->restoreState(settings.value("WindowState").toByteArray());
    }

    docks_->loadVisiblities(settings);
    docks_->loadFilterSettings(settings);

    settings.setValue("Workspaces/CurrentWorkspace", workspaceName);
    activeWorkspace_ =  workspaceName;
    emit workspaceChanged(activeWorkspace_);
}

//-----------------------------------------------------------------------------
// Function: Workspace::createNewWorkspace()
//-----------------------------------------------------------------------------
void WorkspaceSettings::createNewWorkspace(QString const& workspaceName) const
{
    saveWorkspace(workspaceName);

    QString workspacePath = "Workspaces/" + workspaceName;

    QSettings settings;

    settings.beginGroup(workspacePath);
    settings.beginGroup("ComponentEditorFilters");
    settings.beginGroup("HW");
    for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
    {
        auto hierarchy = static_cast<KactusAttribute::ProductHierarchy>(i);

        settings.beginGroup(KactusAttribute::hierarchyToString(hierarchy));

        for (QString const& itemName : ComponentEditor::getHwItemNames())
        {
            settings.setValue(itemName, true);
        }

        settings.endGroup(); // hierarchy
    }
    settings.endGroup(); // HW

    settings.beginGroup("SW");
    for (QString const& itemName : ComponentEditor::getSwItemNames())
    {
        settings.setValue(itemName, true);
    }
    settings.endGroup(); // SW

    settings.endGroup(); // ComponentEditorFilters
    settings.endGroup(); // Workspaces/workspaceName
}

//-----------------------------------------------------------------------------
// Function: Workspace::updateWorkspaceMenu()
//-----------------------------------------------------------------------------
void WorkspaceSettings::updateWorkspaceMenu(QMenu* workspaceMenu) const
{
    // Create the workspace menu based on the settings.
    workspaceMenu->clear();

    QString currentWorkspace = getCurrentWorkspace();

    QActionGroup* workspaceGroup = new QActionGroup(mainWindow_);
    workspaceGroup->setExclusive(true);
    for (QString const& workspaceName : getWorkspaces())
    {
        QAction* action = new QAction(workspaceName, mainWindow_);
        action->setCheckable(true);
        action->setChecked(workspaceName == currentWorkspace);

        workspaceGroup->addAction(action);
        workspaceMenu->addAction(action);
    }

    connect(workspaceGroup, SIGNAL(triggered(QAction*)), this, SLOT(onWorkspaceChanged(QAction*)));

    // Add actions for creating and deleting new workspaces.
    auto addAction = new QAction(tr("New Workspace..."), mainWindow_);
    connect(addAction, SIGNAL(triggered()), this, SLOT(onNewWorkspace()), Qt::UniqueConnection);

    auto deleteAction = new QAction(tr("Delete Workspace..."), mainWindow_);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onDeleteWorkspace()), Qt::UniqueConnection);

    workspaceMenu->addSeparator();
    workspaceMenu->addAction(addAction);
    workspaceMenu->addAction(deleteAction);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onWorkspaceChanged()
//-----------------------------------------------------------------------------
void WorkspaceSettings::onWorkspaceChanged(QAction* action)
{
    QString workspaceName = action->text();

    saveWorkspace(activeWorkspace_);
    loadWorkspace(workspaceName);
}

//-----------------------------------------------------------------------------
// Function: Workspace::onNewWorkspace()
//-----------------------------------------------------------------------------
void WorkspaceSettings::onNewWorkspace()
{
    NewWorkspaceDialog saveDialog(mainWindow_);

    if (saveDialog.exec() == QDialog::Accepted)
    {
        saveWorkspace(activeWorkspace_);

        QString newWorkspace = saveDialog.name();

        createNewWorkspace(newWorkspace);
        copyComponentEditorSettings(newWorkspace);
        saveWorkspace(newWorkspace);

        loadWorkspace(newWorkspace);

        emit requestMenuUpdate();
    }
}

//-----------------------------------------------------------------------------
// Function: Workspace::onDeleteWorkspace()
//-----------------------------------------------------------------------------
void WorkspaceSettings::onDeleteWorkspace()
{
    DeleteWorkspaceDialog saveDialog(mainWindow_);

    // Fill in the dialog with existing workspace names.
    for (QString const& workspace : getWorkspaces())
    {
        if (workspace != "Default" && workspace != activeWorkspace_)
        {
            saveDialog.addWorkspaceName(workspace);
        }
    }

    // Execute the dialog.
    if (saveDialog.exec() == QDialog::Accepted)
    {
        QSettings settings;
        settings.remove("Workspaces/" + saveDialog.name());
        emit requestMenuUpdate();
    }
}

//-----------------------------------------------------------------------------
// Function: Workspace::copyComponentEditorSettings()
//-----------------------------------------------------------------------------
void WorkspaceSettings::copyComponentEditorSettings(QString const& workspaceName) const
{
    QSettings settings;
    settings.beginGroup("Workspaces");

    QString sourceWorkspacePath = activeWorkspace_ + "/ComponentEditorFilters/HW/";
    QString targetWorkspacePath = workspaceName + "/ComponentEditorFilters/HW/";

    for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
    {
        auto hierarchy = static_cast<KactusAttribute::ProductHierarchy>(i);
        QString hwHierarchyName(KactusAttribute::hierarchyToString(hierarchy) + "/");

        for (QString const& name : ComponentEditor::getHwItemNames())
        {
            settings.setValue(targetWorkspacePath + hwHierarchyName + name, 
                settings.value(sourceWorkspacePath + hwHierarchyName + name).toBool());
        }
    }

    sourceWorkspacePath = activeWorkspace_ + "/ComponentEditorFilters/SW/";
    targetWorkspacePath = workspaceName + "/ComponentEditorFilters/SW/";

    for (QString const& name : ComponentEditor::getSwItemNames())
    {
        settings.setValue(targetWorkspacePath + name, settings.value(sourceWorkspacePath + name).toBool());
    }

    settings.endGroup(); // Workspaces
}
