//-----------------------------------------------------------------------------
// File: ComponentParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.01.2015
//
// Description:
// Model for all parameter completions within a single component.
//-----------------------------------------------------------------------------

#include "ComponentParameterModel.h"
#include "ComponentParameterColumns.h"

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::ComponentParameterModel()
//-----------------------------------------------------------------------------
ComponentParameterModel::ComponentParameterModel(QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
    ReferencingTableModel(parameterFinder, parent),
    ParameterizableTable(parameterFinder),
    parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::~ComponentParameterModel()
//-----------------------------------------------------------------------------
ComponentParameterModel::~ComponentParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ComponentParameterModel::rowCount(QModelIndex const& /*parent= QModelIndex() */) const
{
    return parameterFinder_->getNumberOfParameters();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ComponentParameterModel::columnCount(QModelIndex const& /*parent = QModelIndex() */) const
{
    return ComponentParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComponentParameterModel::flags(QModelIndex const& index) const
{
    if (index.isValid())
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    
    return Qt::NoItemFlags;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::parent()
//-----------------------------------------------------------------------------
QModelIndex ComponentParameterModel::parent(QModelIndex const& /*child*/) const
{
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ComponentParameterModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid() || index.row() > rowCount())
    {
        return QVariant();
    }

    QStringList parameterIds = parameterFinder_->getAllParameterIds();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == ComponentParameterColumns::NAME)
        {
            return parameterFinder_->nameForId(parameterIds.at(index.row()));
        }
        else if (index.column() == ComponentParameterColumns::VALUE)
        {
            return formattedValueFor(parameterFinder_->valueForId(parameterIds.at(index.row())));
        }
        else if (index.column() == ComponentParameterColumns::ID)
        {
            return parameterIds.at(index.row());
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool ComponentParameterModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == ComponentParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ComponentParameterModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    return data(index);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool ComponentParameterModel::validateColumnForParameter(QModelIndex const& index) const
{
   if (index.column() == ComponentParameterColumns::NAME || index.column() == ComponentParameterColumns::ID)
   {
       return !data(index).toString().isEmpty();
   }
   else if (index.column() == ComponentParameterColumns::VALUE)
   {
       return isValuePlainOrExpression(data(index).toString());
   }

   return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int ComponentParameterModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    QStringList parameterIDs = parameterFinder_->getAllParameterIds();

    int referencesInValue = parameterFinder_->valueForId(parameterIDs.at(row)).count(valueID);

    return referencesInValue;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::resetParameters()
//-----------------------------------------------------------------------------
void ComponentParameterModel::resetParameterModel()
{
    beginResetModel();
    endResetModel();
}
