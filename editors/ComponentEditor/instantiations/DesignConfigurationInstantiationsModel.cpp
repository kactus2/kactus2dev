//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for design configuration instantiations of a component.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiationsModel.h"
#include "DesignInstantiationColumns.h"

#include <QStringList>
#include <QColor>
#include <QMimeData>
#include <QRegularExpression>

#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::DesignConfigurationInstantiationsModel()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationsModel::DesignConfigurationInstantiationsModel(
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > instantiations, 
    QSharedPointer<InstantiationsValidator> validator,
    QObject* parent):
QAbstractTableModel(parent),
instantiations_(instantiations),
validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::~DesignConfigurationInstantiationsModel()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationsModel::~DesignConfigurationInstantiationsModel()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::rowCount()
//-----------------------------------------------------------------------------
int DesignConfigurationInstantiationsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return instantiations_->size();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::columnCount()
//-----------------------------------------------------------------------------
int DesignConfigurationInstantiationsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return DesignInstantiationsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags DesignConfigurationInstantiationsModel::flags(QModelIndex const& index) const
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
// Function: DesignConfigurationInstantiationsModel::headerData()
//-----------------------------------------------------------------------------
QVariant DesignConfigurationInstantiationsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return tr("Design configuration reference");
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
// Function: DesignConfigurationInstantiationsModel::data()
//-----------------------------------------------------------------------------
QVariant DesignConfigurationInstantiationsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
        return QVariant();
    }

    QSharedPointer<DesignConfigurationInstantiation> instantiation = instantiations_->at(index.row());

    if (role == Qt::EditRole && index.column() == DesignInstantiationsColumns::DESCRIPTION)
    {
        return instantiation->description();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole)
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
            return instantiation->getDesignConfigurationReference()->toString();
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
// Function: DesignConfigurationInstantiationsModel::setData()
//-----------------------------------------------------------------------------
bool DesignConfigurationInstantiationsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
		return false;
	}

    QSharedPointer<DesignConfigurationInstantiation> instantiation = instantiations_->at(index.row());

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
            VLNV designConfigurationReference = VLNV(VLNV::DESIGNCONFIGURATION, value.toString());
            instantiation->getDesignConfigurationReference()->setVLNV(designConfigurationReference);
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
// Function: DesignConfigurationInstantiationsModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions DesignConfigurationInstantiationsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList DesignConfigurationInstantiationsModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::dropMimeData()
//-----------------------------------------------------------------------------
bool DesignConfigurationInstantiationsModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, 
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
        if (vlnv.getType() == VLNV::DESIGNCONFIGURATION)
        {
            setData(index(parent.row(), parent.column()), vlnv.toString(":"));
            emit contentChanged();
        }       
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::onAddItem()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationsModel::onAddItem(QModelIndex const& index)
{
	int row = instantiations_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	instantiations_->insert(row, 
        QSharedPointer<DesignConfigurationInstantiation>(new DesignConfigurationInstantiation()));
	endInsertRows();

	emit designConfigurationInstantiationAdded(row);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationsModel::onRemoveItem(QModelIndex const& index)
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

	emit designConfigurationInstantiationRemoved(index.row());
	emit contentChanged();
}
