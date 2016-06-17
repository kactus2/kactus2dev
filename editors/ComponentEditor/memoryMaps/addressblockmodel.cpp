//-----------------------------------------------------------------------------
// File: addressblockmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The model to manage the registers of a single address block.
//-----------------------------------------------------------------------------

#include "addressblockmodel.h"

#include "AddressBlockColumns.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/RegisterExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QColor>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::AddressBlockModel()
//-----------------------------------------------------------------------------
AddressBlockModel::AddressBlockModel(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<RegisterValidator> registerValidator,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
addressBlock_(addressBlock),
items_(addressBlock->getRegisterData()),
addressUnitBits_(0),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
registerValidator_(registerValidator)
{
	Q_ASSERT(addressBlock_);

    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::~AddressBlockModel()
//-----------------------------------------------------------------------------
AddressBlockModel::~AddressBlockModel()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::rowCount()
//-----------------------------------------------------------------------------
int AddressBlockModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return items_->size();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::columnCount()
//-----------------------------------------------------------------------------
int AddressBlockModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return AddressBlockColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AddressBlockModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::headerData()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::headerData( int section, Qt::Orientation orientation, int role)
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
            QString bitOffset = tr("Offset\n[AUB]") + getExpressionSymbol();
            return bitOffset;
        }
        else if (section == AddressBlockColumns::REGISTER_SIZE)
        {
            QString size = tr("Size\n[bits]") + getExpressionSymbol();
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
        else if (section == AddressBlockColumns::IS_PRESENT)
        {
            return tr("Is present") + getExpressionSymbol();
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
QVariant AddressBlockModel::data(QModelIndex const& index, int role) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_->size()) 
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return valueForIndex(index);
        }
	}

    else if (role == Qt::EditRole)
    {
        if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            return valueForIndex(index);
        }

        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }

	else if (role == Qt::ForegroundRole)
    {
        if (validateIndex(index))
        {
            QSharedPointer<Register> reg = items_->at(index.row()).dynamicCast<Register>();

            if (index.column() != AddressBlockColumns::IS_PRESENT && reg &&
                (!reg->getIsPresent().isEmpty() && parseExpressionToDecimal(reg->getIsPresent()).toInt() != 1))
            {
                return QColor("gray");
            }
            else
            {
                return QColor("black");
            }
        }
        else
        {
            return QColor("red");
        }
	}
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == AddressBlockColumns::NAME ||
            index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else
    {
        return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::valueForIndex(QModelIndex const& index) const
{
    const QSharedPointer<Register> reg = items_->at(index.row()).dynamicCast<Register>();

    if (!reg)
    {
        return QVariant();
    }

    if (index.column() == AddressBlockColumns::NAME)
    {
        return reg->name();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return reg->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return reg->getSize();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return reg->getDimension();
    }
    else if (index.column() == AddressBlockColumns::DESCRIPTION)
    {
        return items_->at(index.row())->description();
    }
    else if (index.column() == AddressBlockColumns::VOLATILE)
    {
        return reg->getVolatile();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        return AccessTypes::access2Str(reg->getAccess());
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return reg->getIsPresent();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::setData()
//-----------------------------------------------------------------------------
bool AddressBlockModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_->size()) 
    {
		return false;
	}

    QSharedPointer<Register> reg = items_->at(index.row()).dynamicCast<Register>();
    if (!reg)
    {
        return false;
    }

    if (role == Qt::EditRole) 
    {
        if (index.column() == AddressBlockColumns::NAME)
        {
            reg->setName(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getAddressOffset());
            }

            reg->setAddressOffset(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getSize());
            }

            reg->setSize(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getDimension());
            }

            reg->setDimension(value.toString());
        }
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            reg->setDescription(value.toString());
        }
        else if (index.column() == AddressBlockColumns::VOLATILE)
        {
            if (value.toString() == QLatin1String("true"))
            {
                reg->setVolatile(true);
            }
            else if (value.toString() == QLatin1String("false"))
            {
                reg->setVolatile(false);
            }
            else
            {
                reg->clearVolatile();
            }
        }
        else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
        {
            reg->setAccess(AccessTypes::str2Access(value.toString(), AccessTypes::ACCESS_COUNT));
        }
        else if (index.column() == AddressBlockColumns::IS_PRESENT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getIsPresent());
            }

            reg->setIsPresent(value.toString());
        }
        else
        {
            return false;
        }

        if (index.column() == AddressBlockColumns::NAME ||
            index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE ||
            index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
            index.column() == AddressBlockColumns::IS_PRESENT)
        {
            emit graphicsChanged();
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AddressBlockModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
        index.column() == AddressBlockColumns::REGISTER_SIZE ||
        index.column() == AddressBlockColumns::REGISTER_OFFSET ||
        index.column() == AddressBlockColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    const QSharedPointer<Register> reg = items_->at(index.row()).dynamicCast<Register>();
    if (!reg)
    {
        return QVariant();
    }

    if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return reg->getDimension();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return reg->getSize();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return reg->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return reg->getIsPresent();
    }
    else 
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::validateIndex()
//-----------------------------------------------------------------------------
bool AddressBlockModel::validateIndex(QModelIndex const& index) const
{
    const QSharedPointer<Register> reg = items_->at(index.row()).dynamicCast<Register>();
    if (!reg)
    {
        return false;
    }

    if (index.column() == AddressBlockColumns::NAME)
    {
        QStringList registerNames;
        foreach (QSharedPointer<RegisterBase> registerItem, *items_)
        {
            registerNames.append(registerItem->name());
        }

        return registerNames.count(reg->name()) == 1 && registerValidator_->hasValidName(reg);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return registerValidator_->hasValidDimension(reg);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return registerValidator_->hasValidSize(reg);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return registerValidator_->hasValidAddressOffset(reg);
    }

    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return registerValidator_->hasValidIsPresent(reg);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int AddressBlockModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    const QSharedPointer<Register> reg = items_->at(row).dynamicCast<Register>();

    if (reg)
    {
        int referencesInOffset = reg->getAddressOffset().count(valueID);
        int referencesInSize = reg->getSize().count(valueID);
        int referencesInDimension = reg->getDimension().count(valueID);
        int referencesInPresence = reg->getIsPresent().count(valueID);

        int totalReferences = referencesInOffset + referencesInSize + referencesInDimension + referencesInPresence;

        return totalReferences;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onAddItem()
//-----------------------------------------------------------------------------
void AddressBlockModel::onAddItem(QModelIndex const& index)
{
    // Usage must be register or unspecified, if trying to add children (registers).
    if (addressBlock_->getUsage() == General::MEMORY || addressBlock_->getUsage() == General::RESERVED)
    {
        emit errorMessage("Registers can only be added to address blocks with usage register.");
        return;
    }

	int row = items_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the currently last register address // Old way
    quint64 regAddress = 0;

    // check all register models
    foreach (QSharedPointer<RegisterBase> regModel, *addressBlock_->getRegisterData())
    {
        QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
        if (reg)
        {
            if (regAddress < parseExpressionToDecimal(reg->getAddressOffset()).toUInt())
            {
                regAddress = parseExpressionToDecimal(reg->getAddressOffset()).toUInt();
            }
        }
    }

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

            QSharedPointer<RegisterBase> baseRegister = addressBlock_->getRegisterData()->at(registerIndex);
            QSharedPointer<Register> selectedRegister = baseRegister.dynamicCast<Register>();
            if (selectedRegister)
            {
                lastRegDimension = parseExpressionToDecimal(selectedRegister->getDimension()).toInt();
                lastRegSize = parseExpressionToDecimal(selectedRegister->getSize()).toInt();
            }
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

    QSharedPointer<Register> newRegister(new Register());
    newRegister->setSize(QStringLiteral("32"));
    newRegister->setDimension(QStringLiteral("0"));
	newRegister->setAddressOffset(newRegAddressInHexaForm);
	items_->insert(row, newRegister);
	
    endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AddressBlockModel::onRemoveItem(QModelIndex const& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= items_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    QSharedPointer<Register> removedRegister = items_->at(index.row()).dynamicCast<Register>();

    if (removedRegister)
    {
        decreaseReferencesWithRemovedRegister(removedRegister);
    }

	items_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::decreaserReferencesWithRemovedRegisters()
//-----------------------------------------------------------------------------
void AddressBlockModel::decreaseReferencesWithRemovedRegister(QSharedPointer<Register> removedRegister)
{
    RegisterExpressionsGatherer registerGatherer;
    QStringList expressionList = registerGatherer.getExpressions(removedRegister);

    ReferenceCalculator referenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);

    foreach (QString referencedID, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedID); ++i)
        {
            emit decreaseReferences(referencedID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void AddressBlockModel::addressUnitBitsChanged(int newAddressUnitbits)
{
    addressUnitBits_ = newAddressUnitbits;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onCopyRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<RegisterBase> > copiedRegisters;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<RegisterBase> registerBase = items_->at(index.row());
        copiedRegisters.append(registerBase);
    }

    QVariant registerVariant;
    registerVariant.setValue(copiedRegisters);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:register", QByteArray());
    newMimeData->setImageData(registerVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onPasteRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<RegisterBase> > >())
        {
            RegisterExpressionsGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<RegisterBase> > newRegsiters =
                pasteVariant.value<QList<QSharedPointer<RegisterBase> > >();

            int rowBegin = items_->size();
            int rowEnd = rowBegin + newRegsiters.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<RegisterBase> copiedRegister, newRegsiters)
            {
                QSharedPointer<Register> registerItem = copiedRegister.dynamicCast<Register>();
                if (registerItem)
                {
                    QSharedPointer<Register> newRegister (new Register(*registerItem.data()));
                    newRegister->setName(getUniqueName(newRegister->name(), getCurrentItemNames()));

                    items_->append(newRegister);

                    increaseReferencesInPastedRegister(newRegister, gatherer, referenceCalculator);

                    emit itemAdded(items_->size() - 1);
                }
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList AddressBlockModel::getCurrentItemNames()
{
    QStringList names;
    foreach (QSharedPointer<RegisterBase> registerItem, *items_)
    {
        names.append(registerItem->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void AddressBlockModel::increaseReferencesInPastedRegister(QSharedPointer<Register> pastedRegister,
    RegisterExpressionsGatherer& gatherer, ReferenceCalculator& referenceCalculator)
{
    QStringList registerExpressions = gatherer.getExpressions(pastedRegister);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(registerExpressions);

    QMapIterator<QString, int> refParameterIterator (referencedParameters);
    while (refParameterIterator.hasNext())
    {
        refParameterIterator.next();
        for (int i = 0; i < refParameterIterator.value(); ++i)
        {
            emit increaseReferences(refParameterIterator.key());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList AddressBlockModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:register";

    return types;
}
