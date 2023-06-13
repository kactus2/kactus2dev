//-----------------------------------------------------------------------------
// File: IndirectInterfacesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Data model for component indirect interfaces.
//-----------------------------------------------------------------------------

#include "IndirectInterfacesModel.h"

#include "IndirectInterfaceColumns.h"

#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>

#include <common/KactusColors.h>

#include <QCoreApplication>
#include <QMap>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::IndirectInterfacesModel()
//-----------------------------------------------------------------------------
IndirectInterfacesModel::IndirectInterfacesModel(QSharedPointer<Component> component,
    QSharedPointer<IndirectInterfaceValidator> validator,
    QObject *parent):
QAbstractTableModel(parent),
    component_(component),
    indirectInterfaces_(component->getIndirectInterfaces()),
    validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::rowCount()
//-----------------------------------------------------------------------------
int IndirectInterfacesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return indirectInterfaces_->size();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::columnCount()
//-----------------------------------------------------------------------------
int IndirectInterfacesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return IndirectInterfaceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags IndirectInterfacesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

    if (index.column() == IndirectInterfaceColumns::TRANSPARENT_BRIDGES)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    if (index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF &&
        indirectInterfaces_->at(index.row())->getTransparentBridges()->isEmpty() == false)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::headerData()
//-----------------------------------------------------------------------------
QVariant IndirectInterfacesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
    }

    if (section == IndirectInterfaceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == IndirectInterfaceColumns::DISPLAY_NAME)
    {
        return tr("Display\nname");
    }
    else if (section == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF)
    {
        return tr("Indirect address\nfield");
    }
    else if (section == IndirectInterfaceColumns::INDIRECT_DATA_REF)
    {
        return tr("Indirect data\nfield");
    }
    else if (section == IndirectInterfaceColumns::MEMORY_MAP_REF)
    {
        return tr("Memory map");
    }
    else if (section == IndirectInterfaceColumns::TRANSPARENT_BRIDGES)
    {
        return tr("Transparent\nbridges");
    }
    else if (section == IndirectInterfaceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::data()
//-----------------------------------------------------------------------------
QVariant IndirectInterfacesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= indirectInterfaces_->size())
    {
		return QVariant();
	}

    QSharedPointer<IndirectInterface> indirectInterface = indirectInterfaces_->at(index.row());
	if (role == Qt::DisplayRole)
    {
        if (index.column() == IndirectInterfaceColumns::NAME)
        {
            return indirectInterface->name();
        }
        else if (index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
        {
            return indirectInterface->displayName();
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF)
        {
            return indirectInterface->getIndirectAddressRef();
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF)
        {
            return indirectInterface->getIndirectDataRef();
        }
        else if (index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF)
        {
            return indirectInterface->getMemoryMapRef();
        }
        else if (index.column() == IndirectInterfaceColumns::TRANSPARENT_BRIDGES)
        {
            QStringList initiatorRefs;
            for (QSharedPointer<TransparentBridge> bridge : *indirectInterface->getTransparentBridges())
            {
                initiatorRefs.append(bridge->getInitiatorRef());
            }
            return initiatorRefs.join(", ");
        }
        else if (index.column() == IndirectInterfaceColumns::DESCRIPTION)
        {
            return indirectInterface->description().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
    else if ((role == Qt::EditRole || role == Qt::ToolTipRole) && 
        index.column() == IndirectInterfaceColumns::DESCRIPTION)
    {
        return indirectInterface->description();
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == IndirectInterfaceColumns::NAME &&
            validator_->hasValidName(indirectInterface))
        {
            return KactusColors::REGULAR_TEXT;     
        }
        else if (index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
        {
            return KactusColors::REGULAR_TEXT;   
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF && 
            validator_->hasValidAddressReference(indirectInterface))
        {
            return KactusColors::REGULAR_TEXT;  
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF && 
            validator_->hasValidDataReference(indirectInterface))
        {
            return KactusColors::REGULAR_TEXT;  
        }
        else if (index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF && 
            validator_->hasValidMemoryMapReference(indirectInterface))
        {
            return KactusColors::REGULAR_TEXT;  
        }
        else if (index.column() == IndirectInterfaceColumns::TRANSPARENT_BRIDGES &&
            validator_->hasValidTransparentBridges(indirectInterface->getTransparentBridges()))
        {
            return KactusColors::DISABLED_TEXT;  
        }
        else if (index.column() == IndirectInterfaceColumns::DESCRIPTION)
        {
            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::ERROR;
        }
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == IndirectInterfaceColumns::NAME ||
            index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF ||
            index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
	}
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::setData()
//-----------------------------------------------------------------------------
bool IndirectInterfacesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= indirectInterfaces_->size())
    {
		return false;
	}

    QSharedPointer<IndirectInterface> IndirectInterface = indirectInterfaces_->at(index.row());
	if (role == Qt::EditRole)
    {
        if (index.column() == IndirectInterfaceColumns::NAME)
        {
            IndirectInterface->setName(value.toString());
        }
        else if (index.column() == IndirectInterfaceColumns::DISPLAY_NAME)
        {
            IndirectInterface->setDisplayName(value.toString());
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_ADDRESS_REF)
        {
            IndirectInterface->setIndirectAddressRef(value.toString());
        }
        else if (index.column() == IndirectInterfaceColumns::INDIRECT_DATA_REF)
        {
            IndirectInterface->setIndirectDataRef(value.toString());
        }
        else if (index.column() == IndirectInterfaceColumns::MEMORY_MAP_REF)
        {
            IndirectInterface->setMemoryMapRef(value.toString());
        }
        else if (index.column() == IndirectInterfaceColumns::DESCRIPTION)
        {
            IndirectInterface->setDescription(value.toString());
        }
        else
        {
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
// Function: IndirectInterfacesModel::onAddItem()
//-----------------------------------------------------------------------------
void IndirectInterfacesModel::onAddItem(QModelIndex const& index)
{
	int row = indirectInterfaces_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    QSharedPointer<IndirectInterface> indirectInterface(new IndirectInterface());
    indirectInterfaces_->insert(row, indirectInterface);
	endInsertRows();

	emit interfaceAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void IndirectInterfacesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= indirectInterfaces_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	indirectInterfaces_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit interfaceRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
