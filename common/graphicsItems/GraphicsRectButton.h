//-----------------------------------------------------------------------------
// File: GraphicsRectButton.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2011
//
// Description:
// Generic graphics rectangle button item.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSRECTBUTTON_H
#define GRAPHICSRECTBUTTON_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QGraphicsItem>

//-----------------------------------------------------------------------------
//! GraphicsRectButton class.
//-----------------------------------------------------------------------------
class GraphicsRectButton : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    GraphicsRectButton(QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsRectButton();

signals:
    //! Emitted when the user clicks the rectangle with the left mouse button.
    void clicked();

protected:
    //! Called when the user presses a mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    // Disable copying.
    GraphicsRectButton(GraphicsRectButton const& rhs);
    GraphicsRectButton& operator=(GraphicsRectButton const& rhs);
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSRECTBUTTON_H
