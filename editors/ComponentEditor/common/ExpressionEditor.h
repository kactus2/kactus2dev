//-----------------------------------------------------------------------------
// File: ExpressionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// Editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONEDITOR_H
#define EXPRESSIONEDITOR_H

#include <QCompleter>
#include <QTextEdit>

class ParameterFinder;
//-----------------------------------------------------------------------------
//! Editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------
class ExpressionEditor : public QTextEdit
{
    Q_OBJECT
public:

	//! The constructor.
    ExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent = 0);

	//! The destructor.
	virtual ~ExpressionEditor();

    /*!
     *  Sets a completer whose selection will be appended to the text.
     *
     *      @param [in] completer   The completer to set.
     */
    void setAppendingCompleter(QCompleter* completer);

    /*!
     *  Gets the completer for the widget.
     *
     *      @return The completer of the widget.
     */
    QCompleter* completer() const;

    /*!
     *  Gets the underlying expression in the editor.
     *
     *      @return The expression in the editor.
     */
    QString getExpression() const;
    
    /*!
     *  Sets the expression in the editor.
     *
     *      @param [in] expression   The expression to set.
     */
    void setExpression(QString const& expression);

    /*!
     *  Ends the editing of current word and commits it to the expression.
     */
    void finishEditingCurrentWord();

protected:

    //! Handler for key press events.
    virtual void keyPressEvent(QKeyEvent* keyEvent);

signals:

    /*!
     *  Increase the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] parameterId     The id of the parameter being referenced.
     */
    void increaseReference(QString const& parameterId);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the id.
     *
     *      @param [in] parameterId     The id of the parameter being referenced.
     */
    void decreaseReference(QString const& parameterId);

private slots:

    /*!
     *  Called when a word completion has been selected. Replaces the word under the cursor with the selection.
     *
     *      @param [in] word   The selected word.
     */
    void complete(QModelIndex const& index);

    /*!
     *  Called when the cursor position changes. If a different word is under the new position, it is suggested
     *  for new completion.
     *
     */
    void onCursorPositionChanged();

private:

    // Disable copying.
    ExpressionEditor(ExpressionEditor const& rhs);
    ExpressionEditor& operator=(ExpressionEditor const& rhs);

    /*!
     *  Gets the delimiter for words.
     *
     *      @return The delimiter for words.
     */
    QRegularExpression wordDelimiter() const;

    /*!
     *  Inserts a given word into the text using the given cursor.
     *
     *      @param [in] word    The word to insert into the text.
     *      @param [in] cursor  The cursor to use to insert the text.
     */
    void insertWord(QString word, QTextCursor& cursor);

    /*!
     *  Checks if a given text is a reference to a parameter.
     *
     *      @param [in] text   The text to check.
     *
     *      @return True, if the text is a reference, otherwise false.
     */
    bool isReference(QString const& text) const;

    /*!
     *  Checks if a given word is a constant value.
     *
     *      @param [in] word   The word to check.
     *
     *      @return True, if the word is a constant, otherwise false.
     */
    bool wordIsConstant(QString const& word) const;

    /*!
     *  Gives a format for a given text color.
     *
     *      @param [in] textColor   The name of the color in the format.
     *
     *      @return The format for the given color.
     */
    QTextCharFormat colorFormat(QString const& textColor) const;

    /*!
     *  Finds the word currently under cursor.
     *
     *      @return The word under cursor.
     */
    QString currentWord() const;

    /*!
     *  Finds the index for the start of the current word.
     *
     *      @return The index for the start of the current word.
     */
    int startOfCurrentWord() const;
    
    /*!
     *  Finds the index for the end of the current word.
     *
     *      @return The index for the end of the current word.
     */
    int endOfCurrentWord() const;

    /*!
     *  Finds the length of the current word.
     *
     *      @return The length of the current word.
     */
    int currentWordLength() const;

    /*!
     *  Gives the index of the word currently under the cursor.
     *
     *      @return The index of the word.
     */
    int currentWordIndex() const;

    /*!
     *  Replaces a word in a given index with another.
     *
     *      @param [in] oldText     The text to replace the word in.
     *      @param [in] n           The index of the word to replace i.e 1 for second word in a sentence.
     *      @param [in] after       The word used to replace the old word.
     *
     *      @return A text where the nth word has been replaced.
     */
    QString replaceNthWordWith(QString const& oldText, int n, QString const& after) const;

    /*!
     *  Finds the nth word in a text.
     *
     *      @param [in] text    The text to search in.
     *      @param [in] n       The word index to find.
     *
     *      @return The nth word in a text.
     */
    QString nthWordIn(QString const& text, int n) const;

    /*!
     *  Changes the color of the font for the current word to red.
     */
    void colorCurrentWordRed();

    /*!
     *  Checks if the given text is a word delimiter.
     *
     *      @param [in] text   The text to check.
     *
     *      @return True, if the text is a word delimiter, otherwise false.
     */
    bool isWordDelimiter(QString const& text) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The completer whose selection is used in the text.
    QCompleter* nameCompleter_;

    //! Resolver for parameter names.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The underlying expression for the editor.
    QString expression_;
};

#endif // EXPRESSIONEDITOR_H