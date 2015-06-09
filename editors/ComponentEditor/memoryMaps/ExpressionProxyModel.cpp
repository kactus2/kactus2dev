//-----------------------------------------------------------------------------
// File: ExpressionProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.06.2015
//
// Description:
// Filter model for sorting items by expression value.
//-----------------------------------------------------------------------------

#include "ExpressionProxyModel.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

//-----------------------------------------------------------------------------
// Function: ExpressionProxyModel::ExpressionProxyModel()
//-----------------------------------------------------------------------------
ExpressionProxyModel::ExpressionProxyModel(QSharedPointer<ExpressionParser> expressionParser, QObject *parent):
QSortFilterProxyModel(parent), expressionParser_(expressionParser), expressionColumns_()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionProxyModel::~ExpressionProxyModel()
//-----------------------------------------------------------------------------
ExpressionProxyModel::~ExpressionProxyModel()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionProxyModel::setColumnToAcceptExpressions()
//-----------------------------------------------------------------------------
void ExpressionProxyModel::setColumnToAcceptExpressions(int column)
{
    if (!expressionColumns_.contains(column))
    {
        expressionColumns_.append(column);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionProxyModel::lessThan()
//-----------------------------------------------------------------------------
bool ExpressionProxyModel::lessThan(QModelIndex const& left, QModelIndex const& right ) const
{
    Q_ASSERT(left.column() == right.column());

    if (expressionColumns_.contains(left.column()))
    {
        // convert the data on left index into number
        QString leftExpression = left.data(Qt::EditRole).toString();
        quint64 leftValue = expressionParser_->parseExpression(leftExpression).toUInt();

        // convert data on right index into number
        QString rightExpression = right.data(Qt::EditRole).toString();
        quint64 rightValue = expressionParser_->parseExpression(rightExpression).toUInt();

        return leftValue < rightValue;
    }
    else
    {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
