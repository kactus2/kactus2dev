/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacesmodel.cpp
 *		Project: Kactus 2
 */

#include "addressspacesmodel.h"
#include "addressspacesdelegate.h"

#include <QColor>

AddressSpacesModel::AddressSpacesModel( QSharedPointer<Component> component, 
									   QObject *parent ):
QAbstractTableModel(parent),
component_(component),
addrSpaces_(component->getAddressSpaces()) {

	Q_ASSERT(component);
}

AddressSpacesModel::~AddressSpacesModel() {
}

int AddressSpacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return addrSpaces_.size();
}

int AddressSpacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return AddressSpacesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags AddressSpacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	// interface references are made in bus interface editor
	if (index.column() == AddressSpacesDelegate::INTERFACE_COLUMN) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant AddressSpacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case AddressSpacesDelegate::NAME_COLUMN: {
				return tr("Name");
					}
			case AddressSpacesDelegate::ADDR_UNIT_COLUMN: {
				return tr("Addressable\nunit bits (AUB)");
					}
			case AddressSpacesDelegate::RANGE_COLUMN: {
				return tr("Range\n[AUB]");
													  }
			case AddressSpacesDelegate::WIDTH_COLUMN: {
				return tr("Width\n [bits]");
					}
			case AddressSpacesDelegate::INTERFACE_COLUMN: {
				return tr("Master interface\nbinding");
													   }
			case AddressSpacesDelegate::DESCRIPTION_COLUMN: {
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

QVariant AddressSpacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= addrSpaces_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case AddressSpacesDelegate::NAME_COLUMN: {
				return addrSpaces_.at(index.row())->getName();
													 }
			case AddressSpacesDelegate::ADDR_UNIT_COLUMN: {
				return addrSpaces_.at(index.row())->getAddressUnitBits();
														  }
			case AddressSpacesDelegate::WIDTH_COLUMN: {
				return addrSpaces_.at(index.row())->getWidth();
													  }
			case AddressSpacesDelegate::RANGE_COLUMN: {
				return addrSpaces_.at(index.row())->getRange();
													  }
			case AddressSpacesDelegate::INTERFACE_COLUMN: {
				QStringList interfaceNames = component_->getMasterInterfaces(
					addrSpaces_.at(index.row())->getName());

				// if no interface refers to the memory map
				if (interfaceNames.isEmpty()) {
					return tr("No binding");
				}
				// if there are then show them separated by space
				else {
					return interfaceNames.join(" ");
				}
													   }
			case AddressSpacesDelegate::DESCRIPTION_COLUMN: {
				return addrSpaces_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	if (AddressSpacesDelegate::USER_DISPLAY_ROLE == role) {
		return component_->getMasterInterfaces(addrSpaces_.at(index.row())->getName());
	}
	else if (Qt::ForegroundRole == role) {
		if (addrSpaces_.at(index.row())->isValid(component_->getChoices())) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case AddressSpacesDelegate::NAME_COLUMN:
			case AddressSpacesDelegate::ADDR_UNIT_COLUMN:
			case AddressSpacesDelegate::WIDTH_COLUMN: 
			case AddressSpacesDelegate::RANGE_COLUMN: {
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

bool AddressSpacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= addrSpaces_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case AddressSpacesDelegate::NAME_COLUMN: {
				addrSpaces_[index.row()]->setName(value.toString());
				break;
													 }
			case AddressSpacesDelegate::ADDR_UNIT_COLUMN: {
				addrSpaces_[index.row()]->setAddressUnitBits(value.toUInt());
				break;
														  }
			case AddressSpacesDelegate::WIDTH_COLUMN: {
				addrSpaces_[index.row()]->setWidth(value.toUInt());
				break;
													  }
			case AddressSpacesDelegate::RANGE_COLUMN: {
				addrSpaces_[index.row()]->setRange(value.toString());
				break;
													  }
			case AddressSpacesDelegate::DESCRIPTION_COLUMN: {
				addrSpaces_[index.row()]->setDescription(value.toString());
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

void AddressSpacesModel::onAddItem( const QModelIndex& index ) {
	int row = addrSpaces_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	addrSpaces_.insert(row, QSharedPointer<AddressSpace>(new AddressSpace()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit addrSpaceAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void AddressSpacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= addrSpaces_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	addrSpaces_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit addrSpaceRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

bool AddressSpacesModel::isValid() const
{
	// if at least one address space is invalid
	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) 
    {
		if (!addrSpace->isValid(component_->getChoices()))
        {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}
