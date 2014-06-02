//-----------------------------------------------------------------------------
// File: FloatingItemRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Remove command for floating items.
//-----------------------------------------------------------------------------

#ifndef FLOATINGITEMREMOVECOMMAND_H
#define FLOATINGITEMREMOVECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>

//-----------------------------------------------------------------------------
//! FloatingItemRemoveCommand class.
//-----------------------------------------------------------------------------
class FloatingItemRemoveCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The item to remove.
     *      @param [in] scene   The scene from which to remove the item.
     *      @param [in] parent  The parent command.
     */
    FloatingItemRemoveCommand(QGraphicsItem* item, QGraphicsScene* scene, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~FloatingItemRemoveCommand();

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
    FloatingItemRemoveCommand(FloatingItemRemoveCommand const& rhs);
    FloatingItemRemoveCommand& operator=(FloatingItemRemoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The item's parent scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

};

//-----------------------------------------------------------------------------
#endif // FLOATINGITEMREMOVECOMMAND_H