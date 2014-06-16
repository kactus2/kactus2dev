//-----------------------------------------------------------------------------
// File: AssociationRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.06.2014
//
// Description:
// Remove command for association items.
//-----------------------------------------------------------------------------

#ifndef ASSOCIATIONREMOVECOMMAND_H
#define ASSOCIATIONREMOVECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class Association;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! AssociationRemoveCommand class.
//-----------------------------------------------------------------------------
class AssociationRemoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] association     The association to remove.
     *      @param [in] scene           The scene from which to remove the association from.
     *      @param [in] parent          The parent command.
     */
    AssociationRemoveCommand(Association* association, QGraphicsScene* scene, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AssociationRemoveCommand();

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
    AssociationRemoveCommand(AssociationRemoveCommand const& rhs);
    AssociationRemoveCommand& operator=(AssociationRemoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    Association* association_;

    //! The item's parent scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the items should be deleted in the destructor.
    bool del_;

};

//-----------------------------------------------------------------------------
#endif // ASSOCIATIONREMOVECOMMAND_H