//-----------------------------------------------------------------------------
// File: SnippetHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.08.2012
//
// Description:
// Highlighter for snippet magic words.
//-----------------------------------------------------------------------------

#ifndef SNIPPETHIGHLIGHTER_H
#define SNIPPETHIGHLIGHTER_H

#include <QSyntaxHighlighter>

//-----------------------------------------------------------------------------
//! Highlighter for snippet magic words.
//-----------------------------------------------------------------------------
class SnippetHighlighter : public QSyntaxHighlighter
{
public:
    /*!
     *  Constructor.
     */
    SnippetHighlighter(QTextDocument* parent);

    /*!
     *  Destructor.
     */
    ~SnippetHighlighter();

    /*!
     *  Adds magic words.
     *
     *      @param [in] magicWords The magic words to add.
     */
    void addMagicWords(QStringList const& magicWords);

    /*!
     *  Clears the magic words.
     */
    void clearMagicWords();

protected:
    /*!
     *  Highlights a block of text.
     */
    void highlightBlock(QString const& text);

private:
    // Disable copying.
    SnippetHighlighter(SnippetHighlighter const& rhs);
    SnippetHighlighter& operator=(SnippetHighlighter const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Magic words to highlight.
    QStringList magicWords_;
};

#endif // SNIPPETHIGHLIGHTER_H
