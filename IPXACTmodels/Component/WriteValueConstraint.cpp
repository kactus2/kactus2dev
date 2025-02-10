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

namespace
{
    QString const WRITE_AS_READ_TEXT = QLatin1String("Write as read");
    QString const ENUM_VALUES_TEXT = QLatin1String("Use enumerated values");
    QString const MIN_MAX_LIMITS_TEXT = QLatin1String("Set minimum and maximum limits");
    QString const NO_CONSTRAINTS_TEXT = QLatin1String("No constraints");
};

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
WriteValueConstraint::WriteValueConstraint(WriteValueConstraint const& other):
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
WriteValueConstraint& WriteValueConstraint::operator=(WriteValueConstraint const& other)
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
    minimum_ = QString();
    maximum_ = QString();

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
        minimum_ = QString();
        maximum_ = QString();
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

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::stringToType()
//-----------------------------------------------------------------------------
WriteValueConstraint::Type WriteValueConversions::stringToType(QString const& typeText)
{
    if (typeText.compare(WRITE_AS_READ_TEXT) == 0)
    {
        return WriteValueConstraint::WRITE_AS_READ;
    }
    else if (typeText.compare(ENUM_VALUES_TEXT) == 0)
    {
        return WriteValueConstraint::USE_ENUM;
    }
    else if (typeText.compare(MIN_MAX_LIMITS_TEXT) == 0)
    {
        return WriteValueConstraint::MIN_MAX;
    }
    else
    {
        return WriteValueConstraint::TYPE_COUNT;
    }
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraint::typeToString()
//-----------------------------------------------------------------------------
QString WriteValueConversions::typeToString(WriteValueConstraint::Type const& constraintType)
{
    if (constraintType == WriteValueConstraint::WRITE_AS_READ)
    {
        return WRITE_AS_READ_TEXT;
    }
    else if (constraintType == WriteValueConstraint::USE_ENUM)
    {
        return ENUM_VALUES_TEXT;
    }
    else if (constraintType == WriteValueConstraint::MIN_MAX)
    {
        return MIN_MAX_LIMITS_TEXT;
    }
    else
    {
        return NO_CONSTRAINTS_TEXT;
    }
}

//-----------------------------------------------------------------------------
// Function: WriteValueConversions::getConstraintTypes()
//-----------------------------------------------------------------------------
QStringList WriteValueConversions::getConstraintTypes()
{
    QStringList types;
    types.append(WRITE_AS_READ_TEXT);
    types.append(ENUM_VALUES_TEXT);
    types.append(MIN_MAX_LIMITS_TEXT);
    types.append(NO_CONSTRAINTS_TEXT);

    return types;
}
