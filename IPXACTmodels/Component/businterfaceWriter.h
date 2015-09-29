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

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>
#include <IPXACTmodels/common/CommonItemsWriter.h>

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
	 *  Write the default position of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeDefaultPos(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the parameters of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeParameters(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the bit steering of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeBitSteering(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the abstraction types of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeAbstractionTypes(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the logical port of port map.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeLogicalPort(QSharedPointer<PortMap> portMap, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the physical port of port map.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writePhysicalPort(QSharedPointer<PortMap> portMap, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the interface mode of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeInterfaceMode(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the master interface of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 *      @param [in] mirrored			True, if mirrored interface, else false.
	 */
	void writeMasterInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer,
		bool mirrored) const;
	
	/*!
	 *  Write the slave interface of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeSlaveInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the mirrored slave interface of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeMirroredSlaveInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
	
	/*!
	 *  Write the monitor interface of businterface.
	 *
	 *      @param [in] businterface		The businterface to be written.		
	 *      @param [in] writer				The used xml writer.
	 */
	void writeMonitorInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const;
};

#endif // businterfaceWriter_H