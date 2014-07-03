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

#include <IPXACTmodels/generaldeclarations.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::DirectoryListSelector()
//-----------------------------------------------------------------------------
DirectoryListSelector::DirectoryListSelector(QString const& basePath,
                                             QStringList const& initialDirs,                                             
                                             QWidget* parent) : 
    QWidget(parent),
    buttonAdd_(new QPushButton(QIcon(":/icons/common/graphics/add.png"), QString(), this)),
    buttonRemove_(new QPushButton(QIcon(":/icons/common/graphics/remove.png"), QString(), this)),    
    directoryListView_(new QListView(this)),
    directoryListModel_(new QStringListModel(initialDirs)),
    basePath_(QFileInfo(basePath).canonicalFilePath()),
    persistentDirectories_()
{
    directoryListView_->setModel(directoryListModel_);
   
    buttonRemove_->setEnabled(false); 

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addDirectory()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeDirectory()));
    connect(directoryListView_, SIGNAL(clicked(const QModelIndex&)),
        this, SLOT(onSelectionChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::~DirectoryListSelector()
//-----------------------------------------------------------------------------
DirectoryListSelector::~DirectoryListSelector()
{
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::addPersistentDirectory()
//-----------------------------------------------------------------------------
void DirectoryListSelector::setPersistentDirectory(QString const& directory)
{
    if (!persistentDirectories_.contains(directory))
    {
        persistentDirectories_.append(directory);
    }
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::removePersistentDirectory()
//-----------------------------------------------------------------------------
void DirectoryListSelector::removePersistentDirectory(QString const& directory)
{
    persistentDirectories_.removeAll(directory);
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::getPersistentDirectories()
//-----------------------------------------------------------------------------
QStringList DirectoryListSelector::getPersistentDirectories() const
{
    return persistentDirectories_;
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
    QString newAbsPath = newDirectory;

    // Use relative path for subdirectories of base path.
    // If relative path could not be resolved, use absolute path instead.
    if (!basePath_.isEmpty() && newDirectory.startsWith(basePath_))
    {
        newDirectory = General::getRelativePath(basePath_, newDirectory);
    }    
    if (newDirectory.size() < 1)
    {
        newDirectory = newAbsPath;
    }

    // Check that new directory does not overlap with existing.
    QStringList directories = directoryListModel_->stringList();
    foreach(QString directory, directories)
    {
        if (QFileInfo(directory).isRelative())
        {
            directory = QFileInfo(basePath_ + "/" + directory).canonicalFilePath();
        }

        if (QString::compare(directory, newAbsPath) == 0)
        {
            return;
        }
    }

    // Add the new directory to the list.
    directories.push_back(newDirectory);
    directoryListModel_->setStringList(directories);
    emit contentChanged();    
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::removeDirectory()
//-----------------------------------------------------------------------------
void DirectoryListSelector::removeDirectory()
{    
    QModelIndex index = directoryListView_->selectionModel()->currentIndex();
    
    if (!index.isValid())
    {
        return;
    }

    // Do not allow persistent directories to be removed.
    if (persistentDirectories_.contains(index.data().toString()))
    {
        return;
    }

    directoryListModel_->removeRow(index.row());

    onSelectionChanged();    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DirectoryListSelector::onSelectionChanged()
//-----------------------------------------------------------------------------
void DirectoryListSelector::onSelectionChanged()
{    
    QModelIndex index = directoryListView_->selectionModel()->currentIndex();
    buttonRemove_->setEnabled(index.isValid() && 
        !persistentDirectories_.contains(directoryListModel_->data(index, Qt::DisplayRole).toString()));
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
    topLayout->addWidget(directoryListView_, 1);
    topLayout->addWidget(addRemoveButtonBox);
}
