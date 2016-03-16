//-----------------------------------------------------------------------------
// File: ComPropertyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 17.4.2012
//
// Description:
// Model for COM properties.
//-----------------------------------------------------------------------------

#include "ComPropertyModel.h"

#include "ComPropertyColumns.h"

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <QColor>
#include <QRegularExpression>

QString const ComPropertyModel::IP_ADDRESS_REGEX("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
QString const ComPropertyModel::TIME_UNIT_REGEX("^(fs|ps|ns|us|µs|ms|sec|s)$");

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::ComPropertyModel()
//-----------------------------------------------------------------------------
ComPropertyModel::ComPropertyModel(QObject *parent)
    : QAbstractTableModel(parent),
      table_()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::~ComPropertyModel()
//-----------------------------------------------------------------------------
ComPropertyModel::~ComPropertyModel()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::setProperties()
//-----------------------------------------------------------------------------
void ComPropertyModel::setProperties(QSharedPointer< QList< QSharedPointer<ComProperty> > > properties)
{
    beginResetModel();

    table_.clear();

    foreach (QSharedPointer<ComProperty> comProperty, *properties)
    {
		QSharedPointer<ComProperty> copyProperty =
			QSharedPointer<ComProperty>( new ComProperty(*comProperty.data()) );
        table_.append(QSharedPointer<ComProperty>(copyProperty));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::getProperties()
//-----------------------------------------------------------------------------
QList< QSharedPointer<ComProperty> > ComPropertyModel::getProperties() const
{
    return table_;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::rowCount()
//-----------------------------------------------------------------------------
int ComPropertyModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::columnCount()
//-----------------------------------------------------------------------------
int ComPropertyModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ComPropertyColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::data()
//-----------------------------------------------------------------------------
QVariant ComPropertyModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ComPropertyColumns::NAME)
        {
            return table_.at(index.row())->name();
        }
        else if (index.column() == ComPropertyColumns::REQUIRED)
        {
            return table_.at(index.row())->isRequired();
        }
        else if (index.column() == ComPropertyColumns::TYPE)
        {
            return table_.at(index.row())->getType();
        }
        else if (index.column() == ComPropertyColumns::DEFAULT_VALUE)
        {
            return table_.at(index.row())->getDefaultValue();
        }
        else if (index.column() == ComPropertyColumns::DESCRIPTION)
        {
            return table_.at(index.row())->getDescription();
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::TextColorRole)
    {
        if (index.column() == ComPropertyColumns::DEFAULT_VALUE)
        {
            // Validate the default value against the data type.
            QString const& value = table_.at(index.row())->getDefaultValue();
            QString const& type = table_.at(index.row())->getType();
            bool ok = true;

            if (type == QLatin1String("integer"))
            {
                value.toInt(&ok);
            }
            else if (type == QLatin1String("ip_address"))
            {
                ok = value.contains(QRegularExpression(IP_ADDRESS_REGEX));
            }

            if (ok)
            {
                return QColor(Qt::black);
            }
            else
            {
                return QColor(Qt::red);
            }
        }

        else
        {
            return QColor(Qt::black);
        }
    }
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ComPropertyColumns::NAME ||
            index.column() == ComPropertyColumns::REQUIRED ||
            index.column() == ComPropertyColumns::TYPE)
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
// Function: ComPropertyModel::headerData()
//-----------------------------------------------------------------------------
QVariant ComPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == ComPropertyColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == ComPropertyColumns::REQUIRED)
    {
        return tr("Required");
    }
    else if (section == ComPropertyColumns::TYPE)
    {
        return tr("Type");
    }
    else if (section == ComPropertyColumns::DEFAULT_VALUE)
    {
        return tr("Default Value");
    }
    else if (section == ComPropertyColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComPropertyModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::setData()
//-----------------------------------------------------------------------------
bool ComPropertyModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == ComPropertyColumns::NAME)
    {
        table_[index.row()]->setName(value.toString());
    }
    else if (index.column() == ComPropertyColumns::REQUIRED)
    {
        table_[index.row()]->setRequired(value.toBool());
    }
    else if (index.column() == ComPropertyColumns::TYPE)
    {
        table_[index.row()]->setType(value.toString());
    }
    else if (index.column() == ComPropertyColumns::DEFAULT_VALUE)
    {
        table_[index.row()]->setDefaultValue(value.toString());
    }
    else if (index.column() == ComPropertyColumns::DESCRIPTION)
    {
        table_[index.row()]->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    emit contentChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::isValid()
//-----------------------------------------------------------------------------
bool ComPropertyModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onAdd()
//-----------------------------------------------------------------------------
void ComPropertyModel::onAdd()
{
    beginInsertRows(QModelIndex(), table_.size(), table_.size());
    table_.append(QSharedPointer<ComProperty>(new ComProperty()));
    endInsertRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onAddItem()
//-----------------------------------------------------------------------------
void ComPropertyModel::onAddItem(QModelIndex const& index)
{
    int row = table_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    table_.insert(row, QSharedPointer<ComProperty>(new ComProperty()));
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onRemove()
//-----------------------------------------------------------------------------
void ComPropertyModel::onRemove(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ComPropertyModel::onRemoveItem(QModelIndex const& index )
{
    // Don't remove anything if index or row is invalid.
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return;
    }

    // Remove the indexed property.
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}
