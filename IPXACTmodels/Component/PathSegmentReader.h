//-----------------------------------------------------------------------------
// File: PathSegmentReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:pathSegment element.
//-----------------------------------------------------------------------------

#ifndef PATHSEGMENTREADER_H
#define PATHSEGMENTREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

struct PathSegment;

//-----------------------------------------------------------------------------
//! Reader for ipxact:pathSegment element.
//-----------------------------------------------------------------------------
namespace PathSegmentReader
{

    /*!
     *  Creates a new path segment from a given path segment node.
     *
     *    @param [in] pathSegmentNode   XML description of the path segment.
     *    @param [in] docRevision         The IP-XACT standard revision.
     *
     *    @return The created path segment.
     */
    IPXACTMODELS_EXPORT QSharedPointer<PathSegment> createPathSegmentFrom(QDomNode const& pathSegmentNode, Document::Revision docRevision);
}

#endif // PATHSEGMENTREADER_H
