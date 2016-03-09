//-----------------------------------------------------------------------------
// File: cpusmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <QColor>

//-----------------------------------------------------------------------------
// Function: CpusModel::CpusModel()
//-----------------------------------------------------------------------------
CpusModel::CpusModel(QSharedPointer<Component> component, QSharedPointer<CPUValidator> validator,
    QObject* parent): QAbstractTableModel(parent),
    component_(component),
    cpus_(component->getCpus()),
    validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: CpusModel::~CpusModel()
//-----------------------------------------------------------------------------
CpusModel::~CpusModel()
{
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

	return cpus_->size();
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

    if (section == CpuColumns::NAME_COLUMN)
    {
        return tr("Name");
    }
    else if (section == CpuColumns::DISPLAY_NAME_COLUMN)
    {
        return tr("Display name");
    }
    else if (section == CpuColumns::ADDRSPACE_COLUMN)
    {
        return tr("Address space references");
    }
    else if (section == CpuColumns::DESCRIPTION_COLUMN)
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

	if (role == Qt::DisplayRole)
    {
        if (index.column() == CpuColumns::NAME_COLUMN)
        {
            return cpu->name();
        }
        else if (index.column() == CpuColumns::DISPLAY_NAME_COLUMN)
        {
            return cpu->displayName();
        }
        else if (index.column() == CpuColumns::ADDRSPACE_COLUMN)
        {
            QStringList addrSpaceNames = cpu->getAddressSpaceRefs();
            return addrSpaceNames.join(' ');
        }
        else if (index.column() == CpuColumns::DESCRIPTION_COLUMN)
        {
            return cpu->description();
        }
        else
        {
            return QVariant();
        }
	}
    // user display role for interface column returns a QStringList
    else if (role == CpuColumns::USER_DISPLAY_ROLE && index.column() == CpuColumns::ADDRSPACE_COLUMN)
    {
        return cpu->getAddressSpaceRefs();
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == CpuColumns::ADDRSPACE_COLUMN && 
            !validator_->hasValidAddressSpaceReferences(cpus_->at(index.row())))
        {
            return QColor("red");
        }
        else
        {
           return QColor("black");
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == CpuColumns::NAME_COLUMN || index.column() == CpuColumns::ADDRSPACE_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
            return QColor("white");
    }
    else
    {
        return QVariant();
    }
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

    if (role == Qt::EditRole)
    {
        if (index.column() == CpuColumns::NAME_COLUMN)
        {
            cpu->setName(value.toString());
        }
        else if (index.column() == CpuColumns::DISPLAY_NAME_COLUMN)
        {
            cpu->setDisplayName(value.toString());		
        }
        else if (index.column() == CpuColumns::ADDRSPACE_COLUMN)
        {
            QString str = value.toString();
            QStringList addrSpaceNames = str.split(' ', QString::SkipEmptyParts);
            cpu->setAddressSpaceRefs(addrSpaceNames);
        }
        else if (index.column() == CpuColumns::DESCRIPTION_COLUMN)
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
    else if (role == CpuColumns::USER_EDIT_ROLE && index.column() == CpuColumns::ADDRSPACE_COLUMN)
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
	cpus_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit cpuRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
