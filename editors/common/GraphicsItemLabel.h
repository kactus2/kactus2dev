//-----------------------------------------------------------------------------
// File: GraphicsItemLabel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2018
//
// Description:
// Text label for design items.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSITEMLABEL_H
#define GRAPHICSITEMLABEL_H

#include <QGraphicsProxyWidget>
#include <QLabel>

//-----------------------------------------------------------------------------
//! Text label for design items.
//-----------------------------------------------------------------------------
class GraphicsItemLabel : public QGraphicsProxyWidget
{
public:

    /*!
     *  Constructor.
     *
     *    @param [in] text    Text for the label.
     *    @param [in] parent  Parent item of the label.
     */
    GraphicsItemLabel(QString const& text, QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    ~GraphicsItemLabel() override = default;

    /*!
     *  Set a new text string for the label.
     *
     *    @param [in] newText     The selected text string.
     */
    void setText(QString const& newText);

    /*!
     *  Get the current text string of the label.
     *
     *    @return The current text string of the label.
     */
    QString getText() const;

    /*!
     *  Update geometry.
     */
    void updateLabelGeometry();

protected:

    /*!
     *  Handle hover move event.
     *
     *    @param [in] event     the event being processed.
     *      
     *      @remarks    The function is reimplemented in order to fix the bug.
     *                  The bug was lock on certain type of cursor after hovering 
     *                  the GraphicsItemLabel.
     *                  Reimplemented function only displays tooltip of the first 
                        parent with non-empty tooltip.
     */
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

private:

	//! No copying
    GraphicsItemLabel(const GraphicsItemLabel& other);
	
    //! No assignment
    GraphicsItemLabel& operator=(const GraphicsItemLabel& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Label containing the text string.
    QLabel* textLabel_ = new QLabel();
};

#endif // GRAPHICSITEMLABEL_H
