//-----------------------------------------------------------------------------
// File: FileDependencyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.02.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyModel.h"

#include "FileDependencyColumns.h"
#include "FileDependencyItem.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>

#include <common/KactusColors.h>

#include <QIcon>
#include <QDir>
#include <QCryptographicHash>

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::FileDependencyModel(QSharedPointer<Component> component, QString const& basePath):
component_(component),
    basePath_(basePath),
    root_(new FileDependencyItem()),
    timer_(0),
    curFolderIndex_(0),
    curFileIndex_(0),
    progressValue_(0),
    dependencies_()
{
    connect(this, SIGNAL(dependencyAdded(FileDependency*)),
        this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(dependencyChanged(FileDependency*)),
        this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(dependencyRemoved(FileDependency*)),
        this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::~FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::~FileDependencyModel()
{
    delete root_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (section == FileDependencyColumns::STATUS)
        {
            return tr("Status");
        }
        else if (section == FileDependencyColumns::PATH)
        {
            return tr("Path");
        }
        else if (section == FileDependencyColumns::FILESETS)
        {
            return tr("Filesets");
        }
        else if (section == FileDependencyColumns::CREATE_DEPENDENCY)
        {
            return tr("#");
        }
        else if (section == FileDependencyColumns::DEPENDENCIES)
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
int FileDependencyModel::columnCount(QModelIndex const& /*parent*/) const
{
    return FileDependencyColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::rowCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::rowCount(QModelIndex const& parent) const
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
QModelIndex FileDependencyModel::index(int row, int column, QModelIndex const& parent) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    FileDependencyItem* child = parentItem->getChild(row);
    
    if (child == 0)
    {
        return QModelIndex();
    }

    return createIndex(row, column, child);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::parent()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::parent(QModelIndex const& child) const
{
    if (child.isValid() == false)
    {
        return QModelIndex();
    }

    FileDependencyItem* childItem = static_cast<FileDependencyItem*>(child.internalPointer());
    FileDependencyItem* parent = childItem->getParent();

    if (parent == 0 || parent == root_)
    {
        return QModelIndex();
    }

    int index = parent->getIndex();

    if (index < 0)
    {
        return QModelIndex();
    }

    return createIndex(index, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::setData()
//-----------------------------------------------------------------------------
bool FileDependencyModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (index.isValid() == false)
    {
        return false;
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::EditRole && index.column() == FileDependencyColumns::FILESETS)
    {
        QStringList fileSetNames = value.toStringList();

        // Retrieve correct file sets from the component.
        QList<QSharedPointer<FileSet> > fileSets;
        foreach (QString const& name, fileSetNames)
        {
            QSharedPointer<FileSet> fileSet = component_->getFileSet(name);
            fileSets.append(fileSet);
        }

        bool canSet = fileSets.count() > 0;
        if (canSet)
        {
            item->setFileSets(fileSets);
            emit dataChanged(index, index);
            emit contentChanged();
        }

        return canSet;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::data()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::data(QModelIndex const& index, int role) const
{
    if (index.isValid() == false)
    {
        return QVariant();
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == FileDependencyColumns::PATH)
        {
            return item->getDisplayPath();
        }
        else if (index.column() == FileDependencyColumns::FILESETS)
        {
            if (item->isExternal())
            {
                return QVariant();
            }

            QStringList fileSets = item->getFileSetNames();
   
            if (fileSets.isEmpty())
            {
                return tr("[none]");
            }
            else if (item->getChildCount() > 1 && item->hasMultipleFileSets())
            {
                return tr("[multiple]");
            }
            else
            {
                return fileSets.join(QStringLiteral("; "));
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == FileDependencyColumns::STATUS)
        {
            if (item->isExternal())
            {
                return QVariant();
            }

            if (item->getStatus() == FileDependencyItem::FILE_DEPENDENCY_STATUS_UNKNOWN)
            {
                return QIcon(":icons/common/graphics/traffic-light_gray.png");
            }
            else if (item->getStatus() == FileDependencyItem::FILE_DEPENDENCY_STATUS_OK)
            {
                return QIcon(":icons/common/graphics/traffic-light_green.png");
            }
            else if (item->getStatus() == FileDependencyItem::FILE_DEPENDENCY_STATUS_CHANGED)
            {
                return QIcon(":icons/common/graphics/traffic-light_yellow.png");
            }
            else if (item->getStatus() == FileDependencyItem::FILE_DEPENDENCY_STATUS_CHANGED2)
            {
                return QIcon(":icons/common/graphics/traffic-light_red.png");
            }
        }
    }
    else if (role == Qt::SizeHintRole && index.column() == FileDependencyColumns::STATUS)
    {
            return QSize(16, 16);
    }
    else if (role == Qt::ForegroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FILE &&
            item->getParent()->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            QString absPath = General::getAbsolutePath(basePath_, item->getPath());
            
            if (QFileInfo(absPath).exists() == false)
            {
                return KactusColors::ERROR;
            }
            else if (item->getFileSetNames().isEmpty())
            {
                return KactusColors::DISABLED_TEXT;
            }
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FOLDER ||
            item->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION ||
            item->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
        {
            return KactusColors::STRONG_FIELD;
        }
        else if (item->getParent()->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            QString absPath = General::getAbsolutePath(basePath_, item->getPath());
            
            if (QFile(absPath).exists() == false)
            {
                return KactusColors::INVALID_FIELD;
            }
        }
        
        if (index.column() == FileDependencyColumns::CREATE_DEPENDENCY)
        {
            return KactusColors::STRONG_FIELD;
        }
    }
    else if (role == Qt::UserRole)
    {
        return item->getFileSetNames();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::hasChildren()
//-----------------------------------------------------------------------------
bool FileDependencyModel::hasChildren(QModelIndex const& parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    return (parentItem->getChildCount() > 0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileDependencyModel::flags(QModelIndex const& index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == FileDependencyColumns::CREATE_DEPENDENCY ||
        index.column() == FileDependencyColumns::DEPENDENCIES)
    {
        return Qt::ItemIsSelectable;
    }
    else if (index.column() == FileDependencyColumns::FILESETS)
    {
        FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());
        
        if ((item->getType() == FileDependencyItem::ITEM_TYPE_FILE && item->isExternal()) == false || 
            item->getChildCount() > 0)
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
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(performAnalysisStep()));
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
        delete timer_;
        timer_ = 0;

        // Reset the progress.
        emit analysisProgressChanged(0);

        // End analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->endAnalysis(component_.data(), basePath_);
        }

        emit dependenciesReset();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addFolder()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::addFolder(QString const& path)
{
    return root_->addFolder(QSharedPointer<Component>(), path);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::beginReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::beginReset()
{
    beginResetModel();

    delete root_;
    root_ = new FileDependencyItem();

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
        QSharedPointer<FileDependency> copy(new FileDependency(*dependency));
        copy->setStatus(FileDependency::STATUS_UNCHANGED);

        FileDependencyItem* startItem = findFileItem(copy->getFile1());

        // First item should always be valid. Otherwise the dependency should be discarded altogether.
        if (startItem == 0)
        {
            emit dependenciesChanged();
            continue;
        }

        // Check if the second one is an external (not found).
        if (findFileItem(copy->getFile2()) == 0)
        {
            // Extract the name of the external folder.
            int endIndex = copy->getFile2().indexOf('$', 1);

            if (copy->getFile2().at(0) != '$' || endIndex == -1)
            {
                continue;
            }

            // Search for a corresponding folder item.
            QString folderName = copy->getFile2().mid(0, endIndex + 1);
            FileDependencyItem* parent = findFolderItem(folderName);

            // Create the folder item if not found.
            if (parent == 0)
            {
                parent = root_->addFolder(component_, folderName);
            }

            parent->addFile(component_, copy->getFile2(), QList<QSharedPointer<File> >());
        }

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

        progressValue_++;
    }
    // Otherwise scan one file on each step.
    else
    {
        // Run the dependency analysis for the current file.
        if (curFileIndex_ < root_->getChild(curFolderIndex_)->getChildCount())
        {
            FileDependencyItem* fileItem = root_->getChild(curFolderIndex_)->getChild(curFileIndex_);
            analyze(fileItem);

            curFileIndex_++;
            progressValue_++;
        }

        // Check if all files in the current folder have been analyzed.
        while (curFileIndex_ == root_->getChild(curFolderIndex_)->getChildCount())
        {
            // Update the status of the folder and continue to the next folder.
            FileDependencyItem* folderItem = root_->getChild(curFolderIndex_);
            folderItem->updateStatus();

            emit dataChanged(getItemIndex(folderItem, 0), getItemIndex(folderItem, 
                FileDependencyColumns::DEPENDENCIES));

            curFolderIndex_++;
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
// Function: FileDependencyModel::getItemIndex()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::getItemIndex(FileDependencyItem* item, int column) const
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

    return createIndex(item->getIndex(), column, item);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::resolvePlugins()
//-----------------------------------------------------------------------------
void FileDependencyModel::resolvePlugins()
{
    analyzerPluginMap_.clear();
    usedPlugins_.clear();

    foreach (IPlugin* plugin, PluginManager::getInstance().getActivePlugins())
    {
        ISourceAnalyzerPlugin* analyzer = dynamic_cast<ISourceAnalyzerPlugin*>(plugin);
        if (analyzer != 0)
        {
            foreach (QString const& fileType, analyzer->getSupportedFileTypes())
            {
                if (analyzerPluginMap_.contains(fileType) == false)
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
void FileDependencyModel::analyze(FileDependencyItem* fileItem)
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
    QString currentHash = QString();
    bool dependenciesChanged = false;

    // If a corresponding plugin was found, let it calculate the hash.
    if (plugin != 0)
    {
        currentHash = plugin->calculateHash(absPath);

        // If the hash has changed, resolve the new dependencies.
        if (currentHash != lastHash)
        {
            QList<FileDependency*> oldDependencies = findDependencies(fileItem->getPath());

            // Scan the current dependencies.
            QList<FileDependencyDesc> analyzedDependencies = plugin->getFileDependencies(component_.data(), 
                basePath_, absPath);

            QString file1 = fileItem->getPath();

            // Go through all current dependencies.
            foreach (FileDependencyDesc const& desc, analyzedDependencies)
            {
                QString file2 = General::getRelativePath(basePath_,
                    QFileInfo(QFileInfo(absPath).path() + "/" + desc.filename).canonicalFilePath());

                FileDependencyItem* fileItem1 = findFileItem(file1);
                FileDependencyItem* fileItem2 = findFileItem(file2);

                // First item should always be valid.
                Q_ASSERT(fileItem1 != 0);

                // Check if the second file was an external (not found).
                if (fileItem2 == 0)
                {
                    file2 = desc.filename;
                    fileItem2 = findExternalFileItem(file2);
                }

                // Check if the dependency already exists.
                FileDependency* found = findDependency(oldDependencies, file1, file2);

                if (found == 0)
                {
                    // Create the item for external file if not found.
                    if (fileItem2 == 0)
                    {
                        FileDependencyItem* folderItem = findFolderItem("$External$");
                        if (folderItem == 0)
                        {
                            beginInsertRows(getItemIndex(root_, 0), root_->getChildCount(), root_->getChildCount());
                            folderItem = root_->addFolder(component_, "$External$");
                            endInsertRows();
                        }

                        file2 = "$External$/" + desc.filename;

                        beginInsertRows(getItemIndex(folderItem, 0), folderItem->getChildCount(),
                            folderItem->getChildCount());
                        folderItem->addFile(component_, file2, QList<QSharedPointer<File> >());
                        endInsertRows();
                    }

                    // Create a new dependency if not found.
                    QSharedPointer<FileDependency> dependency(new FileDependency());
                    dependency->setFile1(file1);
                    dependency->setFile2(file2);
                    dependency->setDescription(desc.description);
                    dependency->setStatus(FileDependency::STATUS_ADDED);

                    addDependency(dependency);
                    dependenciesChanged = true;
                }
                else
                {
                    // Remove the dependency from the temporary list.
                    oldDependencies.removeOne(found);

                    // Check if the existing dependency needs updating to a bidirectional one.
                    if (found->isBidirectional() == false && found->getFile1() != file1)
                    {
                        found->setBidirectional(true);

                        // Combine the descriptions.
                        found->setDescription(found->getDescription() + "\n" + desc.description);
                        emit dependencyChanged(found);

                        dependenciesChanged = true;
                    }
                }
            }

            // Mark all existing old dependencies as removed.
            foreach (FileDependency* dependency, oldDependencies)
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
                    }
                    else
                    {
                        removedDependency->setFile1(dependency->getFile2());
                        removedDependency->setFile2(file1);
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
                    emit dependencyChanged(dependency);
                }
                else if (dependency->getFile1() == file1)
                {
                    dependency->setStatus(FileDependency::STATUS_REMOVED);
                    dependenciesChanged = true;
                    emit dependencyChanged(dependency);
                }
            }
        }
    }
    else
    {
        // Calculate SHA-1 from the whole file.
        currentHash = calculateMd5forFile(absPath);
    }

    if (lastHash.isEmpty() == false && currentHash != lastHash)
    {
        if (dependenciesChanged)
        {
            fileItem->setStatus(FileDependencyItem::FILE_DEPENDENCY_STATUS_CHANGED2);
        }
        else
        {
            fileItem->setStatus(FileDependencyItem::FILE_DEPENDENCY_STATUS_CHANGED);
        }

        emit contentChanged();
    }
    else
    {
        fileItem->setStatus(FileDependencyItem::FILE_DEPENDENCY_STATUS_OK);
    }

    fileItem->setLastHash(currentHash);

    emit dataChanged(getItemIndex(fileItem, FileDependencyColumns::STATUS),
                     getItemIndex(fileItem, FileDependencyColumns::STATUS));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::calculateMd5forFile()
//-----------------------------------------------------------------------------
QString FileDependencyModel::calculateMd5forFile(QString const& absPath) const
{
    QFile file(absPath);
    QCryptographicHash cryptoHash(QCryptographicHash::Md5);

    if (file.open(QIODevice::ReadOnly))
    {
        int const MAX_BYTES_PER_READ = 1024 * 256;
        char readBuffer[MAX_BYTES_PER_READ];
        int bytesRead = 0;

        do 
        {
            bytesRead = file.read(readBuffer, MAX_BYTES_PER_READ);
            cryptoHash.addData(readBuffer, bytesRead);
        }
        while (bytesRead == MAX_BYTES_PER_READ);
    }

    return cryptoHash.result().toHex();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findItem()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::findFileItem(QString const& path)
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
        FileDependencyItem* folderItem = root_->getChild(i);

        if (folderItem->getPath() == folderPath)
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                FileDependencyItem* fileItem = folderItem->getChild(j);

                if (fileItem->getPath() == path)
                {
                    return fileItem;
                }
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findExternalFileItem()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::findExternalFileItem(QString& path)
{
    // Search for the file from all external locations.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        FileDependencyItem* folderItem = root_->getChild(i);

        if (folderItem->isExternal())
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                FileDependencyItem* fileItem = folderItem->getChild(j);

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

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findFolderItem()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::findFolderItem(QString const& path)
{
    // Search for a matching folder.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        if (root_->getChild(i)->getPath() == path)
        {
            return root_->getChild(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependency()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyModel::findDependency(QList<FileDependency*> const& dependencies,
                                                    QString const& file1, QString const& file2) const
{
    foreach (FileDependency* dependency, dependencies)
    {
        if ((dependency->getFile1() == file1 && dependency->getFile2() == file2) ||
            (dependency->getFile1() == file2 && dependency->getFile2() == file1))
        {
            return dependency;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependency()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyModel::findDependency(QString const& file1, QString const& file2) const
{
    foreach (QSharedPointer<FileDependency> dependency, dependencies_)
    {
        if ((dependency->getFile1() == file1 && dependency->getFile2() == file2) ||
            (dependency->isBidirectional() && dependency->getFile1() == file2 && dependency->getFile2() == file1))
        {
            return dependency.data();
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::addDependency(QSharedPointer<FileDependency> dependency)
{
    dependencies_.append(dependency);
    component_->setFileDependendencies(dependencies_);

    emit dependencyAdded(dependency.data());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::removeDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::removeDependency(FileDependency* dependency)
{
    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep == dependency)
        {
            emit dependencyRemoved(dependency);
            dependencies_.removeOne(dep);
            component_->setFileDependendencies(dependencies_);
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
QList<FileDependency*> FileDependencyModel::findDependencies(QString const& file) const
{
    QList<FileDependency*> dependencies;

    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep->getFile1() == file || dep->getFile2() == file)
        {
            dependencies.append(dep.data());
        }
    }

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::defineLocation()
//-----------------------------------------------------------------------------
void FileDependencyModel::defineLocation(FileDependencyItem* item, QString const& path)
{
    // Only the location of externals files can be changed.
    if (item->isExternal() == false || item->getType() != FileDependencyItem::ITEM_TYPE_FILE)
    {
        return;
    }

    // Search for a matching external folder.
    QString fullPath = "$" + path + "$";
    FileDependencyItem* parent = findFolderItem(fullPath);

    // Check if a new external folder needs to be created.
    if (parent == 0)
    {
        int index = root_->getChildCount() - 1;

        beginInsertRows(getItemIndex(root_, 0), index, index);
        parent = root_->addFolder(component_, fullPath);
        endInsertRows();
    }

    moveItem(item, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::resetLocation()
//-----------------------------------------------------------------------------
void FileDependencyModel::resetLocation(FileDependencyItem* item)
{
    // Only the location of externals files can be changed.
    if (item->isExternal() == false || item->getType() != FileDependencyItem::ITEM_TYPE_FILE)
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
void FileDependencyModel::moveItem(FileDependencyItem* item, FileDependencyItem* parent)
{
    FileDependencyItem* oldParent = item->getParent();
    QString oldPath = item->getPath();

    // Move the item into its new parent.
    beginRemoveRows(getItemIndex(oldParent, 0), item->getIndex(), item->getIndex());
    oldParent->removeItem(item);
    endRemoveRows();

    beginInsertColumns(getItemIndex(parent, 0), parent->getChildCount(), parent->getChildCount());
    parent->insertItem(item);
    endInsertRows();

    // Remove the old parent if it got empty and is an external location.
    if (oldParent->getChildCount() == 0 &&
        oldParent->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION)
    {
        beginRemoveRows(getItemIndex(root_, 0), oldParent->getIndex(), oldParent->getIndex());
        root_->removeItem(oldParent);
        delete oldParent;
        endRemoveRows();
    }

    onExternalRelocated(item, oldPath);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::onFileRelocated()
//-----------------------------------------------------------------------------
void FileDependencyModel::onExternalRelocated(FileDependencyItem* item, QString const& oldPath)
{
    foreach (QSharedPointer<FileDependency> dependency, dependencies_)
    {
        if (dependency->getFile2() == oldPath)
        {
            dependency->setFile2(item->getPath());
        }
    }

    component_->setFileDependendencies(dependencies_);
    emit dependenciesReset();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::refresh()
//-----------------------------------------------------------------------------
void FileDependencyModel::refresh()
{
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        FileDependencyItem* folderItem = root_->getChild(i);

        if (folderItem->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                folderItem->getChild(j)->refreshFileRefs();
            }
        }
    }

    emit dataChanged(getItemIndex(root_->getChild(0), 0),
        getItemIndex(root_->getChild(root_->getChildCount() - 1), FileDependencyColumns::DEPENDENCIES));
}
