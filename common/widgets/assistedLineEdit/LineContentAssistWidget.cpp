//-----------------------------------------------------------------------------
// File: LineContentAssistWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.1.2011
//
// Description:
// Content assist widget.
//-----------------------------------------------------------------------------

#include "LineContentAssistWidget.h"

#include <algorithm>
#include <QScrollBar>
#include <QRegExp>
#include <QToolTip>
#include <QTextCursor>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: LineContentAssistWidget()
//-----------------------------------------------------------------------------
LineContentAssistWidget::LineContentAssistWidget(QLineEdit* parent,
                                         QSharedPointer<ILineContentMatcher> matcher) :
    QListWidget(parent), m_maxVisibleItems(DEFAULT_MAX_VISIBLE_ITEMS),
    m_lastAssistStartPos(-1), m_contentFound(false)
{
    Q_ASSERT(parent != 0);
    Q_ASSERT(matcher != 0);
    m_parent = parent;
    m_matcher = matcher;

    // Set widget settings.
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    setFrameShadow(QFrame::Raised);
    setFocusPolicy(Qt::NoFocus);
    setFont(QFont("Tahoma", 10));

    // We want the widget to look active even though it is inactive.
    QPalette pal = palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight, Qt::darkBlue);
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText, Qt::white);
    setPalette(pal);
}

//-----------------------------------------------------------------------------
// Function: ~LineContentAssistWidget()
//-----------------------------------------------------------------------------
LineContentAssistWidget::~LineContentAssistWidget()
{
}

//-----------------------------------------------------------------------------
// Function: handleKey()
//-----------------------------------------------------------------------------
bool LineContentAssistWidget::tryHandleKey(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Space && e->modifiers() == Qt::CTRL)
    {
        updateAssist(0);
        return true;
    }

    QString text = m_parent->text().left(m_parent->cursorPosition()) + e->text();

    // Check if we can commit.
    if (m_contentFound && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Tab ||
                           (canCommitWith(e) && !m_matcher->lookForwardMatch(text))))
    {
        // Commit the selection.
        commitSelection();

        // If the key is a character, let the editor handle it too.
        if (e->key() != Qt::Key_Return && e->key() != Qt::Key_Tab)
        {
            return false;
        }

        // Otherwise we're fully done with it.
        return true;
    }

    // Handle up and down arrows and escape, if the content is shown.
    if (isContentShown())
    {
        switch (e->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
            {
                if (isVisible())
                {
                    keyPressEvent(e);
                    return true;
                }
                break;
            }

        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Home:
        case Qt::Key_End:
            {
                cancel();
                return false;
            }

        case Qt::Key_A:
            {
                if (e->modifiers() == Qt::ControlModifier)
                {
                    cancel();
                    return false;
                }
                break;
            }

        case Qt::Key_Escape:
            {
                cancel();
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: updateContent()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::updateAssist(QKeyEvent* e)
{
    // Invoke the content assist for certain keys.
    if (e == 0 || (e->text().contains(QRegExp("^[a-z|A-z|0-9|_|ƒ|‰|÷|ˆ|≈|Â|(|)|,|&]$")) ||
        e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Return ||
        e->key() == Qt::Key_Down || e->key() == Qt::Key_Up ||
        (m_contentFound && e->text().contains(' '))))
    {
        updateMatches();
    }
    // Cancel assisting if needed.
    else if (e->modifiers() == Qt::NoModifier)
    {
        hideAssist();
    }
}

//-----------------------------------------------------------------------------
// Function: cancel()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::cancel()
{
    hideAssist();
}

//-----------------------------------------------------------------------------
// Function: setMaxVisibleItems()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::setMaxVisibleItems(int count)
{
    Q_ASSERT(count > 0);
    m_maxVisibleItems = count;
}

//-----------------------------------------------------------------------------
// Function: isContentShown()
//-----------------------------------------------------------------------------
bool LineContentAssistWidget::isContentShown() const
{
    return (isVisible());
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::mousePressEvent(QMouseEvent* event)
{
    QListWidget::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QListWidget::mouseDoubleClickEvent(event);

    // Commit the selection.
    commitSelection();
}

//-----------------------------------------------------------------------------
// Function: updateMatches()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::updateMatches()
{
    // Clear the content assist and fill it with new content.
    clear();

    QString toolTipText;
    int toolTipIndex = -1;
    QString text = m_parent->text().left(m_parent->cursorPosition());

    m_contentFound = m_matcher->fillWithContent(text, *this, m_lastAssistStartPos, toolTipText,
                                                toolTipIndex);

    // Check if content was found.
    if (m_contentFound)
    {
        // Fit the widget to the contents and move it accordingly.
        fitToContents();
        moveClose(m_lastAssistStartPos);

        setCurrentRow(0);
        show();
    }
    else
    {
        hide();
    }
}

//-----------------------------------------------------------------------------
// Function: moveClose()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::moveClose(int /*cursorPos*/)
{
    // Determine the correct upper-left corner position for the widget.
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();
    
    // By default, the desired position is below line edit.
    QPoint pos = m_parent->mapToGlobal(QPoint(0, 0)) + QPoint(0,  m_parent->height());
    
    // Restrict x coordinate by the screen width.
    pos.setX(std::min(std::max(0, pos.x()), screenWidth - width()));

    // Lift the widget above the line edit only if necessary to keep it fully in view.
    if (pos.y() + height() > screenHeight)
    {
        pos.setY(m_parent->mapToGlobal(m_parent->pos()).y() - height() - 10);
    }
    
    // Move the widget to the final position.
    move(pos);
}

//-----------------------------------------------------------------------------
// Function: canCommitWith()
//-----------------------------------------------------------------------------
bool LineContentAssistWidget::canCommitWith(QKeyEvent* e) const
{
    return (!e->text().contains(QRegExp("^[a-z|A-z|0-9|_|ƒ|‰|÷|ˆ|≈|Â]$")) &&
            e->key() != Qt::Key_Backspace && e->key() != Qt::Key_Escape &&
            e->key() != Qt::Key_Up && e->key() != Qt::Key_Down &&
            e->key() != Qt::Key_Left && e->key() != Qt::Key_Right &&
            e->key() != Qt::Key_Space && e->modifiers() != Qt::ControlModifier);
}

//-----------------------------------------------------------------------------
// Function: commitSelection()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::commitSelection()
{
    Q_ASSERT(m_lastAssistStartPos != -1);

    // Replace the text with the current content.
    m_parent->setSelection(m_lastAssistStartPos, m_parent->cursorPosition());
    m_parent->insert(currentItem()->text());

    hideAssist();
}

//-----------------------------------------------------------------------------
// Function: fitToContents()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::fitToContents()
{
    if (count() == 0)
    {
        return;
    }

    int visibleRowCount = std::min(count(), m_maxVisibleItems);
    int height = visibleRowCount * sizeHintForRow(0) + frameWidth() * 2;
    int width = sizeHintForColumn(0) + frameWidth() * 2 + verticalScrollBar()->sizeHint().width();
    
    setFixedSize(width, height);
}

//-----------------------------------------------------------------------------
// Function: hideAssist()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::hideAssist()
{
    hide();
    clear();
    m_contentFound = false;
}
