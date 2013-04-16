/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#include "fileset.h"
#include "file.h"
#include "filebuilder.h"
#include "function.h"
#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include <QFileInfo>

FileSet::FileSet(QDomNode & fileSetNode):
nameGroup_(fileSetNode),
	groups_(),
files_(), 
defaultFileBuilders_(),
dependencies_(),
functions_(),
fileSetId_() {

	for (int i = 0; i < fileSetNode.childNodes().count(); ++i) {
		QDomNode tempNode = fileSetNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:group")) {
			QString groupName = tempNode.childNodes().at(0).nodeValue();
			groupName = General::removeWhiteSpace(groupName);
			groups_.append(groupName);
		}

		else if (tempNode.nodeName() == QString("spirit:file")) {
			files_.append(QSharedPointer<File>(new File(tempNode, this)));
		}

		else if (tempNode.nodeName() == QString(
				"spirit:defaultFileBuilder")) {
			defaultFileBuilders_.append(
					QSharedPointer<FileBuilder>(new FileBuilder(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:dependency") ) {
			dependencies_.append(tempNode.childNodes().at(0).nodeValue());
		}

		else if (tempNode.nodeName() == QString("spirit:function")) {
			functions_.append(
					QSharedPointer<Function>(new Function(tempNode)));
		}
		else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode extensionNode = tempNode.childNodes().at(j);

				if (extensionNode.nodeName() == QString("kactus2:fileSetId")) {
					fileSetId_ = extensionNode.childNodes().at(0).nodeValue();
				}
			}
		}
	}
}

FileSet::FileSet(const QString& name, const QString& group):
nameGroup_(),
	groups_(),
	files_(), defaultFileBuilders_(),
	dependencies_(), 
	functions_(),
fileSetId_() {

	if (!group.isEmpty()) {
		groups_.append(group);
	}
		
	nameGroup_.name_ = name;
}

FileSet::FileSet():
nameGroup_(), 
	groups_(), 
	files_(), 
	defaultFileBuilders_(), 
	dependencies_(), 
	functions_(),
fileSetId_() {
}

FileSet::FileSet( const FileSet &other ):
nameGroup_(other.nameGroup_),
groups_(other.groups_),
files_(),
defaultFileBuilders_(),
dependencies_(other.dependencies_),
functions_(),
fileSetId_(other.fileSetId_) {

	foreach (QSharedPointer<File> file, other.files_) {
		if (file) {
			QSharedPointer<File> copy = QSharedPointer<File>(
				new File(*file.data(), this));
			files_.append(copy);
		}
	}

	foreach (QSharedPointer<FileBuilder> fileBuilder, other.defaultFileBuilders_) {
		if (fileBuilder) {
			QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(
				new FileBuilder(*fileBuilder.data()));
			defaultFileBuilders_.append(copy);
		}
	}

	foreach (QSharedPointer<Function> func, other.functions_) {
		if (func) {
			QSharedPointer<Function> copy = QSharedPointer<Function>(
				new Function(*func.data()));
			functions_.append(copy);
		}
	}
}

FileSet & FileSet::operator=( const FileSet &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		groups_ = other.groups_;
		dependencies_ = other.dependencies_;
		fileSetId_ = other.fileSetId_;

		files_.clear();
		foreach (QSharedPointer<File> file, other.files_) {
			if (file) {
				QSharedPointer<File> copy = QSharedPointer<File>(
					new File(*file.data(), this));
				files_.append(copy);
			}
		}

		defaultFileBuilders_.clear();
		foreach (QSharedPointer<FileBuilder> fileBuilder, other.defaultFileBuilders_) {
			if (fileBuilder) {
				QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(
					new FileBuilder(*fileBuilder.data()));
				defaultFileBuilders_.append(copy);
			}
		}

		functions_.clear();
		foreach (QSharedPointer<Function> func, other.functions_) {
			if (func) {
				QSharedPointer<Function> copy = QSharedPointer<Function>(
					new Function(*func.data()));
				functions_.append(copy);
			}
		}
	}
	return *this;
}

FileSet::~FileSet() {
	files_.clear();
	defaultFileBuilders_.clear();
	functions_.clear();
}

void FileSet::write(QXmlStreamWriter& writer) {

	writer.writeStartElement("spirit:fileSet");

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	// if optional groups are defined
	for (int i = 0; i < groups_.size(); ++i) {
		writer.writeTextElement("spirit:group", groups_.at(i));
	}

	// tell each file to write itself
	for (int i = 0; i < files_.size(); ++i) {
		files_.at(i)->write(writer);
	}

	// tell each defaultFileBuilder to write itself
	for (int i = 0; i < defaultFileBuilders_.size(); ++i) {
		defaultFileBuilders_.at(i)->write(writer);
	}

	// write all dependencies
	for (int i = 0; i < dependencies_.size(); ++i) {
		writer.writeTextElement("spirit:dependency", dependencies_.at(i));
	}

	// tell each function to write itself
	for (int i = 0; i < functions_.size(); ++i) {
		functions_.at(i)->write(writer);
	}

	// if file set id has been set
	if (!fileSetId_.isEmpty()) {
		writer.writeStartElement("spirit:vendorExtensions");
		writer.writeTextElement("kactus2:fileSetId", fileSetId_);
		writer.writeEndElement(); // spirit:vendorExtensions
	}

	writer.writeEndElement(); // spirit:fileSet
}

bool FileSet::isValid( QStringList& errorList, 
					  const QString& parentIdentifier, 
					  bool checkChildren /*= true*/ ) const {
	bool valid = true;
	const QString thisIdentifier = QObject::tr("file set %1").arg(nameGroup_.name_);

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("Name of the file set missing within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (checkChildren) {
		QStringList fileIDs;
		QStringList fileNames;
		foreach (QSharedPointer<File> file, files_) {

			if (fileNames.contains(file->getName())) {
				errorList.append(QObject::tr("%1 contains several files with"
					" name %2").arg(thisIdentifier).arg(file->getName()));
				valid = false;
			}
			else {
				fileNames.append(file->getName());
			}

			if (!file->isValid(errorList, thisIdentifier)) {
				valid = false;
			}

			// if file has file id specified then save it to list to be used later.
			if (!file->getFileId().isEmpty()) {
				fileIDs.append(file->getFileId());
			}
		}

		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid(errorList, thisIdentifier)) {
				valid = false;
			}
		}

		foreach (QSharedPointer<Function> func, functions_) {
			if (!func->isValid(fileIDs, errorList, thisIdentifier)) {
				valid = false;
			}
		}
	}

	return valid;
}

bool FileSet::isValid( bool checkChildren ) const {
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (checkChildren) {
		QStringList fileIDs;
		QStringList fileNames;
		foreach (QSharedPointer<File> file, files_) {

			if (fileNames.contains(file->getName())) {
				return false;
			}
			else {
				fileNames.append(file->getName());
			}

			if (!file->isValid()) {
				return false;
			}

			// if file has file id specified then save it to list to be used later.
			if (!file->getFileId().isEmpty()) {
				fileIDs.append(file->getFileId());
			}
		}

		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {
			if (!fileBuilder->isValid()) {
				return false;
			}
		}

		foreach (QSharedPointer<Function> func, functions_) {
			if (!func->isValid(fileIDs, true)) {
				return false;
			}
		}
	}
	return true;
}

const QList<QSharedPointer<Function> >& FileSet::getFunctions() {
	return functions_;
}

QList<QSharedPointer<File> >& FileSet::getFiles() {
	return files_;
}

const QList<QSharedPointer<File> >& FileSet::getFiles() const {
	return files_;
}

QStringList FileSet::getFiles( const QStringList& fileTypes ) const {
	QStringList files;

	foreach (QSharedPointer<File> file, files_) {
		// if the file matches one of the file types
		if (file->matchesFileType(fileTypes)) {
			files.append(file->getName());
		}
	}

	return files;
}

void FileSet::setFiles(QList<QSharedPointer<File> > &files) {
	// delete old files
	files_.clear();

	// save the new files
	files_ = files;
}

void FileSet::setFunctions(QList<QSharedPointer<Function> > &functions) {
	// delete the old functions
	functions_.clear();

	// save the new functions
	functions_ = functions;
}

void FileSet::setDependencies(const QStringList &dependencies) {
	dependencies_.clear();
	dependencies_ = dependencies;
}

void FileSet::setDefaultFileBuilders(
		QList<QSharedPointer<FileBuilder> > &defaultFileBuilders) {
	// delete the old file builders
	defaultFileBuilders_.clear();

	// save the new file builders
	defaultFileBuilders_ = defaultFileBuilders;
}

QString FileSet::getName() const {
	return nameGroup_.name_;
}

void FileSet::setName(const QString &name) {
	nameGroup_.name_ = name;
}

const QStringList& FileSet::getGroups() {
	return groups_;
}

QList<QSharedPointer<FileBuilder> >& FileSet::getDefaultFileBuilders() {
	return defaultFileBuilders_;
}

const QList<QSharedPointer<FileBuilder> >& FileSet::getDefaultFileBuilders() const {
	return defaultFileBuilders_;
}

void FileSet::setGroups(const QStringList &groups) {
	groups_.clear();
	groups_ = groups;
}

void FileSet::setGroups( const QString& groupName ) {
	groups_.clear();
	groups_.append(groupName);
}

const QStringList& FileSet::getDependencies() {
	return dependencies_;
}

const QStringList FileSet::getFilePaths() {
	QStringList filePaths;

	for (int i = 0; i < files_.size(); ++i) {
		filePaths.append(files_.at(i)->getName());
	}
	return filePaths;
}

QList<General::LibraryFilePair> FileSet::getVhdlLibraries() const {

	QList<General::LibraryFilePair> libraries;

	// go through all files
	foreach (QSharedPointer<File> file, files_) {

		if (file->isVhdlFile()) {
			QString filePath = file->getName();
			QString libName = file->getLogicalName();
			if (libName.isEmpty()) {
				libName = QString("work");
			}
			libraries.append(General::LibraryFilePair(filePath, libName));
		}
	}
	return libraries;
}

QStringList FileSet::getVhdlLibraryNames() const {

	QStringList libraries;
	foreach (QSharedPointer<File> file, files_) {
		// if file is vhdl and the library name is not yet on the list.
		// and library is specified
		if (file->isVhdlFile() && !file->getLogicalName().isEmpty() && 
			!libraries.contains(file->getLogicalName())) {
			libraries.append(file->getLogicalName());
		}
	}
	return libraries;
}

QList<General::LibraryFilePair> FileSet::getVerilogLibraries() const {
	QList<General::LibraryFilePair> files;

	for (int i = 0; i < files_.size(); ++i) {

		// if fileType was defined and it is vhdlSource of some kind
		if ((files_.at(i)->getFileTypes().size() != 0) &&
			(files_.at(i)->getFileTypes().at(0) == QString("verilogSource") ||
			files_.at(i)->getFileTypes().at(0) == QString("verilogSource-95")
			|| files_.at(i)->getFileTypes().at(0) == QString(
			"verilogSource-2001"))) {

				QString filePath = files_.at(i)->getName();

				// add the library name to the list of libraries
				QString libName = files_.at(i)->getLogicalName();

				// if library name was not defined
				if (libName.isEmpty()) {
					// set the library name to be "work"
					libName = QString("work");
				}

				files.append(General::LibraryFilePair(filePath, libName));
		}
	}

	return files;
}

void FileSet::addFile(QSharedPointer<File> file) {

	// search the files if there already exists a file with same name
	for (int i = 0; i < files_.size(); ++i) {

		// if files have same name
		if (files_.at(i)->getName() == file->getName()) {

			// remove the old file and replace it with the new one
			files_.removeAt(i);

			// add the new file to the list
			files_.append(file);
			
			// tell the file that it now belongs to this file set.
			file->setParent(this);
			return;
		}
	}

	// if there was no file with same name it is safe to append to file to the
	// file set
	files_.append(file);
}

QSharedPointer<File> FileSet::addFile(const QString& filePath, QSettings& settings) {

	QSharedPointer<File> file;

	// check if file already exists
	foreach (QSharedPointer<File> fileSearch, files_) {
		if (fileSearch->getName() == filePath) {
			file = fileSearch;
		}
	}

	// file did not yet exist so create the file and add it to list
	if (!file) {
		file = QSharedPointer<File>(new File(filePath, this));
		files_.append(file);
	}

	// set the file types for the file
	QFileInfo fileInfo(filePath);
	QStringList types = General::getFileTypes(settings, fileInfo);
	file->setAllFileTypes(types);

	return file;
}

QSharedPointer<File> FileSet::getFile(const QString logicalName) const {
	// search all files
	for (int i = 0; i < files_.size(); ++i) {

		// if the logical name matches
		if (files_.at(i)->getLogicalName() == logicalName) {
			return files_.at(i);
		}
	}

	// no logical names matched
	return QSharedPointer<File>();
}

QStringList FileSet::getFileNames() const {

	QStringList files;

	// go through all file sets
	for (int i = 0; i < files_.size(); ++i) {
		files.append(files_.at(i)->getName());
	}
	return files;
}

QString FileSet::getDisplayName() const
{
	return nameGroup_.displayName_;
}

QString FileSet::getDescription() const
{
	return nameGroup_.description_;
}

void FileSet::setDisplayName( const QString& displayName )
{
	nameGroup_.displayName_ = displayName;
}

void FileSet::setDescription( const QString& description )
{
	nameGroup_.description_ = description;
}

void FileSet::removeFile( const QString& fileName ) {
	
	for (int i = 0; i < files_.size(); ++i) {

		// if the file is the specified one
		if (files_.at(i)->getName() == fileName) {
			files_.value(i).clear();
			files_.removeAt(i);
			return;
		}
	}
}

void FileSet::removeFile( const File* file ) {
	for (int i = 0; i < files_.size(); ++i) {

		// if the file is the specified one
		if (*files_.at(i) == *file) {
			files_[i].clear();
			files_.removeAt(i);
			break;
		}
	}
}

QSharedPointer<File> FileSet::createFile() {

	QSharedPointer<File> file(new File(this));
	files_.append(file);
	return file;
}

QString FileSet::getDefaultCommand( const File* file ) const {

	if (!file) {
		return QString();
	}

	// first search all file types
	QStringList fileTypes = file->getFileTypes();
	foreach (QString fileType, fileTypes) {
		
		// search each default file builder for each file type
		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {

			if (fileBuilder->hasFileType(fileType)) {
				return fileBuilder->getCommand();
			}
		}
	}

	// no match was found
	return QString();
}

QString FileSet::getDefaultFlags( const File* file ) const {

	if (!file) {
		return QString();
	}

	// first search all file types
	QStringList fileTypes = file->getFileTypes();
	foreach (QString fileType, fileTypes) {

		// search each default file builder for each file type
		foreach (QSharedPointer<FileBuilder> fileBuilder, defaultFileBuilders_) {

			if (fileBuilder->hasFileType(fileType)) {
				return fileBuilder->getFlags();
			}
		}
	}

	// no match was found
	return QString();
}

QList<QSharedPointer<File> > FileSet::getRTLFiles() const {

	QList<QSharedPointer<File> > list;
	foreach (QSharedPointer<File> file, files_) {
		if (file->isRTLFile()) {
			list.append(file);
		}
	}
	return list;
}

bool FileSet::contains( const QString& fileName ) const {

	foreach (QSharedPointer<File> file, files_) {
		
		if (file->getName() == fileName)
			return true;
	}
	return false;
}

void FileSet::changeFileName( const QString& from, const QString& to ) {

	foreach (QSharedPointer<File> file, files_) {

		if (file->getName() == from) {
			file->setName(to);
			return;
		}
	}
}

File* FileSet::getQuartusPinMap() const {

	foreach (QSharedPointer<File> file, files_) {

		if (file->isQuartusPinmap())
			return file.data();
	}
	// no pin map was found
	return 0;
}

void FileSet::sortFiles( const QStringList& fileNames ) {

	QList<QSharedPointer<File> > tempList;

	// go through each file name in the correct order
	foreach (QString fileName, fileNames) {

		// try to find a matching file
		foreach (QSharedPointer<File> file, files_) {
			
			// if file is the one searched for
			if (file->getName() == fileName)
				tempList.append(file);
		}
	}

	// clear the original list
	files_.clear();

	// and set it to the sorted list.
	files_ = tempList;
}

void FileSet::useDefaultVhdlBuilders() {
	QSharedPointer<FileBuilder> vhdlBuilder(new FileBuilder("vhdlSource"));
	vhdlBuilder->setCommand("vcom");
	vhdlBuilder->setFlags("-quiet -check_synthesis -work work");
	vhdlBuilder->setReplaceDefaultFlags(true);
	defaultFileBuilders_.append(vhdlBuilder);

	QSharedPointer<FileBuilder> vhdl87Builder(new FileBuilder("vhdlSource-87"));
	vhdl87Builder->setCommand("vcom");
	vhdl87Builder->setFlags("-quiet -check_synthesis -work work");
	vhdl87Builder->setReplaceDefaultFlags(true);
	defaultFileBuilders_.append(vhdl87Builder);

	QSharedPointer<FileBuilder> vhdl93Builder(new FileBuilder("vhdlSource-93"));
	vhdl93Builder->setCommand("vcom");
	vhdl93Builder->setFlags("-quiet -check_synthesis -work work");
	vhdl93Builder->setReplaceDefaultFlags(true);
	defaultFileBuilders_.append(vhdl93Builder);
}

void FileSet::clearFiles() {
	files_.clear();
}

General::NameGroup& FileSet::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& FileSet::getNameGroup() const {
	return nameGroup_;
}

QString FileSet::getFileSetId() const {
	return fileSetId_;
}

void FileSet::setFileSetId( const QString& id ) {
	fileSetId_ = id;
}
