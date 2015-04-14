//-----------------------------------------------------------------------------
// File: RemapConditionModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Table model that can be used to display remap conditions to be edited.
//-----------------------------------------------------------------------------

#include "RemapConditionModel.h"
#include "RemapConditionColumns.h"

#include <QColor>

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::RemapConditionModel()
//-----------------------------------------------------------------------------
RemapConditionModel::RemapConditionModel(QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPorts,
    QList<QSharedPointer<Port> > componentPorts, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject* parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
remapPortsOfRemapState_(new QList<QSharedPointer<RemapPort> > ()),
remapPortsVisibleInModel_(new QList<QSharedPointer<RemapPort> > ()),
expressionFormatter_(expressionFormatter),
componentPorts_(componentPorts)
{
    remapPortsOfRemapState_ = remapPorts;

    setExpressionParser(expressionParser);

    setupVisibleRemapPorts();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::~RemapConditionModel()
//-----------------------------------------------------------------------------
RemapConditionModel::~RemapConditionModel()
{

}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::setupVisibleRemapPorts()
//-----------------------------------------------------------------------------
void RemapConditionModel::setupVisibleRemapPorts()
{
    foreach (QSharedPointer<RemapPort> remapStatePortPointer, *remapPortsOfRemapState_)
    {
        int arrayIndex = getArrayStartIndex(remapStatePortPointer->getPortNameRef());
        arrayIndex = remapStatePortPointer->getPortIndex() - arrayIndex; 

        if (!remapPortIsAlreadyVisible(remapStatePortPointer, arrayIndex))
        {
            QSharedPointer<RemapPort> newRemapPort (new RemapPort(*(remapStatePortPointer)));

            if (arrayIndex > -1)
            {
                QString newValue = newRemapPort->getValue();

                for (int i = 0; i < arrayIndex; ++i)
                {
                    newValue.prepend(',');
                }

                newValue.prepend('{');
                newValue.append(getEndOfArray(newRemapPort->getPortNameRef(), arrayIndex));

                newRemapPort->setValue(newValue);
            }

            remapPortsVisibleInModel_->append(newRemapPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::getArrayStartIndex()
//-----------------------------------------------------------------------------
int RemapConditionModel::getArrayStartIndex(QString const& remapPortName)
{
    foreach (QSharedPointer<Port> targetPort, componentPorts_)
    {
        if (targetPort->getName() == remapPortName)
        {
            int arrayLeft = formattedValueFor(targetPort->getLeftBoundExpression()).toInt();
            int arrayRight = formattedValueFor(targetPort->getRightBoundExpression()).toInt();

            if (arrayLeft < arrayRight)
            {
                return arrayLeft;
            }
            else
            {
                return arrayRight;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::remapPortIsAlreadyVisible()
//-----------------------------------------------------------------------------
bool RemapConditionModel::remapPortIsAlreadyVisible(QSharedPointer<RemapPort> remapStateRemapPort, int arrayIndex)
{
    foreach (QSharedPointer<RemapPort> modelPortPointer, *remapPortsVisibleInModel_)
    {
        if (modelPortPointer->getPortNameRef() == remapStateRemapPort->getPortNameRef())
        {
            QStringList remapPortValueList = modelPortPointer->getValue().split(',');
            remapPortValueList.first() = remapPortValueList.first().remove('{');
            remapPortValueList.last() = remapPortValueList.last().remove('}');
            remapPortValueList[arrayIndex] = remapStateRemapPort->getValue();

            QString newValue = remapPortValueList.join(',');
            newValue.prepend('{');
            newValue.append('}');

            modelPortPointer->setValue(newValue);

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::getEndOfArray()
//-----------------------------------------------------------------------------
QString RemapConditionModel::getEndOfArray(QString const& remapPortName, int newPortIndex)
{
    foreach (QSharedPointer<Port> targetPort, componentPorts_)
    {
        if (targetPort->getName() == remapPortName)
        {
            QString leftBound = parseExpressionToDecimal(targetPort->getLeftBoundExpression());
            QString rightBound = parseExpressionToDecimal(targetPort->getRightBoundExpression());

            int portWidth = abs(leftBound.toInt() - rightBound.toInt()) + 1;
            int commasLeft = portWidth - newPortIndex - 1;

            QString endOfArray('}');
            for (int i = 0; i < commasLeft; ++i)
            {
                endOfArray.prepend(',');
            }

            return endOfArray;
        }
    }

    return QString('}');
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::rowCount()
//-----------------------------------------------------------------------------
int RemapConditionModel::rowCount(QModelIndex const& parent /* = QModelIndex() */) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return remapPortsVisibleInModel_->size();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::columnCount()
//-----------------------------------------------------------------------------
int RemapConditionModel::columnCount(QModelIndex const& parent /* = QModelIndex() */) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return RemapConditionColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::data()
//-----------------------------------------------------------------------------
QVariant RemapConditionModel::data(QModelIndex const& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }

    else if (role == Qt::EditRole)
    {
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
            return expressionOrValueForIndex(index);
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == RemapConditionColumns::LEFT_COLUMN ||
            index.column() == RemapConditionColumns::RIGHT_COLUMN)
        {
            return QColor(Qt::darkGray);
        }
        else
        {
            return blackForValidOrRedForInvalidIndex(index);
        }
    }

    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == RemapConditionColumns::NAME_COLUMN ||
            index.column() == RemapConditionColumns::VALUE_COLUMN)
        {
            return QColor("lemonChiffon");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool RemapConditionModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == RemapConditionColumns::LEFT_COLUMN ||
        index.column() == RemapConditionColumns::RIGHT_COLUMN ||
        index.column() == RemapConditionColumns::VALUE_COLUMN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RemapConditionModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RemapPort> remapPort = remapPortsVisibleInModel_->at(index.row());

    if (index.column() == RemapConditionColumns::NAME_COLUMN)
    {
        return remapPort->getPortNameRef();
    }
    else if (index.column() == RemapConditionColumns::LEFT_COLUMN)
    {
        if (!remapPort->getPortNameRef().isEmpty())
        {
            foreach (QSharedPointer<Port> targetPort, componentPorts_)
            {
                if (targetPort->getName() == remapPort->getPortNameRef())
                {
                    return(targetPort->getLeftBoundExpression());
                }
            }
        }
    }
    else if (index.column() == RemapConditionColumns::RIGHT_COLUMN)
    {
        if (!remapPort->getPortNameRef().isEmpty())
        {
            foreach (QSharedPointer<Port> targetPort, componentPorts_)
            {
                if (targetPort->getName() == remapPort->getPortNameRef())
                {
                    return targetPort->getRightBoundExpression();
                }
            }
        }
    }
    else if (index.column() == RemapConditionColumns::VALUE_COLUMN)
    {
        return remapPort->getValue();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant RemapConditionModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::headerData()
//-----------------------------------------------------------------------------
QVariant RemapConditionModel::headerData(int section, Qt::Orientation orientation,
    int role /* = Qt::DisplayRole */) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == RemapConditionColumns::NAME_COLUMN)
        {
            return tr("Port");
        }
        else if (section == RemapConditionColumns::LEFT_COLUMN)
        {
            return tr("Left bound");
        }
        else if (section == RemapConditionColumns::RIGHT_COLUMN)
        {
            return tr("Right bound");
        }
        else if (section == RemapConditionColumns::VALUE_COLUMN)
        {
            QString valueHeader = tr("Value") + getExpressionSymbol();
            return valueHeader;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::setData()
//-----------------------------------------------------------------------------
bool RemapConditionModel::setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<RemapPort> remapPort = remapPortsVisibleInModel_->at(index.row());

        if (index.column() == RemapConditionColumns::NAME_COLUMN)
        {
            remapPort->setPortNameRef(value.toString());
            emit(newAvailablePorts(getAvailablePorts()));
        }

        else if (index.column() == RemapConditionColumns::VALUE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(remapPort->getValue());
            }

            remapPort->setValue(value.toString());
        }

        else
        {
            return false;
        }

        save();
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
// Function: RemapConditionModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RemapConditionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    else if (index.column() == RemapConditionColumns::LEFT_COLUMN ||
        index.column() == RemapConditionColumns::RIGHT_COLUMN)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::isValid()
//-----------------------------------------------------------------------------
bool RemapConditionModel::isValid() const
{
    foreach (QSharedPointer<RemapPort> remapPort, *remapPortsOfRemapState_)
    {
        if (remapPort->getValue().isEmpty() || remapPort->getPortNameRef().isEmpty() ||
            remapPort->getPortIndex() < -1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::onAddItem()
//-----------------------------------------------------------------------------
void RemapConditionModel::onAddItem(QModelIndex const& index)
{
    int row = remapPortsVisibleInModel_->size();
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    remapPortsVisibleInModel_->insert(row, QSharedPointer<RemapPort>(new RemapPort()));
    endInsertRows();

    save();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::onRemoveItem()
//-----------------------------------------------------------------------------
void RemapConditionModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || index.row() <0 || index.row() >= rowCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());

    remapPortsVisibleInModel_->removeAt(index.row());
    endRemoveRows();

    save();

    emit contentChanged();
    emit newAvailablePorts(getAvailablePorts());
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::save()
//-----------------------------------------------------------------------------
void RemapConditionModel::save()
{
    remapPortsOfRemapState_->clear();

    foreach (QSharedPointer<RemapPort> visibleRemapPort, *remapPortsVisibleInModel_)
    {
        if (visibleRemapPort->getValue().contains('{') && visibleRemapPort->getValue().contains('}'))
        {
            QStringList portValues = visibleRemapPort->getValue().split(',');
            portValues.first().remove('{');
            portValues.last().remove('}');

            int arrayStartIndex = getArrayStartIndex(visibleRemapPort->getPortNameRef());

            for (int i = 0; i < portValues.size(); ++i)
            {
                if (!portValues.at(i).isEmpty())
                {
                    QSharedPointer<RemapPort> newRemapPort (new RemapPort());
                    newRemapPort->setPortNameRef(visibleRemapPort->getPortNameRef());
                    newRemapPort->setValue(portValues.at(i));
                    newRemapPort->setPortIndex(i + arrayStartIndex);

                    remapPortsOfRemapState_->append(newRemapPort);
                }
            }
        }
        else
        {
            QSharedPointer<RemapPort> newRemapPort (new RemapPort());
            newRemapPort->setPortNameRef(visibleRemapPort->getPortNameRef());
            newRemapPort->setValue(visibleRemapPort->getValue());

            remapPortsOfRemapState_->append(newRemapPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int RemapConditionModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    int referencesInValue = remapPortsVisibleInModel_->at(row)->getValue().count(valueID);

    return referencesInValue;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool RemapConditionModel::validateColumnForParameter(QModelIndex const& index) const
{
    QSharedPointer<RemapPort> targetRemapPort = remapPortsVisibleInModel_->at(index.row());

    if (index.column() == RemapConditionColumns::NAME_COLUMN)
    {
        foreach (QSharedPointer<Port> targetPort, componentPorts_)
        {
            if (targetPort->getName() == targetRemapPort->getPortNameRef())
            {
                return true;
            }
        }

        return false;
    }

    else if (index.column() == RemapConditionColumns::VALUE_COLUMN)
    {
        QString targetValue = targetRemapPort->getValue();

        if (targetValue.contains('{') || targetValue.contains('}'))
        {
            QModelIndex portLeftIndex = index.sibling(index.row(), RemapConditionColumns::LEFT_COLUMN);
            int portLeft = portLeftIndex.data(Qt::ToolTipRole).toInt();

            QModelIndex portRightIndex = index.sibling(index.row(), RemapConditionColumns::RIGHT_COLUMN);
            int portRight = portRightIndex.data(Qt::ToolTipRole).toInt();

            int width = abs(portLeft - portRight) + 1;

            if (width != targetValue.count(',') + 1)
            {
                return false;
            }

            QStringList listOfValues = targetValue.split(',');
            listOfValues.first().remove('{');
            listOfValues.last().remove('}');

            foreach (QString value, listOfValues)
            {
                if (!isValuePlainOrExpression(value))
                {
                    return false;
                }
            }
        }

        else if (!isValuePlainOrExpression(targetValue))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::updatePorts()
//-----------------------------------------------------------------------------
void RemapConditionModel::updatePorts(QList<QSharedPointer<Port> > newPorts)
{
    componentPorts_ = newPorts;

    emit(newAvailablePorts(getAvailablePorts()));
}

//-----------------------------------------------------------------------------
// Function: RemapConditionModel::getAvailablePorts()
//-----------------------------------------------------------------------------
QStringList RemapConditionModel::getAvailablePorts()
{
    QStringList usedPorts;

    foreach (QSharedPointer<RemapPort> visibleRemapPort, *remapPortsVisibleInModel_)
    {
        usedPorts.append(visibleRemapPort->getPortNameRef());
    }

    QStringList availablePorts;

    foreach (QSharedPointer<Port> targetPort, componentPorts_)
    {
        if (!usedPorts.contains(targetPort->getName()))
        {
            availablePorts.append(targetPort->getName());
        }
    }

    return availablePorts;
}
