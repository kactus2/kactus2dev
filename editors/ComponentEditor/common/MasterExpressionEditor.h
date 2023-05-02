//-----------------------------------------------------------------------------
// File: MasterExpressionEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.11.2022
//
// Description:
// Parent class for expression editors with parameter name completion.
//-----------------------------------------------------------------------------

#ifndef MASTEREXPRESSIONEDITOR_H
#define MASTEREXPRESSIONEDITOR_H

#include <QCompleter>
#include <QWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTextCursor>

class ParameterFinder;

//-----------------------------------------------------------------------------
//! Parent class for expression editors with parameter name completion.
//-----------------------------------------------------------------------------
class MasterExpressionEditor
{

public:

	/*!
     *  The constructor.
     */
    MasterExpressionEditor(QSharedPointer<ParameterFinder> parameterFinder, QWidget* parentWidget);

	/*!
     *  The destructor.
     */
	virtual ~MasterExpressionEditor() = default;

    /*!
     *  Sets a completer whose selection will be appended to the text.
     *
     *      @param [in] completer   The completer to set.
     */
    virtual void setAppendingCompleter(QCompleter* completer);

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
    virtual void setExpression(QString const& expression);

    /*!
     *  Ends the editing of current word and commits it to the expression.
     */
    void finishEditingCurrentWord();

    /*!
     *  Set the reserved words.
     *
     *      @param [in] newReservations     A list of the new reserved words.
     */
    void setReservedWords(QStringList newReservations);

protected:


    /*!
     *  Captures the user input and edits the underlying expression accordingly.
     *
     *      @param [in] keyEvent   The event representing the user input.
     */
    void handleKeyPressEvent(QKeyEvent* keyEvent);

    /*!
     *  Captures the mouse move and sets/clears notSelectingText flag according to mouse press.
     *
     *      @param [in] mouseEvent   The mouse event representing the mouse movement and button press.
     */
    void handleMouseMoveEvent(QMouseEvent* mouseEvent);

    /*!
     *  Clears notSelectingText flag when the mouse button is released.
     *
     *      @param [in] mouseEvent   The event representing the mouse button release.
     */
    void handleMouseReleaseEvent();

    /*!
     *  Emits a signal informing of the ending of editing the expression.
     *
     *      @param [in] event   The event representing the changing of focus.
     */
    void updateCompleteWords();

    /*!
     *  Called when a word completion has been selected. Replaces the word under the cursor with the selection.
     *
     *      @param [in] word   The selected word.
     */
    void complete(QModelIndex const& index);

    /*!
     *  Gives a format for a given text color.
     *
     *      @param [in] textColor   The name of the color in the format.
     *
     *      @return The format for the given color.
     */
    QTextCharFormat colorFormat(QString const& textColor) const;

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
     *  Gives the index of the word currently under the cursor.
     *
     *      @return The index of the word.
     */
    int currentWordIndex() const;

    /*!
     *  Gets the delimiter for words.
     *
     *      @return The delimiter for words.
     */
    QRegularExpression wordDelimiter() const;

    /*!
     *  Checks if a given text is a reference to a parameter.
     *
     *      @param [in] text   The text to check.
     *
     *      @return True, if the text is a reference, otherwise false.
     */
    bool isReference(QString const& text) const;

    /*!
     *  Find the name for the parameter with the selected ID.
     *
     *      @param [in] parameterID     The selected parameter ID.
     *
     *      @return Name of the selected parameter.
     */
    QString getNameForParameterID(QString const& parameterID) const;

    /*!
     *  Get the list of reserved words.
     *
     *      @return The list of reserved words.
     */
    QStringList getReservedWords() const;

    /*!
     *  Checks if a given word is a constant value.
     *
     *      @param [in] word   The word to check.
     *
     *      @return True, if the word is a constant, otherwise false.
     */
    bool wordIsConstant(QString const& word) const;

    /*!
     *  Finds the index for the start of the current word.
     *
     *      @return The index for the start of the current word.
     */
    int startOfCurrentWord() const;

private:

    // Disable copying.
    MasterExpressionEditor(MasterExpressionEditor const& rhs);
    MasterExpressionEditor& operator=(MasterExpressionEditor const& rhs);

    /*!
     *  Check if a selection exists.
     *
     *      @return True, if a selection exists, false otherwise.
     */
    virtual bool hasSelection() = 0;

    /*!
     *  Let the parent widget handle the key press event.
     *
     *      @param [in] keyEvent    The selected key press event.
     */
    virtual void handleParentKeyPressEvent(QKeyEvent* keyEvent) = 0;

    /*!
     *  Decrease references made to the selected ID.
     *
     *      @param [in] referenceItem   ID of the selected item.
     */
    virtual void handleReferenceDecrease(QString const& referenceItem) = 0;
    
    /*!
     *  Increase references made to the selected ID.
     *
     *      @param [in] referenceItem   ID of the selected item.
     */
    virtual void handleReferenceIncrease(QString const& referenceItem) = 0;

    /*!
     *  Set the completed parameter name for the editor.
     *
     *      @param [in] parameterName   The completed parameter name.
     */
    virtual void setCompletedParameterName(QString const& parameterName) = 0;

    /*!
     *  Get the current cursor position.
     *
     *      @return The position of the cursor.
     */
    virtual int getCursorPosition() const = 0;

    /*!
     *  Get the current text of the editor.
     *
     *      @return The current text.
     */
    virtual QString getCurrentText() const = 0;

    /*!
     *  Get the selected text.
     *
     *      @return The selected text.
     */
    virtual QString getSelectedText() const = 0;

    /*!
     *  Count the length of the first word in the current selection.
     *
     *      @return Length of the first word in the selection.
     */
    virtual int getSelectionFirstWord() const = 0;

    /*!
     *  Count the length of the last word in the current selection.
     *
     *      @return Length of the last word in the selection.
     */
    virtual int getSelectionLastWord() const = 0;

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
    virtual void colorCurrentWordRed() = 0;

    /*!
     *  Changes the color of the font for the current word to black.
     */
    virtual void colorCurrentWordBlack() = 0;

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
     *  Checks if the given event will copy, cut or paste text.
     *
     *      @param [in] keyEvent    The event to check.
     *
     *      @return True, if the event will copy, cut or paste text, otherwise false.
     */
    bool keysequenceCopyCutPaste(QKeyEvent* keyEvent);

    /*!
     *  Checks if the given event will move the cursor.
     *
     *      @param [in] keyEvent    The event to check.
     *
     *      @return True, if the event will move the cursor, otherwise false.
     */
    bool keyMovesCursor(QKeyEvent* keyEvent);

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
    virtual void finishEditingCurrentTerm(QString const& delimiter);
    
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

    //! The list of the reserved words for this editor.
    QStringList reservedWords_;

};

#endif // MASTEREXPRESSIONEDITOR_H