//-----------------------------------------------------------------------------
// File: assistedlineedit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
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
    /*!  Constructor.
     *
     *      @param [in] mainWnd         The parent window of the line edit. Must not be null.
     *      @param [in] parent          The parent widget. Can be null.
     */
    AssistedLineEdit(QWidget* parentWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~AssistedLineEdit();

    /*!
     *  Sets the content matcher.
     *
     *      @param [in] matcher The matcher to set.
     */
    void setContentMatcher(ILineContentMatcher* matcher);

    /*!
     *  Returns the content matcher.
     */
    ILineContentMatcher* getContentMatcher();

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
     *  Handles the focus in event.
     */
    void focusInEvent(QFocusEvent* e);

    /*!
     *  Handles the focus out event.
     */
    void focusOutEvent(QFocusEvent* e);

private:
    // Disable copying.
    AssistedLineEdit(AssistedLineEdit const& rhs);
    AssistedLineEdit& operator=(AssistedLineEdit const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The main window.
    QWidget* mainWnd_;

    //! The content matcher.
    ILineContentMatcher* matcher_;

    //! Content assist list widget.
    LineContentAssistWidget* contentAssist_;
};

//-----------------------------------------------------------------------------

#endif // ASSISTEDLINEEDIT_H
