/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#include "function.h"
#include "generaldeclarations.h"

#include <stdexcept>

#include <QMap>
#include <QSharedPointer>
#include <QList>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QXmlStreamWriter>

// the struct constructor
Function::Argument::Argument(QDomNode &argNode): name_(QString()),
value_(QString()), dataType_(QString()) {

	// get the spirit:dataType attribute
	QDomNamedNodeMap attributeMap = argNode.attributes();
	dataType_ = attributeMap.namedItem(QString("spirit:dataType")).nodeValue();

	for (int i = 0; i < argNode.childNodes().count(); ++i) {
		QDomNode tempNode = argNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}
	}

	// if mandatory fields are missing
// 	if (name_.isNull() || value_.isNull() || dataType_.isNull() ) {
// 		throw Parse_error(QObject::tr("Mandatory element missing in "
// 				"spirit:argument"));
// 	}
	return;
}

bool Function::Argument::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory name missing for argument within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory value missing for argument %1 within %2").arg(
			name_).arg(parentIdentifier));
		valid = false;
	}

	if (dataType_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory datatype not set for argument %1 within %2").arg(
			name_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool Function::Argument::isValid() const {
	if (name_.isEmpty()) {
		return false;
	}

	if (value_.isEmpty()) {
		return false;
	}

	if (dataType_.isEmpty()) {
		return false;
	}

	return true;
}

Function::SourceFile::SourceFile(QDomNode &sourceFileNode): sourceName_(),
		fileType_(), userFileType_() {

	for (int i = 0; i < sourceFileNode.childNodes().count(); ++i) {
		QDomNode tempNode = sourceFileNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:sourceName")) {
			sourceName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:fileType")) {
			fileType_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:userFileType")) {
			userFileType_ = tempNode.childNodes().at(0).nodeValue();
		}
	}

	// if mandatory fields are missing
// 	if (sourceName_.isEmpty() ||
// 			(fileType_.isEmpty() && userFileType_.isEmpty()) ) {
// 		throw Parse_error(QObject::tr("Mandatory element missing in spirit:"
// 				"sourceFile"));
// 	}
// 	else if (!fileType_.isEmpty() && !userFileType_.isEmpty()) {
// 		throw Parse_error(QObject::tr("Both spirit:fileType and spirit:"
// 				"userFileType elements can not be defined in spirit:sourceFile"));
// 	}
	return;
}

Function::SourceFile::SourceFile(): 
sourceName_(), 
fileType_(), 
userFileType_() {
}

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
}

bool Function::SourceFile::isValid() const {
	if (sourceName_.isEmpty()) {
		return false;
	}

	if (fileType_.isEmpty() && userFileType_.isEmpty()) {
		return false;
	}

	return true;
}

// the constructor
Function::Function(QDomNode &functionNode): replicate_(false),
entryPoint_(), fileRef_(), returnType_(),
arguments_(), disabled_(false), disabledAttributes_(), sourceFiles_() {

	// get the spirit:replicate attribute
	QDomNamedNodeMap attributeMap = functionNode.attributes();
	QString replicate = attributeMap.namedItem(QString("spirit:replicate")).
			nodeValue();
	replicate_ = General::str2Bool(replicate, false);

	for (int i = 0; i < functionNode.childNodes().count(); ++i) {
		QDomNode tempNode = functionNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:entryPoint")) {
			entryPoint_ = tempNode.childNodes().at(0).nodeValue();
			entryPoint_ = General::removeWhiteSpace(entryPoint_);
		}

		else if (tempNode.nodeName() == QString("spirit:fileRef")) {
			fileRef_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:returnType")) {
			returnType_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:argument")) {
			arguments_.append(QSharedPointer<Argument>(
					new Argument(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:disabled")) {
			QString disabled = tempNode.childNodes().at(0).nodeValue();
			disabled_ = General::str2Bool(disabled, false);

			General::parseAttributes(tempNode, disabledAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:sourceFile")) {
			sourceFiles_.append(QSharedPointer<SourceFile>(new SourceFile(
					tempNode)));
		}
	}

	// if mandatory fields are missing
// 	if (fileRef_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:fileRef "
// 				"missing in spirit:function"));
// 	}
	return;
}

Function::Function( const Function &other ):
replicate_(other.replicate_),
entryPoint_(other.entryPoint_),
fileRef_(other.fileRef_),
returnType_(other.returnType_),
arguments_(),
disabled_(other.disabled_),
disabledAttributes_(other.disabledAttributes_),
sourceFiles_() {

	foreach (QSharedPointer<Argument> argument, other.arguments_) {
		if (argument) {
			QSharedPointer<Argument> copy = QSharedPointer<Argument>(
				new Argument(*argument.data()));
			arguments_.append(copy);
		}
	}

	foreach (QSharedPointer<SourceFile> sourceFile, other.sourceFiles_) {
		if (sourceFile) {
			QSharedPointer<SourceFile> copy = QSharedPointer<SourceFile>(
				new SourceFile(*sourceFile.data()));
			sourceFiles_.append(copy);
		}
	}
}

Function & Function::operator=( const Function &other ) {
	if (this != &other) {
		replicate_ = other.replicate_;
		entryPoint_ = other.entryPoint_;
		fileRef_ = other.fileRef_;
		returnType_ = other.returnType_;
		disabled_ = other.disabled_;
		disabledAttributes_ = other.disabledAttributes_;

		arguments_.clear();
		foreach (QSharedPointer<Argument> argument, other.arguments_) {
			if (argument) {
				QSharedPointer<Argument> copy = QSharedPointer<Argument>(
					new Argument(*argument.data()));
				arguments_.append(copy);
			}
		}

		sourceFiles_.clear();
		foreach (QSharedPointer<SourceFile> sourceFile, other.sourceFiles_) {
			if (sourceFile) {
				QSharedPointer<SourceFile> copy = QSharedPointer<SourceFile>(
					new SourceFile(*sourceFile.data()));
				sourceFiles_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
Function::~Function() {
	arguments_.clear();
	return;
}

void Function::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:function");
	writer.writeAttribute("spirit:replicate", General::bool2Str(replicate_));

	if (!entryPoint_.isEmpty()) {
		writer.writeTextElement("spirit:entryPoint", entryPoint_);
	}

	writer.writeTextElement("spirit:fileRef", fileRef_);

	if (!returnType_.isEmpty()) {
		writer.writeTextElement("spirit:returnType", returnType_);
	}

	// write each argument
	for (int i = 0; i < arguments_.size(); ++i) {
		writer.writeStartElement("spirit:argument");
		writer.writeAttribute("spirit:dataType", arguments_.at(i)->dataType_);

		writer.writeTextElement("spirit:name", arguments_.at(i)->name_);
		writer.writeTextElement("spirit:value", arguments_.at(i)->value_);

		writer.writeEndElement(); // spirit:argument
	}

	// start the spirit:disabled tag
	writer.writeStartElement("spirit:disabled");

	// write the attributes for the element
	General::writeAttributes(writer, disabledAttributes_);

	// write the value of the element and close the tag
	writer.writeCharacters(General::bool2Str(disabled_));
	writer.writeEndElement(); // spirit:disabled

	for (int i = 0; i < sourceFiles_.size(); ++i) {
		writer.writeStartElement("spirit:sourceFile");

		writer.writeTextElement("spirit:sourceName",
				sourceFiles_.at(i)->sourceName_);

		// if fileType is defined and userFileType is not defined
		if (!sourceFiles_.at(i)->fileType_.isEmpty() &&
				sourceFiles_.at(i)->userFileType_.isEmpty()) {
			writer.writeTextElement("spirit:fileType",
					sourceFiles_.at(i)->fileType_);
		}
		// if fileType is not defined and userFileType is defined
		else if (sourceFiles_.at(i)->fileType_.isEmpty() &&
				!sourceFiles_.at(i)->userFileType_.isEmpty()) {
			writer.writeTextElement("spirit:userFileType",
					sourceFiles_.at(i)->userFileType_);
		}

		writer.writeEndElement(); // spirit:sourceFile
	}

	writer.writeEndElement(); // spirit:function
}

bool Function::isValid( const QStringList& fileIDs, 
					   QStringList& errorList,
					   const QString& parentIdentifier,
					   bool checkChildren /*= true*/ ) const {

	bool valid = true;
    throw std::runtime_error("error");

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

bool Function::isValid(const QStringList& fileIDs,
					   bool checkChildren) const {
    throw std::runtime_error("error");
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

bool Function::getReplicate() const {
	return replicate_;
}

const QList<QSharedPointer<Function::Argument> >& Function::getArguments() {
	return arguments_;
}

void Function::setDisabled(bool disabled) {
	disabled_ = disabled;
}

QString Function::getReturnType() const {
	return returnType_;
}

void Function::setReturnType(QString &returnType) {
	returnType_ = returnType;
}

QString Function::getFileRef() const {
	return fileRef_;
}

const QList<QSharedPointer<Function::SourceFile> >& Function::getSourceFiles() {
	return sourceFiles_;
}

const QMap<QString, QString>& Function::getDisabledAttributes() {
	return disabledAttributes_;
}

QString Function::getEntryPoint() const {
	return entryPoint_;
}

void Function::setArguments(QList<QSharedPointer<Argument> > &arguments) {
	// delete old arguments
	arguments_.clear();

	// save new arguments
	arguments_ = arguments;
}

void Function::setDisabledAttributes(
		QMap<QString, QString> &disabledAttributes) {
	// delete old attributes
	disabledAttributes_.clear();

	// save new attributes
	disabledAttributes_ = disabledAttributes;
}

void Function::setEntryPoint(QString &entryPoint) {
	entryPoint_ = entryPoint;
}

void Function::setReplicate(bool replicate) {
	replicate_ = replicate;
}

bool Function::getDisabled() const {
	return disabled_;
}

void Function::setSourceFiles(QList<QSharedPointer<Function::SourceFile> > &sourceFiles) {
	sourceFiles_.clear();
	sourceFiles_ = sourceFiles;
}

void Function::setFileRef(QString &fileRef) {
	fileRef_ = fileRef;
}
