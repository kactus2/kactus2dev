//-----------------------------------------------------------------------------
// File: ProtocolWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.09.2015
//
// Description:
// Writer class for IP-XACT protocol element.
//-----------------------------------------------------------------------------

#ifndef PROTOCOLWRITER_H
#define PROTOCOLWRITER_H

#include <QSharedPointer>
#include <QXmlStreamWriter>

class Protocol;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT protocol element.
//-----------------------------------------------------------------------------
class ProtocolWriter
{
public:

	//! The constructor.
    ProtocolWriter();

	//! The destructor.
    ~ProtocolWriter(); 

    // Disable copying.
    ProtocolWriter(ProtocolWriter const& rhs) = delete;
    ProtocolWriter& operator=(ProtocolWriter const& rhs) = delete;

    /*!
     *  Writes the given protocol description into XML.
     *
     *    @param [in] writer      The XML writer to use.
     *    @param [in] protocol    The protocol to write.
     */
    void writeProtocol(QXmlStreamWriter& writer, QSharedPointer<Protocol> protocol) const;

private:


    /*!
     *  Writes the transactional port protocol type on system/master/slave into XML.
     *
     *    @param [in] writer          The XML writer to use.
     *    @param [in] portProtocol    The protocol whose type to write.
     */
    void writeProtocolType(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;
       
    /*!
     *  Writes the transactional port protocol payload on system/master/slave into XML.
     *
     *    @param [in] writer          The XML writer to use.
     *    @param [in] portProtocol    The protocol whose payload to write.
     */
    void writePayload(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;

};

#endif // TRANSACTIONALABSTRACTIONWRITER_H