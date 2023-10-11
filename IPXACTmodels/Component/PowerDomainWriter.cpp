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

#include "PowerDomainWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainWriter::createPowerDomainFrom()
//-----------------------------------------------------------------------------
void PowerDomainWriter::write(QXmlStreamWriter& writer, QSharedPointer<PowerDomain> domain)
{
    writer.writeStartElement(QStringLiteral("ipxact:powerDomain"));

    NameGroupWriter::writeNameGroup(writer, domain, Document::Revision::Std22);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:alwaysOn"), domain->getAlwaysOn());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:subDomainOf"), domain->getSubDomainOf());

    CommonItemsWriter::writeParameters(writer, domain->getParameters(), Document::Revision::Std22);

    CommonItemsWriter::writeVendorExtensions(writer, domain);

	writer.writeEndElement(); // ipxact:powerDomain
}
