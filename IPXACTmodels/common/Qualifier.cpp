//-----------------------------------------------------------------------------
// File: Qualifier.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------

#include "Qualifier.h"

namespace {

    const QMap<Qualifier::Type, QString> QUALIFIER_TYPE_STRING =
    {
        {Qualifier::Type::Address, QStringLiteral("address")},
        {Qualifier::Type::Data, QStringLiteral("data")},
        {Qualifier::Type::Clock, QStringLiteral("clock")},
        {Qualifier::Type::Reset, QStringLiteral("reset")},
        {Qualifier::Type::Valid, QStringLiteral("valid")},
        {Qualifier::Type::Interrupt, QStringLiteral("interrupt")},
        {Qualifier::Type::ClockEnable, QStringLiteral("clock enable")},
        {Qualifier::Type::PowerEnable, QStringLiteral("power enable")},
        {Qualifier::Type::Opcode, QStringLiteral("opcode")},
        {Qualifier::Type::Protection, QStringLiteral("protection")},
        {Qualifier::Type::FlowControl, QStringLiteral("flow control")},
        {Qualifier::Type::User, QStringLiteral("user")},
        {Qualifier::Type::Request, QStringLiteral("request")},
        {Qualifier::Type::Response, QStringLiteral("response")}
    };
}

//-----------------------------------------------------------------------------
// Function: Qualifier::clone()
//-----------------------------------------------------------------------------
Qualifier* Qualifier::clone() const
{
    return new Qualifier(*this);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::isSet()
//-----------------------------------------------------------------------------
bool Qualifier::isSet() const
{
    return !types_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Qualifier::clear()
//-----------------------------------------------------------------------------
void Qualifier::clear()
{
    types_.clear();
    attributes_.fill(QString());
}

//-----------------------------------------------------------------------------
// Function: Qualifier::hasType()
//-----------------------------------------------------------------------------
bool Qualifier::hasType(Type type) const
{
    return types_.contains(type);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setType()
//-----------------------------------------------------------------------------
void Qualifier::setType(Type type)
{
    if (!hasType(type))
    {
        types_.append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: Qualifier::removeType()
//-----------------------------------------------------------------------------
void Qualifier::removeType(Type type)
{
    types_.removeOne(type);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getTypes()
//-----------------------------------------------------------------------------
QList<Qualifier::Type> Qualifier::getTypes() const
{
    return types_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getAttribute()
//-----------------------------------------------------------------------------
QString Qualifier::getAttribute(Attribute attribute) const
{   
    return attributes_[static_cast<unsigned int>(attribute)];
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setAttribute()
//-----------------------------------------------------------------------------
void Qualifier::setAttribute(Attribute attribute, QString const& attributeValue)
{
    attributes_[static_cast<unsigned int>(attribute)] = attributeValue;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator==()
//-----------------------------------------------------------------------------
bool Qualifier::operator==(Qualifier const& other) const
{
    return (types_ == other.types_ && attributes_ == other.attributes_);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator!=()
//-----------------------------------------------------------------------------
bool Qualifier::operator!=(Qualifier const& other) const
{
    return !(*this == other);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::typeToString()
//-----------------------------------------------------------------------------
QString Qualifier::typeToString(Type type)
{
    return QUALIFIER_TYPE_STRING.value(type, QString());
}

//-----------------------------------------------------------------------------
// Function: Qualifier::stringToType()
//-----------------------------------------------------------------------------
Qualifier::Type Qualifier::stringToType(QString const& typeString)
{
    return QUALIFIER_TYPE_STRING.key(typeString, Qualifier::Type::Any);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::stringToAttributeName()
//-----------------------------------------------------------------------------
Qualifier::Attribute Qualifier::stringToAttributeName(QString const& attributeName)
{
    if (attributeName == QStringLiteral("resetLevel"))
    {
        return Attribute::ResetLevel;
    }
    else if (attributeName == QStringLiteral("clockEnableLevel"))
    {
        return Attribute::ClockEnableLevel;
    }
    else if (attributeName == QStringLiteral("powerEnableLevel"))
    {
        return Attribute::PowerEnableLevel;
    }
    else if (attributeName == QStringLiteral("powerDomainReference"))
    {
        return Attribute::PowerDomainReference;
    }
    else if (attributeName == QStringLiteral("flowType"))
    {
        return Attribute::FlowType;
    }
    else if (attributeName == QStringLiteral("userFlowType"))
    {
        return Attribute::UserFlowType;
    }
    else 
    {
        return Attribute::UserDefined;
    }
}
