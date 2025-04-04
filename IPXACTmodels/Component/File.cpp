//-----------------------------------------------------------------------------
// File: File.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Describes the ipxact:file element.
//-----------------------------------------------------------------------------

#include "File.h"

#include <IPXACTmodels/common/FileType.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: File::File()
//-----------------------------------------------------------------------------
File::File(QString const& filePath, QString const& fileType) :
Extendable(),
name_(filePath)
{
	parseFileName();

    if (!fileType.isEmpty())
    {
        fileTypes_->append(FileType(fileType));
    }
}

//-----------------------------------------------------------------------------
// Function: File::File()
//-----------------------------------------------------------------------------
File::File(const File& other) :
Extendable(other),
fileId_(other.fileId_),
attributes_(other.attributes_),
name_(other.name_),
fileName_(other.fileName_),
isPresent_(other.isPresent_),
structural_(other.structural_),
includeFile_(other.includeFile_),
externalDeclarations_(other.externalDeclarations_),
logicalName_(other.logicalName_),
logicalNameDefault_(other.logicalNameDefault_),
description_(other.description_)
{
    copyBuildCommand(other);
    copyDefines(other);
    copyStringLists(other);
}

//-----------------------------------------------------------------------------
// Function: File::operator=()
//-----------------------------------------------------------------------------
File& File::operator=(const File &other)
{
    if (this != & other)
    {
        Extendable::operator=(other);
        fileId_ = other.fileId_;
        attributes_ = other.attributes_;
        name_ = other.name_;
        isPresent_ = other.isPresent_;
        structural_ = other.structural_;
        includeFile_ = other.includeFile_;
        externalDeclarations_ = other.externalDeclarations_;
        logicalName_ = other.logicalName_;
        logicalNameDefault_ = other.logicalNameDefault_;
        description_ = other.description_;
        
        buildCommand_.clear();
        copyBuildCommand(other);
        defines_->clear();
        copyDefines(other);
        fileTypes_->clear();
        exportedNames_->clear();
        dependencies_->clear();
        imageTypes_->clear();
        copyStringLists(other);
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: File::~File()
//-----------------------------------------------------------------------------
File::~File()
{

}

//-----------------------------------------------------------------------------
// Function: File::operator==()
//-----------------------------------------------------------------------------
bool File::operator==( const File& other ) const
{
    return name_ == other.name_;
}

//-----------------------------------------------------------------------------
// Function: File::operator!=()
//-----------------------------------------------------------------------------
bool File::operator!=( const File& other ) const
{
    return !operator==(other);
}

//-----------------------------------------------------------------------------
// Function: File::getFileId()
//-----------------------------------------------------------------------------
QString File::getFileId() const
{
    return fileId_;
}

//-----------------------------------------------------------------------------
// Function: File::setFileId()
//-----------------------------------------------------------------------------
void File::setFileId(QString const& fileId)
{
    fileId_ = fileId;
}

//-----------------------------------------------------------------------------
// Function: File::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> File::getAttributes() const
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: File::setAttributes()
//-----------------------------------------------------------------------------
void File::setAttributes(QMap<QString, QString> const& newAttributes)
{
	attributes_.clear();
	attributes_ = newAttributes;
}

//-----------------------------------------------------------------------------
// Function: File::name()
//-----------------------------------------------------------------------------
QString File::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: File::setName()
//-----------------------------------------------------------------------------
void File::setName(QString const& newName)
{
    name_ = newName;
	parseFileName();
}

//-----------------------------------------------------------------------------
// Function: File::getIsPresent()
//-----------------------------------------------------------------------------
QString File::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: File::setIsPresent()
//-----------------------------------------------------------------------------
void File::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: File::getFileTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<FileType> > File::getFileTypes() const
{
    return fileTypes_;
}

//-----------------------------------------------------------------------------
// Function: File::getFileTypeNames()
//-----------------------------------------------------------------------------
QStringList File::getFileTypeNames() const
{
    QStringList types;
    std::for_each(fileTypes_->cbegin(), fileTypes_->cend(), [&types](auto const& fileType) {
        types.append(fileType.type_);
        });

    return types;
}

//-----------------------------------------------------------------------------
// Function: File::setFileTypes()
//-----------------------------------------------------------------------------
void File::setFileTypes( QSharedPointer<QList<FileType> > fileTypes )
{
    fileTypes_ = fileTypes;
}

//-----------------------------------------------------------------------------
// Function: File::isStructural()
//-----------------------------------------------------------------------------
bool File::isStructural() const
{
    return structural_;
}

//-----------------------------------------------------------------------------
// Function: File::setStructural()
//-----------------------------------------------------------------------------
void File::setStructural(bool structuralStatus)
{
    structural_ = structuralStatus;
}

//-----------------------------------------------------------------------------
// Function: File::getIncludeFile()
//-----------------------------------------------------------------------------
bool File::isIncludeFile() const
{
    return includeFile_;
}

//-----------------------------------------------------------------------------
// Function: File::setIncludeFile()
//-----------------------------------------------------------------------------
void File::setIncludeFile(bool includeFile)
{
    includeFile_ = includeFile;
}

//-----------------------------------------------------------------------------
// Function: File::getExternalDeclarations()
//-----------------------------------------------------------------------------
bool File::hasExternalDeclarations() const
{
    return externalDeclarations_;
}

//-----------------------------------------------------------------------------
// Function: File::setExternalDeclarations()
//-----------------------------------------------------------------------------
void File::setExternalDeclarations(bool externalDeclarations)
{
    externalDeclarations_ = externalDeclarations;
}

//-----------------------------------------------------------------------------
// Function: File::getLogicalName()
//-----------------------------------------------------------------------------
QString File::getLogicalName() const
{
    return logicalName_;
}

//-----------------------------------------------------------------------------
// Function: File::setLogicalName()
//-----------------------------------------------------------------------------
void File::setLogicalName(QString const& logicalName)
{
    logicalName_ = logicalName;
}

//-----------------------------------------------------------------------------
// Function: File::getLogicalNameDefault()
//-----------------------------------------------------------------------------
bool File::isLogicalNameDefault() const
{
    return logicalNameDefault_;
}

//-----------------------------------------------------------------------------
// Function: File::setLogicalNameDefault()
//-----------------------------------------------------------------------------
void File::setLogicalNameDefault(bool logicalNameDefault)
{
    logicalNameDefault_ = logicalNameDefault;
}

//-----------------------------------------------------------------------------
// Function: File::getExportedNames()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> File::getExportedNames() const
{
    return exportedNames_;
}

//-----------------------------------------------------------------------------
// Function: File::setExportedNames()
//-----------------------------------------------------------------------------
void File::setExportedNames(QSharedPointer<QStringList> exportedNames)
{
	exportedNames_->clear();
	exportedNames_ = exportedNames;
}

//-----------------------------------------------------------------------------
// Function: File::getBuildcommand()
//-----------------------------------------------------------------------------
QSharedPointer<BuildCommand> File::getBuildCommand() const
{
    if (buildCommand_)
    {
        return buildCommand_;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: File::setBuildcommand()
//-----------------------------------------------------------------------------
void File::setBuildcommand( QSharedPointer<BuildCommand> newBuildCommand)
{
    buildCommand_ = newBuildCommand;
}

//-----------------------------------------------------------------------------
// Function: File::getDependencies()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> File::getDependencies()
{
    return dependencies_;
}

//-----------------------------------------------------------------------------
// Function: File::setDependencies()
//-----------------------------------------------------------------------------
void File::setDependencies(QSharedPointer<QStringList> newDependecies)
{
    dependencies_->clear();
    dependencies_ = newDependecies;
}

//-----------------------------------------------------------------------------
// Function: File::getDefines()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<NameValuePair> > > File::getDefines()
{
    return defines_;
}

//-----------------------------------------------------------------------------
// Function: File::setDefines()
//-----------------------------------------------------------------------------
void File::setDefines(QSharedPointer<QList<QSharedPointer<NameValuePair> > > newDefines)
{
    defines_ = newDefines;
}

//-----------------------------------------------------------------------------
// Function: File::getImageTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> File::getImageTypes()
{
    return imageTypes_;
}

//-----------------------------------------------------------------------------
// Function: File::setImageTypes()
//-----------------------------------------------------------------------------
void File::setImageTypes(QSharedPointer<QStringList> newImageTypes)
{
	imageTypes_->clear();
	imageTypes_ = newImageTypes;
}

//-----------------------------------------------------------------------------
// Function: File::getDescription()
//-----------------------------------------------------------------------------
QString File::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: File::setDescription()
//-----------------------------------------------------------------------------
void File::setDescription(QString const& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: File::matchesFileType()
//-----------------------------------------------------------------------------
bool File::matchesFileType(const QStringList& fileTypes) const
{
    return std::any_of(fileTypes_->cbegin(), fileTypes_->cend(),
        [&fileTypes](auto const& fileType) { return fileTypes.contains(fileType.type_); });
}

//-----------------------------------------------------------------------------
// Function: File::matchesFileType()
//-----------------------------------------------------------------------------
bool File::matchesFileType( const QString& fileType ) const
{
    return std::any_of(fileTypes_->cbegin(), fileTypes_->cend(),
        [&fileType](auto const& type) { return type.type_.compare(fileType) == 0; });
}

//-----------------------------------------------------------------------------
// Function: File::setFileTypes()
//-----------------------------------------------------------------------------
void File::setFileTypes( QSettings& settings )
{
    QFileInfo info(name_);
    QStringList types = FileTypes::getFileTypes(settings, info.suffix());

    fileTypes_->clear();
    for (auto const& fileType : types)
    {
        fileTypes_->append(FileType(fileType));
    }
}

//-----------------------------------------------------------------------------
// Function: File::addFileType()
//-----------------------------------------------------------------------------
void File::addFileType( QString const& fileType, QString const& libext)
{
    if (matchesFileType(fileType) == false)
    {
        fileTypes_->append(FileType(fileType, libext));
    }
}

//-----------------------------------------------------------------------------
// Function: File::clearFileTypes()
//-----------------------------------------------------------------------------
void File::clearFileTypes()
{
    fileTypes_->clear();
}

//-----------------------------------------------------------------------------
// Function: File::clearDefines()
//-----------------------------------------------------------------------------
void File::clearDefines()
{
    defines_->clear();
}

//-----------------------------------------------------------------------------
// Function: File::getCommand()
//-----------------------------------------------------------------------------
QString File::getCommand() const
{
    if (buildCommand_)
    {
        return buildCommand_->getCommand();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: File::getFlags()
//-----------------------------------------------------------------------------
QString File::getFlags() const
{
    QString flags;

    if (buildCommand_)
    {
        if (!buildCommand_->getReplaceDefaultFlags().isEmpty())
        {
            return buildCommand_->getFlags();
        }
        else
        {
            flags += buildCommand_->getFlags();
        }
        flags.append(QLatin1Char(' '));
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: File::isRTLFile()
//-----------------------------------------------------------------------------
bool File::isRTLFile() const
{
    return isVhdlFile() || isVerilogFile();
}

//-----------------------------------------------------------------------------
// Function: File::isVhdlFile()
//-----------------------------------------------------------------------------
bool File::isVhdlFile() const
{
    for (auto const& fileType : *fileTypes_)
    {
        if (fileType.type_.compare(QLatin1String("vhdlSource"), Qt::CaseInsensitive) == 0 ||
            fileType.type_.compare(QLatin1String("vhdlSource-87"), Qt::CaseInsensitive) == 0 ||
            fileType.type_.compare(QLatin1String("vhdlSource-93"), Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: File::isVerilogFile()
//-----------------------------------------------------------------------------
bool File::isVerilogFile() const
{
    for (auto const& fileType : *fileTypes_)
    {
        if (fileType.type_.compare(QLatin1String("verilogSource"), Qt::CaseInsensitive) == 0 ||
            fileType.type_.compare(QLatin1String("verilogSource-95"), Qt::CaseInsensitive) == 0 ||
            fileType.type_.compare(QLatin1String("verilogSource-2001"), Qt::CaseInsensitive) == 0 )
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: File::setCommand()
//-----------------------------------------------------------------------------
void File::setCommand( const QString& buildCommand )
{
    if (!buildCommand_)
    {
        buildCommand_ = QSharedPointer<BuildCommand>(new BuildCommand());
    }

    buildCommand_->setCommand(buildCommand);
}

//-----------------------------------------------------------------------------
// Function: File::setBuildFlags()
//-----------------------------------------------------------------------------
void File::setBuildFlags(QString const& buildFlags, QString const& replaceDefaultFlags)
{
    if (!buildCommand_)
    {
        buildCommand_ = QSharedPointer<BuildCommand>(new BuildCommand());
    }
    buildCommand_->setFlags(buildFlags);
    buildCommand_->setReplaceDefaultFlags(replaceDefaultFlags);
}

//-----------------------------------------------------------------------------
// Function: File::getFileName()
//-----------------------------------------------------------------------------
QString File::getFileName() const
{
	return fileName_;
}

//-----------------------------------------------------------------------------
// Function: File::parseFileName()
//-----------------------------------------------------------------------------
void File::parseFileName()
{
	QFileInfo fileInfo(name_);
	fileName_ = fileInfo.fileName();
}

//-----------------------------------------------------------------------------
// Function: File::copyBuildCommand()
//-----------------------------------------------------------------------------
void File::copyBuildCommand(const File& other)
{
    if (other.buildCommand_)
    {
        buildCommand_ = QSharedPointer<BuildCommand> (new BuildCommand(*other.buildCommand_));
    }
}

//-----------------------------------------------------------------------------
// Function: File::copyDefines()
//-----------------------------------------------------------------------------
void File::copyDefines(const File& other)
{
    for (QSharedPointer<NameValuePair> definition : *other.defines_)
    {
        if (definition)
        {
            QSharedPointer<NameValuePair> copy =
                QSharedPointer<NameValuePair>(new NameValuePair(*definition));
            defines_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: File::copyStringLists()
//-----------------------------------------------------------------------------
void File::copyStringLists(const File& other)
{
    for (auto const& fileType : *other.fileTypes_)
    {
        fileTypes_->append(fileType);
    }

    exportedNames_->append(*other.exportedNames_);

    dependencies_->append(*other.dependencies_);

    imageTypes_->append(*other.imageTypes_);

}