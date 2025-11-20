//-----------------------------------------------------------------------------
// File: LockableParametersModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.11.2025
//
// Description:
// Implementation of parameters model that allows locking editing of parameters
//-----------------------------------------------------------------------------


#include "LockableParametersModel.h"
#include <KactusAPI/include/ParametersInterface.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::LockableParametersModel()
//-----------------------------------------------------------------------------
LockableParametersModel::LockableParametersModel(ParametersInterface* parameterInterface,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder,
    QObject* parent) :
    ParametersModel(parameterInterface, expressionParser, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::~LockableParametersModel()
//-----------------------------------------------------------------------------
LockableParametersModel::~LockableParametersModel()
{

}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::data()
//-----------------------------------------------------------------------------
QVariant LockableParametersModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::BackgroundRole)
    {
        auto colsToLock = indicesToLock_.value(index.row());

        if (colsToLock.contains(static_cast<ParameterColumns::columns>(index.column())))
        {
            return KactusColors::DISABLED_FIELD;
        }
    }

    return ParametersModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LockableParametersModel::flags(QModelIndex const& index) const
{
    if (indicesToLock_.contains(index.row()) && 
        indicesToLock_.value(index.row())
            .contains(static_cast<ParameterColumns::columns>(index.column())))
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return ParametersModel::flags(index);
}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::lockParameterColumn()
//-----------------------------------------------------------------------------
void LockableParametersModel::lockParameterColumn(QString const& parameterName, ParameterColumns::columns column)
{
    auto paramIdx = getInterface()->getItemIndex(parameterName.toStdString());
    
    if (indicesToLock_.contains(paramIdx))
    {
        indicesToLock_[paramIdx].insert(column);
    }
    else
    {
        indicesToLock_.insert(paramIdx, QSet<ParameterColumns::columns>({column}));
    }
}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::onRemoveItem()
//-----------------------------------------------------------------------------
void LockableParametersModel::onRemoveItem(QModelIndex const& index)
{
    // don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
        return;
    }

    // Locked parameters cannot be removed
    if (indicesToLock_.contains(index.row()))
    {
        return;
    }

    ParametersModel::onRemoveItem(index);
}

//-----------------------------------------------------------------------------
// Function: LockableParametersModel::resetModelItems()
//-----------------------------------------------------------------------------
void LockableParametersModel::resetModelItems()
{
    indicesToLock_.clear();
    ParametersModel::resetModelItems();
}
