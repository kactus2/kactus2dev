//-----------------------------------------------------------------------------
// File: SubspaceMapReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.01.2022
//
// Description:
// Reader for ipxact:subspaceMap element.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPREADER_H
#define SUBSPACEMAPREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryBlockBaseReader.h>

#include <QSharedPointer>
#include <QDomNode>

class SubSpaceMap;

//-----------------------------------------------------------------------------
//! Reader for ipxact:subspaceMap element.
//-----------------------------------------------------------------------------
namespace SubspaceMapReader
{
    /*!
     *  Creates a new subspace map from a given subspace map node.
     *
     *    @param [in] subspaceMapNode     XML description of the subspace map.
     *    @param [in] docRevision         The IP-XACT standard revision to comply to.
     *
     *    @return The created subspace map.
     */
    IPXACTMODELS_EXPORT QSharedPointer<SubSpaceMap> createSubspaceMapFrom(QDomNode const& subspaceMapNode,
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Reads the attributes from XML to a subspace map.
         *
         *    @param [in] subspaceMapNode     The XML description of the subspace map.
         *    @param [in/out] newSubspaceMap  The subspace map to insert the attributes into.
         *    @param [in] docRevision         The IP-XACT standard revision to comply to.
         */
        void parseAttributes(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap, 
            Document::Revision docRevision);
    }
}

#endif // SUBSPACEMAPREADER_H