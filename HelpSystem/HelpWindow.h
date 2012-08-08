//-----------------------------------------------------------------------------
// File: HelpWindow.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.8.2012
//
// Description:
// Help window that allows browsing the documentation through table of
// contents and index.
//-----------------------------------------------------------------------------

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QHelpEngine>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>

class ContextHelpBrowser;
class HelpSearchWidget;

//-----------------------------------------------------------------------------
//! Help window that allows browsing the documentation through table of
//! contents and index.
//-----------------------------------------------------------------------------
class HelpWindow : public QMainWindow
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] engine  The help engine.
     *      @param [in] parent  The parent widget.
     */
    HelpWindow(QHelpEngine* engine, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~HelpWindow();

protected:
    virtual void showEvent(QShowEvent* event);

private:
    // Disable copying.
    HelpWindow(HelpWindow const& rhs);
    HelpWindow& operator=(HelpWindow const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used help engine.
    QHelpEngine* engine_;

    //! The menu bar for the help window.
    QMenuBar* menu_;

    //! Dock widget for table of contents.
    QDockWidget* tocDock_;

    //! Dock widget for index.
    QDockWidget* indexDock_;

    //! The search widget.
    HelpSearchWidget* searchWidget_;

    //! Dock widget for search.
    QDockWidget* searchDock_;

    //! The help browser.
    ContextHelpBrowser* browser_;
};

//-----------------------------------------------------------------------------

#endif // HELPWINDOW_H
