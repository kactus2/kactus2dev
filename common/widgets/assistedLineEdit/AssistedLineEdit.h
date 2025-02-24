//-----------------------------------------------------------------------------
// File: assistedlineedit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 8.2.2011
//
// Description:
// Assisted line edit widget.
//-----------------------------------------------------------------------------

#ifndef ASSISTEDLINEEDIT_H
#define ASSISTEDLINEEDIT_H

#include <QLineEdit>

#include <QWidget>
#include <QLineEdit>
#include <QSharedPointer>

class ILineContentMatcher;
class LineContentAssistWidget;

//-----------------------------------------------------------------------------
//! AssistedLineEdit class.
//-----------------------------------------------------------------------------
class AssistedLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] parentWnd   The parent window of the line edit. Must not be null.
     *    @param [in] parent      The parent widget. Can be null.
     */
    AssistedLineEdit(QWidget* parentWnd, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~AssistedLineEdit() = default;

    // Disable copying.
    AssistedLineEdit(AssistedLineEdit const& rhs) = delete;
    AssistedLineEdit& operator=(AssistedLineEdit const& rhs) = delete;

    /*!
     *  Sets the content matcher.
     *
     *    @param [in] matcher The matcher to set.
     */
    void setContentMatcher(ILineContentMatcher* matcher);

protected:

    /*!
     *  Handles the given key press event.
     *
     *    @param [in] e The key event.
     */
    void keyPressEvent(QKeyEvent* e);

    /*!
     *  Handles the focus in event.
     */
    void focusInEvent(QFocusEvent* e);

    /*!
     *  Handles the focus out event.
     */
    void focusOutEvent(QFocusEvent* e);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The content matcher.
    ILineContentMatcher* matcher_ = nullptr;

    //! Content assist list widget.
    LineContentAssistWidget* contentAssist_;
};

//-----------------------------------------------------------------------------

#endif // ASSISTEDLINEEDIT_H
