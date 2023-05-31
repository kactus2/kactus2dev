//-----------------------------------------------------------------------------
// File: AbstractionDefinitionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2015
//
// Description:
// Reader for ipxact:abstractionDefinition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONREADER_H
#define ABSTRACTIONDEFINITIONREADER_H

#include <IPXACTmodels/common/DocumentReader.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QSharedPointer>

class AbstractionDefinition;
class PortAbstraction;
class WireAbstraction;

//-----------------------------------------------------------------------------
//! Reader for ipxact:abstractionDefinition.
//-----------------------------------------------------------------------------
namespace AbstractionDefinitionReader
{
    /*!
     *  Creates an abstraction definition from XML description.
     *
     *      @param [in] document   The XML document to create the abstraction definition from.
     *
     *      @return The created abstraction definition.
     */
    IPXACTMODELS_EXPORT QSharedPointer<AbstractionDefinition> createAbstractionDefinitionFrom(QDomNode const& document);
    
    namespace Details
    {
        /*!
         *  Reads the bus type from XML to an abstraction definition.
         *
         *      @param [in]     definitionNode  The XML description of the abstraction definition.
         *      @param [in/out] busDefinition   The abstraction definition to insert the extends property into.
         */
        void parseBusType(QDomNode const& definitionNode,
            QSharedPointer<AbstractionDefinition> abstractionDefinion);
                        
        /*!
         *  Reads the extends property from XML to an abstraction definition.
         *
         *      @param [in]     definitionNode          The XML description of the abstraction definition.
         *      @param [in/out] abstractionDefinion     The abstraction definition to insert the extends property into.
         */
        void parseExtends(QDomNode const& definitionNode,
            QSharedPointer<AbstractionDefinition> abstractionDefinion);
                            
        /*!
         *  Reads the logical ports from XML to an abstraction definition.
         *
         *      @param [in]     definitionNode          The XML description of the abstraction definition.
         *      @param [in/out] abstractionDefinion     The abstraction definition to insert the ports into.
         */
        void parsePorts(QDomNode definitionNode, QSharedPointer<AbstractionDefinition> abstractionDefinion,
            Document::Revision revision);
                                
        /*!
         *  Read a logical ports from XML to an abstraction definition.
         *
         *      @param [in]     portNode    The XML description of the logical port.
         *
         *      @return The read logical port.
         */
        QSharedPointer<PortAbstraction> parsePort(QDomNode const& portNode, Document::Revision revision);
    
        /*!
         *	Reads port packets from XML.
         *  
         *      @param [in] portNode	The XML description of the logical port.
         *      @param [in/out] port	The logical port definition to insert the packets into.
         */
        void parsePackets(QDomNode const& portNode, QSharedPointer<PortAbstraction> port);

        /*!
         *  Reads the wire description from XML to a logical port definition.
         *
         *      @param [in]     portNode    The XML description of the logical port.
         *      @param [in/out] port        The logical port definition to insert the wire into.
         */
        void parseWire(QDomNode const& portNode, QSharedPointer<PortAbstraction> port, Document::Revision revision);
    
        /*!
         *  Reads the transactional description from XML to a logical port definition.
         *
         *      @param [in]     portNode    The XML description of the logical port.
         *      @param [in/out] port        The logical port definition to insert the transactional into.
         */
        void parseTransactional(QDomNode const& portNode, QSharedPointer<PortAbstraction> port, 
            Document::Revision revision);

        void parseChoices(QDomNode const& definitionNode, QSharedPointer<AbstractionDefinition> definition);
    }
}

#endif // ABSTRACTIONDEFINITIONREADER_H