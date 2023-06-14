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

const QMap<Qualifier::Type, QString> Qualifier::QUALIFIER_TYPE_STRING =
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

//-----------------------------------------------------------------------------
// Function: Qualifier::Qualifier()
//-----------------------------------------------------------------------------
Qualifier::Qualifier()
{

}

//-----------------------------------------------------------------------------
// Function: Qualifier::Qualifier()
//-----------------------------------------------------------------------------
Qualifier::Qualifier(Qualifier const& other) :
    types_(new QList<Type>(*other.types_)),
    resetLevel_(other.resetLevel_),
    clockEnableLevel_(other.clockEnableLevel_),
    powerEnableLevel_(other.powerEnableLevel_),
    powerDomainRef_(other.powerDomainRef_),
    flowType_(other.flowType_),
    userFlowType_(other.userFlowType_),
    userDefined_(other.userDefined_)
{

}

//-----------------------------------------------------------------------------
// Function: Qualifier::isSet()
//-----------------------------------------------------------------------------
bool Qualifier::isSet() const
{
    return !types_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Qualifier::clear()
//-----------------------------------------------------------------------------
void Qualifier::clear()
{
    types_->clear();
    resetLevel_.clear();
    clockEnableLevel_.clear();
    powerEnableLevel_.clear();
    powerDomainRef_.clear();
    flowType_.clear();
    userFlowType_.clear();
    userDefined_.clear();
}

//-----------------------------------------------------------------------------
// Function: Qualifier::hasType()
//-----------------------------------------------------------------------------
bool Qualifier::hasType(Type type) const
{
    return types_->contains(type);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setType()
//-----------------------------------------------------------------------------
void Qualifier::setType(Type type)
{
    if (!hasType(type))
    {
        types_->append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<Qualifier::Type> > Qualifier::getTypes() const
{
    return types_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setResetLevel()
//-----------------------------------------------------------------------------
void Qualifier::setResetLevel(QString const& level)
{
    resetLevel_ = level;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getResetLevel()
//-----------------------------------------------------------------------------
QString Qualifier::getResetLevel() const
{
    return resetLevel_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setClockEnableLevel()
//-----------------------------------------------------------------------------
void Qualifier::setClockEnableLevel(QString const& level)
{
    clockEnableLevel_ = level;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getClockEnableLevel()
//-----------------------------------------------------------------------------
QString Qualifier::getClockEnableLevel() const
{
    return clockEnableLevel_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setPowerEnableLevel()
//-----------------------------------------------------------------------------
void Qualifier::setPowerEnableLevel(QString const& level)
{
    powerEnableLevel_ = level;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getPowerEnableLevel()
//-----------------------------------------------------------------------------
QString Qualifier::getPowerEnableLevel() const
{
    return powerEnableLevel_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setPowerDomainRef()
//-----------------------------------------------------------------------------
void Qualifier::setPowerDomainRef(QString const& reference)
{
    powerDomainRef_ = reference;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getPowerDomainRef()
//-----------------------------------------------------------------------------
QString Qualifier::getPowerDomainRef() const
{
    return powerDomainRef_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setFlowType()
//-----------------------------------------------------------------------------
void Qualifier::setFlowType(QString const& flowType)
{
    flowType_ = flowType;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getFlowType()
//-----------------------------------------------------------------------------
QString Qualifier::getFlowType() const
{
    return flowType_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setUserFlowType()
//-----------------------------------------------------------------------------
void Qualifier::setUserFlowType(QString const& userFlowType)
{
    userFlowType_ = userFlowType;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getUserFlowType()
//-----------------------------------------------------------------------------
QString Qualifier::getUserFlowType() const
{
    return userFlowType_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setUserDefined()
//-----------------------------------------------------------------------------
void Qualifier::setUserDefined(QString const& userDefined)
{
    userDefined_ = userDefined;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getUserDefined()
//-----------------------------------------------------------------------------
QString Qualifier::getUserDefined() const
{
    return userDefined_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::getAttribute()
//-----------------------------------------------------------------------------
QString Qualifier::getAttribute(QString const& attributeName) const
{
    if (attributeName == QStringLiteral("resetLevel"))
    {
        return getResetLevel();
    }
    else if (attributeName == QStringLiteral("clockEnableLevel"))
    {
        return getClockEnableLevel();
    }
    else if (attributeName == QStringLiteral("powerEnableLevel"))
    {
        return getPowerEnableLevel();
    }
    else if (attributeName == QStringLiteral("powerDomainReference"))
    {
        return getPowerDomainRef();
    }
    else if (attributeName == QStringLiteral("flowType"))
    {
        return getFlowType();
    }
    else if (attributeName == QStringLiteral("userFlowType"))
    {
        return getUserFlowType();
    }
    else if (attributeName == QStringLiteral("userDefined"))
    {
        return getUserDefined();
    }
    
    return QString();
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setAttribute()
//-----------------------------------------------------------------------------
void Qualifier::setAttribute(QString const& attributeName, QString const& attributeValue)
{
    if (attributeName == QStringLiteral("resetLevel"))
    {
        setResetLevel(attributeValue);
    }
    else if (attributeName == QStringLiteral("clockEnableLevel"))
    {
        setClockEnableLevel(attributeValue);
    }
    else if (attributeName == QStringLiteral("powerEnableLevel"))
    {
        setPowerEnableLevel(attributeValue);
    }
    else if (attributeName == QStringLiteral("powerDomainReference"))
    {
        setPowerDomainRef(attributeValue);
    }
    else if (attributeName == QStringLiteral("flowType"))
    {
        setFlowType(attributeValue);
    }
    else if (attributeName == QStringLiteral("userFlowType"))
    {
        setUserFlowType(attributeValue);
    }
    else if (attributeName == QStringLiteral("user"))
    {
        setUserDefined(attributeValue);
    }
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator==()
//-----------------------------------------------------------------------------
bool Qualifier::operator==(Qualifier const& other)
{
    return (*types_ == *other.types_ &&
        resetLevel_ == other.resetLevel_ &&
        clockEnableLevel_ == other.clockEnableLevel_ &&
        powerEnableLevel_ == other.powerEnableLevel_ &&
        powerDomainRef_ == other.powerDomainRef_ &&
        flowType_ == other.flowType_ &&
        userFlowType_ == other.userFlowType_ &&
        userDefined_ == other.userDefined_);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator!=()
//-----------------------------------------------------------------------------
bool Qualifier::operator!=(Qualifier const& other)
{
    return !(*this == other);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::typeToString()
//-----------------------------------------------------------------------------
QString Qualifier::typeToString(Type type)
{
    return QUALIFIER_TYPE_STRING.value(type, QStringLiteral(""));
}

//-----------------------------------------------------------------------------
// Function: Qualifier::stringToType()
//-----------------------------------------------------------------------------
Qualifier::Type Qualifier::stringToType(QString const& typeString)
{
    for (auto i = QUALIFIER_TYPE_STRING.cbegin(), end = QUALIFIER_TYPE_STRING.cend(); i != end; ++i)
    {
        if (i.value() == typeString)
        {
            return i.key();
        }
    }
    return Type::Any;
}
