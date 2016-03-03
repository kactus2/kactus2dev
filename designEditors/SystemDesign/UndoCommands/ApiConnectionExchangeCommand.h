//-----------------------------------------------------------------------------
// File: ApiConnectionExchangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.03.2016
//
// Description:
// Undo command for exchanging API connections.
//-----------------------------------------------------------------------------

#ifndef APICONNECTIONEXCHANGECOMMAND_H
#define APICONNECTIONEXCHANGECOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QSharedPointer>

class ApiGraphicsConnection;
class ConnectionEndpoint;
class HierInterface;

//-----------------------------------------------------------------------------
//! Undo command for exchanging API connections.
//-----------------------------------------------------------------------------
class ApiConnectionExchangeCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] connection      The selected API graphics connection.
     *      @param [in] oldEndpoint     The old endpoint from which to disconnect the connection.
     *      @param [in] newEndpoint     The new endpoint to which to connect the connection.
     *      @param [in] parent          The parent command.
     */
    ApiConnectionExchangeCommand(ApiGraphicsConnection* connection, ConnectionEndpoint* oldEndpoint,
        ConnectionEndpoint* newEndpoint, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApiConnectionExchangeCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:

    // Disable copying.
    ApiConnectionExchangeCommand(ApiConnectionExchangeCommand const& rhs);
    ApiConnectionExchangeCommand& operator=(ApiConnectionExchangeCommand const& rhs);

    /*!
     *  Create interfaces for the exchanged connection..
     */
    void createExchangedInterfaces();

    /*!
     *  Create an interface from the selected connection end point.
     *
     *      @param [in] endPoint    The selected connection end point.
     *
     *      @return The created interface.
     */
    QSharedPointer<HierInterface> createConnectionInterface(ConnectionEndpoint* endPoint);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The API graphics connection.
    ApiGraphicsConnection* connection_;

    //! The old endpoint.
    ConnectionEndpoint* oldEndpoint_;

    //! The new endpoint.
    ConnectionEndpoint* newEndpoint_;

    //! The old interface.
    QSharedPointer<HierInterface> oldInterface_;

    //! The new interface.
    QSharedPointer<HierInterface> newInterface_;
};
//-----------------------------------------------------------------------------

#endif // APICONNECTIONEXCHANGECOMMAND_H
