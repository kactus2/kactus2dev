//-----------------------------------------------------------------------------
// File: ResetsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// The model to manage the resets of a field.
//-----------------------------------------------------------------------------

#include "ResetsModel.h"

#include <editors/ComponentEditor/memoryMaps/ResetColumns.h>
#include <KactusAPI/include/ResetInterface.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: ResetsModel::ResetsModel()
//-----------------------------------------------------------------------------
ResetsModel::ResetsModel(ResetInterface* resetInterface, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
resetInterface_(resetInterface)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::~ResetsModel()
//-----------------------------------------------------------------------------
ResetsModel::~ResetsModel()
{

}

//-----------------------------------------------------------------------------
// Function: ResetsModel::rowCount()
//-----------------------------------------------------------------------------
int ResetsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return resetInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::columnCount()
//-----------------------------------------------------------------------------
int ResetsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ResetColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ResetsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ResetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == ResetColumns::RESETTYPEREFERENCE_COLUMN)
        {
            return tr("Reset type\nreference");
        }
        else if (section == ResetColumns::RESETVALUE_COLUMN)
        {
            return tr("Reset\nvalue") + getExpressionSymbol();
        }
        else if (section == ResetColumns::RESETMASK_COLUMN)
        {
            return tr("Reset\nmask") + getExpressionSymbol();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::data()
//-----------------------------------------------------------------------------
QVariant ResetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    else if (index.row() < 0 || index.row() >= resetInterface_->itemCount())
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (Qt::ForegroundRole == role)
    {
        if (validateIndex(index))
        {
            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::ERROR;
        }
    }
    else if (Qt::BackgroundRole == role)
    {
        if (index.column() == ResetColumns::RESETVALUE_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant ResetsModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetValueFormattedExpression(index.row()));
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetMaskFormattedExpression(index.row()));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant ResetsModel::expressionForIndex(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetValueExpression(index.row()));
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetMaskExpression(index.row()));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant ResetsModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        std::string resetType = resetInterface_->getResetTypeReference(index.row());
        return QString::fromStdString(resetType);
    }
    else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetValue(index.row()));
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return QString::fromStdString(resetInterface_->getResetMaskValue(index.row()));
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::setData()
//-----------------------------------------------------------------------------
bool ResetsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= resetInterface_->itemCount())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        int indexRow = index.row();
        std::string newValue = value.toString().toStdString();

        if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
        {
            resetInterface_->setResetTypeReference(indexRow, newValue);
        }
        else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(resetInterface_->getResetValueExpression(indexRow)));
            }

            resetInterface_->setResetValue(indexRow, newValue);
        }
        else if (index.column() == ResetColumns::RESETMASK_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(resetInterface_->getResetMaskExpression(indexRow)));
            }

            resetInterface_->setResetMask(indexRow, newValue);
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool ResetsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETVALUE_COLUMN || index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ResetsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index).toString();
    }
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::validateIndex()
//-----------------------------------------------------------------------------
bool ResetsModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        return resetInterface_->hasValidResetType(index.row());
    }
    else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return resetInterface_->hasValidResetValue(index.row());
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return resetInterface_->hasValidResetMask(index.row());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int ResetsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return resetInterface_->getAllReferencesToIdInItem(
        resetInterface_->getResetTypeReference(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::onAddItem()
//-----------------------------------------------------------------------------
void ResetsModel::onAddItem(const QModelIndex& index)
{
    int row = resetInterface_->itemCount();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    resetInterface_->addReset(row);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ResetsModel::onRemoveItem(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }
    else if (index.row() < 0 || index.row() >= resetInterface_->itemCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());
    resetInterface_->removeReset(index.row());
    endRemoveRows();

    emit contentChanged();
}
