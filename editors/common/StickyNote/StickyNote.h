//-----------------------------------------------------------------------------
// File: StickyNote.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <editors/common/diagramgrid.h>
#include <editors/common/Association/Associable.h>

#include <QDomNode>
#include <QGraphicsSceneMouseEvent>
#include <QUndoCommand>

class VendorExtension;
class Kactus2Group;
class Kactus2Position;
class Kactus2Value;
class ColorFillTextItem;

//-----------------------------------------------------------------------------
//! DesignLabel class.
//-----------------------------------------------------------------------------
class StickyNote : public QObject, public QGraphicsItemGroup, public Associable
{
    Q_OBJECT

public:

    static const int TOP_OFFSET = 20;
    static const int DEFAULT_WIDTH = GridSize * 8 * 2;

    enum { Type = GFX_TYPE_DIAGRAM_STICKY_NOTE };

    /*!
     *  The constructor.
     *
     *    @param [in] parent      The parent item.
     */
    explicit StickyNote(QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~StickyNote() = default;

    //! Disable copying.
    StickyNote(StickyNote const& rhs) = delete;

    //! Disable assignment.
    StickyNote& operator=(StickyNote const& rhs) = delete;

    /*!
     *  Begins the editing of the note.
     */
    void beginEditing();

    /*!
     *  Gets the vendor extension visualized by the note.
     *
     *    @return The vendor extension visualized by the note.
     */
    QSharedPointer<VendorExtension> getVendorExtension() const;

    //! The item type identifier.
    virtual int type() const { return Type; }

    /*!
     *  Adds a new association to the item.
     *
     *    @param [in] association   The association to add.     
     */
    virtual void addAssociation(Association* association);

    /*!
     *  Removes an association from the item.
     *
     *    @param [in] association   The association to remove.     
     */
    virtual void removeAssociation(Association* association);    

    /*!
     *  Defines the connection point for associations in scene coordinates.
     *     
     *    @param [in] otherEnd   The position of the other end connection point.
     *
     *    @return The connection point of the item.
     */
    virtual QPointF connectionPoint(QPointF const& otherEnd = QPointF()) const;

    //! Gets the vendor extension for the associations of the note.
    QSharedPointer<Kactus2Group> getAssociationExtensions() const;

    /*!
     *  Set the vendor extension for the sticky note to use for storing the data. 
     *  Existing values for position, content, associations and timestamp will be read to the note .
     *
     *    @param [in] extension   The vendor extension to set.
     */
    void setVendorExtension(QSharedPointer<Kactus2Group> extension);

    /*!
     *  Sets the note text.
     *
     *    @param [in] text   The text to set.
     */
    void setText(QString const& text);

    /*!
     *  Sets the note timestamp.
     *
     *    @param [in] timestamp   The timestamp to set.
     */
    void setTimestamp(QString const& timestamp);

signals:
   
    //! Emitted when a new association should be created for the item.
    void beginAssociation(Associable* originator);


    /*!
     *  Emitted when the note has been modified.
     *
     *    @param [in] undoCommand   The undo command to undo/redo the modification.
     */
    void modified(QUndoCommand* undoCommand);

protected:

    //! Handler for mouse press events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse release event.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Checks if the item's position has changed as a result of a mouse drag.
     *
     *    @return True, if the position has changed, otherwise false.
     */
    bool positionChanged();

    /*!
     *  Handler for item state changes.
     *
     *    @param [in] change      The parameter of the item changing.
     *    @param [in] value       The new value of the parameter.
     *
     *    @return The set new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private slots:

    //! Called when the note text has changed.
    virtual void onTextEdited();

private:

     //! Initializes the item flags.     
    void setItemOptions();

    //! Creates the glued edge at the top of the note.
    void createGluedEdge();

    //! Creates the writing area of the note.
    void createWritableArea();

    //! Creates a button for initiating an association.
    void createAssociationButton();

    /*!
     *  Checks if clicking a given position hits the association button.
     *
     *    @param [in] clickPosition   The position of the click.
     *
     *    @return True, if the click hits the association button, otherwise false.
     */
    bool hitsAssociationButton(QPointF const& clickPosition) const;   
    
    /*!
     *  Gets the timestamp for current time and date as formatted string.
     *
     *    @return Current time and date.
     */
    QString getFormattedTimestamp();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The position of the item before move.
    QPointF oldPos_;

    //! The vendor extension represented by the note.
    QSharedPointer<Kactus2Group> extension_;

    //! The vendor extension containing the position of the note.
    QSharedPointer<Kactus2Position> positionExtension_;

    //! The vendor extension containing the text of the note.
    QSharedPointer<Kactus2Value> contentExtension_;

    //! The vendor extension containing the text of the note.
    QSharedPointer<Kactus2Group> associationExtensions_;

    //! The vendor extension containing the timestamp of the note.
    QSharedPointer<Kactus2Value> timestampExtension_;

    //! Editor item for the notes.
    ColorFillTextItem* textArea_;

    //! Label for displaying the edit timestamp.
    QGraphicsSimpleTextItem* timeLabel_;

    //! Item acting as a button for creating new associations.
    QGraphicsPixmapItem* associationButton_;
};

#endif // STICKYNOTE_H