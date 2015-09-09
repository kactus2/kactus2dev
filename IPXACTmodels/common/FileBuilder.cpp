//-----------------------------------------------------------------------------
// File: FileBuilder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Implementation for ipxact:fileBuilder element.
//-----------------------------------------------------------------------------

#include "FileBuilder.h"

/*
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
}*/

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder() :
BuildModel(),
Extendable(),
fileType_(),
userFileType_()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder( const QString& fileType ):
BuildModel(),
Extendable(),
fileType_(),
userFileType_()
{
    setFileType(fileType);
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder( const FileBuilder &other ):
BuildModel(other),
Extendable(other),
fileType_(other.fileType_),
userFileType_(other.userFileType_)
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilder::operator=()
//-----------------------------------------------------------------------------
FileBuilder & FileBuilder::operator=( const FileBuilder &other )
{
	if (this != &other)
    {
        BuildModel::operator=(other);
        Extendable::operator=(other);

        fileType_ = other.fileType_;
        userFileType_ = other.userFileType_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::~FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::~FileBuilder()
{

}

/*
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
}*/
/*
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
}*/

//-----------------------------------------------------------------------------
// Function: FileBuilder::getFileType()
//-----------------------------------------------------------------------------
QString FileBuilder::getFileType() const
{
    return fileType_;
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::getUserFileType()
//-----------------------------------------------------------------------------
QString FileBuilder::getUserFileType() const
{
    return userFileType_;
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::setFileType()
//-----------------------------------------------------------------------------
void FileBuilder::setFileType( const QString& newFileType )
{
	if ((newFileType == QString("asmSource")) || (newFileType == QString("cSource")) ||
        (newFileType == QString("cppSource")) || (newFileType == QString("eSource")) ||
        (newFileType == QString("OVASource")) || (newFileType == QString("perlSource")) ||
        (newFileType == QString("pslSource")) || (newFileType == QString("SVASource")) ||
        (newFileType == QString("tclSource")) || (newFileType == QString("veraSource")) ||
        (newFileType == QString("systemCSource")) || (newFileType == QString("systemCSource-2.0")) ||
        (newFileType == QString("systemCSource-2.0.1")) || (newFileType == QString("systemCSource-2.1")) ||
        (newFileType == QString("systemCSource-2.2")) || (newFileType == QString("systemVerilogSource")) ||
        (newFileType == QString("systemVerilogSource-3.0")) || (newFileType == QString("systemVerilogSource-3.1")) ||
        (newFileType == QString("systemVerilogSource-3.1a")) || (newFileType == QString("verilogSource")) ||
        (newFileType == QString("verilogSource-95")) || (newFileType == QString("verilogSource-2001")) ||
        (newFileType == QString("vhdlSource")) || (newFileType == QString("vhdlSource-87")) ||
        (newFileType == QString("vhdlSource-93")) ||
        (newFileType == QString("swObject")) || (newFileType == QString("swObjectLibrary")) ||
        (newFileType == QString("vhdlBinaryLibrary")) || (newFileType == QString("verilogBinaryLibrary")) ||
        (newFileType == QString("executableHdl")) || (newFileType == QString("unelaboratedHdl")) ||
        (newFileType == QString("SDC")) || (newFileType == QString("unknown"))) 
    {
        fileType_ = newFileType;
        userFileType_.clear();
    }
	else if (!newFileType.isEmpty())
    {
        fileType_ = "user";
        userFileType_ = newFileType;
    }
    else
    {
        fileType_.clear();
        userFileType_.clear();
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::hasFileType()
//-----------------------------------------------------------------------------
bool FileBuilder::hasFileType( const QString& fileType ) const
{
    if (fileType_ == fileType || userFileType_ == fileType)
    {
        return true;
    }

    return false;
}
