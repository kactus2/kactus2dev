//-----------------------------------------------------------------------------
// File: PathSegmentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:pathSegment element.
//-----------------------------------------------------------------------------

#include "PathSegmentWriter.h"
#include "Slice.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: PathSegmentWriter::writePathSegment()
//-----------------------------------------------------------------------------
void PathSegmentWriter::writePathSegment(QXmlStreamWriter& writer, QSharedPointer<PathSegment> pathSegment, Document::Revision docRevision)
{
    if (!pathSegment)
    {
        return;
    }

    if (docRevision == Document::Revision::Std14)
    {
        writer.writeStartElement(QStringLiteral("ipxact:pathSegment"));

        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:pathSegmentName"), pathSegment->name_);

        if (!pathSegment->indices_.isEmpty())
        {
            writer.writeStartElement(QStringLiteral("ipxact:indices"));
            for (auto const& index : pathSegment->indices_)
            {
                writer.writeTextElement(QStringLiteral("ipxact:index"), index);
            }
            writer.writeEndElement(); // ipxact:indices
        }

        writer.writeEndElement(); // ipxact:pathSegment
    }
    else if (docRevision == Document::Revision::Std22)
    {     
        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:pathSegment"), pathSegment->name_);
    }
}
