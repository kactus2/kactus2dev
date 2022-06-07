//-----------------------------------------------------------------------------
// File: memorymapmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The model to manage the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#include "memorymapmodel.h"

#include "MemoryMapColumns.h"

#include <KactusAPI/include/AddressBlockInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::MemoryMapModel()
//-----------------------------------------------------------------------------
MemoryMapModel::MemoryMapModel(AddressBlockInterface* blockInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
MemoryBlockModel(blockInterface, expressionParser, parameterFinder, parent),
localBlockInterface_(blockInterface)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::columnCount()
//-----------------------------------------------------------------------------
int MemoryMapModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return MemoryMapColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::headerData()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == MemoryMapColumns::USAGE_COLUMN)
        {
            return tr("Usage");
        }
        else if (section == MemoryMapColumns::RANGE_COLUMN)
        {
            QString range = tr("Range\n [AUB]") + getExpressionSymbol();
            return range;
        }
        else if (section == MemoryMapColumns::WIDTH_COLUMN)
        {
            QString widthHeader = tr("Width\n [bits]") + getExpressionSymbol();
            return widthHeader;
        }

        else if (section == MemoryMapColumns::ACCESS_COLUMN)
        {
            return tr("Access");
        }
        else if (section == MemoryMapColumns::VOLATILE_COLUMN)
        {
            return tr("Volatile");
        }
	}

    return MemoryBlockModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::data()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= localBlockInterface_->itemCount())
    {
		return QVariant();
	}

    std::string blockName = localBlockInterface_->getIndexedItemName(index.row());

    if (role == Qt::BackgroundRole &&
        (index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN))
    {
        return KactusColors::MANDATORY_FIELD;
    }

    return MemoryBlockModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::setData()
//-----------------------------------------------------------------------------
bool MemoryMapModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= localBlockInterface_->itemCount())
    {
		return false;
	}

    if (Qt::EditRole == role)
    {
        std::string blockName = localBlockInterface_->getIndexedItemName(index.row());

        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            if (!localBlockInterface_->setUsage(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            std::string oldValue = localBlockInterface_->getRangeExpression(blockName);
            if (localBlockInterface_->setRange(blockName, value.toString().toStdString()))
            {
                removeReferencesFromSingleExpression(QString::fromStdString(oldValue));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            std::string oldValue = localBlockInterface_->getWidthExpression(blockName);
            if (localBlockInterface_->setWidth(blockName, value.toString().toStdString()))
            {
                removeReferencesFromSingleExpression(QString::fromStdString(oldValue));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
        {
            if (!localBlockInterface_->setAccess(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            if (!localBlockInterface_->setVolatile(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else 
        {
            return MemoryBlockModel::setData(index, value, role);
        }

        if (index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            emit graphicsChanged(index.row());
        }

        if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            emit childAddressingChanged(index.row());
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
// Function: memorymapmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool MemoryMapModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == MemoryMapColumns::WIDTH_COLUMN || index.column() == MemoryMapColumns::IS_PRESENT ||
        MemoryBlockModel::isValidExpressionColumn(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryMapModel::validateIndex(QModelIndex const& index) const
{
    std::string blockName = localBlockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return localBlockInterface_->hasValidRange(blockName);
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return localBlockInterface_->hasValidWidth(blockName);
    }
    else if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        return localBlockInterface_->hasValidUsage(blockName);
    }

    return MemoryBlockModel::validateIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string blockName = localBlockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getRangeFormattedExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getWidthFormattedExpression(blockName));
    }

    return MemoryBlockModel::formattedExpressionForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::expressionForIndex(QModelIndex const& index) const
{
    std::string blockName = localBlockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getRangeExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getWidthExpression(blockName));
    }

    return MemoryBlockModel::expressionForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::valueForIndex(QModelIndex const& index) const
{
    std::string blockName = localBlockInterface_->getIndexedItemName(index.row());

    if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getUsageString(blockName));
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getRangeValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getWidthValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getAccessString(blockName));
    }
    else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
    {
        return QString::fromStdString(localBlockInterface_->getVolatile(blockName));
    }
    
    return MemoryBlockModel::valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList MemoryMapModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:addressBlock";

    return types;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::nameColumn()
//-----------------------------------------------------------------------------
int MemoryMapModel::nameColumn() const
{
    return MemoryMapColumns::NAME_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::baseAddressColumn()
//-----------------------------------------------------------------------------
int MemoryMapModel::baseAddressColumn() const
{
    return MemoryMapColumns::BASE_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::isPresentColumn()
//-----------------------------------------------------------------------------
int MemoryMapModel::isPresentColumn() const
{
    return MemoryMapColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::descriptionColumn()
//-----------------------------------------------------------------------------
int MemoryMapModel::descriptionColumn() const
{
    return MemoryMapColumns::DESCRIPTION_COLUMN;
}
