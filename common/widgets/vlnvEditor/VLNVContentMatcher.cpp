//-----------------------------------------------------------------------------
// File: VLNVContentMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.2.2011
//
// Description:
// VLNV content matcher.
//-----------------------------------------------------------------------------

#include "VLNVContentMatcher.h"

#include <QRegExp>
#include <common/widgets/assistedLineEdit/LineContentAssistWidget.h>
#include <LibraryManager/libraryitem.h>

//-----------------------------------------------------------------------------
// Function: VLNVContentMatcher()
//-----------------------------------------------------------------------------
VLNVContentMatcher::VLNVContentMatcher(LibraryItem const* item) : m_item(item)
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVContentMatcher()
//-----------------------------------------------------------------------------
VLNVContentMatcher::~VLNVContentMatcher()
{
}

//-----------------------------------------------------------------------------
// Function: setLibraryItem()
//-----------------------------------------------------------------------------
void VLNVContentMatcher::setLibraryItem(LibraryItem const* item)
{
    m_item = item;
}

//-----------------------------------------------------------------------------
// Function: getLibraryItem()
//-----------------------------------------------------------------------------
LibraryItem const* VLNVContentMatcher::getLibraryItem() const
{
    return m_item;
}

//-----------------------------------------------------------------------------
// Function: lookForwardMatch()
//-----------------------------------------------------------------------------
bool VLNVContentMatcher::lookForwardMatch(QString const& text)
{
    return enumerateMatches(text, 0);
}

//-----------------------------------------------------------------------------
// Function: fillWithContent()
//-----------------------------------------------------------------------------
bool VLNVContentMatcher::fillWithContent(QString const& text, LineContentAssistWidget& assist,
                                         int& startIndex, QString& /*toolTipText*/, int& /*toolTipIndex*/)
{
    startIndex = 0;
    bool ret = enumerateMatches(text, &assist);
    assist.sortItems();
    return ret;
}

//-----------------------------------------------------------------------------
// Function: enumerateMatches()
//-----------------------------------------------------------------------------
bool VLNVContentMatcher::enumerateMatches(QString const& text, LineContentAssistWidget* assist) const
{
    QRegExp exp("^" + text.toLower() + ".*");

    int count = 0;
    bool exactMatch = false;

    if (m_item != 0)
    {
        // Go through all the children and try to match with their names.
        for (int i = 0; i < m_item->getNumberOfChildren(); ++i)
        {
            LibraryItem const* child = m_item->child(i);

            if (child->getName().toLower().contains(exp))
            {
                if (assist != 0)
                {
                    assist->addItem(new QListWidgetItem(child->getName()));
                }

                ++count;

                if (!exactMatch && child->getName() == text)
                {
                    exactMatch = true;
                }
            }
        }
    }

    return (count > 1 || (count == 1 && !exactMatch));
}
