//-----------------------------------------------------------------------------
// File: HelpSearchWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.8.2012
//
// Description:
// Help search widget.
//-----------------------------------------------------------------------------

#ifndef HELPSEARCHWIDGET_H
#define HELPSEARCHWIDGET_H

#include <QHelpSearchEngine>
#include <QWidget>

//-----------------------------------------------------------------------------
//! Help search widget.
//-----------------------------------------------------------------------------
class HelpSearchWidget : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    HelpSearchWidget(QHelpSearchEngine* searchEngine, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~HelpSearchWidget();

private slots:
    void onSearch();

private:
    // Disable copying.
    HelpSearchWidget(HelpSearchWidget const& rhs);
    HelpSearchWidget& operator=(HelpSearchWidget const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The help search engine.
    QHelpSearchEngine* searchEngine_;
};

//-----------------------------------------------------------------------------

#endif // HELPSEARCHWIDGET_H
