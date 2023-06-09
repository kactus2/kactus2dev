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
#include "FileType.h"

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder() :
BuildModel(),
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder( const QString& fileType ):
BuildModel(),
Extendable()
{
    setFileType(fileType);
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::FileBuilder()
//-----------------------------------------------------------------------------
FileBuilder::FileBuilder( const FileBuilder &other ):
BuildModel(other),
Extendable(other),
fileType_(other.fileType_)
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
FileType FileBuilder::getFileType() const
{
    return fileType_;
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::setFileType()
//-----------------------------------------------------------------------------
void FileBuilder::setFileType(QString const& newFileType)
{
    fileType_.type_ = newFileType;
}

//-----------------------------------------------------------------------------
// Function: FileBuilder::hasFileType()
//-----------------------------------------------------------------------------
bool FileBuilder::hasFileType(QString const& fileType) const
{
    return fileType_.type_ == fileType;
}
