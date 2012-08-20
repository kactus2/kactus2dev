//-----------------------------------------------------------------------------
// File: LineEditEx.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.8.2012
//
// Description:
// Line edit with extended functionality.
//-----------------------------------------------------------------------------

#include "LineEditEx.h"

#include <QStyle>
#include <QPalette>

//-----------------------------------------------------------------------------
// Function: LineEditEx::LineEditEx()
//-----------------------------------------------------------------------------
LineEditEx::LineEditEx(QWidget* parent /*= 0*/)
    : AssistedLineEdit(parent, parent), // TODO: Main window!!
      disallowedInputs_(),
      messageTemplate_(tr("'%1' is already in use")),
      messageIcon_(),
      iconWidget_(0)
{
    connect(this, SIGNAL(textChanged(QString const&)),
            this, SLOT(validateText(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::~LineEditEx()
//-----------------------------------------------------------------------------
LineEditEx::~LineEditEx()
{

}

//-----------------------------------------------------------------------------
// Function: LineEditEx::setDisallowedInputs()
//-----------------------------------------------------------------------------
void LineEditEx::setDisallowedInputs(QStringList const& inputs)
{
    disallowedInputs_ = inputs;
    validateText(text());
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::setMessageTemplate()
//-----------------------------------------------------------------------------
void LineEditEx::setMessageTemplate(QString const& messageTemplate)
{
    messageTemplate_ = messageTemplate;
    validateText(text());
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::setMessageIcon()
//-----------------------------------------------------------------------------
void LineEditEx::setMessageIcon(QPixmap const& icon)
{
    if (iconWidget_ == 0)
    {
        iconWidget_ = new QLabel(this);
        iconWidget_->setCursor(Qt::ArrowCursor);
        iconWidget_->setStyleSheet("QWidget { border: none; padding: 0px; }");
    }

    iconWidget_->setPixmap(icon);
    iconWidget_->setFixedSize(icon.size());

    updateIconPos();
    validateText(text());
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::updateIconPos()
//-----------------------------------------------------------------------------
void LineEditEx::updateIconPos()
{
    if (iconWidget_ != 0)
    {
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

        iconWidget_->move(rect().right() - frameWidth - iconWidget_->size().width(),
                          rect().bottom() - 1 - iconWidget_->size().height());
    }
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::resizeEvent()
//-----------------------------------------------------------------------------
void LineEditEx::resizeEvent(QResizeEvent*)
{
    updateIconPos();
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::validateText()
//-----------------------------------------------------------------------------
void LineEditEx::validateText(QString const& text)
{
    if (iconWidget_ != 0)
    {
        bool invalid = disallowedInputs_.contains(text);
        valid_ = !invalid;

        iconWidget_->setVisible(invalid);
        iconWidget_->setToolTip(messageTemplate_.arg(text));

        if (invalid)
        {
            QPalette p = palette();
            p.setColor(QPalette::Normal, QPalette::Base, QColor(255, 192, 192));
            setPalette(p);
        }
        else
        {
            QPalette p = palette();
            p.setColor(QPalette::Normal, QPalette::Base, Qt::white);
            setPalette(p);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LineEditEx::isInputValid()
//-----------------------------------------------------------------------------
bool LineEditEx::isInputValid() const
{
    return valid_;
}
