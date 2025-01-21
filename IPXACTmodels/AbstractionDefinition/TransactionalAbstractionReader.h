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

#include <IPXACTmodels/common/Document.h>

#include <QDomNode>
#include <QObject>
#include <QSharedPointer>

class Protocol;
class TransactionalAbstraction;
class TransactionalPort;

//-----------------------------------------------------------------------------
//! Reader for ipxact:transactional within abstraction definition.
//-----------------------------------------------------------------------------
namespace TransactionalAbstractionReader
{       
    /*!
     *  Creates a transactional for abstraction definition from XML description.
     *
     *    @param [in] transactionalNode   The XML document to create the transactional definition from.
     *
     *    @return The created transactional definition.
     */
    QSharedPointer<TransactionalAbstraction> createTransactionalAbstractionFrom(QDomNode const& transactionalNode,
        Document::Revision revision);

    namespace Details
    {
        /*!
         *  Reads the qualifier from XML to a transactional.
         *
         *    @param [in]     transactionalNode    The XML description of the transactional.
         *    @param [in/out] transactional        The transactional definition to insert the qualifier into.
         *    @param [in]     revision             The standard revision of the XML document being read.
         */
        void parseQualifier(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional,
            Document::Revision revision);

        /*!
         *  Reads the system ports from XML to a transactional.
         *
         *    @param [in]     transactionalNode    The XML description of the transactional.
         *    @param [in/out] transactional        The transactional definition to insert the system ports into.
         */
        void parseSystems(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional);
       
        /*!
         *  Creates a transactionalPort for a transactional from XML description.
         *
         *    @param [in]     transactionalPortNode    The XML description of the transactionalPort.
         *
         *    @return The created transactionalPort.
         */
        QSharedPointer<TransactionalPort> parseTransactionalPort(QDomNode const& portNode);    
 
        /*!
         *  Reads the protocol description for transactional port.
         *
         *    @param [in]     portNode            The XML description of the transactionalPort.
         *    @param [in/out] transactionalPort   The transactional port to insert the protocol into.
         */
        void parseProtocol(QDomNode const& portNode, QSharedPointer<TransactionalPort> transactionalPort);

        /*!
         *  Reads the initiator port from XML to a transactional.
         *
         *    @param [in]     transactionalNode    The XML description of the transactional.
         *    @param [in/out] transactional        The transactional definition to insert the initiator port into.
         */                                    
        void parseInitiator(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional,
            Document::Revision revision);
    
        /*!
         *  Reads the target port from XML to a transactional.
         *
         *    @param [in]     transactionalNode    The XML description of the transactional.
         *    @param [in/out] transactional        The transactional definition to insert the target port into.
         */ 
        void parseTarget(QDomNode const& transactionalNode, QSharedPointer<TransactionalAbstraction> transactional,
            Document::Revision revision);
    }
}

#endif // TRANSACTIONALABSTRACTIONREADER_H
