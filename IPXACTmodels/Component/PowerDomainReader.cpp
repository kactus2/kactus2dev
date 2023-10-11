//-----------------------------------------------------------------------------
// File: PowerDomainReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 4.10.2023
//
// Description:
// Reader class for IP-XACT PowerDomain element.
//-----------------------------------------------------------------------------

#include "PowerDomainReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainReader::createFrom()
//-----------------------------------------------------------------------------
QSharedPointer<PowerDomain> PowerDomainReader::createFrom(QDomNode const& powerDomainNode)
{
    QSharedPointer<PowerDomain> newPowerDomain(new PowerDomain());

    NameGroupReader::parseNameGroup(powerDomainNode, newPowerDomain);

    Details::parseAlwaysOn(powerDomainNode, newPowerDomain);

    Details::parseSubDomain(powerDomainNode, newPowerDomain);

    newPowerDomain->setParameters(CommonItemsReader::parseAndCreateParameters(powerDomainNode, 
        Document::Revision::Std22));

    CommonItemsReader::parseVendorExtensions(powerDomainNode, newPowerDomain);

    return newPowerDomain;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainReader::Details::parseAlwaysOn()
//-----------------------------------------------------------------------------
void PowerDomainReader::Details::parseAlwaysOn(QDomNode const& powerDomainNode, QSharedPointer<PowerDomain> powerDomain)
{
    QDomElement onElement = powerDomainNode.firstChildElement(QStringLiteral("ipxact:alwaysOn"));

    if (onElement.isNull() == false)
    {
        QString alwaysOn = onElement.firstChild().nodeValue();
        powerDomain->setAlwaysOn(alwaysOn);
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainReader::Details::parseSubDomain()
//-----------------------------------------------------------------------------
void PowerDomainReader::Details::parseSubDomain(QDomNode const& powerDomainNode, QSharedPointer<PowerDomain> powerDomain)
{
    QDomElement domainElement = powerDomainNode.firstChildElement(QStringLiteral("ipxact:subDomainOf"));

    if (domainElement.isNull() == false)
    {
        QString domainName = domainElement.firstChild().nodeValue();
        powerDomain->setSubDomainOf(domainName);
    }
}
