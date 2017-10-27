//-----------------------------------------------------------------------------
// File: FileDependencySourceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Editor for changing source directories.
//-----------------------------------------------------------------------------

#include "FileDependencySourceEditor.h"

#include <common/views/EditableListView/editablelistview.h>

#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::FileDependencySourceEditor()
//-----------------------------------------------------------------------------
FileDependencySourceEditor::FileDependencySourceEditor(QString const& basePath, QStringList const& sourceDirs,
                                                       QWidget* parent):
DirListManager(tr("File set source directories"), basePath, parent),
    basePath_(basePath)
{   
    if (!basePath_.endsWith(QLatin1Char('/')))
    {
        basePath_.append(QLatin1Char('/'));
    }

    initialize(sourceDirs);
}
    
//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::~FileDependencySourceEditor()
//-----------------------------------------------------------------------------
FileDependencySourceEditor::~FileDependencySourceEditor()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList FileDependencySourceEditor::getSourceDirectories() const
{
    QStringList directories = items();
    directories.removeAll(QString());

    return directories;   
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::initialize()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::initialize(QStringList const& items)
{
    DirListManager::initialize(items);

    disconnect(view_, SIGNAL(addItem(QModelIndex const&)), model_, SLOT(addItem(QModelIndex const&)));
    connect(view_, SIGNAL(addItem(QModelIndex const&)), this, SLOT(addSource()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::addSource()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::addSource()
{
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), basePath_);

    if (newDirectory.isEmpty())
    {
        return;
    }

    newDirectory = QFileInfo(newDirectory).filePath();
    newDirectory = General::getRelativePath(basePath_, newDirectory);

    if (newDirectory.isEmpty())
    {
        newDirectory = ".";
    }

    if (!checkIfSelectedDirectoryHasBeenPreviouslyAdded(newDirectory))
    {
        removeUnnecessaryDirectories(newDirectory);

        QStringList newDirectories = items();
        newDirectories.push_back(newDirectory);
        setItems(newDirectories);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------------------
// Function: FileDependencySourceEditor::checkIfSelectedDirectoryHasBeenPreviouslyAdded()
//-----------------------------------------------------------------------------------------
bool FileDependencySourceEditor::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString const& newDirectory)
{
    QStringList oldDirectoriesAbs = getAbsoluteDirectories();
    oldDirectoriesAbs.removeAll(QString());

    QString newDirAbs = QFileInfo(General::getAbsolutePath(basePath_, newDirectory)).canonicalFilePath();
    
    // Check if the selected directory is part of any existing directory.
    for (int i = 0; i < oldDirectoriesAbs.count(); ++i)
    {
        if (newDirAbs.startsWith(oldDirectoriesAbs.at(i)))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::removeUnnecessaryDirectories()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::removeUnnecessaryDirectories(QString const& newDirectory)
{ 
    QString newDirAbs = QFileInfo(General::getAbsolutePath(basePath_, newDirectory)).canonicalFilePath();
    if (newDirAbs.isEmpty())
    {
        return;
    }

    // First change everything to absolute paths.
    QStringList oldDirectoriesAbs =  getAbsoluteDirectories();

    QStringList tempDirectoryList;

    // Checking if unnecessary directories exist in the list.
    for (int i = 0; i < oldDirectoriesAbs.count(); ++i)
    {
        if (!oldDirectoriesAbs.at(i).startsWith(newDirAbs))
        {
            tempDirectoryList.append(items().at(i));
        }
    }

    // Update the source directory model.
    setItems(tempDirectoryList);
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::getAbsoluteDirectories()
//-----------------------------------------------------------------------------
QStringList FileDependencySourceEditor::getAbsoluteDirectories()
{
    QStringList directories;

    foreach (QString const& directory, items())
    {
        QString absolutePath = General::getAbsolutePath(basePath_, directory);
        directories.push_back(QFileInfo(absolutePath).canonicalFilePath());     
    }

    return directories;
}
