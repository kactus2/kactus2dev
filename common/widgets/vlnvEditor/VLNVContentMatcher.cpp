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
#include <common/widgets/vlnvEditor/VLNVDataTree.h>

#include <IPXACTmodels/Component/Component.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: VLNVContentMatcher()
//-----------------------------------------------------------------------------
VLNVContentMatcher::VLNVContentMatcher(LibraryInterface* lh) : lh_(lh), node_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~VLNVContentMatcher()
//-----------------------------------------------------------------------------
VLNVContentMatcher::~VLNVContentMatcher()
{
}

//-----------------------------------------------------------------------------
// Function: setDataNode()
//-----------------------------------------------------------------------------
void VLNVContentMatcher::setDataNode(VLNVDataNode const* node)
{
    node_ = node;
}

//-----------------------------------------------------------------------------
// Function: getVLNVDataNode()
//-----------------------------------------------------------------------------
VLNVDataNode const* VLNVContentMatcher::getDataNode() const
{
    return node_;
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

    if (node_ != 0)
    {
        // Go through all the children and try to match with their names.
        for (int i = 0; i < node_->getChildren().size(); ++i)
        {
            VLNVDataNode const* child = node_->getChildren()[i].data();

            if (child->name().toLower().contains(exp))
            {
                if (assist != 0)
                {
                    QListWidgetItem* item = new QListWidgetItem(child->name());
                    
                    assist->addItem(item);
                }

                ++count;

                if (!exactMatch && child->name() == text)
                {
                    exactMatch = true;
                }
            }
        }
    }

    return (count > 1 || (count == 1 && !exactMatch));
}