//-----------------------------------------------------------------------------
// File: SliceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:slice element.
//-----------------------------------------------------------------------------

#ifndef SLICEWRITER_H
#define SLICEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class Slice;

//-----------------------------------------------------------------------------
//! Writer for ipxact:slice element.
//-----------------------------------------------------------------------------
namespace SliceWriter
{

    /*!
     *  Write a slice to an XML file.
     *
     *    @param [in] writer  The used XML writer.
     *    @param [in] slice   The slice to be written.
     *    @param [in] docRevision  The revision of the document.
     */
    IPXACTMODELS_EXPORT void writeSlice(QXmlStreamWriter& writer, QSharedPointer<Slice> slice, Document::Revision docRevision);
}

#endif // SLICEWRITER_H
