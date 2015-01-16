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

class ParameterResolver;
//-----------------------------------------------------------------------------
//! Editor for expressions with parameter name completion.
//-----------------------------------------------------------------------------
class ExpressionEditor : public QTextEdit
{
    Q_OBJECT
public:

	//! The constructor.
    ExpressionEditor(QSharedPointer<ParameterResolver> resolver, QWidget *parent = 0);

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
     *  Gets the text to display to user. Reference ids are replaced with parameter names.
     *
     *      @return The human-readable text of the editor.
     */
    QString getDisplayText() const;

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

protected:

    //! Handler for key press events.
    virtual void keyPressEvent(QKeyEvent* keyEvent);

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
     *  Gets the delimiter for words.
     *
     *      @return The delimiter for words.
     */
    QRegularExpression wordDelimiter() const;

    /*!
     *  Finds the length of the current word.
     *
     *      @return The length of the current word.
     */
    int currentWordLength() const;
    
    /*!
     *  Checks if the current expression does not contain references.
     *
     *      @return True, if the expression has no references, otherwise false.
     */
    bool hasNoReferencesInExpression();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The completer whose selection is appended to the text.
    QCompleter* appendingCompleter_;

    //! Resolver for parameter names.
    QSharedPointer<ParameterResolver> parameterResolver_;

    //! The underlying expression for the editor.
    QString expression_;
};

#endif // EXPRESSIONEDITOR_H