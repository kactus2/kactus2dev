//-----------------------------------------------------------------------------
// File: hierarchymodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// Contains the items to display the library component hierarchy to user.
//-----------------------------------------------------------------------------

#include "hierarchymodel.h"

#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/libraryhandler.h>
#include <library/LibraryManager/librarydata.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QColor>
#include <QMap>

//-----------------------------------------------------------------------------
// Function: HierarchyModel::HierarchyModel()
//-----------------------------------------------------------------------------
HierarchyModel::HierarchyModel(LibraryData* sourceModel, LibraryInterface* handler, QObject* parent):
QAbstractItemModel(parent),
    dataModel_(sourceModel),
    rootItem_(),
    handler_(handler)
{
	// connect the signals that are needed to print messages
	LibraryHandler* handler2 = static_cast<LibraryHandler*>(handler);
	connect(this, SIGNAL(errorMessage(const QString&)),
		handler2, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		handler2, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	onResetModel();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::~HierarchyModel()
//-----------------------------------------------------------------------------
HierarchyModel::~HierarchyModel()
{
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onResetModel()
//-----------------------------------------------------------------------------
void HierarchyModel::onResetModel()
{
	beginResetModel();

	// create the root item if it does not yet exist
	if (!rootItem_)
    {
		rootItem_ = new HierarchyItem(handler_, this);
		connect(rootItem_, SIGNAL(errorMessage(const QString&)),
			this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(rootItem_, SIGNAL(noticeMessage(const QString&)),
			this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	}
	// if root exists then tell it to remove children
	else
    {
		rootItem_->clear();
	}
	
	QList<VLNV> absDefs;

	// add all items to this model
	foreach (VLNV const& itemVlnv, dataModel_->getItems())
    {
		// make sure the item can be parsed
		QSharedPointer<Document const> libComp = handler_->getModelReadOnly(itemVlnv);
		if (libComp)
        {
            VLNV::IPXactType documentType = handler_->getDocumentType(itemVlnv);

            if (documentType == VLNV::COMPONENT || documentType == VLNV::BUSDEFINITION ||
                documentType == VLNV::COMDEFINITION || documentType == VLNV::APIDEFINITION) 
            {
                rootItem_->createChild(itemVlnv);
            }
            else if (documentType == VLNV::ABSTRACTIONDEFINITION)
            {
                absDefs.append(itemVlnv);
                rootItem_->createChild(itemVlnv);
            }
		}
	}

	// create the abstraction definitions
	foreach (VLNV const& absDefVlnv, absDefs) 
    {
		QSharedPointer<Document const> libComp = handler_->getModelReadOnly(absDefVlnv);
		QSharedPointer<AbstractionDefinition const> absDef = libComp.staticCast<AbstractionDefinition const>();

        foreach (HierarchyItem* busDefItem, rootItem_->findItems(absDef->getBusType()))
        {
			busDefItem->createChild(absDefVlnv);
        }
	}

	// remove the duplicates
	rootItem_->cleanUp();

	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::headerData()
//-----------------------------------------------------------------------------
QVariant HierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == HierarchyModel::OBJECT_COLUMN)
        {
            return tr("Library items");
        }
        else if (section == HierarchyModel::INSTANCE_COLUMN)
        {
            return tr("Instances");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::columnCount()
//-----------------------------------------------------------------------------
int HierarchyModel::columnCount(QModelIndex const&) const
{
	return HierarchyModel::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::rowCount()
//-----------------------------------------------------------------------------
int HierarchyModel::rowCount(QModelIndex const& parent) const
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
// Function: HierarchyModel::index()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModel::index(int row, int column, QModelIndex const& parent) const
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
// Function: HierarchyModel::index()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModel::index(HierarchyItem* item) const
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
// Function: HierarchyModel::findIndexes()
//-----------------------------------------------------------------------------
QModelIndexList HierarchyModel::findIndexes(VLNV const& vlnv)
{
	QModelIndexList list;

	foreach (HierarchyItem* item, rootItem_->findItems(vlnv))
    {
		list.append(index(item));
	}

	return list;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::parent()
//-----------------------------------------------------------------------------
QModelIndex HierarchyModel::parent(QModelIndex const& child) const
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
// Function: HierarchyModel::data()
//-----------------------------------------------------------------------------
QVariant HierarchyModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid())
    {
		return QVariant();
    }

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	if (role == Qt::DisplayRole) 
    {
        if (index.column() == HierarchyModel::OBJECT_COLUMN)
        {
            VLNV vlnv = item->getVLNV();
            if (vlnv.isValid())
            {
                return vlnv.toString();
            }
        }
        else if (index.column() == HierarchyModel::INSTANCE_COLUMN)
        {
            int count = item->instanceCount();
            if (count > 0)
            {
                return count;
            }
        }        
        
        return QVariant();
	}
	else if (role == Qt::ForegroundRole)
    {
		if (item->isValid()) 
        {
			return QColor("black");
        }
		else
        {
			return QColor("red");
        }
	}
	else if (role == Qt::ToolTipRole)
    {
        VLNV vlnv = item->getVLNV();

        QString text = "<b>Vendor:</b> " + vlnv.getVendor() + "<br>" +
                       "<b>Library:</b> " + vlnv.getLibrary() + "<br>" +
                       "<b>Name:</b> " + vlnv.getName() + "<br>" +
                       "<b>Version:</b> " + vlnv.getVersion() + "<br>";

        QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);

        if (document != 0 && !document->getDescription().isEmpty())
        {
            text += "<br><b>Description:</b><br>" + document->getDescription();
        }

        text += QString("<br><b>File Path:</b><br>%1").arg(dataModel_->getPath(vlnv));
        return text;
	}
	else if (role == Qt::DecorationRole && index.column() == HierarchyModel::OBJECT_COLUMN)
    {
        if (item->type() == HierarchyItem::COMPONENT)
        {
            if (item->getImplementation() == KactusAttribute::SYSTEM)
            {
                return QIcon(":/icons/common/graphics/system-component.png");
            }
            else if (item->getImplementation() == KactusAttribute::SW)
            {
                if (item->isHierarchical())
                {
                    return QIcon(":/icons/common/graphics/hier-sw-component.png");
                }
                else
                {
                    return QIcon(":/icons/common/graphics/sw-component24x24.png");
                }
            }
            else
            {
                if (item->isHierarchical()) 
                {
                    return QIcon(":/icons/common/graphics/hier-hw-component.png");
                }
                else
                {
                    return QIcon(":/icons/common/graphics/hw-component.png");
                }		
            }
        }
        else if (item->type() == HierarchyItem::COMDEFINITION)
        {
            return QIcon(":/icons/common/graphics/new-com_definition.png");
        }
        else if (item->type() == HierarchyItem::APIDEFINITION)
        {
            return QIcon(":/icons/common/graphics/new-api_definition.png");
        }
		else if (item->type() == HierarchyItem::HW_DESIGN)
        {
			return QIcon(":/icons/common/graphics/hw-design.png");
		}
		else if (item->type() == HierarchyItem::SW_DESIGN)
        {
			return QIcon(":/icons/common/graphics/sw-design24x24.png");
		}
		else if (item->type() == HierarchyItem::SYS_DESIGN)
        {
			return QIcon(":/icons/common/graphics/system-design.png");
		}
        else if (item->type() == HierarchyItem::ABSDEFINITION)
        {
            return QIcon(":/icons/common/graphics/abs-def.png");
        }
        else
        {
			return QIcon(":/icons/common/graphics/new-bus.png");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::hasChildren()
//-----------------------------------------------------------------------------
bool HierarchyModel::hasChildren(QModelIndex const& parent) const
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
// Function: HierarchyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags HierarchyModel::flags(QModelIndex const& index) const
{
	if (!index.isValid() || index.column() == HierarchyModel::INSTANCE_COLUMN)
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onOpenDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onOpenDesign(QModelIndex const& index)
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

	emit openDesign(componentVLNV, item->getViewName());
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onOpenMemoryDesign(QModelIndex const& index)
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
// Function: HierarchyModel::onOpenSWDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onOpenSWDesign(QModelIndex const& index)
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
	Q_ASSERT(item->type() == HierarchyItem::SW_DESIGN);

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

	emit openSWDesign(componentVLNV, item->getViewName());
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onOpenSystemDesign(QModelIndex const& index)
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
	Q_ASSERT(item->type() == HierarchyItem::SYS_DESIGN);

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

	emit openSystemDesign(componentVLNV, item->getViewName());
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onOpenComponent()
//-----------------------------------------------------------------------------
void HierarchyModel::onOpenComponent(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV componentVLNV = item->getVLNV();
	if (componentVLNV.isValid())
    {
		emit editItem(componentVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onCreateNewComponent()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewComponent(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV componentVLNV = item->getVLNV();
	componentVLNV.setType(VLNV::COMPONENT);
	
	emit createComponent(componentVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onCreateNewDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewDesign(QModelIndex const& index)
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
// Function: HierarchyModel::onCreateNewSWDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewSWDesign(QModelIndex const& index)
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
// Function: HierarchyModel::onCreateNewSystemDesign()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewSystemDesign(QModelIndex const& index)
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
// Function: HierarchyModel::onCreateNewBus()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewBus(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV busVLNV = item->getVLNV();
	busVLNV.setType(VLNV::BUSDEFINITION);

	emit createBus(busVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onCreateNewAbsDef()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewAbsDef(QModelIndex const& index)
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
// Function: HierarchyModel::onCreateNewComDef()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewComDef(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV comVLNV = item->getVLNV();
    comVLNV.setType(VLNV::COMDEFINITION);

    emit createComDef(comVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onCreateNewApiDef()
//-----------------------------------------------------------------------------
void HierarchyModel::onCreateNewApiDef(QModelIndex const& index) 
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

    VLNV apiVLNV = item->getVLNV();
    apiVLNV.setType(VLNV::APIDEFINITION);

    emit createApiDef(apiVLNV);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onExportItem()
//-----------------------------------------------------------------------------
void HierarchyModel::onExportItem(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
	}

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	emit exportItem(item->getVLNV());
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onRemoveVLNV()
//-----------------------------------------------------------------------------
void HierarchyModel::onRemoveVLNV(VLNV const& vlnv)
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
// Function: HierarchyModel::referenceCount()
//-----------------------------------------------------------------------------
int HierarchyModel::referenceCount(VLNV const& vlnv) const
{
	return rootItem_->referenceCount(vlnv);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onComponentSaved()
//-----------------------------------------------------------------------------
void HierarchyModel::onDocumentSaved(VLNV const& vlnv)
{
    bool isValid = handler_->isValid(vlnv);
    foreach (HierarchyItem* savedItem, rootItem_->findItems(vlnv))
    {
        savedItem->setValidity(isValid);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::getOwners()
//-----------------------------------------------------------------------------
int HierarchyModel::getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const
{
	rootItem_->getOwners(list, vlnvToSearch);
	return list.size();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::getChildren()
//-----------------------------------------------------------------------------
void HierarchyModel::getChildren(QList<VLNV>& childList, VLNV const& owner)
{
	rootItem_->getChildren(childList, owner);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::onShowErrors()
//-----------------------------------------------------------------------------
void HierarchyModel::onShowErrors(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    emit showErrors(vlnv);
}
