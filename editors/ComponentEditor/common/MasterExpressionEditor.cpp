//-----------------------------------------------------------------------------
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
#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

#include "SystemVerilogSyntax.h"

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::ExpressionLineEditor()
//-----------------------------------------------------------------------------
MasterExpressionEditor::MasterExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parentWidget):
nameCompleter_(new QCompleter(parentWidget)),
parameterFinder_(parameterFinder),
notSelectingText_(true),
reservedWords_(),
parentWidget_(parentWidget)
{
    reservedWords_ << "true" << "false";

    nameCompleter_->setWidget(parentWidget_);
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::setAppendingCompleter(QCompleter* completer)
{
    nameCompleter_ = completer;
    nameCompleter_->setWidget(parentWidget_);
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
        else if (!finishedWord.isEmpty() && !wordIsConstant(finishedWord))
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
            removeSelectionInExpression();
        }
        if (removesLastCharacterOfWord(keyEvent))
        {
            removeTermUnderCursor();
        }
        else if (removesOperatorBeforeWord(keyEvent))
        {
            removeOperatorBeforeCursorInExpression();
        }
        else if (removesOperatorAfterCursor(keyEvent))
        {
            removeOperatorAfterCursorInExpression();
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

    QModelIndex nameIndex = nameCompleter_->completionModel()->index(selectedRow, ComponentParameterColumns::NAME);
    QString parameterName = nameIndex.data().toString();

    QModelIndex idIndex = nameCompleter_->completionModel()->index(selectedRow, ComponentParameterColumns::ID);
    QString parameterId = idIndex.data().toString();
   
    if (parameterId.isEmpty())
    {
        parameterId = parameterName;
    }

    QString previousTerm = nthWordIn(currentWordIndex(), expression_);
    if (isReference(previousTerm))
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
// Function: MasterExpressionEditor::wordDelimiter()
//-----------------------------------------------------------------------------
QRegularExpression MasterExpressionEditor::wordDelimiter() const
{
    return QRegularExpression("[-+/*() ,{}~^]|[<>]=?|[!=]=?|[|&]{1,2}");
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
bool MasterExpressionEditor::wordIsConstant(QString const& word) const
{
    QRegularExpression constant("^" + SystemVerilogSyntax::INTEGRAL_NUMBER + "|" + SystemVerilogSyntax::STRING_LITERAL + "$");
    return constant.match(word).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::colorFormat()
//-----------------------------------------------------------------------------
QTextCharFormat MasterExpressionEditor::colorFormat(QString const& textColor) const
{
    QTextCharFormat format;
    format.setForeground(QColor(textColor));
    return format;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::editingMiddleOfReference()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::editingMiddleOfReference() const
{
    int newCursorPosition = getCursorPosition();

    return isReference(nthWordIn( currentWordIndex(), expression_)) && newCursorPosition != startOfCurrentWord() && newCursorPosition != endOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesLastCharacterOfWord()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesLastCharacterOfWord(QKeyEvent* keyEvent)
{
    int newCursorPosition = getCursorPosition();

    return ((keyEvent->key() == Qt::Key_Delete && newCursorPosition == startOfCurrentWord()) || (keyEvent->key() == Qt::Key_Backspace && newCursorPosition == endOfCurrentWord())) && currentWordLength() == 1;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removeTermUnderCursor()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::removeTermUnderCursor()
{
    QString removedReference = nthWordIn(currentWordIndex(), expression_);
    expression_ = replaceNthWordWith(currentWordIndex(), expression_, "");

    if (isReference(removedReference))
    {
        handleReferenceDecrease(removedReference);
    }
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::removesOperatorBeforeWord()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::removesOperatorBeforeWord(QKeyEvent* keyEvent)
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
bool MasterExpressionEditor::removesOperatorAfterCursor(QKeyEvent* keyEvent)
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
    QString plainText = getCurrentText();
    QString leftSide = plainText.left(getCursorPosition());

    int operandIndex = leftSide.lastIndexOf(wordDelimiter());

    int operatorSize = 1;

    if (operandIndex > 0 && operandIndex < leftSide.size())
    {
        QString operatorAtPosition = leftSide.at(operandIndex);

        operatorSize = wordDelimiter().match(leftSide, operandIndex).capturedLength();
    }

    return operandIndex + operatorSize;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::endOfCurrentWord()
//-----------------------------------------------------------------------------
int MasterExpressionEditor::endOfCurrentWord() const
{
    int endPos = getCurrentText().indexOf(wordDelimiter(), getCursorPosition());

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
    return getCurrentText().left(getCursorPosition()).count(wordDelimiter());
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

    int endIndex = text.indexOf(wordDelimiter(), startIndex);
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
        startIndex = text.indexOf(wordDelimiter(), startIndex) + 1;
    }

    return startIndex;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::currentWordIsUniqueParameterName()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::currentWordIsUniqueParameterName()
{
    return nameCompleter_->completionCount() == 1 && currentWord() == nameCompleter_->currentCompletion();
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::isWordDelimiter()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::isWordDelimiter(QString const& text) const
{
    return wordDelimiter().match(text).hasMatch();
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
    return keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace || (hasSelection() && !keyEvent->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::keysequenceCopyCutPaste()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::keysequenceCopyCutPaste(QKeyEvent* keyEvent)
{
    return keyEvent == QKeySequence::Copy || keyEvent == QKeySequence::Cut || keyEvent == QKeySequence::Paste;
}

//-----------------------------------------------------------------------------
// Function: MasterExpressionEditor::movesCursor()
//-----------------------------------------------------------------------------
bool MasterExpressionEditor::keyMovesCursor(QKeyEvent* keyEvent)
{
    return keyEvent->key() == Qt::Key_Home || keyEvent->key() == Qt::Key_End || keyEvent->key() == Qt::Key_Left ||
        keyEvent->key() == Qt::Key_Right;
}

//-----------------------------------------------------------------------------
// Function: ExpressionLineEditor::removeSelectionInExpression()
//-----------------------------------------------------------------------------
void MasterExpressionEditor::removeSelectionInExpression()
{
    QString selectedText = getSelectedText();

    int firstWord = getSelectionFirstWord();
    int lastWord = getSelectionLastWord();

    replaceReferencesInExpressionWithNames(firstWord, lastWord);


    int firstTermPos = indexOfNthWord(firstWord, expression_);
    expression_.remove(expression_.indexOf(selectedText, firstTermPos), selectedText.length());
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
void MasterExpressionEditor::finishEditingCurrentTerm(QString delimiter)
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
bool MasterExpressionEditor::showCompletionsRequested(QKeyEvent* keyEvent)
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
