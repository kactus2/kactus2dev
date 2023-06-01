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

//-----------------------------------------------------------------------------
// Function: Qualifier::Qualifier()
//-----------------------------------------------------------------------------
Qualifier::Qualifier()
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
// Function: Qualifier::typeToString()
//-----------------------------------------------------------------------------
QString Qualifier::typeToString(Type type)
{
    switch (type)
    {
    case Type::Address:
        return QStringLiteral("address");
    case Type::Data:
        return QStringLiteral("data");
    case Type::Clock:
        return QStringLiteral("clock");
    case Type::Reset:
        return QStringLiteral("reset");
    case Type::Valid:
        return QStringLiteral("valid");
    case Type::Interrupt:
        return QStringLiteral("interrupt");
    case Type::ClockEnable:
        return QStringLiteral("clock enable");
    case Type::PowerEnable:
        return QStringLiteral("power enable");
    case Type::Opcode:
        return QStringLiteral("opcode");
    case Type::Protection:
        return QStringLiteral("protection");
    case Type::FlowControl:
        return QStringLiteral("flow control");
    case Type::User:
        return QStringLiteral("user");
    case Type::Request:
        return QStringLiteral("request");
    case Type::Response:
        return QStringLiteral("response");
    default:
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: Qualifier::typeToXMLElementName()
//-----------------------------------------------------------------------------
QString Qualifier::typeToXMLElementName(Type type)
{
    auto body = QStringLiteral("ipxact:is");
    
    if (type == Type::ClockEnable)
    {
        return body + QStringLiteral("ClockEn");
    }

    if (type == Type::PowerEnable)
    {
        return body + QStringLiteral("PowerEn");
    }

    if (type == Type::FlowControl)
    {
        return body + QStringLiteral("FlowControl");
    }

    auto typeAsStr = typeToString(type);
    typeAsStr.front() = typeAsStr.front().toUpper();

    return body + typeAsStr;
}
