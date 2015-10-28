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
#include "FileTypes.h"

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
    if (FileTypes::isIpXactFileType(newFileType))
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
