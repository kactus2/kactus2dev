/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#include "file.h"
#include "generaldeclarations.h"
#include "buildcommand.h"
#include "fileset.h"

#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QMap>
#include <QObject>
#include <QXmlStreamWriter>
#include <QFileInfo>

File::Define::Define( const QString name, const QString value ):
nameGroup_(name), value_(value) {
}

File::Define::Define( QDomNode& defineNode ):
nameGroup_(defineNode), value_() {

	for (int i = 0; i < defineNode.childNodes().count(); ++i) {
		QDomNode tempNode = defineNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
}

File::Define::Define(): nameGroup_(), value_() {
}

File::Define::Define( const Define& other ):
nameGroup_(other.nameGroup_),
value_(other.value_) {
}

File::Define& File::Define::operator=( const Define& other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		value_ = other.value_;
	}
	return *this;
}

bool File::Define::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr(
			"Mandatory name missing for define within %1").arg(parentIdentifier));
		valid = false;
	}
	if (value_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory value for define missing in %1").arg(
			parentIdentifier));
		valid = false;
	}
	return valid;
}

bool File::Define::isValid() const {
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}
	if (value_.isEmpty()) {
		return false;
	}
	return true;
}

// the constructor
File::File(QDomNode &fileNode, FileSet* parent): 
name_(),
fileId_(),
attributes_(),
nameAttributes_(),
fileTypes_(), 
userFileTypes_(),
includeFile_(false), 
externalDeclarations_(false),
logicalName_(),
logicalNameDefault_(false),
exportedNames_(), 
dependencies_(), 
imageTypes_(),
description_(),
buildcommand_(),
defines_(), 
parent_(parent) {

	// get the attributes for the file element
	QDomNamedNodeMap attributeMap = fileNode.attributes();
	for (int j = 0; j < attributeMap.size(); ++j) {
		QString name = attributeMap.item(j).nodeName();
		QString value = attributeMap.item(j).nodeValue();

		if (name == QString("spirit:fileId")) {
			fileId_ = value;
			fileId_ = General::removeWhiteSpace(fileId_);
		}
		else {
			attributes_[name] = value;
		}
	}

	// go through all the child nodes of the fileNode
	for (int i = 0; i < fileNode.childNodes().count(); ++i) {
		QDomNode tempNode = fileNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
			General::parseAttributes(tempNode, nameAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:fileType")) {
			fileTypes_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:userFileType")) {
			userFileTypes_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:isIncludeFile")) {
			QString includeFile = tempNode.childNodes().at(0).nodeValue();
			includeFile_ = General::str2Bool(includeFile, false);

			// get the externalDeclarations attribute
			QDomNamedNodeMap includeAttributes = tempNode.attributes();
			QString externalDeclarations = includeAttributes.namedItem(
					QString("spirit:externalDeclarations")).nodeValue();
			externalDeclarations_ = General::str2Bool(externalDeclarations,
					false);
		}

		else if (tempNode.nodeName() == QString("spirit:logicalName")) {
			logicalName_ = tempNode.childNodes().at(0).nodeValue();
			logicalName_ = General::removeWhiteSpace(logicalName_);

			// get the default-attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString nameDefault = attributeMap.namedItem(
					QString("spirit:default")).nodeValue();
			logicalNameDefault_ = General::str2Bool(nameDefault, false);
		}

		else if (tempNode.nodeName() == QString("spirit:exportedName")) {
			QString name = tempNode.childNodes().at(0).nodeValue();
			name = General::removeWhiteSpace(name);
			exportedNames_.append(name);
		}

		else if (tempNode.nodeName() == QString("spirit:buildCommand")) {
			buildcommand_ = QSharedPointer<BuildCommand>(
					new BuildCommand(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:dependency")) {
			dependencies_.append(tempNode.childNodes().at(0).nodeValue());
		}

		// get the defines
		else if (tempNode.nodeName() == QString("spirit:define")) {

			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode defineNode = tempNode.childNodes().at(j);

				defines_.append(Define(defineNode));
			}
		}

		else if (tempNode.nodeName() == QString("spirit:imageType")) {
			imageTypes_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
	return;
}

File::File(const QString filePath, FileSet* parent): 
name_(filePath), 
fileId_(),
attributes_(),
nameAttributes_(),
fileTypes_(), 
userFileTypes_(),
includeFile_(false),
externalDeclarations_(false),
logicalName_(),
logicalNameDefault_(false),
exportedNames_(),
dependencies_(),
imageTypes_(),
description_(),
buildcommand_(), 
defines_(), 
parent_(parent)  {

	QFileInfo fileInfo(filePath);

	// set the default file type if it can be derived from the file suffix

	if (fileInfo.suffix() == QString("vhd") ||
		(fileInfo.suffix() == QString("vhdl"))) {
		fileTypes_.append("vhdlSource");
	}
	else if (fileInfo.suffix() == QString("v")) {
		fileTypes_.append("verilogSource");
	}
	else if (fileInfo.suffix() == QString("sv")) {
		fileTypes_.append("systemVerilogSource");
	}
	else if (fileInfo.suffix() == QString("c")) {
		fileTypes_.append("cSource");
	}
	else if (fileInfo.suffix() == QString("h") ||
		fileInfo.suffix() == QString("hpp") ||
		fileInfo.suffix() == QString("cpp") ||
		fileInfo.suffix() == QString("cc")) {
		fileTypes_.append("cppSource");
	}
}

File::File( FileSet* parent ):
name_(), 
fileId_(),
attributes_(),
nameAttributes_(), 
fileTypes_(),
userFileTypes_(),
includeFile_(false),
externalDeclarations_(false),
logicalName_(),
logicalNameDefault_(false), 
exportedNames_(),
dependencies_(), 
imageTypes_(),
description_(),
buildcommand_(), 
defines_(), 
parent_(parent)  {
}

File::File( const File &other, FileSet* parent ):
name_(other.name_),
fileId_(other.fileId_),
attributes_(other.attributes_),
nameAttributes_(other.nameAttributes_),
fileTypes_(other.fileTypes_),
userFileTypes_(other.userFileTypes_),
includeFile_(other.includeFile_),
externalDeclarations_(other.externalDeclarations_),
logicalName_(other.logicalName_),
logicalNameDefault_(other.logicalNameDefault_),
exportedNames_(other.exportedNames_),
dependencies_(other.dependencies_),
imageTypes_(other.imageTypes_),
description_(other.description_),
buildcommand_(),
defines_(other.defines_),
parent_(parent) {

	if (other.buildcommand_) {
		buildcommand_ = QSharedPointer<BuildCommand>(
			new BuildCommand(*other.buildcommand_.data()));
	}
}

File & File::operator=( const File &other) {
	if (this != &other) {
	
		name_ = other.name_;
		fileId_ = other.fileId_;
		attributes_ = other.attributes_;
		nameAttributes_ = other.nameAttributes_;
		fileTypes_ = other.fileTypes_;
		userFileTypes_ = other.userFileTypes_;
		includeFile_ = other.includeFile_;
		externalDeclarations_ = other.externalDeclarations_;
		logicalName_ = other.logicalName_;
		logicalNameDefault_ = other.logicalNameDefault_;
		exportedNames_ = other.exportedNames_;
		dependencies_ = other.dependencies_;
		imageTypes_ = other.imageTypes_;
		description_ = other.description_;
		defines_ = other.defines_;
		parent_ = other.parent_;

		if (other.buildcommand_) {
			buildcommand_ = QSharedPointer<BuildCommand>(
				new BuildCommand(*other.buildcommand_.data()));
		}
		else
			buildcommand_ = QSharedPointer<BuildCommand>();
	}
	return *this;
}

// the destructor
File::~File() {
}

void File::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:file");

    // TODO: Calculate the hash for the file (will be used as the file id).

	// if fileId is defined
	if (!fileId_.isEmpty()) {
		writer.writeAttribute("spirit:fileId", fileId_);
	}
	General::writeAttributes(writer, attributes_);

    // start the spirit:name tag
    writer.writeStartElement("spirit:name");

    // write the attributes for the element
    General::writeAttributes(writer, nameAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(name_);
    writer.writeEndElement(); // spirit:name

    // write file types
    for (int i = 0; i < fileTypes_.size(); ++i) {
        writer.writeTextElement("spirit:fileType", fileTypes_.at(i));
    }

    // write user file types
    for (int i = 0; i < userFileTypes_.size(); ++i) {
        writer.writeTextElement("spirit:userFileType",
            userFileTypes_.at(i));
    }

	// start the spirit:includeFile tag
	writer.writeStartElement("spirit:isIncludeFile");

	// write the attributes for the element
	writer.writeAttribute("spirit:externalDeclarations",
			General::bool2Str(externalDeclarations_));

	// write the value of the element and close the tag
	writer.writeCharacters(General::bool2Str(includeFile_));
	writer.writeEndElement(); // spirit:isIncludeFile

	// if optional spirit:logicalName element exists
	if (!logicalName_.isEmpty()) {

		// start the spirit:logicalName tag
		writer.writeStartElement("spirit:logicalName");

		// write the attributes for the element
		writer.writeAttribute("spirit:default",
				General::bool2Str(logicalNameDefault_));

		// write the value of the element and close the tag
		writer.writeCharacters(logicalName_);
		writer.writeEndElement(); // spirit:logicalName
	}

	// write all spirit:exportedName elements
	for (int i = 0; i < exportedNames_.size(); ++i) {
		writer.writeTextElement("spirit:exportedName",
				exportedNames_.at(i));
	}

	// if buildCommand is defined, tell it to write itself
	if (buildcommand_) {
		buildcommand_->write(writer);
	}

	// write dependencies
	for (int i = 0; i < dependencies_.size(); ++i) {
		writer.writeTextElement("spirit:dependency",
				dependencies_.at(i));
	}

	// write all defines
	for (QList<File::Define>::iterator i = defines_.begin();
			i != defines_.end(); ++i) {
		writer.writeStartElement("spirit:define");

		// write the name-value pair
		writer.writeTextElement("spirit:name", i->nameGroup_.name_);
		writer.writeTextElement("spirit:displayName", i->nameGroup_.displayName_);
		writer.writeTextElement("spirit:description", i->nameGroup_.description_);
		writer.writeTextElement("spirit:value", i->value_);

		writer.writeEndElement(); // spirit:define
	}

	// write all image types
	for (int i = 0; i < imageTypes_.size(); ++i) {
		writer.writeTextElement("spirit:imageType", imageTypes_.at(i));
	}

	// if description exists, write it
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	writer.writeEndElement(); // spirit:file
}

bool File::isValid( QStringList& errorList, 
				   const QString& parentIdentifier,
				   bool checkChildren /*= true*/ ) const {
	bool valid = true;
	
	if (name_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory name missing for file within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (fileTypes_.isEmpty() && userFileTypes_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory file type missing for file %1 within %2").arg(
			name_).arg(parentIdentifier));
		valid = false;
	}
	// if at least one file type was specified
	else {

		foreach (QString fileType, fileTypes_) {
			if (fileType.isEmpty()) {
				valid = false;
				break;
			}
		}
		foreach (QString userFileType, userFileTypes_) {
			if (userFileType.isEmpty()) {
				valid = false;
				break;
			}
		}
	}

	if (checkChildren) {
		foreach (Define define, defines_) {
			if (!define.isValid(errorList, QObject::tr("file %1").arg(name_))) {
				valid = false;
			}
		}
	}

	return valid;
}

bool File::isValid( bool checkChildren /*= true*/ ) const {
	if (name_.isEmpty()) {
		return false;
	}

	if (fileTypes_.isEmpty() && userFileTypes_.isEmpty()) {
		return false;
	}
	// if at least one file type was specified
	else {

		foreach (QString fileType, fileTypes_) {
			if (fileType.isEmpty()) {
				return false;
			}
		}
		foreach (QString userFileType, userFileTypes_) {
			if (userFileType.isEmpty()) {
				return false;
			}
		}
	}

	if (checkChildren) {
		foreach (Define define, defines_) {
			if (!define.isValid()) {
				return false;
			}
		}
	}

	return true;
}

void File::setAttributes(const QMap<QString, QString> &attributes) {
	attributes_.clear();

	// save the new attributes
	attributes_ = attributes;
}

void File::setExportedNames(const QList<QString> &exportedNames) {
	exportedNames_.clear();
	exportedNames_ = exportedNames;
}

void File::setIncludeFile(bool includeFile) {
	includeFile_ = includeFile;
}

void File::setImageTypes(const QList<QString> &imageTypes) {
	imageTypes_.clear();
	imageTypes_ = imageTypes;
}

void File::setExternalDeclarations(bool externalDeclarations) {
	externalDeclarations_ = externalDeclarations;
}

void File::setDependencies(const QList<QString> &dependencies) {
	dependencies_.clear();
	dependencies_ = dependencies;
}

void File::setDescription(const QString &description) {
	description_ = description;
}

const QList<QString>& File::getExportedNames() {
	return exportedNames_;
}

const QMap<QString, QString>& File::getNameAttributes() {
	return nameAttributes_;
}

QString File::getFileId() const {
	return fileId_;
}

void File::setNameAttributes(const QMap<QString, QString> &nameAttributes) {
	// delete old name attributes
	nameAttributes_.clear();

	// save the new name attributes
	nameAttributes_ = nameAttributes;
}

bool File::getIncludeFile() const {
	return includeFile_;
}

void File::setLogicalNameDefault(bool logicalNameDefault) {
	logicalNameDefault_ = logicalNameDefault;
}

const QStringList& File::getUserFileTypes() {
	return userFileTypes_;
}

BuildCommand *File::getBuildcommand() {
	if (!buildcommand_) {
		buildcommand_ = QSharedPointer<BuildCommand>(new BuildCommand());
	}
	return buildcommand_.data();
}

bool File::getExternalDeclarations() const {
	return externalDeclarations_;
}

QString File::getDescription() const {
	return description_;
}

QString File::getLogicalName() const {
	return logicalName_;
}

void File::setName(const QString &name) {
	name_ = name;
}

const QList<QString>& File::getImageTypes() {
	return imageTypes_;
}

const QList<QString>& File::getDependencies() {
	return dependencies_;
}

void File::setFileId(const QString &fileId) {
	fileId_ = fileId;
}

void File::setLogicalName(const QString &logicalName) {
	logicalName_ = logicalName;
}

void File::setUserFileTypes( const QStringList& userFileTypes ) {
	userFileTypes_.clear();
	userFileTypes_ = userFileTypes;
}

void File::setBuildcommand(BuildCommand *buildcommand) {

	// delete the old build command if one exists
	if (buildcommand_) {
		buildcommand_.clear();
	}
	buildcommand_ = QSharedPointer<BuildCommand>(buildcommand);
}

bool File::getLogicalNameDefault() const {
	return logicalNameDefault_;
}

const QStringList& File::getFileTypes() const {
	return fileTypes_;
}

void File::setFileTypes( const QStringList& fileTypes ) {
	fileTypes_.clear();
	fileTypes_ = fileTypes;
}

const QMap<QString, QString>& File::getAttributes() {
	return attributes_;
}

QString File::getName() const {
	return name_;
}

void File::setDefines(const QList<File::Define> &defines) {
	// delete old defines
	defines_.clear();

	// save the new defines
	defines_ = defines;
}

const QList<File::Define>& File::getDefines() {
	return defines_;
}

void File::addFileType(const QString fileType) {

	// add the file type if it is not already defined
	if (!fileTypes_.contains(fileType)) {
		fileTypes_.append(fileType);
	}
}

void File::addUserFileType(const QString userFileType) {

	// add the file type if it is not already defined
	if (!userFileTypes_.contains(userFileType)) {
		userFileTypes_.append(userFileType);
	}
}

void File::remove() {
	parent_->removeFile(this);
}

void File::setParent( FileSet* fileSet ) {
	parent_ = fileSet;
}

void File::clearFileTypes() {
	fileTypes_.clear();
	userFileTypes_.clear();
}

void File::clearDefines() {

	defines_.clear();
}

void File::appendDefine( const Define& define ) {

	defines_.append(define);
}

QString File::getCommand() const {

	// if build command is specified for this file specifically
	if (buildcommand_ && !buildcommand_->getCommand().isEmpty()) 
		return buildcommand_->getCommand();
	
	// if build command is not specified then is must be asked from the parent
	// file set
	else {
		return parent_->getDefaultCommand(this);
	}
}

QString File::getFlags() const {

	QString flags;

	// if build command is defined for this file specifically
	if (buildcommand_) {
		
		// if flags should replace the default ones
		if (buildcommand_->getReplaceDefaultFlags())
			return buildcommand_->getFlags();

		// if flags should appended with default ones
		else 
			flags += buildcommand_->getFlags();
	}

	// append the flags from file set and return all flags
	flags += parent_->getDefaultFlags(this);
	return flags;
}

bool File::isRTLFile() const {

	// check all file types
	foreach (QString fileType, fileTypes_) {

		if (fileType == QString("vhdlSource") || 
			fileType == QString("vhdlSource-87") ||
			fileType == QString("vhdlSource-93") ||
			fileType == QString("verilogSource") ||
			fileType == QString("verilogSource-95") ||
			fileType == QString("verilogSource-2001"))
			return true;
	}
	return false;
}

void File::setCommand( const QString& buildCommand ) {

	if (!buildcommand_)
		buildcommand_ = QSharedPointer<BuildCommand>(new BuildCommand());
	buildcommand_->setCommand(buildCommand);
}

void File::setBuildFlags( const QString& buildFlags, bool replaceDefault ) {

	if (!buildcommand_)
		buildcommand_ = QSharedPointer<BuildCommand>(new BuildCommand());
	buildcommand_->setFlags(buildFlags);
	buildcommand_->setReplaceDefaultFlags(replaceDefault);
}

bool File::isQuartusPinmap() const {

	foreach (QString type, userFileTypes_) {
		if (type == QString("quartusPinmap"))
			return true;
	}
	return false;
}

bool File::operator==( const File& other ) const {
	return name_ == other.name_;
}

bool File::operator!=( const File& other ) const {
	return name_ != other.name_;
}

bool File::isVhdlFile() const {

	foreach (QString fileType, fileTypes_) {

		// if file type is for vhdl files
		if (fileType.compare(QString("vhdlSource"), Qt::CaseInsensitive) == 0 ||
			fileType.compare(QString("vhdlSource-87"), Qt::CaseInsensitive) == 0 ||
			fileType.compare(QString("vhdlSource-93"), Qt::CaseInsensitive) == 0) {
				return true;
		}
	}
	return false;
}

QString File::fileSetName() const {
	return parent_->getName();
}

QStringList File::getAllFileTypes() const {
	QStringList types = fileTypes_;
	types.append(userFileTypes_);
	return types;
}

void File::setAllFileTypes( const QStringList& fileTypes ) {

	fileTypes_.clear();
	userFileTypes_.clear();

	foreach (QString fileType, fileTypes) {

		// if the file type is one of the specified ones
		if (General::isSpecifiedFileType(fileType)) {
			fileTypes_.append(fileType);
		}
		// if the file type is user defined
		else {
			userFileTypes_.append(fileType);
		}
	}
}
