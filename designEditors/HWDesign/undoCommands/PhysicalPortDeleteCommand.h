//-----------------------------------------------------------------------------
// File: PhysicalPortDeleteCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a physical port in component.
//-----------------------------------------------------------------------------

#ifndef PHYSICALPORTDELETECOMMAND_H
#define PHYSICALPORTDELETECOMMAND_H

#include <QObject>
#include <QSharedPointer>
#include <QUndoCommand>

class Component;
class Port;

//-----------------------------------------------------------------------------
//! Undo command for removing a physical port in component.
//-----------------------------------------------------------------------------
class DeletePhysicalPortCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component from which to delete a physical port.
	 *      @param [in] port       The port to delete.
     *      @param [in] parent     The parent command.
     */
   DeletePhysicalPortCommand(QSharedPointer<Component>, QSharedPointer<Port> port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~DeletePhysicalPortCommand();

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
   DeletePhysicalPortCommand(DeletePhysicalPortCommand const& rhs);
   DeletePhysicalPortCommand& operator=(DeletePhysicalPortCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target component model.
    QSharedPointer<Component> component_;

    //! The port to delete.
	QSharedPointer<Port> port_;
};

//-----------------------------------------------------------------------------

#endif // PHYSICALPORTDELETECOMMAND_H
