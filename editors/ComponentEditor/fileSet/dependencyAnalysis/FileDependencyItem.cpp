//-----------------------------------------------------------------------------
// File: FileDependencyItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.02.2013
//
// Description:
// Base class for dependency items.
//-----------------------------------------------------------------------------

#include "FileDependencyItem.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem()
    : parent_(0),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(ITEM_TYPE_ROOT),
      component_(),
      path_(),
      references_(),
      fileRefs_(),
      children_()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem(FileDependencyItem* parent,
                                       QSharedPointer<Component> component, QString const& path,
                                       ItemType type)
    : parent_(parent),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(type),
      component_(component),
      path_(path),
      references_(),
      fileRefs_(),
      children_()
{
    // Allow only folder/location items to be added.
    Q_ASSERT(type != ITEM_TYPE_FILE);
    Q_ASSERT(type != ITEM_TYPE_ROOT);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem(FileDependencyItem* parent,
                                       QSharedPointer<Component> component, QString const& path,
                                       QString const& fileType,
                                       QList<QSharedPointer<File> > const& fileRefs)
    : parent_(parent),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(ITEM_TYPE_FILE),
      component_(component),
      path_(path),
      references_(),
      fileType_(fileType),
      fileRefs_(fileRefs),
      children_()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::~FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::~FileDependencyItem()
{
    foreach (FileDependencyItem* item, children_)
    {
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getChild()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::getChild(int index)
{
    if (index < 0 || index >= getChildCount())
    {
        return 0;
    }

    return children_[index];
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getChild()
//-----------------------------------------------------------------------------
FileDependencyItem const* FileDependencyItem::getChild(int index) const
{
    if (index < 0 || index >= getChildCount())
    {
        return 0;
    }

    return children_[index];
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getChildCount()
//-----------------------------------------------------------------------------
int FileDependencyItem::getChildCount() const
{
    return children_.size();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getParent()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::getParent()
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getIndex()
//-----------------------------------------------------------------------------
int FileDependencyItem::getIndex()
{
    if (parent_ == 0)
    {
        return -1;
    }

    return parent_->children_.indexOf(this);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getStatus()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyStatus FileDependencyItem::getStatus() const
{
    return status_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getType()
//-----------------------------------------------------------------------------
FileDependencyItem::ItemType FileDependencyItem::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getSimplePath()
//-----------------------------------------------------------------------------
QString FileDependencyItem::getDisplayPath() const
{
    if (type_ == ITEM_TYPE_FILE)
    {
        if (isExternal())
        {
            QString filename = path_.mid(path_.indexOf('$', 1) + 2);
            return filename;
        }
        else
        {
            QFileInfo info(path_);
            return info.fileName();
        }
    }

    else if (type_ == ITEM_TYPE_EXTERNAL_LOCATION)
    {
        return tr("External: ") + path_.mid(1, path_.length() - 2) + "/";
    }

    else if (type_ == ITEM_TYPE_UNKNOWN_LOCATION)
    {
        return tr("Unspecified");
    }

    else
    {
        return path_ + "/";
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getFileSets()
//-----------------------------------------------------------------------------
QList<QSharedPointer<FileSet> > FileDependencyItem::getFileSets() const
{
    QList<QSharedPointer<FileSet> > fileSets;

    if (type_ == ITEM_TYPE_FILE)
    {
        foreach (QSharedPointer<File> file, fileRefs_)
        {
            foreach (QSharedPointer<FileSet> singleFileSet, *component_->getFileSets())
            {
                if (!fileSets.contains(singleFileSet) && singleFileSet->getFiles()->contains(file))
                {
                    fileSets.append(singleFileSet);
                }
            }
        }
    }
    else if (type_ != ITEM_TYPE_ROOT)
    {
        // For folders/locations, we must check in what file sets the files belong to.
        // Otherwise determine the folder filesets.
        QMap<QSharedPointer<FileSet>, int> fileSetBuckets;

        for (int i = 0; i < getChildCount(); ++i)
        {
            QList<QSharedPointer<FileSet> > childFileSets = getChild(i)->getFileSets();

            foreach (QSharedPointer<FileSet> fileSet, childFileSets)
            {
                int count = 1;

                if (fileSetBuckets.contains(fileSet))
                {
                    count = fileSetBuckets[fileSet];
                    ++count;

                    fileSetBuckets[fileSet] = count;
                }
                else
                {
                    fileSetBuckets.insert(fileSet, count);
                }

                if (count == getChildCount())
                {
                    fileSets.append(fileSet);
                }
            }
        }
    }

    return fileSets;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::addFile()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::addFile(QSharedPointer<Component> component, QString const& path,
    QString const& fileType, QList<QSharedPointer<File> > const& fileRefs)
{
    FileDependencyItem* item = new FileDependencyItem(this, component, path, fileType, fileRefs);
    
    if (children_.empty() || children_.back()->getPath() < path)
    {
        children_.append(item);
    }
    else
    {
        for (int i = 0; i < children_.size(); ++i)
        {
            if (children_.at(i)->getPath() > path)
            {
                children_.insert(i, item);
                break;
            }
        }
    }

    return item;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::addFolder()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::addFolder(QSharedPointer<Component> component, QString const& path, ItemType type,
                                                  int index)
{
    FileDependencyItem* item = new FileDependencyItem(this, component, path, type);

    if (index == -1)
    {
        children_.append(item);
    }
    else
    {
        children_.insert(index, item);
    }

    return item;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::updateStatus()
//-----------------------------------------------------------------------------
void FileDependencyItem::updateStatus()
{
    status_ = FILE_DEPENDENCY_STATUS_OK;

    foreach (FileDependencyItem* item, children_)
    {
        if (item->getStatus() == FILE_DEPENDENCY_STATUS_CHANGED2)
        {
            status_ = FILE_DEPENDENCY_STATUS_CHANGED2;
        }

        else if (item->getStatus() == FILE_DEPENDENCY_STATUS_CHANGED)
        {
            if (status_ == FILE_DEPENDENCY_STATUS_OK)
            {
                status_ = FILE_DEPENDENCY_STATUS_CHANGED;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileDependencyItem::getFileTypes() const
{
    QStringList list;

    foreach (QSharedPointer<File> file, fileRefs_)
    {
        foreach (QString const& type, *file->getFileTypes())
        {
            if (!list.contains(type))
            {
                list.append(type);
            }
        }
    }

    if (list.empty() && fileType_ != "")
    {
        list.append(fileType_);
    }

    return list;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getName()
//-----------------------------------------------------------------------------
QString FileDependencyItem::getPath() const
{
    return path_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::setStatus()
//-----------------------------------------------------------------------------
void FileDependencyItem::setStatus(FileDependencyStatus status)
{
    status_ = status;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getLastHash()
//-----------------------------------------------------------------------------
QString FileDependencyItem::getLastHash() const
{
    if (fileRefs_.empty())
    {
        return QString();
    }

    return fileRefs_.first()->getLastHash();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::setLastHash()
//-----------------------------------------------------------------------------
void FileDependencyItem::setLastHash(QString const& hash)
{
    foreach (QSharedPointer<File> file, fileRefs_)
    {
        file->setPendingHash(hash);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::isExternal()
//-----------------------------------------------------------------------------
bool FileDependencyItem::isExternal() const
{
    if (type_ == ITEM_TYPE_FILE)
    {
        return (parent_->type_ != ITEM_TYPE_FOLDER);
    }
    else
    {
        return (type_ != ITEM_TYPE_FOLDER);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::insertItem()
//-----------------------------------------------------------------------------
void FileDependencyItem::insertItem(FileDependencyItem* item)
{
    Q_ASSERT(item->parent_ == 0);

    children_.append(item);
    item->parent_ = this;
    item->path_ = path_ + "/" + item->path_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::removeItem()
//-----------------------------------------------------------------------------
void FileDependencyItem::removeItem(FileDependencyItem* item)
{
    Q_ASSERT(item->parent_ != 0);

    children_.removeOne(item);
    item->parent_ = 0;
    item->path_ = item->path_.mid(item->path_.indexOf('$', 1) + 2);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::hasMultipleFileSets()
//-----------------------------------------------------------------------------
bool FileDependencyItem::hasMultipleFileSets() const
{
    if (type_ == ITEM_TYPE_FILE)
    {
        return false;
    }

    // For folders/locations, we must check in what file sets the files belong to.
    QMap<QSharedPointer<FileSet>, int> fileSetBuckets;
    QList<QSharedPointer<FileSet> > fileSets;

    for (int i = 0; i < getChildCount(); ++i)
    {
        QList<QSharedPointer<FileSet> > childFileSets = getChild(i)->getFileSets();

        foreach (QSharedPointer<FileSet> fileSet, childFileSets)
        {
            int count = 1;

            if (fileSetBuckets.contains(fileSet))
            {
                count = fileSetBuckets[fileSet];
                ++count;

                fileSetBuckets[fileSet] = count;
            }
            else
            {
                fileSetBuckets.insert(fileSet, count);
            }

            if (count == getChildCount())
            {
                fileSets.append(fileSet);
            }
        }
    }

    return (fileSets.count() < fileSetBuckets.count());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::setFileSets()
//-----------------------------------------------------------------------------
void FileDependencyItem::setFileSets(QList<QSharedPointer<FileSet> > fileSets, bool preserveMultiple)
{
    Q_ASSERT(type_ != ITEM_TYPE_ROOT);

    if (type_ == ITEM_TYPE_FILE)
    {
        QList<QSharedPointer<File> > newFileRefs;

        foreach (QSharedPointer<FileSet> fileSet, fileSets)
        {
            // Check if the file already belongs to the file set --> no changes required.
            bool found = false;

            for (int i = 0; i < fileRefs_.size(); ++i)
            {
                foreach (QSharedPointer<File> containedFile, *fileSet->getFiles())
                {
                    if (containedFile == fileRefs_.at(i))
                    {
                        // Add the existing file ref to the new file ref list.
                        newFileRefs.append(fileRefs_[i]);

                        fileRefs_.removeAt(i);
                        --i;
                        found = true;
                        break;
                    }
                }
                if (found == true)
                {
                    break;
                }
            }

            if (!found)
            {
                // Otherwise create a new file reference.
                QSharedPointer<File> file(new File(path_));
                if (!fileType_.isEmpty())
                {
                    file->addFileType(fileType_);
                }

                fileSet->addFile(file);

                newFileRefs.append(file);
            }
        }

        // Go through remaining old file refs and remove them.
        foreach (QSharedPointer<File> fileToBeRemoved, fileRefs_)
        {
            foreach (QSharedPointer<FileSet> singleFileSet, *component_->getFileSets())
            {
                foreach (QSharedPointer<File> containedFile, *singleFileSet->getFiles())
                {
                    if (fileToBeRemoved == containedFile)
                    {
                        singleFileSet->removeFile(fileToBeRemoved->name());
                    }
                }
            }
        }

        fileRefs_ = newFileRefs;

        if (fileRefs_.empty())
        {
            //component_->addIgnoredFile(path_);
        }
        else
        {
           // component_->removeIgnoredFile(path_);
        }
    }
    else
    {
        QList<QSharedPointer<FileSet > > oldFolderFileSets = getFileSets();

        for (int i = 0; i < getChildCount(); ++i)
        {
            QList<QSharedPointer<FileSet> > oldFileSets = getChild(i)->getFileSets();
            QList<QSharedPointer<FileSet> > newFileSets = fileSets;

            if (preserveMultiple)
            {
                foreach (QSharedPointer<FileSet> fileSet, oldFileSets)
                {
                    if (!newFileSets.contains(fileSet) &&
                        !oldFolderFileSets.contains(fileSet))
                    {
                        newFileSets.append(fileSet);
                    }
                }
            }

            getChild(i)->setFileSets(newFileSets, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::refreshFileRefs()
//-----------------------------------------------------------------------------
void FileDependencyItem::refreshFileRefs()
{
    fileRefs_.clear();
    foreach (QSharedPointer<FileSet> containingFileSet, *component_->getFileSets())
    {
        foreach (QSharedPointer<File> containedFile, *containingFileSet->getFiles())
        {
            if (containedFile->name() == path_)
            {
                fileRefs_.append(containedFile);
            }
        }
    }
}
