//-----------------------------------------------------------------------------
// File: registertablemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.08.2012
//
// Description:
// The model to manage the details of a single register.
//-----------------------------------------------------------------------------

#include "registertablemodel.h"
#include "RegisterColumns.h"

#include <IPXACTmodels/Component/validators/FieldValidator.h>

#include <KactusAPI/include//FieldExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <KactusAPI/include/FieldInterface.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: registertablemodel::RegisterTableModel()
//-----------------------------------------------------------------------------
RegisterTableModel::RegisterTableModel(FieldInterface* fieldInterface,
    QSharedPointer <ExpressionParser> expressionParser, QSharedPointer <ParameterFinder> parameterFinder,
    Document::Revision docRevision,
    QObject *parent) :
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
fieldInterface_(fieldInterface),
docRevision_(docRevision)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::rowCount()
//-----------------------------------------------------------------------------
int RegisterTableModel::rowCount( const QModelIndex& /*parent = QModelIndex()*/ ) const 
{
    return fieldInterface_->itemCount();
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

    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

    // Lock resets column
    if (index.column() == RegisterColumns::RESETS_COLUMN)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    // Lock std14 columns (values located in field access policy for std22), if there are multiple field access 
    // policies defined for the field.
    if (isStd14Column(index) && docRevision_ == Document::Revision::Std22 &&
        fieldInterface_->getAccessPolicyCount(fieldName) > 1)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

	// if the field is not testable then the test constraint can not be set
    if (int accessPolicyIndex = docRevision_ == Document::Revision::Std22 ? 0 : -1;
        index.column() == RegisterColumns::TEST_CONSTR_COLUMN && 
        !fieldInterface_->getTestableBool(fieldName, accessPolicyIndex))
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::headerData()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::headerData( int section, Qt::Orientation orientation, 
    int role /*= Qt::DisplayRole*/ ) const 
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) 
    {
        return QString::fromStdString(fieldInterface_->getIndexedItemName(section));
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
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
        else if (section == RegisterColumns::RESETS_COLUMN)
        {
            return tr("Resets");
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
// Function: registertablemodel::data()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const 
{
	if (!index.isValid()) 
    {
		return QVariant();
	}
    else if (index.row() < 0 || index.row() >= fieldInterface_->itemCount()) 
    {
		return QVariant();
	}

    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

	if (Qt::DisplayRole == role) 
    {
        return getIndexValueByStdRevision(index);
	}

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == RegisterColumns::RESETS_COLUMN)
        {
            return QString::fromStdString(fieldInterface_->getResetsToolTip(fieldName));
        }
        else
        {
            return valueForIndex(index);
        }
    }

	else if (Qt::ForegroundRole == role) 
    {
        if (validateIndex(index))
        {
            std::string fieldPresence = fieldInterface_->getIsPresentExpression(fieldName);
            qint64 fieldPresenceValue =
                QString::fromStdString(fieldInterface_->getIsPresentValue(fieldName, 10)).toLongLong();

            if (index.column() != RegisterColumns::IS_PRESENT_COLUMN &&
                (!fieldPresence.empty() && fieldPresenceValue != 1))
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
	else if (Qt::BackgroundRole == role) 
    {
        if (index.column() == RegisterColumns::NAME_COLUMN || index.column() == RegisterColumns::OFFSET_COLUMN ||
            index.column() == RegisterColumns::WIDTH_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else if (index.column() == RegisterColumns::RESETS_COLUMN ||
            index.flags() == (Qt::ItemIsEnabled | Qt::ItemIsSelectable))
        {
            return KactusColors::DISABLED_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
		}
	}
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

    if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getOffsetFormattedExpression(fieldName));
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getWidthFormattedExpression(fieldName));
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getIsPresentFormattedExpression(fieldName));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::expressionForIndex(QModelIndex const& index) const
{
    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

    if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getOffsetExpression(fieldName));
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getWidthExpression(fieldName));
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getIsPresentExpression(fieldName));
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
    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

    // Get the value from first field access policy if std22 is used, and there are zero or one field
    // access policies defined for the field. Get the value from the field if using std14.
    int accessPolicyIndex = docRevision_ == Document::Revision::Std22 ? 0 : -1;

    if (index.column() == RegisterColumns::NAME_COLUMN)
    {
        return QString::fromStdString(fieldName);
    }
    else if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getDescription(fieldName));
    }
    else if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getOffsetValue(fieldName));
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getWidthValue(fieldName));
    }
    else if (index.column() == RegisterColumns::RESETS_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getResets(fieldName));
    }
    else if (index.column() == RegisterColumns::VOLATILE_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getVolatile(fieldName));
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getAccessString(fieldName, accessPolicyIndex));
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getModifiedWriteString(fieldName, accessPolicyIndex));
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getReadActionString(fieldName, accessPolicyIndex));
    }
    else if (index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getTestableValue(fieldName, accessPolicyIndex));
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getTestConstraintString(fieldName, accessPolicyIndex));
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return QString::fromStdString(fieldInterface_->getIsPresentValue(fieldName));
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
    if (!index.isValid() || index.row() < 0 || index.row() >= fieldInterface_->itemCount()) 
    {
		return false;
	}

    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

	if (role == Qt::EditRole) 
    {
        if (index.column() == RegisterColumns::NAME_COLUMN)
        {
            fieldInterface_->setName(fieldName, value.toString().toStdString());

            QString oldName = QString::fromStdString(fieldName);
            QString newName = QString::fromStdString(fieldInterface_->getIndexedItemName(index.row()));

            emit fieldNameChanged(oldName, newName);

            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
            emit graphicsChanged(index.row());
        }
        else if (index.column() == RegisterColumns::DESCRIPTION_COLUMN)
        {
            fieldInterface_->setDescription(fieldName, value.toString().toStdString());
        }
        else if (index.column() == RegisterColumns::OFFSET_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(fieldInterface_->getOffsetExpression(fieldName)));
            }

            fieldInterface_->setOffset(fieldName, value.toString().toStdString());

            emit graphicsChanged(index.row());
            emit addressingChanged(index.row());
        }
        else if (index.column() == RegisterColumns::WIDTH_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(fieldInterface_->getWidthExpression(fieldName)));
            }

            fieldInterface_->setWidth(fieldName, value.toString().toStdString());

            emit graphicsChanged(index.row());
            emit addressingChanged(index.row());
        }
        else if (index.column() == RegisterColumns::VOLATILE_COLUMN)
        {
            fieldInterface_->setVolatile(fieldName, value.toString().toStdString());
        }
        else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(fieldInterface_->getIsPresentExpression(fieldName)));
            }

            fieldInterface_->setIsPresent(fieldName, value.toString().toStdString());

            emit graphicsChanged(index.row());
            emit addressingChanged(index.row());
        }
        else if (isStd14Column(index))
        {
            if (!setStd14ColumnData(index, value, fieldName))
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
        index.column() == RegisterColumns::IS_PRESENT_COLUMN || index.column() == RegisterColumns::RESETS_COLUMN)
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
    if (index.column() == RegisterColumns::OFFSET_COLUMN || index.column() == RegisterColumns::WIDTH_COLUMN ||
        index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::validateIndex()
//-----------------------------------------------------------------------------
bool RegisterTableModel:: validateIndex(QModelIndex const& index) const
{
    std::string fieldName = fieldInterface_->getIndexedItemName(index.row());

    int accessPolicyIndex = docRevision_ == Document::Revision::Std22 ? 0 : -1;

    if (index.column() == RegisterColumns::NAME_COLUMN)
    {
        return fieldInterface_->itemHasValidName(fieldName);
    }
    else if (index.column() == RegisterColumns::OFFSET_COLUMN)
    {
        return fieldInterface_->hasValidOffset(fieldName);
    }
    else if (index.column() == RegisterColumns::WIDTH_COLUMN)
    {
        return fieldInterface_->hasValidWidth(fieldName);
    }
    else if (index.column() == RegisterColumns::RESETS_COLUMN)
    {
        return fieldInterface_->hasValidResets(fieldName);
    }
    else if (index.column() == RegisterColumns::IS_PRESENT_COLUMN)
    {
        return fieldInterface_->hasValidIsPresent(fieldName);
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        return fieldInterface_->hasValidAccess(fieldName, accessPolicyIndex);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int RegisterTableModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return fieldInterface_->getAllReferencesToIdInItem(
        fieldInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::onAddItem()
//-----------------------------------------------------------------------------
void RegisterTableModel::onAddItem( const QModelIndex& index )
{
    int row = fieldInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);

    fieldInterface_->addField(row);

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

    int itemRow = index.row();

	// make sure the row number if valid
    if (itemRow < 0 || itemRow >= fieldInterface_->itemCount())
    {
		return;
	}

    std::string fieldName = fieldInterface_->getIndexedItemName(itemRow);

	// remove the specified item
    beginRemoveRows(QModelIndex(), itemRow, itemRow);

    decreaseReferencesWithRemovedField(QString::fromStdString(fieldName));
    fieldInterface_->removeField(fieldName);

    endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fieldRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::decreaseReferencesWithRemovedField()
//-----------------------------------------------------------------------------
void RegisterTableModel::decreaseReferencesWithRemovedField(QString const& fieldName)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(fieldName);

    foreach(QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void RegisterTableModel::increaseReferencesInPastedField(QString const& fieldName)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(fieldName);

    foreach(QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit increaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: registertablemodel::getReferencedParameters()
//-----------------------------------------------------------------------------
QMap<QString, int> RegisterTableModel::getReferencedParameters(QString const& fieldName) const
{
    std::vector<std::string> fieldNameList;
    fieldNameList.push_back(fieldName.toStdString());

    auto expressionList = fieldInterface_->getExpressionsInSelectedFields(fieldNameList);
    QStringList expressionListQT;
    for (auto expression : expressionList)
    {
        expressionListQT.append(QString::fromStdString(expression));
    }

    ReferenceCalculator memoryMapReferenceCalculator(getParameterFinder());

    return memoryMapReferenceCalculator.getReferencedParameters(expressionListQT);
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::isStd14Column()
//-----------------------------------------------------------------------------
bool RegisterTableModel::isStd14Column(QModelIndex const& index) const
{
    int col = index.column();
    return col == RegisterColumns::ACCESS_COLUMN ||
        col == RegisterColumns::MOD_WRITE_COLUMN ||
        col == RegisterColumns::READ_ACTION_COLUMN ||
        col == RegisterColumns::TESTABLE_COLUMN ||
        col == RegisterColumns::TEST_CONSTR_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::getIndexValueByStdRevision()
//-----------------------------------------------------------------------------
QVariant RegisterTableModel::getIndexValueByStdRevision(QModelIndex const& index) const
{
    if (isStd14Column(index) && docRevision_ == Document::Revision::Std22)
    {
        // Item is not editable, if there is more than one field access policy.
        if (index.flags() == (Qt::ItemIsEnabled | Qt::ItemIsSelectable))
        {
            // Indicate multiple access policies.
            if (index.column() == RegisterColumns::ACCESS_COLUMN)
            {
                return QStringLiteral("[multiple]");
            }

            return QVariant();
        }

        // Return the index value of the first and only field access policy.
        return valueForIndex(index);
    }
    else
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
        }
        else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN &&
            index.flags() == (Qt::ItemIsEnabled | Qt::ItemIsSelectable))
        {
            // Unset test constraint, if testable is false or not set.
            return QVariant();
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

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::setStd14ColumnData()
//-----------------------------------------------------------------------------
bool RegisterTableModel::setStd14ColumnData(QModelIndex const& index, QVariant const& value, std::string const& fieldName)
{
    // Set either directly the field's values, or the values of the first field access policy, depending on
    // std revision.

    // Modify field values directly.
    int fieldAccessPolicyIndex = -1;

    if (docRevision_ == Document::Revision::Std22)
    {
        // Add a new field access policy, if there are none AND if the new index value is not empty.
        if (fieldInterface_->getAccessPolicyCount(fieldName) == 0 && value.toString().isEmpty() == false)
        {
            fieldInterface_->addFieldAccessPolicy(fieldName);
        }

        // Modify first field access policy.
        fieldAccessPolicyIndex = 0;
    }

    if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        fieldInterface_->setAccess(fieldName, value.toString().toStdString(), fieldAccessPolicyIndex);

        // Remove field access policy, if new access is empty.
        if (value.toString().isEmpty())
        {
            fieldInterface_->removeFieldAccessPolicy(fieldName, 0);
        }
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        fieldInterface_->setModifiedWrite(fieldName, value.toString().toStdString(), fieldAccessPolicyIndex);
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        fieldInterface_->setReadAction(fieldName, value.toString().toStdString(), fieldAccessPolicyIndex);
    }
    else if (index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        fieldInterface_->setTestable(fieldName, value.toString().toStdString(), fieldAccessPolicyIndex);
        if (value.toString() == QLatin1String("false"))
        {
            QModelIndex constrIndex = createIndex(index.row(), index.column() + 1, index.internalPointer());
            emit dataChanged(constrIndex, constrIndex);
        }
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        fieldInterface_->setTestConstraint(fieldName, value.toString().toStdString(), fieldAccessPolicyIndex);
    }
    else
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::onCopyRows()
//-----------------------------------------------------------------------------
void RegisterTableModel::onCopyRows(QModelIndexList indexList)
{
    std::vector<int> fieldIndexes;
    for (auto index : indexList)
    {
        fieldIndexes.push_back(index.row());
    }

    fieldInterface_->copyRows(fieldIndexes);
}

//-----------------------------------------------------------------------------
// Function: RegisterTableModel::onPasteRows()
//-----------------------------------------------------------------------------
void RegisterTableModel::onPasteRows()
{
    int fieldRowBegin = fieldInterface_->itemCount();

    std::vector<std::string> pastedFieldNames = fieldInterface_->pasteRows();

    int fieldCount = static_cast<int>(pastedFieldNames.size());
    if (fieldCount == 0)
    {
        return;
    }

    if (fieldCount > 0)
    {
        int rowEnd = fieldRowBegin + fieldCount - 1;

        beginInsertRows(QModelIndex(), fieldRowBegin, rowEnd);

        for (int i = fieldRowBegin; i <= rowEnd; ++i)
        {
            std::string copyName = fieldInterface_->getIndexedItemName(i);
            increaseReferencesInPastedField(QString::fromStdString(copyName));

            emit fieldAdded(i);
        }

        endInsertRows();
    }

    emit contentChanged();
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
