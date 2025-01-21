//-----------------------------------------------------------------------------
// File: TreeItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.8.2023
//
// Description:
// A generic tree item.
//-----------------------------------------------------------------------------

#include <QString>
#include <QList>

#ifndef TREEITEM_H
#define TREEITEM_H

//! A single item in a tree.
struct TreeItem 
{
    /*!
     *  The constructor.
     *
     *    @param [in] name    The name of the item.
     *    @param [in] type    The type of the item.
     *    @param [in] parent  The parent of the item.
     *
     *    @return 
     */
    TreeItem(QString const& name, QString const& type) :
        name(name), type(type), parent(nullptr) {};

    //! The destructor.
    ~TreeItem()
    {
        for (TreeItem* child : children)
        {
            delete child;
        }
    };

    /*!
     *  Adds a child to the item.
     *
     *    @param [in] child   The child to add.
     */
    void addChild(TreeItem* child)
    {
        children.append(child);
        child->parent = this;
    }

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QString name; 
    QString type;
    TreeItem* parent;
    QList<TreeItem*> children;
};

#endif // TREEITEM_H