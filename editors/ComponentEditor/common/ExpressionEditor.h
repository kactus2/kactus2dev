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

    /*!
     *  Captures the user input and edits the underlying expression accordingly.
     *
     *      @param [in] keyEvent   The event representing the user input.
     */
    virtual void keyPressEvent(QKeyEvent* keyEvent);

    /*!
     *  Selects the text when the editor is opened.
     *
     *      @param [in] focusEvent   The event representing the focus change.
     */
    virtual void focusInEvent(QFocusEvent* focusEvent);

    /*!
     *  Captures the mouse move and sets/clears notSelectingText flag according to mouse press.
     *
     *      @param [in] mouseEvent   The mouse event representing the mouse movement and button press.
     */
    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);

    /*!
     *  Clears notSelectingText flag when the mouse button is released.
     *
     *      @param [in] mouseEvent   The event representing the mouse button release.
     */
    virtual void mouseReleaseEvent(QMouseEvent* mouseEvent);

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
    void updateCompletions();

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
     *  Checks if the editing would change text in the middle of a referencing term.
     *
     *      @return True, if the edit position is in the middle of a referencing term, otherwise false.
     */
    bool editingMiddleOfReference() const;

    /*!
     *  Checks if the given key event will remove the last character in a word.
     *
     *      @param [in] keyEvent   The key event to check.
     *
     *      @return True, if the event will remove the last character, otherwise false.
     */
    bool removesLastCharacterOfWord(QKeyEvent* keyEvent);
    
    /*!
     *  Removes the term currently under cursor.
     */
    void removeTermUnderCursor();
    
    /*!
     *  Checks if the given key event will remove an operator before the cursor.
     *
     *      @param [in] keyEvent   The key event to check.
     *
     *      @return True, if the event will remove an operator, otherwise false.
     */
    bool removesOperatorBeforeWord(QKeyEvent* keyEvent);

    /*!
     *  Removes an operator in the expression in front of the cursor.
     */
    void removeOperatorBeforeCursorInExpression();
        
    /*!
     *  Checks if the given key event will remove an operator after the cursor.
     *
     *      @param [in] keyEvent   The key event to check.
     *
     *      @return True, if the event will remove an operator, otherwise false.
     */
    bool removesOperatorAfterCursor(QKeyEvent* keyEvent);
        
    /*!
     *  Removes an operator in the expression after the cursor.
     */
    void removeOperatorAfterCursorInExpression();

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
     *      @param [in] n           The index of the word to replace i.e 1 for second word in a sentence.
     *      @param [in] oldText     The text to replace the word in.
     *      @param [in] after       The word used to replace the old word.
     *
     *      @return A text where the nth word has been replaced.
     */
    QString replaceNthWordWith(int n, QString const& oldText, QString const& after) const;

    /*!
     *  Finds the nth word in a text.
     *
     *      @param [in] n       The word index to find.
     *      @param [in] text    The text to search in.
     *
     *      @return The nth word in a text.
     */
    QString nthWordIn(int n, QString const& text) const;

    /*!
     *  Finds the character index of the beginning of nth word in a text.
     *
     *      @param [in] n       The word to search.
     *      @param [in] text    The text to search the word in.
     *
     *      @return The index at the beginning of the nth word.
     */
    int indexOfNthWord(int n, QString const& text) const;

    /*!
     * Checks if the current word is unambiguous name of a parameter.
     *
     *      @return True, if the word is unambiguous parameter name, otherwise false.
     */
    bool currentWordIsUniqueParameterName();

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

    /*!
     *  Checks if the given key event will remove or replace text.
     *
     *      @param [in] keyEvent   The event to check.
     *
     *      @return True, if the event will remove or replace text, otherwise false.
     */
    bool removesOrReplacesText(QKeyEvent* keyEvent);

    /*!
     *  Removes the current selection in the underlying expression.
     */
    void removeSelectionInExpression();

    /*!
     *  Replaces the references in the underlying expression with the referenced names.
     *
     *      @param [in] firstWord   The sequence number of the first word to replace.
     *      @param [in] lastWord    The sequence number of the last word to replace.
     */
    void replaceReferencesInExpressionWithNames(int firstWord, int lastWord);

    /*!
     *  Ends the editing of current term in the underlying expression with the given word delimiter.
     *
     *      @param [in] delimiter   The word delimiter ending the term edit.
     */
    void finishEditingCurrentTerm(QString delimiter);
    
    /*!
     *  Checks if the given key event represents shortcut for showing all available completions.
     *
     *      @param [in] keyEvent   The key event to check.
     *
     *      @return True, if the key event requests showing completions, otherwise false.
     */
    bool showCompletionsRequested(QKeyEvent* keyEvent);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The completer whose selection is used in the text.
    QCompleter* nameCompleter_;

    //! Resolver for parameter names.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The underlying expression for the editor.
    QString expression_;
    
    //! Flag for indicating that the user is not selecting text with the mouse.
    bool notSelectingText_;
};

#endif // EXPRESSIONEDITOR_H