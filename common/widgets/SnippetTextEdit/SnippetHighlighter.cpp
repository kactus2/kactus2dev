//-----------------------------------------------------------------------------
// File: SnippetHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 22.08.2012
//
// Description:
// Highlighter for snippet magic words.
//-----------------------------------------------------------------------------

#include "SnippetHighlighter.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: SnippedHighlighter::SnippedHighlighter()
//-----------------------------------------------------------------------------
SnippetHighlighter::SnippetHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: SnippedHighlighter::~SnippedHighlighter()
//-----------------------------------------------------------------------------
SnippetHighlighter::~SnippetHighlighter()
{

}

//-----------------------------------------------------------------------------
// Function: SnippedHighlighter::addMagicWords()
//-----------------------------------------------------------------------------
void SnippetHighlighter::addMagicWords(QStringList const& magicWords)
{
    magicWords_.append(magicWords);
}

//-----------------------------------------------------------------------------
// Function: SnippedHighlighter::clearMagicWords()
//-----------------------------------------------------------------------------
void SnippetHighlighter::clearMagicWords()
{
    magicWords_.clear();
}

//-----------------------------------------------------------------------------
// Function: SnippedHighlighter::highlightBlock()
//-----------------------------------------------------------------------------
void SnippetHighlighter::highlightBlock(QString const& text)
{
    foreach (QString const& word, magicWords_)
    {
        QRegularExpression expression(QString("\\$") + word + QString("(_\\d+)?\\$"));
        
        QRegularExpressionMatch match = expression.match(text);
        int index = match.capturedStart();

        while (index >= 0)
        {
            int length = match.capturedLength();
            setFormat(index, length, Qt::blue);

            match = expression.match(text, index + length);
            index = match.capturedStart();
        }
    }
}
