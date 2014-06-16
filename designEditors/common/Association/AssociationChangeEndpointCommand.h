//-----------------------------------------------------------------------------
// File: AssociationChangeEndpointCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.06.2014
//
// Description:
// Command for changing the ending item of an association item.
//-----------------------------------------------------------------------------

#ifndef ASSOCIATIONCHANGEENDPOINTCOMMAND_H
#define ASSOCIATIONCHANGEENDPOINTCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>

class Associable;
class Association;

//-----------------------------------------------------------------------------
//! FloatingItemRemoveCommand class.
//-----------------------------------------------------------------------------
class AssociationChangeEndpointCommand : public QUndoCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] association     The association whose end point to change.
     *      @param [in] oldEndpoint     The previous end point item.
     *      @param [in] newEndpoint     The end point item to set.
     *      @param [in] parent          The parent command.
     */
    AssociationChangeEndpointCommand(Association* association, Associable* oldEndpoint, 
        Associable* newEndpoint, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AssociationChangeEndpointCommand();

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
    AssociationChangeEndpointCommand(AssociationChangeEndpointCommand const& rhs);
    AssociationChangeEndpointCommand& operator=(AssociationChangeEndpointCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    Association* association_;

    //! The previous ending point item.
    Associable* oldEndpoint_;

    //! The new ending point item.
    Associable* newEndpoint_;
};

//-----------------------------------------------------------------------------
#endif // ASSOCIATIONCHANGEENDPOINTCOMMAND_H