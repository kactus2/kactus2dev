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
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QSharedPointer<ExpressionParser> expressionParser, QObject *parent):
QAbstractTableModel(parent),
addressBlock_(addressBlock),
items_(addressBlock->getRegisterData()),
componentChoices_(componentChoices)
{
	Q_ASSERT(addressBlock_);

    setExpressionParser(expressionParser);
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

//-----------------------------------------------------------------------------
// Function: addressblockmodel::headerData()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ )
    const 
{
	if (orientation != Qt::Horizontal) 
    {
		return QVariant();
	}
	if (Qt::DisplayRole == role) 
    {
        if (section == AddressBlockDelegate::NAME_COLUMN)
        {
            return tr("Register\nname");
        }
        else if (section == AddressBlockDelegate::OFFSET_COLUMN)
        {
            QString bitOffset = tr("Offset \n[AUB]") + getExpressionSymbol();
            return bitOffset;
        }
        else if (section == AddressBlockDelegate::SIZE_COLUMN)
        {
            return tr("Size\n [bits]");
        }
        else if (section == AddressBlockDelegate::DIM_COLUMN)
        {
            QString dimension = tr("Dimension") + getExpressionSymbol();
            return dimension;
        }
        else if (section == AddressBlockDelegate::DESC_COLUMN)
        {
            return tr("Description");
        }
        else if (section == AddressBlockDelegate::VOLATILE_COLUMN)
        {
            return tr("Volatile");
        }
        else if (section == AddressBlockDelegate::ACCESS_COLUMN)
        {
            return tr("Access");
        }
        else if (section == AddressBlockDelegate::RESET_VALUE_COLUMN)
        {
            return tr("Reset value");
        }
        else if (section == AddressBlockDelegate::RESET_MASK_COLUMN)
        {
            return tr("Reset mask");
        }
        else
        {
            return QVariant();
        }
	}
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::data()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
	if (!index.isValid()) 
    {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= items_.size()) 
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role) 
    {
        return valueForIndex(index);
	}

    else if (role == Qt::ToolTipRole || role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }

	else if (Qt::ForegroundRole == role) {

		if (items_.at(index.row())->isValid(componentChoices_)) {
            return blackForValidOrRedForInvalidIndex(index);
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

//-----------------------------------------------------------------------------
// Function: addressblockmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::valueForIndex(const QModelIndex& index) const
{
    if (index.column() == AddressBlockDelegate::NAME_COLUMN)
    {
        return items_.at(index.row())->getName();
    }
    else if (index.column() == AddressBlockDelegate::OFFSET_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return formattedValueFor(reg->getAddressOffset());
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::SIZE_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getSize();
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::DIM_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            QString dimension = QString::number(reg->getDim());
            return formattedValueFor(dimension);
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::DESC_COLUMN)
    {
        return items_.at(index.row())->getDescription();
    }
    else if (index.column() == AddressBlockDelegate::VOLATILE_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return General::BooleanValue2Bool(reg->getVolatile(), false);
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::ACCESS_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return General::access2Str(reg->getAccess());
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::RESET_VALUE_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getRegisterValue();
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::RESET_MASK_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getRegisterMask();
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::setData()
//-----------------------------------------------------------------------------
bool AddressBlockModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) 
{
	if (!index.isValid()) 
    {
		return false;
	}
	else if (index.row() < 0 || index.row() >= items_.size()) 
    {
		return false;
	}

	if (Qt::EditRole == role) 
    {
        if (index.column() == AddressBlockDelegate::NAME_COLUMN)
        {
            items_[index.row()]->setName(value.toString());
        }
        else if (index.column() == AddressBlockDelegate::OFFSET_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                reg->setAddressOffset(value.toString());
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::SIZE_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                reg->setSize(value.toUInt());
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::DIM_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                QString calculatedExpression = parseExpressionToDecimal(value.toString(), index);
                
                if (isValuePlainOrExpression(calculatedExpression))
                {
                    reg->removeDimensionExpression();

                    if (calculatedExpression != value.toString())
                    {
                        reg->setDimensionExpression(value.toString());
                    }

                    reg->setDim(calculatedExpression.toInt());
                }
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::DESC_COLUMN)
        {
            items_[index.row()]->setDescription(value.toString());
        }
        else if (index.column() == AddressBlockDelegate::VOLATILE_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                reg->setVolatile(General::bool2BooleanValue(value.toBool()));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::ACCESS_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                reg->setAccess(General::str2Access(value.toString(), General::ACCESS_COUNT));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::RESET_VALUE_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                QString resetValue = value.toString();
                
                // if the reset is to be cleared
                if (resetValue.isEmpty())
                {
                    reg->clearReset();
                    
                    // also the reset mask in cleared when reset value is cleared
                    QModelIndex maskIndex = 
                        QAbstractTableModel::index(index.row(), index.column() + 1, QModelIndex());
                    emit dataChanged(maskIndex, maskIndex);
                }
                // otherwise set the value
                else {
                    reg->setRegisterValue(resetValue);
                }
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockDelegate::RESET_MASK_COLUMN)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                QString resetMask = value.toString();
                
                // if mask is cleared and reset value was already clear
                if (reg->getRegisterValue().isEmpty() && resetMask.isEmpty())
                {
                    reg->clearReset();
                }
                // otherwise set the reset mask
                else
                {
                    reg->setRegisterMask(value.toString());
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
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

//-----------------------------------------------------------------------------
// Function: addressblockmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AddressBlockModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == AddressBlockDelegate::DIM_COLUMN || 
        index.column() == AddressBlockDelegate::OFFSET_COLUMN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == AddressBlockDelegate::DIM_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getDimensionExpression();
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockDelegate::OFFSET_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getAddressOffset();
        }
        else
        {
            return QVariant();
        }
    }

    else 
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool AddressBlockModel::validateColumnForParameter(QModelIndex const& index) const
{
    if (index.column() == AddressBlockDelegate::DIM_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            QString dimension = QString::number(reg->getDim());
            return isValuePlainOrExpression(dimension);
        }
        else
        {
            return false;
        }
    }
    else if (index.column() == AddressBlockDelegate::OFFSET_COLUMN)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            QString bitOffset = reg->getAddressOffset();
            return isValuePlainOrExpression(bitOffset);
        }
        else
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
