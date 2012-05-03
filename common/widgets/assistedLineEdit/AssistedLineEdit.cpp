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
AssistedLineEdit::AssistedLineEdit(QSharedPointer<ILineContentMatcher> contentMatcher,
                                   QWidget* mainWnd, QWidget* parent) : QLineEdit(parent),
                                                                        m_mainWnd(mainWnd),
                                                                        m_contentAssist()
{
    Q_ASSERT(contentMatcher != 0);

    m_contentAssist = new LineContentAssistWidget(this, contentMatcher);

    // Install this as an event filter so that we can track events from the main window.
    if (m_mainWnd != 0)
    {
        m_mainWnd->installEventFilter(this);
        installEventFilter(this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~AssistedLineEdit()
//-----------------------------------------------------------------------------
AssistedLineEdit::~AssistedLineEdit()
{
    // Remove the event filter.
    if (m_mainWnd != 0)
    {
        removeEventFilter(this);
        m_mainWnd->removeEventFilter(this);
    }
}

//-----------------------------------------------------------------------------
// Function: eventFilter()
//-----------------------------------------------------------------------------
bool AssistedLineEdit::eventFilter(QObject* /*obj*/, QEvent* e)
{
    // Cancel the assist if the main window is moved or resized.
    if (m_contentAssist->isContentShown())
    {
        if (e->type() == QEvent::Move || e->type() == QEvent::Resize)
        {
            m_contentAssist->cancel();
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
    if (!m_contentAssist->tryHandleKey(e))
    {
        // Otherwise process the key event as usual.
        QLineEdit::keyPressEvent(e);
        
        // Update the content assist.
        m_contentAssist->updateAssist(e);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::mousePressEvent(QMouseEvent* e)
{
    QLineEdit::mousePressEvent(e);

    // Cancel the content assist if content is shown.
    if (m_contentAssist->isContentShown())
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
        m_contentAssist->updateAssist(0);
    }
}

//-----------------------------------------------------------------------------
// Function: focusOutEvent()
//-----------------------------------------------------------------------------
void AssistedLineEdit::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);

    // Cancel the content assist if content is shown.
    if (m_contentAssist->isContentShown())
    {
        m_contentAssist->cancel();
    }
}
