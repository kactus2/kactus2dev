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

#include <common/KactusColors.h>

#include <IPXACTmodels/Component/validators/FieldValidator.h>

//-----------------------------------------------------------------------------
// Function: ResetsModel::ResetsModel()
//-----------------------------------------------------------------------------
ResetsModel::ResetsModel(QSharedPointer<QList<QSharedPointer<FieldReset> > > resets,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<FieldValidator> fieldValidator,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
resets_(resets),
expressionFormatter_(expressionFormatter),
fieldValidator_(fieldValidator)
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
    return resets_->size();
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
    else if (index.row() < 0 || index.row() >= resets_->size())
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionOrValueForIndex(index);
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
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
// Function: ResetsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant ResetsModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        return resets_->at(index.row())->getResetTypeReference();
    }
    else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return resets_->at(index.row())->getResetValue();
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return resets_->at(index.row())->getResetMask();
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
    if (!index.isValid() || index.row() < 0 || index.row() >= resets_->size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<FieldReset> currentReset = resets_->at(index.row());
        QString newValue = value.toString();

        if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
        {
            currentReset->setResetTypeReference(newValue);
        }
        else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(currentReset->getResetValue());
            }

            currentReset->setResetValue(newValue);
        }
        else if (index.column() == ResetColumns::RESETMASK_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(currentReset->getResetMask());
            }

            currentReset->setResetMask(newValue);
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
    if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return resets_->at(index.row())->getResetValue();
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return resets_->at(index.row())->getResetMask();
    }

    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::validateIndex()
//-----------------------------------------------------------------------------
bool ResetsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<FieldReset> currentReset = resets_->at(index.row());

    if (index.column() == ResetColumns::RESETTYPEREFERENCE_COLUMN)
    {
        return fieldValidator_->hasValidResetTypeReference(currentReset);
    }
    else if (index.column() == ResetColumns::RESETVALUE_COLUMN)
    {
        return fieldValidator_->hasValidResetValue(currentReset);
    }
    else if (index.column() == ResetColumns::RESETMASK_COLUMN)
    {
        return fieldValidator_->hasValidResetMask(currentReset);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int ResetsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInResetValue = resets_->at(row)->getResetValue().count(valueID);
    int referencesInResetMask = resets_->at(row)->getResetMask().count(valueID);

    int totalReferences = referencesInResetValue + referencesInResetMask;

    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: ResetsModel::onAddItem()
//-----------------------------------------------------------------------------
void ResetsModel::onAddItem(const QModelIndex& index)
{
    int row = resets_->size();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    QSharedPointer<FieldReset> newReset(new FieldReset());
    resets_->insert(row, newReset);
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
    else if (index.row() < 0 || index.row() >= resets_->size())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());
    resets_->removeAt(index.row());

    endRemoveRows();

    emit contentChanged();
}
