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

#include "businterface.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT businterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT businterfaceWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this Writer.
     */
    businterfaceWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~businterfaceWriter();
	
    /*!
     *  Write a businterface to an XML file.
     *
     *      @param [in] writer				The used xml writer.
     *      @param [in] businterface		The businterface to be written.
     */
    void writebusinterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;

private:

    //! No copying allowed.
    businterfaceWriter(businterfaceWriter const& rhs);
    businterfaceWriter& operator=(businterfaceWriter const& rhs);

    /*!
     *  Writes the name group to an XML file.
     *
     *      @param [in] writer				The used xml writer.
     *      @param [in] businterface		The businterface to be written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;

    /*!
     *  Writes the presence to an XML file.
     *
     *      @param [in] writer				The used xml writer.
     *      @param [in] businterface		The businterface to be written.
     */
	void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;

    /*!
     *  Writes the connection requirement.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] busInterface    The bus interface to be written.
     */
    void writeConnectionRequired(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Writes the bits in lau.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] busInterface    The bus interface to be written.
     */
    void writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Writes the endianness.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] busInterface    The bus interface to be written.
     */
    void writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const;
	
	/*!
	 *  Write the bit steering of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface to be written.		
	 */
	void writeBitSteering(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;
	
	/*!
	 *  Write the abstraction types of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface to be written.		
	 */
	void writeAbstractionTypes(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;

    void writePortMaps(QXmlStreamWriter& writer, QSharedPointer<AbstractionType> abstractionType) const;

	/*!
	 *  Write the logical port of port map.
	 *
	 *      @param [in] writer				The used xml writer.
     *      @param [in] logicalPort		    The logical port to be written.			 
	 */
	void writeLogicalPort(QXmlStreamWriter& writer, QSharedPointer<PortMap::LogicalPort> physicalPort) const;
	
	/*!
	 *  Write the physical port of port map.
	 *	 
	 *      @param [in] writer				The used xml writer.
     *      @param [in] physicalPort		The physical port to be written.		
	 */
	void writePhysicalPort(QXmlStreamWriter& writer, QSharedPointer<PortMap::PhysicalPort> physicalPort) const;
	
	/*!
	 *  Write the interface mode of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface to be written.		
	 */
	void writeInterfaceMode(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;
	
	/*!
	 *  Write the master interface of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface master to be written.		
	 *      @param [in] mirrored			True, if mirrored interface, else false.
	 */
	void writeMasterInterface(QXmlStreamWriter& writer, QSharedPointer<MasterInterface> masterInterface,
		bool mirrored) const;
	
	/*!
	 *  Write the slave interface of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface slave to be written.		
	 */
	void writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<SlaveInterface> slave) const;
	
	/*!
	 *  Write the mirrored slave interface of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface to be written.			 
	 */
	void writeMirroredSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<MirroredSlaveInterface> mirroredSlave) const;
	
	/*!
	 *  Write the monitor interface of businterface.
	 *
     *      @param [in] writer				The used xml writer.
	 *      @param [in] businterface		The businterface to be written.	
	 */
	void writeMonitorInterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const;
};

#endif // businterfaceWriter_H