//-----------------------------------------------------------------------------
// File: HierarchyModelBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// Contains the items to display the library component hierarchy to user.
//-----------------------------------------------------------------------------

#include "HierarchyModelBase.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <KactusAPI/include/KactusColors.h>

#include <common/KactusUtils.h>

#include <QStringBuilder>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::HierarchyModelBase()
//-----------------------------------------------------------------------------
HierarchyModelBase::HierarchyModelBase(LibraryInterface* handler, QObject* parent):
QAbstractItemModel(parent),
    rootItem_(new HierarchyItem(handler, this)),
    handler_(handler)
{
    connect(rootItem_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(rootItem_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onResetModel()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onResetModel()
{
    beginResetModel();

    rootItem_->clear();

    QVector<VLNV> absDefs;

    // add all items to this model
    for (VLNV const& itemVlnv : handler_->getAllVLNVs())
    {
        VLNV::IPXactType documentType = itemVlnv.getType();

        //! Add supported item types only. Designs and configurations will be created by their top-components.
        if (documentType == VLNV::ABSTRACTIONDEFINITION || documentType == VLNV::BUSDEFINITION ||
            documentType == VLNV::CATALOG || documentType == VLNV::COMPONENT ||
            documentType == VLNV::APIDEFINITION || documentType == VLNV::COMDEFINITION)
        {
            rootItem_->createChild(itemVlnv);

            if (documentType == VLNV::ABSTRACTIONDEFINITION)
            {
                absDefs.append(itemVlnv);
            }
        }
    }

    // create the abstraction definitions
    for (VLNV const& absDefVlnv : absDefs) 
    {
    	QSharedPointer<AbstractionDefinition const> absDef = 
            handler_->getModelReadOnly(absDefVlnv).staticCast<AbstractionDefinition const>();

        for (HierarchyItem* busDefItem : rootItem_->findItems(absDef->getBusType()))
        {
    		busDefItem->createChild(absDefVlnv);
        }
    }

    // Remove the duplicates.
    rootItem_->cleanUp();

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::headerData()
//-----------------------------------------------------------------------------
QVariant HierarchyModelBase::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == HierarchyModelBase::OBJECT_COLUMN)
        {
            return tr("Library items");
        }
        else if (section == HierarchyModelBase::INSTANCE_COLUMN)
        {
            return tr("Instances");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::columnCount()
//-----------------------------------------------------------------------------
int HierarchyModelBase::columnCount(QModelIndex const&) const
{
    return HierarchyModelBase::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::rowCount()
//-----------------------------------------------------------------------------
int HierarchyModelBase::rowCount(QModelIndex const& parent) const
{
    // only one column exists
    if (parent.column() > 0)
    {
    	return 0;
    }

    HierarchyItem* parentItem = 0;

    // if the given item is invalid then it is the root
    if (!parent.isValid())
    {
    	parentItem = rootItem_;
    }
    else
    {
    	parentItem = static_cast<HierarchyItem*>(parent.internalPointer());
    }

    return parentItem->getNumberOfChildren();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::index()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModelBase::index(int row, int column, QModelIndex const& parent) const
{
    if (!hasIndex(row, column, parent))
    {
    	return QModelIndex();
    }

    HierarchyItem* parentItem = 0;

    // if the index of the parent is invalid then it is the root
    if (!parent.isValid())
    {
    	parentItem = rootItem_;
    }
    else
    {
    	parentItem = static_cast<HierarchyItem*>(parent.internalPointer());
    }

    HierarchyItem* child = parentItem->child(row);
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
// Function: HierarchyModelBase::index()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModelBase::index(HierarchyItem* item) const
{
    if (!item)
    {
    	return QModelIndex();
    }

    // if this item has no parent then it is the root item
    HierarchyItem* parent = item->parent();
    if (!parent)
    {
    	return QModelIndex();
    }

    return createIndex(item->row(), 0, item);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::findIndexes()
//-----------------------------------------------------------------------------
QModelIndexList HierarchyModelBase::findIndexes(VLNV const& vlnv)
{
    QModelIndexList list;

    for (HierarchyItem* item : rootItem_->findItems(vlnv))
    {
    	list.append(index(item));
    }

    return list;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::parent()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModelBase::parent(QModelIndex const& child) const
{
    if (!child.isValid())
    {
    	return QModelIndex();
    }

    HierarchyItem* childItem = static_cast<HierarchyItem*>(child.internalPointer());
    HierarchyItem* parent = childItem->parent();

    // if the parent does not exist then this item is root item
    if (!parent)
    {
    	return QModelIndex();
    }

    // If row is invalid then the grandparent does not exist and the parent is the root.
    int row = parent->row();
    if (row == -1)
    {
    	return QModelIndex();
    }

    // create new index and return it
    return createIndex(row, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::data()
//-----------------------------------------------------------------------------
QVariant HierarchyModelBase::data(QModelIndex const& index, int role) const
{
    // Overridden by HierarchyModel
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::hasChildren()
//-----------------------------------------------------------------------------
bool HierarchyModelBase::hasChildren(QModelIndex const& parent) const
{
    // only one column exists
    if (parent.column() > 0)
    {
    	return false;
    }

    HierarchyItem* parentItem = 0;

    // if given item is invalid then it is root item
    if (!parent.isValid())
    {
    	parentItem = rootItem_;
    }
    else
    {
    	parentItem = static_cast<HierarchyItem*>(parent.internalPointer());
    }

    return parentItem->hasChildren();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags HierarchyModelBase::flags(QModelIndex const& index) const
{
    if (!index.isValid() || index.column() == HierarchyModelBase::INSTANCE_COLUMN)
    {
    	return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onOpenDesign()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onOpenDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
    	return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
    if (!item)
    {
    	return;
    }

    // item must always be design
    Q_ASSERT(item->type() == HierarchyItem::HW_DESIGN || item->type() == HierarchyItem::SW_DESIGN);	

    // find the containing component
    HierarchyItem* parent = item->parent();
    // if the design has no parent or the parent is the root item (which is not component)
    if (!parent || parent == rootItem_)
    {
    	emit errorMessage(tr("Design did not have containing component and could not be opened."));
    	return;
    }

    Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

    // find the vlnv of the component
    VLNV componentVLNV = parent->getVLNV();
    Q_ASSERT(componentVLNV.getType() == handler_->getDocumentType(componentVLNV));

    if (item->type() == HierarchyItem::HW_DESIGN)
    {
        emit openDesign(componentVLNV, item->getViewName());
    }

    else if (item->type() == HierarchyItem::SW_DESIGN)
    {
        emit openSWDesign(componentVLNV, item->getViewName());
    }

    else if (item->type() == HierarchyItem::SYS_DESIGN)
    {
        emit openSystemDesign(componentVLNV, item->getViewName());
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onOpenMemoryDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
    if (!item)
    {
        return;
    }

    // item must always be design
    Q_ASSERT(item->type() == HierarchyItem::HW_DESIGN);

    // find the containing component
    HierarchyItem* parent = item->parent();
    // if the design has no parent or the parent is the root item (which is not component)
    if (!parent || parent == rootItem_)
    {
        emit errorMessage(tr("Design did not have containing component and could not be opened."));
        return;
    }
    Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

    // find the vlnv of the component
    VLNV componentVLNV = parent->getVLNV();
    Q_ASSERT(componentVLNV.getType() == handler_->getDocumentType(componentVLNV));

    emit openMemoryDesign(componentVLNV, item->getViewName());
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onOpenItem()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onOpenItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
    	return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV vlnv = item->getVLNV();
    if (vlnv.isValid())
    {
    	emit editItem(vlnv);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onCreateNewDesign()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onCreateNewDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
    	return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV designVLNV = item->getVLNV();
    designVLNV.setType(VLNV::COMPONENT);

    emit createDesign(designVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onCreateNewSWDesign()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onCreateNewSWDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV swDesignVLNV = item->getVLNV();
    swDesignVLNV.setType(VLNV::COMPONENT);

    emit createSWDesign(swDesignVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onCreateNewSystemDesign()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onCreateNewSystemDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV systemDesignVLNV = item->getVLNV();
    systemDesignVLNV.setType(VLNV::COMPONENT);

    emit createSystemDesign(systemDesignVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onCreateNewAbsDef()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onCreateNewAbsDef(QModelIndex const& index)
{
    if (!index.isValid())
    {
    	return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV abstractionVNLV = item->getVLNV();
    abstractionVNLV.setType(VLNV::BUSDEFINITION);

    emit createAbsDef(abstractionVNLV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onExportItem()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onExportItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
    	return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    emit exportItem(item->getVLNV());
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onDeleteItem()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onDeleteItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
   
    QList<VLNV> removedVLNVs;
    removedVLNVs.append(item->getVLNV());
    
    emit removeVLNV(removedVLNVs);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onRemoveVLNV()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onRemoveVLNV(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
    	return;
    }

    beginResetModel();
    rootItem_->removeItems(vlnv);
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::referenceCount()
//-----------------------------------------------------------------------------
int HierarchyModelBase::referenceCount(VLNV const& vlnv) const
{
    return rootItem_->referenceCount(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onDocumentUpdated()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onDocumentUpdated(VLNV const& vlnv)
{
    bool isValid = handler_->isValid(vlnv);
    for (HierarchyItem* updatedItem : rootItem_->findItems(vlnv))
    {
        updatedItem->setValidity(isValid);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::getOwners()
//-----------------------------------------------------------------------------
int HierarchyModelBase::getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const
{
    rootItem_->getOwners(list, vlnvToSearch);
    return list.size();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::getChildren()
//-----------------------------------------------------------------------------
void HierarchyModelBase::getChildren(QList<VLNV>& childList, VLNV const& owner) const
{
    rootItem_->getChildren(childList, owner);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModelBase::onShowErrors()
//-----------------------------------------------------------------------------
void HierarchyModelBase::onShowErrors(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    emit showErrors(vlnv);
}
