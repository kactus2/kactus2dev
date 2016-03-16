//-----------------------------------------------------------------------------
// File: ApiConnectionDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete commands for API connections in system design.
//-----------------------------------------------------------------------------

#ifndef APICONNECTIONDELETECOMMAND_H
#define APICONNECTIONDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class ApiGraphicsConnection;

class Design;

//-----------------------------------------------------------------------------
//! Undo delete commands for API connections in system design.
//-----------------------------------------------------------------------------
class ApiConnectionDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] connection          The connection to delete.
     *      @param [in] containingDesign    The design containing the connection.
     *      @param [in] parent              The parent undo command.
     */
    ApiConnectionDeleteCommand(ApiGraphicsConnection* connection, QSharedPointer<Design> containingDesign,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ApiConnectionDeleteCommand();

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
    ApiConnectionDeleteCommand(ApiConnectionDeleteCommand const& rhs);
    ApiConnectionDeleteCommand& operator=(ApiConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The API interconnection.
    ApiGraphicsConnection* connection_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! The design containing the connection.
    QSharedPointer<Design> containingDesign_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool deleteConnection_;
};

//-----------------------------------------------------------------------------

#endif // APICONNECTIONDELETECOMMAND_H
