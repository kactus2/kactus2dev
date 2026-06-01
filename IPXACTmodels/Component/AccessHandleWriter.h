//-----------------------------------------------------------------------------
// File: AccessHandleWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Writer for ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#ifndef ACCESSHANDLEWRITER_H
#define ACCESSHANDLEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class AccessHandle;

//-----------------------------------------------------------------------------
//! Writer for ipxact:accessHandle element.
//-----------------------------------------------------------------------------
namespace AccessHandleWriter
{

    /*!
     *  Write an access handle to an XML file.
     *
     *    @param [in] writer              The used XML writer.
     *    @param [in] accessHandle        The access handle to be written.
     *    @param [in] docRevision         The IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT void writeAccessHandle(
        QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the force value.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] accessHandle    Selected access handle.
         */
        void writeForce(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle);

        /*!
         *  Write the indices.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] accessHandle    Selected access handle.
         */
        void writeIndices(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle);

        /*!
         *  Write the slices.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] accessHandle    Selected access handle.
         *    @param [in] docRevision     The revision of the document.
         */
        void writeSlices(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision);

        /*!
         *  Write the view references.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] accessHandle    Selected access handle.
         */
        void writeViewReferences(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle);

        /*!
         *  Write the path segments.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] accessHandle    Selected access handle.
         *    @param [in] docRevision     The revision of the document.
         */
        void writePathSegments(QXmlStreamWriter& writer, QSharedPointer<AccessHandle> accessHandle, Document::Revision docRevision);
    }
}

#endif // ACCESSHANDLEWRITER_H
