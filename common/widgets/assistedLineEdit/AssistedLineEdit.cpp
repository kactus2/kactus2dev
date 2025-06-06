//-----------------------------------------------------------------------------
// File: AssistedLineEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 8.2.2010
//
// Description:
// Assisted line edit widget.
//-----------------------------------------------------------------------------

#include "AssistedLineEdit.h"

#include <cassert>
#include <algorithm>
#include <QFont>
#include <QApplication>
#include <QTextBlock>

#include "LineContentAssistWidget.h"

//-----------------------------------------------------------------------------
// Function: AssistedTextEdit()
//-----------------------------------------------------------------------------
AssistedLineEdit::AssistedLineEdit(QWidget* parentWnd, QWidget* parent) : 
QLineEdit(parent),
contentAssist_(new LineContentAssistWidget(this, parentWnd)) // NOTE: parent window set as parent so that 
                                                             // content assist visibility isn't dependent 
                                                             // on "real" parent widget size!
{
    contentAssist_->setFocusProxy(this);
}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::keyPressEvent(QKeyEvent* e)
{
    // First try to handle the key fully by the content assist.
    if (!contentAssist_->tryHandleKey(e))
    {
        // Otherwise process the key event as usual.
        QLineEdit::keyPressEvent(e);
        
        contentAssist_->updateAssist(e);
    }
}

//-----------------------------------------------------------------------------
// Function: focusOutEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::focusInEvent(QFocusEvent* e)
{
    QLineEdit::focusInEvent(e);

    if (e->reason() != Qt::ActiveWindowFocusReason)
    {
        contentAssist_->setWidthForAssist(width());
        contentAssist_->updateAssist(0);
    }
}

//-----------------------------------------------------------------------------
// Function: focusOutEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);

    // Cancel the content assist if content is shown.
    if (contentAssist_->isContentShown())
    {
        contentAssist_->cancel();
    }
}

//-----------------------------------------------------------------------------
// Function: AssistedLineEdit::setContentMatcher()
//-----------------------------------------------------------------------------
void AssistedLineEdit::setContentMatcher(ILineContentMatcher* matcher)
{
    matcher_ = matcher;
    contentAssist_->setContentMatcher(matcher);
}
