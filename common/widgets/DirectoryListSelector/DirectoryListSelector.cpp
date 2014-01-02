//-----------------------------------------------------------------------------
// File: DirectoryListSelector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 2.1.2014
//
// Description:
// Editor for selecting multiple directories.
//-----------------------------------------------------------------------------

#include "DirectoryListSelector.h"

#include <models/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::DirectoryListSelector()
//-----------------------------------------------------------------------------
DirectoryListSelector::DirectoryListSelector(QString const& basePath,
                                             QStringList const& initialDirs,                                             
                                             QWidget* parent)
    : QGroupBox(tr("Directories"), parent),
    buttonAdd_(new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this)),
    buttonRemove_(new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this)),
    directoryListModel_(new QStringListModel(initialDirs)),
    directoryListView_(new QListView(this)),
    basePath_(QFileInfo(basePath).canonicalFilePath())
{
    directoryListView_->setModel(directoryListModel_);
   
    if (initialDirs.count() > 0)
    {
        buttonRemove_->setEnabled(true);
    }
    else
    {
        buttonRemove_->setEnabled(false);
    }

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addDirectory()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeDirectory()));

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::~DirectoryListSelector()
//-----------------------------------------------------------------------------
DirectoryListSelector::~DirectoryListSelector()
{
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList DirectoryListSelector::getDirectories() const
{
    return directoryListModel_->stringList();   
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::addDirectory()
//-----------------------------------------------------------------------------
void DirectoryListSelector::addDirectory()
{
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), basePath_);

    if (newDirectory.size() < 1)
    {
        return;
    }

    newDirectory = QFileInfo(newDirectory).filePath();    
    QString absDirPath = newDirectory;

    // Use relative path for subdirectories of base path.
    if (!basePath_.isEmpty() && newDirectory.startsWith(basePath_))
    {
        newDirectory = General::getRelativePath(basePath_, newDirectory);
    }

    if (newDirectory.size() < 1)
    {
        newDirectory = ".";
    }

    // Check that new directory does not overlap with existing.
    QStringList directories = directoryListModel_->stringList();
    foreach(QString directory, directories)
    {
        if (QFileInfo(directory).isRelative())
        {
            directory = QFileInfo(directory).absolutePath() + "/" + directory;
            directory = QFileInfo(directory).canonicalFilePath();
        }

        if (QString::compare(directory, absDirPath) == 0)
        {
            return;
        }
    }

    // Add the new directory to the list.
    directories.push_back(newDirectory);
    directoryListModel_->setStringList(directories);
    buttonRemove_->setEnabled(true);
    emit contentChanged();    
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::removeDirectory()
//-----------------------------------------------------------------------------
void DirectoryListSelector::removeDirectory()
{
    directoryListModel_->removeRow(directoryListView_->selectionModel()->currentIndex().row());

    if (directoryListModel_->stringList().count() < 1)
    {
        buttonRemove_->setEnabled(false);
    }
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::setupLayout()
//-----------------------------------------------------------------------------
void DirectoryListSelector::setupLayout()
{
    QDialogButtonBox* addRemoveButtonBox = new QDialogButtonBox(Qt::Vertical);
    addRemoveButtonBox->addButton(buttonAdd_, QDialogButtonBox::ActionRole);
    addRemoveButtonBox->addButton(buttonRemove_, QDialogButtonBox::ActionRole);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(directoryListView_);
    topLayout->addWidget(addRemoveButtonBox);
}