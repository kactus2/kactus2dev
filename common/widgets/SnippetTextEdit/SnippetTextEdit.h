//-----------------------------------------------------------------------------
// File: SnippetTextEdit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.08.2012
//
// Description:
// Text edit for writing snippets that can use magic words.
//-----------------------------------------------------------------------------

#ifndef SNIPPETTEXTEDIT_H
#define SNIPPETTEXTEDIT_H

#include <QPlainTextEdit>
#include <QMenu>

class SnippetHighlighter;

//-----------------------------------------------------------------------------
//! Text edit for writing snippets that can use magic words.
//-----------------------------------------------------------------------------
class SnippetTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    SnippetTextEdit(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~SnippetTextEdit();

    /*!
     *  Adds the given magic words available for use.
     *
     *      @param [in] magicWords  The magic words to add.
     *      @param [in] category    The category name for the words. Empty if uncategorized.
     */
    void addMagicWords(QStringList const& magicWords, QString const& category = QString(""));

    /*!
     *  Removes all magic words from use.
     */
    void clearMagicWords();

private slots:
    /*!
     *  Shows a custom context menu.
     *
     *      @param [in] pt The upper-left corner coordinate for the context menu.
     */
    void showContextMenu(QPoint const& pt);

    /*!
     *  Insert the magic word indicated by the given action to the current cursor location in the text.
     */
    void insertMagicWord(QAction* action);

private:
    // Disable copying.
    SnippetTextEdit(SnippetTextEdit const& rhs);
    SnippetTextEdit& operator=(SnippetTextEdit const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The syntax highlighter.
    SnippetHighlighter* highlighter_;

    //! The magic words map.
    QMap<QString, QStringList> magicWordsMap_;
};

//-----------------------------------------------------------------------------

#endif // SNIPPETTEXTEDIT_H
