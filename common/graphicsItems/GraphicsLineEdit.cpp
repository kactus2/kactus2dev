//-----------------------------------------------------------------------------
// File: GraphicsLineEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 25.9.2012
//
// Description:
// Implements the graphics line edit class.
//-----------------------------------------------------------------------------

#include "GraphicsLineEdit.h"

#include <QEvent>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::GraphicsLineEdit()
//-----------------------------------------------------------------------------
GraphicsLineEdit::GraphicsLineEdit(QGraphicsItem* parent)
    : QGraphicsTextItem(parent),
      readOnly_(false),
      oldText_(),
      validator_(0)
{
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(onContentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::~GraphicsLineEdit()
//-----------------------------------------------------------------------------
GraphicsLineEdit::~GraphicsLineEdit()
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::setReadOnly()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;

    if (readOnly_)
    {
        setTextInteractionFlags(Qt::NoTextInteraction);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::sceneEvent()
//-----------------------------------------------------------------------------
bool GraphicsLineEdit::sceneEvent(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
            {
                cancelEdit();
                return true;
                break;
            }

        case Qt::Key_Tab:
        case Qt::Key_Return:
            {
                clearFocus();
                return true;
                break;
            }
        }
    }
    else if (event->type() == QEvent::GraphicsSceneMousePress)
    {
        if (textInteractionFlags() == Qt::NoTextInteraction)
        {
            return true;
        }
    }
    else if (event->type() == QEvent::GraphicsSceneMouseDoubleClick)
    {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton)
        {
            // Check if we're not yet editing.
            if (!readOnly_ && textInteractionFlags() == Qt::NoTextInteraction)
            {
                beginEdit();
            }
        }

        return true;
    }
    else if (event->type() == QEvent::FocusOut)
    {
        clearFocus();
        commitEdit();
        return true;
    }

    return QGraphicsTextItem::sceneEvent(event);
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::beginEdit()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::beginEdit()
{
    oldText_ = toHtml();

    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::cancelEdit()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::cancelEdit()
{
    setHtml(oldText_);
    setTextInteractionFlags(Qt::NoTextInteraction);
    clearFocus();
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::commitEdit()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::commitEdit()
{
    // Validate the text.
    QString text = toPlainText();
    int pos = textCursor().position();

    if (validator_ != 0 && validator_->validate(text, pos) != QValidator::Acceptable)
    {
        // Try to fix the text.
        validator_->fixup(text);

        if (validator_->validate(text, pos) != QValidator::Acceptable)
        {
            emit invalidInputEntered();
            setFocus();
            return;
        }

        setPlainText(text);
    }

    setTextInteractionFlags(Qt::NoTextInteraction);
    clearFocus();

    emit textEdited();
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::setValidator()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::setValidator(QValidator const* validator)
{
    validator_ = validator;
}

//-----------------------------------------------------------------------------
// Function: GraphicsLineEdit::onContentChanged()
//-----------------------------------------------------------------------------
void GraphicsLineEdit::onContentChanged()
{
    disconnect(document(), SIGNAL(contentsChanged()), this, SLOT(onContentChanged()));

    QString text = toPlainText();
    int pos = textCursor().position();

    if (validator_ != 0 && validator_->validate(text, pos) == QValidator::Invalid)
    {
        validator_->fixup(text);
        setPlainText(text);

        QTextCursor cursor = textCursor();

        if (pos < text.length())
        {
            cursor.setPosition(pos);
        }
        else
        {
            cursor.setPosition(text.length());
        }

        setTextCursor(cursor);
    }

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(onContentChanged()), Qt::UniqueConnection);
}
