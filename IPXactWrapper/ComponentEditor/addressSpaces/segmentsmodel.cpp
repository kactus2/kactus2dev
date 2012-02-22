/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmentsmodel.cpp
 *		Project: Kactus 2
 */

#include "segmentsmodel.h"

#include <QColor>

SegmentsModel::SegmentsModel(AddressSpace* addrSpace, QObject *parent):
QAbstractTableModel(parent),
segments_(&addrSpace->getSegments()),
table_() {

	Q_ASSERT(addrSpace);

	restore();
}

SegmentsModel::~SegmentsModel() {
}

int SegmentsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return table_.size();
}

int SegmentsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return 4;
}

QVariant SegmentsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: {
				return table_.at(index.row())->getName();
					}
			case 1: {
				return table_.at(index.row())->getAddressOffset();
					}
			case 2: {
				return table_.at(index.row())->getRange();
					}
			case 3: {
				return table_.at(index.row())->getDescription();
					}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 0:
			case 1: 
			case 2: {
				return QColor("LemonChiffon");
					}
			default:
				return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (table_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant SegmentsModel::headerData( int section, 
								   Qt::Orientation orientation, 
								   int role /*= Qt::DisplayRole*/ ) const {

	if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return tr("Name");
			case 1:
				return tr("Offset");
			case 2:
				return tr("Range");
			case 3:
				return tr("Description");
			default:
				return QVariant();
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

bool SegmentsModel::setData( const QModelIndex& index, 
							const QVariant& value, int role /*= Qt::EditRole*/ ) {

	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {

		switch (index.column()) {
			case 0: {
				table_.at(index.row())->setName(value.toString());
				break;
					}
			case 1: {
				table_.at(index.row())->setOffset(value.toString());
				break;
					}
			case 2: {
				table_.at(index.row())->setRange(value.toString());
				break;
					}
			case 3: {
				table_.at(index.row())->setDescription(value.toString());
				break;
					}
			default: 
				return false;
		}
		emit contentChanged();
		return true;
	}

	// is unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags SegmentsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool SegmentsModel::isValid() const {
	
	// check all segments
	foreach (QSharedPointer<Segment> segment, table_) {
		if (!segment->isValid()) {
			return false;
		}
	}
	return true;
}

void SegmentsModel::apply() {
	// if not valid then don't apply changes
	if (!isValid())
		return;

	*segments_ = table_;
}

void SegmentsModel::restore() {
	beginResetModel();
	table_.clear();
	table_ = *segments_;
	endResetModel();
}

void SegmentsModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QSharedPointer<Segment>(new Segment()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void SegmentsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
