//-----------------------------------------------------------------------------
// File: AppendingEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// LineEdit with appending word completion.
//-----------------------------------------------------------------------------

#include "AppendingEditor.h"

#include <QEvent>
#include <QLineEdit>
#include <QKeyEvent>
#include <QCompleter>
#include <QAbstractItemView>

//-----------------------------------------------------------------------------
// Function: AppendingEditor::AppendingEditor()
//-----------------------------------------------------------------------------
AppendingEditor::AppendingEditor(QWidget *parent)
    : QLineEdit(parent), appendingCompleter_(completer())
{
    
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::~AppendingEditor()
//-----------------------------------------------------------------------------
AppendingEditor::~AppendingEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::setAppendingCompleter()
//-----------------------------------------------------------------------------
void AppendingEditor::setAppendingCompleter(QCompleter* completer)
{
    appendingCompleter_ = completer;

    appendingCompleter_->setWidget(this);
    connect(appendingCompleter_, SIGNAL(activated(QString)), this, SLOT(complete(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void AppendingEditor::keyPressEvent(QKeyEvent* keyEvent)
{
    QLineEdit::keyPressEvent(keyEvent);

    QString word = currentWord();

    if (appendingCompleter_)
    {
        appendingCompleter_->setCompletionPrefix(word);

        if (!word.isEmpty() || (keyEvent->key() == Qt::Key_Space && (keyEvent->modifiers() & Qt::ControlModifier)))
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
// Function: AppendingEditor::complete()
//-----------------------------------------------------------------------------
void AppendingEditor::complete(QString word)
{
    int startAt = startOfCurrentWord();

    setText(text().replace(startAt,  currentWordLength(), word));
    setCursorPosition(startAt + word.length());
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::currentWord()
//-----------------------------------------------------------------------------
QString AppendingEditor::currentWord() const
{
    return text().mid(startOfCurrentWord(), currentWordLength());
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::startOfCurrentWord()
//-----------------------------------------------------------------------------
int AppendingEditor::startOfCurrentWord() const
{
    return text().lastIndexOf(wordDelimiter(), cursorPosition() - 1) + 1;
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::endOfCurrentWord()
//-----------------------------------------------------------------------------
int AppendingEditor::endOfCurrentWord() const
{
    int endPos = text().indexOf(wordDelimiter(), cursorPosition());

    if (endPos == -1)
    {
        endPos = text().length();
    }	

    return endPos;
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::wordDelimiter()
//-----------------------------------------------------------------------------
QRegularExpression AppendingEditor::wordDelimiter() const
{
     return QRegularExpression("[+-/*() ]");
}

//-----------------------------------------------------------------------------
// Function: AppendingEditor::currentWordLength()
//-----------------------------------------------------------------------------
int AppendingEditor::currentWordLength() const
{
    return endOfCurrentWord() - startOfCurrentWord();
}
