//-----------------------------------------------------------------------------
// File: ModuleParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 29.3.2011
//
// Description:
// This model can be used to edit and update a list of module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterModel.h"
#include "ModuleParameterColumns.h"

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <KactusAPI/include/ParametersInterface.h>

#include <common/KactusColors.h>

#include <QPersistentModelIndex>

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::ModuleParameterModel()
//-----------------------------------------------------------------------------
ModuleParameterModel::ModuleParameterModel(ModuleParameterInterface* moduleParameterInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
AbstractParameterModel(moduleParameterInterface, expressionParser, parameterFinder, parent),
editingDisabled_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ModuleParameterModel::columnCount(QModelIndex const& parent /*= QModelIndex() */ ) const
{
	// not hierarchical model
	if (parent.isValid())
    {
		return 0;
    }

	return ModuleParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ModuleParameterModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }

    std::string parameterName = getInterface()->getIndexedItemName(index.row());

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        ModuleParameterInterface* moduleInterface = dynamic_cast<ModuleParameterInterface*>(getInterface());

        if (moduleInterface)
        {
            if (index.column() == ModuleParameterColumns::DATA_TYPE)
            {
                return QString::fromStdString(moduleInterface->getDataType(parameterName));
            }
            else if (index.column() == ModuleParameterColumns::USAGE_TYPE)
            {
                return QString::fromStdString(moduleInterface->getUsageType(parameterName));
            }
        }
    }

    else if (Qt::ForegroundRole == role)
    {
        if (!validateIndex(index))
        {
             return KactusColors::ERROR;
        }
        else if (editingDisabled_)
        {
            return KactusColors::DISABLED_TEXT;
        }
    }

    return AbstractParameterModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModuleParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
    {	
		if (section ==  ModuleParameterColumns::DATA_TYPE)
        {
            return tr("Data\ntype");
        }
        else if (section == ModuleParameterColumns::USAGE_TYPE)
        {
            return tr("OO usage");
        }
	}
	
    return AbstractParameterModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::setData()
//-----------------------------------------------------------------------------
bool ModuleParameterModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount() || editingDisabled_)
    {
		return false;
    }

	if (role == Qt::EditRole)
    {
        std::string parameterName = getInterface()->getIndexedItemName(index.row());

        if (index.column() == ModuleParameterColumns::DATA_TYPE ||
            index.column() == ModuleParameterColumns::USAGE_TYPE)
        {
            ModuleParameterInterface* moduleInterface = dynamic_cast<ModuleParameterInterface*>(getInterface());

            if (moduleInterface)
            {
                if (index.column() == ModuleParameterColumns::DATA_TYPE)
                {
                    moduleInterface->setDataType(parameterName, value.toString().toStdString());
                }
                else if (index.column() == ModuleParameterColumns::USAGE_TYPE)
                {
                    moduleInterface->setUsageType(parameterName, value.toString().toStdString());
                }

                emit dataChanged(index, index);
                return true;
            }
        }
        else
        {
            return AbstractParameterModel::setData(index, value, role);
        }
	}
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModuleParameterModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (editingDisabled_)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return AbstractParameterModel::flags(index);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::onAddItem()
//-----------------------------------------------------------------------------
void ModuleParameterModel::onAddItem(QModelIndex const& index)
{
    if (editingDisabled_)
    {
        return;
    }

    int row = rowCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    ModuleParameterInterface* moduleInterface = dynamic_cast<ModuleParameterInterface*>(getInterface());
    if (moduleInterface)
    {
        beginInsertRows(QModelIndex(), row, row);
        moduleInterface->addModuleParameter(row);
        endInsertRows();
    }

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModuleParameterModel::onRemoveItem(QModelIndex const& index)
{
    int indexRow = index.row();

	// don't remove anything if index is invalid
    if (!index.isValid() || indexRow < 0 || indexRow >= rowCount() || editingDisabled_)
    {
		return;
	}

    if (canRemoveRow(indexRow))
    {
        ModuleParameterInterface* moduleInterface = dynamic_cast<ModuleParameterInterface*>(getInterface());
        if (moduleInterface)
        {
            std::string parameterName = getInterface()->getIndexedItemName(indexRow);

            // remove the specified item
            beginRemoveRows(QModelIndex(), indexRow, indexRow);
            moduleInterface->removeModuleParameter(parameterName);
            endRemoveRows();

            // tell also parent widget that contents have been changed
            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::nameColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::nameColumn() const
{
    return ModuleParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::displayNameColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::displayNameColumn() const
{
    return ModuleParameterColumns::DISPLAY_NAME;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::formatColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::typeColumn() const
{
    return ModuleParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::bitWidthLeftColumn() const
{
    return ModuleParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::bitWidthRightColumn() const
{
    return ModuleParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::minimumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::minimumColumn() const
{
    return ModuleParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::maximumColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::maximumColumn() const
{
    return ModuleParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::choiceColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::choiceColumn() const
{
    return ModuleParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::valueColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::valueColumn() const
{
    return ModuleParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::resolveColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::resolveColumn() const
{
    return ModuleParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::arrayLeftColumn() const
{
    return ModuleParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::arrayRightColumn() const
{
    return ModuleParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::descriptionColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::descriptionColumn() const
{
    return ModuleParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::idColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::idColumn() const
{
    return ModuleParameterColumns::ID;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::usageCountColumn()
//-----------------------------------------------------------------------------
int ModuleParameterModel::usageCountColumn() const
{
    return ModuleParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::resetModelItems()
//-----------------------------------------------------------------------------
void ModuleParameterModel::resetModelItems()
{
    beginResetModel();
    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::enableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterModel::enableEditing()
{
    editingDisabled_ = false;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterModel::disableEditing()
//-----------------------------------------------------------------------------
void ModuleParameterModel::disableEditing()
{
    editingDisabled_ = true;
}
