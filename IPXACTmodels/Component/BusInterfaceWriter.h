//-----------------------------------------------------------------------------
// File: businterfaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Writer class for IP-XACT businterface element.
//-----------------------------------------------------------------------------

#ifndef businterfaceWriter_H
#define businterfaceWriter_H

#include "BusInterface.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT businterface element.
//-----------------------------------------------------------------------------
namespace BusInterfaceWriter 
{
    /*!
     *  Write a businterface to an XML file.
     *
     *    @param [in] writer			The used xml writer.
     *    @param [in] businterface	The businterface to be written.
     *    @param [in] docRevision     The applied IP-XACT standard revision.
     */
	IPXACTMODELS_EXPORT void writeBusInterface(QXmlStreamWriter& writer,
		QSharedPointer<BusInterface> businterface, Document::Revision docRevision);

	namespace Details
	{

		/*!
		 *  Writes the connection requirement.
		 *
		 *    @param [in] writer          The used XML writer.
		 *    @param [in] busInterface    The bus interface to be written.
		 */
		void writeConnectionRequired(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface);

		/*!
		 *  Writes the bits in lau.
		 *
		 *    @param [in] writer          The used XML writer.
		 *    @param [in] busInterface    The bus interface to be written.
		 */
		void writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface);

		/*!
		 *  Writes the endianness.
		 *
		 *    @param [in] writer          The used XML writer.
		 *    @param [in] busInterface    The bus interface to be written.
		 */
		void writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface);

		/*!
		 *  Write the bit steering of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface to be written.
		 */
		void writeBitSteering(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface);

		/*!
		 *  Write the abstraction types of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
         *    @param [in] businterface		The businterface to be written.
         *    @param [in] docRevision			The applied IP-XACT standard revision.
		 */
		void writeAbstractionTypes(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface,
			Document::Revision docRevision);

		/*!
		 *  Write the port maps of an abstraction type.
		 *
		 *    @param [in] writer				The used xml writer.
         *    @param [in] abstractionType		The abstraction type whose port maps to write.
		 *    @param [in] docRevision			The applied IP-XACT standard revision.
		 */
		void writePortMaps(QXmlStreamWriter& writer, QSharedPointer<AbstractionType> abstractionType,
			Document::Revision docRevision);

		/*!
		 *  Write the logical port of port map.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] logicalPort		    The logical port to be written.
		 */
		void writeLogicalPort(QXmlStreamWriter& writer, QSharedPointer<PortMap::LogicalPort> physicalPort);

		/*!
		 *  Write the physical port of port map.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] physicalPort		The physical port to be written.
		 */
		void writePhysicalPort(QXmlStreamWriter& writer, QSharedPointer<PortMap::PhysicalPort> physicalPort);

		/*!
		 *  Write the interface mode of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface to be written.
		 */
		void writeInterfaceMode(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface);

		/*!
		 *  Write the master interface of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface master to be written.
		 */
		void writeMasterInterface(QXmlStreamWriter& writer, QSharedPointer<InitiatorInterface> masterInterface);

		/*!
		 *  Write the slave interface of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface slave to be written.
		 */
		void writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<TargetInterface> slave);

		/*!
		 *  Write the mirrored slave interface of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface to be written.
		 */
		void writeMirroredSlaveInterface(QXmlStreamWriter& writer,
			QSharedPointer<MirroredTargetInterface> mirroredSlave);

		/*!
		 *  Write the monitor interface of businterface.
		 *
		 *    @param [in] writer				The used xml writer.
		 *    @param [in] businterface		The businterface to be written.
		 */
		void writeMonitorInterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface);

		void writeInitiatorInterface(QXmlStreamWriter& writer, 
			QSharedPointer<InitiatorInterface> initiatorInterface, Document::Revision docRevsion);

		void writeTargetInterface(QXmlStreamWriter& writer, QSharedPointer<TargetInterface> target,
			Document::Revision docRevision);
		void writeMirroredTargetInterface(QXmlStreamWriter& writer, 
			QSharedPointer<MirroredTargetInterface> mirroredTarget, Document::Revision docRevision);
	}
};

#endif // businterfaceWriter_H