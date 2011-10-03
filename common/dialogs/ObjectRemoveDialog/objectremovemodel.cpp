/* 
 *  	Created on: 8.8.2011
 *      Author: Antti Kamppi
 * 		filename: objectremovemodel.cpp
 *		Project: Kactus 2
 */

#include "objectremovemodel.h"

ObjectRemoveModel::ObjectRemoveModel(QObject *parent): 
QAbstractTableModel(parent),
items_() {
}

ObjectRemoveModel::~ObjectRemoveModel() {
}

int ObjectRemoveModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return items_.size();
}

int ObjectRemoveModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return 2;
}

QVariant ObjectRemoveModel::data( const QModelIndex& index, 
											   int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid())
		return QVariant();

	// if row is invalid
	if (index.row() < 0 || index.row() >= items_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: 
				// if object is vlnv then display the vlnv fields.
				if (items_.at(index.row()).type_ == ObjectRemoveModel::VLNVOBJECT) {
					QString name(items_.at(index.row()).vlnv_.getVendor());
					name += QString(":");
					name += items_.at(index.row()).vlnv_.getLibrary();
					name += QString(":");
					name += items_.at(index.row()).vlnv_.getName();
					name += QString(":");
					name += items_.at(index.row()).vlnv_.getVersion();
					return name;
				}
				// if object is file then return it's path
				else {
					return items_.at(index.row()).path_;
				}
			case 1:
				if (items_.at(index.row()).type_ == ObjectRemoveModel::VLNVOBJECT)
					return tr("VLNV object");
				else
					return tr("File");
			default:
				return QVariant();
		}
	}
	// only first column is checkable
	else if (role == Qt::CheckStateRole && index.column() == 0) {
		if (items_.at(index.row()).checked_)
			return Qt::Checked;
		else
			return Qt::Unchecked;
	}

	// if tool tip is requested
	else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {

		// if item is vlnv object
		if (items_.at(index.row()).type_ == ObjectRemoveModel::VLNVOBJECT)
			return tr("Removes the library object from the library and the xml-file"
			" from file system");
		
		// if item is file in file system
		else
			return tr("Removes the file from the file system");
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant ObjectRemoveModel::headerData( int section,
													 Qt::Orientation orientation,
													 int role /*= Qt::DisplayRole*/ ) const {
	// only horizontal headers are supported
	if (orientation != Qt::Horizontal) 
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (section) {
			case 0:
				return tr("Name");
			case 1:
				return tr("Type");
			default:
				return QVariant();
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

void ObjectRemoveModel::addItem( const Item& item ) {
	beginInsertRows(QModelIndex(), items_.size(), items_.size());
	items_.append(item);
	endInsertRows();
}

void ObjectRemoveModel::createItem( const VLNV& vlnv, bool locked) {
	ObjectRemoveModel::Item item(vlnv);
	item.locked_ = locked;
	
	if (!items_.contains(item)) {
		beginInsertRows(QModelIndex(), items_.size(), items_.size());
		items_.append(item);
		endInsertRows();
	}
	
}

void ObjectRemoveModel::createItem( const QString& filePath ) {
	ObjectRemoveModel::Item item(filePath);

	if (!items_.contains(item)) {
		beginInsertRows(QModelIndex(), items_.size(), items_.size());
		items_.append(item);
		endInsertRows();
	}
}

Qt::ItemFlags ObjectRemoveModel::flags( const QModelIndex& index ) const {
	if (!index.isValid())
		return Qt::NoItemFlags;
	else if (index.row() < 0 || index.row() >= items_.size())
		return Qt::NoItemFlags;
	
	// if item is locked
	else if (items_.at(index.row()).locked_)
		return Qt::NoItemFlags;

	// if column is 0 and item is not locked
	else if (index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

	// other columns can be selected but not checked.
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ObjectRemoveModel::setData( const QModelIndex& index, const QVariant& value,
								int role /*= Qt::EditRole*/ ) {
	if (!index.isValid())
		return false;

	// if row is invalid
	if (index.row() < 0 || index.row() >= items_.size())
		return false;

	if (role == Qt::CheckStateRole) {
		if (value.toInt() == Qt::Checked) {
			items_[index.row()].checked_ = true;
			emit dataChanged(index, index);
			return true;
		}
		else {
			items_[index.row()].checked_ = false;
			emit dataChanged(index, index);
			return true;
		}
	}
	else
		return false;
}

QList<ObjectRemoveModel::Item> ObjectRemoveModel::getItemsToRemove() const {

	QList<ObjectRemoveModel::Item> removable;
	foreach (ObjectRemoveModel::Item item, items_) {
		if (item.checked_)
			removable.append(item);
	}	
	return removable;
}

void ObjectRemoveModel::onSelectionChange( int selectionState ) {
	// if all items are to be selected
	if (selectionState == Qt::Checked) {
		for (int i = 0; i < items_.size(); ++i) {
			items_[i].checked_ = true;
			QModelIndex index = QAbstractTableModel::index(i, 0, QModelIndex());
			emit dataChanged(index, index);
		}
	}
	// if all are to be unselected
	else {
		for (int i = 0; i < items_.size(); ++i) {
			// if item is not locked
			if (!items_[i].locked_) {
				items_[i].checked_ = false;
				QModelIndex index = QAbstractTableModel::index(i, 0, QModelIndex());
				emit dataChanged(index, index);
			}
		}
	}
}

ObjectRemoveModel::Item::Item( const VLNV& vlnv ):
type_(ObjectRemoveModel::VLNVOBJECT),
vlnv_(vlnv),
path_(),
checked_(true),
locked_(false) {
}

ObjectRemoveModel::Item::Item( const QString& path ):
type_(ObjectRemoveModel::FILE),
vlnv_(),
path_(path),
checked_(true),
locked_(false) {
}

bool ObjectRemoveModel::Item::operator==( const Item& other ) const {
	// if both have vlnv defined and they are the same
	if (type_ == ObjectRemoveModel::VLNVOBJECT && 
		other.type_ == ObjectRemoveModel::VLNVOBJECT &&
		vlnv_ == other.vlnv_)
		return true;

	// if both have path defined and they are the same
	else if (type_ == ObjectRemoveModel::FILE &&
		other.type_ == ObjectRemoveModel::FILE &&
		path_ == other.path_)
		return true;
	else
		return false;
}
