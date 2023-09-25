//-----------------------------------------------------------------------------
// File: FileSetRef.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 4.9.2023
//
// Description:
// Describes the ipxact:fileSetRef element.
//-----------------------------------------------------------------------------

#include "FileSetRef.h"

//-----------------------------------------------------------------------------
// Function: FileSetRef::FileSetRef()
//-----------------------------------------------------------------------------
FileSetRef::FileSetRef(FileSetRef const& other) :
    Extendable(other),
    localName_(other.localName_),
    isPresent_(other.isPresent_)
{

}

//-----------------------------------------------------------------------------
// Function: FileSetRef::FileSetRef()
//-----------------------------------------------------------------------------
FileSetRef::FileSetRef() : Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetRef::getLocalName()
//-----------------------------------------------------------------------------
QString FileSetRef::getReference() const
{
    return localName_;
}

//-----------------------------------------------------------------------------
// Function: FileSetRef::setLocalName()
//-----------------------------------------------------------------------------
void FileSetRef::setReference(QString const& newName)
{
    localName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: FileSetRef::getIsPresent()
//-----------------------------------------------------------------------------
QString FileSetRef::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: FileSetRef::setIsPresent()
//-----------------------------------------------------------------------------
void FileSetRef::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: FileSetRef::operator=()
//-----------------------------------------------------------------------------
FileSetRef& FileSetRef::operator=(FileSetRef const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);
        localName_ = other.localName_;
        isPresent_ = other.isPresent_;
    }

    return *this;
}
