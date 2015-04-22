/* 
 *  	Created on: 21.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsmodel.cpp
 *		Project: Kactus 2
 */

#include "memorymapsmodel.h"
#include "memorymapsdelegate.h"

#include "MemoryMapsColumns.h"

#include <IPXACTmodels/memorymapitem.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::MemoryMapsModel()
//-----------------------------------------------------------------------------
MemoryMapsModel::MemoryMapsModel( QSharedPointer<Component> component, QObject *parent ):
QAbstractItemModel(parent),
component_(component),
rootMemoryMaps_(component->getMemoryMaps())
{
	Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::MemoryMapsModel()
//-----------------------------------------------------------------------------
MemoryMapsModel::~MemoryMapsModel()
{

}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::rowCount()
//-----------------------------------------------------------------------------
int MemoryMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (!parent.isValid())
    {
        return rootMemoryMaps_.size();
    }

    else if (!parent.parent().isValid())
    {
        return rootMemoryMaps_.at(parent.row())->getMemoryRemaps()->size();
    }

    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::columnCount()
//-----------------------------------------------------------------------------
int MemoryMapsModel::columnCount( const QModelIndex& /*parent = QModelIndex()*/ ) const
{
    return MemoryMapsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags MemoryMapsModel::flags( const QModelIndex& index ) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

	// interface references are made in bus interface editor
    if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}

    else if ((!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
        (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
    {
        return Qt::ItemIsSelectable;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::headerData()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ )
    const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}
	if (role == Qt::DisplayRole)
    {
        if (section == MemoryMapsColumns::NAME_COLUMN)
        {
            return tr("Memory map\nname");
        }
        else if (section == MemoryMapsColumns::AUB_COLUMN)
        {
            return tr("Address unit\nbits (AUB)");
        }
        else if (section == MemoryMapsColumns::INTERFACE_COLUMN)
        {
            return tr("Slave interface\nbinding");
        }
        else if (section == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return tr("Remap state");
        }
        else if (section == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
	}
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::data()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

    else if (!isIndexValid(index))
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        return valueForIndex(index);
	}
	if (MemoryMapsDelegate::USER_DISPLAY_ROLE == role)
    {
        QSharedPointer<MemoryMap> parentMemoryMap =
            getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));

        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_.at(index.row());
        }

        return component_->getSlaveInterfaces(parentMemoryMap->getName());
	}
	else if (Qt::ForegroundRole == role)
    {
        return getForeGroundcolour(index);
	}
	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == MemoryMapsColumns::NAME_COLUMN ||
            index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::setData()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ )
{
	if (!index.isValid() || !isIndexValid(index))
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == MemoryMapsColumns::NAME_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_[index.row()]->setName(value.toString());
            }
            else
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
                memoryRemap->setName(value.toString());
            }

        }
        else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_[index.row()]->setAddressUnitBits(value.toUInt());
                emit aubChangedOnRow(index.row());
            }
        }
        else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_[index.row()]->setDescription(value.toString());
            }
            else
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
                memoryRemap->setDescription(value.toString());
            }
        }
        else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            if (index.parent().isValid())
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());

                memoryRemap->setRemapState(value.toString());
            }
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}

    return false;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::isValid()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isValid() const 
{
	foreach (QSharedPointer<MemoryMap> memMap, rootMemoryMaps_)
    {
		if (!memMap->isValid(component_->getChoices(), component_->getRemapStateNames()))
        {
			return false;
		}

        foreach (QSharedPointer<MemoryRemap> memoryRemap, *memMap->getMemoryRemaps())
        {
            if (memoryRemap->isValid(component_->getChoices(), component_->getRemapStateNames()))
            {
                return false;
            }
        }
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onAddItem( const QModelIndex& index )
{
	int row = rootMemoryMaps_.size();

	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	rootMemoryMaps_.insert(row, QSharedPointer<MemoryMap>(new MemoryMap()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit memoryMapAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onRemoveItem( const QModelIndex& index )
{
	if (!index.isValid() || !isIndexValid(index))
    {
		return;
	}

    if (!index.parent().isValid())
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        rootMemoryMaps_.removeAt(index.row());
        endRemoveRows();

        // inform navigation tree that file set has been removed
        emit memoryMapRemoved(index.row());

        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
    else
    {
        QSharedPointer<MemoryRemap> removedMemoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
        QSharedPointer<MemoryMap> parentMemoryMap = getParentMemoryMap(removedMemoryRemap);

        beginRemoveRows(index.parent(), index.row(), index.row());
        parentMemoryMap->getMemoryRemaps()->removeAt(index.row());
        endRemoveRows();

        emit memoryRemapRemoved(index.row(), parentMemoryMap);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onAddMemoryRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onAddMemoryRemap(const QModelIndex& index)
{
    if (!index.isValid() || !isIndexValid(index) || index.parent().isValid())
    {
        return;
    }

    QSharedPointer<MemoryMap> parentMemoryMap = rootMemoryMaps_.at(index.row());
    int rowOfNewItem = parentMemoryMap->getMemoryRemaps()->count();

    QSharedPointer<MemoryRemap> newMemoryRemap (new MemoryRemap);
    QList<QSharedPointer<MemoryMapItem> > newRemapItems;
    foreach (QSharedPointer<MemoryMapItem> memoryMapItem, parentMemoryMap->getItems())
    {
        QSharedPointer<MemoryMapItem> memoryRemapItem = memoryMapItem->clone();
        newRemapItems.append(memoryRemapItem);
    }
    newMemoryRemap->setItems(newRemapItems);

    beginInsertRows(index, rowOfNewItem, rowOfNewItem);
    parentMemoryMap->getMemoryRemaps()->insert(rowOfNewItem, newMemoryRemap);
    endInsertRows();

    emit memoryRemapAdded(rowOfNewItem, parentMemoryMap);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::index()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if (!parent.isValid() && 0 <= row && row < rootMemoryMaps_.size())
    {
        return createIndex(row, column, rootMemoryMaps_.at(row).data());
    }
    else
    {
        QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(parent, row);

        return createIndex(row, column, memoryRemap.data());
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::parent()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    foreach (QSharedPointer<MemoryMap> rootItem, rootMemoryMaps_)
    {
        if (rootItem.data() == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    MemoryRemap* childItem = static_cast<MemoryRemap*>(child.internalPointer());
    return createParentIndexForMemoryRemap(childItem);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::createParentIndexForChildItem()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::createParentIndexForMemoryRemap(MemoryRemap* childItem) const
{
    for (int i = 0; i < rootMemoryMaps_.size(); ++i)
    {
        foreach (QSharedPointer<MemoryRemap> memoryRemap, *rootMemoryMaps_.at(i)->getMemoryRemaps()) 
        {
            if (memoryRemap.data() == childItem)
            {
                return createIndex(i, 0, rootMemoryMaps_.at(i).data());
            }
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getIndexedMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> MemoryMapsModel::getIndexedMemoryRemap(QModelIndex const& parentIndex, int row) const
{
    foreach (QSharedPointer<MemoryMap> rootItem, rootMemoryMaps_)
    {
        if (rootItem.data() == parentIndex.internalPointer())
        {
            for (int childRow = 0; childRow < rootItem->getMemoryRemaps()->size(); ++childRow)
            {
                if (childRow == row)
                {
                    return rootItem->getMemoryRemaps()->at(childRow);
                }
            }
        }
    }

    return QSharedPointer<MemoryRemap> (new MemoryRemap());
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getParentMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryMapsModel::getParentMemoryMap(QSharedPointer<MemoryRemap> memoryRemap) const
{
    foreach (QSharedPointer<MemoryMap> memoryMap, rootMemoryMaps_)
    {
        if (memoryMap->getMemoryRemaps()->contains(memoryRemap))
        {
            return memoryMap;
        }
    }

    return QSharedPointer<MemoryMap> (new MemoryMap());
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::isIndexValid()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isIndexValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.row() < 0 || index.row() >= rootMemoryMaps_.size())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    else
    {
        QSharedPointer<MemoryRemap> memoryRemapAtIndex = getIndexedMemoryRemap(index.parent(), index.row());
        QSharedPointer<MemoryMap> parentMemoryMap = getParentMemoryMap(memoryRemapAtIndex);
        if (index.row() < 0 || index.row() >= parentMemoryMap->getMemoryRemaps()->size())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return rootMemoryMaps_.at(index.row())->getName();
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            return childItem->getName();
        }
    }
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        QSharedPointer<MemoryMap> parentMemoryMap;

        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_.at(index.row());
        }
        else
        {
            parentMemoryMap = getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));
        }

        return parentMemoryMap->getAddressUnitBits();

    }
    else if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
        QSharedPointer<MemoryMap> parentMemoryMap;
        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_.at(index.row());
        }
        else
        {
            parentMemoryMap = getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));
        }

        QStringList interfaceNames = component_->getSlaveInterfaces(parentMemoryMap->getName());
        if (interfaceNames.isEmpty() || parentMemoryMap->isEmpty())
        {
            return tr("No binding");
        }
        else
        {
            return interfaceNames.join(" ");
        }
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return QString(tr("Default"));
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            if (!childItem->getRemapState().isEmpty())
            {
                return childItem->getRemapState();
            }
            else
            {
                return QString(tr("No remap state selected."));
            }
        }
    }
    else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return rootMemoryMaps_.at(index.row())->getDescription();
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            return childItem->getDescription();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getForeGroundcolour()
//-----------------------------------------------------------------------------
QColor MemoryMapsModel::getForeGroundcolour(QModelIndex const& index) const
{
    if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN ||
        (!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
        (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
    {
        return QColor("gray");
    }

    else if (index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
        
        if (!memoryRemap->isValid(component_->getChoices(), component_->getRemapStateNames()))
        {
            return QColor("red");
        }
    }

    return QColor("black");
}
