//-----------------------------------------------------------------------------
// File: SliceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:slice element.
//-----------------------------------------------------------------------------

#ifndef SLICEREADER_H
#define SLICEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

class Slice;

//-----------------------------------------------------------------------------
//! Reader for ipxact:slice element.
//-----------------------------------------------------------------------------
namespace SliceReader
{

    /*!
     *  Creates a new slice from a given slice node.
     *
     *    @param [in] sliceNode   XML description of the slice.
     *    @param [in] docRevision The revision of the document.
     *
     *    @return The created slice.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Slice> createSliceFrom(QDomNode const& sliceNode, Document::Revision docRevision);
}

#endif // SLICEREADER_H
