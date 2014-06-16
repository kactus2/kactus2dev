//-----------------------------------------------------------------------------
// File: AssociationAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.06.2014
//
// Description:
// Add command for association items.
//-----------------------------------------------------------------------------

#ifndef ASSOCIATIONADDCOMMAND_H
#define ASSOCIATIONADDCOMMAND_H


#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>


class Association;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! FloatingItemRemoveCommand class.
//-----------------------------------------------------------------------------
class AssociationAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] association     The association to add.
     *      @param [in] scene           The scene to add the association to.
     *      @param [in] parent          The parent command.
     */
    AssociationAddCommand(Association* association, QGraphicsScene* scene, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AssociationAddCommand();

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
    AssociationAddCommand(AssociationAddCommand const& rhs);
    AssociationAddCommand& operator=(AssociationAddCommand const& rhs);

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
#endif // ASSOCIATIONADDCOMMAND_H