/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarytreemodel.h"
#include "librarydata.h"
#include "libraryitem.h"
#include "libraryinterface.h"

#include <models/librarycomponent.h>
#include <models/component.h>

#include <common/KactusAttribute.h>

#include <QSharedPointer>
#include <QBrush>
#include <QColor>
#include <QIcon>

#include <QDebug>

LibraryTreeModel::LibraryTreeModel(LibraryInterface* handler,
								   LibraryData* sourceModel, 
								   QObject *parent) :
QAbstractItemModel(parent),
dataSource_(sourceModel),
rootItem_(),
handler_(handler) {

	onResetModel();
}

LibraryTreeModel::~LibraryTreeModel() {
}


QVariant LibraryTreeModel::headerData(int section, Qt::Orientation orientation,
		int role) const {

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

int LibraryTreeModel::columnCount(const QModelIndex&) const {
	return 1;
}

int LibraryTreeModel::rowCount(const QModelIndex &parent) const {

	// only one column exists
	if (parent.column() > 0) {
		return 0;
	}

	LibraryItem *parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<LibraryItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->getNumberOfChildren();
}

QModelIndex LibraryTreeModel::index(int row, int column,
		const QModelIndex &parent) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	LibraryItem* parentItem = 0;

	// if the index of the parent is invalid then it is the root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<LibraryItem*>(parent.internalPointer());
	}

	if (!parentItem) {
		return QModelIndex();
	}

	// find the child item of the parent
	LibraryItem *child = parentItem->child(row);

	// if the child is found
	if (child) {
		return createIndex(row, column, child);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex LibraryTreeModel::index(LibraryItem* item ) {

	// if pointer is null
	if (!item)
		return QModelIndex();

	// if this item has no parent then it is the root item
	LibraryItem* parent = item->parent();
	if (!parent)
		return QModelIndex();

	// create the index for the child
	return createIndex(item->row(), 0, item);
}

QModelIndex LibraryTreeModel::parent(const QModelIndex &child) const {

	// if invalid index
	if(!child.isValid()) {
		return QModelIndex();
	}

	// pointer to the child item
	LibraryItem *childItem = static_cast<LibraryItem*>(child.internalPointer());
        LibraryItem *parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent) {
		return QModelIndex();
	}

	// if row is invalid then the grandparent does not exist and parent is
	// a root item so we return an invalid QModelIndex
	int row = parent->row();
	if (row == -1) {
		return QModelIndex();
	}

	// create new index and return it
	return createIndex(row, 0, parent);
}

QVariant LibraryTreeModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	if (role == Qt::DisplayRole) {

		return item->getName();
	}

	else if (role == Qt::ForegroundRole) {
		QColor textColor;

		// if the item is valid it is displayed as black
		if (item->isValid()) {
			textColor = QColor("black");
		}

		// if the document is not valid then it is marked with red color
		else {
			textColor = QColor("red");
		}

		return QBrush(textColor);
	}

	else if (role == Qt::ToolTipRole) {
		
		VLNV* vlnv = item->getVLNV();

        // if item can identify a single library object
		if (vlnv)
        {
            QString text = QString("<b>Vendor:</b> ") + vlnv->getVendor() + "<br>" +
                           QString("<b>Library:</b> ") + vlnv->getLibrary() + "<br>" +
                           QString("<b>Name:</b> ") + vlnv->getName() + "<br>" +
                           QString("<b>Version:</b> ") + vlnv->getVersion() + "<br>";

            QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(*vlnv);
            QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

            if (component != 0 && !component->getDescription().isEmpty())
            {
                text += QString("<br><b>Description:</b><br>") + component->getDescription();
            }

            text += QString("<br><b>File Path:</b><br>%1").arg(dataSource_->getPath(*vlnv));
            return text;
		}

		// if item did not represent a single vlnv item
		return QVariant();
	}
	else if (role == Qt::DecorationRole) {
		
		VLNV* vlnvP = item->getVLNV();
		
		// if item represents a single vlnv object
		if (vlnvP) {

			if (vlnvP->getType() == VLNV::COMPONENT) {

				QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(*vlnvP);
				QSharedPointer<Component const> component = libComp.staticCast<Component const>();

				switch (component->getComponentImplementation()) {
				case KactusAttribute::KTS_SYS: {
					return QIcon(":/icons/graphics/new-system.png");
											   }
				case KactusAttribute::KTS_SW: {
					return QIcon(":/icons/graphics/new-sw_component.png");
											  }
				default: {
					if (component->isHierarchical()) {
						return QIcon(":/icons/graphics/hierarchy.png");
					}
					else
						return QIcon(":/icons/graphics/component.png");
						 }
				}
			}

			// if item is for a bus
			else if (vlnvP->getType() == VLNV::BUSDEFINITION ||
				vlnvP->getType() == VLNV::ABSTRACTIONDEFINITION) {
				return QIcon(":/icons/graphics/new-bus.png");
			}

            // if item is for a COM definition
            else if (vlnvP->getType() == VLNV::COMDEFINITION) {
                    return QIcon(":/icons/graphics/new-com_definition.png");
            }

            // if item is for an API definition
            else if (vlnvP->getType() == VLNV::APIDEFINITION) {
                return QIcon(":/icons/graphics/new-api_definition.png");
            }

			// if item is for a design
			else if (vlnvP->getType() == VLNV::DESIGN) {
				return QIcon(":/icons/graphics/new-design.png");
			}

			// if item is for a design configuration
			else if (vlnvP->getType() == VLNV::DESIGNCONFIGURATION) {
				return QIcon(":/icons/graphics/configuration.png");
			}
			// item has no icon
			else
				return QVariant();
		}
		// if item does not represent a single vlnv then no icon
		else 
			return QVariant();	
	}
	
	// unsupported data role
	else {
		return QVariant();
	}
}

Qt::ItemFlags LibraryTreeModel::flags(const QModelIndex& index) const {

	// if given index is invalid
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

bool LibraryTreeModel::hasChildren(const QModelIndex& parent) const {

	// only one column exists
	if (parent.column() > 0) {
		return false;
	}

	LibraryItem *parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<LibraryItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->hasChildren();
}

void LibraryTreeModel::onExportItem( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	// get pointer to the item that was selected
	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	// ask the item for all the VLNVs it represents
	QList<VLNV*> vlnvList;
	item->getVLNVs(vlnvList);

	emit exportItems(vlnvList);
}

void LibraryTreeModel::onDeleteItem( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	LibraryItem* child = static_cast<LibraryItem*>(index.internalPointer());

	LibraryItem* toRemove = child->findHighestUnique();

	// ask the item for all the VLNVs it represents
	QList<VLNV*> vlnvList;
	toRemove->getVLNVs(vlnvList);

	// inform the library handler that these VLNVs should be removed
	emit removeVLNV(vlnvList);
}

void LibraryTreeModel::onRemoveVLNV(VLNV* vlnv ) {

	if (!vlnv)
		return;

	LibraryItem* toRemove = rootItem_->findHighestUnique(vlnv);
	
	// remove the item and it's sub-items but no need to inform dataModel 
	// because the removal action was started by the dataModel
	if (toRemove)
		removeLibraryItem(toRemove, false);

}

void LibraryTreeModel::onAddVLNV( VLNV* vlnv ) {

	if (!vlnv) {
		return;
	}

	LibraryItem* parentItem = rootItem_->findHighestUnique(vlnv);

	QModelIndex parentIndex;

	// if the highest unique is not the root
	if (parentItem != rootItem_.data()) {

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

void LibraryTreeModel::removeLibraryItem( LibraryItem* toRemove, bool emitSignals) {
	
	int row = toRemove->parent()->getIndexOf(toRemove);

	// ask the item for all the VLNVs it represents
	QList<VLNV*> vlnvList;
	toRemove->getVLNVs(vlnvList);

	QModelIndex removeIndex = createIndex(row, 0, toRemove);

	// get the model index of the parent
	QModelIndex parentIndex = parent(removeIndex);

 	beginRemoveRows(parentIndex, row, row);
 	toRemove->parent()->removeChild(toRemove);
 	endRemoveRows();

	// if signals were to be emitted
	if (emitSignals) {
		// inform the dataModel that these VLNVs should be removed
		emit removeVLNV(vlnvList);
	}

	emit refreshDialer();
}

void LibraryTreeModel::onResetModel() {
	
	beginResetModel();

	// if root item does not yet exist
	if (!rootItem_) {
		// create the root item
		rootItem_ = QSharedPointer<LibraryItem>(new LibraryItem(tr("root"), this));
	}
	// if root item exists then tell it to clear it's children
	else {
		rootItem_->clear();
	}

	// get the items to be displayed from the data source
	QList<VLNV*> items =
		dataSource_->getItems();

	// add all items to this model
	for (int i = 0; i < items.size(); ++i) {
		rootItem_->createChild(items.value(i),
			static_cast<LibraryItem::Level>(0));
	}

	endResetModel();

	emit refreshDialer();
}

LibraryItem* LibraryTreeModel::getRoot() const {
	return rootItem_.data();
}

void LibraryTreeModel::onOpenBus( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV* vlnv = item->getVLNV();

	if (vlnv && vlnv->isValid())
		emit editItem(*vlnv);
}

void LibraryTreeModel::onOpenComDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV* vlnv = item->getVLNV();

    if (vlnv && vlnv->isValid())
        emit editItem(*vlnv);
}

void LibraryTreeModel::onOpenApiDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV* vlnv = item->getVLNV();

    if (vlnv && vlnv->isValid())
        emit editItem(*vlnv);
}

void LibraryTreeModel::onCreateBus( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::BUSDEFINITION);
	item->setVlnv(vlnv);

	emit createBus(vlnv);
}

void LibraryTreeModel::onCreateAbsDef( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::BUSDEFINITION);
	item->setVlnv(vlnv);

	emit createAbsDef(vlnv);
}

void LibraryTreeModel::onCreateComDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMDEFINITION);
    item->setVlnv(vlnv);

    emit createComDef(vlnv);
}

void LibraryTreeModel::onCreateApiDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::APIDEFINITION);
    item->setVlnv(vlnv);

    emit createApiDef(vlnv);
}

void LibraryTreeModel::onOpenComponent( const QModelIndex& index ) {
	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV* vlnv = item->getVLNV();

	if (vlnv && vlnv->isValid())
		emit editItem(*vlnv);
}

void LibraryTreeModel::onOpenDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
	VLNV* vlnv = item->getVLNV();

	if (vlnv && vlnv->isValid())
		emit openDesign(*vlnv);
}

void LibraryTreeModel::onOpenSWDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV* vlnv = item->getVLNV();

    if (vlnv && vlnv->isValid())
        emit openSWDesign(*vlnv);
}

void LibraryTreeModel::onOpenSystemDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV* vlnv = item->getVLNV();

    if (vlnv && vlnv->isValid())
        emit openSystemDesign(*vlnv);
}

void LibraryTreeModel::onCreateNewComponent( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::COMPONENT);

	// tell items to set the fields for the vlnv
	item->setVlnv(vlnv);

	emit createComponent(vlnv);
}

void LibraryTreeModel::onCreateNewDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

	VLNV vlnv;
	vlnv.setType(VLNV::COMPONENT);

	// tell items to set the fields for the vlnv
	item->setVlnv(vlnv);

	emit createDesign(vlnv);
}

void LibraryTreeModel::onCreateNewSWDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMPONENT);

    // tell items to set the fields for the vlnv
    item->setVlnv(vlnv);

    emit createSWDesign(vlnv);
}

void LibraryTreeModel::onCreateNewSystemDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());

    VLNV vlnv;
    vlnv.setType(VLNV::COMPONENT);

    // tell items to set the fields for the vlnv
    item->setVlnv(vlnv);

    emit createSystemDesign(vlnv);
}
