//-----------------------------------------------------------------------------
// File: AddressSpaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Writer for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceWriter_H
#define AddressSpaceWriter_H

#include "AddressSpace.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------
namespace AddressSpaceWriter
{

    /*!
     *  Write a AddressSpace to an XML file.
     *
     *      @param [in] writer				The used xml writer.
     *      @param [in] AddressSpace		The AddressSpace to be written.
     *      @param [in] docRevision			The standard revision to use.
     */
	IPXACTMODELS_EXPORT void writeAddressSpace(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace,
		Document::Revision docRevision);
	
	namespace Details
	{

		/*!
		 *  Writes block size of the item.
		 *
		 *      @param [in] writer				The used writer.
		 *      @param [in] addressSpace		The address space being written.
		 */
		void writeBlockSize(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace);

		/*!
		 *  Writes segments, if any exists
		 *
		 *      @param [in] addressSpace		The address space which is to be written to XML.
		 *      @param [in] writer				The writer use to write XLM.
		 */
		void writeSegments(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter& writer);

		/*!
		 *  Writes a single segment.
		 *
		 *      @param [in] writer		The used writer.
		 *      @param [in] segment     The segment being written.
		 */
		void writeSingleSegment(QXmlStreamWriter& writer, QSharedPointer<Segment> segment);

		/*!
		 *  Writes block size of the item.
		 *
		 *      @param [in] writer				The used writer.
		 *      @param [in] addressSpace		The written attributes.
		 */
		void writeAttributeMap(QXmlStreamWriter& writer, QMap<QString, QString> attributes);

		/*!
		 *  Writes address unit bits.
		 *
		 *      @param [in] writer				The used writer.
		 *      @param [in] addressSpace		The address space being written.
		 */
		void writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace);

		/*!
		 *  Writes the local memory map.
		 *
		 *      @param [in] writer				The used writer.
		 *      @param [in] addressSpace		The address space being written.
		 *      @param [in] docRevision         The standard revision in use.
		 */
		void writeLocalMemoryMap(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace, 
			Document::Revision docRevision);
	}
};

#endif // AddressSpaceWriter_H