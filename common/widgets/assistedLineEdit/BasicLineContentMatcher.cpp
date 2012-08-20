//-----------------------------------------------------------------------------
// File: BasicLineContentMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.08.2012
//
// Description:
// Line content matcher for simple list-based suggestions.
//-----------------------------------------------------------------------------

#include "BasicLineContentMatcher.h"

#include <QRegExp>
#include <common/widgets/assistedLineEdit/LineContentAssistWidget.h>

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::BasicLineContentMatcher()
//-----------------------------------------------------------------------------
BasicLineContentMatcher::BasicLineContentMatcher()
    : items_()
{
}

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::~BasicLineContentMatcher()
//-----------------------------------------------------------------------------
BasicLineContentMatcher::~BasicLineContentMatcher()
{
}

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::addItems()
//-----------------------------------------------------------------------------
void BasicLineContentMatcher::setItems(QStringList const& items)
{
    items_ = items;
}

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::lookForwardMatch()
//-----------------------------------------------------------------------------
bool BasicLineContentMatcher::lookForwardMatch(QString const& text)
{
    return enumerateMatches(text, 0);
}

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::fillWithContent()
//-----------------------------------------------------------------------------
bool BasicLineContentMatcher::fillWithContent(QString const& text, LineContentAssistWidget& assist,
                                              int& startIndex, QString&, int&)
{
    startIndex = 0;
    bool ret = enumerateMatches(text, &assist);
    assist.sortItems();
    return ret;
}

//-----------------------------------------------------------------------------
// Function: BasicLineContentMatcher::enumerateMatches()
//-----------------------------------------------------------------------------
bool BasicLineContentMatcher::enumerateMatches(QString const& text, LineContentAssistWidget* assist) const
{
    QRegExp exp("^" + text.toLower() + ".*");

    int count = 0;
    bool exactMatch = false;

    foreach (QString const& item, items_)
    {
        if (item.toLower().contains(exp))
        {
            if (assist != 0)
            {
                QListWidgetItem* listItem = new QListWidgetItem(item);
                assist->addItem(listItem);
            }

            ++count;

            if (!exactMatch && item == text)
            {
                exactMatch = true;
            }
        }
    }

    return (count > 1 || (count == 1 && !exactMatch));
}
