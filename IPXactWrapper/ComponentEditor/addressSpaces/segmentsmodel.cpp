/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmentsmodel.cpp
 *		Project: Kactus 2
 */

#include "segmentsmodel.h"

#include <QColor>

SegmentsModel::SegmentsModel(QSharedPointer<AddressSpace> addrSpace, 
							 QObject *parent):
QAbstractTableModel(parent),
addrSpace_(addrSpace),
segments_(addrSpace->getSegments()) {

	Q_ASSERT(addrSpace);
}

SegmentsModel::~SegmentsModel() {
}

int SegmentsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return segments_.size();
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
	else if (index.row() < 0 || index.row() >= segments_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: {
				return segments_.at(index.row())->getName();
					}
			case 1: {
				return segments_.at(index.row())->getAddressOffset();
					}
			case 2: {
				return segments_.at(index.row())->getRange();
					}
			case 3: {
				return segments_.at(index.row())->getDescription();
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

		// check that all fields are correctly defined
		if (!segments_.at(index.row())->isValid()) {
			return QColor("red");
		}

		// make sure the segment does not exceed the address space's range
		quint64 segLimit = segments_.at(index.row())->getLastAddress();
		quint64 addrSpaceLimit = addrSpace_->getLastAddress();
		if (addrSpaceLimit < segLimit) {
			return QColor("red");
		}
		
		// everything was fine
		return QColor("black");
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
	else if (index.row() < 0 || index.row() >= segments_.size())
		return false;

	if (role == Qt::EditRole) {

		switch (index.column()) {
			case 0: {
				const QString oldName = segments_.at(index.row())->getName();
				emit segmentRenamed(oldName, value.toString());

				segments_.at(index.row())->setName(value.toString());
				break;
					}
			case 1: {
				segments_.at(index.row())->setOffset(value.toString());
				emit segmentChanged(segments_.at(index.row()));
				break;
					}
			case 2: {
				segments_.at(index.row())->setRange(value.toString());
				emit segmentChanged(segments_.at(index.row()));
				break;
					}
			case 3: {
				segments_.at(index.row())->setDescription(value.toString());
				break;
					}
			default: 
				return false;
		}
		emit dataChanged(index, index);
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
	foreach (QSharedPointer<Segment> segment, segments_) {
		if (!segment->isValid()) {
			return false;
		}
	}
	return true;
}

void SegmentsModel::onAddItem( const QModelIndex& index ) {
	int row = segments_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	// the last segmented address
	quint64 previousEnd = addrSpace_->getLastSegmentedAddress();
	// if this is the first item to add then do not increase address
	if (previousEnd != 0) {
		++previousEnd;
	}

	// convert the address to hexadecimal form
	QString newBase = QString::number(previousEnd, 16);
	newBase = newBase.toUpper();
	newBase.prepend("0x");

	QSharedPointer<Segment> segment(new Segment());
	segment->setOffset(newBase);

	beginInsertRows(QModelIndex(), row, row);
	segments_.insert(row, segment);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();

	emit segmentAdded(segment);
}

void SegmentsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= segments_.size()) {
		return;
	}

	emit segmentRemoved(segments_.at(index.row())->getName());

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	segments_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
