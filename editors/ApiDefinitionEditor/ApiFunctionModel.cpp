//-----------------------------------------------------------------------------
// File: ApiFunctionModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Model for API functions.
//-----------------------------------------------------------------------------

#include "ApiFunctionModel.h"

#include <models/ApiFunction.h>
#include <models/ApiDefinition.h>

#include <QVariant>
#include <QColor>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::ApiFunctionModel()
//-----------------------------------------------------------------------------
ApiFunctionModel::ApiFunctionModel(QObject *parent)
    : QAbstractListModel(parent),
      apiFunctions_()
{
    connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::~ApiFunctionModel()
//-----------------------------------------------------------------------------
ApiFunctionModel::~ApiFunctionModel()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::restore()
//-----------------------------------------------------------------------------
void ApiFunctionModel::restore(ApiDefinition const& apiDef)
{
    beginResetModel();

    apiFunctions_.clear();

    for (int i = 0; i < apiDef.getFunctionCount(); ++i)
    {
        apiFunctions_.append(QSharedPointer<ApiFunction>(new ApiFunction(*apiDef.getFunction(i).data())));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::save()
//-----------------------------------------------------------------------------
void ApiFunctionModel::save(ApiDefinition& apiDef)
{
    apiDef.removeFunctions();

    foreach (QSharedPointer<ApiFunction> func, apiFunctions_)
    {
        apiDef.addFunction(QSharedPointer<ApiFunction>(new ApiFunction(*func.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::rowCount()
//-----------------------------------------------------------------------------
int ApiFunctionModel::rowCount( const QModelIndex&) const {

    // if there are no items then there is the help text
    if (apiFunctions_.isEmpty()) {
        return 1;
    }

    return apiFunctions_.size();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::data()
//-----------------------------------------------------------------------------
QVariant ApiFunctionModel::data( const QModelIndex& index, int role) const {

    // nothing for invalid indexes
    if (!index.isValid()) {
        return QVariant();
    }

    // if there are no real items then display the help text
    else if (index.row() == 0 && apiFunctions_.isEmpty()) {
        switch (role) {
            case Qt::DisplayRole: {
                return tr("Double click to add new item.");
                                  }
            case Qt::ForegroundRole: {
                return QColor("silver");
                                     }
            default: {
                return QVariant();
                     }
        }
    }

    // if index.row() is invalid
    else if (index.row() < 0 || index.row() >= apiFunctions_.size()) {
        return QVariant();
    }

    // return data for display role
    if (role == Qt::DisplayRole) {
        return apiFunctions_.at(index.row())->getName();
    }
    // if unsupported role
    else {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::headerData()
//-----------------------------------------------------------------------------
QVariant ApiFunctionModel::headerData(int section, Qt::Orientation orientation, int role) const {

    // only one column to display
    if (section != 0) {
        return QVariant();
    }
    // only horizontal headers
    else if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    // data for displayRole
    if (role == Qt::DisplayRole) {
        return tr("Items");
    }
    // unsupported role
    else {
        return QVariant();
    }

}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::setData()
//-----------------------------------------------------------------------------
bool ApiFunctionModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    // if there are no real items then create one to avoid mis indexing
    else if (index.row() == 0 && apiFunctions_.isEmpty()) {
        apiFunctions_.append(QSharedPointer<ApiFunction>(new ApiFunction()));
    }

    // if row is invalid
    else if (index.row() < 0 || index.row() >= apiFunctions_.size()) {
        return false;
    }

    // list model has only one column
    else if (index.column() != 0) {
        return false;
    }

    if (role == Qt::EditRole) {
        apiFunctions_[index.row()]->setName(value.toString());
        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }

    // unsupported role
    else {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ApiFunctionModel::flags( const QModelIndex& index ) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::addItem()
//-----------------------------------------------------------------------------
void ApiFunctionModel::addItem(const QModelIndex& index)
{
    int row = apiFunctions_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid()) {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);

    QSharedPointer<ApiFunction> func(new ApiFunction());
    func->setName("function");

    apiFunctions_.insert(row, func);

    endInsertRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::remove()
//-----------------------------------------------------------------------------
void ApiFunctionModel::remove(const QModelIndex& index)
{
    // don't remove anything if index is invalid
    if (!index.isValid()) {
        return;
    }
    // make sure the row number if valid
    else if (index.row() < 0 || index.row() >= apiFunctions_.size()) {
        return;
    }

    // remove the specified item
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    apiFunctions_.removeAt(index.row());
    endRemoveRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::moveItem()
//-----------------------------------------------------------------------------
void ApiFunctionModel::moveItem( const QModelIndex& originalPos, 
                                 const QModelIndex& newPos )
{
    // if there was no item in the starting point
    if (!originalPos.isValid()) {
        return;
    }
    // if the indexes are the same
    else if (originalPos == newPos) {
        return;
    }
    else if (originalPos.row() < 0 || originalPos.row() >= apiFunctions_.size()) {
        return;
    }

    // if the new position is invalid index then put the item last in the list
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= apiFunctions_.size()) {

        beginResetModel();
        QSharedPointer<ApiFunction> func = apiFunctions_.takeAt(originalPos.row());
        apiFunctions_.append(func);
        endResetModel();
    }
    // if both indexes were valid
    else {
        beginResetModel();
        apiFunctions_.swap(originalPos.row(), newPos.row());
        endResetModel();
    }
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::replace()
//-----------------------------------------------------------------------------
void ApiFunctionModel::replace(QModelIndex& index, const QString newText)
{
    if (!index.isValid()) {
        return;
    }
    // make sure the row is correct
    else if (index.row() < 0 || index.row() >= apiFunctions_.size()) {
        return;
    }

    apiFunctions_[index.row()]->setName(newText);
    emit dataChanged(index, index);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionModel::getFunction()
//-----------------------------------------------------------------------------
QSharedPointer<ApiFunction> ApiFunctionModel::getFunction(QModelIndex const& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= apiFunctions_.size())
    {
        return QSharedPointer<ApiFunction>();
    }

    return apiFunctions_.at(index.row());
}

