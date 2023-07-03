//-----------------------------------------------------------------------------
// File: AddressSpaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Reader for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceReader_H
#define AddressSpaceReader_H

#include "AddressSpace.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------
namespace AddressSpaceReader
{
    /*!
     *  Creates a new AddressSpace from a given AddressSpace node.
     *
     *      @param [in] AddressSpaceNode    XML description of the address space.
     *      @param [in] docRevision         The standard revision to use.
     *
	 *		@return		The created address space.
     */
    IPXACTMODELS_EXPORT QSharedPointer<AddressSpace> createAddressSpaceFrom(QDomNode const& addressSpaceNode, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Parse the name group.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void parseNameGroup(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);

        /*!
         *  Parse the block size group.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void readBlockSize(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);

        /*!
         *  Parse the segments.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void readSegments(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);

        /*!
         *  Parse the address offset.
         *
         *      @param [in] segmentNode     XML description of a segment.
         *      @param [in] newSegment      The new segment.
         */
        void parseAddressOffset(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment);

        /*!
         *  Parse the range.
         *
         *      @param [in] segmentNode     XML description of a segment.
         *      @param [in] newSegment      The new segment.
         */
        void parseRange(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment);

        /*!
         *  Parse the address unit bits.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void parseAddressUnitBits(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);

        /*!
         *  Parse the local memory map.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void parseLocalMemoryMap(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);

        /*!
         *  Parse the parameters.
         *
         *      @param [in] addressSpaceNode    XML description of the address space.
         *      @param [in] newAddressSpace     The new address space.
         */
        void readParameters(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace);
    }
};

#endif // AddressSpaceReader_H