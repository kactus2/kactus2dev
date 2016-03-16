//-----------------------------------------------------------------------------
// File: DesignInstantiationsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for desing instantiations of a component.
//-----------------------------------------------------------------------------

#include "DesignInstantiationsModel.h"
#include "DesignInstantiationColumns.h"

#include <QStringList>
#include <QColor>
#include <QRegularExpression>

#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::DesignInstantiationsModel()
//-----------------------------------------------------------------------------
DesignInstantiationsModel::DesignInstantiationsModel(
    QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > instantiations, 
    QSharedPointer<InstantiationsValidator> validator,
    QObject* parent):
QAbstractTableModel(parent),
    instantiations_(instantiations),
    validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::~DesignInstantiationsModel()
//-----------------------------------------------------------------------------
DesignInstantiationsModel::~DesignInstantiationsModel()
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::rowCount()
//-----------------------------------------------------------------------------
int DesignInstantiationsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return instantiations_->size();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::columnCount()
//-----------------------------------------------------------------------------
int DesignInstantiationsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return DesignInstantiationsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags DesignInstantiationsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

    if (index.column() == DesignInstantiationsColumns::VLNV_REFERENCE)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    }

	// other columns can be edited
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::headerData()
//-----------------------------------------------------------------------------
QVariant DesignInstantiationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (section == DesignInstantiationsColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == DesignInstantiationsColumns::DISPLAY_NAME)
        {
            return tr("Display name");
        }
        else if (section == DesignInstantiationsColumns::VLNV_REFERENCE)
        {
            return tr("Design reference");
        }
        else if (section == DesignInstantiationsColumns::DESCRIPTION)
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
// Function: DesignInstantiationsModel::data()
//-----------------------------------------------------------------------------
QVariant DesignInstantiationsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
        return QVariant();
    }

    QSharedPointer<DesignInstantiation> instantiation = instantiations_->at(index.row());

    if (role == Qt::EditRole && index.column() == DesignInstantiationsColumns::DESCRIPTION)
    {
        return instantiation->description();
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == DesignInstantiationsColumns::NAME)
        {
            if (!instantiation->name().isEmpty())
            {
                return instantiation->name();
            }
            else
            {
                return "unnamed";
            }
        }
        else if (index.column() == DesignInstantiationsColumns::DISPLAY_NAME)
        {
            return instantiation->displayName();
        }
        else if (index.column() == DesignInstantiationsColumns::VLNV_REFERENCE)
        {
            return instantiation->getDesignReference()->toString();
        }
        else if (index.column() == DesignInstantiationsColumns::DESCRIPTION)
        {
            return instantiation->description().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == DesignInstantiationsColumns::NAME && 
            !validator_->hasValidName(instantiation->name()))
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
        if (index.column() == DesignInstantiationsColumns::NAME ||
            index.column() == DesignInstantiationsColumns::VLNV_REFERENCE)
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
// Function: DesignInstantiationsModel::setData()
//-----------------------------------------------------------------------------
bool DesignInstantiationsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
		return false;
	}

    QSharedPointer<DesignInstantiation> instantiation = instantiations_->at(index.row());

	if (role == Qt::EditRole)
    {
        if (index.column() == DesignInstantiationsColumns::NAME)
        {
            instantiation->setName(value.toString());
        }
        else if (index.column() == DesignInstantiationsColumns::DISPLAY_NAME)
        {
            instantiation->setDisplayName(value.toString());
        }
        else if (index.column() == DesignInstantiationsColumns::VLNV_REFERENCE)
        {
            VLNV designReference = VLNV(VLNV::DESIGN, value.toString());
            instantiation->getDesignReference()->setVLNV(designReference);
        }
        else if (index.column() == DesignInstantiationsColumns::DESCRIPTION)
        {
            instantiation->setDescription(value.toString());
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
// Function: DesignInstantiationsModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions DesignInstantiationsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList DesignInstantiationsModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::dropMimeData()
//-----------------------------------------------------------------------------
bool DesignInstantiationsModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, 
    int column, QModelIndex const& parent)
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
    if (parent.column() == DesignInstantiationsColumns::VLNV_REFERENCE)
    {
        if (vlnv.getType() == VLNV::DESIGN)
        {
            setData(index(parent.row(), parent.column()), vlnv.toString(":"));
            emit contentChanged();
        }       
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::onAddItem()
//-----------------------------------------------------------------------------
void DesignInstantiationsModel::onAddItem(QModelIndex const& index)
{
	int row = instantiations_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	instantiations_->insert(row, QSharedPointer<DesignInstantiation>(new DesignInstantiation()));
	endInsertRows();

	emit designInstantiationAdded(row);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void DesignInstantiationsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	instantiations_->removeAt(index.row());
	endRemoveRows();

	emit designInstantiationRemoved(index.row());
	emit contentChanged();
}
