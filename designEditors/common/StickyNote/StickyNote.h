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

class Kactus2Group;
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
    explicit StickyNote(QSharedPointer<Kactus2Group> extension, QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~StickyNote();

    /*!
     *  Begins the editing of the note.
     */
    void beginEditing();

    /*!
     *  Handler for item state changes.
     *
     *      @param [in] change      The parameter of the item changing.
     *      @param [in] value       The new value of the parameter.
     *
     *      @return The set new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    /*!
     *  Gets the vendor extension visualized by the note.
     *
     *      @return The vendor extension visualized by the note.
     */
    QSharedPointer<VendorExtension> getVendorExtension() const;

    //! The item type identifier.
    virtual int type() const { return Type; }

protected:

    //! Handler for mouse press events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse move events.
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse release event.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private slots:

    //! Called when the notes text has changed.
    virtual void onTextEdited();

private:

    //! Disable copying.
    StickyNote(StickyNote const& rhs);

    //! Disable assignment.
    StickyNote& operator=(StickyNote const& rhs);

     //!Initializes the item flags.     
    void setItemOptions();

    //! Creates the glued edge at the top of the note.
    void createGluedEdge();

    //! Creates the writing area of the note.
    void createWritableArea();

    //! Initializes the vendor extensions for the note.
    void initializeExtensions();

    //! Initialized the vendor extension for tracking the note position.
    void initializePosition();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The position of the item before move.
    QPointF oldPos_;

    //! The vendor extension represented by the note.
    QSharedPointer<Kactus2Group> extension_;

    //! The vendor extension containing the position of the note.
    QSharedPointer<Kactus2Position> position_;

    //! Editor item for the notes.
    ColorFillTextItem* textArea_;
};

#endif // STICKYNOTE_H