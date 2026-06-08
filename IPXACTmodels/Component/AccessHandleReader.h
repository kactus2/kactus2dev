//-----------------------------------------------------------------------------
// File: AccessHandleReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Reader for ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#ifndef ACCESSHANDLEREADER_H
#define ACCESSHANDLEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/AccessHandle.h>

#include <QSharedPointer>
#include <QDomNode>


//-----------------------------------------------------------------------------
//! Reader for ipxact:accessHandle element.
//-----------------------------------------------------------------------------
namespace AccessHandleReader
{

    /*!
     *  Creates a new access handle from a given access handle node.
     *
     *    @param [in] accessHandleNode    XML description of the access handle.
     *    @param [in] type                The type of access handle to parse, depends on std revision and containing element.
     *    @param [in] docRevision         The IP-XACT standard revision.
     *
     *    @return The created access handle.
     */
    IPXACTMODELS_EXPORT QSharedPointer<AccessHandle> createAccessHandleFrom(
        QDomNode const& accessHandleNode, AccessHandle::ElementType elementType, Document::Revision docRevision);

    namespace Details
    {

        //! Parse the different types of access handles
        void parseSimple14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parseSimple22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parseNonIndexedLeaf14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parseIndexed14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parseLeaf14(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parseSliced22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
        void parsePort22(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);

        /*!
         *  Reads the force value.
         *
         *    @param [in] accessHandleNode    XML description of the access handle.
         *    @param [in] newAccessHandle     The new access handle.
         */
        void parseForce(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle);

        /*!
         *  Reads the indices.
         *
         *    @param [in] accessHandleNode    XML description of the access handle.
         *    @param [in] newAccessHandle     The new access handle.
         */
        void parseIndices(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle);

        /*!
         *  Reads the slices.
         *
         *    @param [in] accessHandleNode    XML description of the access handle.
         *    @param [in] newAccessHandle     The new access handle.
         *    @param [in] docRevision         The IP-XACT standard revision.
         */
        void parseSlices(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);

        /*!
         *  Reads the view references.
         *
         *    @param [in] accessHandleNode    XML description of the access handle.
         *    @param [in] newAccessHandle     The new access handle.
         */
        void parseViewReferences(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle);

        /*!
         *  Reads the path segments.
         *
         *    @param [in] accessHandleNode    XML description of the access handle.
         *    @param [in] newAccessHandle     The new access handle.
         *    @param [in] docRevision         The IP-XACT standard revision.
         */
        void parsePathSegments(QDomNode const& accessHandleNode, QSharedPointer<AccessHandle> newAccessHandle, Document::Revision docRevision);
    }
}

#endif // ACCESSHANDLEREADER_H
