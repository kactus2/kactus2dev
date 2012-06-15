/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "businterfacesmodel.h"
#include <LibraryManager/libraryinterface.h>
#include "businterfacesdelegate.h"

#include <QMap>
#include <QString>

BusInterfacesModel::BusInterfacesModel( LibraryInterface* libHandler, 
									   QSharedPointer<Component> component,
									   QObject *parent ):
QAbstractTableModel(parent),
libHandler_(libHandler),
component_(component),
busifs_() {

	Q_ASSERT(libHandler_);
	Q_ASSERT(component_);

	// add each bus interface to the map
	QMap<QString, QSharedPointer<BusInterface> >& busInterfaces = component_->getBusInterfaces();
	foreach (QSharedPointer<BusInterface> busif, busInterfaces) {
		Q_ASSERT(busif);

		busifs_.append(busif);
	}
}

BusInterfacesModel::~BusInterfacesModel() {
}

int BusInterfacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return busifs_.size();
}

int BusInterfacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return BusInterfacesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags BusInterfacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// bus def and abs def can not be edited
	else if (BusInterfacesDelegate::BUSDEF_COLUMN == index.column() ||
		BusInterfacesDelegate::ABSDEF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant BusInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	return QVariant();
}

QVariant BusInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	return QVariant();
}

bool BusInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	return false;
}

void BusInterfacesModel::onAddItem( const QModelIndex& index ) {

}

void BusInterfacesModel::onRemoveItem( const QModelIndex& index ) {

}

bool BusInterfacesModel::isValid() const {
	return false;
}
