/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#include "filebuilder.h"
#include "buildmodel.h"


#include "../exceptions/write_error.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

// the constructor
FileBuilder::FileBuilder(QDomNode &fileBuilderNode):
BuildModel(fileBuilderNode), fileTypes_(), userFileTypes_() {

	for (int i = 0; i < fileBuilderNode.childNodes().count(); ++i) {
		QDomNode tempNode = fileBuilderNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:fileType")) {
			fileTypes_.append(tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:userFileType")) {
			userFileTypes_.append(tempNode.childNodes().at(0).nodeValue());
		}
	}
	// if mandatory field is missing
// 	if ((fileTypes_.size() == 0) && (userFileTypes_.size() == 0)) {
// 		throw Parse_error(QObject::tr(
// 				"No file types found in spirit:fileBuilder"));
// 	}
	return;
}

FileBuilder::FileBuilder():
BuildModel(), fileTypes_(), userFileTypes_() {

}

FileBuilder::FileBuilder( const FileBuilder &other ):
BuildModel(other),
fileTypes_(other.fileTypes_),
userFileTypes_(other.userFileTypes_) {
}

FileBuilder::FileBuilder( const QString& fileType ):
BuildModel(),
fileTypes_(),
userFileTypes_() {

	fileTypes_.append(fileType);
}

FileBuilder & FileBuilder::operator=( const FileBuilder &other ) {
	if (this != &other) {
		BuildModel::operator=(other);
		fileTypes_ = other.fileTypes_;
		userFileTypes_ = other.userFileTypes_;
	}
	return *this;
}

// the destructor
FileBuilder::~FileBuilder() {
}

void FileBuilder::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:defaultFileBuilder");

	// atleast one file type must be specified
	if ((fileTypes_.size() == 0) && (userFileTypes_.size() == 0)) {
		throw Write_error(QObject::tr("No file type specified in spirit:"
				"defaultFileBuilder"));
	}

	for (int i = 0; i < fileTypes_.size(); ++i) {
		writer.writeTextElement("spirit:fileType", fileTypes_.at(i));
	}

	for (int i = 0; i < userFileTypes_.size(); ++i) {
		writer.writeTextElement("spirit:userFileType", userFileTypes_.at(i));
	}

	// call the base class write() to write base class information
	BuildModel::write(writer);

	writer.writeEndElement(); // spirit:defaultFileBuilder
}

bool FileBuilder::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	if (fileTypes_.isEmpty() && userFileTypes_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory file type missing in file builder within %1").arg(
			parentIdentifier));
		return false;
	}
	// if at least one file type is specified then make sure it is not empty
	else {
		foreach (QString fileType, fileTypes_) {
			if (fileType.isEmpty()) {
				errorList.append(QObject::tr("Empty file type specified for file "
					"builder within %1").arg(parentIdentifier));
				return false;
			}
		}
		foreach (QString userFileType, userFileTypes_) {
			if (userFileType.isEmpty()) {
				errorList.append(QObject::tr("Empty user file type specified for file "
					"builder within %1").arg(parentIdentifier));
				return false;
			}
		}
	}
	return true;
}

bool FileBuilder::isValid() const {
	if (fileTypes_.isEmpty() && userFileTypes_.isEmpty()) {
		return false;
	}
	// if at least one file type is specified then make sure it is not empty
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
	return true;
}

const QList<QString>& FileBuilder::getFileTypes() {
	return fileTypes_;
}

void FileBuilder::setFileTypes(QList<QString> &fileTypes) {
	fileTypes_.clear();
	fileTypes_ = fileTypes;
}

const QList<QString>& FileBuilder::getUsetFileTypes() {
	return userFileTypes_;
}

void FileBuilder::setUserFileTypes(QList<QString>& userFileTypes) {
	userFileTypes_.clear();
	userFileTypes_ = userFileTypes;
}

QString FileBuilder::getFileType() const {
	if (!fileTypes_.isEmpty())
		return fileTypes_.first();
	else if (!userFileTypes_.isEmpty())
		return userFileTypes_.first();
	else
		return QString();
}

void FileBuilder::setFileType( const QString& fileType ) {

	// remove old file types
	fileTypes_.clear();
	userFileTypes_.clear();

	// if the file type is empty then don't add it
	if (fileType.isEmpty()) {
		return;
	}

	// if file type is one of the specified by IP-Xact
	if ((fileType == QString("asmSource")) ||
		(fileType == QString("cSource")) ||
		(fileType == QString("cppSource")) ||
		(fileType == QString("eSource")) ||
		(fileType == QString("OVASource")) ||
		(fileType == QString("perlSource")) ||
		(fileType == QString("pslSource")) ||
		(fileType == QString("SVASource")) ||
		(fileType == QString("tclSource")) ||
		(fileType == QString("veraSource")) ||
		(fileType == QString("systemCSource")) ||
		(fileType == QString("systemCSource-2.0")) ||
		(fileType == QString("systemCSource-2.0.1")) ||
		(fileType == QString("systemCSource-2.1")) ||
		(fileType == QString("systemCSource-2.2")) ||
		(fileType == QString("systemVerilogSource")) ||
		(fileType == QString("systemVerilogSource-3.0")) ||
		(fileType == QString("systemVerilogSource-3.1")) ||
		(fileType == QString("systemVerilogSource-3.1a")) ||
		(fileType == QString("verilogSource")) ||
		(fileType == QString("verilogSource-95")) ||
		(fileType == QString("verilogSource-2001")) ||
		(fileType == QString("vhdlSource")) ||
		(fileType == QString("vhdlSource-87")) ||
		(fileType == QString("vhdlSource-93")) ||
		(fileType == QString("swObject")) ||
		(fileType == QString("swObjectLibrary")) ||
		(fileType == QString("vhdlBinaryLibrary")) ||
		(fileType == QString("verilogBinaryLibrary")) ||
		(fileType == QString("executableHdl")) ||
		(fileType == QString("unelaboratedHdl")) ||
		(fileType == QString("SDC")) ||
		(fileType == QString("unknown"))) 
		
		fileTypes_.prepend(fileType);
	else
		userFileTypes_.prepend(fileType);
}

bool FileBuilder::hasFileType( const QString& fileType ) const {

	// check all file types
	foreach (QString builderFileType, fileTypes_) {
		
		// if match is found
		if (fileType == builderFileType)
			return true;
	}
	// check all user file types
	foreach (QString builderFileType, userFileTypes_) {

		// if match is found
		if (builderFileType == fileType)
			return true;
	}
	// no match was found
	return false;
}
