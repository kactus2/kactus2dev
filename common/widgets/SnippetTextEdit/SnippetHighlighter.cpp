//-----------------------------------------------------------------------------
// File: SnippetHighlighter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 22.08.2012
//
// Description:
// Highlighter for snippet magic words.
//-----------------------------------------------------------------------------

#include "SnippetHighlighter.h"

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
        QRegExp expression(QString("\\$") + word + QString("\\$"));
        int index = expression.indexIn(text);

        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, Qt::blue);
            index = expression.indexIn(text, index + length);
        }
    }
}
