//-----------------------------------------------------------------------------
// File: PathSegmentWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:pathSegment element.
//-----------------------------------------------------------------------------

#ifndef PATHSEGMENTWRITER_H
#define PATHSEGMENTWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

struct PathSegment;

//-----------------------------------------------------------------------------
//! Writer for ipxact:pathSegment element.
//-----------------------------------------------------------------------------
namespace PathSegmentWriter
{

    /*!
     *  Write a path segment to an XML file.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] pathSegment The path segment to be written.
     *    @param [in] docRevision The IP-XACT document revision.
     */
    IPXACTMODELS_EXPORT void writePathSegment(QXmlStreamWriter& writer, QSharedPointer<PathSegment> pathSegment, Document::Revision docRevision);
}

#endif // PATHSEGMENTWRITER_H
