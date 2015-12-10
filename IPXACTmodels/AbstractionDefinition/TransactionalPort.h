//-----------------------------------------------------------------------------
// File: TransactionalPort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation for ipxact:transactionalPort.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALPORT_H
#define TRANSACTIONALPORT_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/PresenceTypes.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <QSharedPointer>

class Protocol;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:transactionalPort. 
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TransactionalPort
{
public:
    
    //! The constructor.
    TransactionalPort();

    //! Copy constructor.
    TransactionalPort(TransactionalPort const& other);

    //! The destructor.
    ~TransactionalPort();
        
    /*!
     *  Sets the group name for system mode.
     *
     *      @param [in] groupName   The system group to set.
     */
    void setSystemGroup(QString const& group);
        
    /*!
     *  Gets the group name for system mode.
     *
     *      @return The system group the transactional port belongs to.
     */
    QString getSystemGroup() const;

    /*!
     *  Sets the presence restriction for the port.
     *
     *      @param [in] presence   The restriction to set.
     */
    void setPresence(PresenceTypes::Presence presence);

    /*!
     *  Gets the presence restriction for the port.
     *
     *      @return The presence restriction for the port.
     */
    PresenceTypes::Presence getPresence() const;

    /*!
     *  Sets the access type for the transactional port.
     *
     *      @param [in] initiative   The access type to set.
     */
    void setInitiative(QString const& initiative);
    
    /*!
     *  Gets the access type for the transactional port.
     *
     *      @return The access type for the port.
     */
    QString getInitiative() const;

    /*!
     *  Sets the port type.
     *
     *      @param [in] kind   The port type to set.
     */
    void setKind(QString const& kind);
    
    /*!
     *  Gets the port type.
     *
     *      @return The port type.
     */
    QString getKind() const;

    /*!
     *  Sets the data width on the bus.
     *
     *      @param [in] widthExpression   The number of bits on the bus.
     */
    void setBusWidth(QString const& widthExpression);
    
    /*!
     *  Gets the data width on the bus.
     *
     *      @return The number of bits on the bus.
     */
    QString getBusWidth() const;
    
    /*!
     *  Checks if a protocol has been defined for the port.
     *
     *
     *      @return True, if a protocol is defined, otherwise false.
     */
    bool hasProtocol() const;

    /*!
     *  Sets the protocol for the information transportation.
     *
     *      @param [in] portProtocol   The protocol to use on the port.
     */
    void setProtocol(QSharedPointer<Protocol> portProtocol);

    /*!
     *  Gets the protocol for the information transportation.
     *
     *      @return The protocol used on the port.
     */
    QSharedPointer<Protocol> getProtocol() const;

private:

    // Disable assignment.
    TransactionalPort& operator=(TransactionalPort const& rhs);

    //! The group this transactional port belongs to in system mode.
    QString systemGroup_;
    
    //! Defines if the port is required or forbidden in a bus interface.
    PresenceTypes::Presence presence_;

    //! Defines the access type for the port.
    QString initiative_;

    //! Defines the port type.
    QString kind_;

    //! Defines the bus width.
    QString busWidth_;

    //! The protocol to use by the port.
    QSharedPointer<Protocol> protocol_;

};

#endif // TRANSACTIONALPORT_H