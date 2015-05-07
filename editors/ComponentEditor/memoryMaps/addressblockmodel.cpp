/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockmodel.cpp
 *		Project: Kactus 2
 */

#include "addressblockmodel.h"

#include "AddressBlockColumns.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/RegisterExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QRegularExpression>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: addressblockmodel::AddressBlockModel()
//-----------------------------------------------------------------------------
AddressBlockModel::AddressBlockModel(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
addressBlock_(addressBlock),
items_(addressBlock->getRegisterData()),
componentChoices_(componentChoices),
addressUnitBits_(0),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter)
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
	return AddressBlockColumns::COLUMN_COUNT;
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
        if (section == AddressBlockColumns::NAME)
        {
            return tr("Register\nname");
        }
        else if (section == AddressBlockColumns::REGISTER_OFFSET)
        {
            QString bitOffset = tr("Offset \n[AUB]") + getExpressionSymbol();
            return bitOffset;
        }
        else if (section == AddressBlockColumns::REGISTER_SIZE)
        {
            QString size = tr("Size\n [bits]") + getExpressionSymbol();
            return size;
        }
        else if (section == AddressBlockColumns::REGISTER_DIMENSION)
        {
            QString dimension = tr("Dimension") + getExpressionSymbol();
            return dimension;
        }
        else if (section == AddressBlockColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else if (section == AddressBlockColumns::VOLATILE)
        {
            return tr("Volatile");
        }
        else if (section == AddressBlockColumns::REGISTER_ACCESS)
        {
            return tr("Access");
        }
        else if (section == AddressBlockColumns::RESET_VALUE)
        {
            return tr("Reset value");
        }
        else if (section == AddressBlockColumns::RESET_MASK)
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
        if (index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE ||
            index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
	}

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE ||
            index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }

	else if (Qt::ForegroundRole == role) {

		if (items_.at(index.row())->isValid(componentChoices_))
        {
            return blackForValidOrRedForInvalidIndex(index);
		}
		else
        {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case AddressBlockColumns::NAME:
			case AddressBlockColumns::REGISTER_OFFSET:
			case AddressBlockColumns::REGISTER_SIZE: {
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
    if (index.column() == AddressBlockColumns::NAME)
    {
        return items_.at(index.row())->getName();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
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
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            QString size = reg->getSizeExpression();

            if (size == "n/a")
            {
                reg->setSize(0);
            }

            return size;
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            QString dimension = reg->getDimensionExpression();

            if (dimension == "n/a")
            {
                reg->setDim(1);
            }

            return dimension;
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockColumns::DESCRIPTION)
    {
        return items_.at(index.row())->getDescription();
    }
    else if (index.column() == AddressBlockColumns::VOLATILE)
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
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
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
    else if (index.column() == AddressBlockColumns::RESET_VALUE)
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
    else if (index.column() == AddressBlockColumns::RESET_MASK)
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
        if (index.column() == AddressBlockColumns::NAME)
        {
            items_[index.row()]->setName(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(reg->getAddressOffset());
                }

                reg->setAddressOffset(value.toString());
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(reg->getSizeExpression());
                }

                QString calculatedExpression = parseExpressionToDecimal(value.toString());

                if (isValuePlainOrExpression(calculatedExpression))
                {
                    reg->removeSizeExpression();

                    if (calculatedExpression != value.toString())
                    {
                        reg->setSizeExpression(value.toString());
                    }

                    reg->setSize(calculatedExpression.toInt());
                }
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            QSharedPointer<Register> reg = items_[index.row()].dynamicCast<Register>();
            if (reg)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(reg->getDimensionExpression());
                }

                QString calculatedExpression = parseExpressionToDecimal(value.toString());
                
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
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            items_[index.row()]->setDescription(value.toString());
        }
        else if (index.column() == AddressBlockColumns::VOLATILE)
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
        else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
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
        else if (index.column() == AddressBlockColumns::RESET_VALUE)
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
        else if (index.column() == AddressBlockColumns::RESET_MASK)
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

        if (index.column() == AddressBlockColumns::NAME ||
            index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE ||
            index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            emit graphicsChanged();
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
    if (index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
        index.column() == AddressBlockColumns::REGISTER_SIZE ||
        index.column() == AddressBlockColumns::REGISTER_OFFSET)
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
    if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
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
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();
        if (reg)
        {
            return reg->getSizeExpression();
        }
        else
        {
            return QVariant();
        }
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
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
    const QSharedPointer<Register> reg = items_.at(index.row()).dynamicCast<Register>();

    if (reg)
    {
        if (index.column() == AddressBlockColumns::NAME)
        {
            QStringList registerNames;
            foreach (QSharedPointer<RegisterModel> registerItem, items_)
            {
                registerNames.append(registerItem->getName());
            }

            return registerNames.count(reg->getName()) == 1;
        }
        else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            QString dimension = QString::number(reg->getDim());
            return isValuePlainOrExpression(dimension);
        }
        else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            QString size = reg->getSizeExpression();
            if (isValuePlainOrExpression(size))
            {
                QString resetValue = reg->getRegisterValue();
                QString resetMask = reg->getRegisterMask();
                int registerSize = formattedValueFor(size).toInt();
                if (!resetValue.isEmpty())
                {
                    if (!validateResetColumn(resetValue, registerSize))
                    {
                        return false;
                    }
                }

                if (!resetMask.isEmpty())
                {
                    if (!validateResetColumn(resetMask, registerSize))
                    {
                        return false;
                    }
                }

                return true;
            }
            else
            {
                return false;
            }
        }

        else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
        {
            QString bitOffset = reg->getAddressOffset();
            return isValuePlainOrExpression(bitOffset);
        }

        else if (index.column() == AddressBlockColumns::RESET_VALUE)
        {
            QString resetValue = reg->getRegisterValue();
            int registerSize = formattedValueFor(reg->getSizeExpression()).toInt();

            return validateResetColumn(resetValue, registerSize);
        }

        else if (index.column() == AddressBlockColumns::RESET_MASK)
        {
            QString resetMask = reg->getRegisterMask();
            int registerSize = formattedValueFor(reg->getSizeExpression()).toInt();

            return validateResetColumn(resetMask, registerSize);
        }

        return true;
    }

    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::validateResetColumns()
//-----------------------------------------------------------------------------
bool AddressBlockModel::validateResetColumn(QString const& resetValue, int const& registerSize) const
{
    //QRegularExpression resetExpression("(?<size>[1-9]?[0-9]*)'[sS]?(?<bitChar>[bB])(?<bits>[01_]+)");
    QRegularExpression resetExpression("(?<size>[1-9]?[0-9]*)'(?<bitChar>[bB])(?<bits>[01_]+)");
    QRegularExpressionMatch match = resetExpression.match(resetValue);

    if (match.hasMatch())
    {
        QString sizeOfBits = match.captured("size");
        QString bitChar = match.captured("bitChar");
        QString bits = match.captured("bits");
        bits.remove("_");

        if (bits.size() != registerSize || (!sizeOfBits.isEmpty() && sizeOfBits.toInt() != registerSize))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int AddressBlockModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    const QSharedPointer<Register> reg = items_.at(row).dynamicCast<Register>();

    if (reg)
    {
        int referencesInOffset = reg->getAddressOffset().count(valueID);
        int referencesInSize = reg->getSizeExpression().count(valueID);
        int referencesInDimension = reg->getDimensionExpression().count(valueID);

        int totalReferences = referencesInOffset + referencesInSize + referencesInDimension;

        return totalReferences;
    }
    else
    {
        return 0;
    }
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

	// the currently last register address // Old way
	quint64 regAddress = addressBlock_->getLastRegisterAddress();

    QStringList allRegisterOffsets = addressBlock_->getAllRegisterOffsets();

    qreal lastRegDimension = 0;
    qreal lastRegSize = 0;

    for (int registerIndex = 0; registerIndex < allRegisterOffsets.size(); ++registerIndex)
    {
        QString calculatedExpression = parseExpressionToDecimal(allRegisterOffsets.at(registerIndex));
        quint64 expressionToInt = calculatedExpression.toULongLong();

        if (expressionToInt > regAddress || (registerIndex == allRegisterOffsets.size()-1 && regAddress == 0))
        {
            regAddress = expressionToInt;

            lastRegDimension = addressBlock_->getLastRegisterDimension(registerIndex);
            lastRegSize = addressBlock_->getLastRegisterSize(registerIndex);
        }
    }

    qreal offsetIncrease = 0;
    if (addressUnitBits_ != 0)
    {
        offsetIncrease = (lastRegSize / addressUnitBits_) * lastRegDimension;
    }

    regAddress = regAddress + offsetIncrease;

	// convert the address to hexadecimal form
	QString newRegAddressInHexaForm = QString::number(regAddress, 16);
	newRegAddressInHexaForm.prepend("'h");

	beginInsertRows(QModelIndex(), row, row);
	QSharedPointer<Register> reg(new Register(addressBlock_->getVolatile(),
		addressBlock_->getAccess()));
	reg->setSize(32);
	reg->setDim(0);
	reg->setAddressOffset(newRegAddressInHexaForm);
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

    QSharedPointer<Register> removedRegister = items_.at(index.row()).dynamicCast<Register>();

    if (removedRegister)
    {
        decreaseReferencesWithRemovedRegister(removedRegister);
    }

	items_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::decreaserReferencesWithRemovedRegisters()
//-----------------------------------------------------------------------------
void AddressBlockModel::decreaseReferencesWithRemovedRegister(QSharedPointer<Register> removedRegister)
{
    RegisterExpressionsGatherer* registerGatherer = new RegisterExpressionsGatherer();
    QStringList expressionList = registerGatherer->getExpressions(removedRegister);

    ReferenceCalculator* referenceCalculator = new ReferenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters = referenceCalculator->getReferencedParameters(expressionList);

    foreach (QString referencedID, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedID); ++i)
        {
            emit decreaseReferences(referencedID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void AddressBlockModel::addressUnitBitsChanged(int newAddressUnitbits)
{
    addressUnitBits_ = newAddressUnitbits;
}
