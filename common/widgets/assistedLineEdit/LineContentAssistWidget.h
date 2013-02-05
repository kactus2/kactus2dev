//-----------------------------------------------------------------------------
// File: LineContentAssistWidget.h
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
#include <QLineEdit>
#include <QSharedPointer>

#include "ILineContentMatcher.h"

//-----------------------------------------------------------------------------
//! LineContentAssistWidget class.
//-----------------------------------------------------------------------------
class LineContentAssistWidget : public QListWidget
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] target     The target line edit. Must not be null.
     *      @param [in] parentWnd  The parent window of the line edit.
     */
    LineContentAssistWidget(QLineEdit* target, QWidget* parentWnd);

    /*!
     *  Destructor.
     */
    ~LineContentAssistWidget();

    /*!
     *  Sets the content matcher.
     *
     *      @param [in] matcher The matcher to set.
     */
    void setContentMatcher(ILineContentMatcher* matcher);

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
    LineContentAssistWidget(LineContentAssistWidget const& rhs);
    LineContentAssistWidget& operator=(LineContentAssistWidget const& rhs);

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
     *  Moves the widget close to the line edit, taking parent window dimensions into account.
     */
    void moveClose(int cursorPos);

    //! The default maximum number of visible items.
    static int const DEFAULT_MAX_VISIBLE_ITEMS = 6;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target line edit.
    QLineEdit* target_;

    //! The content matcher.
    ILineContentMatcher* matcher_;

    //! Maximum number of visible items at once.
    int maxVisibleItems_;

    //! Last content position.
    int lastAssistStartPos_;
    
    //! Current content state.
    bool contentFound_;
};

//-----------------------------------------------------------------------------

#endif // CONTENTASSISTWIDGET_H
