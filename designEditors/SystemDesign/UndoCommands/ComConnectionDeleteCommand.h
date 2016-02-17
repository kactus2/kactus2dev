//-----------------------------------------------------------------------------
// File: ComConnectionDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete commands for COM connections in system design.
//-----------------------------------------------------------------------------

#ifndef COMCONNECTIONDELETECOMMAND_H
#define COMCONNECTIONDELETECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class ComGraphicsConnection;

class Design;

//-----------------------------------------------------------------------------
//! Undo delete commands for COM connections in system design.
//-----------------------------------------------------------------------------
class ComConnectionDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] connection          The connection to delete.
     *      @param [in] containingDesign    The design containing the connection.
     *      @param [in] parent              The parent undo command.
     */
    ComConnectionDeleteCommand(ComGraphicsConnection* connection, QSharedPointer<Design> containingDesign,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComConnectionDeleteCommand();

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
    ComConnectionDeleteCommand(ComConnectionDeleteCommand const& rhs);
    ComConnectionDeleteCommand& operator=(ComConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The COM interconnection.
    ComGraphicsConnection* connection_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! The design containing the interconnection.
    QSharedPointer<Design> containingDesign_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool deleteConnection_;
};

//-----------------------------------------------------------------------------

#endif // COMCONNECTIONDELETECOMMAND_H
