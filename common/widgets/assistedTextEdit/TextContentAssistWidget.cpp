//-----------------------------------------------------------------------------
// File: TextContentAssistWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.1.2011
//
// Description:
// Content assist widget.
//-----------------------------------------------------------------------------

#include "TextContentAssistWidget.h"

#include <algorithm>
#include <QScrollBar>
#include <QRegExp>
#include <QToolTip>
#include <QTextCursor>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: TextContentAssistWidget()
//-----------------------------------------------------------------------------
TextContentAssistWidget::TextContentAssistWidget(QPlainTextEdit* parent,
                                                 QSharedPointer<ITextContentMatcher> matcher) :
    QListWidget(parent), maxVisibleItems_(DEFAULT_MAX_VISIBLE_ITEMS),
    lastAssistStartPos_(-1), contentFound_(false)
{
    Q_ASSERT(parent != 0);
    Q_ASSERT(matcher != 0);
    parent_ = parent;
    matcher_ = matcher;

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
// Function: ~TextContentAssistWidget()
//-----------------------------------------------------------------------------
TextContentAssistWidget::~TextContentAssistWidget()
{
}

//-----------------------------------------------------------------------------
// Function: handleKey()
//-----------------------------------------------------------------------------
bool TextContentAssistWidget::tryHandleKey(QKeyEvent* e)
{
    QString text = parent_->toPlainText().left(parent_->textCursor().position()) + e->text();

    // Check if we can commit.
    if (contentFound_ && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Tab ||
                           (canCommitWith(e) && !matcher_->lookForwardMatch(text))))
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
void TextContentAssistWidget::updateAssist(QKeyEvent* e)
{
    // Invoke the content assist for certain keys.
    if (e->text().contains(QRegExp("^[a-z|A-z|0-9|_|;|ƒ|‰|÷|ˆ|≈|Â|(|)|,|.|&]$")) ||
        e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Return ||
        (contentFound_ && (e->key() == Qt::Key_Down || e->key() == Qt::Key_Up || e->text().contains(' '))))
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
void TextContentAssistWidget::cancel()
{
    toolTipHintWidget_.hide();
    hideAssist();
}

//-----------------------------------------------------------------------------
// Function: setMaxVisibleItems()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::setMaxVisibleItems(int count)
{
    Q_ASSERT(count > 0);
    maxVisibleItems_ = count;
}

//-----------------------------------------------------------------------------
// Function: isContentShown()
//-----------------------------------------------------------------------------
bool TextContentAssistWidget::isContentShown() const
{
    return (isVisible() || toolTipHintWidget_.isVisible());
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::mousePressEvent(QMouseEvent* event)
{
    QListWidget::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QListWidget::mouseDoubleClickEvent(event);

    // Commit the selection.
    commitSelection();
}

//-----------------------------------------------------------------------------
// Function: updateMatches()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::updateMatches()
{
    // Clear the content assist and fill it with new content.
    clear();

    QString toolTipText;
    int toolTipIndex = -1;
    QString text = parent_->toPlainText().left(parent_->textCursor().position());

    contentFound_ = matcher_->fillWithContent(text, *this, lastAssistStartPos_, toolTipText,
                                              toolTipIndex);

    // Show the tool tip if there was some content for it.
    if (toolTipIndex > 0)
    {
        showToolTipHint(toolTipText, toolTipIndex);
    }
    else
    {
        toolTipHintWidget_.hide();
    }
    
    // Check if content was found.
    if (contentFound_)
    {
        // Fit the widget to the contents and move it accordingly.
        fitToContents();
        moveCloseToCursor(lastAssistStartPos_);

        setCurrentRow(0);
        show();
    }
    else
    {
        hide();
    }
}

//-----------------------------------------------------------------------------
// Function: moveCloseToCursor()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::moveCloseToCursor(int cursorPos)
{
    // Save the old cursor.
    QTextCursor oldCursor = parent_->textCursor();

    // Move the cursor to the given position.
    QTextCursor cursor = oldCursor;
    cursor.setPosition(cursorPos, QTextCursor::MoveAnchor);
    parent_->setTextCursor(cursor);

    // Determine the correct upper-left corner position for the widget.
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();

    // By default, the desired position is below the cursor position.
    QPoint pos = parent_->mapToGlobal(parent_->cursorRect().bottomLeft()) + QPoint(-10, 10);
    
    // Restrict x coordinate by the screen width.
    pos.setX(qMin(qMax(0, pos.x()), screenWidth - width()));

    // Check if the tool tip hint is visible.
    if (toolTipHintWidget_.isVisible())
    {
        // Then by default, place the widget above the cursor.
        pos.setY(parent_->mapToGlobal(parent_->cursorRect().topLeft()).y() - height() - 10);

        // Lift it up if the tool tip hint is also above the cursor.
        if (pos.y() > toolTipHintWidget_.pos().y() - height())
        {
            pos.setY(toolTipHintWidget_.pos().y() - height());
        }

        // If the widget goes over the top edge of the screen, move it below the tool tip hint.
        if (pos.y() < 0)
        {
            pos.setY(toolTipHintWidget_.pos().y() + toolTipHintWidget_.height());
        }
    }
    // Otherwise lift the widget above the cursor only if necessary to keep it fully in view.
    else if (pos.y() + height() > screenHeight)
    {
        pos.setY(parent_->mapToGlobal(parent_->cursorRect().topLeft()).y() - height() - 10);
    }
    
    // Move the widget to the final position.
    move(pos);

    // Restore the old cursor.
    parent_->setTextCursor(oldCursor);
}

//-----------------------------------------------------------------------------
// Function: canCommitWith()
//-----------------------------------------------------------------------------
bool TextContentAssistWidget::canCommitWith(QKeyEvent* e) const
{
    return (!e->text().contains(QRegExp("^[a-z|A-z|0-9|_|;|ƒ|‰|÷|ˆ|≈|Â]$")) &&
            e->key() != Qt::Key_Backspace && e->key() != Qt::Key_Escape &&
            e->key() != Qt::Key_Up && e->key() != Qt::Key_Down &&
            e->key() != Qt::Key_Left && e->key() != Qt::Key_Right &&
            e->key() != Qt::Key_Space && e->modifiers() != Qt::ControlModifier);
}


//-----------------------------------------------------------------------------
// Function: commitSelection()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::commitSelection()
{
    Q_ASSERT(lastAssistStartPos_ != -1);

    // Setup the text cursor for replacement.
    QTextCursor cursor = parent_->textCursor();
    cursor.setPosition(lastAssistStartPos_, QTextCursor::KeepAnchor);
    cursor.insertText(currentItem()->text());    

    hideAssist();
}

//-----------------------------------------------------------------------------
// Function: fitToContents()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::fitToContents()
{
    if (count() == 0)
    {
        return;
    }

    int visibleRowCount = qMin(count(), maxVisibleItems_);
    int height = visibleRowCount * sizeHintForRow(0) + frameWidth() * 2;
    int width = sizeHintForColumn(0) + frameWidth() * 2 + verticalScrollBar()->sizeHint().width();
    
    setFixedSize(width, height);
}

//-----------------------------------------------------------------------------
// Function: showToolTipHint()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::showToolTipHint(QString const& text, int cursorPos)
{
    // Set the tool tip hint text.
    toolTipHintWidget_.setText(text);
    toolTipHintWidget_.adjustSize();

    // Retrieve the y coordinate from the current cursor.
    QPoint pos;
    pos.setY(parent_->mapToGlobal(parent_->cursorRect().bottomLeft()).y() + 5);

    // Save the old cursor.
    QTextCursor oldCursor = parent_->textCursor();

    // Move the cursor to the given position.
    QTextCursor cursor = oldCursor;
    cursor.setPosition(cursorPos, QTextCursor::MoveAnchor);
    parent_->setTextCursor(cursor);

    // Determine the correct upper-left corner position for the widget.
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();

    // By default, the desired position is below the cursor position.
    pos.setX(parent_->mapToGlobal(parent_->cursorRect().bottomLeft()).x());

    // Restrict x coordinate by the screen width.
    pos.setX(qMin(qMax(0, pos.x()), screenWidth - toolTipHintWidget_.width()));

    // If the widget would be partially below the screen, lift it above the cursor.
    if (pos.y() + toolTipHintWidget_.height() > screenHeight)
    {
        pos.setY(parent_->mapToGlobal(parent_->cursorRect().topLeft()).y() - toolTipHintWidget_.height() - 5);
    }

    // Move the widget to the final position.
    toolTipHintWidget_.move(pos);

    // Restore the old cursor.
    parent_->setTextCursor(oldCursor);

    // Show the hint.
    toolTipHintWidget_.show();
}

//-----------------------------------------------------------------------------
// Function: hideAssist()
//-----------------------------------------------------------------------------
void TextContentAssistWidget::hideAssist()
{
    hide();
    clear();
    contentFound_ = false;
}
