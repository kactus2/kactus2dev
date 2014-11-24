/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockmodel.cpp
 *		Project: Kactus 2
 */

#include "addressblockmodel.h"
#include "addressblockdelegate.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QColor>

AddressBlockModel::AddressBlockModel(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QObject *parent):
QAbstractTableModel(parent),
addressBlock_(addressBlock),
items_(addressBlock->getRegisterData()),
componentChoices_(componentChoices)
{
	Q_ASSERT(addressBlock_);
}

AddressBlockModel::~AddressBlockModel() {
}

int AddressBlockModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return items_.size();
}

int AddressBlockModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return AddressBlockDelegate::COLUMN_COUNT;
}

Qt::ItemFlags AddressBlockModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant AddressBlockModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case AddressBlockDelegate::NAME_COLUMN: {
				return tr("Register\nname");
												  }
			case AddressBlockDelegate::OFFSET_COLUMN: {
				return tr("Offset \n[AUB]");
													}
			case AddressBlockDelegate::SIZE_COLUMN: {
				return tr("Size\n [bits]");
												 }
			case AddressBlockDelegate::DIM_COLUMN: {
				return tr("Dimension");
												  }
			case AddressBlockDelegate::DESC_COLUMN: {
				return tr("Description");
												  }
			case AddressBlockDelegate::VOLATILE_COLUMN: {
				return tr("Volatile");
												  }
			case AddressBlockDelegate::ACCESS_COLUMN: {
				return tr("Access");
												   }
			case AddressBlockDelegate::RESET_VALUE_COLUMN: {
				return tr("Reset value");
												  }
			case AddressBlockDelegate::RESET_MASK_COLUMN: {
				return tr("Reset mask");
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

QVariant AddressBlockModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case AddressBlockDelegate::NAME_COLUMN: {
				return items_.at(index.row())->getName();
													}
			case AddressBlockDelegate::OFFSET_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return reg->getAddressOffset();
				}	
				else {
					return QVariant();
				}
													  }
			case AddressBlockDelegate::SIZE_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return reg->getSize();	
				}
				else {
					return QVariant();
				}
												 }
			case AddressBlockDelegate::DIM_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return reg->getDim();
				}
				else {
					return QVariant();
				}
												   }
			case AddressBlockDelegate::DESC_COLUMN: {
				return items_.at(index.row())->getDescription();
													}
			case AddressBlockDelegate::VOLATILE_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return General::BooleanValue2Bool(reg->getVolatile(), false);
				}	
				else {
					return QVariant();
				}
												  }
			case AddressBlockDelegate::ACCESS_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return General::access2Str(reg->getAccess());
				}
				else {
					return QVariant();
				}
												   }
			case AddressBlockDelegate::RESET_VALUE_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return reg->getRegisterValue();
				}
				else {
					return QVariant();
				}
														   }
			case AddressBlockDelegate::RESET_MASK_COLUMN: {
				const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
				if (reg) {
					return reg->getRegisterMask();
				}
				else {
					return QVariant();
				}
														  }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (items_.at(index.row())->isValid(componentChoices_)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case AddressBlockDelegate::NAME_COLUMN:
			case AddressBlockDelegate::OFFSET_COLUMN:
			case AddressBlockDelegate::SIZE_COLUMN: {
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

bool AddressBlockModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case AddressBlockDelegate::NAME_COLUMN: {
				items_[index.row()]->setName(value.toString());
				break;
													}
			case AddressBlockDelegate::OFFSET_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {
					reg->setAddressOffset(value.toString());
					break;
				}	
				else {
					return false;
				}
													  }
			case AddressBlockDelegate::SIZE_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {
					reg->setSize(value.toUInt());
					break;
				}
				else {
					return false;
				}
												 }
			case AddressBlockDelegate::DIM_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {
					reg->setDim(value.toInt());
					break;
				}
				else {
					return false;
				}
												   }
			case AddressBlockDelegate::DESC_COLUMN: {
				items_[index.row()]->setDescription(value.toString());
				break;
													}
			case AddressBlockDelegate::VOLATILE_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {
					reg->setVolatile(General::bool2BooleanValue(value.toBool()));
					break;
				}	
				else {
					return false;
				}
												  }
			case AddressBlockDelegate::ACCESS_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {
					reg->setAccess(General::str2Access(value.toString(), General::ACCESS_COUNT));
					break;
				}
				else {
					return false;
				}
												   }
			case AddressBlockDelegate::RESET_VALUE_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {

					QString resetValue = value.toString();
					
					// if the reset is to be cleared
					if (resetValue.isEmpty()) {
						
						reg->clearReset();

						// also the reset mask in cleared when reset value is cleared
						QModelIndex maskIndex = QAbstractTableModel::index(index.row(), index.column() + 1, QModelIndex());
						emit dataChanged(maskIndex, maskIndex);
					}
					// otherwise set the value
					else {
						reg->setRegisterValue(resetValue);
					}
					break;
				}
				else {
					return false;
				}
														   }
			case AddressBlockDelegate::RESET_MASK_COLUMN: {
				QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
				if (reg) {

					QString resetMask = value.toString();

					// if mask is cleared and reset value was already clear
					if (reg->getRegisterValue().isEmpty() && resetMask.isEmpty()) {
						reg->clearReset();
					}
					// otherwise set the reset mask
					else {
						reg->setRegisterMask(value.toString());
					}
					break;
				}
				else {
					return false;
				}
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

bool AddressBlockModel::isValid() const 
{ 
    // Usage must be register or unspecified, if address block has children (registers).
    if (!items_.empty() && 
        (addressBlock_->getUsage() == General::MEMORY || addressBlock_->getUsage() == General::RESERVED))
    {
        return false;
    }

	foreach (QSharedPointer<RegisterModel> regModel, items_)
    {
		if (!regModel->isValid(componentChoices_))
        {
			return false;
		}
	}
	return true;
}

void AddressBlockModel::onAddItem( const QModelIndex& index ) {
    // Usage must be register or unspecified, if trying to add children (registers).
    if (addressBlock_->getUsage() == General::MEMORY || addressBlock_->getUsage() == General::RESERVED)
    {
        emit errorMessage("Registers can only be added to address blocks with usage register.");
        return;
    }

	int row = items_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	// the currently last register address
	quint64 regAddress = addressBlock_->getLastRegisterAddress();
	// if this is the first item then do not increase
	if (regAddress != 0) {
		// increase the address for a 32 bit register by default
		regAddress += 4;
	}

	// convert the address to hexadecimal form
	QString newBase = QString::number(regAddress, 16);
	newBase = newBase.toUpper();
	newBase.prepend("0x");

	beginInsertRows(QModelIndex(), row, row);
	QSharedPointer<Register> reg(new Register(addressBlock_->getVolatile(),
		addressBlock_->getAccess()));
	reg->setSize(32);
	reg->setDim(0);
	reg->setAddressOffset(newBase);
	items_.insert(row, reg);
	endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void AddressBlockModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	items_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
