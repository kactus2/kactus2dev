//-----------------------------------------------------------------------------
// File: ComConnectionAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2016
//
// Description:
// Undo add commands for COM connections.
//-----------------------------------------------------------------------------

#ifndef COMCONNECTIONADDCOMMAND_H
#define COMCONNECTIONADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class ComGraphicsConnection;
class ConnectionEndpoint;

class Design;

//-----------------------------------------------------------------------------
//! Undo add commands for COM connections.
//-----------------------------------------------------------------------------
class ComConnectionAddCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] scene               The scene.
     *      @param [in] connection          The connection to add.
     *      @param [in] containingDesign    The design containing the connection.
     *      @param [in] parent              The parent command.
     */
    ComConnectionAddCommand(QGraphicsScene* scene, ComGraphicsConnection* connection,
        QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComConnectionAddCommand();

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
    ComConnectionAddCommand(ComConnectionAddCommand const& rhs);
    ComConnectionAddCommand& operator=(ComConnectionAddCommand const& rhs);

    /*!
     *  Change the port item status from API interface to COM interface.
     *
     *      @param [in] endPoint    The selected port item.
     */
    void changePortItemFromApiToCom(ConnectionEndpoint* endPoint);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The COM connection.
    ComGraphicsConnection* connection_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;

    //! The design containing the interconnection.
    QSharedPointer<Design> containingDesign_;
};

//-----------------------------------------------------------------------------

#endif // COMCONNECTIONADDCOMMAND_H
