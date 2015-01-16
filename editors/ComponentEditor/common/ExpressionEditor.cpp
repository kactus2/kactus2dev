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
        if (parameterResolver_->hasId(term))
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

    QTextEdit::keyPressEvent(keyEvent);

    if (hasNoReferencesInExpression())
    {
        expression_ = toPlainText();
    }
    else
    {
        expression_.append(keyEvent->text());
    }

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
    
    int startAt = startOfCurrentWord();

    QString oldText = toPlainText();
    int len = currentWordLength();

    setText(toPlainText().replace(startAt, len, parameterName));

    disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    QTextCursor cursor = textCursor();
    cursor.setPosition(startAt + parameterName.length());
    setTextCursor(cursor);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()), Qt::UniqueConnection);

    expression_ = oldText.replace(startAt, len, parameterId);
}

//-----------------------------------------------------------------------------
// Function: ExpressionEditor::onTextChanged()
//-----------------------------------------------------------------------------
void ExpressionEditor::onCursorPositionChanged()
{
    QString word = currentWord();

    if(appendingCompleter_)
    {
        appendingCompleter_->setCompletionPrefix(word);
    }

    //expression_ = toPlainText();
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
     return QRegularExpression("[+-/*() ]");
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