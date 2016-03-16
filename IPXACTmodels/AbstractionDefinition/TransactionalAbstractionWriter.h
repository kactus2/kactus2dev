//-----------------------------------------------------------------------------
// File: TransactionalAbstractionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Writer class for IP-XACT Transactional element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALABSTRACTIONWRITER_H
#define TRANSACTIONALABSTRACTIONWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class TransactionalAbstraction;
class TransactionalPort;
class Protocol;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Transactional element within abstraction definition. 
//-----------------------------------------------------------------------------
class TransactionalAbstractionWriter : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
	TransactionalAbstractionWriter(QObject* parent = 0);

	//! The destructor.
	~TransactionalAbstractionWriter();
     
    /*!
     *  Writes the given transactional description into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] transactional   The transactional to write.
     */
    void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

private:

	// Disable copying.
	TransactionalAbstractionWriter(TransactionalAbstractionWriter const& rhs);
	TransactionalAbstractionWriter& operator=(TransactionalAbstractionWriter const& rhs);

        
    /*!
     *  Writes the given transactional qualifier into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] transactional   The transactional whose qualifier to write.
     */
    void writeQualifier(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const; 
            
    /*!
     *  Writes the system port for the given transactional into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] wire        The transactional whose system port to write.
     */
    void writeSystem(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;
                
    /*!
     *  Writes the transactional port on system/master/slave into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] port        The transactional port to write.
     */
    void writeTransactionalPort(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
                   
    /*!
     *  Writes the transactional port presence on system/master/slave into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] port        The transactional port whose presence to write.
     */
    void writePresence(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
                       
    /*!
     *  Writes the transactional port initiative on system/master/slave into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] port        The transactional port whose initiative to write.
     */
    void writeInitiative(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
                           
    /*!
     *  Writes the transactional port type on system/master/slave into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] port        The transactional port whose type to write.
     */
    void writeKind(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
                               
    /*!
     *  Writes the transactional port width in bits on system/master/slave into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] port        The transactional port whose width to write.
     */
    void writeBusWidth(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
       
    /*!
     *  Writes the transactional port protocol vendor extensions on system/master/slave into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] portProtocol    The protocol whose vendor extensions to write.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;
                
    /*!
     *  Writes the master port for the given transactional into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] wire        The transactional whose master port to write.
     */
    void writeMaster(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

    /*!
     *  Writes the slave port for the given transactional into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] wire        The transactional whose slave port to write.
     */
    void writeSlave(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

};

#endif // TRANSACTIONALABSTRACTIONWRITER_H