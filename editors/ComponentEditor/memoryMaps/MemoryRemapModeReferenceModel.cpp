//-----------------------------------------------------------------------------
// File: MemoryRemapModeReferenceModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.10.2023
//
// Description:
// Model to manage mode references of a memory remap.
//-----------------------------------------------------------------------------

#include "MemoryRemapModeReferenceModel.h"

#include <KactusAPI/include/MemoryMapInterface.h>

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::MemoryRemapModeReferenceModel()
//-----------------------------------------------------------------------------
MemoryRemapModeReferenceModel::MemoryRemapModeReferenceModel(MemoryMapInterface* memMapInterface, 
    QString const& memMapName, QString const& remapName, QObject* parent) :
    ModeReferenceModel(QList<QPair<QString, int> >(), parent),
    interface_(memMapInterface),
    memMapName_(memMapName.toStdString()),
    remapName_(remapName.toStdString())
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::rowCount()
//-----------------------------------------------------------------------------
int MemoryRemapModeReferenceModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return interface_->getRemapModeReferenceCount(memMapName_, remapName_);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::data()
//-----------------------------------------------------------------------------
QVariant MemoryRemapModeReferenceModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == 0)
        {
            return QString::number(interface_->getRemapModeReferencePriority(memMapName_, remapName_, index.row()));
        }

        else if (index.column() == 1)
        {
            return QString::fromStdString(interface_->getRemapModeReferenceValue(memMapName_, remapName_, index.row()));
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::setData()
//-----------------------------------------------------------------------------
bool MemoryRemapModeReferenceModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= interface_->getRemapModeReferenceCount(memMapName_, remapName_) ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == 0)
    {
        interface_->setRemapModeReferencePriority(memMapName_, remapName_, index.row(), value.toInt());
    }
    else if (index.column() == 1)
    {
        interface_->setRemapModeReferenceValue(memMapName_, remapName_, index.row(), value.toString().toStdString());
    }

    emit dataChanged(index, index);
    emit contentChanged();

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::onAddRow()
//-----------------------------------------------------------------------------
void MemoryRemapModeReferenceModel::onAddRow(QModelIndex const& index)
{
    int lastRow = interface_->getRemapModeReferenceCount(memMapName_, remapName_);

    beginInsertRows(QModelIndex(), lastRow, lastRow);
    interface_->addRemapModeReference(memMapName_, remapName_);
    endInsertRows();

    emit invalidateFilter();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapModeReferenceModel::onRemoveItem()
//-----------------------------------------------------------------------------
void MemoryRemapModeReferenceModel::onRemoveItem(QModelIndex const& index)
{
    int rowToRemove = index.row();

    if (!index.isValid() || rowToRemove < 0 ||
        rowToRemove >= interface_->getRemapModeReferenceCount(memMapName_, remapName_))
    {
        return;
    }

    beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
    interface_->removeRemapModeReference(memMapName_, remapName_, rowToRemove);
    endRemoveRows();

    emit contentChanged();
}
