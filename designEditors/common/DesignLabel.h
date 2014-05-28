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
    DesignLabel(QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~DesignLabel(void);

    /*!
     *  Draws the label.
     *
     *      @param [in] painter     The painter for drawing.
     *      @param [in] option      The style options for drawing.
     *      @param [in] widget      The widget being painted on.
     */
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget * widget );

};

