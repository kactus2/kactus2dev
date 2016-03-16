//-----------------------------------------------------------------------------
// File: ApiConnectionAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo add commands for API connections.
//-----------------------------------------------------------------------------

#ifndef APICONNECTIONADDCOMMAND_H
#define APICONNECTIONADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class ApiGraphicsConnection;
class ConnectionEndpoint;

class Design;

//-----------------------------------------------------------------------------
//! Undo add commands for API connections.
//-----------------------------------------------------------------------------
class ApiConnectionAddCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] scene               The scene.
     *      @param [in] conn                The connection to add.
     *      @param [in] containingDesign    The design containing the API interconnection.
     *      @param [in] parent              The parent command.
     */
    ApiConnectionAddCommand(QGraphicsScene* scene, ApiGraphicsConnection* connection,
        QSharedPointer<Design> containingDesign, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApiConnectionAddCommand();

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
    ApiConnectionAddCommand(ApiConnectionAddCommand const& rhs);
    ApiConnectionAddCommand& operator=(ApiConnectionAddCommand const& rhs);

    /*!
     *  Change the port item status from COM interface to API interface.
     *
     *      @param [in] endPoint    The selected port item.
     */
    void changePortItemFromComToApi(ConnectionEndpoint* endPoint);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The API interconnection.
    ApiGraphicsConnection* connection_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;

    //! The design containing the interconnection.
    QSharedPointer<Design> containingDesign_;
};

//-----------------------------------------------------------------------------

#endif // APICONNECTIONADDCOMMAND_H
