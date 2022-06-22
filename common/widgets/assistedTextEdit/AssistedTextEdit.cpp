//-----------------------------------------------------------------------------
// File: AssistedTextEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// Text edit widget which offers content assist.
//-----------------------------------------------------------------------------

#include "AssistedTextEdit.h"

#include <cassert>
#include <algorithm>
#include <QFont>
#include <QRegularExpression>
#include <QApplication>
#include <QTextBlock>

#include "TextContentAssistWidget.h"

//-----------------------------------------------------------------------------
// Function: AssistedTextEdit()
//-----------------------------------------------------------------------------
AssistedTextEdit::AssistedTextEdit(QSharedPointer<ITextContentMatcher> contentMatcher,
                                   QWidget* parentWnd, QWidget* parent)
    : QPlainTextEdit(parent),
      matcher_(contentMatcher),
      contentAssist_(0),
      indentStyle_(INDENT_STYLE_TAB),
      indentWidth_(DEFAULT_INDENT_WIDTH)
{
    //setFont(QFont("Courier", 10));
    setLineWrapMode(NoWrap);

    contentAssist_ = new TextContentAssistWidget(this, parentWnd, contentMatcher);

    // Install this as an event filter so that we can track events from the main window.
    parentWnd->installEventFilter(this);
}

//-----------------------------------------------------------------------------
// Function: ~AssistedTextEdit()
//-----------------------------------------------------------------------------
AssistedTextEdit::~AssistedTextEdit()
{
    delete contentAssist_;
}

//-----------------------------------------------------------------------------
// Function: setIndentStyle()
//-----------------------------------------------------------------------------
void AssistedTextEdit::setIndentStyle(IndentStyle style, unsigned int width)
{
    indentStyle_ = style;
    indentWidth_ = width;

    // Set the correct tab stop width.
    QFontMetrics metrics(font());
    setTabStopWidth(indentWidth_ * metrics.averageCharWidth());
}

//-----------------------------------------------------------------------------
// Function: getIndentStyle()
//-----------------------------------------------------------------------------
IndentStyle AssistedTextEdit::getIndentStyle() const
{
    return indentStyle_;
}

//-----------------------------------------------------------------------------
// Function: getIndentWidth()
//-----------------------------------------------------------------------------
unsigned int AssistedTextEdit::getIndentWidth() const
{
    return indentWidth_;
}

//-----------------------------------------------------------------------------
// Function: eventFilter()
//-----------------------------------------------------------------------------
bool AssistedTextEdit::eventFilter(QObject*, QEvent* e)
{
    // Cancel the assist if the main window is moved or resized.
    if (contentAssist_->isContentShown() && (e->type() == QEvent::Move || e->type() == QEvent::Resize))
    {
        contentAssist_->cancel();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void AssistedTextEdit::keyPressEvent(QKeyEvent* e)
{
    // First try to handle the key full by the content assist.
    if (!contentAssist_->tryHandleKey(e))
    {
        // Process the key event if it still needs handling by the editor.
        switch (e->key())
        {
        case Qt::Key_Tab:
            {
                // Check if the tab indentation needs to be replaced with spaces.
                if (indentStyle_ == INDENT_STYLE_SPACES)
                {
                    // Invoke a custom key press event for indentation.
                    QKeyEvent indentEvent(QEvent::KeyPress, 0, Qt::NoModifier, createIndentString(1),
                                          indentWidth_);
                    QPlainTextEdit::keyPressEvent(&indentEvent);
                }
                break;
            }

        case Qt::Key_Return:
            {
                onReturnKeyPress(e);
                break;
            }

        case Qt::Key_BraceRight:
            {
                // Handle the key event.
                QPlainTextEdit::keyPressEvent(e);

                // Indent the right brace.
                indentRightBrace();
                break;
            }

        case Qt::Key_Colon:
            {
                // Handle the key event.
                QPlainTextEdit::keyPressEvent(e);

                // Indent the colon correctly.
                indentColon();
                break;
            }

        default:
            {
                // Default operation.
                QPlainTextEdit::keyPressEvent(e);
                break;
            }
        }

        // Update the content assist.
        contentAssist_->updateAssist(e);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AssistedTextEdit::mousePressEvent(QMouseEvent* e)
{
    QPlainTextEdit::mousePressEvent(e);

    // Cancel the content assist if content is shown.
    if (contentAssist_->isContentShown())
    {
        contentAssist_->cancel();
    }
}

//-----------------------------------------------------------------------------
// Function: focusOutEvent()
//-----------------------------------------------------------------------------
void AssistedTextEdit::focusOutEvent(QFocusEvent* e)
{
    QPlainTextEdit::focusOutEvent(e);

    // Cancel the content assist if content is shown.
    if (contentAssist_->isContentShown())
    {
        contentAssist_->cancel();
    }
}

//-----------------------------------------------------------------------------
// Function: createIndentString()
//-----------------------------------------------------------------------------
QString AssistedTextEdit::createIndentString(unsigned int depth)
{
    switch (indentStyle_)
    {
    case INDENT_STYLE_TAB:
        {
            return QString(depth, '\t');
        }

    case INDENT_STYLE_SPACES:
        {
            return QString(depth * indentWidth_, ' ');
        }
    }

    // Shouldn't ever get here.
    assert(false);
    return QString();
}

//-----------------------------------------------------------------------------
// Function: calculateIndentDepth()
//-----------------------------------------------------------------------------
int AssistedTextEdit::calculateIndentDepth(int pos)
{
    int indentDepth = 0;
    int searchPos = 0;
    int discardPos = 0;
    QString text = toPlainText();
    QRegularExpression braceExp("(\\{|\\})");
    QRegularExpression discardExp("(\"([^\"]|(\\\\\"))*\")|(\'([^\']|(\\\\\'))*\')|(//[^\n]*\\n)|(/\\*[^*/]*\\*/)");

    // Search all braces up to the cursor position discarding comments, strings and char literals.
    searchPos = text.indexOf(braceExp);
    discardPos = text.indexOf(discardExp);

    while (searchPos >= 0 && searchPos < pos)
    {
        // If the search position has gone over the discard area, search the next discard area.
        auto discardLength = discardExp.match(text, discardPos).capturedLength();
        while (discardPos >= 0 && searchPos >= discardPos + discardLength)
        {
            discardPos = text.indexOf(discardExp, discardPos + discardLength);
            discardLength = discardExp.match(text, discardPos).capturedLength();
        }

        if (discardPos < 0 || searchPos < discardPos || searchPos > discardPos + discardLength)
        {
            QChar c = text.at(searchPos);

            if (c == '{')
            {
                ++indentDepth;
            }
            else if (c == '}')
            {
                --indentDepth;
            }
        }

        searchPos = text.indexOf(braceExp, searchPos + 1);
    }

    return qMax(indentDepth, 0);
}

//-----------------------------------------------------------------------------
// Function: indentRightBrace()
//-----------------------------------------------------------------------------
void AssistedTextEdit::indentRightBrace()
{
    static QRegularExpression braceRightExp("^\\s*(?=\\})");
    QString blockStr = textCursor().block().text();

    // Update the beginning of the current text block with the correct indentation
    // if there is only whitespace before the first right brace.
    auto blockMatch = braceRightExp.match(blockStr);

    if (blockMatch.hasMatch())
    {
        // Calculate the indentation depth.
        int indentDepth = calculateIndentDepth(textCursor().block().position() +
            blockMatch.capturedLength() + 1);

        int startIndex = textCursor().block().position();
        int endIndex = startIndex + blockMatch.capturedLength();

        // Replace the area with the correct indentation.
        replaceWithIndent(startIndex, endIndex, indentDepth);
    }
}

//-----------------------------------------------------------------------------
// Function: indentColon()
//-----------------------------------------------------------------------------
void AssistedTextEdit::indentColon()
{
    static QRegularExpression wpStartExp("^\\s*(?=[^;]*:)");
    QString blockStr = textCursor().block().text();

    // Update the beginning of the current text block with the correct indentation
    // for the whitespace part, if there is no following semicolon.
    auto wpMatch = wpStartExp.match(blockStr);

    if (wpMatch.hasMatch())
    {
        // Calculate the indentation depth.
        int indentDepth = qMax(0, calculateIndentDepth(textCursor().block().position() +
            wpMatch.capturedLength()) - 1);

        int startIndex = textCursor().block().position();
        int endIndex = startIndex + wpMatch.capturedLength();

        // Replace the area with the correct indentation.
        replaceWithIndent(startIndex, endIndex, indentDepth);
    }
}

//-----------------------------------------------------------------------------
// Function: replaceWithIndent()
//-----------------------------------------------------------------------------
void AssistedTextEdit::replaceWithIndent(int startIndex, int endIndex, int indentDepth)
{
    // Save the old cursor.
    QTextCursor oldCursor = textCursor();

    // Select the part of the block to auto-indent.
    QTextCursor cursor = oldCursor;
    cursor.setPosition(startIndex, QTextCursor::MoveAnchor);
    cursor.setPosition(endIndex, QTextCursor::KeepAnchor);
    setTextCursor(cursor);

    // And then replace it with the correct indentation.
    if (indentDepth > 0)
    {
        QKeyEvent indentEvent(QEvent::KeyPress, 0, Qt::NoModifier, createIndentString(indentDepth),
                              indentDepth);
        QPlainTextEdit::keyPressEvent(&indentEvent);
    }
    else
    {
        textCursor().removeSelectedText();
    }

    // Restore the cursor.
    setTextCursor(oldCursor);
}

//-----------------------------------------------------------------------------
// Function: onReturnKeyPress()
//-----------------------------------------------------------------------------
void AssistedTextEdit::onReturnKeyPress(QKeyEvent* e)
{
    // Calculate the indentation depth.
    int indentDepth = calculateIndentDepth(textCursor().position());

    // Handle the enter event.
    QPlainTextEdit::keyPressEvent(e);

    // Apply auto-indentation.
    if (indentDepth > 0)
    {
        QKeyEvent indentEvent(QEvent::KeyPress, 0, Qt::NoModifier, createIndentString(indentDepth),
                              indentDepth);
        QPlainTextEdit::keyPressEvent(&indentEvent);
    }
}

//-----------------------------------------------------------------------------
// Function: AssistedTextEdit::getMatcher()
//-----------------------------------------------------------------------------
ITextContentMatcher& AssistedTextEdit::getMatcher()
{
    return *matcher_;
}
