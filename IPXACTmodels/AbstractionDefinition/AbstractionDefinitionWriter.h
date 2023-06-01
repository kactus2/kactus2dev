//-----------------------------------------------------------------------------
// File: AbstractionDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Writer for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONWRITER_H
#define ABSTRACTIONDEFINITIONWRITER_H


#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/DocumentWriter.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

class AbstractionDefinition;
class PortAbstraction;
class VLNV;
class WirePort;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------
namespace AbstractionDefinitionWriter
{    
    /*!
     *  Writes the given abstraction definition into XML.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] abstractionDefinition   The abstraction definition to write.
     */
    IPXACTMODELS_EXPORT void writeAbstractionDefinition(QXmlStreamWriter& writer, 
        QSharedPointer<AbstractionDefinition> abstractionDefinition);

    namespace Details
    {
        /*!
         *  Writes the bus type (VLNV) detailed in the given abstraction definition into XML.
         *
         *      @param [in] writer                  The XML writer to use.
         *      @param [in] abstractionDefinition   The abstraction definition whose bus type to write.
         */
        void writeBusType(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition);
       
        /*!
         *  Writes the abstraction definition (VLNV) extended in the given abstraction definition into XML.
         *
         *      @param [in] writer                  The XML writer to use.
         *      @param [in] abstractionDefinition   The abstraction definition whose extended VLVN to write.
         */
        void writeExtends(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> busDefinition);
        
        /*!
         *  Writes the port definitions in the given abstraction definition into XML.
         *
         *      @param [in] writer                  The XML writer to use.
         *      @param [in] abstractionDefinition   The abstraction definition whose ports to write.
         */
        void writePorts(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition);

        /*!
         *  Writes a port definitions into XML.
         *
         *      @param [in] writer          The XML writer to use.
         *      @param [in] logicalPort     The port to write.
         */
        void writePort(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort, 
            Document::Revision revision);
    
        /*!
         *  Writes the wire definition in a port definitions into XML.
         *
         *      @param [in] writer          The XML writer to use.
         *      @param [in] logicalPort     The port whose wire definition to write.
         */
        void writeWire(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort,
            Document::Revision revision);
        
        /*!
         *  Writes the transactional definition in a port definitions into XML.
         *
         *      @param [in] writer          The XML writer to use.
         *      @param [in] logicalPort     The port whose transactional definition to write.
         */
        void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort,
            Document::Revision revision);

        void writePackets(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort);

        void writeDescription(QXmlStreamWriter& writer,
            QSharedPointer<AbstractionDefinition> abstractionDefinition);

        void writeChoices(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition);
    }
};    

#endif // ABSTRACTIONDEFINITIONWRITER_H