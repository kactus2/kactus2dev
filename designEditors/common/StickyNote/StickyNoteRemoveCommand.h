//-----------------------------------------------------------------------------
// File: StickyNoteRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Remove command for sticky note items.
//-----------------------------------------------------------------------------

#ifndef STICKYNOTEREMOVECOMMAND_H
#define STICKYNOTEREMOVECOMMAND_H

#include "StickyNote.h"

#include <IPXACTmodels/VendorExtension.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QSharedPointer>

class DesignDiagram;

//-----------------------------------------------------------------------------
//! FloatingItemRemoveCommand class.
//-----------------------------------------------------------------------------
class StickyNoteRemoveCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item        The item to remove.
     *      @param [in] parent      The parent command.
     */
    StickyNoteRemoveCommand(StickyNote* noteItem, DesignDiagram* diagram, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~StickyNoteRemoveCommand();

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
    StickyNoteRemoveCommand(StickyNoteRemoveCommand const& rhs);
    StickyNoteRemoveCommand& operator=(StickyNoteRemoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    StickyNote* note_;

    //! The item's parent scene.
    QGraphicsScene* diagram_;

    //! Boolean flag for indicating if the items should be deleted in the destructor.
    bool del_;

};

//-----------------------------------------------------------------------------
#endif // STICKYNOTEREMOVECOMMAND_H