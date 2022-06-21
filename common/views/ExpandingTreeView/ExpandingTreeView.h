//-----------------------------------------------------------------------------
// File: ExpandingTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.06.2022
//
// Description:
// Base class for treeviews with automatic expansion of lone children.
//-----------------------------------------------------------------------------

#ifndef EXPANDINGTREEVIEW_H
#define EXPANDINGTREEVIEW_H

#include <QTreeView>

class ExpandingTreeView : public QTreeView
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ExpandingTreeView(QWidget* parent);
    
    //! The destructor.
    virtual ~ExpandingTreeView() = default;
    
    //! No copying.
    ExpandingTreeView(const ExpandingTreeView &other) = delete;

    //! No assignment.
    ExpandingTreeView &operator=(const ExpandingTreeView &other) = delete;

private slots:

    /*!
     * Expand the tree until first branch i.e. index with more than one children is found.
     *
     *     @param [in] index  The starting index for expanding.
     */
    void expandUntilFirstBranch(QModelIndex const& index);
};


#endif // EXPANDINGTREEVIEW_H
