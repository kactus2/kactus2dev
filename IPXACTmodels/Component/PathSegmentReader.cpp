//-----------------------------------------------------------------------------
// File: PathSegmentReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:pathSegment element.
//-----------------------------------------------------------------------------

#include "PathSegmentReader.h"
#include "Slice.h"

//-----------------------------------------------------------------------------
// Function: PathSegmentReader::createPathSegmentFrom()
//-----------------------------------------------------------------------------
QSharedPointer<PathSegment> PathSegmentReader::createPathSegmentFrom(QDomNode const& pathSegmentNode, Document::Revision docRevision)
{
    QSharedPointer<PathSegment> newPathSegment(new PathSegment());

    if (docRevision == Document::Revision::Std22)
    {
        // 22 standard has path segment name as value and apparently no indices
        newPathSegment->name_ = pathSegmentNode.nodeValue();
    }
    else
    {
        newPathSegment->name_ = pathSegmentNode.firstChildElement(QStringLiteral("ipxact:pathSegmentName"))
            .firstChild().nodeValue();

        QDomElement indicesElement = pathSegmentNode.firstChildElement(QStringLiteral("ipxact:indices"));
        if (!indicesElement.isNull())
        {
            QStringList indices;
            QDomNodeList indexNodes = indicesElement.childNodes();
            for (int i = 0; i < indexNodes.count(); ++i)
            {
                indices.append(indexNodes.at(i).firstChild().nodeValue());
            }
            newPathSegment->indices_ = indices;
        }  
    }

    return newPathSegment;
}
