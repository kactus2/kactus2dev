//-----------------------------------------------------------------------------
// File: ProtocolReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.09.2015
//
// Description:
// Reader class for ipxact:protocl within component.
//-----------------------------------------------------------------------------

#ifndef PROTOCOLREADER_H
#define PROTOCOLREADER_H

#include <QDomNode>
#include <QSharedPointer>

class Protocol;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:protocl within component.
//-----------------------------------------------------------------------------
class ProtocolReader
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    ProtocolReader();

    //! The destructor.
    ~ProtocolReader();  

    // Disable copying.
    ProtocolReader(ProtocolReader const& rhs) = delete;
    ProtocolReader& operator=(ProtocolReader const& rhs) = delete;

    /*!
     *  Creates a protocol from XML description.
     *
     *      @param [in] protocolNode    The XML description of the protocol.
     */
    QSharedPointer<Protocol> createProtocolFrom(QDomNode const& protocolNode) const;

};

#endif // PROTOCOLREADER_H
