//-----------------------------------------------------------------------------
// File: TreeItemModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.8.2023
//
// Description:
// A model for generic tree items.
//-----------------------------------------------------------------------------

#include "TreeItem.h"

#include <QAbstractItemModel>

//-----------------------------------------------------------------------------
//! A model for generic tree items.
//-----------------------------------------------------------------------------
class TreeItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    TreeItemModel(TreeItem* rootItem, QObject* parent = nullptr);

    //! The destructor.
    virtual ~TreeItemModel() = default;



    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;


    QModelIndex parent(const QModelIndex& child) const override;


    int rowCount(const QModelIndex& parent = QModelIndex()) const override;



    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;



private:

    TreeItem* getItem(QModelIndex const& index ) const;

    TreeItem* rootItem_ = nullptr;
};