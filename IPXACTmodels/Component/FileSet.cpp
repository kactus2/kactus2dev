//-----------------------------------------------------------------------------
// File: FileSet.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Describes the ipxact:fileSet element.
//-----------------------------------------------------------------------------

#include "FileSet.h"

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/FileType.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <IPXACTmodels/utilities/Search.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileSet::FileSet()
//-----------------------------------------------------------------------------
FileSet::FileSet(QString const& name, QString const& group) :
NameGroup(name),
Extendable()
{
    if (!group.isEmpty())
    {
        groups_->append(group);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::FileSet()
//-----------------------------------------------------------------------------
FileSet::FileSet(const FileSet &other) :
NameGroup(other),
Extendable(other)
{
    copyFiles(other);
    copyDefaultFileBuilders(other);
    copyFunctions(other);
    copyStringLists(other);
}

//-----------------------------------------------------------------------------
// Function: FileSet::operator=()
//-----------------------------------------------------------------------------
FileSet& FileSet::operator=(const FileSet& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        files_->clear();
        copyFiles(other);
        defaultFileBuilders_->clear();
        copyDefaultFileBuilders(other);
        functions_->clear();
        copyFunctions(other);
        groups_->clear();
        dependencies_->clear();
        copyStringLists(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: FileSet::~FileSet()
//-----------------------------------------------------------------------------
FileSet::~FileSet()
{

}

//-----------------------------------------------------------------------------
// Function: FileSet::getGroups()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> FileSet::getGroups() const
{
    return groups_;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setGroups()
//-----------------------------------------------------------------------------
void FileSet::setGroups(QSharedPointer<QStringList> newGroups)
{
    groups_->clear();
    groups_ = newGroups;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setGroups()
//-----------------------------------------------------------------------------
void FileSet::setGroups(QString const& groupName )
{
    groups_->clear();
    groups_->append(groupName);
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFiles()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<File> > > FileSet::getFiles() const
{
    return files_;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFiles()
//-----------------------------------------------------------------------------
QStringList FileSet::getFileNames(QStringList const& fileTypes ) const
{
    QStringList files;

    for (QSharedPointer<File> file : *files_)
    {
        if (file->matchesFileType(fileTypes))
        {
            files.append(file->name());
        }
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setFiles()
//-----------------------------------------------------------------------------
void FileSet::setFiles(QSharedPointer<QList<QSharedPointer<File> > > newFiles)
{
    files_->clear();
    files_ = newFiles;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getDefaultFileBuilders()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FileBuilder> > > FileSet::getDefaultFileBuilders() const
{
    return defaultFileBuilders_;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSet::setDefaultFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder> > > newDefaultFileBuilders)
{
    defaultFileBuilders_->clear();
    defaultFileBuilders_ = newDefaultFileBuilders;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getDependencies()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> FileSet::getDependencies() const
{
    return dependencies_;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setDependencies()
//-----------------------------------------------------------------------------
void FileSet::setDependencies(QSharedPointer<QStringList> newDependencies)
{
    dependencies_->clear();
    dependencies_ = newDependencies;
}

//-----------------------------------------------------------------------------
// Function: FileSet::addDependency()
//-----------------------------------------------------------------------------
void FileSet::addDependency( QString const& path )
{
    dependencies_->append(path);
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFunctions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Function> > > FileSet::getFunctions() const
{
	return functions_;
}

//-----------------------------------------------------------------------------
// Function: FileSet::setFunctions()
//-----------------------------------------------------------------------------
void FileSet::setFunctions(QSharedPointer<QList<QSharedPointer<Function> > > newFunctions)
{
	functions_->clear();
	functions_ = newFunctions;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getVhdlLibraryNames()
//-----------------------------------------------------------------------------
QStringList FileSet::getVhdlLibraryNames() const
{
    QStringList libraries;
    for (QSharedPointer<File> file : *files_)
    {
        if (file->isVhdlFile() && !file->getLogicalName().isEmpty() && !libraries.contains(file->getLogicalName()))
        {
            libraries.append(file->getLogicalName());
        }
    }
    return libraries;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFilePaths()
//-----------------------------------------------------------------------------
QStringList FileSet::getFilePaths() const
{
    QStringList filePaths;

    std::for_each(files_->cbegin(), files_->cend(), 
        [&filePaths](auto const& file) { filePaths.append(file->name()); });

    return filePaths;
}

//-----------------------------------------------------------------------------
// Function: FileSet::addFile()
//-----------------------------------------------------------------------------
void FileSet::addFile(QSharedPointer<File> file)
{
    auto fileName = file->name();
    auto it = std::find_if(files_->cbegin(), files_->cend(),
        [&fileName](auto const& existingFile) { return existingFile->name() == fileName; });

    if (it == files_->cend())
    {
        files_->append(file);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::addFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileSet::addFile(QString const& filePath, QSettings& settings)
{
    QSharedPointer<File> file = Search::findByName(filePath, *files_);
    
    if (!file)
    {
        file = QSharedPointer<File> (new File(filePath));
        files_->append(file);
    }

    QFileInfo fileInfo(filePath);
    QStringList types = FileTypes::getFileTypes(settings, fileInfo);

    for (QString const& fileType : types)
    {
        if (!file->matchesFileType(fileType))
        {
            file->addFileType(fileType);
        }
	}

    return file;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileSet::getFile(QString const& logicalName) const
{
    auto it = std::find_if(files_->cbegin(), files_->cend(),
        [&logicalName](auto const& file) { return file->getLogicalName() == logicalName; });

    if (it != files_->cend())
    {
        return *it;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFileNames()
//-----------------------------------------------------------------------------
QStringList FileSet::getFileNames() const
{
    return getFilePaths();
}

//-----------------------------------------------------------------------------
// Function: FileSet::findFilesByFileType()
//-----------------------------------------------------------------------------
QStringList FileSet::findFilesByFileType(QString const& fileType ) const
{
    QStringList files;
    for (QSharedPointer<const File> file : *files_)
    {
        if (file->matchesFileType(fileType))
        {
            files.append(file->name());
        }
    }

    return files;
}

//-----------------------------------------------------------------------------
// Function: FileSet::removeFile()
//-----------------------------------------------------------------------------
void FileSet::removeFile(QString const& fileName )
{
    QSharedPointer<File> file = Search::findByName(fileName, *files_);

    files_->removeOne(file);
}

//-----------------------------------------------------------------------------
// Function: FileSet::removeFile()
//-----------------------------------------------------------------------------
void FileSet::removeFile(QSharedPointer<File> file)
{
    files_->removeOne(file);
}

//-----------------------------------------------------------------------------
// Function: FileSet::createFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileSet::createFile()
{
    QSharedPointer<File> file(new File());
    files_->append(file);
    return file;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getDefaultCommand()
//-----------------------------------------------------------------------------
QString FileSet::getDefaultCommand(QSharedPointer<File> file) const
{
    if (!file)
    {
        return QString();
    }

    for (auto const& fileType : *file->getFileTypes())
    {
        for (QSharedPointer<FileBuilder> fileBuilder : *defaultFileBuilders_)
        {
            if (fileBuilder->hasFileType(fileType.type_))
            {
                return fileBuilder->getCommand();
            }
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: FileSet::getDefaultFlags()
//-----------------------------------------------------------------------------
QString FileSet::getDefaultFlags(QSharedPointer<File> file) const
{
    if (!file)
    {
        return QString();
    }
    
    for (auto const& fileType : *file->getFileTypes())
    {
        for (QSharedPointer<FileBuilder> fileBuilder : *defaultFileBuilders_)
        {
            if (fileBuilder->hasFileType(fileType.type_))
            {
                return fileBuilder->getFlags();
            }
        }
    }
    return QString();
}

//-----------------------------------------------------------------------------
// Function: FileSet::getRTLFiles()
//-----------------------------------------------------------------------------
QList<QSharedPointer<File> > FileSet::getRTLFiles() const
{
    QList<QSharedPointer<File> > list;
    for (QSharedPointer<File> file : *files_)
    {
        if (file->isRTLFile())
        {
            list.append(file);
        }
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: FileSet::contains()
//-----------------------------------------------------------------------------
bool FileSet::contains(QString const& fileName ) const
{
    return Search::findByName(fileName, *files_).isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: FileSet::changeFileName()
//-----------------------------------------------------------------------------
void FileSet::changeFileName(QString const& from, QString const& to )
{
    QSharedPointer<File> file = Search::findByName(from, *files_);
    if (file.isNull() == false)
    {
        file->setName(to);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::clearFiles()
//-----------------------------------------------------------------------------
void FileSet::clearFiles()
{
    files_->clear();
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFileSetId()
//-----------------------------------------------------------------------------
QString FileSet::getFileSetId() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:fileSetId"));
    if (extension != nullptr)
    {
        QSharedPointer<Kactus2Value> idExtension = extension.dynamicCast<Kactus2Value>();
        return idExtension->value();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: FileSet::setFileSetId()
//-----------------------------------------------------------------------------
void FileSet::setFileSetId(QString const& id )
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:fileSetId")).dynamicCast<Kactus2Value>();
    if (!id.isEmpty())
    {
        if (extension == nullptr)
        {
            extension = QSharedPointer<Kactus2Value>(new Kactus2Value(QStringLiteral("kactus2:fileSetId"), QString()));
            getVendorExtensions()->append(extension);
        }

        extension->setValue(id);
    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFiles()
//-----------------------------------------------------------------------------
void FileSet::copyFiles(const FileSet& other)
{
    for (QSharedPointer<File> file : *other.files_)
    {
        QSharedPointer<File> copy = QSharedPointer<File>(new File(*file));
        files_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSet::copyDefaultFileBuilders(const FileSet& other)
{
    for (QSharedPointer<FileBuilder> fileBuilder : *other.defaultFileBuilders_)
    {
        QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(new FileBuilder(*fileBuilder));
        defaultFileBuilders_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFunctions()
//-----------------------------------------------------------------------------
void FileSet::copyFunctions(const FileSet& other)
{
    for (QSharedPointer<Function> funcion : *other.functions_)
    {
        QSharedPointer<Function> copy = QSharedPointer<Function>(new Function(*funcion));
        functions_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFunctions()
//-----------------------------------------------------------------------------
void FileSet::copyStringLists(const FileSet& other)
{
    groups_->append(*other.groups_);

    dependencies_->append(*other.dependencies_);
}
