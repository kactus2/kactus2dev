//-----------------------------------------------------------------------------
// File: PowerDomainReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 4.10.2023
//
// Description:
// Reader class for IP-XACT PowerDomain element.
//-----------------------------------------------------------------------------

#ifndef PowerDomainReader_H
#define PowerDomainReader_H

#include "PowerDomain.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QDomNode>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT PowerDomain element.
//-----------------------------------------------------------------------------
namespace  PowerDomainReader 
{
    /*!
     *  Creates a new PowerDomain from a given PowerDomain node.
     *
     *      @param [in] PowerDomainNode    XML description of the PowerDomain.
     *      @param [in] docRevision    The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT QSharedPointer<PowerDomain> createFrom(QDomNode const& PowerDomainNode);

    namespace Details
    {



        void parseAlwaysOn(QDomNode const& powerDomainNode, QSharedPointer<PowerDomain> powerDomain);
        void parseSubDomain(QDomNode const& powerDomainNode, QSharedPointer<PowerDomain> powerDomain);
    }
};

#endif // PowerDomainReader_H
