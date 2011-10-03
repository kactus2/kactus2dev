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

#include "ColorBox.h"

#include <QPalette>
#include <QMouseEvent>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ColorBox()
//-----------------------------------------------------------------------------
ColorBox::ColorBox(QSize const& size, QWidget* parent) : QFrame(parent), textLabel_(0)
{
    setLineWidth(1);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setAutoFillBackground(true);
    setFixedSize(size);

    color_ = palette().color(backgroundRole());

    textLabel_ = new QLabel(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(textLabel_, 1, Qt::AlignHCenter | Qt::AlignVCenter);
}

//-----------------------------------------------------------------------------
// Function: ~ColorBox()
//-----------------------------------------------------------------------------
ColorBox::~ColorBox()
{
}

//-----------------------------------------------------------------------------
// Function: setColor()
//-----------------------------------------------------------------------------
void ColorBox::setColor(QColor const& color)
{
    QPalette pal = palette();
    pal.setColor(backgroundRole(), color);
    setPalette(pal);

    color_ = color;
}

//-----------------------------------------------------------------------------
// Function: getColor()
//-----------------------------------------------------------------------------
QColor const& ColorBox::getColor() const
{
    return color_;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void ColorBox::mousePressEvent(QMouseEvent* event)
{
    QFrame::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

//-----------------------------------------------------------------------------
// Function: setText()
//-----------------------------------------------------------------------------
void ColorBox::setText(QString const& text)
{
    textLabel_->setText(text);
}

//-----------------------------------------------------------------------------
// Function: setTextFont()
//-----------------------------------------------------------------------------
void ColorBox::setTextFont(QFont const& font)
{
    textLabel_->setFont(font);
}

//-----------------------------------------------------------------------------
// Function: setTextColor()
//-----------------------------------------------------------------------------
void ColorBox::setTextColor(QColor const& color)
{
    QPalette pal = textLabel_->palette();
    pal.setColor(textLabel_->foregroundRole(), color);
    textLabel_->setPalette(pal);
}
