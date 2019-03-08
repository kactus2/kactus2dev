//-----------------------------------------------------------------------------
// File: VendorExtensionsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2018
//
// Description:
// Model class to manage the vendor extension values being edited.
//-----------------------------------------------------------------------------

#include "VendorExtensionsModel.h"

#include "VendorExtensionColumns.h"

#include <common/KactusColors.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::VendorExtensionsModel()
//-----------------------------------------------------------------------------
VendorExtensionsModel::VendorExtensionsModel(QObject *parent) :
    QAbstractItemModel(parent),
    vendorExtensions_(nullptr)
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::setVendorExtensions()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::setVendorExtensions(
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > extensions)
{
    beginResetModel();
    vendorExtensions_ = extensions;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::clear()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::clear() 
{
	beginResetModel();
    vendorExtensions_ = nullptr;
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::rowCount()
//-----------------------------------------------------------------------------
int VendorExtensionsModel::rowCount(QModelIndex const& parent) const 
{
    if (parent.isValid() == false && vendorExtensions_.isNull() == false)
    {
        return vendorExtensions_->size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::columnCount()
//-----------------------------------------------------------------------------
int VendorExtensionsModel::columnCount(QModelIndex const& /*parent*/) const 
{
    return VendorExtensionColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::data()
//-----------------------------------------------------------------------------
QVariant VendorExtensionsModel::data(QModelIndex const& index, int role) const 
{
    if (!isIndexValid(index))
    {
        return QVariant();
    }
    
    if (role == Qt::ForegroundRole)
    {
        if (indexIsEditable(index))
        {
            return KactusColors::REGULAR_TEXT;
        }        
        else
        {
            return KactusColors::DISABLED_TEXT;
        }
    }
    else if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant VendorExtensionsModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<VendorExtension> extension = vendorExtensions_->at(index.row());
    QSharedPointer<GenericVendorExtension> genericExtension = extension.dynamicCast<GenericVendorExtension>();

    if (index.column() == VendorExtensionColumns::NAME)
    {
        if (genericExtension.isNull())
        {
            return extension->type();
        }
        else
        {
            return genericExtension->name();
        }
    }
    else if (index.column() == VendorExtensionColumns::TYPE)
    {
        if (genericExtension.isNull() == false)
        {
            return genericExtension->attributeValue(QStringLiteral("type"));
        }
    }
    else if (index.column() == VendorExtensionColumns::VALUE)
    {
        if (genericExtension.isNull())
        {
            return QStringLiteral("N/A");
        }
        else
        {
            return genericExtension->value();
        }
    }
    else if (index.column() == VendorExtensionColumns::DESCRIPTION && !genericExtension.isNull())
    {
        return genericExtension->getDescription();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::headerData()
//-----------------------------------------------------------------------------
QVariant VendorExtensionsModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
    {
        if (section == VendorExtensionColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == VendorExtensionColumns::TYPE)
        {
            return tr("Type");
        }
        else if (section == VendorExtensionColumns::VALUE)
        {            
            return tr("Value");
        }
        else if (section == VendorExtensionColumns::DESCRIPTION)
        {
            return tr("Description");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::setData()
//-----------------------------------------------------------------------------
bool VendorExtensionsModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
    if (!isIndexValid(index))
    {
        return false;
    }
    
    if (role == Qt::EditRole && indexIsEditable(index))
    {        
        QSharedPointer<GenericVendorExtension> genericExtension = 
            vendorExtensions_->at(index.row()).dynamicCast<GenericVendorExtension>();

        if (index.column() == VendorExtensionColumns::NAME && value.toString().isEmpty() == false)
        {
            genericExtension->setName(value.toString());
        }
        else if (index.column() == VendorExtensionColumns::TYPE)
        {
            genericExtension->setAttributeValue(QStringLiteral("type"), value.toString());
        }
        else if (index.column() == VendorExtensionColumns::VALUE)
        {
            genericExtension->setValue(value.toString());
        }
        else if (index.column() == VendorExtensionColumns::DESCRIPTION)
        {
            genericExtension->setDescription(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags VendorExtensionsModel::flags(QModelIndex const& index) const 
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

    if (indexIsEditable(index))
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
 
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::indexIsEditable()
//-----------------------------------------------------------------------------
bool VendorExtensionsModel::indexIsEditable(QModelIndex const& index) const
{
    if (isIndexValid(index))
    {
        return (vendorExtensions_->at(index.row()).dynamicCast<GenericVendorExtension>().isNull() == false);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::onAddItem()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::onAddItem(QModelIndex const& index)
{
    if (vendorExtensions_.isNull())
    {
        return;
    }

    QSharedPointer<GenericVendorExtension> newExtension(new GenericVendorExtension());
    newExtension->setName(QStringLiteral("unnamed"));

    int row = vendorExtensions_->size();
    if (isIndexValid(index))
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    vendorExtensions_->insert(row, newExtension);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::onRemoveItem(QModelIndex const& index)
{
    if (indexIsEditable(index))
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        vendorExtensions_->removeAt(index.row());
        endRemoveRows();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::index()
//-----------------------------------------------------------------------------
QModelIndex VendorExtensionsModel::index(int row, int column, const QModelIndex& ) const
{
    if (vendorExtensions_.isNull())
    {
        return createIndex(row, column, nullptr);
    }
    else
    {
        QSharedPointer<VendorExtension> extension = vendorExtensions_->at(row);
        return createIndex(row, column, extension.data());
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::parent()
//-----------------------------------------------------------------------------
QModelIndex VendorExtensionsModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::isIndexValid()
//-----------------------------------------------------------------------------
bool VendorExtensionsModel::isIndexValid(QModelIndex const& index) const
{
    if (index.isValid() && index.row() >= 0 && vendorExtensions_.isNull() == false &&
        index.row() < vendorExtensions_->size())
    {
        return true;
    }

    return false;
}
