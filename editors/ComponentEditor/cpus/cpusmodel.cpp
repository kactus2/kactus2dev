//-----------------------------------------------------------------------------
// File: cpusmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// Model for cpu elements within a component.
//-----------------------------------------------------------------------------

#include "cpusmodel.h"

#include "CpuColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <IPXACTmodels/Component/validators/CPUValidator.h>

#include <QStringList>
#include <QRegularExpression>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: CpusModel::CpusModel()
//-----------------------------------------------------------------------------
CpusModel::CpusModel(QSharedPointer<Component> component, QSharedPointer<CPUValidator> validator,
    ExpressionSet expressions,
    QObject* parent): 
    ReferencingTableModel(expressions.finder, parent),
    ParameterizableTable(expressions.finder),
    component_(component),
    cpus_(component->getCpus()),
    validator_(validator),
    expressionFormatter_(expressions.formatter)
{
    setExpressionParser(expressions.parser);
}

//-----------------------------------------------------------------------------
// Function: CpusModel::rowCount()
//-----------------------------------------------------------------------------
int CpusModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return cpus_->count();
}

//-----------------------------------------------------------------------------
// Function: CpusModel::columnCount()
//-----------------------------------------------------------------------------
int CpusModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return CpuColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: CpusModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags CpusModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: CpusModel::headerData()
//-----------------------------------------------------------------------------
QVariant CpusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == CpuColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == CpuColumns::DISPLAY_NAME)
    {
        return tr("Display\nname");
    }
    else if (section == CpuColumns::SHORT_DESCRIPTION)
    {
        return tr("Short\ndescription");
    }
    else if (section == CpuColumns::ADDRSPACE)
    {
        return tr("Address space\nreferences");
    }
    else if (section == CpuColumns::MEMORY_MAP)
    {
        return tr("Memory map\nreference");
    }
    else if (section == CpuColumns::RANGE)
    {
        return tr("Range") + getExpressionSymbol();
    }
    else if (section == CpuColumns::WIDTH)
    {
        return tr("Width\n[bits]") + getExpressionSymbol();
    }
    else if (section == CpuColumns::AUB)
    {
        return tr("Address unit\nbits") + getExpressionSymbol();
    }
    else if (section == CpuColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: CpusModel::data()
//-----------------------------------------------------------------------------
QVariant CpusModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= cpus_->size())
    {
        return QVariant();
    }

    QSharedPointer<Cpu> cpu = cpus_->at(index.row());
    const int column = index.column(); 

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == CpuColumns::DESCRIPTION)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$",
                QRegularExpression::DotMatchesEverythingOption), "...");
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
    else if (role == CpuColumns::USER_DISPLAY_ROLE && column == CpuColumns::ADDRSPACE)
    {
        return cpu->getAddressSpaceRefs();
    }
    else if (role == Qt::ForegroundRole)
    {

        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (column == CpuColumns::NAME || column == CpuColumns::ADDRSPACE ||
            column == CpuColumns::MEMORY_MAP || column == CpuColumns::RANGE || column == CpuColumns::WIDTH)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: CpusModel::setData()
//-----------------------------------------------------------------------------
bool CpusModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_->size())
    {
		return false;
	}

    QSharedPointer<Cpu> cpu = cpus_->at(index.row());
    const int column = index.column();
    if (role == Qt::EditRole)
    {
        if (column == CpuColumns::NAME)
        {
            cpu->setName(value.toString());
        }
        else if (column == CpuColumns::DISPLAY_NAME)
        {
            cpu->setDisplayName(value.toString());		
        }
        else if (column == CpuColumns::SHORT_DESCRIPTION)
        {
            cpu->setShortDescription(value.toString());
        }
        else if (column == CpuColumns::ADDRSPACE)
        {
            QString str = value.toString();
            QStringList addrSpaceNames = str.split(' ', Qt::SkipEmptyParts);
            cpu->setAddressSpaceRefs(addrSpaceNames);
        }
        else if (column == CpuColumns::MEMORY_MAP)
        {
            cpu->setMemoryMapReference(value.toString());
        }
        else if (column == CpuColumns::RANGE)
        {
            cpu->setRange(value.toString());
        }
        else if (column == CpuColumns::WIDTH)
        {
            cpu->setWidth(value.toString());
        }
        else if (column == CpuColumns::AUB)
        {
            cpu->setAddressUnitBits(value.toString());
        }
        else if (column == CpuColumns::DESCRIPTION)
        {
            cpus_->at(index.row())->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}

	// user edit role for interface column operates on QStringList
    else if (role == CpuColumns::USER_EDIT_ROLE && column == CpuColumns::ADDRSPACE)
    {
        cpu->setAddressSpaceRefs(value.toStringList());
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
// Function: CpusModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int CpusModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int total = 0;

    QSharedPointer<Cpu> cpu = cpus_->at(row);
    total += cpu->getRange().count(valueID);
    total += cpu->getWidth().count(valueID);
    total += cpu->getAddressUnitBits().count(valueID);

    return total;
}

//-----------------------------------------------------------------------------
// Function: CpusModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool CpusModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == CpuColumns::RANGE || index.column() == CpuColumns::WIDTH ||
        index.column() == CpuColumns::AUB;
}

//-----------------------------------------------------------------------------
// Function: CpusModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant CpusModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Cpu> cpu = cpus_->at(index.row());
    const int column = index.column();

    if (column == CpuColumns::NAME)
    {
        return cpu->name();
    }
    else if (column == CpuColumns::DISPLAY_NAME)
    {
        return cpu->displayName();
    }
    else if (column == CpuColumns::SHORT_DESCRIPTION)
    {
        return cpu->shortDescription();
    }
    else if (column == CpuColumns::ADDRSPACE)
    {
        QStringList addrSpaceNames = cpu->getAddressSpaceRefs();
        return addrSpaceNames.join(' ');
    }
    else if (column == CpuColumns::MEMORY_MAP)
    {
        return cpu->getMemoryMapReference();
    }
    else if (column == CpuColumns::RANGE)
    {
        return cpu->getRange();
    }
    else if (column == CpuColumns::WIDTH)
    {
        return cpu->getWidth();
    }
    else if (column == CpuColumns::AUB)
    {
        return cpu->getAddressUnitBits();
    }
    else if (column == CpuColumns::DESCRIPTION)
    {
        return cpu->description();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: CpusModel::validateIndex()
//-----------------------------------------------------------------------------
bool CpusModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == CpuColumns::ADDRSPACE &&
        !validator_->hasValidAddressSpaceReferences(cpus_->at(index.row())))
    {
        return false;
    }
    else if (index.column() == CpuColumns::MEMORY_MAP &&
        !validator_->hasValidMemoryMapReference(cpus_->at(index.row())))
    {
        return false;
    }
    else if (index.column() == CpuColumns::RANGE &&
        !validator_->hasValidRange(cpus_->at(index.row())))
    {
        return false;
    }
    else if (index.column() == CpuColumns::WIDTH &&
        !validator_->hasValidWidth(cpus_->at(index.row())))
    {
        return false;
    }
    else if (index.column() == CpuColumns::AUB && 
        !validator_->hasValidAddressUnitBits(cpus_->at(index.row())))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: CpusModel::onAddItem()
//-----------------------------------------------------------------------------
void CpusModel::onAddItem(QModelIndex const& index)
{
	int row = cpus_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	cpus_->insert(row, QSharedPointer<Cpu>(new Cpu()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit cpuAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: CpusModel::onRemoveItem()
//-----------------------------------------------------------------------------
void CpusModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    removeReferencesInItemOnRow(index.row());
	cpus_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit cpuRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: CpusModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant CpusModel::valueForIndex(QModelIndex const& index) const
{
    return expressionOrValueForIndex(index);
}
