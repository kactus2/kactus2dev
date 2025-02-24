//-----------------------------------------------------------------------------
// File: TreeItemEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.8.2023
//
// Description:
// Editor for selecting a single item from a tree.
//-----------------------------------------------------------------------------

#ifndef TREEITEMEDITOR_H
#define TREEITEMEDITOR_H

#include "TreeItemModel.h"

#include <editors/ComponentEditor/common/MemoryTreeFactory.h>

#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QWidget>

//-----------------------------------------------------------------------------
//! Editor for selecting a single item from a tree.
//-----------------------------------------------------------------------------
class TreeItemEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] factory     Factory for creating the tree.
     *    @param [in] parent      The parent widget.
     */
    TreeItemEditor(MemoryTreeFactory const* factory, QWidget* parent = nullptr);


    //! The destructor.
    virtual ~TreeItemEditor() = default;

    // Disable copying.
    TreeItemEditor(TreeItemEditor const& rhs) = delete;
    TreeItemEditor& operator=(TreeItemEditor const& rhs) = delete;

    /*!
     *  Selects the given path in the tree.
     *
     *    @param [in] path    The path to select.
     */
    void selectPath(QStringList const& path);

    QStringList selectedPath() const;

signals:

    //! Emitted when the selection changes.
    void selectionChanged();

private slots:

    //! Handler for search bar text changes.
    void onSearchChanged(QString const& searchText);

    //! Handler for expand/collapse button clicks.
    void onExpandCollapse();

private:

    /*!
     *  Finds a child with the given name.
     *
     *    @param [in] parent  The parent of the child.
     *    @param [in] name    The name of the child to find.
     *
     *    @return The index of the found child.
     */
    QModelIndex findChild(QModelIndex const& parent, QString const& name);

    //! Setup the layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The search bar.
    QLineEdit* searchEditor_ = new QLineEdit(this);

    //! The button for expanding/collapsing the tree.
    QPushButton* expandButton_ = new QPushButton(this);
    
    //! The tree expand/collapse state.
    bool expanded_ = true;

    //! The tree view.
    QTreeView* view_ = new QTreeView(this);

    //! The proxy model for filtering the tree.
    QSortFilterProxyModel* proxy_ = new QSortFilterProxyModel(this);

    //! The tree model.
    TreeItemModel* model_ = nullptr;
};

#endif // TREEITEMEDITOR_H