//-----------------------------------------------------------------------------
// File: EnumerationModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing Enumeration within a Choice element.
//-----------------------------------------------------------------------------

#include "EnumerationModel.h"

#include <KactusAPI/include/KactusColors.h>

#include <editors/ComponentEditor/choices/EnumerationColumns.h>

#include <IPXACTmodels/common/Enumeration.h>

//-----------------------------------------------------------------------------
// Function: EnumerationModel::EnumerationModel()
//-----------------------------------------------------------------------------
EnumerationModel::EnumerationModel(QObject* parent):
TableModelBase(parent),
enumerations_()
{

}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::setupEnumerations()
//-----------------------------------------------------------------------------
void EnumerationModel::setupEnumerations(QSharedPointer<QList<QSharedPointer<Enumeration> > > newEnumerations)
{
    beginResetModel();

    enumerations_ = newEnumerations;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::setupEnumerations()
//-----------------------------------------------------------------------------
void EnumerationModel::clearEnumerations()
{
    beginResetModel();

    enumerations_.clear();

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::columnCount()
//-----------------------------------------------------------------------------
int EnumerationModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return EnumerationColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::rowCount()
//-----------------------------------------------------------------------------
int EnumerationModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid() || enumerations_.isNull())
    {
        return 0;
    }

    return enumerations_->size();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::data()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::data(QModelIndex const& index, int role) const
{
    if (isNotValidIndex(index))
    {
        return QVariant();
    }

    QSharedPointer<Enumeration> enumeration = enumerations_->at(index.row());
    auto column = index.column();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (column == EnumerationColumns::ENUMERATION)
        {
            return enumeration->getValue();
        }
        else if (column == EnumerationColumns::TEXT)
        {
            return enumeration->getText();
        }
        else if (column == EnumerationColumns::HELP)
        {
            return enumeration->getHelp();
        }
    }

    return TableModelBase::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::headerData()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == EnumerationColumns::ENUMERATION)
        {
            return tr("Enumeration");
        }
        else if (section == EnumerationColumns::TEXT)
        {
            return tr("Text");
        } 
        else if (section == EnumerationColumns::HELP)
        {
            return tr("Description");
        } 
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::setData()
//-----------------------------------------------------------------------------
bool EnumerationModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (isNotValidIndex(index))
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == EnumerationColumns::ENUMERATION)
        {
            enumerations_->at(index.row())->setValue(value.toString());
        }
        else if (index.column() == EnumerationColumns::TEXT)
        {
            enumerations_->at(index.row())->setText(value.toString());
        }
        else if (index.column() == EnumerationColumns::HELP)
        {
            enumerations_->at(index.row())->setHelp(value.toString());
        }
        else
        {
            Q_ASSERT_X(false, "setData()", "Undefined column requested.");
            return false;
        }
    } 

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags EnumerationModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags validFlags = QAbstractTableModel::flags(index);
    validFlags |= Qt::ItemIsEditable;
    return validFlags;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::onAddItem()
//-----------------------------------------------------------------------------
void EnumerationModel::onAddItem(QModelIndex const& index)
{
    if (enumerations_.isNull())
    {
        return;
    }

    int row = rowCount();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);

    QSharedPointer<Enumeration> enumeration(new Enumeration());
    enumeration->setValue(QString::number(rowCount()));
    enumerations_->insert(row, enumeration);
    
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::onRemoveItem()
//-----------------------------------------------------------------------------
void EnumerationModel::onRemoveItem(QModelIndex const& index)
{
    if (isNotValidIndex(index))
    {
        return;
    }

    int row = index.row();
    beginRemoveRows(QModelIndex(), row, row);

    enumerations_->removeAt(row);

    endRemoveRows();

    emit contentChanged();
}

bool EnumerationModel::validateIndex(QModelIndex const& index) const
{
    return enumerations_->at(index.row())->getValue().isEmpty() == false;
}

bool EnumerationModel::indexIsMandatory(QModelIndex const& index) const
{
    return index.column() == EnumerationColumns::ENUMERATION;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::isNotValidIndex()
//-----------------------------------------------------------------------------
bool EnumerationModel::isNotValidIndex(QModelIndex const& index) const
{
    return !index.isValid() ||  index.row() > enumerations_->size() || index.column() < 0 || index.column() >= EnumerationColumns::COLUMN_COUNT;
}
