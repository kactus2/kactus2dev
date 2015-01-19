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

#include <editors/ComponentEditor/common/ParameterResolver.h>
#include <editors/ComponentEditor/parameters/ComponentParameterColumns.h>

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::ExpressionEditor()
//-----------------------------------------------------------------------------
ExpressionEditor::ExpressionEditor(QSharedPointer<ParameterResolver> resolver, QWidget *parent)
    : QTextEdit(parent), appendingCompleter_(0), parameterResolver_(resolver)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTabChangesFocus(true);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::UniqueConnection);
    connect(document(), SIGNAL(contentsChange(int, int, int)), 
        this, SLOT(onTextChanged(int, int, int)), Qt::UniqueConnection);
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
    appendingCompleter_ = completer;

    appendingCompleter_->setWidget(this);
    connect(appendingCompleter_, SIGNAL(activated(QModelIndex const&)), 
        this, SLOT(complete(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::completer()
//-----------------------------------------------------------------------------
QCompleter* ExpressionEditor::completer() const
{
    return appendingCompleter_;
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::getDisplayText()
//-----------------------------------------------------------------------------
QString ExpressionEditor::getDisplayText() const
{
    return toPlainText();
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
    QStringList terms = expression.split(wordDelimiter(), QString::SkipEmptyParts);
    QString replaced = expression;
    foreach(QString term, terms)
    {
        if (parameterResolver_ && parameterResolver_->hasId(term))
        {
            replaced.replace(term, parameterResolver_->nameForId(term));
        }
    }

    setPlainText(replaced);
    
    expression_ = expression;
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

    //    setExpression(replaceNthWordWith(expression_, currentWordIndex(), currentWord()));
    
    if (keyEvent->key() == Qt::Key_Space)
    {
        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::WordUnderCursor);

        QString word = cursor.selectedText();
        QTextCharFormat redFormat;
        redFormat.setForeground(QBrush(Qt::red));
        cursor.setCharFormat(redFormat);
    }

    QTextEdit::keyPressEvent(keyEvent);
    
    if (appendingCompleter_)
    {
        if (!currentWord().isEmpty() || 
            (keyEvent->key() == Qt::Key_Space && (keyEvent->modifiers() & Qt::ControlModifier)))
        {
            appendingCompleter_->complete();
        }
        else 
        {
            appendingCompleter_->popup()->hide();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::complete()
//-----------------------------------------------------------------------------
void ExpressionEditor::complete(QModelIndex const& index)
{
    int selectedRow = index.row();

    QModelIndex nameIndex = appendingCompleter_->completionModel()->index(selectedRow, ComponentParameterColumns::NAME);
    QString parameterName = nameIndex.data().toString();

    QModelIndex idIndex = appendingCompleter_->completionModel()->index(selectedRow, ComponentParameterColumns::ID);
    QString parameterId = idIndex.data().toString();
   
    if (parameterId.isEmpty())
    {
        parameterId = parameterName;
    }

    emit increaseReference(parameterId);
    
    int startAt = startOfCurrentWord();

    int nthWord = currentWordIndex();

    expression_ = replaceNthWordWith(expression_, nthWord, parameterId);

    disconnect(document(), SIGNAL(contentsChange(int, int, int)), this, SLOT(onTextChanged(int, int, int)));
    setText(replaceNthWordWith(toPlainText(), nthWord, parameterName));
    connect(document(), SIGNAL(contentsChange(int, int, int)), 
        this, SLOT(onTextChanged(int, int, int)), Qt::UniqueConnection);

    disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    QTextCursor cursor = textCursor();
    cursor.setPosition(startAt + parameterName.length());
    setTextCursor(cursor);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::onTextChanged()
//-----------------------------------------------------------------------------
void ExpressionEditor::onCursorPositionChanged()
{
    if(appendingCompleter_)
    {
        appendingCompleter_->setCompletionPrefix(currentWord());
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::onTextChanged()
//-----------------------------------------------------------------------------
void ExpressionEditor::onTextChanged(int position, int charsRemoved, int charsAdded)
{
    if (hasNoReferencesInExpression())
    {
        expression_ = toPlainText();
    }
    else
    {

        expression_.append(toPlainText().mid(position, charsAdded));
    }
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
// Function: ExpressionEditor::wordDelimiter()
//-----------------------------------------------------------------------------
QRegularExpression ExpressionEditor::wordDelimiter() const
{
    return QRegularExpression("[+/*() ]|[-](?![a-fA-F0-9]+([-][a-fA-F0-9]+)*[}])");
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::currentWordLength()
//-----------------------------------------------------------------------------
int ExpressionEditor::currentWordLength() const
{
    return endOfCurrentWord() - startOfCurrentWord();
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::hasNoReferencesInExpression()
//-----------------------------------------------------------------------------
bool ExpressionEditor::hasNoReferencesInExpression()
{
    if (parameterResolver_.isNull())
    {
        return true;
    }

    QStringList terms = expression_.split(wordDelimiter(), QString::SkipEmptyParts);
    foreach(QString term, terms)
    {
        if (parameterResolver_->hasId(term))
        {
            return false;
        }
    }

    return true;
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
// Function: ExpressionEditor::currentWordIndex()
//-----------------------------------------------------------------------------
int ExpressionEditor::currentWordIndex()
{
    return toPlainText().left(textCursor().position()).count(wordDelimiter());
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
// Function: ExpressionEditor::isReference()
//-----------------------------------------------------------------------------
bool ExpressionEditor::isReference(QString const& text) const
{
    if (parameterResolver_.isNull())
    {
        return false;
    }

    return parameterResolver_->hasId(text);
}