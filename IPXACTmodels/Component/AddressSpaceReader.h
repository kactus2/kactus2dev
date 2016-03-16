//-----------------------------------------------------------------------------
// File: AddressSpaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Reader class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceReader_H
#define AddressSpaceReader_H

#include "AddressSpace.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressSpaceReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    AddressSpaceReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~AddressSpaceReader();

    /*!
     *  Creates a new AddressSpace from a given AddressSpace node.
     *
     *      @param [in] AddressSpaceNode    XML description of the address space.
     *
	 *		@return		The created address space.
     */
    QSharedPointer<AddressSpace> createAddressSpaceFrom(QDomNode const& addressSpaceNode) const;

private:

    //! No copying allowed.
    AddressSpaceReader(AddressSpaceReader const& rhs);
	AddressSpaceReader& operator=(AddressSpaceReader const& rhs);

    /*!
     *  Parse the name group.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
    void parseNameGroup(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;
    
    /*!
     *  Parse the block size group.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
	void readBlockSize(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;

    /*!
     *  Parse the segments.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
    void readSegments(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;

    /*!
     *  Parse the address offset.
     *
     *      @param [in] segmentNode     XML description of a segment.
     *      @param [in] newSegment      The new segment.
     */
    void parseAddressOffset(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment) const;

    /*!
     *  Parse the range.
     *
     *      @param [in] segmentNode     XML description of a segment.
     *      @param [in] newSegment      The new segment.
     */
    void parseRange(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment) const;

    /*!
     *  Parse the address unit bits.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
    void parseAddressUnitBits(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;

    /*!
     *  Parse the local memory map.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
    void parseLocalMemoryMap(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;

    /*!
     *  Parse the parameters.
     *
     *      @param [in] addressSpaceNode    XML description of the address space.
     *      @param [in] newAddressSpace     The new address space.
     */
    void readParameters(QDomNode const& addressSpaceNode, QSharedPointer<AddressSpace> newAddressSpace) const;
};

#endif // AddressSpaceReader_H