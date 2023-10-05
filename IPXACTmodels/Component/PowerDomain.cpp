//-----------------------------------------------------------------------------
// File: PowerDomain.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.09.2015
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
Extendable(other)
{

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
