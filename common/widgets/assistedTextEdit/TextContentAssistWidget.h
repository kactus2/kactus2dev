//-----------------------------------------------------------------------------
// File: TextContentAssistWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.1.2011
//
// Description:
// Content assist widget.
//-----------------------------------------------------------------------------

#ifndef CONTENTASSISTWIDGET_H
#define CONTENTASSISTWIDGET_H

#include <QListWidget>
#include <QPlainTextEdit>
#include <QSharedPointer>

#include "ITextContentMatcher.h"
#include "ToolTipHintWidget.h"

//-----------------------------------------------------------------------------
//! TextContentAssistWidget class.
//-----------------------------------------------------------------------------
class TextContentAssistWidget : public QListWidget
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent   The parent text editor. Must not be null.
     *      @param [in] matcher  The content matcher to use. Must not be null.
     */
    TextContentAssistWidget(QPlainTextEdit* parent, QSharedPointer<ITextContentMatcher> matcher);

    /*!
     *  Destructor.
     */
    ~TextContentAssistWidget();

    /*!
     *  Tries to handle the given key press fully, committing the selection if possible.
     *
     *      @return True, if the key was handle fully and the editor should not handle it.
     *              False, if the editor still needs to handle the key.
     *
     *      @remarks Assumes that the given key event has not yet been handled by the editor.
     */
    bool tryHandleKey(QKeyEvent* e);

    /*!
     *  Updates the contents of the content assist.
     *
     *      @remarks Assumes that the given key event has already been handled by the editor.
     */
    void updateAssist(QKeyEvent* e);

    void hideAssist();

    /*!
     *  Cancels the content assist usage.
     */
    void cancel();

    /*!
     *  Sets the maximum number of items to be visible at once.
     *
     *      @param [in] count The maximum number of visible items at once.
     */
    void setMaxVisibleItems(int count);

    /*!
     *  Returns true if there is some content shown by the content assist widget.
     */
    bool isContentShown() const;

protected:
    /*!
     *  Handles the mouse press event.
     *
     *      @param [in] event The mouse press event.
     */
    void mousePressEvent(QMouseEvent* event);

    /*!
     *  Handles the mouse double click event.
     *
     *      @param [in] event The mouse double click event.
     */
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    // Disable copying.
    TextContentAssistWidget(TextContentAssistWidget const& rhs);
    TextContentAssistWidget& operator=(TextContentAssistWidget const& rhs);

    /*!
     *  Tries to match the contents to the text close to the text cursor.
     *  If there is a match, the content assist is automatically shown. Otherwise it is hidden.
     */
    void updateMatches();

    /*!
     *  Returns true if the content assist can currently commit with the given key.
     *
     *      @param [in] key The commit key.
     *
     *      @return True, if the content assist can currently commit.
     */
    bool canCommitWith(QKeyEvent* e) const;

    /*!
     *  Commits the selection with the contents from the content assist.
     *
     *      @remarks Assumes that there is at least a partial match found when match()
     *               was called the previous time.
     */
    void commitSelection();

    /*!
     *  Fits the widget to the contents.
     */
    void fitToContents();

    /*!
     *  Moves the widget close to the given cursor in the text, taking screen dimensions into account.
     */
    void moveCloseToCursor(int cursorPos);

    /*!
     *  Shows the tool tip hint.
     */
    void showToolTipHint(QString const& text, int cursorPos);

    //! The default maximum number of visible items.
    static int const DEFAULT_MAX_VISIBLE_ITEMS = 6;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent text editor.
    QPlainTextEdit* parent_;

    //! The content matcher.
    QSharedPointer<ITextContentMatcher> matcher_;

    //! Maximum number of visible items at once.
    int maxVisibleItems_;

    //! Last content position.
    int lastAssistStartPos_;
    
    //! Current content state.
    bool contentFound_;

    //! The tool tip hint widget.
    ToolTipHintWidget toolTipHintWidget_;
};

//-----------------------------------------------------------------------------

#endif // CONTENTASSISTWIDGET_H
