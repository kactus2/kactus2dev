//-----------------------------------------------------------------------------
// File: TransactionalAbstractionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.08.2015
//
// Description:
// Reader class for ipxact:transactional within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALABSTRACTIONREADER_H
#define TRANSACTIONALABSTRACTIONREADER_H

#include <QDomNode>
#include <QObject>
#include <QSharedPointer>

class Protocol;
class TransactionalAbstraction;
class TransactionalPort;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:transactional within abstraction definition.
//-----------------------------------------------------------------------------
class TransactionalAbstractionReader : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    TransactionalAbstractionReader(QObject* parent = 0);

    //! The destructor.
    ~TransactionalAbstractionReader();
        
    /*!
     *  Creates a transactional for abstraction definition from XML description.
     *
     *      @param [in] transactionalNode   The XML document to create the transactional definition from.
     *
     *      @return The created transactional definition.
     */
    QSharedPointer<TransactionalAbstraction> createTransactionalAbstractionFrom(QDomNode const& transactionalNode) const;

private:	
    // Disable copying.
    TransactionalAbstractionReader(TransactionalAbstractionReader const& rhs);
    TransactionalAbstractionReader& operator=(TransactionalAbstractionReader const& rhs);
                               
    /*!
     *  Reads the qualifier from XML to a transactional.
     *
     *      @param [in]     transactionalNode    The XML description of the transactional.
     *      @param [in/out] transactional        The transactional definition to insert the qualifier into.
     */
    void parseQualifier(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional) const;

    /*!
     *  Reads the system ports from XML to a transactional.
     *
     *      @param [in]     transactionalNode    The XML description of the transactional.
     *      @param [in/out] transactional        The transactional definition to insert the system ports into.
     */
    void parseSystems(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional) const;
       
    /*!
     *  Creates a transactionalPort for a transactional from XML description.
     *
     *      @param [in]     transactionalPortNode    The XML description of the transactionalPort.
     *
     *      @return The created transactionalPort.
     */
    QSharedPointer<TransactionalPort> parseTransactionalPort(QDomNode const& portNode) const;    
 
    /*!
     *  Reads the protocol description for transactional port.
     *
     *      @param [in]     portNode            The XML description of the transactionalPort.
     *      @param [in/out] transactionalPort   The transactional port to insert the protocol into.
     */
    void parseProtocol(QDomNode const& portNode, QSharedPointer<TransactionalPort> transactionalPort) const;

    /*!
     *  Reads the master port from XML to a transactional.
     *
     *      @param [in]     transactionalNode    The XML description of the transactional.
     *      @param [in/out] transactional        The transactional definition to insert the master port into.
     */                                    
    void parseMaster(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional) const;
    
    /*!
     *  Reads the slave port from XML to a transactional.
     *
     *      @param [in]     transactionalNode    The XML description of the transactional.
     *      @param [in/out] transactional        The transactional definition to insert the slave port into.
     */ 
    void parseSlave(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional) const;
  
};

#endif // TRANSACTIONALABSTRACTIONREADER_H
