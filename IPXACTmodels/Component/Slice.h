//-----------------------------------------------------------------------------
// File: Slice.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Describes the ipxact:slice element and the path segments contained in it.
//-----------------------------------------------------------------------------

#pragma once

#include <QStringList>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Range.h>

#include <IPXACTmodels/utilities/Copy.h>

struct IPXACTMODELS_EXPORT PathSegment
{
    QString name_;
    QStringList indices_;
};

struct IPXACTMODELS_EXPORT Slice : public Range
{
    Slice() = default;
    virtual ~Slice() = default;
    Slice(Slice const& other) : Range(other)
    {
        Copy::copyList(other.pathSegments_, pathSegments_);
    }

    Slice& operator=(Slice const& other)
    {
        if (this != &other)
        {
            Range::operator=(other);
            Copy::copyList(other.pathSegments_, pathSegments_);
        }
        return *this;
    }

    QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments_;
};