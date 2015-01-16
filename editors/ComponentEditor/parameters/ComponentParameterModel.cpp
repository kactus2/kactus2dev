//-----------------------------------------------------------------------------
// File: ComponentParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.01.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ComponentParameterModel.h"
#include "ComponentParameterColumns.h"

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::ComponentParameterModel()
//-----------------------------------------------------------------------------
ComponentParameterModel::ComponentParameterModel(QSharedPointer<Component> component, QObject *parent)
    : QAbstractTableModel(parent), component_(component)
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
int ComponentParameterModel::rowCount(QModelIndex const& parent /*= QModelIndex() */) const
{
    return component_->getParameters().count();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ComponentParameterModel::columnCount(QModelIndex const& parent /*= QModelIndex() */) const
{
    return ComponentParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComponentParameterModel::flags(QModelIndex const& index) const
{
    return QAbstractTableModel::flags(index);

    if (index.column() == ComponentParameterColumns::NAME)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return Qt::NoItemFlags;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterModel::parent()
//-----------------------------------------------------------------------------
QModelIndex ComponentParameterModel::parent(QModelIndex const& child) const
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

    QSharedPointer<Parameter> parameter = component_->getParameters().at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == ComponentParameterColumns::NAME)
        {
            return parameter->getName();
        }
        else if (index.column() == ComponentParameterColumns::VALUE)
        {
            return formattedValueFor(parameter->getValue());
        }
        else if (index.column() == ComponentParameterColumns::ID)
        {
            return parameter->getValueId();
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
   if (index.column() == ComponentParameterColumns::NAME || 
       index.column() == ComponentParameterColumns::ID)
   {
       return !data(index).toString().isEmpty();
   }
   else if (index.column() == ComponentParameterColumns::VALUE)
   {
       return isValuePlainOrExpression(data(index).toString());
   }

   return false;
}
