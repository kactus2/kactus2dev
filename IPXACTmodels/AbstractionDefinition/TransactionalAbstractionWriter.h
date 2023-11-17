//-----------------------------------------------------------------------------
// File: TransactionalAbstractionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Writer for IP-XACT Transactional element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALABSTRACTIONWRITER_H
#define TRANSACTIONALABSTRACTIONWRITER_H

#include <IPXACTmodels/common/Document.h>

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class TransactionalAbstraction;
class TransactionalPort;
class Protocol;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT Transactional element within abstraction definition. 
//-----------------------------------------------------------------------------
namespace TransactionalAbstractionWriter
{

    /*!
     *  Writes the given transactional description into XML.
     *
     *      @param [in] writer          The XML writer to use.
     *      @param [in] transactional   The transactional to write.
     *      @param [in] revision    The document standard revision to use.
     */
    IPXACTMODELS_EXPORT void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional,
        Document::Revision revision);

    namespace Details
    {
        /*!
         *  Writes the system port for the given transactional into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The transactional whose system port to write.
         */
        void writeSystem(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional);

        /*!
         *  Writes the transactional port on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] port        The transactional port to write.
         */
        void writeTransactionalPort(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port);

        /*!
         *  Writes the transactional port presence on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] port        The transactional port whose presence to write.
         */
        void writePresence(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port);

        /*!
         *  Writes the transactional port initiative on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] port        The transactional port whose initiative to write.
         */
        void writeInitiative(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port);

        /*!
         *  Writes the transactional port type on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] port        The transactional port whose type to write.
         */
        void writeKind(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port);

        /*!
         *  Writes the transactional port width in bits on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] port        The transactional port whose width to write.
         */
        void writeBusWidth(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port);

        /*!
         *  Writes the transactional port protocol vendor extensions on system/master/slave into XML.
         *
         *      @param [in] writer          The XML writer to use.
         *      @param [in] portProtocol    The protocol whose vendor extensions to write.
         */
        void writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol);

        /*!
         *  Writes the master port for the given transactional into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The transactional whose master port to write.
         *      @param [in] elementName     The XML element name to use.
         */
        void writeMaster(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional,
            QString const& elementName);

        /*!
         *  Writes the slave port for the given transactional into XML.
         *
         *      @param [in] writer          The XML writer to use.
         *      @param [in] wire            The transactional whose slave port to write.
         *      @param [in] elementName     The XML element name to use.
         */
        void writeSlave(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional,
            QString const& elementName);
    }
};

#endif // TRANSACTIONALABSTRACTIONWRITER_H