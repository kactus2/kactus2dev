#include "registerfilemodel.h"

#include "AddressBlockColumns.h"
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
RegisterFileModel::RegisterFileModel(QSharedPointer<RegisterFile> registerFile,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<RegisterFileValidator> registerFileValidator,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
registerFile_(registerFile),
items_(registerFile->getRegisterData()),
addressUnitBits_(0),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
registerFileValidator_(registerFileValidator),
registerValidator_(registerFileValidator->getRegisterValidator())
{
	Q_ASSERT(registerFile_);

    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::~RegisterFileModel()
//-----------------------------------------------------------------------------
RegisterFileModel::~RegisterFileModel()
{

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
	return items_->size();
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
	return AddressBlockColumns::COLUMN_COUNT;
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

    QSharedPointer<RegisterBase> regBase = items_->at(index.row());
    if (index.column() == AddressBlockColumns::REGISTER_SIZE && !regBase.dynamicCast<Register>()){
        return Qt::NoItemFlags;
    }
    else if (index.column() == AddressBlockColumns::VOLATILE && !regBase.dynamicCast<Register>()) {
        return Qt::NoItemFlags;
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS && !regBase.dynamicCast<Register>()){
        return Qt::NoItemFlags;
    }
    else if (index.column() == AddressBlockColumns::REGISTERFILE_RANGE && !regBase.dynamicCast<RegisterFile>()){
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


//-----------------------------------------------------------------------------
// Function: RegisterFileModel::headerData()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::headerData( int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return items_->at(section)->name();
    }
    else if (orientation == Qt::Vertical && role == Qt::DecorationRole){
        QSharedPointer<RegisterFile> regFile = items_->at(section).dynamicCast<RegisterFile>();
        if(regFile){
            return QIcon(":/icons/common/graphics/hierarchy.png");
        }
    }
    else if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == AddressBlockColumns::NAME)
        {
            return tr("Register\nname");
        }
        else if (section == AddressBlockColumns::REGISTER_OFFSET)
        {
            return tr("Offset\n[AUB]") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::REGISTERFILE_RANGE)
        {
            return tr("Range\n[AUB]") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::REGISTER_SIZE)
        {
            return tr("Size\n[bits]") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::REGISTER_DIMENSION)
        {
            return tr("Dimension") + getExpressionSymbol();
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

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::data()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::data(QModelIndex const& index, int role) const
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
        if(flags(index) == Qt::NoItemFlags){
          return KactusColors::DISABLED_FIELD;
        }
        else if (index.column() == AddressBlockColumns::NAME ||
            index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTERFILE_RANGE ||
            index.column() == AddressBlockColumns::REGISTER_SIZE){
            return KactusColors::MANDATORY_FIELD;
        }
        else{
            return KactusColors::REGULAR_FIELD;
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::valueForIndex(QModelIndex const& index) const
{

    const QSharedPointer<RegisterBase> regBase = items_->at(index.row());
    if (index.column() == AddressBlockColumns::NAME){
        return regBase->name();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET){
        return regBase->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION){
        return regBase->getDimension();
    }
    else if (index.column() == AddressBlockColumns::DESCRIPTION){
        return regBase->description();
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT){
        return regBase->getIsPresent();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE){
        const QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
        if(reg){
            return reg->getSize();
        }
    }
    else if (index.column() == AddressBlockColumns::VOLATILE) {
        const QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
        if(reg){
            return reg->getVolatile();
        }
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS){
        const QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
        if(reg){
            return AccessTypes::access2Str(reg->getAccess());
        }
    }
    else if (index.column() == AddressBlockColumns::REGISTERFILE_RANGE){
        const QSharedPointer<RegisterFile> regFile = regBase.dynamicCast<RegisterFile>();
        if(regFile){
            return regFile->getRange();
        }
    }

    return QVariant();

}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterFileModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    const QSharedPointer<RegisterBase> regBase = items_->at(index.row());
    if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return regBase->getDimension();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return regBase->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return regBase->getIsPresent();
    }
    else if(index.column() == AddressBlockColumns::REGISTER_SIZE){
        const QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
        if (reg){
            return reg->getSize();
        }
    }
    else if(index.column() == AddressBlockColumns::REGISTERFILE_RANGE){
        const QSharedPointer<RegisterFile> regFile = regBase.dynamicCast<RegisterFile>();
        if(regFile){
            return regFile->getRange();
        }
    }
    return data(index, Qt::DisplayRole);
}




//-----------------------------------------------------------------------------
// Function: registerfilemodel::setData()
//-----------------------------------------------------------------------------
bool RegisterFileModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_->size())
    {
		return false;
	}


    if (role == Qt::EditRole)
    {
        bool base_changed=false;
        QSharedPointer<RegisterBase> regBase = items_->at(index.row());
        if(regBase){
            base_changed=true;
            if (index.column() == AddressBlockColumns::NAME)
            {
                regBase->setName(value.toString());
                emit headerDataChanged(Qt::Vertical, index.row(), index.row());
            }
            else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(regBase->getAddressOffset());
                }

                regBase->setAddressOffset(value.toString());
            }
            else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(regBase->getDimension());
                }

                regBase->setDimension(value.toString());
            }
            else if (index.column() == AddressBlockColumns::DESCRIPTION)
            {
                regBase->setDescription(value.toString());
            }
            else if (index.column() == AddressBlockColumns::IS_PRESENT)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(regBase->getIsPresent());
                }

                regBase->setIsPresent(value.toString());
            }
            else{
                base_changed=false;
            }
        }

        bool reg_changed=false;
        QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
        QSharedPointer<RegisterFile> regFile = regBase.dynamicCast<RegisterFile>();
        if(reg){
            reg_changed=true;
            if (index.column() == AddressBlockColumns::REGISTER_SIZE)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(reg->getSize());
                }

                reg->setSize(value.toString());
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
            else{
                reg_changed=false;
            }
        }
        else if(regFile){
            reg_changed=true;
            if (index.column() == AddressBlockColumns::REGISTERFILE_RANGE)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(regFile->getRange());
                }

                regFile->setRange(value.toString());
            }
        }


        if (base_changed ||reg_changed){
            if (index.column() == AddressBlockColumns::NAME ||
                index.column() == AddressBlockColumns::REGISTER_OFFSET ||
                index.column() == AddressBlockColumns::REGISTERFILE_RANGE ||
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
    }

    return false;

}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool RegisterFileModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
           index.column() == AddressBlockColumns::REGISTER_SIZE ||
           index.column() == AddressBlockColumns::REGISTERFILE_RANGE ||
           index.column() == AddressBlockColumns::REGISTER_OFFSET ||
           index.column() == AddressBlockColumns::IS_PRESENT;
}


//-----------------------------------------------------------------------------
// Function: RegisterFileModel::validateIndex()
//-----------------------------------------------------------------------------
bool RegisterFileModel::validateIndex(QModelIndex const& index) const
{


    const QSharedPointer<RegisterBase> regBase = items_->at(index.row());
    if (index.column() == AddressBlockColumns::NAME)
    {
        QStringList registerNames;
        foreach (QSharedPointer<RegisterBase> registerItem, *items_)
        {
            registerNames.append(registerItem->name());
        }

        return registerNames.count(regBase->name()) == 1 && registerFileValidator_->hasValidName(regBase);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return registerFileValidator_->hasValidDimension(regBase);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return registerFileValidator_->hasValidAddressOffset(regBase);
    }

    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return registerFileValidator_->hasValidIsPresent(regBase);
    }




    const QSharedPointer<Register> reg = regBase.dynamicCast<Register>();
    if (reg && index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return registerValidator_->hasValidSize(reg);
    }

    const QSharedPointer<RegisterFile> regFile = regBase.dynamicCast<RegisterFile>();
    if (regFile && index.column() == AddressBlockColumns::REGISTERFILE_RANGE)
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

    const QSharedPointer<RegisterBase> regBase = items_->at(row);
    const QSharedPointer<Register> reg         = regBase.dynamicCast<Register>();
    const QSharedPointer<RegisterFile> regFile = regBase.dynamicCast<RegisterFile>();

    int count = 0;
    if(regBase){
      count+=regBase->getAddressOffset().count(valueID);
      count+=regBase->getDimension().count(valueID);
      count+=regBase->getIsPresent().count(valueID);
    }
    if (reg) {
        count+=reg->getSize().count(valueID);
    }
    else if(regFile){
      count += regFile->getRange().count(valueID);

    }
    return count;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onAddItem()
//-----------------------------------------------------------------------------
void RegisterFileModel::onAddItem(QSharedPointer<RegisterBase> regItem, QModelIndex const& index)
{
	int row = items_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the currently last register address // Old way
    quint64 regAddress = 0;

    // check all register models
    foreach (QSharedPointer<RegisterBase> regModel, *registerFile_->getRegisterData())
    {
        QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
        if (reg)
        {
            if (regAddress < parseExpressionToDecimal(reg->getAddressOffset()).toUInt())
            {
                regAddress = parseExpressionToDecimal(reg->getAddressOffset()).toUInt();
            }
        }

        QSharedPointer<RegisterFile> regFile = regModel.dynamicCast<RegisterFile>();
        if (regFile)
        {
            if (regAddress < parseExpressionToDecimal(regFile->getAddressOffset()).toUInt())
            {
                regAddress = parseExpressionToDecimal(regFile->getAddressOffset()).toUInt();
            }
        }
    }

    QStringList allRegisterOffsets = registerFile_->getAllRegisterOffsets();

    qreal lastRegDimension = 0;
    qreal lastRegSize = 0;

    for (int registerIndex = 0; registerIndex < allRegisterOffsets.size(); ++registerIndex)
    {
        QString calculatedExpression = parseExpressionToDecimal(allRegisterOffsets.at(registerIndex));
        quint64 expressionToInt = calculatedExpression.toULongLong();

        if (expressionToInt > regAddress || (registerIndex == allRegisterOffsets.size()-1 && regAddress == 0))
        {
            regAddress = expressionToInt;

            QSharedPointer<RegisterBase> baseRegister = registerFile_->getRegisterData()->at(registerIndex);
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


    regItem->setDimension(QStringLiteral("0"));
    regItem->setAddressOffset(newRegAddressInHexaForm);
    items_->insert(row, regItem);
    endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(row);
	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void RegisterFileModel::onAddRegister(QModelIndex const& index)
{
    QSharedPointer<Register> newRegister(new Register());
    newRegister->setSize(QStringLiteral("32"));
    onAddItem(newRegister, index);
}

void RegisterFileModel::onAddRegisterFile(QModelIndex const& index)
{
    QSharedPointer<RegisterFile> newRegFile(new RegisterFile());
    onAddItem(newRegFile, index);
}




//-----------------------------------------------------------------------------
// Function: RegisterFileModel::onRemoveItem()
//-----------------------------------------------------------------------------
void RegisterFileModel::onRemoveItem(QModelIndex const& index )
{
	// don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= items_->size())
    {
		return;
	}


	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    QSharedPointer<RegisterBase> removedRegister = items_->at(index.row());
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
// Function: RegisterFileModel::decreaserReferencesWithRemovedRegisters()
//-----------------------------------------------------------------------------
void RegisterFileModel::decreaseReferencesWithRemovedRegister(QSharedPointer<RegisterBase> removedRegister)
{
    QStringList expressionList;

    QSharedPointer<Register> reg = removedRegister.dynamicCast<Register>();
    QSharedPointer<RegisterFile> regFile = removedRegister.dynamicCast<RegisterFile>();
    if(reg){
      RegisterExpressionsGatherer registerGatherer;
       expressionList << registerGatherer.getExpressions(reg);
    }
    else if(regFile){
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
// Function: RegisterFileModel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void RegisterFileModel::increaseReferencesInPastedRegister(QSharedPointer<Register> pastedRegister,
                                                           RegisterExpressionsGatherer& gatherer,
                                                           ReferenceCalculator& referenceCalculator)
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

void RegisterFileModel::increaseReferencesInPastedRegisterFile(QSharedPointer<RegisterFile> pastedRegisterFile,
                                                           RegisterFileExpressionsGatherer& gatherer,
                                                           ReferenceCalculator& referenceCalculator)
{
    QStringList registerExpressions = gatherer.getExpressions(pastedRegisterFile);

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
// Function: RegisterFileModel::onPasteRows()
//-----------------------------------------------------------------------------
void RegisterFileModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<RegisterBase> > >())
        {
            RegisterExpressionsGatherer gatherer;
            RegisterFileExpressionsGatherer rfgatherer;
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
                    continue;
                }
                QSharedPointer<RegisterFile> regFileItem = copiedRegister.dynamicCast<RegisterFile>();
                if (regFileItem)
                {
                    QSharedPointer<RegisterFile> newRegFile (new RegisterFile(*regFileItem.data()));
                    newRegFile->setName(getUniqueName(newRegFile->name(), getCurrentItemNames()));

                    items_->append(newRegFile);

                    increaseReferencesInPastedRegisterFile(newRegFile, rfgatherer, referenceCalculator);

                    emit itemAdded(items_->size() - 1);
                }
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: registerfilemodel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList RegisterFileModel::getCurrentItemNames()
{
    QStringList names;
    foreach (QSharedPointer<RegisterBase> registerItem, *items_)
    {
        names.append(registerItem->name());
    }

    return names;
}


//-----------------------------------------------------------------------------
// Function: RegisterFileModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList RegisterFileModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:register";

    return types;
}
