//-----------------------------------------------------------------------------
// File: AddressSpaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Writer class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceWriter_H
#define AddressSpaceWriter_H

#include "AddressSpace.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressSpaceWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    AddressSpaceWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~AddressSpaceWriter();

    /*!
     *  Write a AddressSpace to an XML file.
     *
     *      @param [in] writer				The used xml writer.
     *      @param [in] AddressSpace		The AddressSpace to be written.
     */
    void writeAddressSpace(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace) const;

private:

    //! No copying allowed.
    AddressSpaceWriter(AddressSpaceWriter const& rhs);
	AddressSpaceWriter& operator=(AddressSpaceWriter const& rhs);

	/*!
	 *  Writes block size of the item.
	 *
	 *      @param [in] writer				The used writer.
	 *      @param [in] addressSpace		The address space being written.
	 */
	void writeBlockSize(QXmlStreamWriter &writer, QSharedPointer<AddressSpace> addressSpace) const;

	/*!
	 *  Writes segments, if any exists
	 *
	 *      @param [in] addressSpace		The address space which is to be written to XML.
	 *      @param [in] writer				The writer use to write XLM.
	 */
	void writeSegments(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter &writer) const;

	/*!
	 *  Writes a single segment.
	 *
	 *      @param [in] writer		The used writer.
	 *      @param [in] segment     The segment being written.
	 */
    void writeSingleSegment(QXmlStreamWriter& writer, QSharedPointer<Segment> segment) const;

	/*!
	 *  Writes block size of the item.
	 *
	 *      @param [in] writer				The used writer.
	 *      @param [in] addressSpace		The written attributes.
	 */
    void writeAttributeMap(QXmlStreamWriter& writer, QMap<QString, QString> attributes) const;

	/*!
	 *  Writes address unit bits.
	 *
	 *      @param [in] writer				The used writer.
	 *      @param [in] addressSpace		The address space being written.
	 */
    void writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace) const;

	/*!
	 *  Writes the local memory map.
	 *
	 *      @param [in] writer				The used writer.
	 *      @param [in] addressSpace		The address space being written.
	 */
    void writeLocalMemoryMap(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace) const;
};

#endif // AddressSpaceWriter_H