//-----------------------------------------------------------------------------
// File: VendorExtensionAttributesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2019
//
// Description:
// Model for editing vendor extension attributes.
//-----------------------------------------------------------------------------

#include "VendorExtensionAttributesModel.h"

#include <common/KactusColors.h>

#include <editors/common/VendorExtensionEditor/VendorExtensionAttributesEditor/VendorExtensionAttributesColumns.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::VendorExtensionAttributesModel()
//-----------------------------------------------------------------------------
VendorExtensionAttributesModel::VendorExtensionAttributesModel(GenericVendorExtension* genericExtension,
    QObject* parent) :
QAbstractTableModel(parent),
attributes_(genericExtension->getAttributes())
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::~VendorExtensionAttributesModel()
//-----------------------------------------------------------------------------
VendorExtensionAttributesModel::~VendorExtensionAttributesModel()
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::columnCount()
//-----------------------------------------------------------------------------
int VendorExtensionAttributesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return VendorExtensionAttributesColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::rowCount()
//-----------------------------------------------------------------------------
int VendorExtensionAttributesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return attributes_.count();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::headerData()
//-----------------------------------------------------------------------------
QVariant VendorExtensionAttributesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == VendorExtensionAttributesColumns::TYPE)
        {
            return tr("Type");
        }
        else if (section == VendorExtensionAttributesColumns::VALUE)
        {
            return tr("Value");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::data()
//-----------------------------------------------------------------------------
QVariant VendorExtensionAttributesModel::data(const QModelIndex &index, int role) const
{
    if (indexIsNotValid(index))
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == VendorExtensionAttributesColumns::TYPE)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::setData()
//-----------------------------------------------------------------------------
bool VendorExtensionAttributesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (indexIsNotValid(index))
    {
        return false;
    }

    if (role == Qt::EditRole && (index.column() == VendorExtensionAttributesColumns::TYPE ||
        index.column() == VendorExtensionAttributesColumns::VALUE))
    {
        QPair<QString, QString> newPair = attributes_.at(index.row());
        QString newValue = value.toString();

        if (index.column() == VendorExtensionAttributesColumns::TYPE && !newValue.isEmpty() &&
            !nameExistsInAttributes(newValue))
        {
            newPair.first = newValue;
        }
        else if (index.column() == VendorExtensionAttributesColumns::VALUE)
        {
            newPair.second = newValue;
        }
        else
        {
            return false;
        }

        attributes_.replace(index.row(), newPair);
    }

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags VendorExtensionAttributesModel::flags(QModelIndex const& index) const
{
    if (indexIsNotValid(index))
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::isNotValidIndex()
//-----------------------------------------------------------------------------
bool VendorExtensionAttributesModel::indexIsNotValid(QModelIndex const& index) const
{
    return !index.isValid() || index.row() < 0 || index.row() >= attributes_.size() || index.column() < 0 ||
        index.column() >= VendorExtensionAttributesColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant VendorExtensionAttributesModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == VendorExtensionAttributesColumns::TYPE ||
        index.column() == VendorExtensionAttributesColumns::VALUE)
    {
        QPair<QString, QString> indexedAttribute =  attributes_.at(index.row());

        if (index.column() == VendorExtensionAttributesColumns::TYPE)
        {
            return indexedAttribute.first;
        }
        else if (index.column() == VendorExtensionAttributesColumns::VALUE)
        {
            return indexedAttribute.second;
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::onAddItem()
//-----------------------------------------------------------------------------
void VendorExtensionAttributesModel::onAddItem(QModelIndex const& index)
{
    int row = rowCount();
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);

    QPair<QString, QString> newAttribute;
    newAttribute.first = getUniqueNameForNewAttribute();
    attributes_.append(newAttribute);

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::getUniqueNameForNewAttribute()
//-----------------------------------------------------------------------------
QString VendorExtensionAttributesModel::getUniqueNameForNewAttribute() const
{
    QString newAttributeName = QStringLiteral("attribute");
    QString changingName = newAttributeName + QStringLiteral("_");

    int cycleCount = 0;
    while (nameExistsInAttributes(newAttributeName))
    {
        newAttributeName = changingName;
        newAttributeName.append(QString::number(cycleCount));
        cycleCount++;
    }

    return newAttributeName;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::nameExistsInAttributes()
//-----------------------------------------------------------------------------
bool VendorExtensionAttributesModel::nameExistsInAttributes(QString const& attributeName) const
{
    for (auto const& attribute : attributes_)
    {
        if (attribute.first == attributeName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void VendorExtensionAttributesModel::onRemoveItem(QModelIndex const& index)
{
    if (index.isValid())
    {
        int row = index.row();
        
        beginRemoveRows(QModelIndex(), row, row);
        attributes_.removeAt(row);
        endRemoveRows();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionAttributesModel::getAttributes()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > VendorExtensionAttributesModel::getAttributes() const
{
    return attributes_;
}
