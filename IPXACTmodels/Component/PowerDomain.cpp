//-----------------------------------------------------------------------------
// File: PowerDomain.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 4.10.2023
//
// Description:
// Implementation for ipxact:powerDomain element.
//-----------------------------------------------------------------------------

#include "PowerDomain.h"

#include <QString>

//-----------------------------------------------------------------------------
// Function: PowerDomain::PowerDomain()
//-----------------------------------------------------------------------------
PowerDomain::PowerDomain(): NameGroup(), Extendable()
{
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::PowerDomain()
//-----------------------------------------------------------------------------
PowerDomain::PowerDomain(PowerDomain const& other):
NameGroup(other),
Extendable(other),
alwaysOn_(other.alwaysOn_),
subDomainOf_(other.subDomainOf_)
{
    copyParameters(other);
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::operator=()
//-----------------------------------------------------------------------------
PowerDomain& PowerDomain::operator=(PowerDomain const& other)
{
	if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        alwaysOn_ = other.alwaysOn_;
        subDomainOf_ = other.subDomainOf_;

        copyParameters(other);
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::~PowerDomain()
//-----------------------------------------------------------------------------
PowerDomain::~PowerDomain()
{
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::setAlwaysOn()
//-----------------------------------------------------------------------------
void PowerDomain::setAlwaysOn(QString const& expression)
{
    alwaysOn_ = expression;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::getAlwaysOn()
//-----------------------------------------------------------------------------
QString PowerDomain::getAlwaysOn() const
{
    return alwaysOn_;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::setSubDomainOf()
//-----------------------------------------------------------------------------
void PowerDomain::setSubDomainOf(QString const& domainName)
{
    subDomainOf_ = domainName;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::getSubDomainOf()
//-----------------------------------------------------------------------------
QString PowerDomain::getSubDomainOf() const
{
    return subDomainOf_;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::setParameters()
//-----------------------------------------------------------------------------
void PowerDomain::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters)
{
    parameters_ = parameters;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > PowerDomain::getParameters()
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: PowerDomain::copyParameters()
//-----------------------------------------------------------------------------
void PowerDomain::copyParameters(PowerDomain const& other)
{
    for (auto parameter : *other.parameters_)
    {
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameter)));
    }
}
