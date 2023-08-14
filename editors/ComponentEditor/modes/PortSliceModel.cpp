//-----------------------------------------------------------------------------
// File: PortSliceModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#include "PortSliceModel.h"

#include "PortSliceColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <QStringList>
#include <QRegularExpression>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: PortSliceModel::PortSliceModel()
//-----------------------------------------------------------------------------
PortSliceModel::PortSliceModel(QSharedPointer <Mode> mode,
    ExpressionSet expressions,
    QObject* parent): 
    ReferencingTableModel(expressions.finder, parent),
    ParameterizableTable(expressions.finder),
    portSlices_(mode->getPortSlices()),
    expressionFormatter_(expressions.formatter)
{
    
    setExpressionParser(expressions.parser);
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::rowCount()
//-----------------------------------------------------------------------------
int PortSliceModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return portSlices_->count();
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::columnCount()
//-----------------------------------------------------------------------------
int PortSliceModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return PortSliceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortSliceModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortSliceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == PortSliceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == PortSliceColumns::PORT_REF)
    {
        return tr("Physical port");
    }
    else if (section == PortSliceColumns::SUB_PORT)
    {
        return tr("Sub-port");
    }
    else if (section == PortSliceColumns::LEFT_BOUND)
    {
        return tr("Left bound") + getExpressionSymbol();
    }
    else if (section == PortSliceColumns::RIGHT_BOUND)
    {
        return tr("Right bound") + getExpressionSymbol();
    }
    else if (section == PortSliceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::data()
//-----------------------------------------------------------------------------
QVariant PortSliceModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= portSlices_->size())
    {
        return QVariant();
    }

    QSharedPointer<PortSlice> slice = portSlices_->at(index.row());
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
        return expressionOrValueForIndex(index);
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
        if (column == PortSliceColumns::NAME ||
            column == PortSliceColumns::PORT_REF)
        {
            return KactusColors::MANDATORY_FIELD;
        }
    }


    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::setData()
//-----------------------------------------------------------------------------
bool PortSliceModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= portSlices_->size() || role != Qt::EditRole)
    {
        return false;
    }

    QSharedPointer<PortSlice> slice = portSlices_->at(index.row());
    const int column = index.column();

    if (column == PortSliceColumns::NAME)
    {
        slice->setName(value.toString());
    }
    else if (column == PortSliceColumns::PORT_REF)
    {
        slice->setPortRef(value.toString());
    }
    else if (column == PortSliceColumns::LEFT_BOUND)
    {
        slice->setLeftRange(value.toString());
    }
    else if (column == PortSliceColumns::RIGHT_BOUND)
    {
        slice->setRightRange(value.toString());
    }
    else if (column == PortSliceColumns::DESCRIPTION)
    {
        portSlices_->at(index.row())->setDescription(value.toString());
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
// Function: PortSliceModel::onAddItem()
//-----------------------------------------------------------------------------
void PortSliceModel::onAddItem(QModelIndex const& index)
{
	int row = portSlices_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	portSlices_->insert(row, QSharedPointer<PortSlice>(new PortSlice()));
	endInsertRows();


	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PortSliceModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= portSlices_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	portSlices_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PortSliceModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInLeft = portSlices_->at(row)->getLeftRange().count(valueID);
    int referencesInRight = portSlices_->at(row)->getRightRange().count(valueID);

    return referencesInLeft + referencesInRight;
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PortSliceModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == PortSliceColumns::LEFT_BOUND || index.column() == PortSliceColumns::RIGHT_BOUND;
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortSliceModel::expressionOrValueForIndex(QModelIndex const& index) const
{
//     const int column = index.column();
// 
//     QSharedPointer<PortSlice> slice = portSlices_->at(index.row());
//     if (column == PortSliceColumns::LEFT_BOUND)
//     {
//         return slice->getLeftRange();
//     }
//     else if (column == PortSliceColumns::RIGHT_BOUND)
//     {
//         return slice->getRightRange();
//     }
//     else
//     {
        return valueForIndex(index);
   // }
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::validateIndex()
//-----------------------------------------------------------------------------
bool PortSliceModel::validateIndex(QModelIndex const& index) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortSliceModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortSliceModel::valueForIndex(QModelIndex const& index) const
{
    const int column = index.column();

    QSharedPointer<PortSlice> slice = portSlices_->at(index.row());

    if (column == PortSliceColumns::NAME)
    {
        return slice->name();
    }
    else if (column == PortSliceColumns::PORT_REF)
    {
        return slice->getPortRef();
    }
    else if (column == PortSliceColumns::LEFT_BOUND)
    {
        return slice->getLeftRange();
    }
    else if (column == PortSliceColumns::RIGHT_BOUND)
    {
        return slice->getRightRange();
    }
    else if (column == PortSliceColumns::DESCRIPTION)
    {
        return slice->description();
    }

    return QVariant();
}
