//-----------------------------------------------------------------------------
// File: FloatingItemAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Add command for sticky note items.
//-----------------------------------------------------------------------------

#ifndef STICKYNOTEADDCOMMAND_H
#define STICKYNOTEADDCOMMAND_H

#include <QUndoCommand>

#include <IPXACTmodels/VendorExtension.h>

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>
#include <QSharedPointer>

class StickyNote;

//-----------------------------------------------------------------------------
//! StickyNoteAddCommand class.
//-----------------------------------------------------------------------------
class StickyNoteAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item        The item to add.
     *      @param [in] scene       The scene to which to add the item.
     *      @param [in] position    The initial position of the item.
     *      @param [in] parent      The parent command.
     */
    StickyNoteAddCommand(StickyNote* noteItem, QGraphicsScene* scene, QPointF position, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~StickyNoteAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

    void addVendorExtension(QSharedPointer<VendorExtension>);

    void removeVendorExtension(QSharedPointer<VendorExtension>);

private:
    // Disable copying.
    StickyNoteAddCommand(StickyNoteAddCommand const& rhs);
    StickyNoteAddCommand& operator=(StickyNoteAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    StickyNote* note_;

    //! The item's parent column.
    QGraphicsScene* scene_;

    //! The item's initial position.
    QPointF position_;

    //! Boolean flag for indicating if the item should be deleted in the destructor.
    bool del_;

};

#endif //STICKYNOTEADDCOMMAND_H