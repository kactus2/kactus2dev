//-----------------------------------------------------------------------------
// File: viewsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.6.2012
//
// Description:
// The model to manage the views summary.
//-----------------------------------------------------------------------------

#include "viewsmodel.h"

#include <QStringList>
#include <QColor>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ViewsModel::ViewsModel()
//-----------------------------------------------------------------------------
ViewsModel::ViewsModel(QSharedPointer<Component> component, QObject *parent):
QAbstractTableModel(parent),
component_(component),
views_(component->getViews()) 
{
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::~ViewsModel()
//-----------------------------------------------------------------------------
ViewsModel::~ViewsModel()
{
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::rowCount()
//-----------------------------------------------------------------------------
int ViewsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return views_.size();
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::columnCount()
//-----------------------------------------------------------------------------
int ViewsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return ViewsModel::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ViewsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	// the type only displays the type of the view, it can not be edited.
	else if (index.column() == ViewsModel::TYPE_COLUMN)
    {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}

	// other columns can be edited
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ViewsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (section == ViewsModel::NAME_COLUMN)
        {
            return tr("Name");
        }
        else if (section == ViewsModel::TYPE_COLUMN)
        {
            return tr("View type");
        }
        else if (section == ViewsModel::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::data()
//-----------------------------------------------------------------------------
QVariant ViewsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
        return QVariant();
    }

    QSharedPointer<View> view = views_.at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ViewsModel::NAME_COLUMN)
        {
            if (!view->getName().isEmpty())
            {
                return views_.at(index.row())->getName();
            }
            else
            {
                return "unnamed";
            }
        }
        else if (index.column() == ViewsModel::TYPE_COLUMN)
        {
            if (view->isHierarchical())
            {
                return tr("hierarchical");
            }
            else
            {
                return tr("non-hierarchical");
            }
        }
        else if (index.column() == ViewsModel::DESCRIPTION_COLUMN)
        {
            return view->getDescription().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
    else if (role == Qt::EditRole && index.column() == ViewsModel::DESCRIPTION_COLUMN)
    {
        return view->getDescription();
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == ViewsModel::TYPE_COLUMN)
        {
            return QColor("gray");
        }
		else if (view->isValid(component_->getFileSetNames(), component_->getChoices()))
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
        if (index.column() == ViewsModel::NAME_COLUMN)
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
// Function: ViewsModel::setData()
//-----------------------------------------------------------------------------
bool ViewsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
		return false;
	}

    QSharedPointer<View> view = views_.at(index.row());

	if (role == Qt::EditRole)
    {
        if (index.column() == ViewsModel::NAME_COLUMN)
        {
            view->setName(value.toString());
        }
        else if (index.column() == ViewsModel::DESCRIPTION_COLUMN)
        {
            view->setDescription(value.toString());
        }
        // type is not edited directly, it is only shown
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
// Function: ViewsModel::isValid()
//-----------------------------------------------------------------------------
bool ViewsModel::isValid() const
{
	// file set names are needed to check that references within views are valid
	QStringList fileSetNames = component_->getFileSetNames();

	// if at least one view is invalid
	foreach (QSharedPointer<View> view, views_)
    {
		if (!view->isValid(fileSetNames, component_->getChoices()))
        {
			return false;
		}
	}
	// all views were valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::onAddItem()
//-----------------------------------------------------------------------------
void ViewsModel::onAddItem(QModelIndex const& index)
{
	int row = views_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	views_.insert(row, QSharedPointer<View>(new View()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit viewAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ViewsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ViewsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= views_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	views_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit viewRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
