//-----------------------------------------------------------------------------
// File: ExpressionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// Editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------

#include "ExpressionEditor.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QTextCursor>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/SystemVerilogSyntax.h>

#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>


//-----------------------------------------------------------------------------
// Function: ExpressionEditor::ExpressionEditor()
//-----------------------------------------------------------------------------
ExpressionEditor::ExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent):
QTextEdit(parent),
MasterExpressionEditor(parameterFinder, this)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setUndoRedoEnabled(false);

    setTabChangesFocus(true);
    setLineWrapMode(QTextEdit::NoWrap);

    setFrameStyle(QFrame::Plain);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Sunken);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateCompletions()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void ExpressionEditor::setAppendingCompleter(QCompleter* completer)
{
    MasterExpressionEditor::setAppendingCompleter(completer);
    completer->setWidget(this);

    connect(completer, SIGNAL(activated(QModelIndex const&)), 
        this, SLOT(complete(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::setExpression()
//-----------------------------------------------------------------------------
void ExpressionEditor::setExpression(QString const& expression)
{
    clear();
    QTextCursor cursor = textCursor();
    cursor.setPosition(0);

    auto const& delimiter = WORD_DELIMITER;
    int delimiterIndex = expression.indexOf(delimiter, 0);

    QString word = expression.mid(0, delimiterIndex);

    insertWord(word, cursor);

    while(delimiterIndex != -1)
    {
        int wordEndIndex = expression.indexOf(delimiter, delimiterIndex);

        QString term = expression.mid(delimiterIndex, wordEndIndex - delimiterIndex);
        insertWord(term, cursor);

        QString operation = delimiter.match(expression, delimiterIndex).captured();
        cursor.insertText(operation, colorFormat(Qt::black));

        delimiterIndex = wordEndIndex + operation.length();
    }
    
    MasterExpressionEditor::setExpression(expression);
    previousExpression_ = expression;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getPreviousExpression()
//-----------------------------------------------------------------------------
QString ExpressionEditor::getPreviousExpression() const
{
    return previousExpression_;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::contextMenuEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::contextMenuEvent(QContextMenuEvent* menuEvent)
{
    menuEvent->accept();
    return;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::mousePressEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::mousePressEvent(QMouseEvent* mouseEvent)
{
    finishEditingCurrentWord();

    QTextEdit::mousePressEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
        QWidget::keyPressEvent(keyEvent);
        return;
    }

    handleKeyPressEvent(keyEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::focusInEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::focusInEvent(QFocusEvent* focusEvent)
{
    previousExpression_ = getExpression();
    QTextEdit::focusInEvent(focusEvent);

    if (focusEvent->reason() != Qt::PopupFocusReason )
    {
        selectAll();
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::complete()
//-----------------------------------------------------------------------------
void ExpressionEditor::complete(QModelIndex const& index)
{
    MasterExpressionEditor::complete(index);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::updateCompletions()
//-----------------------------------------------------------------------------
void ExpressionEditor::updateCompletions()
{
    updateCompleteWords();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::insertWord()
//-----------------------------------------------------------------------------
void ExpressionEditor::insertWord(QString const& word, QTextCursor& cursor)
{
    if (isReference(word))
    {
        cursor.insertText(getNameForParameterID(word), colorFormat(Qt::darkGreen));
    }
    else if (wordIsConstant(word) || getReservedWords().contains(word, Qt::CaseInsensitive) || wordIsMathFunction(word))
    {
        cursor.insertText(word, colorFormat(Qt::black));
    }
    else
    {
        cursor.insertText(word, colorFormat(Qt::red));
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::colorCurrentWordRed()
//-----------------------------------------------------------------------------
void ExpressionEditor::colorCurrentWordRed()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.setCharFormat(colorFormat(Qt::red));
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::colorCurrentWordBlack()
//-----------------------------------------------------------------------------
void ExpressionEditor::colorCurrentWordBlack()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.setCharFormat(colorFormat(Qt::black));
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::mouseMoveEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    handleMouseMoveEvent(mouseEvent);

    QTextEdit::mouseMoveEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    handleMouseReleaseEvent();

    QTextEdit::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::focusOutEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::focusOutEvent(QFocusEvent *event)
{
    QTextEdit::focusOutEvent(event);

    emit editingFinished();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::hasSelection()
//-----------------------------------------------------------------------------
bool ExpressionEditor::hasSelection()
{
    return textCursor().hasSelection();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::handleParentKeyPressEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::handleParentKeyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->text().isEmpty() == false)
    {
        auto cursor = textCursor();
        cursor.setCharFormat(colorFormat(Qt::black));
        setTextCursor(cursor);
    }

    QTextEdit::keyPressEvent(keyEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::handleReferenceDecrease()
//-----------------------------------------------------------------------------
void ExpressionEditor::handleReferenceDecrease(QString const& referenceItem)
{
    emit decreaseReference(referenceItem);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::handleReferenceIncrease()
//-----------------------------------------------------------------------------
void ExpressionEditor::handleReferenceIncrease(QString const& referenceItem)
{
    emit increaseReference(referenceItem);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::setCompletedParameterName()
//-----------------------------------------------------------------------------
void ExpressionEditor::setCompletedParameterName(QString const& parameterName)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(startOfCurrentWord());
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.insertText(parameterName, colorFormat(Qt::darkGreen));
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getCursorPosition()
//-----------------------------------------------------------------------------
int ExpressionEditor::getCursorPosition() const
{
    return textCursor().position();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getCurrentText()
//-----------------------------------------------------------------------------
QString ExpressionEditor::getCurrentText() const
{
    return toPlainText();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getSelectedText()
//-----------------------------------------------------------------------------
QString ExpressionEditor::getSelectedText() const
{
    return textCursor().selectedText();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getSelectionFirstWord()
//-----------------------------------------------------------------------------
int ExpressionEditor::getSelectionFirstWord() const
{
    return toPlainText().left(textCursor().selectionStart()).count(WORD_DELIMITER);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getSelectionLastWord()
//-----------------------------------------------------------------------------
int ExpressionEditor::getSelectionLastWord() const
{
    return toPlainText().left(textCursor().selectionEnd()).count(WORD_DELIMITER);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getSelectionStartIndex()
//-----------------------------------------------------------------------------
int ExpressionEditor::getSelectionStartIndex() const
{
    return textCursor().selectionStart();
}
