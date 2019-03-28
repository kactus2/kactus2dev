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

#include <common/KactusColors.h>

#include <editors/common/VendorExtensionEditor/VendorExtensionsGeneral.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionColumns.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::VendorExtensionsModel()
//-----------------------------------------------------------------------------
VendorExtensionsModel::VendorExtensionsModel(QObject *parent) :
QAbstractItemModel(parent),
vendorExtensions_(nullptr),
emptyExtension_()
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
    if (vendorExtensions_.isNull() == false)
    {
        if (!parent.isValid())
        {
            return vendorExtensions_->size();
        }
        else
        {
            VendorExtension* genericParent = static_cast<VendorExtension*>(parent.internalPointer());
            GenericVendorExtension* genericExtension = dynamic_cast<GenericVendorExtension*>(genericParent);

            if (genericExtension)
            {
                return genericExtension->getChildExtensions().count();
            }
        }
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
    
    if (role == VendorExtensionsGeneral::getGenericExtensionRole)
    {
        return QVariant::fromValue(getExtensionForIndex(index));
    }
    else if (role == Qt::ForegroundRole)
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
    else if (role == Qt::BackgroundRole)
    {
        if (mandatoryColumn(index))
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
// Function: VendorExtensionsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant VendorExtensionsModel::valueForIndex(QModelIndex const& index) const
{
    VendorExtension* extension = static_cast<VendorExtension*>(index.internalPointer());
    GenericVendorExtension* genericExtension = dynamic_cast<GenericVendorExtension*>(extension);

    if (index.column() == VendorExtensionColumns::NAMESPACE)
    {
        if (!genericExtension)
        {
            return extension->type().split(":").first();
        }
        else
        {
            return genericExtension->nameSpace();
        }
    }
    else if (index.column() == VendorExtensionColumns::NAME)
    {
        if (!genericExtension)
        {
            return extension->type().split(":").last();
        }
        else
        {
            return genericExtension->name();
        }
    }
    else if (index.column() == VendorExtensionColumns::ATTRIBUTES)
    {
        if (genericExtension)
        {
            QStringList typeList;
            for (auto attribute : genericExtension->getAttributes())
            {
                typeList.append(attribute.first);
            }

            return typeList.join(",");
        }
    }
    else if (index.column() == VendorExtensionColumns::VALUE)
    {
        if (!genericExtension)
        {
            return QStringLiteral("N/A");
        }
        else
        {
            return genericExtension->value();
        }
    }
    else if (index.column() == VendorExtensionColumns::DESCRIPTION && genericExtension)
    {
        return genericExtension->getDescription();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::mandatoryColumn()
//-----------------------------------------------------------------------------
bool VendorExtensionsModel::mandatoryColumn(QModelIndex const& index) const
{
    return index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::getExtensionForIndex()
//-----------------------------------------------------------------------------
GenericVendorExtension* VendorExtensionsModel::getExtensionForIndex(QModelIndex const& index) const
{
    VendorExtension* indexExtension = static_cast<VendorExtension*>(index.internalPointer());
    return dynamic_cast<GenericVendorExtension*>(indexExtension);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::headerData()
//-----------------------------------------------------------------------------
QVariant VendorExtensionsModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
    {
        if (section == VendorExtensionColumns::NAMESPACE)
        {
            return tr("Namespace");
        }
        else if (section == VendorExtensionColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == VendorExtensionColumns::ATTRIBUTES)
        {
            return tr("Attributes");
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
        GenericVendorExtension* genericExtension = getExtensionForIndex(index);
        if (genericExtension && index.column() == VendorExtensionColumns::NAMESPACE && !value.toString().isEmpty())
        {
            genericExtension->setNameSpace(value.toString());
        }
        else if (genericExtension &&
            index.column() == VendorExtensionColumns::NAME && value.toString().isEmpty() == false)
        {
            genericExtension->setName(value.toString());
        }
        else if (genericExtension && index.column() == VendorExtensionColumns::ATTRIBUTES)
        {
            QVector<QPair<QString, QString> > newAttributes = value.value<QVector<QPair<QString, QString> > >();
            genericExtension->setNewAttributes(newAttributes);
        }
        else if (genericExtension && index.column() == VendorExtensionColumns::VALUE)
        {
            genericExtension->setValue(value.toString());
        }
        else if (genericExtension && index.column() == VendorExtensionColumns::DESCRIPTION)
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
    GenericVendorExtension* genericExtension = getExtensionForIndex(index);
    if (genericExtension)
    {
        return true;
    }
    else
    {
        return false;
    }
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

    QString newExtensionName = QStringLiteral("unnamed");

    QSharedPointer<GenericVendorExtension> newExtension(new GenericVendorExtension());
    newExtension->setNameSpace(newExtensionName);
    newExtension->setName(newExtensionName);

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
// Function: VendorExtensionsModel::onAddSubItem()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::onAddSubItem(QModelIndex const& index)
{
    if (vendorExtensions_.isNull() || !indexIsEditable(index))
    {
        return;
    }

    GenericVendorExtension* indexedExtension = getExtensionForIndex(index);
    if (indexedExtension)
    {
        GenericVendorExtension* newChildExtension(new GenericVendorExtension());
        newChildExtension->setNameSpace(indexedExtension->nameSpace());
        newChildExtension->setName(QStringLiteral("unnamed"));
        newChildExtension->setParent(indexedExtension);
        int childExtensionCount = indexedExtension->getChildExtensions().count();
        
        beginInsertRows(index, childExtensionCount, childExtensionCount);
        indexedExtension->getChildExtensions().append(newChildExtension);
        endInsertRows();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || !indexIsEditable(index))
    {
        return;
    }

    QModelIndex parentIndex = index.parent();
    if (parentIndex.isValid())
    {
        GenericVendorExtension* genericParent = getExtensionForIndex(parentIndex);
        if (genericParent)
        {
            beginRemoveRows(parentIndex, index.row(), index.row());
            genericParent->getChildExtensions().removeAt(index.row());
            endRemoveRows();
        }
    }
    else
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        vendorExtensions_->removeAt(index.row());
        endRemoveRows();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::onRemoveAllSubItems()
//-----------------------------------------------------------------------------
void VendorExtensionsModel::onRemoveAllSubItems(QModelIndex const& index)
{
    if (vendorExtensions_.isNull() || !indexIsEditable(index))
    {
        return;
    }

    GenericVendorExtension* genericExtension = getExtensionForIndex(index);
    if (genericExtension)
    {
        int startIndex = 0;
        int endIndex = genericExtension->getChildExtensions().count() - 1;

        if (genericExtension->hasDescription())
        {
            int descriptionIndex = genericExtension->getDescriptionIndex();
            int secondStartIndex = -1;
            int secondEndIndex = -1;

            if (descriptionIndex == startIndex)
            {
                startIndex++;
            }
            else if (descriptionIndex == endIndex)
            {
                endIndex--;
            }
            else
            {
                endIndex = descriptionIndex - 1;
                secondStartIndex = descriptionIndex + 1;
                secondEndIndex = genericExtension->getChildExtensions().count() - 1;

                beginRemoveRows(index, secondStartIndex, secondEndIndex);
                genericExtension->removeIndexedChildExtensions(secondStartIndex, secondEndIndex);
                endRemoveRows();

                emit contentChanged();
            }
        }
        
        if (startIndex <= endIndex)
        {
            beginRemoveRows(index, startIndex, endIndex);
            genericExtension->removeIndexedChildExtensions(startIndex, endIndex);
            endRemoveRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::index()
//-----------------------------------------------------------------------------
QModelIndex VendorExtensionsModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!vendorExtensions_.isNull())
    {
        if (parent.isValid())
        {
            VendorExtension* parentExtension = static_cast<VendorExtension*>(parent.internalPointer());
            GenericVendorExtension* genericParent = dynamic_cast<GenericVendorExtension*>(parentExtension);
            if (genericParent)
            {
                GenericVendorExtension* indexData = genericParent->getChildExtensions()[row];
                return createIndex(row, column, indexData);
            }
        }
        else
        {
            QSharedPointer<VendorExtension> extension = vendorExtensions_->at(row);
            QSharedPointer<GenericVendorExtension> generic = extension.dynamicCast<GenericVendorExtension>();
            if (generic)
            {
                return createIndex(row, column, generic.data());
            }
            else
            {
                return createIndex(row, column, extension.data());
            }
        }
    }

    return createIndex(row, column, nullptr);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::parent()
//-----------------------------------------------------------------------------
QModelIndex VendorExtensionsModel::parent(const QModelIndex& child) const
{
    if (!vendorExtensions_.isNull() && child.isValid())
    {
        for (auto rootItem : *vendorExtensions_)
        {
            if (rootItem.data() == child.internalPointer())
            {
                return QModelIndex();
            }
        }

        return createParentIndexForChildExtension(child);
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::createParentIndexForChildExtension()
//-----------------------------------------------------------------------------
QModelIndex VendorExtensionsModel::createParentIndexForChildExtension(QModelIndex const& childIndex) const
{
    GenericVendorExtension* childExtension = static_cast<GenericVendorExtension*>(childIndex.internalPointer());

    GenericVendorExtension* parentExtension = childExtension->getParent();

    int parentRow = 0;
    if (parentExtension->getParent())
    {
        parentRow = getRowForItem(parentExtension);
    }
    else
    {
        parentRow = getRowForRootItem(parentExtension);
    }

    return createIndex(parentRow, 0, parentExtension);
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::getRowForItem()
//-----------------------------------------------------------------------------
int VendorExtensionsModel::getRowForItem(GenericVendorExtension* extensionItem) const
{
    GenericVendorExtension* parentItem = extensionItem->getParent();
    for (int i = 0; i < parentItem->getChildExtensions().count(); ++i)
    {
        if (parentItem->getChildExtensions().at(i)->isSame(*extensionItem))
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::getRowForRootItem()
//-----------------------------------------------------------------------------
int VendorExtensionsModel::getRowForRootItem(GenericVendorExtension* parentItem) const
{
    for (int i = 0; i < vendorExtensions_->size(); ++i)
    {
        QSharedPointer<GenericVendorExtension> genericRoot =
            vendorExtensions_->at(i).dynamicCast<GenericVendorExtension>();
        if (genericRoot && genericRoot.data() == parentItem)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsModel::isIndexValid()
//-----------------------------------------------------------------------------
bool VendorExtensionsModel::isIndexValid(QModelIndex const& index) const
{
    if (index.isValid() && index.row() >= 0 && vendorExtensions_.isNull() == false)
    {
        if (index.parent().isValid())
        {
            GenericVendorExtension* genericParent =
                static_cast<GenericVendorExtension*>(index.parent().internalPointer());
            return index.row() < genericParent->getChildExtensions().count();
        }
        else if (index.row() < vendorExtensions_->size())
        {
            return true;
        }
    }

    return false;
}
