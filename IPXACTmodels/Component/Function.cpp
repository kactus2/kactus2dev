//-----------------------------------------------------------------------------
// File: Function.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Describes the ipxact:function element.
//-----------------------------------------------------------------------------

#include "Function.h"

/*
bool Function::SourceFile::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (sourceName_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory source name missing for source file within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (fileType_.isEmpty() && userFileType_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory file type missing for source file %1 within %2").arg(
			sourceName_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}*/
/*
bool Function::SourceFile::isValid() const {
	if (sourceName_.isEmpty()) {
		return false;
	}

	if (fileType_.isEmpty() && userFileType_.isEmpty()) {
		return false;
	}

	return true;
}*/

//-----------------------------------------------------------------------------
// Function: Function::SourceFile()
//-----------------------------------------------------------------------------
Function::SourceFile::SourceFile(QString const& sourceName /* = QString() */,
    QString const& fileType /* = QString() */) :
sourceName_(sourceName),
fileType_(fileType)
{

}

//-----------------------------------------------------------------------------
// Function: Function::SourceFile()
//-----------------------------------------------------------------------------
Function::SourceFile::SourceFile(const SourceFile& other) :
sourceName_(other.sourceName_),
fileType_(other.fileType_)
{

}

//-----------------------------------------------------------------------------
// Function: Function::getSourceName()
//-----------------------------------------------------------------------------
QString Function::SourceFile::getSourceName() const
{
    return sourceName_;
}

//-----------------------------------------------------------------------------
// Function: Function::setSourceName()
//-----------------------------------------------------------------------------
void Function::SourceFile::setSourceName(QString const& newSourceName)
{
    sourceName_ = newSourceName;
}

//-----------------------------------------------------------------------------
// Function: Function::getFileType()
//-----------------------------------------------------------------------------
QString Function::SourceFile::getFileType() const
{
    return fileType_;
}

//-----------------------------------------------------------------------------
// Function: Function::setFileType()
//-----------------------------------------------------------------------------
void Function::SourceFile::setFileType(QString const& newFileType)
{
    fileType_ = newFileType;
}

//-----------------------------------------------------------------------------
// Function: Function::Function()
//-----------------------------------------------------------------------------
Function::Function(QString const& fileReference /* = QString() */) :
replicate_(false),
entryPoint_(),
fileRef_(fileReference),
returnType_(),
arguments_(new QList<QSharedPointer<NameValuePair> > ()),
disabled_(),
disabledAttributes_(),
sourceFiles_(new QList<QSharedPointer<SourceFile> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Function::Function()
//-----------------------------------------------------------------------------
Function::Function(const Function &other):
replicate_(other.replicate_),
entryPoint_(other.entryPoint_),
fileRef_(other.fileRef_),
returnType_(other.returnType_),
arguments_(new QList<QSharedPointer<NameValuePair> > ()),
disabled_(other.disabled_),
disabledAttributes_(other.disabledAttributes_),
sourceFiles_(new QList<QSharedPointer<SourceFile> > ())
{
    copyArguments(other);
    copySourceFiles(other);
}

//-----------------------------------------------------------------------------
// Function: Function::operator=()
//-----------------------------------------------------------------------------
Function& Function::operator=(const Function& other)
{
    if (this != &other)
    {
        replicate_ = other.replicate_;
        entryPoint_ = other.entryPoint_;
        fileRef_ = other.fileRef_;
        returnType_ = other.returnType_;
        disabled_ = other.disabled_;
        disabledAttributes_ = other.disabledAttributes_;

        arguments_->clear();
        copyArguments(other);
        sourceFiles_->clear();
        copySourceFiles(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Function::~Function()
//-----------------------------------------------------------------------------
Function::~Function()
{
    arguments_.clear();
    sourceFiles_.clear();
}

/*
bool Function::isValid( const QStringList& fileIDs, QStringList& errorList, const QString& parentIdentifier,
    bool checkChildren ) const
{
	bool valid = true;

	if (fileRef_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory file reference missing in function within %1").arg(
			parentIdentifier));
		valid = false;
	}
	else if (!fileIDs.contains(fileRef_)) {
		errorList.append(QObject::tr("File reference %1 not found in files of %2").arg(
			fileRef_).arg(parentIdentifier));
		valid = false;
	}

	if (checkChildren) {
		foreach (QSharedPointer<Argument> argument, arguments_) {
			if (!argument->isValid(errorList, QObject::tr("function %1").arg(entryPoint_))) {
				valid = false;
			}
		}

		foreach (QSharedPointer<SourceFile> sourceFile, sourceFiles_) {
			if (!sourceFile->isValid(errorList, QObject::tr("function %1").arg(entryPoint_))) {
				valid = false;
			}
		}
	}

	return valid;
}
*//*
bool Function::isValid(const QStringList& fileIDs, bool checkChildren) const
{
	if (fileRef_.isEmpty()) {
		return false;
	}
	else if (!fileIDs.contains(fileRef_)) {
		return false;
	}

	if (checkChildren) {
		foreach (QSharedPointer<Argument> argument, arguments_) {
			if (!argument->isValid()) {
				return false;
			}
		}

		foreach (QSharedPointer<SourceFile> sourceFile, sourceFiles_) {
			if (!sourceFile->isValid()) {
				return false;
			}
		}
	}

	return true;
}
*/

//-----------------------------------------------------------------------------
// Function: Function::getReplicate()
//-----------------------------------------------------------------------------
bool Function::getReplicate() const
{
    return replicate_;
}

//-----------------------------------------------------------------------------
// Function: Function::setReplicate()
//-----------------------------------------------------------------------------
void Function::setReplicate(bool replicate)
{
    replicate_ = replicate;
}

//-----------------------------------------------------------------------------
// Function: Function::getEntryPoint()
//-----------------------------------------------------------------------------
QString Function::getEntryPoint() const
{
    return entryPoint_;
}

//-----------------------------------------------------------------------------
// Function: Function::setEntryPoint()
//-----------------------------------------------------------------------------
void Function::setEntryPoint(QString const& entryPoint)
{
    entryPoint_ = entryPoint;
}

//-----------------------------------------------------------------------------
// Function: Function::getFileRef()
//-----------------------------------------------------------------------------
QString Function::getFileRef() const
{
    return fileRef_;
}

//-----------------------------------------------------------------------------
// Function: Function::setFileRef()
//-----------------------------------------------------------------------------
void Function::setFileRef(QString const& fileRef)
{
    fileRef_ = fileRef;
}

//-----------------------------------------------------------------------------
// Function: Function::getReturnType()
//-----------------------------------------------------------------------------
QString Function::getReturnType() const
{
    return returnType_;
}

//-----------------------------------------------------------------------------
// Function: Function::setReturnType()
//-----------------------------------------------------------------------------
void Function::setReturnType(QString const& returnType)
{
    if (returnType == "int" || returnType == "void" || returnType.isEmpty())
    {
        returnType_ = returnType;
    }
}

//-----------------------------------------------------------------------------
// Function: Function::getArguments()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<NameValuePair> > > Function::getArguments() const
{
    return arguments_;
}

//-----------------------------------------------------------------------------
// Function: Function::setArguments()
//-----------------------------------------------------------------------------
void Function::setArguments(QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments)
{
    arguments_.clear();
    arguments_ = arguments;
}

//-----------------------------------------------------------------------------
// Function: Function::isDisabled()
//-----------------------------------------------------------------------------
QString Function::getDisabled() const
{
    return disabled_;
}

//-----------------------------------------------------------------------------
// Function: Function::setDisabled()
//-----------------------------------------------------------------------------
void Function::setDisabled(QString const& newDisabled)
{
    disabled_ = newDisabled;
}

//-----------------------------------------------------------------------------
// Function: Function::getDisabledAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> Function::getDisabledAttributes() const
{
    return disabledAttributes_;
}

//-----------------------------------------------------------------------------
// Function: Function::setDisabledAttributes()
//-----------------------------------------------------------------------------
void Function::setDisabledAttributes(QMap<QString, QString> &disabledAttributes)
{
    disabledAttributes_.clear();
    disabledAttributes_ = disabledAttributes;
}

//-----------------------------------------------------------------------------
// Function: Function::getSourceFiles()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > Function::getSourceFiles() const
{
    return sourceFiles_;
}

//-----------------------------------------------------------------------------
// Function: Function::setSourceFiles()
//-----------------------------------------------------------------------------
void Function::setSourceFiles(QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > sourceFiles)
{
    sourceFiles_.clear();
    sourceFiles_ = sourceFiles;
}

//-----------------------------------------------------------------------------
// Function: Function::copyArguments()
//-----------------------------------------------------------------------------
void Function::copyArguments(const Function& other)
{
    foreach (QSharedPointer<NameValuePair> argument, *other.arguments_)
    {
        if (argument)
        {
            QSharedPointer<NameValuePair> copy = QSharedPointer<NameValuePair>(new NameValuePair(*argument.data()));
            arguments_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Function::copySourceFiles()
//-----------------------------------------------------------------------------
void Function::copySourceFiles(const Function& other)
{
    foreach (QSharedPointer<SourceFile> sourceFile, *other.sourceFiles_)
    {
        if (sourceFile)
        {
            QSharedPointer<SourceFile> copy = QSharedPointer<SourceFile>(new SourceFile(*sourceFile.data()));
            sourceFiles_->append(copy);
        }
    }
}