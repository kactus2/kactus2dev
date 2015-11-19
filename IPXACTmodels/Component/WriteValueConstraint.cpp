//-----------------------------------------------------------------------------
// File: WriteValueConstraint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Describes the ipxact:writeValueConstraint element.
//-----------------------------------------------------------------------------

#include "WriteValueConstraint.h"

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::WriteValueConstraint()
//-----------------------------------------------------------------------------
WriteValueConstraint::WriteValueConstraint():
type_(WriteValueConstraint::TYPE_COUNT),
writeAsRead_(false),
useEnumValues_(false),
minimum_(),
maximum_()
{

}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::WriteValueConstraint()
//-----------------------------------------------------------------------------
WriteValueConstraint::WriteValueConstraint( const WriteValueConstraint& other):
type_(other.type_),
writeAsRead_(other.writeAsRead_),
useEnumValues_(other.useEnumValues_),
minimum_(other.minimum_),
maximum_(other.maximum_)
{

}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::~WriteValueConstraint()
//-----------------------------------------------------------------------------
WriteValueConstraint::~WriteValueConstraint()
{

}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::operator=()
//-----------------------------------------------------------------------------
WriteValueConstraint& WriteValueConstraint::operator=( const WriteValueConstraint& other)
{
    if (&other != this)
    {
        type_ = other.type_;
		writeAsRead_ = other.writeAsRead_;
		useEnumValues_ = other.useEnumValues_;
		minimum_ = other.minimum_;
		maximum_ = other.maximum_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::getType()
//-----------------------------------------------------------------------------
WriteValueConstraint::Type WriteValueConstraint::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::setType()
//-----------------------------------------------------------------------------
void WriteValueConstraint::setType(WriteValueConstraint::Type newType )
{
    type_ = newType;

    writeAsRead_ = false;
    useEnumValues_ = true;
    minimum_ = "";
    maximum_ = "";

    if (type_ == WriteValueConstraint::WRITE_AS_READ)
    {
        writeAsRead_ = true;
    }
    else if(type_ == WriteValueConstraint::USE_ENUM)
    {
        useEnumValues_ = true;
    }
    else if (type_ == WriteValueConstraint::MIN_MAX)
    {
        minimum_ = "";
        maximum_ = "";
    }
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::getMinimum()
//-----------------------------------------------------------------------------
QString WriteValueConstraint::getMinimum() const
{
    if (type_ == WriteValueConstraint::MIN_MAX)
    {
        return minimum_;
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::setMinimum()
//-----------------------------------------------------------------------------
void WriteValueConstraint::setMinimum(QString const& newMinimum)
{
    useEnumValues_ = false;
    writeAsRead_ = false;

    type_ = WriteValueConstraint::MIN_MAX;
    minimum_ = newMinimum;
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::getMaximum()
//-----------------------------------------------------------------------------
QString WriteValueConstraint::getMaximum() const
{
    if (type_ == WriteValueConstraint::MIN_MAX)
    {
        return maximum_;
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::setMaximum()
//-----------------------------------------------------------------------------
void WriteValueConstraint::setMaximum(QString const& newMaximum)
{
    useEnumValues_ = false;
    writeAsRead_ = false;

    type_ = WriteValueConstraint::MIN_MAX;
    maximum_ = newMaximum;
}
