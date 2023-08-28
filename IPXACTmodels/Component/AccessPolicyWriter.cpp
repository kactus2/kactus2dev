//-----------------------------------------------------------------------------
// File: AccessPolicyWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Writer for ipxact:accessPolicy.
//-----------------------------------------------------------------------------

#include "AccessPolicyWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: AccessPolicyWriter::writeAccessPolicy()
//-----------------------------------------------------------------------------
void AccessPolicyWriter::writeAccessPolicy(QXmlStreamWriter& writer,
    QSharedPointer<AccessPolicy> fieldAccessPolicy)
{
    writer.writeStartElement(QStringLiteral("ipxact:accessPolicy"));

    CommonItemsWriter::writeModeReferences(writer, fieldAccessPolicy->getModeReferences());

    Details::writeAccess(writer, fieldAccessPolicy);

    CommonItemsWriter::writeVendorExtensions(writer, fieldAccessPolicy);

    writer.writeEndElement(); // ipxact:accessPolicy
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyWriter::Details::writeAccess()
//-----------------------------------------------------------------------------
void AccessPolicyWriter::Details::writeAccess(QXmlStreamWriter& writer, QSharedPointer<AccessPolicy> fieldAccessPolicy)
{
    if (fieldAccessPolicy->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString access = AccessTypes::access2Str(fieldAccessPolicy->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), access);
    }
}
