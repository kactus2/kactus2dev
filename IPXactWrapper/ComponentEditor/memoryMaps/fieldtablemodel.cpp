/* 
 *  	Created on: 3.5.2012
 *      Author: Antti Kamppi
 * 		filename: fieldtablemodel.cpp
 *		Project: Kactus 2
 */

#include "fieldtablemodel.h"

FieldTableModel::FieldTableModel(QObject *parent):
QAbstractTableModel(parent) {
}

FieldTableModel::~FieldTableModel() {
}

int FieldTableModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	return 4;
}

int FieldTableModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	return 15;
}

QVariant FieldTableModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	return QVariant();
}

QVariant FieldTableModel::headerData( int section, 
									 Qt::Orientation orientation,
									 int role /*= Qt::DisplayRole*/ ) const {

	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	else if (role != Qt::DisplayRole) {
		return QVariant();
	}

	switch (section) {
		case 0:
			return tr("Name");	
		case 1: 
			return tr("Description");
		case 2: 
			return tr("Offset");
		case 3: 
			return tr("Width");
		case 4: 
			return tr("Volatile");
		case 5: 
			return tr("Access");
		case 6: 
			return tr("Reset\nValue");
		case 7: 
			return tr("Modified\nwrite");
		case 8: 
			return tr("Read\naction");
		case 9: 
			return tr("Testable");
		case 10: 
			return tr("Test\nconstraint");
		case 11: 
			return tr("Write\nas\nread");
		case 12: 
			return tr("Minimum\nvalue");
		case 13: 
			return tr("Maximum\nvalue");
		case 14: 
			return tr("Enumeration\nPair-Name\nValue");
		default: 
			return QVariant();
	}
}
