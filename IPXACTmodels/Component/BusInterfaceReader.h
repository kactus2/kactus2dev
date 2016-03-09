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
#include <IPXACTmodels/Component/PortMap.h>
//-----------------------------------------------------------------------------
//! Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusinterfaceReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    BusinterfaceReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~BusinterfaceReader();

    /*!
     *  Creates a new businterface from a given bus interface XML description.
     *
	 *      @param [in] businterfaceNode    XML description of the businterface.
	 *
	 *      @return							A new BusInterface.
     */
    QSharedPointer<BusInterface> createbusinterfaceFrom(QDomNode const& businterfaceNode) const;

private:

    //! No copying allowed.
    BusinterfaceReader(BusinterfaceReader const& rhs);
    BusinterfaceReader& operator=(BusinterfaceReader const& rhs);

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
     *  Parses the abstraction types for the bus interface.
     *
     *      @param [in] inspected			The potential XML-node to yield abstraction types.
	 *      @param [in] busInterface        The new businterface item.
     */
	void parseAbstractionTypes(QDomElement const& abstractionTypesElement,
        QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Tries to extract port maps from the parameter inspected and append them to parameter abstractionType.
     *
     *      @param [in] inspected			The potential XML-node to yield abstraction types.
	 *      @param [in] abstractionType	    The object that will receive extracted port maps as a field.
     */
	void readPortMaps(QDomElement const& portMapsElement, QSharedPointer<AbstractionType> abstractionType) const;

	/*!
	 *  Extracts a logical port from parameter logicalPortElement and sets it to parameter portMap.
	 *
	 *      @param [in] logicalPortElement			The XML-node to yield port.
	 *      @param [in] portMap				        The port map, which port is being set.
	 */
    void readLogicalPort(QDomElement const& logicalPortElement, QSharedPointer<PortMap> portMap) const;

	/*!
	 *  Extracts a physical port from parameter physicalPortElement and sets it to parameter portMap.
	 *
	 *      @param [in] physicalPortElement		The XML-node to yield port.
	 *      @param [in] portMap				    The port map, which port is being set.
	 */
    void readPhysicalPort(QDomElement const& physicalPortElement, QSharedPointer<PortMap> portMap) const;

    /*!
     *  Tries to read interface mode from parameter busInterfaceElement and set it to newbusinterface.
     *
     *      @param [in] busInterfaceElement		The potential XML-node to yield interface mode.
	 *      @param [in] newbusinterface		    This interface will have the extracted interface mode.
     */
	void parseInterfaceMode(QDomElement const& busInterfaceElement, 
        QSharedPointer<BusInterface> newbusinterface) const;
	
	/*!
	*  Reads fields for a master interface struct from XML-input.
	*
	*      @param [in] masterInterfaceElement	The XML description to parse the information from.
	*      @param [in] masterInterface			The interface object, which fields will be assigned.
	*/
	void readMasterInterface(QDomElement const& masterInterfaceElement,
        QSharedPointer<MasterInterface> masterInterface) const;
	
	/*!
	*  Reads fields for a slave interface struct from XML-input.
	*
	*      @param [in] slaveIntefaceElement			The XML description to parse the information from.
	*      @param [in] slaveInterface				The interface object, which fields will be assigned.
	*/
	void readSlaveInterface(QDomElement const& slaveIntefaceElement, QSharedPointer<SlaveInterface> slaveInterface) const;
	
	/*!
	*  Reads fields for a mirrored interface struct from XML-input.
	*
	*      @param [in] mirroredInterfaceElement		The XML description to parse the information from.
	*      @param [in] mirroredSlaveInterface		The interface object, which fields will be assigned.
	*/
	void parseMirroredSlaveInterface(QDomElement const& mirroredInterfaceElement,
        QSharedPointer<MirroredSlaveInterface> mirroredSlaveInterface) const;

	/*!
	*  Reads fields for a monitor interface struct from XML-input.
	*
	*      @param [in] inpstected			The XML description to parse the information from.
	*      @param [in] newmode				The interface object, which fields will be assigned.
	*/
	void parseMonitorInterface(QDomElement const& monitorElement, QSharedPointer<BusInterface::MonitorInterface> monitorInterface) const;

    /*!
     *  Read the extensions of the bus interface.
     *
     *      @param [in] interfaceNode   XML description of the bus interface.
     *      @param [in] newInterface    The new bus interface item.
     */
    void readBusInterfaceExtensions(QDomNode const& interfaceNode, QSharedPointer<BusInterface> newInterface) const;
};

#endif // businterfaceReader_H
