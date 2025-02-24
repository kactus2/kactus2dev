//-----------------------------------------------------------------------------
// File: Protocol.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation of ipxact:protocol.
//-----------------------------------------------------------------------------

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Extendable.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:protocol.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Protocol : public Extendable
{
public:

	//! The destructor.
	~Protocol() final = default;

    /*!
     *  Create a copy of the Protocol.
     *
     *    @return A Protocol identical to this.
     */
    Protocol* clone() const;

    /*!
     *  Sets the protocol type.
     *
     *    @param [in] type   The type to set.
     */
    void setProtocolType(QString const& type);

    /*!
     *  Gets the protocol type.
     *
     *    @return The protocol type.
     */
    QString getProtocolType() const;
    
    QString getCustomProtocolType() const;

    /*!
     *  Checks if the protocol has payload defined.
     *
     *    @return True, if payload is defined, otherwise false.
     */
    bool hasPayload() const;

    /*!
     *  Sets the name for the payload.
     *
     *    @param [in] payloadName   The name to set.
     */
    void setPayloadName(QString const& payloadName);
   
    /*!
     *  Gets the name of the payload.
     *
     *    @return The name of the payload.
     */
    QString getPayloadName() const;

    /*!
     *  Sets the type for the payload.
     *
     *    @param [in] payloadType   The type to set.
     */
    void setPayloadType(QString const& payloadType);

    /*!
     *  Gets the payload type.
     *
     *    @return The payload type.
     */
    QString getPayloadType() const;

    /*!
     *  Sets the extension for the payload.
     *
     *    @param [in] extension   The extension to set.
     *    @param [in] mandatory   Specifies if the extension is mandatory or not.
     */
    void setPayloadExtension(QString const& extension, bool mandatory);

    /*!
     *  Gets the payload extension.
     *
     *    @return The payload extension.
     */
    QString getPayloadExtension() const;

    /*!
     *  Checks if the payload extension is mandatory or not.
     *
     *    @return True, if the extension is mandatory, otherwise false.
     */
    bool hasMandatoryPayloadExtension() const;

private:

    //! The type of the protocol.
    QString protocolType_;

    //! The type of a custom protocol.
    QString customProtocolType_;

    //! The name of the payload in the protocol.
    QString payloadName_;

    //! The type of the payload in the protocol.
    QString payloadType_;

    //! The extension of the payload in the protocol.
    QString payloadExtension_;

    //! Specifies if the payload extension is mandatory or not.
    bool mandatoryPayloadExtension_{ false };
};

#endif // PROTOCOL_H
