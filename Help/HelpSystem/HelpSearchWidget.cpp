//-----------------------------------------------------------------------------
// File: HelpSearchWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.8.2012
//
// Description:
// Help search widget.
//-----------------------------------------------------------------------------

#include "HelpSearchWidget.h"

#include <QVBoxLayout>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>

//-----------------------------------------------------------------------------
// Function: HelpSearchWidget::HelpSearchWidget()
//-----------------------------------------------------------------------------
HelpSearchWidget::HelpSearchWidget(QHelpSearchEngine* searchEngine, QWidget* parent /*= 0*/)
    : QWidget(parent),
      searchEngine_(searchEngine)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(searchEngine->queryWidget());
    layout->addWidget(searchEngine->resultWidget(), 1);

    connect(searchEngine->queryWidget(), SIGNAL(search()), this, SLOT(onSearch()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HelpSearchWidget::~HelpSearchWidget()
//-----------------------------------------------------------------------------
HelpSearchWidget::~HelpSearchWidget()
{
}

//-----------------------------------------------------------------------------
// Function: HelpSearchWidget::onSearch()
//-----------------------------------------------------------------------------
void HelpSearchWidget::onSearch()
{
    QList<QHelpSearchQuery> query = searchEngine_->queryWidget()->query();
    searchEngine_->search(query);
}
