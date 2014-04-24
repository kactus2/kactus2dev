//-----------------------------------------------------------------------------
// File: FileDependencySourceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Editor for changing source directories.
//-----------------------------------------------------------------------------

#include "FileDependencySourceEditor.h"

#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::FileDependencySourceEditor()
//-----------------------------------------------------------------------------
FileDependencySourceEditor::FileDependencySourceEditor(QString const& basePath,
                                                       QStringList const& sourceDirs,
                                                       QWidget* parent)
    : QGroupBox(tr("Directories"), parent)
{
    basePath_ = basePath;

    buttonAdd_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    buttonRemove_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    buttonRemove_->setEnabled(false);

    directoryListView_ = new QListView(this);

    QDialogButtonBox* addRemoveButtonBox = new QDialogButtonBox(Qt::Vertical);
    addRemoveButtonBox->addButton(buttonAdd_, QDialogButtonBox::ActionRole);
    addRemoveButtonBox->addButton(buttonRemove_, QDialogButtonBox::ActionRole);

    horizontalGroupBoxLayout_ = new QHBoxLayout(this);
    horizontalGroupBoxLayout_->addWidget(directoryListView_);
    horizontalGroupBoxLayout_->addWidget(addRemoveButtonBox);

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addSource()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeSource()));
    
    if (sourceDirs.count() > 0)
    {
        buttonRemove_->setEnabled(true);
    }

    directoryListModel_ = new QStringListModel(sourceDirs);
    directoryListView_->setModel(directoryListModel_);
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
    return directoryListModel_->stringList();   
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::addSource()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::addSource()
{
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), basePath_);

    if (newDirectory.size() < 1)
    {
        return;
    }

    newDirectory = QFileInfo(newDirectory).filePath();
    newDirectory = General::getRelativePath(basePath_, newDirectory);

    if (newDirectory.size() < 1)
    {
        newDirectory = ".";
    }

    if (!checkIfSelectedDirectoryHasBeenPreviouslyAdded(newDirectory))
    {
        // Now removing possibly unnecessary directories
        removeUnnecessaryDirectories(newDirectory);

        // Adding the new directory to the list.
        QStringList newDirectories = directoryListModel_->stringList();
        newDirectories.push_back(newDirectory);
        directoryListModel_->setStringList(newDirectories);
        buttonRemove_->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------------------
// Function: FileDependencySourceEditor::checkIfSelectedDirectoryHasBeenPreviouslyAdded()
//-----------------------------------------------------------------------------------------
bool FileDependencySourceEditor::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString const& newDirectory)
{
    QStringList oldDirectoriesAbs;
    getAbsoluteDirectories(oldDirectoriesAbs);
    
    QString newDirAbs = QFileInfo(General::getAbsolutePath(basePath_ + "/", newDirectory)).canonicalFilePath();

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
    // First change everything to absolute paths.
    QStringList oldDirectoriesAbs;
    getAbsoluteDirectories(oldDirectoriesAbs);
    
    QString newDirAbs = QFileInfo(General::getAbsolutePath(basePath_ + "/", newDirectory)).canonicalFilePath();

    // Temporary directory list for holding necessary directories.
    QStringList tempDirectoryList;

    // Checking if unnecessary directories exist in the list.
    for (int i = 0; i < oldDirectoriesAbs.count(); ++i)
    {
        if (!oldDirectoriesAbs.at(i).startsWith(newDirAbs))
        {
            tempDirectoryList.append(directoryListModel_->stringList().at(i));
        }
    }

    // Update the source directory model.
    directoryListModel_->setStringList(tempDirectoryList);
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::removeSource()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::removeSource()
{
    directoryListModel_->removeRow(directoryListView_->selectionModel()->currentIndex().row());

    if (directoryListModel_->stringList().count() < 1)
    {
        buttonRemove_->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::getAbsoluteDirectories()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::getAbsoluteDirectories(QStringList &oldDirectoriesAbs)
{
    oldDirectoriesAbs.clear();

    for (int i = 0; i < directoryListModel_->stringList().count(); ++i)
    {
        QString path = QFileInfo(General::getAbsolutePath(basePath_ + "/", directoryListModel_->stringList().at(i))).canonicalFilePath();

        oldDirectoriesAbs.push_back(path);
    }
}
