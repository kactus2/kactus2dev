//-----------------------------------------------------------------------------
// File: ContextHelpBrowser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.8.2012
//
// Description:
// Context help browser.
//-----------------------------------------------------------------------------

#include "ContextHelpBrowser.h"

#include <QHelpContentWidget>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ContextHelpBrowser::ContextHelpBrowser()
//-----------------------------------------------------------------------------
ContextHelpBrowser::ContextHelpBrowser(QHelpEngine* engine,QString const& urlPrefix, QWidget* parent):
QTextBrowser(parent),
    engine_(engine),
    urlPrefix_(urlPrefix)
{
    
}

//-----------------------------------------------------------------------------
// Function: ContextHelpBrowser::~ContextHelpBrowser()
//-----------------------------------------------------------------------------
ContextHelpBrowser::~ContextHelpBrowser()
{
}

//-----------------------------------------------------------------------------
// Function: ContextHelpBrowser::loadResource()
//-----------------------------------------------------------------------------
QVariant ContextHelpBrowser::loadResource(int type, QUrl const& url)
{
    if (url.scheme() == "qthelp")
    {
        return QVariant(engine_->fileData(url));
    }
    else
    {
        return QTextBrowser::loadResource(type, url);
    }
}

//-----------------------------------------------------------------------------
// Function: ContextHelpBrowser::setSource()
//-----------------------------------------------------------------------------
void ContextHelpBrowser::setSource(QUrl const& url)
{
    if (url.scheme() == "http" || url.scheme() == "https")
    {
        QDesktopServices::openUrl(url);
    }
    else
    {
        QTextBrowser::setSource(url);
    }    
}

//-----------------------------------------------------------------------------
// Function: ContextHelpBrowser::onHelpRequested()
//-----------------------------------------------------------------------------
void ContextHelpBrowser::onHelpRequested(QString const& url)
{
    setSource(QUrl(urlPrefix_ + "/" + url));
}
