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

#include <designEditors/common/diagramgrid.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

class ColorFillTextItem;

//-----------------------------------------------------------------------------
//! DesignLabel class.
//-----------------------------------------------------------------------------
class StickyNote : public QObject, public QGraphicsItemGroup 
{
    Q_OBJECT
public:

    static const int TOP_OFFSET = 15;

    static const int DEFAULT_WIDTH = GridSize * 8 * 2;

    enum { Type = GFX_TYPE_DIAGRAM_STICKY_NOTE };

    /*!
     *  The constructor.
     *
     *      @param [in] extension   The vendor extension representing the note.
     *      @param [in] parent      The parent item.
     */
    explicit StickyNote(QSharedPointer<VendorExtension> extension, QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~StickyNote();

    /*!
     *  Begins the editing of the note.
     */
    void beginEditing();

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    QSharedPointer<VendorExtension> getVendorExtension() const;

    //! The item type identifier.
    virtual int type() const { return Type; }

protected:

    //! Handler for mouse press events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse release event.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private slots:

    //! Called when the notes text has changed.
    virtual void onTextEdited();

private:

    //! Disable copying.
    StickyNote(StickyNote const& rhs);

    void setItemOptions();

    void createWritableArea();

    void createGluedEdge();


    //! Disable assignment.
    StickyNote& operator=(StickyNote const& rhs);

    //! The position of the item before move.
    QPointF oldPos_;

    QSharedPointer<Kactus2Position> position_;

    //! Editor item for the notes.
    ColorFillTextItem* textArea_;
};

#endif // STICKYNOTE_H