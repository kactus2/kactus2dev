//-----------------------------------------------------------------------------
// File: DrawingBoard.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.3.2012
//
// Description:
// The drawing board for holding various IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef DRAWINGBOARD_H
#define DRAWINGBOARD_H

#include <QTabWidget>

class TabDocument;

//-----------------------------------------------------------------------------
//! The drawing board for holding various IP-XACT documents.
//-----------------------------------------------------------------------------
class DrawingBoard : public QTabWidget
{
    Q_OBJECT

public:
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent widget.
     */
    DrawingBoard(QWidget* parent = 0);

    //! The destructor.
    ~DrawingBoard();
       
    /*!
     *  Registers a tab document (connects common signals etc.).
     *
     *      @param [in] doc            The document to register.
     *      @param [in] forceUnlocked  If true the document is initially unlocked.
     */
    void addAndOpenDocument(TabDocument* doc, bool forceUnlocked);

public slots:
    
    //! Saves the currently open document.
    void saveCurrentDocument();

    //! Saves the currently open document under a different name.
    void saveCurrentDocumentAs();

    //! Saves all open documents.
    void saveAll();

    //! Prints the currently open document.
    void printCurrentDocument();

    //! Refreshes the currently open document.
    void refreshCurrentDocument();

    /*!
     *  Closes and removes the document at given index.
     *
     *      @param [in] index   The document index to close.
     */
    void closeAndRemoveDocument(int index);

    //! Closes all open documents.
    void closeAll();

protected:
    
    /*!
     *  Filters and handles keyboard shortcut events.
     */
    virtual bool event(QEvent* event);

    /*!
     *  Filters the events for given target. Used to create context menu for closing tabs.
     *
     *      @param [in] target      The target object whose event to filter.
     *      @param [in] event       The event captured before passing to target.
     *
     *      @return True, if the event was filtered and should not be passed further, otherwise false.
     */
    virtual bool eventFilter(QObject* target, QEvent* event);

signals:
    
    //! Emitted to notify the user of an error.
    void errorMessage(QString const& message);

    //! Emitted to notify the user.
    void noticeMessage(QString const& message);

    //! Emitted to request opening a help url.
    void helpUrlRequested(QString const& url);

    //! Emitted when document content has changed.
    void documentContentChanged();
 
    //! Emitted when document modified state has changed.
    void documentModifiedChanged(bool);

    //! Emitted when document undo/redo state has changed.
    void documentEditStateChanged();

    //! Emitted when the last document has been closed.
    void lastDocumentClosed();

private slots:

    //! Called when the title of a document changes.
    void onDocumentTitleChanged(TabDocument* document, QString const& newTitle);

    //! Called when a document has been saved.
    void onDocumentSaved(TabDocument* doc);

private:
    // Disable copying.
    DrawingBoard(DrawingBoard const& rhs);
    DrawingBoard& operator=(DrawingBoard const& rhs);
   
    /*!
     *  Checks if the current document should be saved. If the document contains errors, prompts the user.
     *
     *      @param [in] doc   The document to save.
     *
     *      @return True, if the document should be saved, otherwise false.
     */
    bool shouldSave(TabDocument* doc);

};

//-----------------------------------------------------------------------------

#endif // DRAWINGBOARD_H
