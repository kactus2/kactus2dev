//-----------------------------------------------------------------------------
// File: FileDependencySourceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Tommi Korhonen
// Date: 22.01.2013
//
// Description:
// Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------

#include "FileDependencySourceDialog.h"
#include <models/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::FileDependencySourceDialog(QString const& basePath,
                                                       QStringList const& sourceDirs,
                                                       QWidget* parent)
    : QDialog(parent)
{
    basePath_ = basePath;
    setWindowTitle(tr("Import Sources"));

    mainGroupBox_ = new QGroupBox(tr("Directories"), this);
    buttonAdd_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    buttonRemove_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    buttonRemove_->setEnabled(false);

    directoryListView_ = new QListView(this);

    horizontalGroupBoxLayout_ = new QHBoxLayout;
    verizontalMainLayout_ = new QVBoxLayout;

    QDialogButtonBox* addRemoveButtonBox = new QDialogButtonBox(Qt::Vertical);
    addRemoveButtonBox->addButton(buttonAdd_, QDialogButtonBox::ActionRole);
    addRemoveButtonBox->addButton(buttonRemove_, QDialogButtonBox::ActionRole);

    horizontalGroupBoxLayout_->addWidget(directoryListView_);
    horizontalGroupBoxLayout_->addWidget(addRemoveButtonBox);
    mainGroupBox_->setLayout(horizontalGroupBoxLayout_);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    verizontalMainLayout_->addWidget(mainGroupBox_);
    verizontalMainLayout_->addWidget(buttonBox);

    this->setLayout(verizontalMainLayout_);

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addSource()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeSource()));
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()), Qt::UniqueConnection);

    if (sourceDirs.count() > 0)
    {
        buttonRemove_->setEnabled(true);
    }

    directoryListModel_ = new QStringListModel(sourceDirs);
    directoryListView_->setModel(directoryListModel_);
    resize(600, sizeHint().height());

}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::~FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::~FileDependencySourceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList FileDependencySourceDialog::getSourceDirectories() const
{
    return directoryListModel_->stringList();   
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::addSource()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::addSource()
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
        newDirectory = "./";
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
// Function: FileDependencySourceDialog::checkIfSelectedDirectoryHasBeenPreviouslyAdded()
//-----------------------------------------------------------------------------------------
bool FileDependencySourceDialog::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString newDirectory)
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
// Function: FileDependencySourceDialog::removeUnnecessaryDirectories()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::removeUnnecessaryDirectories(QString newDirectory)
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
// Function: FileDependencySourceDialog::removeSource()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::removeSource()
{
    directoryListModel_->removeRow(directoryListView_->selectionModel()->currentIndex().row());
    if (directoryListModel_->stringList().count() < 1)
    {
        buttonRemove_->setEnabled(false);
    }
}
