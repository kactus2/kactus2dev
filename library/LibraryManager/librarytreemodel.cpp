//-----------------------------------------------------------------------------
// File: librarytreemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The model that contains the LibraryItems to display library hierarchically.
//-----------------------------------------------------------------------------

#include "librarytreemodel.h"
#include "librarydata.h"
#include "libraryitem.h"
#include "libraryinterface.h"

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QSharedPointer>
#include <QBrush>
#include <QColor>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::LibraryTreeModel()
//-----------------------------------------------------------------------------
LibraryTreeModel::LibraryTreeModel(LibraryInterface* handler, LibraryData* sourceModel, QObject* parent) :
QAbstractItemModel(parent),
    dataSource_(sourceModel),
    rootItem_(),
    handler_(handler)
{
    onResetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::~LibraryTreeModel()
//-----------------------------------------------------------------------------
LibraryTreeModel::~LibraryTreeModel()
{
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::headerData()
//-----------------------------------------------------------------------------
QVariant LibraryTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
		return tr("Library items");
	}
	else 
    {
		return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::columnCount()
//-----------------------------------------------------------------------------
int LibraryTreeModel::columnCount(QModelIndex const&) const 
{
	return 1;
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::rowCount()
//-----------------------------------------------------------------------------
int LibraryTreeModel::rowCount(QModelIndex const& parent) const
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
		parentItem = rootItem_.data();
	}
	else
    {
		parentItem = static_cast<LibraryItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->getNumberOfChildren();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::index()
//-----------------------------------------------------------------------------
QModelIndex LibraryTreeModel::index(int row, int column, QModelIndex const& parent) const
{
	if (!hasIndex(row, column, parent))
    {
		return QModelIndex();
	}

	LibraryItem* parentItem = 0;

	// if the index of the parent is invalid then it is the root item
	if (!parent.isValid()) 
    {
		parentItem = rootItem_.data();
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
// Function: LibraryTreeModel::index()
//-----------------------------------------------------------------------------
QModelIndex LibraryTreeModel::index(LibraryItem* item) 
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
// Function: LibraryTreeModel::parent()
//-----------------------------------------------------------------------------
QModelIndex LibraryTreeModel::parent(QModelIndex const& child) const
{
	// if invalid index
	if(!child.isValid())
    {
		return QModelIndex();
	}

	// pointer to the child item
	LibraryItem *childItem = static_cast<LibraryItem*>(child.internalPointer());
    LibraryItem *parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent)
    {
		return QModelIndex();
	}

	// if row is invalid then the grandparent does not exist and parent is
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
// Function: LibraryTreeModel::data()
//-----------------------------------------------------------------------------
QVariant LibraryTreeModel::data(QModelIndex const& index, int role) const
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

	else if (role == Qt::ForegroundRole)
    {
		// check all vlnvs that the item represents
		QList<VLNV> vlnvs;
		item->getVLNVs(vlnvs);

		foreach (VLNV const& vlnv, vlnvs)
        {
			// if at least one vlnv is valid then the item is valid
			if (handler_->isValid(vlnv))
            {
				return QBrush(QColor("black"));
			}
		}

		// if the document is not valid then it is marked with red color
		return QBrush(QColor("red"));		
	}

	else if (role == Qt::ToolTipRole)
    {
		VLNV vlnv = item->getVLNV();

		// if item can identify a single library object
		if (vlnv.isValid())
        {
			QString text = "<b>Vendor:</b> " + vlnv.getVendor() + "<br>" +
				"<b>Library:</b> " + vlnv.getLibrary() + "<br>" + 
                "<b>Name:</b> " + vlnv.getName() + "<br>" +
				"<b>Version:</b> " + vlnv.getVersion() + "<br>";

			QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);

			if (document != 0 && !document->getDescription().isEmpty())
            {
				text += "<br><b>Description:</b><br>" + document->getDescription();
			}

			text += "<br><b>File Path:</b><br>" + dataSource_->getPath(vlnv);
			return text;
		}

		// if item did not represent a single vlnv item
		return QVariant();
	}
	
    else if (role == Qt::DecorationRole)
    {		
		VLNV vlnv = item->getVLNV();
		
		// if item represents a single vlnv object
		if (vlnv.isValid())
        {
            VLNV::IPXactType documentType = vlnv.getType();
			if (documentType == VLNV::COMPONENT)
            {
				QSharedPointer<Component const> component = 
                    handler_->getModelReadOnly(vlnv).staticCast<Component const>();

                if (component != 0)
                {
                    if (component->getImplementation() == KactusAttribute::SYSTEM)
                    {
                        return QIcon(":/icons/common/graphics/system-component.png");
                    }
                    else if (component->getImplementation() == KactusAttribute::SW)
                    {
                        if (component->hasSWViews())
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
                        if (component->isHierarchical())
                        {
                            return QIcon(":/icons/common/graphics/hier-hw-component.png");
                        }
                        else
                        {
                            return QIcon(":/icons/common/graphics/hw-component.png");
                        }
                    }
                }

                return QIcon(":/icons/common/graphics/hw-component.png");
			}

			else if (documentType == VLNV::BUSDEFINITION || documentType == VLNV::ABSTRACTIONDEFINITION)
            {
                return QIcon(":/icons/common/graphics/new-bus.png");
            }

            else if (documentType == VLNV::COMDEFINITION)
            {
                return QIcon(":/icons/common/graphics/new-com_definition.png");
            }

            else if (documentType == VLNV::APIDEFINITION)
            {
                return QIcon(":/icons/common/graphics/new-api_definition.png");
            }

            else if (documentType == VLNV::DESIGN)
            {
                // Determine the design type.
                QSharedPointer<const Design> design = handler_->getModelReadOnly(vlnv).staticCast<const Design>();
                if (design && design->getImplementation() == KactusAttribute::SW)
                {
                    return QIcon(":/icons/common/graphics/sw-design24x24.png");
                }
                else
                {               
                    return QIcon(":/icons/common/graphics/hw-design.png");
                }
            }

			else if (documentType == VLNV::DESIGNCONFIGURATION)
            {
				return QIcon(":/icons/common/graphics/configuration.png");
			}
			
			else
            {
				return QVariant();
            }
		}

		else 
        {
			return QVariant();	
        }
	}
	
	// unsupported data role
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LibraryTreeModel::flags(QModelIndex const& index) const
{
	// if given index is invalid
	if (!index.isValid())
    {
		return Qt::ItemIsEnabled;
	}

	return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::hasChildren()
//-----------------------------------------------------------------------------
bool LibraryTreeModel::hasChildren(QModelIndex const& parent) const
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
		parentItem = rootItem_.data();
	}
	else
    {
		parentItem = static_cast<LibraryItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->hasChildren();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onExportItem()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onExportItem(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	// ask the item for all the VLNVs it represents
	QList<VLNV> vlnvList;
	item->getVLNVs(vlnvList);

	emit exportItems(vlnvList);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onDeleteItem()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onDeleteItem(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	LibraryItem* child = static_cast<LibraryItem*>(index.internalPointer());
	LibraryItem* toRemove = child->findHighestUnique();

	// ask the item for all the VLNVs it represents
	QList<VLNV> vlnvList;
	toRemove->getVLNVs(vlnvList);

	// inform the library handler that these VLNVs should be removed
	emit removeVLNV(vlnvList);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onRemoveVLNV(VLNV const& vlnv)
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
// Function: LibraryTreeModel::onAddVLNV()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onAddVLNV(VLNV const& vlnv)
{
	if (!vlnv.isValid())
    {
		return;
	}

	LibraryItem* parentItem = rootItem_->findHighestUnique(vlnv);

	QModelIndex parentIndex;

	// if the highest unique is not the root
	if (parentItem != rootItem_.data())
    {
		int row = parentItem->row();
		Q_ASSERT(row >= 0);

		parentIndex = createIndex(row, 0, parentItem);
	}
	
	int rowToAdd = parentItem->getNumberOfChildren();

	beginInsertRows(parentIndex, rowToAdd, rowToAdd);
	rootItem_->createChild(vlnv, static_cast<LibraryItem::Level>(0));
	endInsertRows();

	emit refreshDialer();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::removeLibraryItem()
//-----------------------------------------------------------------------------
void LibraryTreeModel::removeLibraryItem(LibraryItem* toRemove)
{
	int row = toRemove->parent()->getIndexOf(toRemove);

	// ask the item for all the VLNVs it represents
	QList<VLNV> vlnvList;
	toRemove->getVLNVs(vlnvList);

	QModelIndex removeIndex = createIndex(row, 0, toRemove);

	// get the model index of the parent
	QModelIndex parentIndex = parent(removeIndex);

 	beginRemoveRows(parentIndex, row, row);
 	toRemove->parent()->removeChild(toRemove);
 	endRemoveRows();

	emit refreshDialer();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onResetModel()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onResetModel()
{	
	beginResetModel();

	if (!rootItem_)
    {
		rootItem_ = QSharedPointer<LibraryItem>(new LibraryItem(tr("root"), this));
	}
	else
    {
		rootItem_->clear();
	}

	// get the items to be displayed from the data source
	QList<VLNV> items = dataSource_->getItems();

    int itemCount = items.count();
	for (int i = 0; i < itemCount; i++)
    {
		rootItem_->createChild(items.at(i), LibraryItem::ROOT);
	}

	endResetModel();

	emit refreshDialer();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::getRoot()
//-----------------------------------------------------------------------------
LibraryItem* LibraryTreeModel::getRoot() const
{
	return rootItem_.data();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onCreateBus()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateBus(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::BUSDEFINITION);
	item->setVlnv(vlnv);

	emit createBus(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onCreateAbsDef()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateAbsDef(QModelIndex const& index)
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
// Function: LibraryTreeModel::onCreateComDef()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateComDef(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMDEFINITION);
    item->setVlnv(vlnv);

    emit createComDef(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onCreateApiDef()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateApiDef(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::APIDEFINITION);
    item->setVlnv(vlnv);

    emit createApiDef(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onOpenDocument()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onOpenDocument(QModelIndex const& index)
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
// Function: LibraryTreeModel::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onOpenDesign(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
	VLNV vlnv = item->getVLNV();

	if (vlnv.isValid())
    {
		emit openDesign(vlnv);
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onOpenSWDesign(QModelIndex const& index)
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
// Function: LibraryTreeModel::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onOpenSystemDesign(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

	if (vlnv.isValid())
    {
        emit openSystemDesign(vlnv);
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onCreateNewComponent()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateNewComponent(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::COMPONENT);
	item->setVlnv(vlnv);

	emit createComponent(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeModel::onCreateNewDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateNewDesign(QModelIndex const& index)
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
// Function: LibraryTreeModel::onCreateNewSWDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateNewSWDesign(QModelIndex const& index)
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
// Function: LibraryTreeModel::onCreateNewSystemDesign()
//-----------------------------------------------------------------------------
void LibraryTreeModel::onCreateNewSystemDesign(QModelIndex const& index)
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
void LibraryTreeModel::onShowErrors(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    emit showErrors(item->getVLNV());
}
