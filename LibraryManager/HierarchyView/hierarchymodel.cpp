/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchymodel.cpp
 *		Project: Kactus 2
 */

#include "hierarchymodel.h"

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/libraryhandler.h>
#include <LibraryManager/librarydata.h>

#include <common/KactusAttribute.h>

#include <QColor>

HierarchyModel::HierarchyModel(LibraryData* sourceModel, 
							   LibraryInterface* handler,
							   QObject *parent):
QAbstractItemModel(parent),
dataModel_(sourceModel),
rootItem_(),
handler_(handler) {

	// connect the signals that are needed to print messages
	LibraryHandler* handler2 = static_cast<LibraryHandler*>(handler);
	connect(this, SIGNAL(errorMessage(const QString&)),
		handler2, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		handler2, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	onResetModel();
}

HierarchyModel::~HierarchyModel() {
}

void HierarchyModel::onResetModel() {
	
	beginResetModel();

	// create the root item if it does not yet exist
	if (!rootItem_) {
		rootItem_ = QSharedPointer<HierarchyItem>(new HierarchyItem(handler_, this));
		connect(rootItem_.data(), SIGNAL(errorMessage(const QString&)),
			this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(rootItem_.data(), SIGNAL(noticeMessage(const QString&)),
			this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	}
	// if root exists then tell it to remove children
	else
		rootItem_->clear();
	
	// get the items to be displayed from the data source
	QList<VLNV*> items = dataModel_->getItems();

	QList<VLNV*> absDefs;

	// add all items to this model
	for (int i = 0; i < items.size(); ++i) {

		// make sure the item can be parsed
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(*items.at(i));
		if (!libComp)
			continue;

		// if item is component or bus definition
		if (items.at(i) && (handler_->getDocumentType(*items.at(i)) == VLNV::COMPONENT ||
			handler_->getDocumentType(*items.at(i)) == VLNV::BUSDEFINITION)) {
			
			rootItem_->createChild(items.at(i));
		}

		// if item is abstraction definition append the abstraction definition to the list
		else if (handler_->getDocumentType(*items.at(i)) == VLNV::ABSTRACTIONDEFINITION) {
			absDefs.append(items.at(i));
			rootItem_->createChild(items.at(i));
		}
	}

	// create the abstraction definitions
	for (int i = 0; i < absDefs.size(); ++i) {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(*absDefs.at(i));
		QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

		VLNV busDefVlnv = absDef->getBusType();

		QList<HierarchyItem*> busDefItems = rootItem_->findItems(busDefVlnv);
		for (int j = 0; j < busDefItems.size(); ++j) {
			busDefItems.at(j)->createChild(absDefs.at(i));
		}
	}

	// remove the duplicates
	rootItem_->cleanUp();

	endResetModel();
}

QVariant HierarchyModel::headerData(int section, 
									Qt::Orientation orientation, 
									int role /*= Qt::DisplayRole */ ) const {
	// only one column in tree view
	if (section != 0) {
		return QVariant();
	}
	// only horizontal headers
	else if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	// for displayRole
	else if (role == Qt::DisplayRole) {
		return tr("Library components");
	}
	// not supported role
	else {
		return QVariant();
	}
}

int HierarchyModel::columnCount(const QModelIndex&) const {
	return 1;
}

int HierarchyModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const {
	// only one column exists
	if (parent.column() > 0) {
		return 0;
	}

	HierarchyItem* parentItem = 0;

	// if the given item is invalid then it is the root
	if (!parent.isValid())
		parentItem = rootItem_.data();
	else
		parentItem = static_cast<HierarchyItem*>(parent.internalPointer());

	return parentItem->getNumberOfChildren();
}

QModelIndex HierarchyModel::index(int row, 
								  int column,
								  const QModelIndex &parent /*= QModelIndex()*/ ) const {
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	HierarchyItem* parentItem = 0;

	// if the index of the parent is invalid then it is the root
	if (!parent.isValid())
		parentItem = rootItem_.data();
	else
		parentItem = static_cast<HierarchyItem*>(parent.internalPointer());

	HierarchyItem* child = parentItem->child(row);

	// if child is found
	if (child) {
		return createIndex(row, column, child);
	}
	else
		return QModelIndex();
}

QModelIndex HierarchyModel::index(HierarchyItem* item) const {
	if (!item)
		return QModelIndex();

	// if this item has no parent then it is the root item
	HierarchyItem* parent = item->parent();
	if (!parent)
		return QModelIndex();

	// create the index for the child
	return createIndex(item->row(), 0, item);
}

QModelIndexList HierarchyModel::findIndexes( const VLNV& vlnv ) {
	QList<HierarchyItem*> items = rootItem_->findItems(vlnv);

	QModelIndexList list;

	// get model index of each hierarchy item
	foreach (HierarchyItem* item, items) {
		list.append(index(item));
	}
	return list;
}

QModelIndex HierarchyModel::parent(const QModelIndex& child) const {
	if (!child.isValid())
		return QModelIndex();

	// pointer to the child item
	HierarchyItem* childItem = static_cast<HierarchyItem*>(child.internalPointer());
	HierarchyItem* parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent)
		return QModelIndex();

	// if row is invalid then the grandparent does not exist and the parent is
	// a root item so we return an invalid QModelIndex
	int row = parent->row();
	if (row == -1) {
		return QModelIndex();
	}

	// create new index and return it
	return createIndex(row, 0, parent);
}

QVariant HierarchyModel::data(const QModelIndex& index, 
							  int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid())
		return QVariant();

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	if (role == Qt::DisplayRole) {
		VLNV vlnv = item->getVLNV();
		if (vlnv.isValid()) {
			return QString("\n%1:%2\n"
				"%3:%4").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion());
		}

		// root item has no vlnv and no data to display
		else
			return QVariant();
	}
	else if (role == Qt::ForegroundRole) {
		
		// valid items are displayed in black color
		if (item->isValid()) 
			return QColor("black");

		// invalid items are displayed in red color
		else
			return QColor("red");
	}
	else if (role == Qt::ToolTipRole) {
		return QString("File path: %1").arg(dataModel_->getPath(item->getVLNV()));
	}
	else if (role == Qt::DecorationRole) {
		// if item is component
		if (item->type() == HierarchyItem::COMPONENT) {
			
			switch (item->getImplementation()) {
				case KactusAttribute::KTS_SYS: {
					return QIcon(":/icons/graphics/new-system.png");
											   }
				case KactusAttribute::KTS_SW: {
					KactusAttribute::SWType swType = item->getSoftwareType();

					if (swType == KactusAttribute::KTS_SW_APPLICATION)
						return QIcon(":/icons/graphics/new-sw_component.png");
					else if (swType == KactusAttribute::KTS_SW_PLATFORM) 
						return QIcon(":icons/graphics/API.png");
					else if (swType == KactusAttribute::KTS_SW_ENDPOINTS)
						return QIcon(":/icons/graphics/endpoints.png");
					else if (swType == KactusAttribute::KTS_SW_MAPPING) 
						return QIcon(":/icons/graphics/new-sw_design.png");
											  }
				default: {
					if (item->isHierarchical()) {
						return QIcon(":/icons/graphics/hierarchy.png");
					}
					else
						return QIcon(":/icons/graphics/component.png");
						 }
			}
			
		}
		// if item is bus
		else 
			return QIcon(":/icons/graphics/new-bus.png");
	}

	// unsupported role
	else
		return QVariant();
}

bool HierarchyModel::hasChildren(const QModelIndex& parent /*= QModelIndex()*/ ) const {
	
	// only one column exists
	if (parent.column() > 0)
		return false;

	HierarchyItem* parentItem = 0;

	// if given item is invalid then it is root item
	if (!parent.isValid())
		parentItem = rootItem_.data();
	else
		parentItem = static_cast<HierarchyItem*>(parent.internalPointer());

	// ask parent if it has children
	return parentItem->hasChildren();
}

Qt::ItemFlags HierarchyModel::flags(const QModelIndex& index) const {
	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void HierarchyModel::onOpenDesign( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
	VLNV vlnv = item->getVLNV();

	if (vlnv.isValid())
		emit openDesign(vlnv);
}

void HierarchyModel::onOpenComponent( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();

	if (vlnv.isValid())
		emit editItem(vlnv);
}

void HierarchyModel::onCreateNewComponent( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::COMPONENT);
	
	emit createComponent(vlnv);
}

void HierarchyModel::onCreateNewDesign( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::COMPONENT);

	emit createDesign(vlnv);
}

void HierarchyModel::onCreateNewBus( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::BUSDEFINITION);

	emit createBus(vlnv);
}

void HierarchyModel::onCreateNewAbsDef( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();
	vlnv.setType(VLNV::BUSDEFINITION);

	emit createAbsDef(vlnv);
}

void HierarchyModel::onExportItem( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	// get pointer to the item that was selected
	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());

	VLNV vlnv = item->getVLNV();

	emit exportItem(vlnv);
}

void HierarchyModel::onRemoveVLNV( VLNV* vlnv ) {
	if (!vlnv)
		return;
	else if (!vlnv->isValid())
		return;

	beginResetModel();
	rootItem_->removeItems(*vlnv);
	endResetModel();
}

int HierarchyModel::referenceCount( const VLNV& vlnv ) const {
	return rootItem_->referenceCount(vlnv);
}

void HierarchyModel::onComponentSaved( const VLNV&) {
	onResetModel();
}

int HierarchyModel::getOwners( QList<VLNV>& list, const VLNV& vlnvToSearch ) const {
	rootItem_->getOwners(list, vlnvToSearch);
	return list.size();
}

void HierarchyModel::getChildren( QList<VLNV>& childList, const VLNV& owner ) {
	rootItem_->getChildren(childList, owner);
}

