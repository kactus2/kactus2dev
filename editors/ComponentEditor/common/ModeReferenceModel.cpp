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

#include <KactusAPI/include/ModeReferenceInterface.h>

#include <IPXACTmodels/Component/Mode.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::ModeReferenceModel()
//-----------------------------------------------------------------------------
ModeReferenceModel::ModeReferenceModel(ModeReferenceInterface* modeRefInterface, QObject* parent):
    QAbstractTableModel(parent),
    interface_(modeRefInterface)
{

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

    return interface_->getModeReferenceCount();
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
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
    {
        if (index.column() == 0)
        {
            return QString::number(interface_->getModeReferencePriority(index.row()));
        }

        else if (index.column() == 1)
        {
            return QString::fromStdString(interface_->getModeReferenceValue(index.row()));
        }
    }

    // For getting available component modes.
    if (role == Qt::UserRole && index.column() == 1)
    {
        auto modes = interface_->getComponentModes();

        QStringList modeNames;
        std::for_each(modes.begin(), modes.end(), [&modeNames](std::string const& mode)
            {
                modeNames.append(QString::fromStdString(mode));
            });

        return modeNames;
    }

    // Show error, if mode ref value or priority is not valid.
    if (role == Qt::ForegroundRole)
    {
        bool isValid = false;

        if (index.column() == 0)
        {
            isValid = interface_->modeReferencePriorityIsValid(index.row());

        }
        else if (index.column() == 1)
        {
            isValid = interface_->modeReferenceValueIsValid(index.row());
        }

        return isValid ? KactusColors::REGULAR_TEXT : KactusColors::ERROR;

    }

    if (role == Qt::BackgroundRole)
    {
        return KactusColors::MANDATORY_FIELD;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceModel::setData()
//-----------------------------------------------------------------------------
bool ModeReferenceModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= interface_->getModeReferenceCount() ||
        !(flags(index) & Qt::ItemIsEditable) || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == 0)
    {
        interface_->setModeReferencePriority(index.row(), value.toInt());
    }

    else if (index.column() == 1)
    {
        interface_->setModeReferenceValue(index.row(), value.toString().toStdString());
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
    int lastRow = interface_->getModeReferenceCount();

    beginInsertRows(QModelIndex(), lastRow, lastRow);
    interface_->addModeReference();
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
        rowToRemove >= interface_->getModeReferenceCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
    interface_->removeModeReference(rowToRemove);
    endRemoveRows();

    emit contentChanged();
}
