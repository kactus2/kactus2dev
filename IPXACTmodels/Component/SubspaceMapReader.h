//-----------------------------------------------------------------------------
// File: SubspaceMapReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.01.2022
//
// Description:
// Reader class for ipxact:subspaceMap element.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPREADER_H
#define SUBSPACEMAPREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

class SubSpaceMap;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:subspaceMap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubspaceMapReader : public CommonItemsReader
{
public:

    /*!
     *  The constructor.
     */
    SubspaceMapReader();

    /*!
     *  The destructor.
     */
    ~SubspaceMapReader() = default;

    /*!
     *  Creates a new subspace map from a given subspace map node.
     *
     *      @param [in] subspaceMapNode     XML description of the subspace map.
     *
     *      @return The created subspace map.
     */
    QSharedPointer<SubSpaceMap> createSubspaceMapFrom(QDomNode const& subspaceMapNode) const;

private:

    //! No copying allowed.
    SubspaceMapReader(SubspaceMapReader const& rhs);
    SubspaceMapReader& operator=(SubspaceMapReader const& rhs);
    
    /*!
     *  Reads the attributes from XML to a subspace map.
     *
     *      @param [in] subspaceMapNode     The XML description of the subspace map.
     *      @param [in/out] newSubspaceMap  The subspace map to insert the attributes into.
     */
    void parseAttributes(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap) const;

    /*!
     *  Reads the name group.
     *
     *      @param [in] subspaceMapNode     XML description of the subspace map.
     *      @param [in] newSubspaceMap      The new subspace map.
     */
    void parseNameGroup(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap) const;

    /*!
     *  Reads the presence.
     *
     *      @param [in] subspaceMapNode     XML description of the subspace map.
     *      @param [in] newSubspaceMap      The new subspace map.
     */
    void parsePresence(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap) const;

    /*!
     *  Reads the base address.
     *
     *      @param [in] subspaceMapNode     XML description of the subspace map.
     *      @param [in] newSubspaceMap      The new subspace map.
     */
    void parseBaseAddress(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap) const;

    /*!
     *  Reads the parameters.
     *
     *      @param [in] subspaceMapNode     XML description of the subspace map.
     *      @param [in] newSubspaceMap      The new subspace map.
     */
    void parseParameters(QDomNode const& subspaceMapNode, QSharedPointer<SubSpaceMap> newSubspaceMap) const;
};

#endif // SUBSPACEMAPREADER_H