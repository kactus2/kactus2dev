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
		rootItem_ = QSharedPointer<HierarchyItem>(new HierarchyItem(handler_, this));
		connect(rootItem_.data(), SIGNAL(errorMessage(const QString&)),
			this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(rootItem_.data(), SIGNAL(noticeMessage(const QString&)),
			this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	}
	// if root exists then tell it to remove children
	else
    {
		rootItem_->clear();
	}
	
	// get the items to be displayed from the data source

	QList<VLNV> absDefs;

	// add all items to this model
	foreach (VLNV const& itemVlnv, dataModel_->getItems())
    {
		// make sure the item can be parsed
		QSharedPointer<Document const> libComp = handler_->getModelReadOnly(itemVlnv);
		if (!libComp)
        {
			continue;
		}

		// if item is component, bus definition, COM definition or API definition
		if (handler_->getDocumentType(itemVlnv) == VLNV::COMPONENT ||
			handler_->getDocumentType(itemVlnv) == VLNV::BUSDEFINITION ||
			handler_->getDocumentType(itemVlnv) == VLNV::COMDEFINITION ||
			handler_->getDocumentType(itemVlnv) == VLNV::APIDEFINITION) 
        {
				rootItem_->createChild(itemVlnv);
		}

		// if item is abstraction definition append the abstraction definition to the list
		else if (handler_->getDocumentType(itemVlnv) == VLNV::ABSTRACTIONDEFINITION)
        {
			absDefs.append(itemVlnv);
			rootItem_->createChild(itemVlnv);
		}
	}

	// create the abstraction definitions
	foreach (VLNV const& absDefVlnv, absDefs) 
    {
		QSharedPointer<Document const> libComp = handler_->getModelReadOnly(absDefVlnv);
		QSharedPointer<AbstractionDefinition const> absDef = libComp.staticCast<AbstractionDefinition const>();

		VLNV busDefVlnv = absDef->getBusType();

		QList<HierarchyItem*> busDefItems = rootItem_->findItems(busDefVlnv);
		for (int i = 0; i < busDefItems.size(); i++)
        {
			busDefItems.at(i)->createChild(absDefVlnv);
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
	if (orientation != Qt::Horizontal && role != Qt::DisplayRole)
    {
		return QVariant();
	}

    if (section == HierarchyModel::OBJECT_COLUMN)
    {
        return tr("Library items");
    }
    else if (section == HierarchyModel::INSTANCE_COLUMN)
    {
        return tr("Instances");
    }
    else
    {
        return QVariant();
    }
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
		parentItem = rootItem_.data();
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
		parentItem = rootItem_.data();
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

	// create the index for the child
	return createIndex(item->row(), 0, item);
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::findIndexes()
//-----------------------------------------------------------------------------
QModelIndexList HierarchyModel::findIndexes(VLNV const& vlnv)
{
	QList<HierarchyItem*> items = rootItem_->findItems(vlnv);

	QModelIndexList list;

	// get model index of each hierarchy item
	foreach (HierarchyItem* item, items)
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

	// pointer to the child item
	HierarchyItem* childItem = static_cast<HierarchyItem*>(child.internalPointer());
	HierarchyItem* parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent)
    {
		return QModelIndex();
    }

	// if row is invalid then the grandparent does not exist and the parent is
	// a root item so we return an invalid QModelIndex
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

	// for instance count column
	if (index.column() == HierarchyModel::INSTANCE_COLUMN)
    {
		int count = item->instanceCount();
		if (Qt::DisplayRole == role && count > 0)
        {
			return count;
		}
		else 
        {
			return QVariant();
		}
	}

	if (role == Qt::DisplayRole) 
    {
		VLNV vlnv = item->getVLNV();
		if (vlnv.isValid())
        {
			return vlnv.toString();
		}
		else
        {
			return QVariant();
        }
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

        QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);

        if (libComp != 0 && !libComp->getDescription().isEmpty())
        {
            text += "<br><b>Description:</b><br>" + libComp->getDescription();
        }

        text += QString("<br><b>File Path:</b><br>%1").arg(dataModel_->getPath(vlnv));
        return text;
	}
	else if (role == Qt::DecorationRole)
    {
        // if item is component
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
		// if item is bus
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

	else
    {
		return QVariant();
    }
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
		parentItem = rootItem_.data();
    }
	else
    {
		parentItem = static_cast<HierarchyItem*>(parent.internalPointer());
    }

	// ask parent if it has children
	return parentItem->hasChildren();
}

//-----------------------------------------------------------------------------
// Function: HierarchyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags HierarchyModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	if (index.column() == HierarchyModel::INSTANCE_COLUMN)
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
	
	// if item is not found
	if (!item)
    {
		return;
	}

	// item must always be design
	Q_ASSERT(item->type() == HierarchyItem::HW_DESIGN);
	VLNV designVLNV = item->getVLNV();

	// find the containing component
	HierarchyItem* parent = item->parent();
	// if the design has no parent or the parent is the root item (which is not component)
	if (!parent || parent == rootItem_.data())
    {
		emit errorMessage(tr("Design did not have containing component and could not be opened."));
		return;
	}

	Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

	// find the vlnv of the component
	VLNV compVLNV = parent->getVLNV();
	Q_ASSERT(compVLNV.getType() == handler_->getDocumentType(compVLNV));

	emit openDesign(compVLNV, item->getViewName());
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

    // if item is not found
    if (!item)
    {
        return;
    }

    // item must always be design
    Q_ASSERT(item->type() == HierarchyItem::HW_DESIGN);
    VLNV designVLNV = item->getVLNV();

    // find the containing component
    HierarchyItem* parent = item->parent();
    // if the design has no parent or the parent is the root item (which is not component)
    if (!parent || parent == rootItem_.data())
    {
        emit errorMessage(tr("Design did not have containing component and could not be opened."));
        return;
    }
    Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

    // find the vlnv of the component
    VLNV compVLNV = parent->getVLNV();
    Q_ASSERT(compVLNV.getType() == handler_->getDocumentType(compVLNV));

    emit openMemoryDesign(compVLNV, item->getViewName());
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
	// if item is not found
	if (!item)
    {
		return;
	}

	// item must always be design
	Q_ASSERT(item->type() == HierarchyItem::SW_DESIGN);
	VLNV designVLNV = item->getVLNV();

	// find the containing component
	HierarchyItem* parent = item->parent();
	// if the design has no parent or the parent is the root item (which is not component)
	if (!parent || parent == rootItem_.data())
    {
		emit errorMessage(tr("Design did not have containing component and could not be opened."));
		return;
	}
	Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

	// find the vlnv of the component
	VLNV compVLNV = parent->getVLNV();
	Q_ASSERT(compVLNV.getType() == handler_->getDocumentType(compVLNV));

	emit openSWDesign(compVLNV, item->getViewName());
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
    
	// if item is not found
	if (!item)
    {
		return;
	}

	// item must always be design
	Q_ASSERT(item->type() == HierarchyItem::SYS_DESIGN);
	VLNV designVLNV = item->getVLNV();

	// find the containing component
	HierarchyItem* parent = item->parent();
	// if the design has no parent or the parent is the root item (which is not component)
	if (!parent || parent == rootItem_.data())
    {
		emit errorMessage(tr("Design did not have containing component and could not be opened."));
		return;
	}
	Q_ASSERT(parent->type() == HierarchyItem::COMPONENT);

	// find the vlnv of the component
	VLNV compVLNV = parent->getVLNV();
	Q_ASSERT(compVLNV.getType() == handler_->getDocumentType(compVLNV));

	emit openSystemDesign(compVLNV, item->getViewName());
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

	VLNV vlnv = item->getVLNV();
	if (vlnv.isValid())
    {
		emit editItem(vlnv);
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

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::COMPONENT);
	
	emit createComponent(vlnv);
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

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::COMPONENT);

	emit createDesign(vlnv);
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

    VLNV vlnv = item->getVLNV();
    vlnv.setType(VLNV::COMPONENT);

    emit createSWDesign(vlnv);
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

    VLNV vlnv = item->getVLNV();
    vlnv.setType(VLNV::COMPONENT);

    emit createSystemDesign(vlnv);
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

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::BUSDEFINITION);

	emit createBus(vlnv);
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

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::BUSDEFINITION);

	emit createAbsDef(vlnv);
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

    VLNV vlnv = item->getVLNV();
    vlnv.setType(VLNV::COMDEFINITION);

    emit createComDef(vlnv);
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

    VLNV vlnv = item->getVLNV();
    vlnv.setType(VLNV::APIDEFINITION);

    emit createApiDef(vlnv);
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
void HierarchyModel::onComponentSaved(VLNV const&)
{
	onResetModel();
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

