/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "businterfacesmodel.h"
#include <LibraryManager/libraryinterface.h>
#include "businterfacesdelegate.h"
#include <models/generaldeclarations.h>
#include <LibraryManager/vlnv.h>

#include <QMap>
#include <QString>
#include <QColor>

BusInterfacesModel::BusInterfacesModel( LibraryInterface* libHandler, 
									   QSharedPointer<Component> component,
									   QObject *parent ):
QAbstractTableModel(parent),
libHandler_(libHandler),
component_(component),
busifs_(component->getBusInterfaces()) {

	Q_ASSERT(libHandler_);
	Q_ASSERT(component_);
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

	// bus def and abs def can not be edited but can have data dropped on them
	else if (BusInterfacesDelegate::BUSDEF_COLUMN == index.column() ||
		BusInterfacesDelegate::ABSDEF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant BusInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				return tr("Name");
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				return tr("Bus\nDefinition");
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				return tr("Abstraction\ndefinition");
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				return tr("Interface\nmode");
													   }
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
				return tr("Description");
															}
			default: {
				return QVariant();
					 }
		}
	}
	else {
		return QVariant();
	}
}

QVariant BusInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				return busifs_.at(index.row())->getName();
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				return busifs_.at(index.row())->getBusType().toString(":");
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				return busifs_.at(index.row())->getAbstractionType().toString(":");
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				General::InterfaceMode mode = busifs_.at(index.row())->getInterfaceMode();
				return General::interfaceMode2Str(mode);
													   }
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
				return busifs_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		// list of component's physical ports and their bounds is needed for
		// bus interface to check the port maps
		QList<General::PortBounds> physPorts = component_->getPortBounds();

		if (busifs_.at(index.row())->isValid(physPorts)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN:
			case BusInterfacesDelegate::BUSDEF_COLUMN:
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				return QColor("LemonChiffon");
								 }
			default:
				return QColor("white");
		}
	}
	else {
		return QVariant();
	}
}

bool BusInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				busifs_[index.row()]->setName(value.toString());
				break;
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				VLNV busType = VLNV(VLNV::BUSDEFINITION, value.toString(), ":");
				busifs_[index.row()]->setBusType(busType);
				break;
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				VLNV absType = VLNV(VLNV::ABSTRACTIONDEFINITION, value.toString(), ":");
				busifs_[index.row()]->setAbstractionType(absType);
				break;
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				General::InterfaceMode mode = static_cast<General::InterfaceMode>(value.toInt());
				busifs_[index.row()]->setInterfaceMode(mode);
				break;
														}
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
				busifs_[index.row()]->setDescription(value.toString());
				break;
													   }
			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else {
		return false;
	}
}

void BusInterfacesModel::onAddItem( const QModelIndex& index ) {
	int row = busifs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	busifs_.insert(row, QSharedPointer<BusInterface>(new BusInterface()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit busifAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void BusInterfacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	busifs_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit busifRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

bool BusInterfacesModel::isValid() const {
	// list of component's physical ports and their bounds is needed for
	// bus interface to check the port maps
	QList<General::PortBounds> physPorts = component_->getPortBounds();

	// if at least one address space is invalid
	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		if (!busif->isValid(physPorts)) {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}
