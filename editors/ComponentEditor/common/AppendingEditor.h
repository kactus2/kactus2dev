//-----------------------------------------------------------------------------
// File: AppendingEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// LineEdit with appending word completion.
//-----------------------------------------------------------------------------

#ifndef APPENDINGEDITOR_H
#define APPENDINGEDITOR_H

#include <QLineEdit>

//-----------------------------------------------------------------------------
//! LineEdit with appending word completion.
//-----------------------------------------------------------------------------
class AppendingEditor : public QLineEdit
{
    Q_OBJECT
public:

	//! The constructor.
    AppendingEditor(QWidget *parent = 0);

	//! The destructor.
	virtual ~AppendingEditor();

    /*!
     *  Sets a completer whose selection will be appended to the text.
     *
     *      @param [in] completer   The completer to set.
     */
    void setAppendingCompleter(QCompleter* completer);

protected:

    //! Handler for key press events.
    virtual void keyPressEvent(QKeyEvent* keyEvent);

private slots:

    /*!
     *  Called when a word completion has been selected. Replaces the word under the cursor with the selection.
     *
     *      @param [in] word   The selected word.
     */
    void complete(QString word);

private:

    // Disable copying.
    AppendingEditor(AppendingEditor const& rhs);
    AppendingEditor& operator=(AppendingEditor const& rhs);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The completer whose selection is appended to the text.
    QCompleter* appendingCompleter_;
};

#endif // APPENDINGEDITOR_H