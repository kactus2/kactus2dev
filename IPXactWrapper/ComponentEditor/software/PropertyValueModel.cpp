//-----------------------------------------------------------------------------
// File: PropertyValueModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// Model for property values.
//-----------------------------------------------------------------------------

#include "PropertyValueModel.h"

#include <models/ComProperty.h>

#include <QRegExp>
#include <QFont>

QString const PropertyValueModel::IP_ADDRESS_REGEX("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::PropertyValueModel()
//-----------------------------------------------------------------------------
PropertyValueModel::PropertyValueModel(QObject *parent)
    : QAbstractTableModel(parent),
      allowedProperties_(0)
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
void PropertyValueModel::setAllowedProperties(QList< QSharedPointer<ComProperty> > const* properties)
{
    beginResetModel();

    allowedProperties_ = properties;

    if (allowedProperties_ != 0)
    {
        // Check that at least all required properties are found in the values.
        foreach (QSharedPointer<ComProperty const> prop, *properties)
        {
            if (prop->isRequired())
            {
                int i = 0;

                for (; i < table_.size(); ++i)
                {
                    if (table_.at(i).first == prop->getName())
                    {
                        break;
                    }
                }

                // If the value was not found, add it.
                if (i == table_.size())
                {
                    table_.append(NameValuePair(prop->getName(), prop->getDefaultValue()));
                }
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
// Function: PropertyValueModel::rowCount()
//-----------------------------------------------------------------------------
int PropertyValueModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
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
int PropertyValueModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
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
QVariant PropertyValueModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return table_.at(index.row()).first;
        
        case 1:
            return table_.at(index.row()).second;
        
        default:
            return QVariant();
        }
    }
    else if (role == Qt::FontRole)
    {
        if (index.column() == 0 && allowedProperties_ != 0)
        {
            foreach (QSharedPointer<ComProperty const> prop, *allowedProperties_)
            {
                if (prop->getName() == table_.at(index.row()).first &&
                    prop->isRequired())
                {
                    QFont font;
                    font.setBold(true);
                    return font;
                }
            }
        }

        return QVariant();
    }
    else if (role == Qt::TextColorRole)
    {
        switch (index.column())
        {
        case 0:
            {
                // Check if the property value is found from the list of allowed properties.
                if (allowedProperties_ != 0)
                {
                    foreach (QSharedPointer<ComProperty const> prop, *allowedProperties_)
                    {
                        if (prop->getName() == table_.at(index.row()).first)
                        {
                            return Qt::black;
                        }
                    }
                }

                // Use red to indicate property values that are not found in the allowed properties.
                return Qt::red;
            }
            
        case 1:
            {
                // Validate the property value against the data type defined in the property definition.
                if (allowedProperties_ != 0)
                {
                    foreach (QSharedPointer<ComProperty const> prop, *allowedProperties_)
                    {
                        // Check if we found a match.
                        if (prop->getName() == table_.at(index.row()).first)
                        {
                            QString const& value = table_.at(index.row()).second;
                            bool ok = true;

                            if (prop->getType() == "integer")
                            {
                                value.toInt(&ok);
                            }
                            else if (prop->getType() == "ip_address")
                            {
                                ok = value.contains(QRegExp(IP_ADDRESS_REGEX));
                            }

                            if (ok)
                            {
                                return Qt::black;
                            }
                            else
                            {
                                return Qt::red;
                            }
                        }
                    }
                }

                return Qt::black;
            }

        default:
            {
                return Qt::black;
            }
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
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Name");

        case 1:
            return tr("Value");

        default:
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
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
        if (allowedProperties_ != 0)
        {
            foreach (QSharedPointer<ComProperty const> prop, *allowedProperties_)
            {
                // Check if we found a match.
                if (prop->getName() == table_.at(index.row()).first && prop->isRequired())
                {
                    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
                }
            }
        }
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setData()
//-----------------------------------------------------------------------------
bool PropertyValueModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid())
    {
        return false;
    }
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0:
            {
                table_[index.row()].first = value.toString();
                break;
            }

        case 1:
            {
                table_[index.row()].second = value.toString();
                break;
            }

        default:
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
    // don't remove anything if index is invalid
    if (!index.isValid())
    {
        return;
    }
    // make sure the row number if valid
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return;
    }

    // Required property values cannot be deleted.
    if (allowedProperties_ != 0)
    {
        foreach (QSharedPointer<ComProperty const> prop, *allowedProperties_)
        {
            // Check if we found a match.
            if (prop->getName() == table_.at(index.row()).first && prop->isRequired())
            {
                return;
            }
        }
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::isValid()
//-----------------------------------------------------------------------------
bool PropertyValueModel::isValid() const
{
    // Validate the property values against the allowed properties.
    if (allowedProperties_ != 0)
    {
        foreach (NameValuePair const& pair, table_)
        {
            int i = 0;

            for (; i < allowedProperties_->size(); ++i)
            {
                if (allowedProperties_->at(i)->getName() == pair.first)
                {
                    break;
                }
            }

            // Check if the property was not found.
            if (i == allowedProperties_->size())
            {
                emit noticeMessage(tr("Property value '%1' not found in the COM definition").arg(pair.first));                                                                                             
            }
        }
    }

    return true;
}
