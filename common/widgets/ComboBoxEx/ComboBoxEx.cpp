//-----------------------------------------------------------------------------
// File: ComboBoxEx.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.8.2012
//
// Description:
// Combo box with extended functionality.
//-----------------------------------------------------------------------------

#include "ComboBoxEx.h"

#include <QStyle>
#include <QPalette>

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::ComboBoxEx()
//-----------------------------------------------------------------------------
ComboBoxEx::ComboBoxEx(QWidget* parent /*= 0*/)
    : QComboBox(parent),
      disallowedInputs_(),
      messageTemplate_(tr("'%1' is already in use")),
      messageIcon_(),
      iconWidget_(0)
{
    connect(this, SIGNAL(editTextChanged(QString const&)),
            this, SLOT(validateText(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::~ComboBoxEx()
//-----------------------------------------------------------------------------
ComboBoxEx::~ComboBoxEx()
{

}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::setDisallowedInputs()
//-----------------------------------------------------------------------------
void ComboBoxEx::setDisallowedInputs(QStringList const& inputs)
{
    disallowedInputs_ = inputs;
    validateText(currentText());
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::setMessageTemplate()
//-----------------------------------------------------------------------------
void ComboBoxEx::setMessageTemplate(QString const& messageTemplate)
{
    messageTemplate_ = messageTemplate;
    validateText(currentText());
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::setMessageIcon()
//-----------------------------------------------------------------------------
void ComboBoxEx::setMessageIcon(QPixmap const& icon)
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
    validateText(currentText());
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::updateIconPos()
//-----------------------------------------------------------------------------
void ComboBoxEx::updateIconPos()
{
    if (iconWidget_ != 0)
    {
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

        iconWidget_->move(rect().right() - frameWidth - iconWidget_->size().width(),
            rect().bottom() - 1 - iconWidget_->size().height());
    }
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::resizeEvent()
//-----------------------------------------------------------------------------
void ComboBoxEx::resizeEvent(QResizeEvent* event)
{
    QComboBox::resizeEvent(event);
    updateIconPos();
}

//-----------------------------------------------------------------------------
// Function: ComboBoxEx::validateText()
//-----------------------------------------------------------------------------
void ComboBoxEx::validateText(QString const& text)
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
// Function: ComboBoxEx::isInputValid()
//-----------------------------------------------------------------------------
bool ComboBoxEx::isInputValid() const
{
    return valid_;
}
