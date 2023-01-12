//-----------------------------------------------------------------------------
// File: PropertyValueModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 16.4.2012
//
// Description:
// Model for property values.
//-----------------------------------------------------------------------------

#include "PropertyValueModel.h"

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <common/KactusColors.h>

#include <QFont>
#include <QRegularExpression>

QString const PropertyValueModel::IP_ADDRESS_REGEX("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
    "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::PropertyValueModel()
//-----------------------------------------------------------------------------
PropertyValueModel::PropertyValueModel(QObject *parent): QAbstractTableModel(parent), allowedProperties_(), 
    locked_(false)
{

}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::~PropertyValueModel()
//-----------------------------------------------------------------------------
PropertyValueModel::~PropertyValueModel()
{

}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setAllowedProperties()
//-----------------------------------------------------------------------------
void PropertyValueModel::setAllowedProperties(QList< QSharedPointer<ComProperty> > properties)
{
    beginResetModel();

    allowedProperties_ = properties;

    // Check that at least all required properties are found in the values.
    foreach (QSharedPointer<ComProperty const> comProperty, properties)
    {
        if (comProperty->isRequired())
        {
            bool foundInTable = false;

            for (int i = 0; i < table_.size() && !foundInTable; i++)
            {
                if (table_.at(i).first.compare(comProperty->name()) == 0)
                {
                    foundInTable = true;
                }
            }

            if (!foundInTable)
            {
                table_.append(NameValuePair(comProperty->name(), comProperty->getDefaultValue()));
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setData()
//-----------------------------------------------------------------------------
void PropertyValueModel::setData(QMap<QString, QString> const& propertyValues)
{
    beginResetModel();

    table_.clear();

    QMapIterator<QString, QString> iter(propertyValues);
    while (iter.hasNext())
    {
        iter.next();
        table_.append(NameValuePair(iter.key(), iter.value()));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::getData()
//-----------------------------------------------------------------------------
QMap<QString, QString> PropertyValueModel::getData() const
{
    QMap<QString, QString> values;

    foreach (NameValuePair const& pair, table_)
    {
        values[pair.first] = pair.second;
    }

    return values;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setLock()
//-----------------------------------------------------------------------------
void PropertyValueModel::setLock(bool locked)
{
    locked_ = locked;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::rowCount()
//-----------------------------------------------------------------------------
int PropertyValueModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::columnCount()
//-----------------------------------------------------------------------------
int PropertyValueModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return 2;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::data()
//-----------------------------------------------------------------------------
QVariant PropertyValueModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() ==  0)
        {
            return table_.at(index.row()).first;
        }
        else if (index.column() == 1)
        {
            return table_.at(index.row()).second;
        }
        else
        {
            return QVariant();
        }
    }

    else if (role == Qt::FontRole && index.column() == 0)
    {
        foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
        {
            if (comProperty->name() == table_.at(index.row()).first && comProperty->isRequired())
            {
                QFont font;
                font.setBold(true);
                return font;
            }
        }

        return QVariant();
    }

    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == 0)
        {
            // Check if the property value is found from the list of allowed properties.
            foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
            {
                if (comProperty->name() == table_.at(index.row()).first)
                {
                    return KactusColors::REGULAR_TEXT;
                }
            }

            // Use red to indicate property values that are not found in the allowed properties.
            return KactusColors::ERROR;
        }

        else if (index.column() == 1)
        {
            // Validate the property value against the data type defined in the property definition.
            foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
            {
                // Check if we found a match.
                if (comProperty->name() == table_.at(index.row()).first)
                {
                    QString const& value = table_.at(index.row()).second;
                    bool ok = true;

                    if (comProperty->getType() == QLatin1String("integer"))
                    {
                        value.toInt(&ok);
                    }
                    else if (comProperty->getType() == QLatin1String("ip_address"))
                    {
                        ok = value.contains(QRegularExpression(IP_ADDRESS_REGEX));
                    }

                    if (ok)
                    {
                        return KactusColors::REGULAR_TEXT;
                    }
                    else
                    {
                        return KactusColors::ERROR;
                    }
                }
            }

            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
    }

    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::headerData()
//-----------------------------------------------------------------------------
QVariant PropertyValueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return tr("Name");
        }
        else if (section == 1)
        {
            return tr("Value");
        }
    }
       
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PropertyValueModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == 0)
    {
        QString const& propertyName = table_.at(index.row()).first;

        foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
        {
            if (comProperty->name().compare(propertyName) == 0 && comProperty->isRequired())
            {
                return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            }
        }
    }

    Qt::ItemFlags indexFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (!locked_)
    {
        indexFlags |= Qt::ItemIsEditable;
    }

    return indexFlags;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setData()
//-----------------------------------------------------------------------------
bool PropertyValueModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (locked_ || !index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == 0)
    {
        table_[index.row()].first = value.toString();
    }
    else if (index.column() == 1)
    {
        table_[index.row()].second = value.toString();
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
// Function: PropertyValueModel::onAddItem()
//-----------------------------------------------------------------------------
void PropertyValueModel::onAddItem(QModelIndex const& index)
{
    int row = table_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    NameValuePair element(QString(""), QString(""));

    if (!table_.contains(element))
    {
        beginInsertRows(QModelIndex(), row, row);
        table_.insert(row, element);
        endInsertRows();

        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PropertyValueModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return;
    }

    // Required property values cannot be deleted.
    foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
    {
        if (comProperty->name() == table_.at(index.row()).first && comProperty->isRequired())
        {
            return;
        }
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}
