//-----------------------------------------------------------------------------
// File: EndpointModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint data model for the endpoint editor.
//-----------------------------------------------------------------------------

#include "EndpointModel.h"

#include <QDebug>

EndpointModel::EndpointModel(void* dataPointer, QObject *parent): 
QAbstractTableModel(parent), modelParameters_(), table_()
{
    Q_ASSERT_X(dataPointer, "EndpointModel constructor",
        "Null dataPointer given as parameter");

    // Set the pointer to the actual data structure containing the endpoints.
    modelParameters_ = static_cast<
        QMap<QString, QSharedPointer<ModelParameter> > *>(dataPointer);

    restore();
}

EndpointModel::~EndpointModel() {
}

int EndpointModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const {

    // not hierarchical model
    if (parent.isValid())
        return 0;

    return table_.size();
}

int EndpointModel::columnCount( const QModelIndex & parent /*= QModelIndex() */ ) const {

    // not hierarchical model
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant EndpointModel::data(const QModelIndex & index, 
                             int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    // if row is invalid
    if (index.row() < 0 || index.row() >= table_.size())
        return QVariant();

    if (role == Qt::DisplayRole) {

        switch (index.column()) {
            case 0: 
                return table_.value(index.row())->getName();
            case 1:
                return table_.value(index.row())->getDataType();
            case 2:
                return table_.value(index.row())->getValue();
            default:
                return QVariant();
        }
    }

    // if unsupported role
    else {
        return QVariant();
    }
}

QVariant EndpointModel::headerData(int section, Qt::Orientation orientation, 
                                   int role /*= Qt::DisplayRole */ ) const
{
    // only horizontal headers are supported
    if (orientation != Qt::Horizontal) 
        return QVariant();

    if (role == Qt::DisplayRole) {

        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Data type");
            case 2:
                return tr("Direction");
            default:
                return QVariant();
        }
    }

    // if unsupported role
    else {
        return QVariant();
    }
}

bool EndpointModel::setData(const QModelIndex& index, const QVariant& value, 
                            int role /*= Qt::EditRole */ )
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {

        switch (index.column()) {
            case 0: {

                // if name changes then the map has to change the indexing also
                QString oldKey = table_.value(index.row())->getName();

                // remove the item with old key and insert the value with new key
                //QSharedPointer<ModelParameter> modParam = modelParameters_->take(oldKey);
                //modelParameters_->insert(value.toString(), modParam);

                table_.value(index.row())->setName(value.toString());
                break;				
                    }
            case 1: {
                table_.value(index.row())->setDataType(value.toString());
                break;
                    }
            case 2: {
                table_.value(index.row())->setValue(value.toString());
                break;
                    }
            default:
                return false;

        }
        emit dataChanged(index, index);
        return true;
    }
    // if role is not supported
    else {
        return false;
    }
}

Qt::ItemFlags EndpointModel::flags(const QModelIndex& index) const {

    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void EndpointModel::onRemoveRow( int row ) {

    // if row is invalid
    if (row < 0 || row >= table_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);

    // remove the object from the map
    table_.removeAt(row);

    endRemoveRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

void EndpointModel::onAddRow() {

    beginInsertRows(QModelIndex(), table_.size(), table_.size());

    table_.append(QSharedPointer<ModelParameter>(new ModelParameter()));

    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

bool EndpointModel::isValid() const {

    for (int i = 0; i < table_.size(); ++i) {

        // if one model parameter is not valid
        if (!table_.value(i)->isValid()) {

            emit errorMessage(tr("Endpoint %1 is not valid").arg(
                table_.value(i)->getName()));
            return false;
        }
    }
    return true;
}

void EndpointModel::apply() {

    if (!isValid())
        return;

    // remove old model parameters from original model
    modelParameters_->clear();

    for (int i = 0; i < table_.size(); ++i) {
        modelParameters_->insert(table_.value(i)->getName(), table_.value(i));
    }
}

void EndpointModel::restore() {

    beginResetModel();

    // remove old model parameters
    table_.clear();

    // add all model parameters to the table
    foreach (QSharedPointer<ModelParameter> modelParameter, *modelParameters_) {

        // create a copy of the model parameter
        table_.append(QSharedPointer<ModelParameter>(
            new ModelParameter(*modelParameter.data())));
    }

    endResetModel();
}
