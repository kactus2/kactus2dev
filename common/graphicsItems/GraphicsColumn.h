//-----------------------------------------------------------------------------
// File: GraphicsColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Graphics column class for managing a collection of graphics items in one
// column.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCOLUMN_H
#define GRAPHICSCOLUMN_H

#include "IGraphicsItemStack.h"
#include "../layouts/IVGraphicsLayout.h"

#include <common/ColumnTypes.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>
#include <QUndoCommand>

class GraphicsColumnLayout;

//-----------------------------------------------------------------------------
//! GraphicsColumn class.
//-----------------------------------------------------------------------------
class GraphicsColumn : public QObject, public QGraphicsRectItem, public IGraphicsItemStack
{
    Q_OBJECT

public:
    enum
    {
        HEIGHT = 30
    };

    /*!
     *  Constructor.
     *
     *      @param [in] desc    The column description.
     *      @param [in] layout  The parent column layout.
     */
    GraphicsColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout);

    /*!
     *  Destructor.
     */
    virtual ~GraphicsColumn();

    /*!
     *  Sets the name of the system column.
     *
     *      @param [in] name The name of the column.
     */
    void setName(QString const& name);

    /*!
     *  Sets the column description.
     */
    void setColumnDesc(ColumnDesc const& desc);

    /*!
     *  Sets the column width.
     *
     *      @param [in] width The new width to set.
     */
    void setWidth(unsigned int width);

    /*!
     *  Sets the y coordinate offset.
     *
     *      @param [in] y The y coordinate offset.
     */
    void setOffsetY(qreal y);

    /*!
     *  Returns the name of the system column.
     */
    QString const& getName() const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

    /*!
     *  Returns the column description.
     */
    ColumnDesc const& getColumnDesc() const;

    /*!
     *  Returns the parent layout.
     */
    GraphicsColumnLayout& getLayout();

    /*!
     *  Returns the items that are added to the column.
     */
    QList<QGraphicsItem*> const& getItems() const;

    /*!
     *  Returns true if the contents of the column conforms to the given allowed items configuration.
     */
    bool isAllowedItemsValid(unsigned int allowedItems) const;

    /*!
     *  Returns true if the column is empty (i.e. not containing any items).
     */
    bool isEmpty() const;

    //-----------------------------------------------------------------------------
    // IGraphicsItemStack implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Adds an item to the system column.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     */
    void onMoveItem(QGraphicsItem* item);
    
    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    void onReleaseItem(QGraphicsItem* item);

    /*!
     *  Updates the item positions so that there are no violations of the stacking rule.
     */
    void updateItemPositions();

    /*!
     *  Maps the given local position to scene coordinates.
     */
    QPointF mapStackToScene(QPointF const& pos) const;

    /*!
     *  Maps the given scene position to local coordinates.
     */
    QPointF mapStackFromScene(QPointF const& pos) const;

    /*!
     *  Returns true if the stack is allowed to contain the given item.
     *
     *      @param [in] item The item to test for.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

protected:
    //! Called when the user presses the mouse over the column.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the mouse hover enters the column header.
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover moves inside the column header.
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover leaves the column header.
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Returns true if the given item is allowed to reside in the column based on the allowed items.
     *
     *      @param [in] item          The item.
     *      @param [in] allowedItems  The allowed items flags.
     */
    virtual bool isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const;

    /*!
     *  Prepares for column move.
     */
    virtual void prepareColumnMove();

    /*!
     *  Creates an undo command for the column move.
     *
     *      @return The created undo command.
     */
    virtual QSharedPointer<QUndoCommand> createMoveUndoCommand();

protected:
    /*!
     *  Changes the item layout.
     *
     *      @param [in] itemLayout The item layout to set.
     */
    void setItemLayout(QSharedPointer< IVGraphicsLayout<QGraphicsItem> > itemLayout);

private:
    // Disable copying.
    GraphicsColumn(GraphicsColumn const& rhs);
    GraphicsColumn& operator=(GraphicsColumn const& rhs);

    /*!
     *  Updates the mouse cursor based on the hover position.
     *
     *      @param [in] event The hover event information.
     */
    void updateCursor(QGraphicsSceneHoverEvent* event);

    /*!
     *  Updates the name label.
     */
    void updateNameLabel();

    /*!
     *  Switches the given item to another column.
     *
     *      @param [in] item    The item to move.
     *      @param [in] column  The destination column.
     */
    void switchColumn(QGraphicsItem* item, GraphicsColumn* column);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        MIN_Y_PLACEMENT = 60,
        SPACING = 30,
        IO_SPACING = 18,
    };

    //! The parent column layout.
    GraphicsColumnLayout* layout_;

    //! The column description.
    ColumnDesc desc_;

    //! The column name label.
    QGraphicsTextItem* nameLabel_;

    //! The layout used for contained items.
    QSharedPointer< IVGraphicsLayout<QGraphicsItem> > itemLayout_;

    //! The node items ordered from top to bottom.
    QList<QGraphicsItem*> items_;

    //! The old position of the column before mouse move.
    QPointF oldPos_;

    //! If true, the mouse hovers near the resize area.
    bool mouseNearResizeArea_;

    //! The old column width before resize.
    unsigned int oldWidth_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCOLUMN_H
