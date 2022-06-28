//-----------------------------------------------------------------------------
// File: ExpandingTreeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.06.2022
//
// Description:
// Base class for treeviews with automatic expansion of lone children.
//-----------------------------------------------------------------------------

#include "ExpandingTreeView.h"

//-----------------------------------------------------------------------------
// Function: ExpandingTreeView::ExpandingTreeView()
//-----------------------------------------------------------------------------
ExpandingTreeView::ExpandingTreeView(QWidget* parent):
    QTreeView(parent)
{
    connect(this, SIGNAL(expanded(QModelIndex const&)), 
        this, SLOT(expandUntilFirstBranch(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ExpandingTreeView::expandUntilFirstBranch()
//-----------------------------------------------------------------------------
void ExpandingTreeView::expandUntilFirstBranch(QModelIndex const& index)
{
    QModelIndex currentIndex = model()->index(0, 0, index);
    if (currentIndex.isValid() && currentIndex.sibling(1, 0).isValid() == false)
    {
        expand(currentIndex);
        currentIndex = model()->index(0, 0, currentIndex);
    }
}
