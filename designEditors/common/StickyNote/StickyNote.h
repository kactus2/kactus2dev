//-----------------------------------------------------------------------------
// File: StickyNote.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Sticky note for designs.
//-----------------------------------------------------------------------------

#ifndef STICKYNOTE_H
#define STICKYNOTE_H

#include <QGraphicsItemGroup>

#include <common/graphicsItems/GraphicsItemTypes.h>

class ColorFillTextItem;

//-----------------------------------------------------------------------------
//! DesignLabel class.
//-----------------------------------------------------------------------------
class StickyNote : public QObject, public QGraphicsItemGroup 
{
    Q_OBJECT
public:

    enum { Type = GFX_TYPE_DIAGRAM_STICKY_NOTE };

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent item.
     */
    explicit StickyNote(QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~StickyNote();

    /*!
     *  Begins the editing of the note.
     */
    void beginEditing();

    //! The item type identifier.
    virtual int type() const { return Type; }

protected:

    //! Handler for mouse press events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse release event.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:

    //! Disable copying.
    StickyNote(StickyNote const& rhs);

    //! Disable assignment.
    StickyNote& operator=(StickyNote const& rhs);

    //! Position of the label before move.
    QPointF oldPos_;

    //! Editor item for the notes.
    ColorFillTextItem* textArea_;
};

#endif // HWSTICKYNOTE_H