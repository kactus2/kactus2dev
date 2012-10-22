//-----------------------------------------------------------------------------
// File: AddressSectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.9.2012
//
// Description:
// Declares the address block section class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSECTIONITEM_H
#define ADDRESSSECTIONITEM_H

#include "MemoryBaseItem.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class AddressBlock;
class MemoryItem;
class AddressSubsection;

//-----------------------------------------------------------------------------
//! Address block section class.
//-----------------------------------------------------------------------------
class AddressSectionItem : public MemoryBaseItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_ADDRESS_SECTION_ITEM };

    /*!
     *  Constructor.
     *  
     *      @param [in] name            The name of the section.
     *      @param [in] startAddress    The start address of the section.
     *      @param [in] range           The address range.
     *      @param [in] parent          The parent.
     */
    AddressSectionItem(QString const& name, unsigned int startAddress,
                       unsigned int range, QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    ~AddressSectionItem();

    /*!
     *  Draws address guide lines.
     *
     *      @param [in] painter  The painter for drawing.
     *      @param [in] rect     The visible rectangle area where to draw the lines.
     */
    virtual void drawGuides(QPainter* painter, QRectF const& rect) const;

    int type() const { return Type; }

protected:
    //! Called when the mouse hover enters the column header.
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover moves inside the column header.
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the mouse hover leaves the column header.
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    //! Called when the user presses the mouse over the column.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user double-clicks the section.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //! Draws the memory item.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    //! Called when the start address of a subsection has been edited.
    void onSubsectionStartAddressEdited(AddressSubsection* subsection);

private:
    // Disable copying.
    AddressSectionItem(AddressSectionItem const& rhs);
    AddressSectionItem& operator=(AddressSectionItem const& rhs);

    /*!
     *  Updates the mouse cursor based on the hover position.
     *
     *      @param [in] event The hover event information.
     */
    void updateCursor(QGraphicsSceneHoverEvent* event);

    /*!
     *  Sets the height of the section item.
     *  
     *      @param [in] height  The height to set.
     */
    void setHeight(int height);

    /*!
     *  Retrieves subsection at the given position.
     *  
     *      @param [in] pos The position.
     *  
     *      @return The subsection.
     */
    int getSubsectionAt(QPointF const& pos) const;

    /*!
     *  Fixes subsection heights so that no subsection is smaller than the minimum subsection height.
     */
    void fixSubsectionHeights();

    enum
    {
        WIDTH = 230,
        ADDR_COLUMN_WIDTH = 80,
        DEFAULT_SECTION_HEIGHT = 120,
        MIN_SUBSECTION_HEIGHT = 40,
        SPACING = 10,
    };

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! The name of the section.
    QString name_;

    //! The start address.
    unsigned int startAddress_;

    //! the address range.
    unsigned int range_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! If true, the mouse hovers near the resize area.
    bool mouseNearResizeArea_;

    //! The resize index.
    int resizeIndex_;

    //! The subsections. The list contains always at least one item.
    QList< QSharedPointer<AddressSubsection> > subsections_;
};

#endif // ADDRESSSECTIONITEM_H
