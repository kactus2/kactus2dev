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

#include <QCollator>
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
    QSharedPointer<File> file;
    foreach (QSharedPointer<File> fileSearch, *files_)
    {
        if (fileSearch->name() == filePath)
        {
            file = fileSearch;
        }
    }

    if (!file)
    {
        file = QSharedPointer<File> (new File(filePath));
        files_->append(file);
    }

    QFileInfo fileInfo(filePath);
    QStringList types = FileTypes::getFileTypes(settings, fileInfo);

    foreach (QString fileType, types)
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
    foreach (QSharedPointer<const File> file, *files_)
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
            break;
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

    QSharedPointer<QStringList> fileTypes = file->getFileTypes();
    foreach (QString fileType, *fileTypes)
    {
        foreach (QSharedPointer<FileBuilder> fileBuilder, *defaultFileBuilders_)
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

    QSharedPointer<QStringList> fileTypes = file->getFileTypes();
    foreach (QString fileType, *fileTypes)
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
    foreach (QSharedPointer<File> file, *files_)
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
    foreach (QSharedPointer<File> file, *files_)
    {
        if (file->name() == fileName)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: FileSet::changeFileName()
//-----------------------------------------------------------------------------
void FileSet::changeFileName( const QString& from, const QString& to )
{
    foreach (QSharedPointer<File> file, *files_)
    {
        if (file->name() == from)
        {
            file->setName(to);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::sortFiles()
//-----------------------------------------------------------------------------
void FileSet::sortFiles( const QStringList& fileNames )
{
    QSharedPointer<QList<QSharedPointer<File> > > tempList (new QList<QSharedPointer<File> > ());

    foreach (QString fileName, fileNames)
    {
        foreach (QSharedPointer<File> file, *files_)
        {
            if (file->name() == fileName)
            {
                tempList->append(file);
            }
        }
    }

    files_.clear();
    files_ = tempList;
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:fileSetId")
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
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:fileSetId")
        {
            getVendorExtensions()->removeAll(extension);
        }
    }

    if (!id.isEmpty())
    {
        QSharedPointer<Kactus2Value> idExtension (new Kactus2Value("kactus2:fileSetId", id));
        getVendorExtensions()->append(idExtension);
    }
}

bool fileSort(QSharedPointer<File> file1, QSharedPointer<File> file2)
{
	// Used to commit the comparison.
	QCollator collator;
	collator.setNumericMode(true);

	// Path infos are needed to extract the actual file names.
	QFileInfo filePathInfo1(file1->name());
	QFileInfo filePathInfo2(file2->name());

	// Return based on the comparison result.
	return collator.compare(filePathInfo1.fileName(), filePathInfo2.fileName()) < 0;
}

//-----------------------------------------------------------------------------
// Function: FileSet::sortFiles()
//-----------------------------------------------------------------------------
void FileSet::sortFiles()
{
	// Get the list of files.
	QSharedPointer<QList<QSharedPointer<File> > > entryList = files_;

	// STD-sort was recommended by web sources.
	std::sort(
		entryList->begin(),
		entryList->end(),
		fileSort);
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
    foreach (QSharedPointer<FileBuilder> fileBuilder, *other.defaultFileBuilders_)
    {
        if (fileBuilder)
        {
            QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(new FileBuilder(*fileBuilder.data()));
            defaultFileBuilders_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFunctions()
//-----------------------------------------------------------------------------
void FileSet::copyFunctions(const FileSet& other)
{
    foreach (QSharedPointer<Function> funcion, *other.functions_)
    {
        if (funcion)
        {
            QSharedPointer<Function> copy = QSharedPointer<Function>(new Function(*funcion.data()));
            functions_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSet::copyFunctions()
//-----------------------------------------------------------------------------
void FileSet::copyStringLists(const FileSet& other)
{
    foreach (QString singleGroup, *other.groups_)
    {
        groups_->append(singleGroup);
    }

    foreach (QString dependency, *other.dependencies_)
    {
        dependencies_->append(dependency);
    }
}
