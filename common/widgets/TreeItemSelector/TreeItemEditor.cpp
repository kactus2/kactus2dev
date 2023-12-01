//-----------------------------------------------------------------------------
// File: TreeItemEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.8.2023
//
// Description:
// Editor for selecting a single item from a tree.
//-----------------------------------------------------------------------------

#include "TreeItemEditor.h"

#include "TreeItem.h"

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::TreeItemEditor()
//-----------------------------------------------------------------------------
TreeItemEditor::TreeItemEditor(MemoryTreeFactory const* factory, QWidget* parent) : QWidget(parent)
{
    setAutoFillBackground(true);

    searchEditor_->addAction(QIcon(":/icons/common/graphics/search.png"), QLineEdit::LeadingPosition);
    searchEditor_->setPlaceholderText(tr("Search"));
    searchEditor_->setClearButtonEnabled(true);

    expandButton_->setFlat(true);
    expandButton_->setToolTip(tr("Expand all"));
    expandButton_->setIcon(QIcon(":/icons/common/graphics/triangle_arrow_down.png"));

    model_ = new TreeItemModel(factory->create(), this);
    
    proxy_->setSourceModel(model_);
    proxy_->setFilterKeyColumn(0);
    proxy_->setRecursiveFilteringEnabled(true);

    view_->setModel(proxy_);
    view_->setHeaderHidden(true);
    view_->setUniformRowHeights(true);
    view_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view_->expandAll();

    connect(searchEditor_, SIGNAL(textChanged(QString const&)), this, SLOT(onSearchChanged(QString const&)));
    
    connect(expandButton_, SIGNAL(clicked()), this, SLOT(onExpandCollapse()));

    connect(view_, SIGNAL(clicked(QModelIndex)), this, SIGNAL(selectionChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::selectPath()
//-----------------------------------------------------------------------------
void TreeItemEditor::selectPath(QStringList const& path)
{
    auto index = proxy_->mapFromSource(QModelIndex());

    // Select the item at the end of the path
    for (auto const& item : path)
    {
        // find the child item with given name
        index = findChild(index, item.split('/').last());

        if (index.isValid() == false)
        {
            return;
        }
    }

    view_->setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::findChild()
//-----------------------------------------------------------------------------
QModelIndex TreeItemEditor::findChild(QModelIndex const& parent, QString const& name)
{
    for (int i = 0; i < proxy_->rowCount(parent); ++i)
    {
        auto child = proxy_->index(i, 0, parent);

        if (child.data().toString() == name)
        {
            return child;

        }
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::selectedPath()
//-----------------------------------------------------------------------------
QStringList TreeItemEditor::selectedPath() const
{
    QStringList path;

    if (auto const& index = proxy_->mapToSource(view_->currentIndex()); index.isValid())
    {
        auto treeItem = static_cast<TreeItem*>(index.internalPointer());

        while (treeItem)
        {
            path.prepend(treeItem->type + "/" + treeItem->name);
            treeItem = treeItem->parent;
        }
    }
    
    // Remove the root item
    if (path.isEmpty() == false)
    {
        path.pop_front();
    }
    return path;
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::onSearchChanged()
//-----------------------------------------------------------------------------
void TreeItemEditor::onSearchChanged(QString const& searchText)
{
    proxy_->setFilterRegularExpression(QRegularExpression(searchText, QRegularExpression::CaseInsensitiveOption));
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::onExpandCollapse()
//-----------------------------------------------------------------------------
void TreeItemEditor::onExpandCollapse()
{
    if (expanded_)
    {
        view_->collapseAll();
        expandButton_->setIcon(QIcon(":/icons/common/graphics/triangle_arrow_right.png"));
        expandButton_->setToolTip(tr("Expand all"));
    }
    else
    {
        view_->expandAll();
        expandButton_->setIcon(QIcon(":/icons/common/graphics/triangle_arrow_down.png"));
        expandButton_->setToolTip(tr("Collapse all"));
    }

    expanded_ = !expanded_;
}

//-----------------------------------------------------------------------------
// Function: TreeItemEditor::setupLayout()
//-----------------------------------------------------------------------------
void TreeItemEditor::setupLayout()
{
    auto controlLayout = new QHBoxLayout();
    controlLayout->addWidget(searchEditor_);
    controlLayout->addWidget(expandButton_);
    controlLayout->setSpacing(1);

    view_->setContentsMargins(0, 0, 0, 0);

    auto layout = new QVBoxLayout(this);
    layout->addLayout(controlLayout);
    layout->addWidget(view_);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);
}
