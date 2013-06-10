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

#include <models/generaldeclarations.h>

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
bool FileDependencySourceEditor::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString newDirectory)
{
    QStringList oldDirectoriesAbs;
    for(int i = 0; i < directoryListModel_->stringList().count(); ++i)
    {
        oldDirectoriesAbs.push_back(General::getAbsolutePath(basePath_ + "/", directoryListModel_->stringList().at(i)));
    }
    QString newDirAbs = General::getAbsolutePath(basePath_ + "/", newDirectory);
    // Checking if the selected directory has been previously added.
    for(int i = 0; i < oldDirectoriesAbs.count(); ++i)
    {
        int subDirectory = 0;
        for(int j = 0; j < newDirAbs.count("/"); ++j)
        {
            subDirectory = newDirAbs.indexOf("/", subDirectory+1);
            // Checking whether old directory is a root directory.
            QString oldDirString = oldDirectoriesAbs.at(i);
            if (oldDirString.right(1) == "/")
            {
                oldDirString = oldDirString.left(oldDirString.size() -1);
            }
            if (newDirAbs.left(subDirectory) == oldDirString)
            {
                return true;
            }
            else if (newDirAbs == oldDirectoriesAbs.at(i))
            {
                return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceEditor::removeUnnecessaryDirectories()
//-----------------------------------------------------------------------------
void FileDependencySourceEditor::removeUnnecessaryDirectories(QString newDirectory)
{
    // First changing everything to absolute paths.
    QStringList oldDirectoriesAbs;
    for(int i = 0; i < directoryListModel_->stringList().count(); ++i)
    {
        oldDirectoriesAbs.push_back(General::getAbsolutePath(basePath_ + "/", directoryListModel_->stringList().at(i)));
    }
    QString newDirAbs = General::getAbsolutePath(basePath_ + "/", newDirectory);

    // Temporary directory list for holding necessary directories.
    QStringList tempDirectoryList;
    bool necessaryDirectory = true;

    // Checking if root directory
    if (newDirAbs.right(2) == ":/")
    {
        // Removing unnecessary forward slash.
        newDirAbs = newDirAbs.left(newDirectory.size() -1);
    }

    // Checking if unnecessary directories exist in the list.
    for(int i = 0; i < oldDirectoriesAbs.count(); ++i)
    {
        int subDirectory = 0;
        necessaryDirectory = true;
        QString oldDirectory = oldDirectoriesAbs.at(i);
        for(int j = 0; j < oldDirectoriesAbs.at(i).count("/"); ++j)
        {
            subDirectory = oldDirectoriesAbs.at(i).indexOf("/", subDirectory+1);
            // Checking to see if old directory is contained in the new directory.

            if (newDirAbs == oldDirectory.left(subDirectory))
            {
                // Unnecessary directory found. Marking it unnecessary.
                necessaryDirectory = false;
                break;
            }
        }
        // Checking to see if directory is needed after the new added directory
        if (necessaryDirectory)
        {
            QString necessaryDir = General::getRelativePath(basePath_, oldDirectoriesAbs.at(i));
            tempDirectoryList.push_back(necessaryDir);
        }
    }
    // Updating the source directory model.
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
