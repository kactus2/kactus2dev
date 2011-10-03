//-----------------------------------------------------------------------------
// File: AssistedTextEdit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.12.2010
//
// Description:
// Text edit widget which offers content assist.
//-----------------------------------------------------------------------------

#ifndef ASSISTEDTEXTEDIT_H
#define ASSISTEDTEXTEDIT_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QSharedPointer>

class ITextContentMatcher;
class TextContentAssistWidget;

//-----------------------------------------------------------------------------
//! AssistedTextEdit class.
//-----------------------------------------------------------------------------
class AssistedTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! IndentStyle enumeration.
    //-----------------------------------------------------------------------------
    enum IndentStyle
    {
        INDENT_STYLE_TAB = 0,
        INDENT_STYLE_SPACES
    };

    /*!
     *  Constructor.
     *
     *      @param [in] contentMatcher  The content matcher for the content assist.
     *      @param [in] mainWnd         The program main window. Must not be null.
     *      @param [in] parent          The parent widget. Can be null.
     */
    AssistedTextEdit(QSharedPointer<ITextContentMatcher> contentMatcher,
                     QWidget* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~AssistedTextEdit();

    /*!
     *  Sets the indentation style.
     *
     *      @param [in] style The indentation style.
     *      @param [in] width The indentation width.
     */
    void setIndentStyle(IndentStyle style, unsigned int width);

    /*!
     *  Returns the indent style.
     */
    IndentStyle getIndentStyle() const;

    /*!
     *  Returns the indent width.
     */
    unsigned int getIndentWidth() const;

    /*!
     *  Filters events.
     *
     *      @param [in] obj  The event owner.
     *      @param [in] e    The event.
     */
    bool eventFilter(QObject* obj, QEvent* e);
    
protected:
    /*!
     *  Handles the given key press event.
     *
     *      @param [in] e The key event.
     */
    void keyPressEvent(QKeyEvent* e);

    /*!
     *  Handles the given mouse press event.
     *
     *      @param [in] e The mouse press event.
     */
    void mousePressEvent(QMouseEvent* e);

    /*!
     *  Handles the focus out event.
     */
    void focusOutEvent(QFocusEvent* e);

private:
    // Disable copying.
    AssistedTextEdit(AssistedTextEdit const& rhs);
    AssistedTextEdit& operator=(AssistedTextEdit const& rhs);

    /*!
     *  Creates a correct indentation string based on the indentation style.
     *
     *      @param [in] depth The indentation depth, that is, the number of indentations.
     */
    QString createIndentString(unsigned int depth);

    /*!
     *  Determines the correct indentation depth for the given position.
     *
     *      @param [in] The position index.
     */
    int calculateIndentDepth(int pos);

    /*!
     *  Indents the right brace correctly.
     */
    void indentRightBrace();

    /*!
     *  Indents the colon correctly.
     */
    void indentColon();

    /*!
     *  Replaces a substring in the text with the given indentation.
     *
     *      @param [in] startIndex   The start index of the substring.
     *      @param [in] endIndex     The end index of the substring (excluded from the substring).
     *      @param [in] indentDepth  The desired indentation depth.
     */
    void replaceWithIndent(int startIndex, int endIndex, int indentDepth);

    /*!
     *  Handles the Return key press event.
     *
     *      @param [in] e The Return key press event.
     */
    void onReturnKeyPress(QKeyEvent* e);

    //! Default indentation width.
    static unsigned int const DEFAULT_INDENT_WIDTH = 4;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Content assist list widget.
    QSharedPointer<TextContentAssistWidget> contentAssist_;

    //! The indentation style.
    IndentStyle indentStyle_;

    //! The indentation width in characters.
    unsigned int indentWidth_;
};

//-----------------------------------------------------------------------------

#endif // ASSISTEDTEXTEDIT_H
