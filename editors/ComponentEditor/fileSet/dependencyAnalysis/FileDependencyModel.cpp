//-----------------------------------------------------------------------------
// File: FileDependencyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.02.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyModel.h"
#include "FileDependencyItem.h"
#include "FileDependencyColumns.h"

#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <QIcon>
#include <QCryptographicHash>

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::FileDependencyModel(PluginManager const& pluginMgr, QSharedPointer<Component> component,
                                         QString const& basePath):
pluginMgr_(pluginMgr),
component_(component),
basePath_(basePath),
root_(new FileDependencyItem()),
unspecifiedLocation_(),
timer_(0),
curFolderIndex_(0),
curFileIndex_(0),
progressValue_(0),
usedPlugins_(),
analyzerPluginMap_(),
dependencies_(component->getPendingFileDependencies())
{
    connect(this, SIGNAL(dependencyAdded(FileDependency*)), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(dependencyChanged(FileDependency*)), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(dependencyRemoved(FileDependency*)), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::~FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::~FileDependencyModel()
{
    root_.clear();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::headerData(int section, Qt::Orientation /*orientation*/,
    int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (section == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_STATUS)
        {
            return tr("Status");
        }
        else if (section == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_PATH)
        {
            return tr("Path");
        }
        else if (section == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_FILESETS)
        {
            return tr("Filesets");
        }
        else if (section == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_CREATE)
        {
            return tr("#");
        }
        else if (section == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_DEPENDENCIES)
        {
            return tr("Dependencies");
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::columnCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::columnCount(const QModelIndex& /*parent = QModelIndex()*/) const
{
    return FileDependencyColumns::FILE_DEPENDENCY_COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::rowCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return static_cast<FileDependencyItem*>(parent.internalPointer())->getChildCount();
    }
    else
    {
        return root_->getChildCount();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::index()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_.data();

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    QSharedPointer<FileDependencyItem> child = parentItem->getChild(row);
    if (child.isNull())
    {
        return QModelIndex();
    }

    return createIndex(row, column, child.data());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::parent()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    FileDependencyItem* childItem = static_cast<FileDependencyItem*>(child.internalPointer());
    FileDependencyItem* parent = childItem->getParent();

    if (parent == 0 || parent == root_)
    {
        return QModelIndex();
    }

    int index = -1;
    foreach (QSharedPointer<FileDependencyItem> childDependency, parent->getChildItems())
    {
        if (childDependency.data() == childItem)
        {
            index = parent->getIndex(childDependency);
        }
    }

    if (index < 0)
    {
        return QModelIndex();
    }

    return createIndex(index, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::setData()
//-----------------------------------------------------------------------------
bool FileDependencyModel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */)
{
    if (!index.isValid())
    {
        return false;
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::EditRole)
    {
        if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_FILESETS)
        {
            QStringList fileSetNames = value.toStringList();

            // Retrieve correct file sets from the component.
            QList<QSharedPointer<FileSet> > fileSets;

            foreach (QString const& name, fileSetNames)
            {
                if (name != "[multiple]")
                {
                    QSharedPointer<FileSet> fileSet = component_->getFileSet(name);
                    fileSets.append(fileSet);
                }
            }

            // Determine whether multiple (colliding) filesets should be preserved.
            bool multiple = fileSetNames.contains("[multiple]");

            item->setFileSets(fileSets, multiple);

            emit dataChanged(index, index);
            emit contentChanged();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::data()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_PATH)
        {
            return item->getDisplayPath();
        }
        else if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_FILESETS)
        {
            QList<QSharedPointer<FileSet> > fileSets = item->getFileSets();
            bool multiple = item->hasMultipleFileSets();

            if (item->isExternal())
            {
                return QVariant();
            }
            else if (fileSets.isEmpty())
            {
                if (multiple)
                {
                    return tr("[multiple]");
                }
                else
                {
                    return tr("[none]");
                }
            }
            else
            {
                QString str = fileSets[0]->name();

                for (int i = 1; i < fileSets.count(); ++i)
                {
                    str += "; " + fileSets[i]->name();
                }

                if (multiple)
                {
                    str += "; [multiple]";
                }

                return str;
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_STATUS)
        {
            if (item->isExternal())
            {
                return QVariant();
            }

            if (item->getStatus() == FILE_DEPENDENCY_STATUS_UNKNOWN)
            {
                return QIcon(":icons/common/graphics/traffic-light_gray.png");
            }
            else if (item->getStatus() == FILE_DEPENDENCY_STATUS_OK)
            {
                return QIcon(":icons/common/graphics/traffic-light_green.png");
            }
            else if (item->getStatus() == FILE_DEPENDENCY_STATUS_CHANGED)
            {
                return QIcon(":icons/common/graphics/traffic-light_yellow.png");
            }
            else if (item->getStatus() == FILE_DEPENDENCY_STATUS_CHANGED2)
            {
                return QIcon(":icons/common/graphics/traffic-light_red.png");
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_STATUS)
        {
            return QSize(16, 16);
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FILE &&
            item->getParent()->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            QString absPath = General::getAbsolutePath(basePath_, item->getPath());
            
            if (!QFileInfo(absPath).exists())
            {
                return QColor(Qt::red);
            }
            else if (item->getFileSets().empty())
            {
                return QColor(Qt::gray);
            }
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FOLDER ||
            item->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION ||
            item->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
        {
            return QColor(230, 230, 230);
        }
        else if (item->getParent()->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            QString absPath = General::getAbsolutePath(basePath_, item->getPath());
            
            if (!QFile(absPath).exists())
            {
                return QColor(255, 215, 215);
            }
        }
        
        if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_CREATE)
        {
            return QColor(240, 240, 240);
        }
    }
    else if (role == Qt::UserRole)
    {
        QStringList fileSetNames;

        if (item->hasMultipleFileSets())
        {
            fileSetNames.append("[multiple]");
        }

        foreach (QSharedPointer<FileSet> fileSet, item->getFileSets())
        {
            fileSetNames.append(fileSet->name());
        }

        return fileSetNames;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::hasChildren()
//-----------------------------------------------------------------------------
bool FileDependencyModel::hasChildren(const QModelIndex& parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_.data();

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    return (parentItem->getChildCount() > 0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileDependencyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_CREATE ||
        index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_DEPENDENCIES)
    {
        return Qt::ItemIsSelectable;
    }

    else if (index.column() == FileDependencyColumns::FILE_DEPENDENCY_COLUMN_FILESETS)
    {
        FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

        if (item->getType() == FileDependencyItem::ITEM_TYPE_FILE || item->getChildCount() > 0)
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }
        else
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }

    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::startAnalysis()
//-----------------------------------------------------------------------------
void FileDependencyModel::startAnalysis()
{
    // Reset state variables.
    curFolderIndex_ = 0;
    curFileIndex_ = 0;
    progressValue_ = 0;
    emit analysisProgressChanged(progressValue_ + 1);

    // Start the analysis timer.
    timer_ = QSharedPointer<QTimer> (new QTimer(this));
    connect(timer_.data(), SIGNAL(timeout()), this, SLOT(performAnalysisStep()));
    timer_->start();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::stopAnalysis()
//-----------------------------------------------------------------------------
void FileDependencyModel::stopAnalysis()
{
    if (timer_ != 0)
    {
        timer_->stop();
        timer_.clear();

        // Reset the progress.
        emit analysisProgressChanged(0);

        // End analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->endAnalysis(component_.data(), basePath_);
        }

        // TODO: Reset dependencies?
        emit dependenciesReset();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addFolder()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependencyItem> FileDependencyModel::addFolder(QString const& path)
{
    if (root_->getChildCount() == 0 || root_->getChild(root_->getChildCount() - 1)->getPath() < path)
    {
        return root_->addFolder(QSharedPointer<Component>(), path);
    }
    else
    {
        for (int i = 0; i < root_->getChildCount(); ++i)
        {
            if (root_->getChild(i)->getPath() > path)
            {
                return root_->addFolder(QSharedPointer<Component>(), path, FileDependencyItem::ITEM_TYPE_FOLDER, i);
            }
        }

        return root_->addFolder(QSharedPointer<Component>(), path);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::beginReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::beginReset()
{
    beginResetModel();

    root_.clear();
    root_ = QSharedPointer<FileDependencyItem>(new FileDependencyItem());

    dependencies_.clear();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::endReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::endReset()
{
    // Add the existing dependencies to the model.
    foreach (QSharedPointer<FileDependency> dependency, component_->getFileDependencies())
    {
        QSharedPointer<FileDependency> copy(new FileDependency(*dependency.data()));
        copy->setStatus(FileDependency::STATUS_UNCHANGED);

        QSharedPointer<FileDependencyItem> fileItem1 = findFileItem(copy->getFile1());
        QSharedPointer<FileDependencyItem> fileItem2 = findFileItem(copy->getFile2());

        // First item should always be valid. Otherwise the dependency should be discarded altogether.
        if (fileItem1 == 0)
        {
            emit dependenciesChanged();
            continue;
        }

        // Check if the second one is an external (not found).
        if (fileItem2 == 0)
        {
            // Extract the name of the external folder.
            int endIndex = copy->getFile2().indexOf('$', 1);

            if (copy->getFile2().at(0) != '$' || endIndex == -1)
            {
                continue;
            }

            // Search for a corresponding folder item.
            QString folderName = copy->getFile2().mid(0, endIndex + 1);
            QSharedPointer<FileDependencyItem> parent = findFolderItem(folderName);

            // Create the folder item if not found.
            if (parent == 0)
            {
                int index = root_->getChildCount();
                FileDependencyItem::ItemType type = FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION;
                
                // Special case for the unspecified external folder.
                if (folderName == "$External$")
                {
                    type = FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION;
                }
                else if (root_->getChildCount() > 0 &&
                         root_->getChild(root_->getChildCount() - 1)->getType() ==
                         FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
                {
                    index = root_->getChildCount() - 1;
                }

                //beginInsertRows(getItemIndex(root_, 0), index, index);
                parent = root_->addFolder(component_, folderName, type, index);
                //endInsertRows();
            }

            //beginInsertRows(getItemIndex(parent, 0), parent->getChildCount(), parent->getChildCount());
            fileItem2 = parent->addFile(component_, copy->getFile2(), "", QList<QSharedPointer<File> > ());
            //endInsertRows();
        }

        copy->setItemPointers(fileItem1, fileItem2);
        dependencies_.append(copy);
    }

    endResetModel();

    emit dependenciesReset();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::performAnalysisStep()
//-----------------------------------------------------------------------------
void FileDependencyModel::performAnalysisStep()
{
    // Safe-check whether the analysis has already ended.
    if (progressValue_ == getTotalStepCount())
    {
        return;
    }

    // On first step begin analysis for each resolved plugin.
    if (progressValue_ == 0)
    {
        resolvePlugins();

        // Begin analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->beginAnalysis(component_.data(), basePath_);
        }

        ++progressValue_;
    }
    // Otherwise scan one file on each step.
    else
    {
        // Run the dependency analysis for the current file.
        if (curFileIndex_ < root_->getChild(curFolderIndex_)->getChildCount())
        {
            QSharedPointer<FileDependencyItem> fileItem = root_->getChild(curFolderIndex_)->getChild(curFileIndex_);
            analyze(fileItem);

            ++curFileIndex_;
            ++progressValue_;
        }

        // Check if all files in the current folder have been analyzed.
        while (curFileIndex_ == root_->getChild(curFolderIndex_)->getChildCount())
        {
            // Update the status of the folder and continue to the next folder.
            QSharedPointer<FileDependencyItem> folderItem = root_->getChild(curFolderIndex_);
            folderItem->updateStatus();
            emit dataChanged(getItemIndex(folderItem, 0),
                getItemIndex(folderItem, FileDependencyColumns::FILE_DEPENDENCY_COLUMN_DEPENDENCIES));

            ++curFolderIndex_;
            curFileIndex_ = 0;

            if (curFolderIndex_ == root_->getChildCount() ||
                root_->getChild(curFolderIndex_)->getType() != FileDependencyItem::ITEM_TYPE_FOLDER)
            {
                break;
            }
        }
    }

    // Stop the timer when there are no more folders.
    if (progressValue_ == getTotalStepCount())
    {
        stopAnalysis();
    }
    else
    {
        // Otherwise notify progress of the next file.
        emit analysisProgressChanged(progressValue_ + 1);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::getTotalFileCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::getTotalStepCount() const
{
    int count = 1;

    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        if (root_->getChild(i)->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            count += root_->getChild(i)->getChildCount();
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::updateData()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::getItemIndex(QSharedPointer<FileDependencyItem> item, int column) const
{
    if (item == 0)
    {
        return QModelIndex();
    }

    FileDependencyItem* parent = item->getParent();

    if (parent == 0)
    {
        return QModelIndex();
    }

    return createIndex(parent->getIndex(item), column, item.data());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::resolvePlugins()
//-----------------------------------------------------------------------------
void FileDependencyModel::resolvePlugins()
{
    analyzerPluginMap_.clear();
    usedPlugins_.clear();

    foreach (IPlugin* plugin, pluginMgr_.getActivePlugins())
    {
        ISourceAnalyzerPlugin* analyzer = dynamic_cast<ISourceAnalyzerPlugin*>(plugin);

        if (analyzer != 0)
        {
            foreach (QString const& fileType, analyzer->getSupportedFileTypes())
            {
                if (!analyzerPluginMap_.contains(fileType))
                {
                    analyzerPluginMap_.insert(fileType, analyzer);
                }
            }

            usedPlugins_.append(analyzer);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::analyze()
//-----------------------------------------------------------------------------
void FileDependencyModel::analyze(QSharedPointer<FileDependencyItem> fileItem)
{
    Q_ASSERT(fileItem != 0);

    // Retrieve the corresponding plugin based on the file type.
    ISourceAnalyzerPlugin* plugin = 0;
    
    foreach (QString const& fileType, fileItem->getFileTypes())
    {
        plugin = analyzerPluginMap_.value(fileType);

        if (plugin != 0)
        {
            break;
        }
    }

    // Check the file for modifications by calculating its hash and comparing to the saved value.
    QString absPath = General::getAbsolutePath(basePath_, fileItem->getPath());
    QString lastHash = fileItem->getLastHash();
    QString hash = "";
    bool dependenciesChanged = false;

    // If a corresponding plugin was found, let it calculate the hash.
    if (plugin != 0)
    {
        hash = plugin->calculateHash(absPath);

        // If the hash has changed, resolve dependencies.
        if (hash != lastHash)
        {
            // Retrieve the old dependencies.
            QList<QSharedPointer<FileDependency> > oldDependencies = findDependencies(fileItem->getPath());

            // Scan the current dependencies.
            QList<FileDependencyDesc> dependencyDescs = plugin->getFileDependencies(component_.data(), 
                basePath_, absPath);

            QString file1 = fileItem->getPath();

            // Go through all current dependencies.
            foreach (FileDependencyDesc const& desc, dependencyDescs)
            {
                QString file2 = General::getRelativePath(basePath_,
                    QFileInfo(QFileInfo(absPath).path() + "/" + desc.filename).canonicalFilePath());

                // Retrieve the item pointers.
                QSharedPointer<FileDependencyItem> fileItem1 = findFileItem(file1);
                QSharedPointer<FileDependencyItem> fileItem2 = findFileItem(file2);

                // First item should always be valid.
                Q_ASSERT(fileItem1 != 0);

                // Check if the second file was an external (not found).
                if (fileItem2 == 0)
                {
                    file2 = desc.filename;
                    fileItem2 = findExternalFileItem(file2);
                }

                // Check if the dependency already exists.
                QSharedPointer<FileDependency> found = findDependency(oldDependencies, file1, file2);

                if (found == 0)
                {
                    // Create the item for external file if not found.
                    if (fileItem2 == 0)
                    {
                        // Create the unspecified folder if not yet created.
                        QSharedPointer<FileDependencyItem> folderItem = findFolderItem("$External$");

                        if (folderItem == 0)
                        {
                            beginInsertRows(getItemIndex(root_, 0), root_->getChildCount(),
                                            root_->getChildCount());

                            folderItem = root_->addFolder(component_, "$External$",
                                                          FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION);
                            endInsertRows();
                        }

                        file2 = "$External$/" + desc.filename;

                        beginInsertRows(getItemIndex(folderItem, 0),
                                        folderItem->getChildCount(), folderItem->getChildCount());
                        fileItem2 = folderItem->addFile(component_, file2, "", QList<QSharedPointer<File> >());
                        endInsertRows();
                    }

                    // Create a new dependency if not found.
                    QSharedPointer<FileDependency> dependency(new FileDependency());
                    dependency->setFile1(file1);
                    dependency->setFile2(file2);
                    dependency->setDescription(desc.description);
                    dependency->setItemPointers(fileItem1, fileItem2);
                    dependency->setStatus(FileDependency::STATUS_ADDED);

                    addDependency(dependency);
                    dependenciesChanged = true;
                }
                else
                {
                    // Remove the dependency from the temporary list.
                    oldDependencies.removeOne(found);

                    // Check if the existing dependency needs updating to a bidirectional one.
                    if (!found->isBidirectional() && found->getFile1() != file1)
                    {
                        found->setBidirectional(true);

                        // Combine the descriptions.
                        found->setDescription(found->getDescription() + "\n" + desc.description);
                        emit dependencyChanged(found.data());

                        dependenciesChanged = true;
                    }
                    else
                    {
                        // TODO: Update the description even in this case?
                    }
                }
            }

            // Mark all existing old dependencies as removed.
            foreach (QSharedPointer<FileDependency> dependency, oldDependencies)
            {
                // If the dependency is a bidirectional one, change it to unidirectional one.
                if (dependency->isBidirectional())
                {
                    // Add the removed dependency to be able to make the diff view correctly. 
                    QSharedPointer<FileDependency> removedDependency(new FileDependency());

                    if (dependency->getFile1() == file1)
                    {
                        removedDependency->setFile1(file1);
                        removedDependency->setFile2(dependency->getFile2());
                        removedDependency->setItemPointers(dependency->getFileItem1(), dependency->getFileItem2());
                    }
                    else
                    {
                        removedDependency->setFile1(dependency->getFile2());
                        removedDependency->setFile2(file1);
                        removedDependency->setItemPointers(dependency->getFileItem2(), dependency->getFileItem1());
                    }
                    
                    removedDependency->setDescription("");
                    removedDependency->setStatus(FileDependency::STATUS_REMOVED);

                    addDependency(removedDependency);
                    
                    // Change the existing dependency into a unidirectional one.
                    dependency->setBidirectional(false);

                    // Change the direction if needed.
                    if (dependency->getFile1() == file1)
                    {
                        dependency->reverse();
                    }

                    dependenciesChanged = true;
                    emit dependencyChanged(dependency.data());
                }
                else if (dependency->getFile1() == file1)
                {
                    dependency->setStatus(FileDependency::STATUS_REMOVED);
                    dependenciesChanged = true;
                    emit dependencyChanged(dependency.data());
                }
            }
        }
    }
    else
    {
        // Calculate SHA-1 from the whole file.
        QFile file(absPath);
        QCryptographicHash cryptoHash(QCryptographicHash::Md5);

        int const bytesToRead = 1024 * 256;
        char buffer[bytesToRead];
        int bytesRead = 0;

        if (file.open(QIODevice::ReadOnly))
        {
            do 
            {
                bytesRead = file.read(buffer, bytesToRead);
                cryptoHash.addData(buffer, bytesRead);
            }
            while (bytesRead == bytesToRead);
        }

        hash = cryptoHash.result().toHex();
    }

    if (!lastHash.isEmpty() && hash != lastHash)
    {
        if (dependenciesChanged)
        {
            fileItem->setStatus(FILE_DEPENDENCY_STATUS_CHANGED2);
        }
        else
        {
            fileItem->setStatus(FILE_DEPENDENCY_STATUS_CHANGED);
        }

        emit contentChanged();
    }
    else
    {
        fileItem->setStatus(FILE_DEPENDENCY_STATUS_OK);
    }

    fileItem->setLastHash(hash);

    emit dataChanged(getItemIndex(fileItem, FileDependencyColumns::FILE_DEPENDENCY_COLUMN_STATUS),
                     getItemIndex(fileItem, FileDependencyColumns::FILE_DEPENDENCY_COLUMN_STATUS));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findItem()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependencyItem> FileDependencyModel::findFileItem(QString const& path)
{
    // Extract the folder part of the path.
    QFileInfo info(path);
    QString folderPath = info.path();

    // External folders need a special treatment.
    if (path.startsWith('$'))
    {
        folderPath = path.left(path.indexOf('$', 1) + 1);
    }

    // Search for a matching folder.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        QSharedPointer<FileDependencyItem> folderItem = root_->getChild(i);

        if (folderItem->getPath() == folderPath)
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                QSharedPointer<FileDependencyItem> fileItem = folderItem->getChild(j);

                if (fileItem->getPath() == path)
                {
                    return fileItem;
                }
            }
        }
    }

//     return 0;
    return QSharedPointer<FileDependencyItem> ();
}


//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findExternalFileItem()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependencyItem> FileDependencyModel::findExternalFileItem(QString& path)
{
    // Search for the file from all external locations.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        QSharedPointer<FileDependencyItem> folderItem = root_->getChild(i);

        if (folderItem->isExternal())
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                QSharedPointer<FileDependencyItem> fileItem = folderItem->getChild(j);

                // Check if the file's path ends with the path we're searching for.
                if (fileItem->getPath().endsWith(path))
                {
                    // TODO: Verify that it is fully correct?
                    path = fileItem->getPath();                   
                    return fileItem;
                }
            }
        }
    }

    return QSharedPointer<FileDependencyItem>();
}


//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findFolderItem()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependencyItem> FileDependencyModel::findFolderItem(QString const& path)
{
    // Search for a matching folder.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        if (root_->getChild(i)->getPath() == path)
        {
            return root_->getChild(i);
        }
    }

    return QSharedPointer<FileDependencyItem>();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependency()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependency> FileDependencyModel::findDependency(
    QList<QSharedPointer<FileDependency> > dependencies, QString const& file1, QString const& file2) const
{
    foreach (QSharedPointer<FileDependency> dependency, dependencies)
    {
        if ((dependency->getFile1() == file1 && dependency->getFile2() == file2) ||
            (dependency->getFile1() == file2 && dependency->getFile2() == file1))
        {
            return dependency;
        }
    }

    return QSharedPointer<FileDependency>();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependency()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependency> FileDependencyModel::findDependency(QString const& file1, QString const& file2) const
{
    foreach (QSharedPointer<FileDependency> dependency, dependencies_)
    {
        if ((dependency->getFile1() == file1 && dependency->getFile2() == file2) ||
            (dependency->isBidirectional() && dependency->getFile1() == file2 && dependency->getFile2() == file1))
        {
            return dependency;
        }
    }

    return QSharedPointer<FileDependency>();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::addDependency(QSharedPointer<FileDependency> dependency)
{
    // Update the file item pointers if not yet up to date.
    if (dependency->getFileItem1() == 0 || dependency->getFileItem2() == 0)
    {
        QSharedPointer<FileDependencyItem> fileItem1 = findFileItem(dependency->getFile1());
        QSharedPointer<FileDependencyItem> fileItem2 = findFileItem(dependency->getFile2());
        dependency->setItemPointers(fileItem1, fileItem2);
    }

    dependencies_.append(dependency);
    emit dependencyAdded(dependency.data());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::removeDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::removeDependency(QSharedPointer<FileDependency> dependency)
{
    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep == dependency)
        {
            emit dependencyRemoved(dependency.data());
            dependencies_.removeOne(dep);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::getDependencies()
//-----------------------------------------------------------------------------
QList< QSharedPointer<FileDependency> > FileDependencyModel::getDependencies() const
{
    return dependencies_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependencies()
//-----------------------------------------------------------------------------
QList<QSharedPointer<FileDependency> > FileDependencyModel::findDependencies(QString const& file) const
{
    QList<QSharedPointer<FileDependency> > dependencies;

    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep->getFile1() == file || (/*dep->isBidirectional() &&*/ dep->getFile2() == file))
        {
            dependencies.append(dep);
        }
    }

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::defineLocation()
//-----------------------------------------------------------------------------
void FileDependencyModel::defineLocation(QSharedPointer<FileDependencyItem> item, QString const& path)
{
    if (!item->isExternal() || item->getType() != FileDependencyItem::ITEM_TYPE_FILE)
    {
        return;
    }

    // Search for a matching external folder.
    QString fullPath = "$" + path + "$";
    QSharedPointer<FileDependencyItem> parent = findFolderItem(fullPath);

    // Check if a new external folder needs to be created.
    if (parent == 0)
    {
        int index = root_->getChildCount() - 1;

        beginInsertRows(getItemIndex(root_, 0), index, index);

        parent = root_->addFolder(component_, fullPath, FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION, index);
        endInsertRows();
    }

    moveItem(item, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::resetLocation()
//-----------------------------------------------------------------------------
void FileDependencyModel::resetLocation(QSharedPointer<FileDependencyItem> item)
{
    // Only the location of externals files can be changed.
    if (!item->isExternal() || item->getType() != FileDependencyItem::ITEM_TYPE_FILE)
    {
        return;
    }

    if (item->getParent()->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION)
    {
        moveItem(item, root_->getChild(root_->getChildCount() - 1));
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::moveItem()
//-----------------------------------------------------------------------------
void FileDependencyModel::moveItem(QSharedPointer<FileDependencyItem> item,
    QSharedPointer<FileDependencyItem> parent)
{
    FileDependencyItem* oldParentPointer = item->getParent();
    QSharedPointer<FileDependencyItem> oldParent = transformToSharedPointer(oldParentPointer, root_);

    QString oldPath = item->getPath();

    // Move the item into its new parent.
    beginRemoveRows(getItemIndex(oldParent, 0), oldParent->getIndex(item), oldParent->getIndex(item));
    oldParent->removeItem(item);
    endRemoveRows();

    beginInsertColumns(getItemIndex(parent, 0), parent->getChildCount(), parent->getChildCount());
    parent->insertItem(item);
    endInsertRows();

    // Remove the old parent if it got empty and is an external location.
    if (oldParent->getChildCount() == 0 &&
        oldParent->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION)
    {
        beginRemoveRows(getItemIndex(root_, 0), oldParent->getIndex(item), oldParent->getIndex(item));
        root_->removeItem(oldParent);
        delete oldParentPointer;
        oldParentPointer = 0;
        oldParent.clear();
        endRemoveRows();
    }

    onExternalRelocated(item, oldPath);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::onFileRelocated()
//-----------------------------------------------------------------------------
void FileDependencyModel::onExternalRelocated(QSharedPointer<FileDependencyItem> item, QString const& oldPath)
{
    foreach (QSharedPointer<FileDependency> dependency, dependencies_)
    {
        if (dependency->getFile2() == oldPath)
        {
            dependency->setFile2(item->getPath());
        }
    }

    emit dependenciesReset();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::refresh()
//-----------------------------------------------------------------------------
void FileDependencyModel::refresh()
{
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        QSharedPointer<FileDependencyItem> folderItem = root_->getChild(i);

        if (folderItem->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                folderItem->getChild(j)->refreshFileRefs();
            }
        }
    }

    emit dataChanged(getItemIndex(root_->getChild(0), 0),
                     getItemIndex(root_->getChild(root_->getChildCount() - 1),
                     FileDependencyColumns::FILE_DEPENDENCY_COLUMN_DEPENDENCIES));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::transformToSharedPointer()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependencyItem> FileDependencyModel::transformToSharedPointer(FileDependencyItem* targetItem,
    QSharedPointer<FileDependencyItem> currentItem)
{
    if (currentItem.data() == targetItem)
    {
        return currentItem;
    }

    foreach (QSharedPointer<FileDependencyItem> childItem, currentItem->getChildItems())
    {
        if (childItem.data() == targetItem)
        {
            return childItem;
        }
        else
        {
            transformToSharedPointer(targetItem, currentItem);
        }
    }

    // Not found.
    return QSharedPointer<FileDependencyItem>();
}
