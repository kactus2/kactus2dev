//-----------------------------------------------------------------------------
// File: ExpressionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// Editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------

#include "ExpressionEditor.h"

#include <QEvent>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCompleter>
#include <QTextCursor>
#include <QAbstractItemView>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

#include "SystemVerilogSyntax.h"

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::ExpressionEditor()
//-----------------------------------------------------------------------------
ExpressionEditor::ExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent)
    : QTextEdit(parent), nameCompleter_(0), parameterFinder_(parameterFinder)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTabChangesFocus(true);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::~ExpressionEditor()
//-----------------------------------------------------------------------------
ExpressionEditor::~ExpressionEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void ExpressionEditor::setAppendingCompleter(QCompleter* completer)
{
    nameCompleter_ = completer;
    nameCompleter_->setWidget(this);

    connect(nameCompleter_, SIGNAL(activated(QModelIndex const&)), 
        this, SLOT(complete(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::completer()
//-----------------------------------------------------------------------------
QCompleter* ExpressionEditor::completer() const
{
    return nameCompleter_;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getExpression()
//-----------------------------------------------------------------------------
QString ExpressionEditor::getExpression() const
{
    return expression_;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::setExpression()
//-----------------------------------------------------------------------------
void ExpressionEditor::setExpression(QString const& expression)
{
    clear();
    QTextCursor cursor = textCursor();
    cursor.setPosition(0);

    QRegularExpression delimiter = wordDelimiter();
    int delimiterIndex = expression.indexOf(delimiter, 0);

    QString word = expression.mid(0, delimiterIndex);

    insertWord(word, cursor);

    while(delimiterIndex != -1)
    {
        int wordEndIndex = expression.indexOf(delimiter, delimiterIndex);

        QString term = expression.mid(delimiterIndex, wordEndIndex - delimiterIndex);
        insertWord(term, cursor);

        QString operation = delimiter.match(expression, delimiterIndex).captured();
        cursor.insertText(operation, colorFormat("black"));

        delimiterIndex = wordEndIndex + operation.length();
    }
    
    expression_ = expression;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::finishEditingCurrentWord()
//-----------------------------------------------------------------------------
void ExpressionEditor::finishEditingCurrentWord()
{
    QString finishedWord = currentWord();
    QString finishedTerm = nthWordIn(expression_, currentWordIndex());

    bool termIsReference = isReference(finishedTerm);
    bool referenceHasChanged = !termIsReference || parameterFinder_->nameForId(finishedTerm) != finishedWord;

    if (referenceHasChanged)
    {
        expression_ = replaceNthWordWith(expression_, currentWordIndex(), finishedWord);

        if (!finishedWord.isEmpty() && !wordIsConstant(finishedWord))
        {
            colorCurrentWordRed();
        }

        emit decreaseReference(finishedTerm);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ExpressionEditor::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->key() == Qt::Key_Return)
    {
        QWidget::keyPressEvent(keyEvent);
        return;
    }

    QString input = keyEvent->text();
    if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace || isWordDelimiter(input))
    {
        if (isReference(nthWordIn(expression_, currentWordIndex())))
        {
            int cursorPosition = textCursor().position();
            if (cursorPosition != startOfCurrentWord() && cursorPosition != endOfCurrentWord())
            {
                return;
            }
        }

        if (isWordDelimiter(input))
        {
            finishEditingCurrentWord();

            QString termAndDelimiter = nthWordIn(expression_, currentWordIndex()) + input;
            expression_ = replaceNthWordWith(expression_, currentWordIndex(), termAndDelimiter);

            QTextCursor cursor = textCursor();
            cursor.setCharFormat(colorFormat("black"));
            setTextCursor(cursor);
        }
    }

    QTextEdit::keyPressEvent(keyEvent);

    if (nameCompleter_)
    {
        if (!currentWord().isEmpty() || 
            (keyEvent->key() == Qt::Key_Space && (keyEvent->modifiers() & Qt::ControlModifier)))
        {
            nameCompleter_->complete();
        }
        else
        {
            nameCompleter_->popup()->hide();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::complete()
//-----------------------------------------------------------------------------
void ExpressionEditor::complete(QModelIndex const& index)
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

    expression_ = replaceNthWordWith(expression_, currentWordIndex(), parameterId);

    QTextCursor cursor = textCursor();
    cursor.setPosition(startOfCurrentWord());
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.insertText(parameterName, colorFormat("darkGreen"));

    disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::UniqueConnection);

    emit(increaseReference(parameterId));
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::onTextChanged()
//-----------------------------------------------------------------------------
void ExpressionEditor::onCursorPositionChanged()
{
    if(nameCompleter_)
    {
        nameCompleter_->setCompletionPrefix(currentWord());
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::wordDelimiter()
//-----------------------------------------------------------------------------
QRegularExpression ExpressionEditor::wordDelimiter() const
{
    return QRegularExpression("[+/*() ]|[-](?![a-fA-F0-9]+([-][a-fA-F0-9]+)*[}])");
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::insertWord()
//-----------------------------------------------------------------------------
void ExpressionEditor::insertWord(QString word, QTextCursor& cursor)
{
    if (isReference(word))
    {
        cursor.insertText(parameterFinder_->nameForId(word), colorFormat("darkGreen"));
    }
    else if(wordIsConstant(word))
    {
        cursor.insertText(word, colorFormat("black"));
    }
    else
    {
        cursor.insertText(word, colorFormat("red"));
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::isReference()
//-----------------------------------------------------------------------------
bool ExpressionEditor::isReference(QString const& text) const
{
    if (parameterFinder_.isNull())
    {
        return false;
    }

    return parameterFinder_->hasId(text);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::wordIsConstant()
//-----------------------------------------------------------------------------
bool ExpressionEditor::wordIsConstant(QString const& word) const
{
    QRegularExpression constant(SystemVerilogSyntax::INTEGRAL_NUMBER + "|" + SystemVerilogSyntax::STRING_LITERAL);
    return constant.match(word).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::colorFormat()
//-----------------------------------------------------------------------------
QTextCharFormat ExpressionEditor::colorFormat(QString const& textColor) const
{
    QTextCharFormat format;
    format.setForeground(QColor(textColor));
    return format;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::currentWord()
//-----------------------------------------------------------------------------
QString ExpressionEditor::currentWord() const
{
    return toPlainText().mid(startOfCurrentWord(), currentWordLength());
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::startOfCurrentWord()
//-----------------------------------------------------------------------------
int ExpressionEditor::startOfCurrentWord() const
{
    return toPlainText().lastIndexOf(wordDelimiter(), textCursor().position() - 1) + 1;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::endOfCurrentWord()
//-----------------------------------------------------------------------------
int ExpressionEditor::endOfCurrentWord() const
{
    int endPos = toPlainText().indexOf(wordDelimiter(), textCursor().position());

    if (endPos == -1)
    {
        endPos = toPlainText().length();
    }	

    return endPos;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::currentWordLength()
//-----------------------------------------------------------------------------
int ExpressionEditor::currentWordLength() const
{
    return endOfCurrentWord() - startOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::currentWordIndex()
//-----------------------------------------------------------------------------
int ExpressionEditor::currentWordIndex() const
{
    return toPlainText().left(textCursor().position()).count(wordDelimiter());
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::replaceNthWordWith()
//-----------------------------------------------------------------------------
QString ExpressionEditor::replaceNthWordWith(QString const& oldText, int n, QString const& after) const
{
    int startIndex = 0;
    for (int i = 0; i < n; i++)
    {
        startIndex = oldText.indexOf(wordDelimiter(), startIndex) + 1;
    }

    QString replaced = oldText;
    replaced.replace(startIndex, nthWordIn(oldText, n).length(), after);
    return replaced;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::nthWordIn()
//-----------------------------------------------------------------------------
QString ExpressionEditor::nthWordIn(QString const& text, int n) const
{
    int startIndex = 0;
    for (int i = 0; i < n; i++)
    {
        startIndex = text.indexOf(wordDelimiter(), startIndex) + 1;
    }

    int endIndex = text.indexOf(wordDelimiter(), startIndex);
    if (endIndex == -1)
    {
        endIndex = text.length();
    }

    int wordLength = endIndex - startIndex;
    return text.mid(startIndex, wordLength);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::colorCurrentWordRed()
//-----------------------------------------------------------------------------
void ExpressionEditor::colorCurrentWordRed()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.setCharFormat(colorFormat("red"));
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::isWordDelimiter()
//-----------------------------------------------------------------------------
bool ExpressionEditor::isWordDelimiter(QString const& text) const
{
    return wordDelimiter().match(text).hasMatch();
}
