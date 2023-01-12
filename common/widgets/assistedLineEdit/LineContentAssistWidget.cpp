//-----------------------------------------------------------------------------
// File: LineContentAssistWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 4.1.2011
//
// Description:
// Content assist widget.
//-----------------------------------------------------------------------------

#include "LineContentAssistWidget.h"

#include <common/KactusColors.h>

#include <algorithm>
#include <QScrollBar>
#include <QRegularExpression>
#include <QToolTip>
#include <QTextCursor>
#include <QApplication>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: LineContentAssistWidget()
//-----------------------------------------------------------------------------
LineContentAssistWidget::LineContentAssistWidget(QLineEdit* target, QWidget* parentWnd):
QListWidget(parentWnd),
target_(target),
matcher_(0),
maxVisibleItems_(DEFAULT_MAX_VISIBLE_ITEMS),
lastAssistStartPos_(-1),
contentFound_(false),
lineAssistWidth_(0)
{
    Q_ASSERT(parentWnd != 0);
    Q_ASSERT(target != 0);

    // Set widget settings.
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShadow(QFrame::Raised);
    setFocusPolicy(Qt::NoFocus);
    setFont(QFont("Tahoma", 10));

    // We want the widget to look active even though it is inactive.
    QString borderColorName = KactusColors::DISABLED_TEXT.name();

    setStyleSheet("border-style: solid;"
        "border-width: 1px;"
        "border-color: " + borderColorName + ";"
        "selection-color: black; selection-background-color: "
        "QLinearGradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d6e7ff, stop: 1 #84aede);");

    hideAssist();
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
    if (matcher_ == 0)
    {
        return false;
    }

    if (e->key() == Qt::Key_Space && e->modifiers() == Qt::CTRL)
    {
        updateAssist(0);
        return true;
    }

    QString text = target_->text().left(target_->cursorPosition()) + e->text();

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
        int keyEvent = e->key();
        if (keyEvent == Qt::Key_Up || keyEvent == Qt::Key_Down)
        {
            if (isVisible())
            {
                return handleKeyboardItemSelection(e, keyEvent);
            }
        }
        else if (keyEvent == Qt::Key_Left || keyEvent == Qt::Key_Right || keyEvent == Qt::Key_Home ||
            keyEvent == Qt::Key_End)
        {
            cancel();
            return false;
        }
        else if (keyEvent == Qt::Key_A)
        {
            if (e->modifiers() == Qt::ControlModifier)
            {
                cancel();
                return false;
            }
        }
        else if (keyEvent == Qt::Key_Escape)
        {
            cancel();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: updateContent()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::updateAssist(QKeyEvent* e)
{
    if (matcher_ == 0)
    {
        return;
    }

    // Invoke the content assist for certain keys.
    if (e == 0 || (e->text().contains(QRegularExpression("^[a-z|A-z|0-9|_|Ä|ä|Ö|ö|Å|å|(|)|,|&]$")) ||
        e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Return ||
        e->key() == Qt::Key_Down || e->key() == Qt::Key_Up ||
        (contentFound_ && e->text().contains(' '))))
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
    maxVisibleItems_ = count;
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
    QString text = target_->text().left(target_->cursorPosition());

    contentFound_ = matcher_->fillWithContent(text, *this, lastAssistStartPos_, toolTipText, toolTipIndex);

    // Check if content was found.
    if (contentFound_)
    {
        // Fit the widget to the contents and move it accordingly.
        fitToContents();
        moveClose(lastAssistStartPos_);

        setCurrentRow(0);
        raise();
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
    int parentWidth = parentWidget()->width();
    
    // By default, the desired position is below line edit.
    QPoint pos = target_->mapTo(parentWidget(), QPoint(0, 0)) + QPoint(0,  target_->height());
    
    // Restrict x coordinate by the screen width.
    pos.setX(qMin(qMax(0, pos.x()), qMax(0, parentWidth - width())));
    
    // Move the widget to the final position.
    move(pos);
}

//-----------------------------------------------------------------------------
// Function: canCommitWith()
//-----------------------------------------------------------------------------
bool LineContentAssistWidget::canCommitWith(QKeyEvent* e) const
{
    return (!e->text().contains(QRegularExpression("^[a-z|A-z|0-9|_|Ä|ä|Ö|ö|Å|å]$")) &&
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
    Q_ASSERT(lastAssistStartPos_ != -1);

    // Replace the text with the current content.
    target_->setSelection(lastAssistStartPos_, target_->cursorPosition());
    target_->insert(currentItem()->text());

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

    int visibleRowCount = qMin(count(), maxVisibleItems_);

    int rowHeight = sizeHintForRow(0);
    int height = visibleRowCount * rowHeight + frameWidth() * 2;

    int parentHeight = parentWidget()->height();
    int assistPosition = target_->mapTo(parentWidget(), QPoint(0, 0)).y() + target_->height();

    if (assistPosition + height > parentHeight)
    {
        int availableHeight = parentHeight - assistPosition;
        int availableRows = availableHeight / rowHeight;
        height = availableRows * rowHeight + frameWidth() * 2;
    }

    setFixedSize(lineAssistWidth_, height);
}

//-----------------------------------------------------------------------------
// Function: hideAssist()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::hideAssist()
{
    hide();
    clear();
    contentFound_ = false;
}

//-----------------------------------------------------------------------------
// Function: LineContentAssistWidget::setContentMatcher()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::setContentMatcher(ILineContentMatcher* matcher)
{
    matcher_ = matcher;
}

//-----------------------------------------------------------------------------
// Function: LineContentAssistWidget::setWidthForAssist()
//-----------------------------------------------------------------------------
void LineContentAssistWidget::setWidthForAssist(int newWidth)
{
    lineAssistWidth_ = newWidth;
}

//-----------------------------------------------------------------------------
// Function: LineContentAssistWidget::handleKeyboardItemSelection()
//-----------------------------------------------------------------------------
bool LineContentAssistWidget::handleKeyboardItemSelection(QKeyEvent* event, int key)
{
    int numberOfItems = count();

    if (currentIndex().row() == 0 && key == Qt::Key_Up)
    {
        setCurrentIndex(model()->index(numberOfItems - 1, 0));
    }
    else if (currentIndex().row() == numberOfItems -1 && key == Qt::Key_Down)
    {
        setCurrentIndex(model()->index(0, 0));
    }
    else
    {
        keyPressEvent(event);
    }

    return true;
}
