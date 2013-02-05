//-----------------------------------------------------------------------------
// File: AssistedLineEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.2.2010
//
// Description:
// Assisted line edit widget.
//-----------------------------------------------------------------------------

#include "AssistedLineEdit.h"

#include <cassert>
#include <algorithm>
#include <QFont>
#include <QRegExp>
#include <QApplication>
#include <QTextBlock>

#include "LineContentAssistWidget.h"

//-----------------------------------------------------------------------------
// Function: AssistedTextEdit()
//-----------------------------------------------------------------------------
AssistedLineEdit::AssistedLineEdit(QWidget* parentWnd, QWidget* parent) : QLineEdit(parent),
                                                                          mainWnd_(parentWnd),
                                                                          matcher_(0),
                                                                          contentAssist_()
{
    contentAssist_ = new LineContentAssistWidget(this, parentWnd);
    
    // Install this as an event filter so that we can track events from the main window.
    if (mainWnd_ != 0)
    {
        mainWnd_->installEventFilter(this);
        installEventFilter(this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~AssistedLineEdit()
//-----------------------------------------------------------------------------
AssistedLineEdit::~AssistedLineEdit()
{
    // Remove the event filter.
    if (mainWnd_ != 0)
    {
        removeEventFilter(this);
        mainWnd_->removeEventFilter(this);
    }

    delete contentAssist_;
}

//-----------------------------------------------------------------------------
// Function: eventFilter()
//-----------------------------------------------------------------------------
bool AssistedLineEdit::eventFilter(QObject* /*obj*/, QEvent* e)
{
    // Cancel the assist if the main window is moved or resized.
    if (contentAssist_->isContentShown())
    {
        if (e->type() == QEvent::Move || e->type() == QEvent::Resize)
        {
            contentAssist_->cancel();
        }
        else if (e->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);

            if (keyEvent->key() == Qt::Key_Tab)
            {
                keyPressEvent(keyEvent);
            }
        }
    }

    return false;
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
        
        // Update the content assist.
        contentAssist_->updateAssist(e);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::mousePressEvent(QMouseEvent* e)
{
    QLineEdit::mousePressEvent(e);

    // Cancel the content assist if content is shown.
    if (contentAssist_->isContentShown())
    {
        //m_contentAssist->cancel();
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
        // Update the assist.
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
// Function: AssistedLineEdit::getMatcher()
//-----------------------------------------------------------------------------
ILineContentMatcher* AssistedLineEdit::getContentMatcher()
{
    return matcher_;
}

//-----------------------------------------------------------------------------
// Function: AssistedLineEdit::setContentMatcher()
//-----------------------------------------------------------------------------
void AssistedLineEdit::setContentMatcher(ILineContentMatcher* matcher)
{
    matcher_ = matcher;
    contentAssist_->setContentMatcher(matcher);
}
