//-----------------------------------------------------------------------------
// File: ComConnectionExchangeCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.03.2016
//
// Description:
// Undo command for exchanging COM connections.
//-----------------------------------------------------------------------------

#ifndef COMCONNECTIONEXCHANGECOMMAND_H
#define COMCONNECTIONEXCHANGECOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QSharedPointer>

class ComGraphicsConnection;
class ConnectionEndpoint;
class HierInterface;

//-----------------------------------------------------------------------------
//! Undo command for exchanging COM connections.
//-----------------------------------------------------------------------------
class ComConnectionExchangeCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] connection      The selected COM graphics connection.
     *      @param [in] oldEndpoint     The old endpoint from which to disconnect the connection.
     *      @param [in] newEndpoint     The new endpoint to which to connect the connection.
     *      @param [in] parent          The parent command.
     */
    ComConnectionExchangeCommand(ComGraphicsConnection* connection, ConnectionEndpoint* oldEndpoint,
        ConnectionEndpoint* newEndpoint, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComConnectionExchangeCommand();

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
    ComConnectionExchangeCommand(ComConnectionExchangeCommand const& rhs);
    ComConnectionExchangeCommand& operator=(ComConnectionExchangeCommand const& rhs);

    /*!
     *  Create interfaces for the exchanged connection.
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

    //! The COM graphics connection.
    ComGraphicsConnection* connection_;

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

#endif // COMCONNECTIONEXCHANGECOMMAND_H
