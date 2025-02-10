//-----------------------------------------------------------------------------
// File: FieldReset.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.01.2019
//
// Description:
// Describes the ipxact:reset element.
//-----------------------------------------------------------------------------

#include "FieldReset.h"

//-----------------------------------------------------------------------------
// Function: FieldReset::FieldReset()
//-----------------------------------------------------------------------------
FieldReset::FieldReset():
resetTypeReference_(),
resetValue_(),
resetMask_()
{

}

//-----------------------------------------------------------------------------
// Function: FieldReset::FieldReset()
//-----------------------------------------------------------------------------
FieldReset::FieldReset(const FieldReset& other):
resetTypeReference_(other.resetTypeReference_),
resetValue_(other.resetValue_),
resetMask_(other.resetMask_)
{

}

//-----------------------------------------------------------------------------
// Function: FieldReset::operator=()
//-----------------------------------------------------------------------------
FieldReset& FieldReset::operator=(const FieldReset& other)
{
    if (this != &other)
    {
        resetTypeReference_ = other.resetTypeReference_;
        resetValue_ = other.resetValue_;
        resetMask_ = other.resetMask_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::~FieldReset()
//-----------------------------------------------------------------------------
FieldReset::~FieldReset()
{

}

//-----------------------------------------------------------------------------
// Function: FieldReset::getResetTypeReference()
//-----------------------------------------------------------------------------
QString FieldReset::getResetTypeReference() const
{
    return resetTypeReference_;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::setResetTypeReference()
//-----------------------------------------------------------------------------
void FieldReset::setResetTypeReference(QString const& newResetTypeReference)
{
    resetTypeReference_ = newResetTypeReference;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::getResetValue()
//-----------------------------------------------------------------------------
QString FieldReset::getResetValue() const
{
    return resetValue_;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::setResetValue()
//-----------------------------------------------------------------------------
void FieldReset::setResetValue(QString const& newResetValue)
{
    resetValue_ = newResetValue;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::getResetMask()
//-----------------------------------------------------------------------------
QString FieldReset::getResetMask() const
{
    return resetMask_;
}

//-----------------------------------------------------------------------------
// Function: FieldReset::setResetMask()
//-----------------------------------------------------------------------------
void FieldReset::setResetMask(QString const& newResetMask)
{
    resetMask_ = newResetMask;
}
