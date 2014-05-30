//-----------------------------------------------------------------------------
// File: FloatingItemAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Add command for floating items.
//-----------------------------------------------------------------------------

#ifndef FLOATINGITEMADDCOMMAND_H
#define FLOATINGITEMADDCOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>

//-----------------------------------------------------------------------------
//! FloatingItemAddCommand class.
//-----------------------------------------------------------------------------
class FloatingItemAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene   The scene to which to add the item.
     *      @param [in] item    The item to add.
     *      @param [in] parent  The parent command.
     */
    FloatingItemAddCommand(QGraphicsScene* scene, QGraphicsItem* item, QPointF position, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~FloatingItemAddCommand();

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
    FloatingItemAddCommand(FloatingItemAddCommand const& rhs);
    FloatingItemAddCommand& operator=(FloatingItemAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The item's parent column.
    QGraphicsScene* scene_;

    //! The item's initial position.
    QPointF position_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

};

#endif //FLOATINGITEMADDCOMMAND_H