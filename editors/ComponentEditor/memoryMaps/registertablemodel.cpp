//-----------------------------------------------------------------------------
// File: registertablemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.08.2012
//
// Description:
// The model to manage the details of a single register.
//-----------------------------------------------------------------------------

#include "registertablemodel.h"
#include "RegisterColumns.h"

#include <IPXACTmodels/Component/validators/FieldValidator.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/FieldExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QColor>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: registertablemodel::RegisterTableModel()
//-----------------------------------------------------------------------------
RegisterTableModel::RegisterTableModel(QSharedPointer<Register> reg,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<FieldValidator> fieldValidator,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
reg_(reg),
fields_(reg->getFields()),
expressionFormatter_(expressionFormatter),
fieldValidator_(fieldValidator)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::~RegisterTableModel()
//-----------------------------------------------------------------------------
RegisterTableModel::~RegisterTableModel() 
{

}

//-----------------------------------------------------------------------------
// Function: registertablemodel::rowCount()
//-----------------------------------------------------------------------------
int RegisterTableModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const 
{
	if (parent.isValid()) 
    {
		return 0;
	}
	return fields_->size();
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::columnCount()
//-----------------------------------------------------------------------------
int RegisterTableModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const 
{
	if (parent.isValid()) 
    {
		return 0;
	}
    return RegisterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RegisterTableModel::flags( const QModelIndex& index ) const 
{
	if (!index.isValid()) 
    {
		return Qt::NoItemFlags;
	}
	// if the field is not testable then the test constraint can not be set
    if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN && !fields_->at(index.row())->getTestable().toBool())
    {
        return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::headerData()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::headerData( int section, Qt::Orientation orientation, 
    int role /*= Qt::DisplayRole*/ ) const 
{
	if (orientation != Qt::Horizontal) 
    {
		return QVariant();
	}
	if (Qt::DisplayRole == role) 
    {
        if (section == RegisterColumns::NAME_COLUMN)
        {
            return tr("Field\nname");
        }
        else if (section == RegisterColumns::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
        else if (section == RegisterColumns::OFFSET_COLUMN)
        {
            QString bitOffset = tr("Offset \n[bits]") + getExpressionSymbol();
            return bitOffset;
        }
        else if (section == RegisterColumns::WIDTH_COLUMN)
        {
            QString bitWidth = tr("Width \n[bits]") + getExpressionSymbol();
            return bitWidth;
        }
        else if (section == RegisterColumns::RESETVALUE_COLUMN)
        {
            QString resetValueHeader = tr("Reset value") + getExpressionSymbol();
            return resetValueHeader;
        }
        else if (section == RegisterColumns:: RESETMASK_COLUMN)
        {
            QString resetMaskHeader = tr("Reset mask") + getExpressionSymbol();
            return resetMaskHeader;
        }
        else if (section == RegisterColumns::VOLATILE_COLUMN)
        {
            return tr("Volatile");
        }
        else if (section == RegisterColumns::ACCESS_COLUMN)
        {
            return tr("Access");
        }
        else if (section == RegisterColumns::MOD_WRITE_COLUMN)
        {
            return tr("Modified\nwrite value");
        }
        else if (section == RegisterColumns::READ_ACTION_COLUMN)
        {
            return tr("Read\naction");
        }
        else if (section == RegisterColumns::TESTABLE_COLUMN)
        {
            return tr("Testable");
        }
        else if (section == RegisterColumns::TEST_CONSTR_COLUMN)
        {
            return tr("Test\nconstraint");
        }
        else if (section == RegisterColumns::IS_PRESENT_COLUMN)
        {
            QString isPresent = tr("Is Present") + getExpressionSymbol();
            return isPresent;
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
// Function: registertablemodel::data()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
	if (!index.isValid()) 
    {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= fields_->size()) 
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
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
        if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
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

	else if (Qt::ForegroundRole == role) 
    {
        if (validateIndex(index))
        {
            QString isPresentValue = fields_->at(index.row())->getIsPresent();
            if (index.column() != RegisterColumns::IS_PRESENT_COLUMN && 
                (!isPresentValue.isEmpty() && parseExpressionToDecimal(isPresentValue).toInt() != 1))
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
	else if (Qt::BackgroundRole == role) 
    {
        if (index.column() == RegisterColumns::NAME_COLUMN || index.column() == RegisterColumns::OFFSET_COLUMN ||
            index.column() == RegisterColumns::WIDTH_COLUMN)
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
// Function: registertablemodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == RegisterColumns::NAME_COLUMN)
    {
        return fields_->at(index.row())->name();
    }
    else if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
    {
        return fields_->at(index.row())->description();
    }
    else if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        QString bitOffset = fields_->at(index.row())->getBitOffset();
        return bitOffset;
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        QString bitWidth = fields_->at(index.row())->getBitWidth();
        return bitWidth;
    }
    else if (index.column() == RegisterColumns::RESETVALUE_COLUMN)
    {
        QString resetValue = fields_->at(index.row())->getResetValue();
        return resetValue;
    }
    else if (index.column() == RegisterColumns::RESETMASK_COLUMN)
    {
        QString resetMask = fields_->at(index.row())->getResetMask();
        return resetMask;
    }
    else if (index.column() == RegisterColumns::VOLATILE_COLUMN)
    {
        return fields_->at(index.row())->getVolatile().toString();
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        return AccessTypes::access2Str(fields_->at(index.row())->getAccess());
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        return General::modifiedWrite2Str(fields_->at(index.row())->getModifiedWrite());
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        return General::readAction2Str(fields_->at(index.row())->getReadAction());
    }
    else if (index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        return fields_->at(index.row())->getTestable().toString();
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        return General::testConstraint2Str(fields_->at(index.row())->getTestConstraint());
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return fields_->at(index.row())->getIsPresent();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::setData()
//-----------------------------------------------------------------------------
bool RegisterTableModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= fields_->size()) 
    {
		return false;
	}

	if (role == Qt::EditRole) 
    {
        if (index.column() == RegisterColumns::NAME_COLUMN)
        {
            fields_->at(index.row())->setName(value.toString());

            emit graphicsChanged();
        }
        else if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
        {
            fields_->at(index.row())->setDescription(value.toString());
        }
        else if (index.column() == RegisterColumns::OFFSET_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(fields_->at(index.row())->getBitOffset());
            }

            fields_->at(index.row())->setBitOffset(value.toString());

            emit graphicsChanged();
        }
        else if (index.column() == RegisterColumns::WIDTH_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(fields_->at(index.row())->getBitWidth());
            }

            fields_->at(index.row())->setBitWidth(value.toString());

            emit graphicsChanged();
        }
        else if (index.column() == RegisterColumns::RESETVALUE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(fields_->at(index.row())->getResetValue());
            }

            fields_->at(index.row())->setResetValue(value.toString());
        }
        else if (index.column() == RegisterColumns::RESETMASK_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(fields_->at(index.row())->getResetMask());
            }

            fields_->at(index.row())->setResetMask(value.toString());
        }
        else if (index.column() == RegisterColumns::VOLATILE_COLUMN)
        {
            if (value.toString() == QLatin1String("true"))
            {
                fields_->at(index.row())->setVolatile(true);
            }
            else if (value.toString() == QLatin1String("false"))
            {
                fields_->at(index.row())->setVolatile(false);
            }
            else
            {
                fields_->at(index.row())->clearVolatile();
            }
        }
        else if (index.column() == RegisterColumns::ACCESS_COLUMN)
        {
            fields_->at(index.row())->setAccess(
                AccessTypes::str2Access(value.toString(), AccessTypes::ACCESS_COUNT));
        }
        else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
        {
            fields_->at(index.row())->setModifiedWrite(General::str2ModifiedWrite(value.toString()));
        }
        else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
        {
            fields_->at(index.row())->setReadAction(General::str2ReadAction(value.toString()));
        }
        else if (index.column() == RegisterColumns::TESTABLE_COLUMN)
        {
            if (value.toString() == QLatin1String("true"))
            {
                fields_->at(index.row())->setTestable(true);
            }
            else if (value.toString() == QLatin1String("false"))
            {
                fields_->at(index.row())->setTestable(false);
                fields_->at(index.row())->setTestConstraint(General::TESTCONSTRAINT_COUNT);
                QModelIndex constrIndex = createIndex(index.row(), index.column() + 1, index.internalPointer());
                emit dataChanged(constrIndex, constrIndex);
            }
            else
            {
                fields_->at(index.row())->clearTestable();
            }
        }
        else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
        {
            fields_->at(index.row())->setTestConstraint(General::str2TestConstraint(value.toString()));
        }
        else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(fields_->at(index.row())->getIsPresent());
            }

            fields_->at(index.row())->setIsPresent(value.toString());

            emit graphicsChanged();
        }
        else
        {
            return false;
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
// Function: registertablemodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool RegisterTableModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == RegisterColumns::OFFSET_COLUMN || index.column() == RegisterColumns::WIDTH_COLUMN ||
        index.column() == RegisterColumns::IS_PRESENT_COLUMN ||
        index.column() == RegisterColumns::RESETVALUE_COLUMN ||
        index.column() == RegisterColumns::RESETMASK_COLUMN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return fields_->at(index.row())->getBitOffset();
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return fields_->at(index.row())->getBitWidth();
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return fields_->at(index.row())->getIsPresent();
    }
    else if (index.column() == RegisterColumns::RESETVALUE_COLUMN)
    {
        return fields_->at(index.row())->getResetValue();
    }
    else if (index.column() == RegisterColumns::RESETMASK_COLUMN)
    {
        return fields_->at(index.row())->getResetMask();
    }

    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::validateIndex()
//-----------------------------------------------------------------------------
bool RegisterTableModel:: validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Field> field = fields_->at(index.row());

    if (index.column() == RegisterColumns::NAME_COLUMN)
    {
        return fieldValidator_->hasValidName(field);
    }
    else if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return fieldValidator_->hasValidBitOffset(field);
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return fieldValidator_->hasValidBitWidth(field);
    }
    else if (index.column() == RegisterColumns::RESETVALUE_COLUMN)
    {
        return fieldValidator_->hasValidResetValue(field);
    }
    else if (index.column() == RegisterColumns::RESETMASK_COLUMN)
    {
        return fieldValidator_->hasValidResetMask(field);
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return fieldValidator_->hasValidIsPresent(field);
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        return fieldValidator_->hasValidAccess(field);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int RegisterTableModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInBitOffset = fields_->at(row)->getBitOffset().count(valueID);
    int referencesInBitWidth = fields_->at(row)->getBitWidth().count(valueID);
    int referencesInIsPresent = fields_->at(row)->getIsPresent().count(valueID);

    int referencesInResetValue = fields_->at(row)->getResetValue().count(valueID);
    int referencesInResetMask = fields_->at(row)->getResetMask().count(valueID);

    int totalReferences = referencesInBitOffset + referencesInBitWidth + referencesInIsPresent +
        referencesInResetValue + referencesInResetMask;

    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::onAddItem()
//-----------------------------------------------------------------------------
void RegisterTableModel::onAddItem( const QModelIndex& index )
{
	int row = fields_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    QSharedPointer<Field> field(new Field());
	fields_->insert(row, field);
	endInsertRows();

	// inform navigation tree that file set is added
	emit fieldAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::onRemoveItem()
//-----------------------------------------------------------------------------
void RegisterTableModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= fields_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());
	fields_->removeAt(index.row());

    endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fieldRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::onCopyRows()
//-----------------------------------------------------------------------------
void RegisterTableModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<Field> > copiedFields;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<Field> field = fields_->at(index.row());
        copiedFields.append(field);
    }

    QVariant fieldVariant;
    fieldVariant.setValue(copiedFields);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:field", QByteArray());
    newMimeData->setImageData(fieldVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::onPasteRows()
//-----------------------------------------------------------------------------
void RegisterTableModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Field> > >())
        {
            FieldExpressionsGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<Field> > newFields = pasteVariant.value<QList<QSharedPointer<Field> > >();

            int rowBegin = fields_->size();
            int rowEnd = rowBegin + newFields.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<Field> copiedField, newFields)
            {
                QSharedPointer<Field> newField (new Field(*copiedField.data()));
                newField->setName(getUniqueName(newField->name(), getCurrentItemNames()));

                fields_->append(newField);

                increaseReferencesInPastedField(newField, gatherer, referenceCalculator);

                emit fieldAdded(fields_->size() - 1);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList RegisterTableModel::getCurrentItemNames() const
{
    QStringList names;
    foreach (QSharedPointer<Field> currentField, *fields_)
    {
        names.append(currentField->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void RegisterTableModel::increaseReferencesInPastedField(QSharedPointer<Field> pastedField,
    FieldExpressionsGatherer& gatherer, ReferenceCalculator& referenceCalculator)
{
    QStringList fieldExpressions = gatherer.getExpressions(pastedField);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(fieldExpressions);

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
// Function: RegisterTableModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList RegisterTableModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:field";

    return types;
}
