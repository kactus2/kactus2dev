//-----------------------------------------------------------------------------
// File: HelpWindow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.8.2012
//
// Description:
// Help window that allows browsing the documentation through table of
// contents and index.
//-----------------------------------------------------------------------------

#include "HelpWindow.h"

#include "ContextHelpBrowser.h"
#include "HelpSearchWidget.h"

#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>

//-----------------------------------------------------------------------------
// Function: HelpWindow::HelpWindow()
//-----------------------------------------------------------------------------
HelpWindow::HelpWindow(QHelpEngine* engine, QWidget* parent /*= 0*/)
    : QMainWindow(parent),
      engine_(engine),
      menu_(new QMenuBar(this)),
      tocDock_(new QDockWidget(tr("Contents"), this)),
      //indexDock_(new QDockWidget(tr("Index"), this)),
      //searchWidget_(new HelpSearchWidget(engine->searchEngine(), this)),
      //searchDock_(new QDockWidget(tr("Search"), this)),
      browser_(new ContextHelpBrowser(engine, "qthelp://com.tut.kactus2.2.0/doc", this))
{
    setWindowTitle(tr("Kactus2 Help"));
    resize(800, 500);

    // Create the menu bar and all its menus and actions.
    QMenu* fileMenu = menu_->addMenu(tr("File"));
    fileMenu->addAction(tr("Exit"), this, SLOT(close()));

    setMenuBar(menu_);

    // Add the table of contents dock.
    tocDock_->setWidget(engine_->contentWidget());
    addDockWidget(Qt::LeftDockWidgetArea, tocDock_);

    // Add index dock.
//     indexDock_->setWidget(engine_->indexWidget());
//     addDockWidget(Qt::LeftDockWidgetArea, indexDock_);

    // Add search dock.
//     searchDock_->setWidget(searchWidget_);
//     addDockWidget(Qt::LeftDockWidgetArea, searchDock_);

    setCentralWidget(browser_);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);

    connect(engine_->indexWidget(), SIGNAL(linkActivated(QUrl const&, QString const&)),
            browser_, SLOT(setSource(QUrl const&)), Qt::UniqueConnection);

    connect(engine_->contentWidget(), SIGNAL(linkActivated(QUrl const&)),
            browser_, SLOT(setSource(QUrl const&)), Qt::UniqueConnection);

    connect(engine_->searchEngine()->resultWidget(), SIGNAL(requestShowLink(QUrl const&)),
            browser_, SLOT(setSource(QUrl const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HelpWindow::~HelpWindow()
//-----------------------------------------------------------------------------
HelpWindow::~HelpWindow()
{

}

//-----------------------------------------------------------------------------
// Function: HelpWindow::showEvent()
//-----------------------------------------------------------------------------
void HelpWindow::showEvent(QShowEvent* /*event*/)
{
    browser_->onHelpRequested("index.html");
}
