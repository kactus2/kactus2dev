//-----------------------------------------------------------------------------
// File: LibraryModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The model that contains the LibraryItems to display library hierarchically.
//-----------------------------------------------------------------------------

#include "LibraryModel.h"
#include "LibraryItem.h"
#include "LibraryInterface.h"

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <common/KactusUtils.h>

#include <KactusAPI/include/KactusColors.h>

#include <QSharedPointer>
#include <QBrush>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: LibraryModel::LibraryModel()
//-----------------------------------------------------------------------------
LibraryModel::LibraryModel(LibraryInterface* handler, QObject* parent):
QAbstractItemModel(parent),
    handler_(handler),
    rootItem_()
{
    onResetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::~LibraryModel()
//-----------------------------------------------------------------------------
LibraryModel::~LibraryModel()
{
    delete rootItem_;
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::headerData()
//-----------------------------------------------------------------------------
QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return tr("Library items");
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::columnCount()
//-----------------------------------------------------------------------------
int LibraryModel::columnCount(QModelIndex const&) const
{
    return 1;
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::rowCount()
//-----------------------------------------------------------------------------
int LibraryModel::rowCount(QModelIndex const& parent) const
{
    // only one column exists
    if (parent.column() > 0)
    {
        return 0;
    }

    LibraryItem *parentItem = 0;

    // if the given item is invalid, it is interpreted as root item
    if (!parent.isValid()) 
    {
        parentItem = rootItem_;
    }
    else
    {
        parentItem = static_cast<LibraryItem*>(parent.internalPointer());
    }

    // return how many children the parent has
    return parentItem->getNumberOfChildren();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::index()
//-----------------------------------------------------------------------------
QModelIndex LibraryModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    LibraryItem* parentItem = 0;

    // if the index of the parent is invalid then it is the root item
    if (!parent.isValid()) 
    {
        parentItem = rootItem_;
    }
    else
    {
        parentItem = static_cast<LibraryItem*>(parent.internalPointer());
    }

    if (!parentItem)
    {
        return QModelIndex();
    }

    // find the child item of the parent
    LibraryItem *child = parentItem->child(row);

    // if the child is found
    if (child)
    {
        return createIndex(row, column, child);
    }
    else
    {
        return QModelIndex();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::index()
//-----------------------------------------------------------------------------
QModelIndex LibraryModel::index(LibraryItem* item)
{
    // if pointer is null
    if (!item)
    {
        return QModelIndex();
    }

    // if this item has no parent then it is the root item
    LibraryItem* parent = item->parent();
    if (!parent)
    {
        return QModelIndex();
    }

    // create the index for the child
    return createIndex(item->row(), 0, item);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::parent()
//-----------------------------------------------------------------------------
QModelIndex LibraryModel::parent(QModelIndex const& child) const
{
    if(!child.isValid())
    {
        return QModelIndex();
    }

    LibraryItem *childItem = static_cast<LibraryItem*>(child.internalPointer());
    LibraryItem *parent = childItem->parent();

    // If the parent does not exist then this item is the root item.
    if (!parent)
    {
        return QModelIndex();
    }

    // If row is invalid then the grandparent does not exist and parent is the root.
    int row = parent->row();
    if (row == -1)
    {
        return QModelIndex();
    }

    return createIndex(row, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::data()
//-----------------------------------------------------------------------------
QVariant LibraryModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        return item->name();
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LibraryModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::hasChildren()
//-----------------------------------------------------------------------------
bool LibraryModel::hasChildren(QModelIndex const& parent) const
{
    // only one column exists
    if (parent.column() > 0)
    {
        return false;
    }

    LibraryItem *parentItem = 0;

    // if the given item is invalid, it is interpreted as root item
    if (!parent.isValid())
    {
        parentItem = rootItem_;
    }
    else
    {
        parentItem = static_cast<LibraryItem*>(parent.internalPointer());
    }

    // return how many children the parent has
    return parentItem->hasChildren();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onExportItem()
//-----------------------------------------------------------------------------
void LibraryModel::onExportItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    emit exportItems(item->getVLNVs().toList());
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onDeleteItem()
//-----------------------------------------------------------------------------
void LibraryModel::onDeleteItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* child = static_cast<LibraryItem*>(index.internalPointer());
    LibraryItem* toRemove = child->findHighestUnique();

    // inform the library handler that these VLNVs should be removed
    emit removeVLNV(toRemove->getVLNVs().toList());
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryModel::onRemoveVLNV(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
        return;
    }

    LibraryItem* toRemove = rootItem_->findHighestUnique(vlnv);
    
    // Remove the item and it's sub-items.
    if (toRemove)
    {
        removeLibraryItem(toRemove);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onAddVLNV()
//-----------------------------------------------------------------------------
void LibraryModel::onAddVLNV(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
        return;
    }

    LibraryItem* parentItem = rootItem_->findHighestUnique(vlnv);

    QModelIndex parentIndex;

    // if the highest unique is not the root
    if (parentItem != rootItem_)
    {
        int row = parentItem->row();
        Q_ASSERT(row >= 0);

        parentIndex = createIndex(row, 0, parentItem);
    }
    
    int rowToAdd = parentItem->getNumberOfChildren();

    beginInsertRows(parentIndex, rowToAdd, rowToAdd);
    rootItem_->createChild(vlnv, static_cast<LibraryItem::Level>(0));
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::removeLibraryItem()
//-----------------------------------------------------------------------------
void LibraryModel::removeLibraryItem(LibraryItem* toRemove)
{
    int row = toRemove->parent()->getIndexOf(toRemove);

    QModelIndex removeIndex = createIndex(row, 0, toRemove);

    // get the model index of the parent
    QModelIndex parentIndex = parent(removeIndex);

     beginRemoveRows(parentIndex, row, row);
     toRemove->parent()->removeChild(toRemove);
     endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onResetModel()
//-----------------------------------------------------------------------------
void LibraryModel::onResetModel()
{    
    beginResetModel();

    if (!rootItem_)
    {
        rootItem_ = new LibraryItem(this);
    }
    else
    {
        rootItem_->clear();
    }

    // get the items to be displayed from the data source
    for (VLNV const& item : handler_->getAllVLNVs())
    {
        VLNV::IPXactType documentType = item.getType();
        if (documentType == VLNV::ABSTRACTIONDEFINITION || documentType == VLNV::BUSDEFINITION ||
            documentType == VLNV::CATALOG || documentType == VLNV::COMPONENT ||
            documentType == VLNV::DESIGN || documentType == VLNV::DESIGNCONFIGURATION ||
            documentType == VLNV::APIDEFINITION || documentType == VLNV::COMDEFINITION)
        {
            rootItem_->createChild(item, LibraryItem::Level::ROOT);
        }
    }

    validate(rootItem_);

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onDocumentUpdated()
//-----------------------------------------------------------------------------
void LibraryModel::onDocumentUpdated(VLNV const& vlnv)
{
    LibraryItem* item = rootItem_->findHighestUnique(vlnv);
    if (item)
    {
        validate(item);

        QModelIndex itemIndex = index(item);
        emit dataChanged(itemIndex, itemIndex);
    }    
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::getRoot()
//-----------------------------------------------------------------------------
LibraryItem* LibraryModel::getRoot() const
{
    return rootItem_;
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onCreateAbsDef()
//-----------------------------------------------------------------------------
void LibraryModel::onCreateAbsDef(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::BUSDEFINITION);
    item->setVlnv(vlnv);

    emit createAbsDef(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onOpenDocument()
//-----------------------------------------------------------------------------
void LibraryModel::onOpenDocument(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    if (vlnv.isValid())
    {
        emit editItem(vlnv);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onOpenDesign(QModelIndex const& index, QString const& viewName)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    if (vlnv.isValid())
    {
        emit openDesign(vlnv, viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onOpenMemoryDesign(QModelIndex const& index, QString const& viewName)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    if (item)
    {
        VLNV vlnv = item->getVLNV();

        if (vlnv.isValid())
        {
            emit openMemoryDesign(vlnv, viewName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onOpenSWDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    if (vlnv.isValid())
    {
        emit openSWDesign(vlnv);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onOpenSystemDesign(QModelIndex const& index, QString const& viewName)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    if (vlnv.isValid())
    {
        emit openSystemDesign(vlnv, viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onCreateNewDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onCreateNewDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMPONENT);
    item->setVlnv(vlnv);

    emit createDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onCreateNewSWDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onCreateNewSWDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMPONENT);
    item->setVlnv(vlnv);

    emit createSWDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::onCreateNewSystemDesign()
//-----------------------------------------------------------------------------
void LibraryModel::onCreateNewSystemDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMPONENT);
    item->setVlnv(vlnv);

    emit createSystemDesign(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryModel::onShowErrors(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    emit showErrors(item->getVLNV());
}

//-----------------------------------------------------------------------------
// Function: LibraryModel::validate()
//-----------------------------------------------------------------------------
bool LibraryModel::validate(LibraryItem* item)
{
    bool isValid = false;
    if (item->getLevel() == LibraryItem::Level::VERSION)
    {
        isValid = handler_->isValid(item->getVLNV());
    }
    else
    {
        bool hasOneValidChild = false;

        int childCount = item->getNumberOfChildren();        
        for (int i = 0; i < childCount; i++)
        {
            bool childIsValid = validate(item->child(i));
            hasOneValidChild |= childIsValid;
        }

        isValid = hasOneValidChild;
    }

    item->setValid(isValid);
    return isValid;
}
