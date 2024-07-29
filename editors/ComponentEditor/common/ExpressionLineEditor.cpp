//-----------------------------------------------------------------------------
// File: ExpressionLineEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.10.2022
//
// Description:
// Line editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------

#include "ExpressionLineEditor.h"

#include <QKeyEvent>
#include <QRegularExpression>

#include <KactusAPI/include/ParameterFinder.h>

#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

#include "SystemVerilogSyntax.h"

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::ExpressionLineEditor()
//-----------------------------------------------------------------------------
ExpressionLineEditor::ExpressionLineEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent):
QLineEdit(parent),
MasterExpressionEditor(parameterFinder, this)
{
    connect(this, SIGNAL(cursorPositionChanged(int, int)), 
        this, SLOT(updateCompletions(int, int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::setAppendingCompleter(QCompleter* completer)
{
    MasterExpressionEditor::setAppendingCompleter(completer);
    completer->setWidget(this);

    connect(completer, SIGNAL(activated(QModelIndex const&)), 
        this, SLOT(complete(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::setExpression()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::setExpression(QString const& expression)
{
    MasterExpressionEditor::setExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::contextMenuEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::contextMenuEvent(QContextMenuEvent* menuEvent)
{
    menuEvent->accept();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::mousePressEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::mousePressEvent(QMouseEvent* mouseEvent)
{
    finishEditingCurrentWord();

    QLineEdit::mousePressEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
        QWidget::keyPressEvent(keyEvent);
        return;
    }

    handleKeyPressEvent(keyEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::focusInEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::focusInEvent(QFocusEvent* focusEvent)
{
    QLineEdit::focusInEvent(focusEvent);

    if (focusEvent->reason() != Qt::PopupFocusReason )
    {
        selectAll();
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::complete()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::complete(QModelIndex const& index)
{
    MasterExpressionEditor::complete(index);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::updateCompletions()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::updateCompletions(int /*oldPosition*/, int /*newPosition*/)
{
    updateCompleteWords();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::colorCurrentWordRed()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::colorCurrentWordRed()
{
    // Empty by intent.
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::colorCurrentWordBlack()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::colorCurrentWordBlack()
{
    // Empty by intent.
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::mouseMoveEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    handleMouseMoveEvent(mouseEvent);

    QLineEdit::mouseMoveEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    handleMouseReleaseEvent();

    QLineEdit::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::focusOutEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);

    emit editingFinished();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::hasSelection()
//-----------------------------------------------------------------------------
bool ExpressionLineEditor::hasSelection()
{
    int textStart = selectionStart();
    int textEnd = selectionEnd();

    return textStart >= 0 && textEnd >= 0 && textStart != textEnd;
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::handleParentKeyPressEvent()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::handleParentKeyPressEvent(QKeyEvent* keyEvent)
{
    QLineEdit::keyPressEvent(keyEvent);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::handleReferenceDecrease()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::handleReferenceDecrease(QString const& referenceItem)
{
    emit decreaseReference(referenceItem);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::handleReferenceIncrease()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::handleReferenceIncrease(QString const& referenceItem)
{
    emit increaseReference(referenceItem);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::setCompletedParameterName()
//-----------------------------------------------------------------------------
void ExpressionLineEditor::setCompletedParameterName(QString const& parameterName)
{
    setText(replaceNthWordWith(currentWordIndex(), text(), parameterName));
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getCursorPosition()
//-----------------------------------------------------------------------------
int ExpressionLineEditor::getCursorPosition() const
{
    return cursorPosition();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getCurrentText()
//-----------------------------------------------------------------------------
QString ExpressionLineEditor::getCurrentText() const
{
    return text();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getSelectedText()
//-----------------------------------------------------------------------------
QString ExpressionLineEditor::getSelectedText() const
{
    return selectedText();
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getSelectionFirstWord()
//-----------------------------------------------------------------------------
int ExpressionLineEditor::getSelectionFirstWord() const
{
    return text().left(selectionStart()).count(WORD_DELIMITER);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getSelectionLastWord()
//-----------------------------------------------------------------------------
int ExpressionLineEditor::getSelectionLastWord() const
{
    return text().left(selectionEnd()).count(WORD_DELIMITER);
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::getSelectionStartIndex()
//-----------------------------------------------------------------------------
int ExpressionLineEditor::getSelectionStartIndex() const
{
    return selectionStart();
}
