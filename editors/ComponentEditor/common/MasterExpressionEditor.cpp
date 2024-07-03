//---------
// --------------------------------------------------------------------
// File: MasterExpressionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.11.2022
//
// Description:
// Parent class for expression editors with parameter name completion.
//-----------------------------------------------------------------------------

#include "MasterExpressionEditor.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/SystemVerilogSyntax.h>

#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::ExpressionLineEditor()
//-----------------------------------------------------------------------------
MasterExpressionEditor::MasterExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parentWidget):
nameCompleter_(new QCompleter(parentWidget)),
parameterFinder_(parameterFinder)
{
    nameCompleter_->setWidget(parentWidget);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::setAppendingCompleter(QCompleter* completer)
{
    nameCompleter_ = completer;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::completer()
//-----------------------------------------------------------------------------
QCompleter* MasterExpressionEditor::completer() const
{
    return nameCompleter_;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::getExpression()
//-----------------------------------------------------------------------------
QString MasterExpressionEditor::getExpression() const
{
    return expression_;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::setExpression()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::setExpression(QString const& expression)
{
    expression_ = expression;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::finishEditingCurrentWord()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::finishEditingCurrentWord()
{
    QString finishedWord = currentWord();
    QString currentTerm = nthWordIn(currentWordIndex(), expression_);

    bool termIsReference = isReference(currentTerm);
    bool shouldChangeTerm = !termIsReference || parameterFinder_->nameForId(currentTerm) != finishedWord;

    if (shouldChangeTerm)
    {
        expression_ = replaceNthWordWith(currentWordIndex(), expression_, finishedWord);

        if (currentWordIsUniqueParameterName())
        {
            complete(nameCompleter_->currentIndex());
        }
        else if (reservedWords_.contains(finishedWord, Qt::CaseInsensitive))
        {
            colorCurrentWordBlack();
        }
        else if (!finishedWord.isEmpty() && !wordIsConstant(finishedWord) && !wordIsMathFunction(finishedWord))
        {
            colorCurrentWordRed();
        }

        if (termIsReference)
        {
            handleReferenceDecrease(currentTerm);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::handleKeyPressEvent()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::handleKeyPressEvent(QKeyEvent* keyEvent)
{
    if (keysequenceCopyCutPaste(keyEvent))
    {
        keyEvent->accept();
        return;
    }

    if (keyMovesCursor(keyEvent))
    {
        finishEditingCurrentWord();
    }

    else if (removesOrReplacesText(keyEvent))
    {
        if (hasSelection())
        {
            replaceSelectionInExpression(keyEvent);
        }
        else if (removesOperatorBeforeWord(keyEvent))
        {
            removeOperatorBeforeCursorInExpression();
        }
        else if (removesOperatorAfterCursor(keyEvent))
        {
            removeOperatorAfterCursorInExpression();
        }
        if (removesLastCharacterOfWord(keyEvent))
        {
            removeTermUnderCursor();
        }
    }
    else if (isWordDelimiter(keyEvent->text()))
    {
        finishEditingCurrentWord();
        finishEditingCurrentTerm(keyEvent->text());
    }

    if (!showCompletionsRequested(keyEvent))
    {
        handleParentKeyPressEvent(keyEvent);
    }

    if (!currentWord().isEmpty() || showCompletionsRequested(keyEvent))
    {
        nameCompleter_->setCompletionPrefix(currentWord());
        nameCompleter_->complete();
    }
    else
    {
        nameCompleter_->popup()->hide();
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::complete()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::complete(QModelIndex const& index)
{
    int selectedRow = index.row();

    auto const nameIndex = nameCompleter_->completionModel()->index(selectedRow, ComponentParameterColumns::NAME);
    auto const parameterName = nameIndex.data().toString();

    auto const idIndex = nameIndex.siblingAtColumn(ComponentParameterColumns::ID);
    auto parameterId = idIndex.data().toString();
   
    if (parameterId.isEmpty())
    {
        parameterId = parameterName;
    }

    if (auto previousTerm = nthWordIn(currentWordIndex(), expression_); 
        isReference(previousTerm))
    {
        handleReferenceDecrease(previousTerm);
    }

    expression_ = replaceNthWordWith(currentWordIndex(), expression_, parameterId);

    setCompletedParameterName(parameterName);

    nameCompleter_->popup()->hide();

    handleReferenceIncrease(parameterId);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::updateCompleteWords()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::updateCompleteWords()
{
    if (notSelectingText_)
    {
        nameCompleter_->setCompletionPrefix(currentWord());

        if (!currentWord().isEmpty())
        {
            nameCompleter_->complete();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::isReference()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::isReference(QString const& text) const
{
    if (parameterFinder_.isNull())
    {
        return false;
    }

    return parameterFinder_->hasId(text);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::wordIsConstant()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::wordIsConstant(QString const& word) 
{
    static QRegularExpression constant("^" + SystemVerilogSyntax::INTEGRAL_NUMBER + "|" +
        SystemVerilogSyntax::STRING_LITERAL + "$");
    return constant.match(word).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::wordIsMathFunction()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::wordIsMathFunction(QString const& word)
{
    static const QRegularExpression mathOperator("^" + SystemVerilogSyntax::MATH_FUNCTION + "$");
    return mathOperator.match(word).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::colorFormat()
//-----------------------------------------------------------------------------
QTextCharFormat MasterExpressionEditor::colorFormat(Qt::GlobalColor textColor) const
{
    QTextCharFormat format;
    format.setForeground(textColor);
    return format;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::editingMiddleOfReference()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::editingMiddleOfReference() const
{
    int newCursorPosition = getCursorPosition();

    return isReference(nthWordIn( currentWordIndex(), expression_)) && 
        newCursorPosition != startOfCurrentWord() &&
        newCursorPosition != endOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesLastCharacterOfWord()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesLastCharacterOfWord(QKeyEvent* keyEvent) const
{
    int newCursorPosition = getCursorPosition();

    return ((keyEvent->key() == Qt::Key_Delete && newCursorPosition == startOfCurrentWord()) ||
        (keyEvent->key() == Qt::Key_Backspace && newCursorPosition == endOfCurrentWord())) &&
        currentWordLength() == 1;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removeTermUnderCursor()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::removeTermUnderCursor()
{
    auto const removedReference = nthWordIn(currentWordIndex(), expression_);
    expression_ = replaceNthWordWith(currentWordIndex(), expression_, "");

    if (isReference(removedReference))
    {
        handleReferenceDecrease(removedReference);
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesOperatorBeforeWord()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesOperatorBeforeWord(QKeyEvent* keyEvent) const
{
    return keyEvent->key() == Qt::Key_Backspace && getCursorPosition() == startOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removeOperatorBeforeCursorInExpression()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::removeOperatorBeforeCursorInExpression()
{
    int position = indexOfNthWord(currentWordIndex(), expression_);
    expression_.remove(position - 1, 1);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesOperatorAfterCursor()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesOperatorAfterCursor(QKeyEvent* keyEvent) const
{
    return keyEvent->key() == Qt::Key_Delete && getCursorPosition() == endOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removeOperatorAfterCursorInExpression()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::removeOperatorAfterCursorInExpression()
{
    int position = indexOfNthWord(currentWordIndex() + 1, expression_);
    expression_.remove(position - 1, 1);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::currentWord()
//-----------------------------------------------------------------------------
QString MasterExpressionEditor::currentWord() const
{
    return getCurrentText().mid(startOfCurrentWord(), currentWordLength());
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::startOfCurrentWord()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::startOfCurrentWord() const
{
    auto plainText = getCurrentText();
    auto leftSide = plainText.left(getCursorPosition());

    int operandIndex = leftSide.lastIndexOf(WORD_DELIMITER);

    int operatorSize = 1;

    if (0 < operandIndex && operandIndex < leftSide.size())
    {
        operatorSize = WORD_DELIMITER.match(leftSide, operandIndex).capturedLength();
    }

    return operandIndex + operatorSize;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::endOfCurrentWord()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::endOfCurrentWord() const
{
    int endPos = getCurrentText().indexOf(WORD_DELIMITER, getCursorPosition());

    if (endPos == -1)
    {
        endPos = getCurrentText().length();
    }

    return endPos;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::currentWordLength()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::currentWordLength() const
{
    return endOfCurrentWord() - startOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::currentWordIndex()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::currentWordIndex() const
{
    return getCurrentText().left(getCursorPosition()).count(WORD_DELIMITER);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::replaceNthWordWith()
//-----------------------------------------------------------------------------
QString MasterExpressionEditor::replaceNthWordWith(int n, QString const& oldText, QString const& after) const
{
    QString replaced = oldText;
    replaced.replace(indexOfNthWord(n, oldText), nthWordIn(n, oldText).length(), after);
    return replaced;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::nthWordIn()
//-----------------------------------------------------------------------------
QString MasterExpressionEditor::nthWordIn(int n, QString const& text) const
{
    int startIndex = indexOfNthWord(n, text);

    int endIndex = text.indexOf(WORD_DELIMITER, startIndex);
    if (endIndex == -1)
    {
        endIndex = text.length();
    }

    int wordLength = endIndex - startIndex;
    return text.mid(startIndex, wordLength);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::indexOfNthWord()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::indexOfNthWord(int n, QString const& text) const
{
    int startIndex = 0;
    for (int i = 0; i < n; i++)
    {
        startIndex = text.indexOf(WORD_DELIMITER, startIndex) + 1;
    }

    return startIndex;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::currentWordIsUniqueParameterName()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::currentWordIsUniqueParameterName() const
{
    return nameCompleter_->completionCount() == 1 && currentWord() == nameCompleter_->currentCompletion();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::isWordDelimiter()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::isWordDelimiter(QString const& text) const
{
    return WORD_DELIMITER.match(text).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: ExpressionMasterEditor::handleMouseMoveEvent()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::handleMouseMoveEvent(QMouseEvent* mouseEvent)
{
    notSelectingText_ = !(mouseEvent->buttons() & Qt::LeftButton);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::handleMouseReleaseEvent()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::handleMouseReleaseEvent()
{
    notSelectingText_ = true;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesOrReplacesText()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesOrReplacesText(QKeyEvent* keyEvent)
{
    return keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace ||
        (hasSelection() && !keyEvent->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::keysequenceCopyCutPaste()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::keysequenceCopyCutPaste(QKeyEvent* keyEvent) const
{
    return keyEvent == QKeySequence::Copy || keyEvent == QKeySequence::Cut || keyEvent == QKeySequence::Paste;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::movesCursor()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::keyMovesCursor(QKeyEvent* keyEvent) const
{
    return keyEvent->key() == Qt::Key_Home || keyEvent->key() == Qt::Key_End || keyEvent->key() == Qt::Key_Left ||
        keyEvent->key() == Qt::Key_Right;
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::removeSelectionInExpression()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::replaceSelectionInExpression(QKeyEvent* keyEvent)
{
    QString selectedText = getSelectedText();

    int firstWord = getSelectionFirstWord();
    int lastWord = getSelectionLastWord();

    replaceReferencesInExpressionWithNames(firstWord, lastWord);

    int firstTermPos = indexOfNthWord(firstWord, expression_);
    expression_.remove(expression_.indexOf(selectedText, firstTermPos), selectedText.length());
    expression_.insert(firstTermPos, keyEvent->text()); // Don't just remove selection, also insert whatever was typed.
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::replaceReferencesInExpressionWithNames()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::replaceReferencesInExpressionWithNames(int firstWord, int lastWord)
{
    for (int i = firstWord; i <= lastWord; i++)
    {
        QString term = nthWordIn(i, expression_);
        if (isReference(term))
        {
            handleReferenceDecrease(term);

            expression_ = replaceNthWordWith(i, expression_, nthWordIn(i, getCurrentText()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::finishEditingCurrentTerm()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::finishEditingCurrentTerm(QString const& delimiter)
{
    QString term = nthWordIn(currentWordIndex(), expression_);
    QString termAndDelimiter = term;
    if (getCursorPosition() == startOfCurrentWord())
    {
        termAndDelimiter.prepend(delimiter);
    }
    else if (getCursorPosition() == endOfCurrentWord())
    {
        termAndDelimiter.append(delimiter);
    }
    else
    {
        if (isReference(term))
        {
            handleReferenceDecrease(term);

            termAndDelimiter = nthWordIn(currentWordIndex(), getCurrentText());
        }
        termAndDelimiter.insert(getCursorPosition() - startOfCurrentWord(), delimiter);
    }

    expression_ = replaceNthWordWith(currentWordIndex(), expression_, termAndDelimiter);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::showCompletionsReguested()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::showCompletionsRequested(QKeyEvent* keyEvent) const
{
    return keyEvent->key() == Qt::Key_Space && (keyEvent->modifiers() & Qt::ControlModifier);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::setReservedWords()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::setReservedWords(QStringList newReservations)
{
    reservedWords_ = newReservations;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::getNameForParameterID()
//-----------------------------------------------------------------------------
QString MasterExpressionEditor::getNameForParameterID(QString const& parameterID) const
{
    return parameterFinder_->nameForId(parameterID);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::getReservedWords()
//-----------------------------------------------------------------------------
QStringList MasterExpressionEditor::getReservedWords() const
{
    return reservedWords_;
}
