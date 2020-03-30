//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#include "AbstractParameterModel.h"

#include <editors/ComponentEditor/parameters/ParametersInterface.h>

#include <common/KactusColors.h>

#include <QFont>
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::AbstractParameterModel(QSharedPointer<ParametersInterface> parameterInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
parameterInterface_(parameterInterface)
{
    setExpressionParser(expressionParser);
    parameterFinder->registerParameterModel(this);
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractParameterModel::rowCount(QModelIndex const& parent) const
{
    // not hierarchical model
    if (parent.isValid())
    {
        return 0;
    }

    return parameterInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::data( QModelIndex const& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }
    
    if (role == Qt::DisplayRole)
    {
        if (index.column() == valueColumn() || 
            index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn() ||
            index.column() == arrayLeftColumn() || index.column() == arrayRightColumn())
        {
            return formattedExpressionForIndex(index);
        }
        else if (index.column() == descriptionColumn())
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$",
                QRegularExpression::DotMatchesEverythingOption), QStringLiteral("..."));
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionForIndex(index);
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::BackgroundRole) 
    {
        return backgroundColorForIndex(index);
    }
    else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }
    else // if unsupported role
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Vertical)
    {
        return QString::fromStdString(parameterInterface_->getIndexedItemName(section));
    }

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
    {
        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == displayNameColumn())
        {
            return tr("Display\nname");
        }
        else if (section == typeColumn())
        {
            return tr("Type");   
        }
        else if (section == bitWidthLeftColumn())
        {
            QString bitWidthLeft = tr("Bit vector\nleft") + getExpressionSymbol();
            return bitWidthLeft;
        }
        else if (section == bitWidthRightColumn())
        {
            QString bitWidthRight = tr("Bit vector\nright") + getExpressionSymbol();
            return bitWidthRight;
        }
        else if (section == minimumColumn())
        {
            return tr("Min");
        }
        else if (section == maximumColumn())
        {
            return tr("Max");
        }
        else if (section == choiceColumn())
        {
            return tr("Choice");
        }     
        else if (section == valueColumn())
        {
            QString valueHeader = tr("Value") + getExpressionSymbol();
            return valueHeader;
        }  
        else if (section == resolveColumn())
        {     
            return tr("Resolve");
        }  
        else if (section == arrayLeftColumn())
        {
            QString arrayLeftHeader = tr("Array\nleft") + getExpressionSymbol();
            return arrayLeftHeader;
        }  
        else if (section == arrayRightColumn())
        {
            QString arrayRightHeader = tr("Array\nright") + getExpressionSymbol();
            return arrayRightHeader;
        } 
        else if (section == descriptionColumn())
        { 
            return tr("Description");
        }
        else if (section == idColumn())
        {
            return tr("Parameter id");
        }
        else if (section == usageCountColumn())
        {
            return tr("Usage\ncount");
        }

        else
        {
            return QVariant();
        }
    }

    // if unsupported role
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::setData()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::setData(QModelIndex const& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    if (role == Qt::EditRole)
    {
        std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

        if (index.column() == nameColumn())
        {
            parameterInterface_->setName(parameterName, value.toString().toStdString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
        }
        else if (index.column() == displayNameColumn())
        {
            parameterInterface_->setDisplayName(parameterName, value.toString().toStdString());
        }
        else if (index.column() == typeColumn())
        {
            parameterInterface_->setType(parameterName, value.toString().toStdString());
        }
        else if (index.column() == bitWidthLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(parameterInterface_->getBitWidthLeftExpression(parameterName)));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameterInterface_->setBitWidthLeft(parameterName, value.toString().toStdString());

            if (value.isValid() && parameterInterface_->getBitWidthRightExpression(parameterName).empty() &&
                !value.toString().isEmpty())
            {
                parameterInterface_->setBitWidthRight(parameterName, "0");
            }
        }
        else if (index.column() == bitWidthRightColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(parameterInterface_->getBitWidthRightExpression(parameterName)));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameterInterface_->setBitWidthRight(parameterName, value.toString().toStdString());

            if (value.isValid() && parameterInterface_->getBitWidthLeftExpression(parameterName).empty() &&
                !value.toString().isEmpty())
            {
                parameterInterface_->setBitWidthLeft(parameterName, "0");
            }
        }
        else if (index.column() == minimumColumn())
        {
            parameterInterface_->setMinimum(parameterName, value.toString().toStdString());
        }
        else if (index.column() == maximumColumn())
        {
            parameterInterface_->setMaximum(parameterName, value.toString().toStdString());
        }
        else if (index.column() == choiceColumn())
        {
            parameterInterface_->setChoice(parameterName, value.toString().toStdString());
        }
        else if (index.column() == valueColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(parameterInterface_->getValue(parameterName)));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameterInterface_->setValue(parameterName, value.toString().toStdString());
        }
        else if (index.column() == resolveColumn())
        {
            parameterInterface_->setResolve(parameterName, value.toString().toStdString());
        }
        else if (index.column() == arrayLeftColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(parameterInterface_->getArrayLeftExpression(parameterName)));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameterInterface_->setArrayLeft(parameterName, value.toString().toStdString());

            if (value.isValid() && parameterInterface_->getArrayRightExpression(parameterName).empty() &&
                !parameterInterface_->getArrayLeftExpression(parameterName).empty())
            {
                parameterInterface_->setArrayRight(parameterName, "0");
            }
        }
        else if (index.column() == arrayRightColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(parameterInterface_->getArrayRightExpression(parameterName)));

                emit dataChanged(QAbstractTableModel::index(0, usageCountColumn()),
                    QAbstractTableModel::index(rowCount() - 1, usageCountColumn()));
            }

            parameterInterface_->setArrayRight(parameterName, value.toString().toStdString());

            if (value.isValid() && parameterInterface_->getArrayLeftExpression(parameterName).empty() &&
                !parameterInterface_->getArrayRightExpression(parameterName).empty())
            {
                parameterInterface_->setArrayLeft(parameterName, "0");
            }
        }
        else if (index.column() == descriptionColumn())
        {
            parameterInterface_->setDescription(parameterName, value.toString().toStdString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else // is unsupported role
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractParameterModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isMandatoryColumn()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isMandatoryColumn(int column) const
{
    return column == nameColumn() || column == valueColumn();
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::validateIndex()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::validateIndex(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

    if (index.column() == nameColumn())
    {
        return parameterInterface_->itemHasValidName(parameterName);
    }
    else if (index.column() == typeColumn())
    {
        return parameterInterface_->hasValidType(parameterName);
    }
    else if (index.column() == bitWidthLeftColumn())
    {
        return parameterInterface_->hasValidBitWidth(parameterName);
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return parameterInterface_->hasValidBitWidth(parameterName);
    }
    else if (index.column() == minimumColumn())
    {
        return parameterInterface_->hasValidMinimum(parameterName);
    }
    else if (index.column() == maximumColumn())
    {
        return parameterInterface_->hasValidMaximum(parameterName);
    }
    else if (index.column() == choiceColumn())
    {
        return parameterInterface_->hasValidChoice(parameterName);
    }
    else if (index.column() == valueColumn())
    {
        return parameterInterface_->hasValidValue(parameterName);
    }
    else if (index.column() == resolveColumn())
    {
        return parameterInterface_->hasValidResolve(parameterName);
    }
    else if (index.column() == arrayLeftColumn())
    {
        return parameterInterface_->hasValidArrayValues(parameterName);
    }
    else if (index.column() == arrayRightColumn())
    {
        return parameterInterface_->hasValidArrayValues(parameterName);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValidExpressionColumn(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

    if ((index.column() == valueColumn() && parameterInterface_->getType(parameterName) != "string") ||
        index.column() == arrayLeftColumn() || index.column() == arrayRightColumn() ||
        index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

    if (index.column() == bitWidthLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthLeftFormattedExpression(parameterName));
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthRightFormattedExpression(parameterName));
    }
    else if (index.column() == valueColumn())
    {
        return QString::fromStdString(parameterInterface_->getValueFormattedExpression(parameterName));
    }
    else if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayLeftFormattedExpression(parameterName));
    }
    else if (index.column() == arrayRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayRightFormattedExpression(parameterName));
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::expressionForIndex(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

    if (index.column() == bitWidthLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthLeftExpression(parameterName));
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthRightExpression(parameterName));
    }
    else if (index.column() == valueColumn())
    {
        return QString::fromStdString(parameterInterface_->getValueExpression(parameterName));
    }
    else if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayLeftExpression(parameterName));
    }
    else if (index.column() == arrayRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayRightExpression(parameterName));
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::valueForIndex(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());

    if (index.column() == nameColumn())
    {
        return QString::fromStdString(parameterName);
    }
    else if (index.column() == displayNameColumn())
    {
        return QString::fromStdString(parameterInterface_->getDisplayName(parameterName));
    }
    else if (index.column() == typeColumn())
    {
        return QString::fromStdString(parameterInterface_->getType(parameterName));
    }
    else if (index.column() == bitWidthLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthLeftValue(parameterName));
    }
    else if (index.column() == bitWidthRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getBitWidthRightValue(parameterName));
    }
    else if (index.column() == minimumColumn())
    {
        return QString::fromStdString(parameterInterface_->getMinimum(parameterName));
    }
    else if (index.column() == maximumColumn())
    {
        return QString::fromStdString(parameterInterface_->getMaximum(parameterName));
    }
    else if (index.column() == choiceColumn())
    {
        return QString::fromStdString(parameterInterface_->getChoice(parameterName));
    }
    else if (index.column() == valueColumn())
    {
        return QString::fromStdString(parameterInterface_->getValue(parameterName));
    }
    else if (index.column() == resolveColumn())
    {
        return QString::fromStdString(parameterInterface_->getResolve(parameterName));
    }
    else if (index.column() == arrayLeftColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayLeftValue(parameterName));
    }
    else if (index.column() == arrayRightColumn())
    {
        return QString::fromStdString(parameterInterface_->getArrayRightValue(parameterName));
    }
    else if (index.column() == descriptionColumn())
    {
        return QString::fromStdString(parameterInterface_->getDescription(parameterName));
    }
    else if (index.column() == idColumn())
    {
        return QString::fromStdString(parameterInterface_->getID(parameterName));
    }
    else if (index.column() == usageCountColumn())
    {
        return parameterInterface_->getUsageCount(parameterName);
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::canRemoveRow()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::canRemoveRow(int const& row) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(row);
    int parameterUsageCount = parameterInterface_->getUsageCount(parameterName);

    if (parameterUsageCount > 0)
    {
        QMessageBox removeWarning;
        removeWarning.setText("Are you sure you want to remove " + QString::fromStdString(parameterName) +
            "? There are " + QString::number(parameterUsageCount) + " references to it. \n\nTo see where " +
            QString::fromStdString(parameterName) + " has been referenced, double click its usage count.");
        removeWarning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        removeWarning.setDefaultButton(QMessageBox::No);
        removeWarning.setIcon(QMessageBox::Warning);

        QApplication::setOverrideCursor(Qt::ArrowCursor);

        if (removeWarning.exec() == QMessageBox::No)
        {
            QApplication::restoreOverrideCursor();
            return false;
        }

        QApplication::restoreOverrideCursor();
    }

    removeReferencesInItemOnRow(row);

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::backgroundColorForIndex()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::backgroundColorForIndex(QModelIndex const& index) const
{
    std::string parameterName = parameterInterface_->getIndexedItemName(index.row());
    std::string parameterType = parameterInterface_->getType(parameterName);

    if (isMandatoryColumn(index.column())) 
    {
        return KactusColors::MANDATORY_FIELD;
    }
    else if (((index.column() == minimumColumn() || index.column() == maximumColumn()) &&
        (parameterType.empty() || parameterType == "bit" || parameterType == "string")) ||
        ((index.column() == bitWidthLeftColumn() || index.column() == bitWidthRightColumn()) &&
            parameterType != "bit"))
    {
        return KactusColors::DISABLED_FIELD;
    }
    else
    {
        return KactusColors::REGULAR_FIELD;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::getAllReferencesToIdInItemRow()
//-----------------------------------------------------------------------------
int AbstractParameterModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return parameterInterface_->getAllReferencesToIdInItem(
        parameterInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::onGetParametersMachingIndexes()
//-----------------------------------------------------------------------------
void AbstractParameterModel::onGetParametersMachingIndexes(QModelIndexList indexes)
{
    QVector<QString> parameterNames;
    for (auto index : indexes)
    {
        std::string indexedName = parameterInterface_->getIndexedItemName(index.row());
        parameterNames.append(QString::fromStdString(indexedName));
    }

    beginResetModel();

    emit recalculateReferencesToParameters(parameterNames, parameterInterface_);

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ParametersInterface> AbstractParameterModel::getInterface() const
{
    return parameterInterface_;
}
