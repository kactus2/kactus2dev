//-----------------------------------------------------------------------------
// File: AccessHandleReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#include "AccessHandleReader.h"
#include "AccessHandle.h"
#include "Slice.h"
#include "SliceReader.h"
#include "PathSegmentReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::createAccessHandleFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AccessHandle> AccessHandleReader::createAccessHandleFrom(
    QDomNode const& accessHandleNode, Document::Revision docRevision)
{
    QSharedPointer<AccessHandle> newAccessHandle(new AccessHandle());

    Details::parseForce(accessHandleNode, newAccessHandle);

    Details::parseViewReferences(accessHandleNode, newAccessHandle);

    Details::parseIndices(accessHandleNode, newAccessHandle);

    Details::parseSlices(accessHandleNode, newAccessHandle, docRevision);

    Details::parsePathSegments(accessHandleNode, newAccessHandle, docRevision);

    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsReader::parseVendorExtensions(accessHandleNode, newAccessHandle);
    }

    return newAccessHandle;
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseForce()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseForce(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle)
{
    QDomElement forceElement = accessHandleNode.firstChildElement(QStringLiteral("ipxact:force"));
    if (!forceElement.isNull())
    {
        QString forceString = forceElement.firstChild().nodeValue();
        newAccessHandle->setForce(forceString.compare(QStringLiteral("true")) == 0);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseIndices()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseIndices(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle)
{
    QDomElement indicesElement = accessHandleNode.firstChildElement(QStringLiteral("ipxact:indices"));
    if (!indicesElement.isNull())
    {
        QStringList indices;
        QDomNodeList indexNodes = indicesElement.elementsByTagName(QStringLiteral("ipxact:index"));
        for (int i = 0; i < indexNodes.count(); ++i)
        {
            indices.append(indexNodes.at(i).firstChild().nodeValue());
        }
        newAccessHandle->setIndices(indices);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseSlices()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseSlices(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    QDomElement slicesElement = accessHandleNode.firstChildElement(QStringLiteral("ipxact:slices"));
    if (!slicesElement.isNull())
    {
        QList<QSharedPointer<Slice> > slices;
        QDomNodeList sliceNodes = slicesElement.elementsByTagName(QStringLiteral("ipxact:slice"));

        for (int i = 0; i < sliceNodes.count(); ++i)
        {
            auto slice = SliceReader::createSliceFrom(sliceNodes.at(i), docRevision);
            slices.append(slice);
        }

        newAccessHandle->getSlices()->append(slices);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseViewReferences()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseViewReferences(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle)
{
    QDomElement viewRefsElement = accessHandleNode.firstChildElement(QStringLiteral("ipxact:viewRefs"));
    if (!viewRefsElement.isNull())
    {
        QStringList viewRefs;
        QDomNodeList viewRefNodes = viewRefsElement.elementsByTagName(QStringLiteral("ipxact:viewRef"));
        for (int i = 0; i < viewRefNodes.count(); ++i)
        {
            viewRefs.append(viewRefNodes.at(i).firstChild().nodeValue());
        }

        newAccessHandle->setViewReferences(viewRefs);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parsePathSegments()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parsePathSegments(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    QDomElement pathSegmentsElement = accessHandleNode.firstChildElement(QStringLiteral("ipxact:pathSegments"));
    if (!pathSegmentsElement.isNull())
    {
        QList<QSharedPointer<PathSegment> > pathSegments;
        QDomNodeList pathSegmentNodes = pathSegmentsElement.elementsByTagName(QStringLiteral("ipxact:pathSegment"));

        for (int i = 0; i < pathSegmentNodes.count(); ++i)
        {
            auto pathSegment = PathSegmentReader::createPathSegmentFrom(pathSegmentNodes.at(i), docRevision);
            pathSegments.append(pathSegment);
        }

        newAccessHandle->getPathSegments()->append(pathSegments);
    }
}
