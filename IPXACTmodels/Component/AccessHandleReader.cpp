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
    QDomNode const& accessHandleNode, AccessHandle::ElementType elementType, Document::Revision docRevision)
{
    QSharedPointer<AccessHandle> newAccessHandle(new AccessHandle());

    // Parse access handle according to which type it is. Type depends on containing element and the standard revision.
    auto accessHandleType = AccessHandle::typeFromElementAndRevision(elementType, docRevision);

    switch (accessHandleType)
    {
    case AccessHandle::Type::Simple14:
        Details::parseSimple14(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::Simple22:
        Details::parseSimple22(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::NonIndexedLeaf14:
        Details::parseNonIndexedLeaf14(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::Indexed14:
        Details::parseIndexed14(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::Leaf14:
        Details::parseLeaf14(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::Sliced22:
        Details::parseSliced22(accessHandleNode, newAccessHandle, docRevision);
        break;
    case AccessHandle::Type::Port22:
        Details::parsePort22(accessHandleNode, newAccessHandle, docRevision);
        break;
    default:
        return nullptr;
    }

    return newAccessHandle;
}

void AccessHandleReader::Details::parseSimple14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parsePathSegments(accessHandleNode, newAccessHandle, docRevision);
}

void AccessHandleReader::Details::parseSimple22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parsePathSegments(accessHandleNode, newAccessHandle, docRevision);
    CommonItemsReader::parseVendorExtensions(accessHandleNode, newAccessHandle);
}

void AccessHandleReader::Details::parseNonIndexedLeaf14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseForce(accessHandleNode, newAccessHandle);
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parseSlices(accessHandleNode, newAccessHandle, docRevision);
}

void AccessHandleReader::Details::parseIndexed14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parseIndices(accessHandleNode, newAccessHandle);
    Details::parsePathSegments(accessHandleNode, newAccessHandle, docRevision);
}

void AccessHandleReader::Details::parseLeaf14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseForce(accessHandleNode, newAccessHandle);
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parseIndices(accessHandleNode, newAccessHandle);    
    Details::parseSlices(accessHandleNode, newAccessHandle, docRevision);
}

void AccessHandleReader::Details::parseSliced22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseForce(accessHandleNode, newAccessHandle);
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parseSlices(accessHandleNode, newAccessHandle, docRevision);
    CommonItemsReader::parseVendorExtensions(accessHandleNode, newAccessHandle);
}

void AccessHandleReader::Details::parsePort22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision)
{
    Details::parseForce(accessHandleNode, newAccessHandle);
    Details::parseViewReferences(accessHandleNode, newAccessHandle);
    Details::parseIndices(accessHandleNode, newAccessHandle);
    Details::parseSlices(accessHandleNode, newAccessHandle, docRevision);
    CommonItemsReader::parseVendorExtensions(accessHandleNode, newAccessHandle);
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseForce()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseForce(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle)
{

    auto attributes = CommonItemsReader::parseAttributes(accessHandleNode);
    if (attributes.contains(QStringLiteral("force")))
    {
        newAccessHandle->setForce(attributes.value(QStringLiteral("force")).compare(QStringLiteral("true")) == 0);
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
        QDomNodeList sliceNodes = slicesElement.elementsByTagName(QStringLiteral("ipxact:slice"));

        for (int i = 0; i < sliceNodes.count(); ++i)
        {
            auto slice = SliceReader::createSliceFrom(sliceNodes.at(i), docRevision);
            newAccessHandle->getSlices()->append(slice);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AccessHandleReader::Details::parseViewReferences()
//-----------------------------------------------------------------------------
void AccessHandleReader::Details::parseViewReferences(QDomNode const& accessHandleNode,
    QSharedPointer<AccessHandle> newAccessHandle)
{
    QStringList viewRefs;
    QDomNodeList viewRefNodes = accessHandleNode.toElement().elementsByTagName(QStringLiteral("ipxact:viewRef"));
    for (int i = 0; i < viewRefNodes.count(); ++i)
    {
        viewRefs.append(viewRefNodes.at(i).firstChild().nodeValue());
    }

    newAccessHandle->setViewReferences(viewRefs);
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
        QDomNodeList pathSegmentNodes = pathSegmentsElement.elementsByTagName(QStringLiteral("ipxact:pathSegment"));

        for (int i = 0; i < pathSegmentNodes.count(); ++i)
        {
            auto pathSegment = PathSegmentReader::createPathSegmentFrom(pathSegmentNodes.at(i), docRevision);
            newAccessHandle->getPathSegments()->append(pathSegment);
        }
    }
}
