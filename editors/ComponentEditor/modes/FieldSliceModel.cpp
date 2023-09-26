//-----------------------------------------------------------------------------
// File: FieldSliceModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#include "FieldSliceModel.h"

#include "FieldSliceColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <QStringList>
#include <QRegularExpression>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::FieldSliceModel()
//-----------------------------------------------------------------------------
FieldSliceModel::FieldSliceModel(QSharedPointer <Mode> mode,
    QSharedPointer<FieldSliceValidator> validator,
    ExpressionSet expressions,
    QObject* parent): 
    ReferencingTableModel(expressions.finder, parent),
    ParameterizableTable(expressions.finder),
    validator_(validator),
    fieldSlices_(mode->getFieldSlices()),
    expressionFormatter_(expressions.formatter)
{
    setExpressionParser(expressions.parser);

}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::rowCount()
//-----------------------------------------------------------------------------
int FieldSliceModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return fieldSlices_->count();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::columnCount()
//-----------------------------------------------------------------------------
int FieldSliceModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return FieldSliceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FieldSliceModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::headerData()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == FieldSliceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == FieldSliceColumns::FIELD_REF)
    {
        return tr("Field");
    }
    else if (section == FieldSliceColumns::RANGE_LEFT)
    {
        return tr("Leftmost bit") + getExpressionSymbol();
    }
    else if (section == FieldSliceColumns::RANGE_RIGHT)
    {
        return tr("Rightmost bit") + getExpressionSymbol();
    }
    else if (section == FieldSliceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::data()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size())
    {
        return QVariant();
    }

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());
    const int column = index.column(); 

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }

    else if (role == Qt::EditRole)
    {
        if (column == FieldSliceColumns::FIELD_REF)
        {
            return getFieldRefPath(slice);
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
            return expressionOrValueForIndex(index);
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }
     
    else if (role == Qt::BackgroundRole)
    {
        if (column == FieldSliceColumns::NAME || column == FieldSliceColumns::FIELD_REF)
        {
            return KactusColors::MANDATORY_FIELD;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::setData()
//-----------------------------------------------------------------------------
bool FieldSliceModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size() || role != Qt::EditRole)
    {
        return false;
    }

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());
    const int column = index.column();

    if (column == FieldSliceColumns::NAME)
    {
        slice->setName(value.toString());
    }
    else if (column == FieldSliceColumns::FIELD_REF)
    {
        setFieldRef(slice, value.toStringList());
    }
    else if (column == FieldSliceColumns::RANGE_LEFT)
    {
        slice->setLeft(value.toString());
    }
    else if (column == FieldSliceColumns::RANGE_RIGHT)
    {
        slice->setRight(value.toString());
    }
    else if (column == FieldSliceColumns::DESCRIPTION)
    {
        fieldSlices_->at(index.row())->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    emit contentChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::onAddItem()
//-----------------------------------------------------------------------------
void FieldSliceModel::onAddItem(QModelIndex const& index)
{
	int row = fieldSlices_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fieldSlices_->insert(row, QSharedPointer<FieldSlice>(new FieldSlice()));
	endInsertRows();


	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FieldSliceModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fieldSlices_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}


//-----------------------------------------------------------------------------
// Function: FieldSliceModel::validateIndex()
//-----------------------------------------------------------------------------
bool FieldSliceModel::validateIndex(QModelIndex const& index) const
{
    const int column = index.column();
    auto fieldSlice = fieldSlices_->at(index.row());

    if (column == FieldSliceColumns::NAME)
    {
        return validator_->hasValidName(fieldSlice->name());
    }
    else if (column == FieldSliceColumns::FIELD_REF)
    {
        return validator_->hasValidFieldReference(fieldSlice);
    }
    else if (column == FieldSliceColumns::RANGE_LEFT)
    {
        return validator_->hasValidLeftRange(fieldSlice);
    }
    else if (column == FieldSliceColumns::RANGE_RIGHT)
    {
        return validator_->hasValidRightRange(fieldSlice);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int FieldSliceModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    auto fieldSlice = fieldSlices_->at(row);

    int referencesInLeft = fieldSlice->getLeft().count(valueID);
    int referencesInRight = fieldSlice->getRight().count(valueID);

    return referencesInLeft + referencesInRight;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool FieldSliceModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == FieldSliceColumns::RANGE_LEFT || index.column() == FieldSliceColumns::RANGE_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::valueForIndex(QModelIndex const& index) const
{
    const int column = index.column();

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());

    if (column == FieldSliceColumns::NAME)
    {
        return slice->name();
    }
    else if (column == FieldSliceColumns::FIELD_REF)
    {
        auto ref = slice->getReference(FieldReference::FIELD);
        if (ref.isNull() == false)
        {
            return ref->reference_;
        }
    }
    else if (column == FieldSliceColumns::RANGE_LEFT)
    {
        return slice->getLeft();
    }
    else if (column == FieldSliceColumns::RANGE_RIGHT)
    {
        return slice->getRight();
    }
    else if (column == FieldSliceColumns::DESCRIPTION)
    {
        return slice->description();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::getFieldRefPath()
//-----------------------------------------------------------------------------
QStringList FieldSliceModel::getFieldRefPath(QSharedPointer<FieldSlice> slice) const
{
    QStringList path;

    auto ref = slice->getReference(FieldReference::MEMORY_MAP);
    if (ref)
    {
        path.append("Memory map/" + ref->reference_);
    }

    ref = slice->getReference(FieldReference::ADDRESS_SPACE);
    if (ref)
    {
        path.append("Address space/" + ref->reference_);
    }

    ref = slice->getReference(FieldReference::ADDRESS_BLOCK);
    if (ref)
    {
        path.append("Address block/" + ref->reference_);
    }

    auto registerFiles = slice->getMultipleReference(FieldReference::REGISTER_FILE);
    if (registerFiles)
    {
        for (auto registerFile : *registerFiles)
        {
            path.append("Register file/" + registerFile->reference_);
        }
    }

    ref = slice->getReference(FieldReference::REGISTER);
    if (ref)
    {
        path.append("Register/" + ref->reference_);
    }

    ref = slice->getReference(FieldReference::FIELD);
    if (ref)
    {
        path.append("Field/" + ref->reference_);
    }

    return path;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::setFieldRef()
//-----------------------------------------------------------------------------
void FieldSliceModel::setFieldRef(QSharedPointer<FieldSlice> slice, QStringList const& fieldPath)
{
    QMap<QString, FieldReference::Type> refMap({
        { QStringLiteral("Address space"), FieldReference::ADDRESS_SPACE },
        { QStringLiteral("Memory map"), FieldReference::MEMORY_MAP },
        { QStringLiteral("Address block"), FieldReference::ADDRESS_BLOCK },
        { QStringLiteral("Register file"), FieldReference::REGISTER_FILE },
        { QStringLiteral("Register"), FieldReference::REGISTER },
        { QStringLiteral("Field"), FieldReference::FIELD }
     });

    slice->clearReference(FieldReference::MEMORY_MAP);
    slice->clearReference(FieldReference::ADDRESS_SPACE);
    slice->clearReference(FieldReference::ADDRESS_BLOCK);
    slice->clearReference(FieldReference::REGISTER_FILE);
    slice->clearReference(FieldReference::REGISTER);
    slice->clearReference(FieldReference::FIELD);

    for (auto const& step : fieldPath)
    {
        auto parts = step.split("/", Qt::SkipEmptyParts);

        auto ref = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference());
        ref->reference_ = parts.last();
        
        slice->setReference(ref, refMap[parts.first()]);
    }
}
