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
#include <IPXACTmodels/common/FileTypes.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <IPXACTmodels/utilities/Search.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileSet::FileSet()
//-----------------------------------------------------------------------------
FileSet::FileSet(const QString& name /* = QString() */, const QString& group /* = QString() */) :
NameGroup(name),
Extendable(),
groups_(new QStringList()),
files_(new QList<QSharedPointer<File> > ()),
defaultFileBuilders_(new QList<QSharedPointer<FileBuilder> > ()),
dependencies_(new QStringList()),
functions_(new QList<QSharedPointer<Function> > ())
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
Extendable(other),
groups_(new QStringList()),
files_(new QList<QSharedPointer<File> > ()),
defaultFileBuilders_(new QList<QSharedPointer<FileBuilder> > ()),
dependencies_(new QStringList()),
functions_(new QList<QSharedPointer<Function> > ())
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
    files_.clear();
    defaultFileBuilders_.clear();
    functions_->clear();
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
void FileSet::setGroups( const QString& groupName )
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
QStringList FileSet::getFileNames( const QStringList& fileTypes ) const
{
    QStringList files;

    foreach (QSharedPointer<File> file, *files_)
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
void FileSet::addDependency( const QString& path )
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
    foreach (QSharedPointer<File> file, *files_)
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

    for (int i = 0; i < files_->size(); ++i)
    {
        filePaths.append(files_->at(i)->name());
    }
    return filePaths;
}

//-----------------------------------------------------------------------------
// Function: FileSet::addFile()
//-----------------------------------------------------------------------------
void FileSet::addFile(QSharedPointer<File> file)
{
	// If a duplicate exists, remove it.
    for (int i = 0; i < files_->size(); ++i)
    {
        if (files_->at(i)->name() == file->name())
        {
            files_->removeAt(i);

            break;
        }
    }

	// At any rate, append the file to the list.
    files_->append(file);
}

//-----------------------------------------------------------------------------
// Function: FileSet::addFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileSet::addFile(const QString& filePath, QSettings& settings)
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
        if (!file->getFileTypes()->contains(fileType))
        {
            file->getFileTypes()->append(fileType);
        }
	}

    return file;
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileSet::getFile(const QString logicalName) const
{
    for (int i = 0; i < files_->size(); ++i)
    {
        if (files_->at(i)->getLogicalName() == logicalName)
        {
            return files_->at(i);
        }
    }

    return QSharedPointer<File>();
}

//-----------------------------------------------------------------------------
// Function: FileSet::getFileNames()
//-----------------------------------------------------------------------------
QStringList FileSet::getFileNames() const
{
    QStringList fileNames;
    for (int i = 0; i < files_->size(); ++i)
    {
        fileNames.append(files_->at(i)->name());
    }
    return fileNames;
}

//-----------------------------------------------------------------------------
// Function: FileSet::findFilesByFileType()
//-----------------------------------------------------------------------------
QStringList FileSet::findFilesByFileType( const QString& fileType ) const
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
void FileSet::removeFile( const QString& fileName )
{
    for (int i = 0; i < files_->size(); ++i)
    {
        if (files_->at(i)->name() == fileName)
        {
            files_->value(i).clear();
            files_->removeAt(i);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::removeFile()
//-----------------------------------------------------------------------------
void FileSet::removeFile(QSharedPointer<File> file)
{
    for (int i = 0; i < files_->size(); ++i)
    {
        if (*files_->at(i) == *file)
        {
            files_->value(i).clear();
            files_->removeAt(i);
            return;
        }
    }
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

    for (QString const& fileType : *file->getFileTypes())
    {
        for (QSharedPointer<FileBuilder> fileBuilder : *defaultFileBuilders_)
        {
            if (fileBuilder->hasFileType(fileType))
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
    
    for(QString const& fileType : *file->getFileTypes())
    {
        foreach (QSharedPointer<FileBuilder> fileBuilder, *defaultFileBuilders_)
        {
            if (fileBuilder->hasFileType(fileType))
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
bool FileSet::contains( const QString& fileName ) const
{
    return Search::findByName(fileName, *files_).isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: FileSet::changeFileName()
//-----------------------------------------------------------------------------
void FileSet::changeFileName( const QString& from, const QString& to )
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
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:fileSetId"))
        {
            QSharedPointer<Kactus2Value> idExtension = extension.dynamicCast<Kactus2Value>();
            return idExtension->value();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: FileSet::setFileSetId()
//-----------------------------------------------------------------------------
void FileSet::setFileSetId( const QString& id ) 
{
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:fileSetId"))
        {
            getVendorExtensions()->removeAll(extension);
        }
    }

    if (!id.isEmpty())
    {
        QSharedPointer<Kactus2Value> idExtension (new Kactus2Value(QStringLiteral("kactus2:fileSetId"), id));
        getVendorExtensions()->append(idExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFiles()
//-----------------------------------------------------------------------------
void FileSet::copyFiles(const FileSet& other)
{
    foreach (QSharedPointer<File> file, *other.files_)
    {
        if (file)
        {
            QSharedPointer<File> copy = QSharedPointer<File>(new File(*file.data()));
            files_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSet::copyDefaultFileBuilders(const FileSet& other)
{
    for (QSharedPointer<FileBuilder> fileBuilder : *other.defaultFileBuilders_)
    {
        QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(new FileBuilder(*fileBuilder.data()));
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
        QSharedPointer<Function> copy = QSharedPointer<Function>(new Function(*funcion.data()));
        functions_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFunctions()
//-----------------------------------------------------------------------------
void FileSet::copyStringLists(const FileSet& other)
{
    for (QString singleGroup : *other.groups_)
    {
        groups_->append(singleGroup);
    }

    for (QString dependency : *other.dependencies_)
    {
        dependencies_->append(dependency);
    }
}
