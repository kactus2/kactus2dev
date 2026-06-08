//-----------------------------------------------------------------------------
// File: AccessHandleWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#include "AccessHandleWriter.h"
#include "AccessHandle.h"
#include "Slice.h"
#include "SliceWriter.h"
#include "PathSegmentWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::writeAccessHandle()
//-----------------------------------------------------------------------------
void AccessHandleWriter::writeAccessHandle(
    QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:accessHandle"));

    Details::writeForce(writer, accessHandle);
    
    Details::writeViewReferences(writer, accessHandle);
    
    Details::writeIndices(writer, accessHandle);
    
    Details::writeSlices(writer, accessHandle, docRevision);

    Details::writePathSegments(writer, accessHandle, docRevision);

    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeVendorExtensions(writer, accessHandle);
    }

    writer.writeEndElement(); // ipxact:accessHandle
}

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::Details::writeForce()
//-----------------------------------------------------------------------------
void AccessHandleWriter::Details::writeForce(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle)
{
    CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("force"), accessHandle->getForce());
}

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::Details::writeIndices()
//-----------------------------------------------------------------------------
void AccessHandleWriter::Details::writeIndices(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle)
{
    auto indices = accessHandle->getIndices();
    if (!indices.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:indices"));
        for (auto const& index : indices)
        {
            writer.writeTextElement(QStringLiteral("ipxact:index"), index);
        }
        writer.writeEndElement(); // ipxact:indices
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::Details::writeSlices()
//-----------------------------------------------------------------------------
void AccessHandleWriter::Details::writeSlices(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision)
{
    auto slices = accessHandle->getSlices();
    if (slices && !slices->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:slices"));
        for (auto const& slice : *slices)
        {
            SliceWriter::writeSlice(writer, slice, docRevision);
        }
        writer.writeEndElement(); // ipxact:slices
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::Details::writeViewReferences()
//-----------------------------------------------------------------------------
void AccessHandleWriter::Details::writeViewReferences(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle)
{
    for (auto const& viewRef : accessHandle->getViewReferences())
    {
        writer.writeTextElement(QStringLiteral("ipxact:viewRef"), viewRef);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleWriter::Details::writePathSegments()
//-----------------------------------------------------------------------------
void AccessHandleWriter::Details::writePathSegments(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision)
{
    QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments = accessHandle->getPathSegments();
    if (pathSegments && !pathSegments->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:pathSegments"));
        for (auto const& pathSegment : *pathSegments)
        {
            PathSegmentWriter::writePathSegment(writer, pathSegment, docRevision);
        }
        writer.writeEndElement(); // ipxact:pathSegments
    }
}
