//-----------------------------------------------------------------------------
// File: registerfilemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// The model to manage the register files of a single address block or another register file.
//-----------------------------------------------------------------------------

#include "registerfilemodel.h"

#include "RegisterFileColumns.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/RegisterFileExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/RegisterExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::RegisterFileModel()
//-----------------------------------------------------------------------------
RegisterFileModel::RegisterFileModel(QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData,
    QSharedPointer<ExpressionParser> expressionParser, 
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
registerData_(registerData),
items_(),
addressUnitBits_(0),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
registerFileValidator_(registerFileValidator)
{
    for (auto item : *registerData_)
    {
        QSharedPointer<RegisterFile> registerFile = item.dynamicCast<RegisterFile>();
        if (registerFile)
        {
            items_.append(registerFile);
        }
    }

    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::rowCount()
//-----------------------------------------------------------------------------
int RegisterFileModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}    

    return items_.size();    
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::columnCount()
//-----------------------------------------------------------------------------
int RegisterFileModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return RegisterFileColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RegisterFileModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::headerData()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::headerData( int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        return items_.at(section)->name();
    }

    else if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == RegisterFileColumns::NAME)
        {
            return tr("Register file\nname");
        }
        else if (section == RegisterFileColumns::ADDRESS_OFFSET)
        {
            return tr("Offset\n[AUB]") + getExpressionSymbol();
        }
        else if (section == RegisterFileColumns::RANGE)
        {
            return tr("Range\n[AUB]") + getExpressionSymbol();
        }
        else if (section == RegisterFileColumns::DIMENSION)
        {
            return tr("Dimension") + getExpressionSymbol();
        }
        else if (section == RegisterFileColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else if (section == RegisterFileColumns::IS_PRESENT)
        {
            return tr("Is present") + getExpressionSymbol();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::data()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == RegisterFileColumns::DESCRIPTION)
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
        if (index.column() == RegisterFileColumns::DESCRIPTION)
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
            QSharedPointer<RegisterFile> regFile = items_.at(index.row());

            if (index.column() != RegisterFileColumns::IS_PRESENT &&
                (!regFile->getIsPresent().isEmpty() && 
                    parseExpressionToDecimal(regFile->getIsPresent()).toInt() != 1))
            {
                return KactusColors::DISABLED_TEXT;
            }
            else
            {
                return KactusColors::REGULAR_TEXT;
            }            
        }
        else
        {
            return KactusColors::ERROR;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == RegisterFileColumns::NAME ||
            index.column() == RegisterFileColumns::ADDRESS_OFFSET ||
            index.column() == RegisterFileColumns::RANGE)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RegisterFile> const& regFile = items_.at(index.row());

    if (index.column() == RegisterFileColumns::NAME)
    {
        return regFile->name();
    }
    else if (index.column() == RegisterFileColumns::ADDRESS_OFFSET)
    {
        return regFile->getAddressOffset();
    }
    else if (index.column() == RegisterFileColumns::RANGE)
    {
        return regFile->getRange();
    }
    else if (index.column() == RegisterFileColumns::DIMENSION)
    {
        return regFile->getDimension();
    }
    else if (index.column() == RegisterFileColumns::DESCRIPTION)
    {
        return regFile->description();
    }
    else if (index.column() == RegisterFileColumns::IS_PRESENT)
    {
        return regFile->getIsPresent();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RegisterFile> const& regFile = items_.at(index.row());
    if (index.column() == RegisterFileColumns::DIMENSION)
    {
        return regFile->getDimension();
    }
    else if (index.column() == RegisterFileColumns::ADDRESS_OFFSET)
    {
        return regFile->getAddressOffset();
    }
    else if (index.column() == RegisterFileColumns::IS_PRESENT)
    {
        return regFile->getIsPresent();
    }
    else if(index.column() == RegisterFileColumns::RANGE)
    {
        return regFile->getRange();
    }
  
    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: registerfilemodel::setData()
//-----------------------------------------------------------------------------
bool RegisterFileModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<RegisterFile> const& regFile = items_.at(index.row());

        bool changed = true;
        if (index.column() == RegisterFileColumns::NAME)
        {
            regFile->setName(value.toString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
        }
        else if (index.column() == RegisterFileColumns::ADDRESS_OFFSET)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(regFile->getAddressOffset());
            }

            regFile->setAddressOffset(value.toString());
        }
        else if (index.column() == RegisterFileColumns::DIMENSION)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(regFile->getDimension());
            }

            regFile->setDimension(value.toString());
        }
        else if (index.column() == RegisterFileColumns::DESCRIPTION)
        {
            regFile->setDescription(value.toString());
        }
        else if (index.column() == RegisterFileColumns::IS_PRESENT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(regFile->getIsPresent());
            }

            regFile->setIsPresent(value.toString());
        }
        else if (index.column() == RegisterFileColumns::RANGE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(regFile->getRange());
            }

            regFile->setRange(value.toString());
            changed = true;
        }
        else
        {
            changed = false;
        }

        if (changed)
        {
            if (index.column() == RegisterFileColumns::NAME ||
                index.column() == RegisterFileColumns::ADDRESS_OFFSET ||
                index.column() == RegisterFileColumns::RANGE ||
                index.column() == RegisterFileColumns::DIMENSION ||
                index.column() == RegisterFileColumns::IS_PRESENT)
            {
                emit graphicsChanged(registerData_->indexOf(regFile));

                if (index.column() != RegisterFileColumns::NAME)
                {
                    emit childAddressingChanged(registerData_->indexOf(regFile));
                }
            }

            emit dataChanged(index, index);
            emit contentChanged();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList RegisterFileModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());
    types << "text/xml/ipxact:registerFile";

    return types;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool RegisterFileModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == RegisterFileColumns::DIMENSION||
           index.column() == RegisterFileColumns::RANGE ||
           index.column() == RegisterFileColumns::ADDRESS_OFFSET ||
           index.column() == RegisterFileColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::validateIndex()
//-----------------------------------------------------------------------------
bool RegisterFileModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<RegisterFile> regFile = items_.at(index.row());

    if (index.column() == RegisterFileColumns::NAME)
    {
        QStringList registerNames;
        foreach (QSharedPointer<RegisterFile> registerItem, items_)
        {
            registerNames.append(registerItem->name());
        }

        return registerNames.count(regFile->name()) == 1 && registerFileValidator_->hasValidName(regFile);
    }
    else if (index.column() == RegisterFileColumns::DIMENSION)
    {
        return registerFileValidator_->hasValidDimension(regFile);
    }

    else if (index.column() == RegisterFileColumns::ADDRESS_OFFSET)
    {
        return registerFileValidator_->hasValidAddressOffset(regFile);
    }

    else if (index.column() == RegisterFileColumns::IS_PRESENT)
    {
        return registerFileValidator_->hasValidIsPresent(regFile);
    }

    if (regFile && index.column() == RegisterFileColumns::RANGE)
    {
        return registerFileValidator_->hasValidRange(regFile);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int RegisterFileModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{    
    QSharedPointer<RegisterFile> const& regFile = items_.at(row);

    int count = 0;
    count += regFile->getAddressOffset().count(valueID);
    count += regFile->getDimension().count(valueID);
    count += regFile->getIsPresent().count(valueID);
    count += regFile->getRange().count(valueID);

    return count;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onAddItem()
//-----------------------------------------------------------------------------
void RegisterFileModel::onAddItem(QModelIndex const& index)
{
    // Find last register/registerFile by address offset.
    auto lastRegister = std::max_element(registerData_->cbegin(), registerData_->cend(),
        [this](QSharedPointer<RegisterBase> const& a, QSharedPointer<RegisterBase> const& b)
    {
        return parseExpressionToDecimal(a->getAddressOffset()).toInt() < 
            parseExpressionToDecimal(b->getAddressOffset()).toInt();
    });

    quint64 lastRegAddress = 0;
    quint64 lastRegDimension = 0;
    quint64 lastRegRange = 0;

    if (lastRegister != registerData_->cend())
    {
        lastRegAddress = parseExpressionToDecimal((*lastRegister)->getAddressOffset()).toInt();
        lastRegDimension = parseExpressionToDecimal((*lastRegister)->getDimension()).toInt();

        QSharedPointer<Register> reg = lastRegister->dynamicCast<Register>();
        QSharedPointer<RegisterFile> regFile = lastRegister->dynamicCast<RegisterFile>();
        if (reg)
        {
            lastRegRange = parseExpressionToDecimal(reg->getSize()).toInt() / qMax(addressUnitBits_, 1u);
        }
        else if (regFile)
        {
            lastRegRange = parseExpressionToDecimal(regFile->getRange()).toInt();
        }
    }
   
    qreal offsetIncrease = qMax(quint64(1), lastRegRange * qMax(quint64(1), lastRegDimension));

    // convert the address to hexadecimal form
    quint64 itemAddress = lastRegAddress + offsetIncrease;
	QString offset = QStringLiteral("'h") + QString::number(itemAddress, 16);

    // if the index is valid then add the item to the correct position
    int row = items_.size();
    if (index.isValid())
    {
        row = index.row();
    }

    int dataIndex = registerData_->size();
    if (row < items_.size() - 1)
    {
        dataIndex = registerData_->indexOf(items_.at(row + 1));
    }

	beginInsertRows(QModelIndex(), row, row);
    QSharedPointer<RegisterFile> regItem(new RegisterFile());    
    regItem->setAddressOffset(offset);
    regItem->setRange(QString::number(1));
    items_.insert(row, regItem);
    registerData_->insert(dataIndex, regItem);
    endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(dataIndex);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onRemoveItem()
//-----------------------------------------------------------------------------
void RegisterFileModel::onRemoveItem(QModelIndex const& index )
{
	// don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return;
	}


	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    QSharedPointer<RegisterBase> removedRegister = items_.at(index.row());
    if (removedRegister)
    {
        decreaseReferencesWithRemovedRegister(removedRegister);
    }

    int itemRow = registerData_->indexOf(removedRegister);

	items_.removeAt(index.row());
    registerData_->removeAt(itemRow);
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(itemRow);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::decreaserReferencesWithRemovedRegisters()
//-----------------------------------------------------------------------------
void RegisterFileModel::decreaseReferencesWithRemovedRegister(QSharedPointer<RegisterBase> removedRegister)
{
    QStringList expressionList;

    QSharedPointer<RegisterFile> regFile = removedRegister.dynamicCast<RegisterFile>();
    
    if(regFile)
    {
      RegisterFileExpressionsGatherer regFileGatherer;
      expressionList << regFileGatherer.getExpressions(regFile);
    }

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
// Function: RegisterFileModel::increaseReferencesInPastedRegisterFile()
//-----------------------------------------------------------------------------
void RegisterFileModel::increaseReferencesInPastedRegisterFile(QSharedPointer<RegisterFile> pastedRegisterFile,
    RegisterFileExpressionsGatherer& gatherer,
    ReferenceCalculator& referenceCalculator)
{
    QStringList registerExpressions = gatherer.getExpressions(pastedRegisterFile);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(registerExpressions);

    QMapIterator<QString, int> refParameterIterator(referencedParameters);
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
// Function: RegisterFileModel::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void RegisterFileModel::addressUnitBitsChanged(int newAddressUnitbits)
{
    addressUnitBits_ = newAddressUnitbits;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onCopyRows()
//-----------------------------------------------------------------------------
void RegisterFileModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<RegisterFile> > copiedRegisters;
    for (QModelIndex index : indexList)
    {
        copiedRegisters.append(items_.at(index.row()));
    }

    QVariant registerVariant;
    registerVariant.setValue(copiedRegisters);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:registerFile", QByteArray());
    newMimeData->setImageData(registerVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onPasteRows()
//-----------------------------------------------------------------------------
void RegisterFileModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<RegisterFile> > >())
        {            
            RegisterFileExpressionsGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<RegisterFile> > newRegisters =
                pasteVariant.value<QList<QSharedPointer<RegisterFile> > >();

            int rowBegin = items_.size();
            int rowEnd = rowBegin + newRegisters.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            for (QSharedPointer<RegisterFile> copiedRegister : newRegisters)
            {                

                    QSharedPointer<RegisterFile> newRegFile (new RegisterFile(*copiedRegister));
                    newRegFile->setName(getUniqueName(newRegFile->name(), getAllNames()));

                    registerData_->append(newRegFile);
                    items_.append(newRegFile);

                    increaseReferencesInPastedRegisterFile(newRegFile, gatherer, referenceCalculator);

                    emit itemAdded(registerData_->size() - 1);                
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: registerfilemodel::getAllNames()
//-----------------------------------------------------------------------------
QStringList RegisterFileModel::getAllNames() const
{
    QStringList names;
    for (QSharedPointer<RegisterFile> registerItem : items_)
    {
        names.append(registerItem->name());
    }

    return names;
}
