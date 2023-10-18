//-----------------------------------------------------------------------------
// File: ModeReferenceModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.8.2023
//
// Description:
// Model to manage mode references.
//-----------------------------------------------------------------------------

#include "ModeReferenceModel.h"

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::ModeReferenceModel()
//-----------------------------------------------------------------------------
ModeReferenceModel::ModeReferenceModel(QList<QPair<QString, int> > const& modeRefs, QObject* parent):
    QAbstractTableModel(parent),
    modeRefs_(modeRefs)
{

}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::getModeRefs()
//-----------------------------------------------------------------------------
QList<QPair<QString, int> > ModeReferenceModel::getModeRefs() const
{
    return modeRefs_;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::setModeRefs()
//-----------------------------------------------------------------------------
void ModeReferenceModel::setModeRefs(QList<QPair<QString, int> > const& modeRefs)
{
    modeRefs_ = modeRefs;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::rowCount()
//-----------------------------------------------------------------------------
int ModeReferenceModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return modeRefs_.size();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::columnCount()
//-----------------------------------------------------------------------------
int ModeReferenceModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return 2;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModeReferenceModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModeReferenceModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return QStringLiteral("Priority");
        }

        if (section == 1)
        {
            return QStringLiteral("Mode");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::data()
//-----------------------------------------------------------------------------
QVariant ModeReferenceModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == 0)
        {
            return QString::number(modeRefs_.at(index.row()).second);
        }

        else if (index.column() == 1)
        {
            return modeRefs_.at(index.row()).first;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::setData()
//-----------------------------------------------------------------------------
bool ModeReferenceModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= modeRefs_.size() ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == 0)
    {
        modeRefs_[index.row()].second = value.toInt();
    }
    
    else if (index.column() == 1)
    {
        modeRefs_[index.row()].first = value.toString();
    }

    emit dataChanged(index, index);
    emit contentChanged();

    return true;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::onAddRow()
//-----------------------------------------------------------------------------
void ModeReferenceModel::onAddRow(QModelIndex const& index)
{
    int lastRow = modeRefs_.size();

    beginInsertRows(QModelIndex(), lastRow, lastRow);
    modeRefs_.append(QPair<QString, int>());
    endInsertRows();

    emit invalidateFilter();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModeReferenceModel::onRemoveItem(QModelIndex const& index)
{
    int rowToRemove = index.row();

    if (!index.isValid() || rowToRemove < 0 ||
        rowToRemove >= modeRefs_.size())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
    modeRefs_.removeAt(rowToRemove);
    endRemoveRows();

    emit contentChanged();
}
