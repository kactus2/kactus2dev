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
#include <QObject>
#include <QSharedPointer>

class Protocol;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:protocl within component.
//-----------------------------------------------------------------------------
class ProtocolReader : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    ProtocolReader(QObject* parent = 0);

    //! The destructor.
    ~ProtocolReader();  

    /*!
     *  Creates a protocol from XML description.
     *
     *      @param [in] protocolNode    The XML description of the protocol.
     */
    QSharedPointer<Protocol> createProtocolFrom(QDomNode const& protocolNode) const;

private:	
    // Disable copying.
    ProtocolReader(ProtocolReader const& rhs);
    ProtocolReader& operator=(ProtocolReader const& rhs);
    
    /*!
     *  Reads the vendor extensions for protocol.
     *
     *      @param [in]     payloadNode     The XML description of the payload.
     *      @param [in/out] protocol        The protocol to insert the vendor extensions into.
     */
    void parseVendorExtensions(QDomNode const& payloadNode, QSharedPointer<Protocol> protocol) const;
};

#endif // PROTOCOLREADER_H
