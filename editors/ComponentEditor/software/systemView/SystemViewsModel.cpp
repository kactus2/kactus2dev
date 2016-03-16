//-----------------------------------------------------------------------------
// File: SystemViewsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The model to manage system views for a table view.
//-----------------------------------------------------------------------------

#include "SystemViewsModel.h"
#include "SystemViewsDelegate.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QColor>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::SystemViewsModel()
//-----------------------------------------------------------------------------
SystemViewsModel::SystemViewsModel(QSharedPointer<Component> component, QObject* parent):
QAbstractTableModel(parent),
    views_(component->getSystemViews()),
    component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::~SystemViewsModel()
//-----------------------------------------------------------------------------
SystemViewsModel::~SystemViewsModel()
{

}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::SystemViewsModel()
//-----------------------------------------------------------------------------
int SystemViewsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid()) 
    {
		return 0;
	}

	return views_.size();
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::columnCount()
//-----------------------------------------------------------------------------
int SystemViewsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return SystemViewsDelegate::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::columnCount()
//-----------------------------------------------------------------------------
Qt::ItemFlags SystemViewsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	// hierarchy reference can only be dropped.
	else if (index.column() == SystemViewsDelegate::HIER_REF_COLUMN)
    {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::headerData()
//-----------------------------------------------------------------------------
QVariant SystemViewsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
	}

    if (section == SystemViewsDelegate::NAME_COLUMN)
    {
        return tr("Name");
    }
    else if (section == SystemViewsDelegate::HIER_REF_COLUMN)
    {
        return tr("Hierarchy reference");
    }
    else if (section == SystemViewsDelegate::DISPLAY_NAME_COLUMN)
    {
        return tr("Display name");
    }
    else if (section == SystemViewsDelegate::DESCRIPTION_COLUMN)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::data()
//-----------------------------------------------------------------------------
QVariant SystemViewsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
		return QVariant();
	}
	
	if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == SystemViewsDelegate::NAME_COLUMN)
        {
            return views_.at(index.row())->name();
        }
        else if (index.column() == SystemViewsDelegate::HIER_REF_COLUMN)
        {
            return views_.at(index.row())->getHierarchyRef().toString(":");
        }
        else if (index.column() == SystemViewsDelegate::DISPLAY_NAME_COLUMN)
        {
            return views_.at(index.row())->displayName();
        }
        else if (index.column() == SystemViewsDelegate::DESCRIPTION_COLUMN)
        {
            return views_.at(index.row())->description();
        }
        else
        {
            return QVariant();
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        QStringList fileSetNames = component_->getFileSetNames();
		QStringList viewNames = component_->getViewNames();

		if (views_.at(index.row())->isValid(fileSetNames, viewNames))
        {
			return QColor("black");
		}
		else
        {
			return QColor("red");
		}
	}

    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == SystemViewsDelegate::NAME_COLUMN || 
            index.column() == SystemViewsDelegate::HIER_REF_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::setData()
//-----------------------------------------------------------------------------
bool SystemViewsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
		return false;
	}

    if (role == Qt::EditRole)
    {
        QSharedPointer<SystemView> targetSystemView = views_.at(index.row());

        if (index.column() == SystemViewsDelegate::NAME_COLUMN)
        {
            targetSystemView->setName(value.toString());
        }
        else if (index.column() == SystemViewsDelegate::HIER_REF_COLUMN)
        {
            VLNV hierRef = VLNV(VLNV::DESIGNCONFIGURATION, value.toString(), ":");
            targetSystemView->setHierarchyRef(hierRef);
        }
        else if (index.column() == SystemViewsDelegate::DISPLAY_NAME_COLUMN)
        {
            targetSystemView->setDisplayName(value.toString());
        }
        else if (index.column() == SystemViewsDelegate::DESCRIPTION_COLUMN)
        {
            targetSystemView->setDescription(value.toString());
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
// Function: SystemViewsModel::isValid()
//-----------------------------------------------------------------------------
bool SystemViewsModel::isValid() const
{
	QStringList fileSetNames = component_->getFileSetNames();
	QStringList viewNames = component_->getViewNames();

	// check that each software view is valid
	foreach (QSharedPointer<SystemView> swView, views_)
    {
		if (!swView->isValid(fileSetNames, viewNames))
        {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions SystemViewsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList SystemViewsModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::dropMimeData()
//-----------------------------------------------------------------------------
bool SystemViewsModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if (row != -1 || column != -1 || !parent.isValid() || row > rowCount())
    {
        return false;
    }

    QVariant variant = data->imageData();
    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if (parent.column() == SystemViewsDelegate::HIER_REF_COLUMN)
    {
        if (vlnv.getType() != VLNV::DESIGNCONFIGURATION)
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::onAddItem()
//-----------------------------------------------------------------------------
void SystemViewsModel::onAddItem(QModelIndex const& index)
{
	int row = views_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	views_.insert(row, QSharedPointer<SystemView>(new SystemView()));
    component_->setSystemViews(views_);
	endInsertRows();

	emit viewAdded(row);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemViewsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void SystemViewsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	views_.removeAt(index.row());
    component_->setSystemViews(views_);
	endRemoveRows();

	emit viewRemoved(index.row());
	emit contentChanged();
}
