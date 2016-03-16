//-----------------------------------------------------------------------------
// File: AbstractionDefinitionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Writer class for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONWRITER_H
#define ABSTRACTIONDEFINITIONWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/DocumentWriter.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class AbstractionDefinition;
class PortAbstraction;
class VLNV;
class WirePort;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionDefinitionWriter : public DocumentWriter
{
    Q_OBJECT
public:

	//! The constructor.
	AbstractionDefinitionWriter(QObject* parent = 0);

	//! The destructor.
	~AbstractionDefinitionWriter();
    
    /*!
     *  Writes the given abstraction definition into XML.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] abstractionDefinition   The abstraction definition to write.
     */
    void writeAbstractionDefinition(QXmlStreamWriter& writer, 
        QSharedPointer<AbstractionDefinition> abstractionDefinition) const;

private:

	// Disable copying.
	AbstractionDefinitionWriter(AbstractionDefinitionWriter const& rhs);
	AbstractionDefinitionWriter& operator=(AbstractionDefinitionWriter const& rhs);
   
    /*!
     *  Writes the bus type (VLNV) detailed in the given abstraction definition into XML.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] abstractionDefinition   The abstraction definition whose bus type to write.
     */
    void writeBusType(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition) const;
       
    /*!
     *  Writes the abstraction definition (VLNV) extended in the given abstraction definition into XML.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] abstractionDefinition   The abstraction definition whose extended VLVN to write.
     */
    void writeExtends(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> busDefinition) const;
        
    /*!
     *  Writes the port definitions in the given abstraction definition into XML.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] abstractionDefinition   The abstraction definition whose ports to write.
     */
    void writePorts(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition) const;

    /*!
     *  Writes a port definitions into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] logicalPort     The port to write.
     */
    void writePort(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort) const;
    
    /*!
     *  Writes the wire definition in a port definitions into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] logicalPort     The port whose wire definition to write.
     */
    void writeWire(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort) const;
        
    /*!
     *  Writes the transactional definition in a port definitions into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] logicalPort     The port whose transactional definition to write.
     */
    void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort) const;
};    

#endif // ABSTRACTIONDEFINITIONWRITER_H