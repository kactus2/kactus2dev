//-----------------------------------------------------------------------------
// File: ContextHelpBrowser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.8.2012
//
// Description:
// Context help browser.
//-----------------------------------------------------------------------------

#ifndef CONTEXTHELPBROWSER_H
#define CONTEXTHELPBROWSER_H

#include <QHelpEngine>
#include <QTextBrowser>

//-----------------------------------------------------------------------------
//! Context-based help browser.
//-----------------------------------------------------------------------------
class ContextHelpBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] engine     The help engine containing the documentation.
     *      @param [in] urlPrefix  A prefix used automatically for all URLs.
     *      @param [in] parent     The parent widget.
     */
    ContextHelpBrowser(QHelpEngine* engine, QString const& urlPrefix, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~ContextHelpBrowser();

    /*!
     *  Loads the resource at the given URL.
     */
    virtual QVariant loadResource(int type, QUrl const& url);

public slots:
    /*!
     *  Shows the given help page.
     *
     *      @param [in] url The URL of the page to show. This will be appended to the given URL prefix.
     */
    void onHelpRequested(QString const& url);

private:
    // Disable copying.
    ContextHelpBrowser(ContextHelpBrowser const& rhs);
    ContextHelpBrowser& operator=(ContextHelpBrowser const& rhs);

    //! The help engine.
    QHelpEngine* engine_;

    //! The URL prefix.
    QString urlPrefix_;
};

//-----------------------------------------------------------------------------

#endif // CONTEXTHELPBROWSER_H
