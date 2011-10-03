//-----------------------------------------------------------------------------
// File: ColorBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.6.2011
//
// Description:
// Color box widget which click support.
//-----------------------------------------------------------------------------

#ifndef COLORBOX_H
#define COLORBOX_H

#include <QColor>
#include <QFrame>
#include <QLabel>

//-----------------------------------------------------------------------------
//! ColorBox class.
//-----------------------------------------------------------------------------
class ColorBox : public QFrame
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    ColorBox(QSize const& size, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColorBox();

    /*!
     *  Sets the color for the color box.
     *
     *      @param [in] color The color to set.
     */
    void setColor(QColor const& color);

    /*!
     *  Sets the text inside the box.
     */
    void setText(QString const& text);

    /*!
     *  Sets the text font.
     *
     *      @param [in] font The font to set.
     */
    void setTextFont(QFont const& font);

    /*!
     *  Sets the text color.
     *
     *      @param [in] color The color to set.
     */
    void setTextColor(QColor const& color);

    /*!
     *  Returns the color of the color box.
     */
    QColor const& getColor() const;

signals:
    //! Signaled when the color box is clicked.
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    // Disable copying.
    ColorBox(ColorBox const& rhs);
    ColorBox& operator=(ColorBox const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The current color.
    QColor color_;

    //! Text label.
    QLabel* textLabel_;
};

//-----------------------------------------------------------------------------

#endif // COLORBOX_H
