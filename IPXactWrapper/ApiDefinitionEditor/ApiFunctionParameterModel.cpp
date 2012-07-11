//-----------------------------------------------------------------------------
// File: ApiFunctionParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Model for API function parameters.
//-----------------------------------------------------------------------------

#include "ApiFunctionParameterModel.h"

#include "ApiFunctionParameterDelegate.h"

#include <models/ApiFunction.h>
#include <models/ApiFunctionParameter.h>

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::ApiFunctionParameterModel()
//-----------------------------------------------------------------------------
ApiFunctionParameterModel::ApiFunctionParameterModel(QObject *parent)
    : QAbstractTableModel(parent),
      func_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::~ApiFunctionParameterModel()
//-----------------------------------------------------------------------------
ApiFunctionParameterModel::~ApiFunctionParameterModel()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::setFunction()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::setFunction(QSharedPointer<ApiFunction> func)
{
    beginResetModel();
    func_ = func;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ApiFunctionParameterModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid() || func_ == 0)
    {
        return 0;
    }

    return func_->getParamCount();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ApiFunctionParameterModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return API_FUNC_PARAM_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ApiFunctionParameterModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    else if (func_ == 0 || index.row() < 0 || index.row() >= func_->getParamCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case API_FUNC_PARAM_COL_NAME:
            return func_->getParam(index.row())->getName();

        case API_FUNC_PARAM_COL_TYPE:
            return func_->getParam(index.row())->getType();

        case API_FUNC_PARAM_COM_TRANSFER_TYPE:
            {
                QString transferType = func_->getParam(index.row())->getComTransferType();
                
                if (transferType == "")
                {
                    return tr("any");
                }
                
                return transferType;
            }

        case API_FUNC_PARAM_COM_DIRECTION:
            {
                General::Direction direction = func_->getParam(index.row())->getComDirection();

                if (direction == General::DIRECTION_INVALID)
                {
                    return tr("any");
                }

                return General::direction2Str(direction);
            }

        case API_FUNC_PARAM_CONTENT_SOURCE:
            return func_->getParam(index.row())->getContentSource();

        case API_FUNC_PARAM_DEPENDENT_PARAM:
            {
                int paramIndex = func_->getParam(index.row())->getDependentParameterIndex();

                if (paramIndex < 0)
                {
                    return "no";
                }
                
                return (func_->getParam(index.row())->getDependentParameterIndex() + 1);
            }

        case API_FUNC_PARAM_COL_DESC:
            return func_->getParam(index.row())->getDescription();

        default:
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ApiFunctionParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case API_FUNC_PARAM_COL_NAME:
                return tr("Name");

            case API_FUNC_PARAM_COL_TYPE:
                return tr("Type");

            case API_FUNC_PARAM_COM_TRANSFER_TYPE:
                return tr("COM transfer type");
                
            case API_FUNC_PARAM_COM_DIRECTION:
                return tr("COM direction");

            case API_FUNC_PARAM_CONTENT_SOURCE:
                return tr("Source");

            case API_FUNC_PARAM_DEPENDENT_PARAM:
                return tr("Dependent parameter");

            case API_FUNC_PARAM_COL_DESC:
                return tr("Description");

            default:
                return QVariant();
            }
        }
        else
        {
            return section + 1;
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ApiFunctionParameterModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    switch (index.column())
    {
    case API_FUNC_PARAM_COM_TRANSFER_TYPE:
    case API_FUNC_PARAM_COM_DIRECTION:
    case API_FUNC_PARAM_DEPENDENT_PARAM:
        {
            if (func_->getParam(index.row())->getContentSource().isEmpty())
            {
                return Qt::ItemIsSelectable;
            }
            else
            {
                return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
            }
        }

    default:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::setData()
//-----------------------------------------------------------------------------
bool ApiFunctionParameterModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid())
    {
        return false;
    }
    else if (func_ == 0 || index.row() < 0 || index.row() >= func_->getParamCount())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case API_FUNC_PARAM_COL_NAME:
            {
                func_->getParam(index.row())->setName(value.toString());
                break;
            }

        case API_FUNC_PARAM_COL_TYPE:
            {
                func_->getParam(index.row())->setType(value.toString());
                break;
            }

        case API_FUNC_PARAM_COM_TRANSFER_TYPE:
            {
                if (value.toString() == tr("any"))
                {
                    func_->getParam(index.row())->setComTransferType("");
                }
                else
                {
                    func_->getParam(index.row())->setComTransferType(value.toString());
                }

                break;
            }

        case API_FUNC_PARAM_COM_DIRECTION:
            {
                func_->getParam(index.row())->setComDirection(General::str2Direction(value.toString(), General::DIRECTION_INVALID));
                break;
            }

        case API_FUNC_PARAM_CONTENT_SOURCE:
            {
                if (value.toString().isEmpty())
                {
                    func_->getParam(index.row())->setComDirection(General::DIRECTION_INVALID);
                    func_->getParam(index.row())->setComTransferType("");
                    func_->getParam(index.row())->setDependentParameterIndex(-1);
                }

                func_->getParam(index.row())->setContentSource(value.toString());

                emit dataChanged(index, this->index(index.row(), index.column() + 3));
                break;
            }

        case API_FUNC_PARAM_DEPENDENT_PARAM:
            {
                QString text = value.toString();

                if (text == "no")
                {
                    func_->getParam(index.row())->setDependentParameterIndex(-1);
                }
                else
                {
                    func_->getParam(index.row())->setDependentParameterIndex(value.toInt() - 1);
                }
                break;
            }

        case API_FUNC_PARAM_COL_DESC:
            {
                func_->getParam(index.row())->setDescription(value.toString());
                break;
            }

        default:
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
// Function: ApiFunctionParameterModel::isValid()
//-----------------------------------------------------------------------------
bool ApiFunctionParameterModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onAdd()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onAdd()
{
    beginInsertRows(QModelIndex(), func_->getParamCount(), func_->getParamCount());
    func_->addParam(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter()));
    endInsertRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onAddItem()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onAddItem(QModelIndex const& index)
{
    int row = func_->getParamCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    func_->addParam(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter()), row);
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onRemove()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onRemove(QModelIndex const& index)
{
    if (!index.isValid() || func_ == 0)
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    func_->removeParam(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ApiFunctionParameterModel::onRemoveItem(QModelIndex const& index)
{
    // don't remove anything if index is invalid
    if (!index.isValid())
    {
        return;
    }
    // make sure the row number if valid
    else if (func_ == 0 || index.row() < 0 || index.row() >= func_->getParamCount())
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    func_->removeParam(index.row());
    endRemoveRows();

    emit contentChanged();
}

void ApiFunctionParameterModel::onMoveItem( const QModelIndex& originalPos, const QModelIndex& newPos ) {

    // if there was no item in the starting point
    if (!originalPos.isValid())
    {
        return;
    }
    // if the indexes are the same
    else if (originalPos == newPos)
    {
        return;
    }
    else if (originalPos.row() < 0 || originalPos.row() >= func_->getParamCount())
    {
        return;
    }

    // if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= func_->getParamCount())
    {

        beginResetModel();

        QSharedPointer<ApiFunctionParameter> param = func_->getParam(originalPos.row());
        func_->removeParam(originalPos.row());
        func_->addParam(param);

        endResetModel();
    }
    // if both indexes were valid
    else {
        beginResetModel();
        
        QSharedPointer<ApiFunctionParameter> param = func_->getParam(originalPos.row());
        func_->removeParam(originalPos.row());
        func_->addParam(param, newPos.row());

        endResetModel();
    }

    emit contentChanged();
}