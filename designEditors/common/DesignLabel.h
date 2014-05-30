//-----------------------------------------------------------------------------
// File: DesignLabel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef DESIGNLABEL_H
#define DESIGNLABEL_H

#include <QGraphicsTextItem>

//-----------------------------------------------------------------------------
//! DesignLabel class.
//-----------------------------------------------------------------------------
class DesignLabel : public QGraphicsTextItem
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent item.
     */
    explicit DesignLabel(QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~DesignLabel();

    /*!
     *  Draws the label.
     *
     *      @param [in] painter     The painter for drawing.
     *      @param [in] option      The style options for drawing.
     *      @param [in] widget      The widget being painted on.
     */
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

protected:

    //! Handler for mouse press events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Handler for mouse release event.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:

    //! Position of the label before move.
    QPointF oldPos_;

};

#endif // HWDESIGNLABEL_H