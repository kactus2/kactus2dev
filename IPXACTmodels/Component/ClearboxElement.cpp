//-----------------------------------------------------------------------------
// File: ClearboxElement.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Describes the ipxact:clearboxElement/whiteboxElement elements.
//-----------------------------------------------------------------------------

#include "ClearboxElement.h"

#include <utilities/Copy.h>

ClearboxElement::ClearboxElement(QString const& name) :
    NameGroup(name),
    Extendable(),
    isPresent_(),
    driveable_()
{
}

ClearboxElement::ClearboxElement(ClearboxElement const& other) :
    type_(other.type_),
    driveable_(other.driveable_)
{
    Copy::copyList(other.parameters_, parameters_);
}

ClearboxElement& ClearboxElement::operator=(ClearboxElement const& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        type_ = other.type_;
        driveable_ = other.driveable_;

        parameters_->clear();
        Copy::copyList(other.parameters_, parameters_);
    }

    return *this;
}

QString ClearboxElement::getIsPresent() const
{
    return isPresent_;
}

void ClearboxElement::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

ClearboxElement::Type ClearboxElement::getType() const
{
    return type_;
}

void ClearboxElement::setType(Type type)
{
    type_ = type;
}

QString ClearboxElement::getDriveable() const
{
    return driveable_.toString();
}

void ClearboxElement::setDriveable(bool value)
{
    value ? driveable_.setValue(true) : driveable_.setValue(false);
}

QSharedPointer<QList<QSharedPointer<Parameter> > > ClearboxElement::getParameters() const
{
    return parameters_;
}

void ClearboxElement::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters)
{
    parameters_ = parameters;
}

QString ClearboxElement::typeToString(Type type)
{
    switch (type)
    {
    case Type::PIN: return QStringLiteral("pin");
    case Type::SIGNAL: return QStringLiteral("signal");
    case Type::INTERFACE: return QStringLiteral("interface");
    default: return QString();
    }
}

ClearboxElement::Type ClearboxElement::stringToType(QString const& typeStr)
{
    auto lower = typeStr.toLower();
    if (lower.compare(QStringLiteral("pin")) == 0) return Type::PIN;
    else if (lower.compare(QStringLiteral("signal")) == 0) return Type::SIGNAL;
    else if (lower.compare(QStringLiteral("interface")) == 0) return Type::INTERFACE;
    return Type::UKNOWN;
}
