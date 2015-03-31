//-----------------------------------------------------------------------------
// File: ParameterArrayModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.03.2015
//
// Description:
// Model for editing an array within parameter value.
//-----------------------------------------------------------------------------

#include "ParameterArrayModel.h"
#include "ArrayColumns.h"

#include <IPXACTmodels/Enumeration.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::ParameterArrayModel()
//-----------------------------------------------------------------------------
ParameterArrayModel::ParameterArrayModel(int sizeOfArray, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<Choice> selectedChoice, QObject* parent /* = 0 */):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
sizeOfArray_(sizeOfArray),
expressionformatter_(expressionFormatter),
arrayValues_(),
selectedChoice_(selectedChoice),
validator_(new ParameterValidator2014(expressionParser, parameterFinder)),
parameterType_()
{
    QString repeater = ",";
    QString newArray = repeater.repeated(sizeOfArray_ - 1);
    arrayValues_ = newArray.split(repeater);

    QString arrayValue;
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::~ParameterArrayModel()
//-----------------------------------------------------------------------------
ParameterArrayModel::~ParameterArrayModel()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::columnCount()
//-----------------------------------------------------------------------------
int ParameterArrayModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ArrayColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::rowCount()
//-----------------------------------------------------------------------------
int ParameterArrayModel::rowCount(const QModelIndex &parent /* = QModelIndex() */) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return sizeOfArray_;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::headerData()
//-----------------------------------------------------------------------------
QVariant ParameterArrayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == ArrayColumns::ARRAY_INDEX)
        {
            return tr("Index");
        }
        else if (section == ArrayColumns::VALUE)
        {
            return tr("Value") + getExpressionSymbol();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::data()
//-----------------------------------------------------------------------------
QVariant ParameterArrayModel::data(const QModelIndex &index, int role) const
{
    if (indexIsNotValid(index))
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ArrayColumns::VALUE)
        {
            return expressionformatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }

    else if (role == Qt::UserRole)
    {
        if (index.row() == sizeOfArray_ - 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == ArrayColumns::VALUE)
        {
            if (!selectedChoice_->getName().isEmpty())
            {
                return expressionOrValueForIndex(index);
            }
            else
            {
                return formattedValueFor(valueForIndex(index).toString());
            }
        }
        else
        {
            return valueForIndex(index);
        }
    }

    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ArrayColumns::VALUE)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        if (isValidExpressionColumn(index))
        {
            return blackForValidOrRedForInvalidIndex(index);
        }

        return QVariant();
    }

    else if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == ArrayColumns::ARRAY_INDEX)
        {
            //return Qt::AlignRight | Qt::AlignVCenter;
            return Qt::AlignCenter;
        }

        else return QVariant();
    }

    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::setData()
//-----------------------------------------------------------------------------
bool ParameterArrayModel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */)
{
    if (indexIsNotValid(index))
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == ArrayColumns::ARRAY_INDEX)
        {
            return false;
        }
        else if (index.column() == ArrayColumns::VALUE)
        {
            arrayValues_.replace(index.row(), value.toString());
        }
    }

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ParameterArrayModel::flags(QModelIndex const& index) const
{
    if (indexIsNotValid(index) || index.column() == ArrayColumns::ARRAY_INDEX)
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::isNotValidIndex()
//-----------------------------------------------------------------------------
bool ParameterArrayModel::indexIsNotValid(QModelIndex const& index) const
{
    return !index.isValid() || index.row() > sizeOfArray_ || index.column() >= ArrayColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::()
//-----------------------------------------------------------------------------
bool ParameterArrayModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == ArrayColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant ParameterArrayModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == ArrayColumns::VALUE)
    {
        return arrayValues_.at(index.row());
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int ParameterArrayModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    return arrayValues_.at(row).count(valueID);
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool ParameterArrayModel::validateColumnForParameter(QModelIndex const& index) const
{
    if (index.column() == ArrayColumns::VALUE)
    {
        //return isValuePlainOrExpression(arrayValues_.at(index.row()));
        return validator_->hasValidValueForType(arrayValues_.at(index.row()), parameterType_);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant ParameterArrayModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == ArrayColumns::ARRAY_INDEX)
    {
        return index.row();
    }
    else if (index.column() == ArrayColumns::VALUE)
    {
        return evaluateValueWithChoice(index.row());
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::evaluateValueWithChoice()
//-----------------------------------------------------------------------------
QString ParameterArrayModel::evaluateValueWithChoice(const int& row) const
{
    if (!selectedChoice_->getName().isEmpty())
    {
        foreach (QSharedPointer<Enumeration> enumeration, *selectedChoice_->enumerations())
        {
            if (enumeration->getValue() == arrayValues_.at(row) && !enumeration->getText().isEmpty())
            {
                return enumeration->getText();
            }
        }
    }

    return arrayValues_.at(row);
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::setArrayData()
//-----------------------------------------------------------------------------
void ParameterArrayModel::setArrayData(QString const& arrayData)
{
    if (arrayData.contains('{') && arrayData.contains('}'))
    {
        QStringList newArrayData = arrayData.split(',');
        newArrayData.first().remove('{');
        newArrayData.last().remove('}');

        int arraySize = arrayValues_.size();
        if (newArrayData.size() < arrayValues_.size())
        {
            arraySize = newArrayData.size();
        }

        for (int i = 0; i < arraySize; ++i)
        {
            arrayValues_[i] = newArrayData.at(i);
        }
    }

    else
    {
        arrayValues_.first() = arrayData;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::getArrayData()
//-----------------------------------------------------------------------------
QString ParameterArrayModel::getArrayData()
{
    arrayValues_.first().prepend('{');
    arrayValues_.last().append('}');
    QString newValue = arrayValues_.join(',');

    return newValue;
}

//-----------------------------------------------------------------------------
// Function: ParameterArrayModel::setParameterType()
//-----------------------------------------------------------------------------
void ParameterArrayModel::setParameterType(QString const& parameterType)
{
    parameterType_ = parameterType;
}