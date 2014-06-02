//-----------------------------------------------------------------------------
// File: ColorFillTextItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Graphic item for text editing with color filled background.
//-----------------------------------------------------------------------------

#ifndef COLORFILLTEXTITEM_H
#define COLORFILLTEXTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

//-----------------------------------------------------------------------------
//! ColorFillTextItem class.
//-----------------------------------------------------------------------------
class ColorFillTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent item.
     */
    explicit ColorFillTextItem(QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~ColorFillTextItem();

    /*!
     *  Sets the background fill color.
     *
     *      @param [in] color   The background color to set.
     *
     *      @return 
     */
    void setFill(QColor const& color);

    //! Draws the item.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

private:

    //! Disable copying.
    ColorFillTextItem(ColorFillTextItem const& rhs);

    //! Disable assignment.
    ColorFillTextItem& operator=(ColorFillTextItem const& rhs);

    //! The background fill color.
    QColor fillColor_;
};

#endif // COLORFILLTEXTITEM_H