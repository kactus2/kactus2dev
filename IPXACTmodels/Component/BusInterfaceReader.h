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

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------
namespace BusinterfaceReader 
{
    /*!
     *  Creates a new businterface from a given bus interface XML description.
     *
     *    @param [in] businterfaceNode    XML description of the businterface.
     *    @param [in] docRevision         The applied IP-XACT standard revision.
     *
	 *    @return							A new BusInterface.
     */
    IPXACTMODELS_EXPORT QSharedPointer<BusInterface> createBusinterfaceFrom(QDomNode const& businterfaceNode,
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Parses the bus type for the bus interface.
         *
         *    @param [in] businterfaceElement XML description of the businterface.
         *    @param [in] busInterface        The new businterface item.
         */
        void parseBusType(QDomElement& businterfaceElement, QSharedPointer<BusInterface> newbusinterface);

        /*!
         *  Parses the connectionRequired element for the bus interface.
         *
         *    @param [in] businterfaceElement XML description of the businterface.
         *    @param [in] busInterface        The new businterface item.
         */
        void parseConnectionRequired(QDomElement& businterfaceElement, QSharedPointer<BusInterface> newbusinterface);

        /*!
         *  Parses the bits in lau element for the bus interface.
         *
         *    @param [in] businterfaceElement XML description of the businterface.
         *    @param [in] busInterface        The new businterface item.
         */
        void parseBitsInLau(QDomElement& businterfaceElement, QSharedPointer<BusInterface> newbusinterface);

        /*!
         *  Parses the bit steering element for the bus interface.
         *
         *    @param [in] businterfaceElement XML description of the businterface.
         *    @param [in] busInterface        The new businterface item.
         */
        void parseBitSteering(QDomElement& businterfaceElement, QSharedPointer<BusInterface> newbusinterface);

        /*!
         *  Parses the endianess element for the bus interface.
         *
         *    @param [in] businterfaceElement XML description of the businterface.
         *    @param [in] busInterface        The new businterface item.
         */
        void parseEndianess(QDomElement& businterfaceElement, QSharedPointer<BusInterface> newbusinterface);

        /*!
         *  Parses the abstraction types for the bus interface.
         *
         *    @param [in] businterfaceElement The potential XML-node to yield abstraction types.
         *    @param [in] busInterface        The new businterface item.
         *    @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void parseAbstractionTypes(QDomElement const& businterfaceElement,
            QSharedPointer<BusInterface> busInterface, Document::Revision docRevision);

        /*!
         *  Reads the view references.
         *
         *    @param [in] abstractionNode     The examined abstraction type node.
         *    @param [in] newAbstractionType  The abstraction type matching the selected node.
         */
        void parseViewReferences(QDomNode const& abstractionNode,
            QSharedPointer<AbstractionType> newAbstractionType);

        /*!
         *  Tries to extract port maps from the parameter inspected and append them to parameter abstractionType.
         *
         *    @param [in] inspected			The potential XML-node to yield abstraction types.
         *    @param [in] abstractionType	    The object that will receive extracted port maps as a field.
         *    @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void parsePortMaps(QDomElement const& portMapsElement, QSharedPointer<AbstractionType> abstractionType,
            Document::Revision docRevision);

        /*!
         *  Extracts a logical port from parameter logicalPortElement and sets it to parameter portMap.
         *
         *    @param [in] logicalPortElement			The XML-node to yield port.
         *    @param [in] portMap				        The port map, which port is being set.
         */
        void parseLogicalPort(QDomElement const& logicalPortElement, QSharedPointer<PortMap> portMap);

        /*!
         *  Extracts a physical port from parameter physicalPortElement and sets it to parameter portMap.
         *
         *    @param [in] physicalPortElement		The XML-node to yield port.
         *    @param [in] portMap				    The port map, which port is being set.
         */
        void parsePhysicalPort(QDomElement const& physicalPortElement, QSharedPointer<PortMap> portMap);

        /*!
         *  Tries to read interface mode from parameter busInterfaceElement and set it to newbusinterface.
         *
         *    @param [in] busInterfaceElement		The potential XML-node to yield interface mode.
         *    @param [in] newbusinterface		    This interface will have the extracted interface mode.
         */
        void parseInterfaceMode(QDomElement const& busInterfaceElement,
            QSharedPointer<BusInterface> newbusinterface, Document::Revision docRevision);

        /*!
        *  Reads fields for a master interface struct from XML-input.
        *
        *    @param [in] masterInterfaceElement	The XML description to parse the information from.
        *    @param [in] masterInterface			The interface object, which fields will be assigned.
        */
        void parseMasterInterface(QDomElement const& masterInterfaceElement,
            QSharedPointer<InitiatorInterface> masterInterface);

        /*!
        *  Reads fields for a slave interface struct from XML-input.
        *
        *    @param [in] slaveIntefaceElement			The XML description to parse the information from.
        *    @param [in] slaveInterface				The interface object, which fields will be assigned.
        */
        void parseSlaveInterface(QDomElement const& slaveIntefaceElement,
            QSharedPointer<TargetInterface> slaveInterface);

        /*!
        *  Reads fields for a mirrored interface struct from XML-input.
        *
        *    @param [in] mirroredInterfaceElement		The XML description to parse the information from.
        *    @param [in] mirroredSlaveInterface		The interface object, which fields will be assigned.
        */
        void parseMirroredSlaveInterface(QDomElement const& mirroredInterfaceElement,
            QSharedPointer<MirroredTargetInterface> mirroredSlaveInterface);

        /*!
        *  Reads fields for a monitor interface struct from XML-input.
        *
        *    @param [in] monitorElement       The XML description to parse the information from.
        *    @param [in] newmode				The interface object, which fields will be assigned.
        */
        void parseMonitorInterface(QDomElement const& monitorElement,
            QSharedPointer<BusInterface::MonitorInterface> monitorInterface);

        void parseInitiatorInterface(QDomElement const& initiatorNode, QSharedPointer<InitiatorInterface> initiatorInterface);

        void parseTargetInterface(QDomElement const& targetNode, QSharedPointer<TargetInterface> targetInterface, 
            Document::Revision docRevision);

        void parseMirroredTargetInterface(QDomElement const& mirroredInterfaceElement, 
            QSharedPointer<MirroredTargetInterface> mirroredSlaveInterface);

        /*!
         *  Read the extensions of the bus interface.
         *
         *    @param [in] interfaceNode   XML description of the bus interface.
         *    @param [in] newInterface    The new bus interface item.
         */
        void parseBusInterfaceExtensions(QDomNode const& interfaceNode, QSharedPointer<BusInterface> newInterface);

        QStringList parseModeRefs(QDomElement const& containingElement);
    }
};

#endif // businterfaceReader_H

