//-----------------------------------------------------------------------------
// File: PowerDomainWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 4.10.2023
//
// Description:
// Writer namespace for ipxact:powerDomain.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINWRITER_H
#define POWERDOMAINWRITER_H

#include "PowerDomain.h"


#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer namespace for ipxact:powerDomain.
//-----------------------------------------------------------------------------
namespace PowerDomainWriter
{

    /*!
     *  Write a channel to an XML file.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] domain		The power domain to be written.
     */
    IPXACTMODELS_EXPORT void write(QXmlStreamWriter& writer, QSharedPointer<PowerDomain> domain);

};

#endif // POWERDOMAINWRITER_H
