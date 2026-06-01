//-----------------------------------------------------------------------------
// File: SliceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:slice element.
//-----------------------------------------------------------------------------

#include "SliceReader.h"
#include "Slice.h"
#include "PathSegmentReader.h"
#include <IPXACTmodels/common/CommonItemsReader.h>

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: SliceReader::createSliceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Slice> SliceReader::createSliceFrom(QDomNode const& sliceNode, Document::Revision docRevision)
{
    QSharedPointer<Slice> newSlice(new Slice());

    QDomElement pathSegmentsElement = sliceNode.firstChildElement(QStringLiteral("ipxact:pathSegments"));
    if (!pathSegmentsElement.isNull())
    {
        QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments(new QList<QSharedPointer<PathSegment> >());
        QDomNodeList pathSegmentNodes = pathSegmentsElement.childNodes();

        for (int i = 0; i < pathSegmentNodes.count(); ++i)
        {
            QDomElement pathSegmentElement = pathSegmentNodes.at(i).toElement();
            if (!pathSegmentElement.isNull() && pathSegmentElement.tagName() == QLatin1String("ipxact:pathSegment"))
            {
                auto pathSegment = PathSegmentReader::createPathSegmentFrom(pathSegmentElement, docRevision);
                pathSegments->append(pathSegment);
            }
        }

        if (!pathSegments->isEmpty())
        {
            newSlice->pathSegments_ = pathSegments;
        }
    }

    QDomElement rangeElement = sliceNode.firstChildElement(QStringLiteral("ipxact:range"));
    if (!rangeElement.isNull())
    {
        Range range = CommonItemsReader::parseRange(rangeElement);
        newSlice->setLeft(range.getLeft());
        newSlice->setRight(range.getRight());
    }

    return newSlice;
}
