//-----------------------------------------------------------------------------
// File: DiagramColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCOLUMN_H
#define DIAGRAMCOLUMN_H

#include <common/ColumnTypes.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <models/ColumnDesc.h>

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>

class DiagramColumnLayout;
class DiagramInterconnection;
class DiagramConnectionEndPoint;
class Component;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! DiagramColumn class.
//-----------------------------------------------------------------------------
class DiagramColumn : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_COLUMN };

    /*!
     *  Constructor.
     *
     *      @param [in] desc    The column description.
     *      @param [in] layout  The parent column layout.
     */
    DiagramColumn(ColumnDesc const& desc, DiagramColumnLayout* layout);

    /*!
     *  Destructor.
     */
    virtual ~DiagramColumn();

    /*!
     *  Sets the name of the diagram column.
     *
     *      @param [in] name The name of the column.
     */
    void setName(QString const& name);

    /*!
     *  Sets the column description which determines the name and width of the column and the allowed items.
     *
     *      @param [in] desc The column description.
     */
    void setColumnDesc(ColumnDesc const& desc);

    /*!
     *  Sets the column width.
     *
     *      @param [in] width The new width to set.
     */
    void setWidth(unsigned int width);

    /*!
     *  Checks whether the given item is allowed to be placed to this column.
     *
     *      @param [in] item The item.
     *
     *      @return True if the item can be placed to the column; otherwise false.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

    /*!
     *  Adds an item to the diagram column.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the diagram column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Sets the y coordinate offset.
     *
     *      @param [in] y The y coordinate offset.
     */
    void setOffsetY(qreal y);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     *      @param [in] oldColumn  The old column where the item was located when the movement began.
     */
    void onMoveItem(QGraphicsItem* item, DiagramColumn* oldColumn);

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
     *  Returns the name of the diagram column.
     */
    QString const& getName() const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

    /*!
     *  Returns the list of allowed items. See ColumnItemType for possible values.
     */
    unsigned int getAllowedItems() const;

    /*!
     *  Returns the column description.
     */
    ColumnDesc const& getColumnDesc() const;

    /*!
     *  Returns true if the contents of the column conforms to the given allowed items configuration.
     */
    bool isAllowedItemsValid(unsigned int allowedItems) const;

    /*!
     *  Returns true if the column is empty (i.e. not containing any items).
     */
    bool isEmpty() const;

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

    /*!
     *  Returns the column layout.
     */
    DiagramColumnLayout& getLayout();

    int type() const { return Type; }

signals:
    //! Signals that the contents of the column have changed.
    void contentChanged();

protected:
    //! Called when the user presses the mouse on the column bar.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

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

private:
    // Disable copying.
    DiagramColumn(DiagramColumn const& rhs);
    DiagramColumn& operator=(DiagramColumn const& rhs);

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
     *  Returns true if the given item is allowed to reside in the column based on the allowed items.
     *
     *      @param [in] item          The item.
     *      @param [in] allowedItems  The allowed items flags.
     */
    bool isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const;

    /*!
     *  Begins the position updates for the connections that start from the given end point.
     *
     *      @param [in] endPoint The end point.
     */
    void beginUpdateConnPositions(DiagramConnectionEndPoint* endPoint);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        HEIGHT = 30,
        MIN_Y_PLACEMENT = 60,
        SPACING = 30,
        IO_SPACING = 18
    };

    //! The parent column layout.
    DiagramColumnLayout* layout_;

    //! The column description.
    ColumnDesc desc_;

    //! The column name label.
    QGraphicsTextItem* nameLabel_;

    //! The child graphics items ordered from top to bottom.
    QList<QGraphicsItem*> items_;

    //! The old position of the column before mouse move.
    QPointF oldPos_;

    //! The connections that need to be also stored for undo.
    QSet<DiagramInterconnection*> conns_;


    //! If true, the mouse hovers near the resize area.
    bool mouseNearResizeArea_;

    //! Old cursor for restoring purposes.
    QCursor oldCursor_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCOLUMN_H
