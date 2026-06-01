//-----------------------------------------------------------------------------
// File: SliceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:slice element.
//-----------------------------------------------------------------------------

#include "SliceWriter.h"
#include "Slice.h"
#include "PathSegmentWriter.h"

//-----------------------------------------------------------------------------
// Function: SliceWriter::writeSlice()
//-----------------------------------------------------------------------------
void SliceWriter::writeSlice(QXmlStreamWriter& writer, QSharedPointer<Slice> slice, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:slice"));

    if (slice && slice->pathSegments_ && !slice->pathSegments_->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:pathSegments"));
        for (auto const& pathSegment : *slice->pathSegments_)
        {
            PathSegmentWriter::writePathSegment(writer, pathSegment, docRevision);
        }
        writer.writeEndElement(); // ipxact:pathSegments
    }

    if (slice && (!slice->getLeft().isEmpty() || !slice->getRight().isEmpty()))
    {
        writer.writeStartElement(QStringLiteral("ipxact:range"));
        writer.writeTextElement(QStringLiteral("ipxact:left"), slice->getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), slice->getRight());
        writer.writeEndElement(); // ipxact:range
    }

    writer.writeEndElement(); // ipxact:slice
}
