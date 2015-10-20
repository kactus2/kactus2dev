//-----------------------------------------------------------------------------
// File: businterfaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------

#ifndef businterfaceReader_H
#define businterfaceReader_H

#include "BusInterface.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT businterfaceReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    businterfaceReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~businterfaceReader();

    /*!
     *  Creates a new businterface from a given businterface node.
     *
	 *      @param [in] businterfaceNode    XML description of the businterface.
	 *
	 *      @return							A new BusInterface.
     */
    QSharedPointer<BusInterface> createbusinterfaceFrom(QDomNode const& businterfaceNode) const;

private:

    //! No copying allowed.
    businterfaceReader(businterfaceReader const& rhs);
    businterfaceReader& operator=(businterfaceReader const& rhs);

    /*!
     *  Reads the presence.
     *
     *      @param [in] businterfaceNode    XML description.
     *      @param [in] newbusinterface     The new businterface item.
     */
    void parseIsPresent(QDomNode const& businterfaceNode, QSharedPointer<BusInterface> newbusinterface) const;

    /*!
     *  Reads the name group.
     *
     *      @param [in] businterfaceNode    XML description.
     *      @param [in] newbusinterface		The new businterface item.
     */
	void parseNameGroup(QDomNode const& businterfaceNode, QSharedPointer<BusInterface> newbusinterface) const;
	
    /*!
     *  Tries to extract abstraction types from the parameter inspected and append them to parameter abstractionTypes.
     *
     *      @param [in] inspected			The potential XML-node to yield abstraction types.
	 *      @param [in] abstractionTypes    Each extracted abstraction type is appended to this list.
     */
	void readAbstractionTypes(QDomNode& inspected,
        QSharedPointer<QList<QSharedPointer<BusInterface::AbstractionType> > > abstractionTypes) const;

    /*!
     *  Tries to extract port maps from the parameter inspected and append them to parameter newAbstractionType.
     *
     *      @param [in] inspected			The potential XML-node to yield abstraction types.
	 *      @param [in] newAbstractionType	The object that will receive extracted port maps as a field.
     */
	void readPortMaps(QDomNode &inspected, QSharedPointer<BusInterface::AbstractionType> newAbstractionType) const;

	/*!
	 *  Extracts a logical port from parameter tempNode and sets it to parameter newPort.
	 *
	 *      @param [in] tempNode			The XML-node to yield port.
	 *      @param [in] newPort				The port map, which port is being set.
	 */
    void readLogicalPort(QDomNode& tempNode, QSharedPointer<PortMap> newPort) const;

	/*!
	 *  Extracts a physical port from parameter tempNode and sets it to parameter newPort.
	 *
	 *      @param [in] tempNode			The XML-node to yield port.
	 *      @param [in] newPort				The port map, which port is being set.
	 */
    void readPhysicalPort(QDomNode& tempNode, QSharedPointer<PortMap> newPort) const;

    /*!
     *  Tries to read interface mode from parameter inspected and set it to the parameter newbusinterface.
     *
     *      @param [in] inspected			The potential XML-node to yield interface mode.
	 *      @param [in] newbusinterface		This interface will have the extracted interface mode.
     */
	void readInterfaceMode(QDomNode& inspected, QSharedPointer<BusInterface> newbusinterface) const;
	
	/*!
	*  Reads fields for a master interface struct from XML-input.
	*
	*      @param [in] inpstected			A reference to a QDomNode to parse the information from.
	*      @param [in] newmode				The interface object, which fields will be assigned.
	*/
	void readMasterInterface(QDomNode &inspected, QSharedPointer<MasterInterface> newmode) const;
	
	/*!
	*  Reads fields for a slave interface struct from XML-input.
	*
	*      @param [in] inpstected			A reference to a QDomNode to parse the information from.
	*      @param [in] newmode				The interface object, which fields will be assigned.
	*/
	void readSlaveInterface(QDomNode &inspected, QSharedPointer<SlaveInterface> newmode) const;
	
	/*!
	*  Reads fields for a mirrored interface struct from XML-input.
	*
	*      @param [in] inpstected			A reference to a QDomNode to parse the information from.
	*      @param [in] newmode				The interface object, which fields will be assigned.
	*/
	void readMirroredSlaveInterface(QDomNode &inspected, QSharedPointer<MirroredSlaveInterface> newmode) const;

	/*!
	*  Reads fields for a monitor interface struct from XML-input.
	*
	*      @param [in] inpstected			A reference to a QDomNode to parse the information from.
	*      @param [in] newmode				The interface object, which fields will be assigned.
	*/
	void readMonitorInterface(QDomNode& inpstected, QSharedPointer<BusInterface::MonitorInterface> newmode) const;

    /*!
     *  Read the extensions of the bus interface.
     *
     *      @param [in] interfaceNode   XML description of the bus interface.
     *      @param [in] newInterface    The new bus interface item.
     */
    void readBusInterfaceExtensions(QDomNode const& interfaceNode, QSharedPointer<BusInterface> newInterface) const;
};

#endif // businterfaceReader_H