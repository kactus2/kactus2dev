//-----------------------------------------------------------------------------
// File: PortMapTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeModel.h"
#include "PortMapsColumns.h"

#include <library/LibraryInterface.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>

#include <QIcon>
#include <QSize>
#include <QMimeData>

#include <common/KactusColors.h>

namespace
{
    QString const MULTIPLE_SELECTED = "[multiple]";
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::PortMapTreeModel()
//-----------------------------------------------------------------------------
PortMapTreeModel::PortMapTreeModel(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, PortMapInterface* portMapInterface, QObject *parent):
QAbstractItemModel(parent),
ParameterizableTable(parameterFinder),
component_(component),
handler_(handler),
portMapInterface_(portMapInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::~PortMapTreeModel()
//-----------------------------------------------------------------------------
PortMapTreeModel::~PortMapTreeModel()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::columnCount()
//-----------------------------------------------------------------------------
int PortMapTreeModel::columnCount(const QModelIndex&) const
{
    return PortMapsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::rowCount()
//-----------------------------------------------------------------------------
int PortMapTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return portMapInterface_->logicalPortCount();
    }
    else if (!parent.parent().isValid())
    {
        std::string logicalPortName = portMapInterface_->getIndexedItemName(parent.row());
        return portMapInterface_->portMapCount(logicalPortName);
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortMapTreeModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!parent.isValid() && 0 <= row && row < portMapInterface_->logicalPortCount())
    {
        std::string logicalPortName = portMapInterface_->getIndexedItemName(row);
        PortAbstraction* logicalPort = portMapInterface_->getLogicalPortPointer(logicalPortName);
        if (logicalPort)
        {
            QModelIndex logicalIndex = createIndex(row, column, logicalPort);
            return logicalIndex;
        }
    }
    else
    {
        std::string logicalPortName = portMapInterface_->getIndexedItemName(parent.row());
        PortMap* portMap = portMapInterface_->getPortMapPointer(logicalPortName, row);

        if (portMap)
        {
            return createIndex(row, column, portMap);
        }
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::parent()
//-----------------------------------------------------------------------------
QModelIndex PortMapTreeModel::parent(QModelIndex const& child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    for (int portMappingIndex = 0; portMappingIndex < portMapInterface_->logicalPortCount(); ++portMappingIndex)
    {
        std::string logicalPortName = portMapInterface_->getIndexedItemName(portMappingIndex);
        PortAbstraction* logicalPort = portMapInterface_->getLogicalPortPointer(logicalPortName);
        if (logicalPort == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    PortMap* childItem = static_cast<PortMap*>(child.internalPointer());
    return createParentIndexForPortMap(childItem);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::createParentIndexForPortMap()
//-----------------------------------------------------------------------------
QModelIndex PortMapTreeModel::createParentIndexForPortMap(PortMap* childItem) const
{
    for (int i = 0; i < portMapInterface_->logicalPortCount(); ++i)
    {
        std::string logicalPortName = portMapInterface_->getIndexedItemName(i);
        for (int portMapIndex = 0; portMapIndex < portMapInterface_->portMapCount(logicalPortName); ++portMapIndex)
        {
            PortMap* portMap = portMapInterface_->getPortMapPointer(logicalPortName, portMapIndex);
            if (portMap && portMap == childItem)
            {
                PortAbstraction* logicalPort = portMapInterface_->getLogicalPortPointer(logicalPortName);
                if (logicalPort)
                {
                    return createIndex(i, 0, logicalPort);
                }
            }
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == PortMapsColumns::LOGICAL_PORT)
            {
                return tr("Logical port");
            }
            else if (section == PortMapsColumns::LOGICAL_PRESENCE)
            {
                return tr("Requirement");
            }
            else if (section == PortMapsColumns::LOGICAL_LEFT)
            {
                QString logicalLeft = tr("Logical left") + getExpressionSymbol();
                return logicalLeft;
            }
            else if (section == PortMapsColumns::LOGICAL_RIGHT)
            {
                QString logicalRight = tr("Logical right") + getExpressionSymbol();
                return logicalRight;
            }
            else if (section == PortMapsColumns::PHYSICAL_PORT)
            {
                return tr("Physical port");
            }
            else if (section == PortMapsColumns::PHYSICAL_LEFT)
            {
                QString physicalLeft = tr("Physical left") + getExpressionSymbol();
                return physicalLeft;
            }
            else if (section == PortMapsColumns::PHYSICAL_RIGHT)
            {
                QString physicalRight = tr("Physical right") + getExpressionSymbol();
                return physicalRight;
            }
            else if (section == PortMapsColumns::INVERT)
            {
                return tr("Invert");
            }
            else if (section == PortMapsColumns::ISINFORMATIVE)
            {
                return tr("Informative");
            }
            else if (section == PortMapsColumns::TIEOFF)
            {
                QString tieOff = tr("Tie Off") + getExpressionSymbol();
                return tieOff;
            }
        }

        else if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::data()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::data(QModelIndex const& index, int role) const
{
    if (!indexIsValid(index))
    {
        return QVariant();
    }

    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);
    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    if (role == Qt::DisplayRole)
    {
        return formattedExpressionForIndex(index);
    }

    else if (index.parent().isValid() && role == Qt::CheckStateRole &&
        (index.column() == PortMapsColumns::INVERT || index.column() == PortMapsColumns::ISINFORMATIVE))
    {
        if ((index.column() == PortMapsColumns::INVERT &&
                portMapInterface_->getInvertValue(abstractPortName, portMapIndex)) ||
            (index.column() == PortMapsColumns::ISINFORMATIVE &&
                portMapInterface_->getIsInformativeValue(abstractPortName, portMapIndex)))
        {
            return Qt::Checked;
        }
        else
        {
            return Qt::Unchecked;
        }
    }

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == PortMapsColumns::LOGICAL_PRESENCE)
        {
            return KactusColors::DISABLED_TEXT;
        }
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        return getBackgroundColour(index, abstractPortName, portMapIndex);
    }
    else if (role == Qt::DecorationRole)
    {
        if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PORT)
        {
            return QIcon(QString::fromStdString(portMapInterface_->getLogicalPortIconPath(abstractPortName)));
        }
        else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            return QIcon(QString::fromStdString(
                portMapInterface_->getPhysicalPortIconPath(abstractPortName, portMapIndex)));
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(100, 20);
    }
    else if (role == Qt::UserRole + 1)
    {
        return portMapInterface_->logicalPortExists(abstractPortName);
    }

    // Unsupported role.
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getBackgroundColour()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::getBackgroundColour(QModelIndex const& index, std::string const& logicalPortName,
    int const& portMapIndex) const
{
    QString presence = QString::fromStdString(portMapInterface_->getLogicalPresence(logicalPortName));

    if (!index.parent().isValid())
    {
        if (PresenceTypes::str2Presence(presence, PresenceTypes::UNKNOWN) == PresenceTypes::REQUIRED &&
            index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::LOGICAL_PORT_FIELD;
        }
    }
    else
    {
        QString logicalTieOff =
            QString::fromStdString(portMapInterface_->getLogicalTieOffValue(logicalPortName, portMapIndex));
        bool physicalPortExists = portMapInterface_->hasPhysicalPort(logicalPortName, portMapIndex);

        if (index.column() == PortMapsColumns::LOGICAL_PORT ||
            (index.column() == PortMapsColumns::PHYSICAL_PORT && logicalTieOff.isEmpty()) ||
            (index.column() == PortMapsColumns::TIEOFF && !physicalPortExists))
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else if (index.column() == PortMapsColumns::LOGICAL_PRESENCE)
        {
            return KactusColors::DISABLED_FIELD;
        }
    }

    return KactusColors::REGULAR_FIELD;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getPortNamePortMapIndex()
//-----------------------------------------------------------------------------
QPair<std::string, int> PortMapTreeModel::getPortNamePortMapIndex(QModelIndex const& index) const
{
    std::string abstractPortName = "";
    int portMapIndex = -1;

    if (!index.parent().isValid())
    {
        abstractPortName = portMapInterface_->getIndexedItemName(index.row());
    }
    else
    {
        abstractPortName = portMapInterface_->getIndexedItemName(index.parent().row());
        portMapIndex = index.row();
    }

    QPair<std::string, int> combinedID;
    combinedID.first = abstractPortName;
    combinedID.second = portMapIndex;

    return combinedID;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);

    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalLeftBoundFormattedExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalRightBoundFormattedExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return QString::fromStdString(
            portMapInterface_->getPhysicalLeftBoundFormattedExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return QString::fromStdString(
            portMapInterface_->getPhysicalRightBoundFormattedExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalTieOffFormattedExpression(abstractPortName, portMapIndex));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::expressionForIndex(QModelIndex const& index) const
{
    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);

    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalLeftBoundExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalRightBoundExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return QString::fromStdString(
            portMapInterface_->getPhysicalLeftBoundExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return QString::fromStdString(
            portMapInterface_->getPhysicalRightBoundExpression(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalTieOffExpression(abstractPortName, portMapIndex));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::valueForIndex(QModelIndex const& index) const
{
    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);
    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    if (index.column() == PortMapsColumns::LOGICAL_PORT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalPortName(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalPortName(abstractPortName, portMapIndex));
    }
    else if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PRESENCE)
    {
        return QString::fromStdString(portMapInterface_->getLogicalPresence(abstractPortName));
    }
    else if (index.column() == PortMapsColumns::INVERT && !index.parent().isValid() &&
        portMapInterface_->portMapCount(abstractPortName) > 0)
    {
        return QString::fromStdString(portMapInterface_->getInvertString(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::ISINFORMATIVE && !index.parent().isValid() &&
        portMapInterface_->portMapCount(abstractPortName) > 0)
    {
        return QString::fromStdString(portMapInterface_->getIsInformativeString(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::TIEOFF && !index.parent().isValid() &&
        portMapInterface_->portMapCount(abstractPortName) > 0)
    {
        return QString::fromStdString(
            portMapInterface_->getLogicalTieOffFormattedExpression(abstractPortName, portMapIndex));
    }
    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalLeftBoundValue(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return
            QString::fromStdString(portMapInterface_->getLogicalRightBoundValue(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return
            QString::fromStdString(portMapInterface_->getPhysicalLeftBoundValue(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return
            QString::fromStdString(portMapInterface_->getPhysicalRightBoundValue(abstractPortName, portMapIndex));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(portMapInterface_->getLogicalTieOffValue(abstractPortName, portMapIndex));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::setData()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    if (!indexIsValid(index))
    {
        return false;
    }

    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);

    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    if (role == Qt::EditRole)
    {
        if (!index.parent().isValid() && PortMapsColumns::PHYSICAL_PORT &&
            !value.toString().isEmpty() && value.toString().compare(index.data(Qt::DisplayRole).toString()) != 0)
        {
            QModelIndex logicalColumnIndex = index.sibling(index.row(), PortMapsColumns::LOGICAL_PORT);
            int portMapCountInIndex = portMapInterface_->portMapCount(abstractPortName);

            beginInsertRows(logicalColumnIndex, portMapCountInIndex, portMapCountInIndex);

            portMapInterface_->setPhysicalPort(
                abstractPortName, portMapCountInIndex, value.toString().toStdString());

            endInsertRows();

            sendPortConnectionSignal("", value.toString());
        }
        else if (index.parent().isValid())
        {
            if (index.column() == PortMapsColumns::LOGICAL_PORT)
            {
                switchMappedLogicalPort(index, abstractPortName, portMapIndex, value);
            }
            else if (index.column() == PortMapsColumns::LOGICAL_LEFT)
            {
                portMapInterface_->setLogicalLeftBound(
                    abstractPortName, portMapIndex, value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                portMapInterface_->setLogicalRightBound(
                    abstractPortName, portMapIndex, value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
            {
                QString oldPhysicalName =
                    QString::fromStdString(portMapInterface_->getPhysicalPortName(abstractPortName, portMapIndex));
                QString newPhysicalName = value.toString();

                portMapInterface_->setPhysicalPort(abstractPortName, portMapIndex, newPhysicalName.toStdString());
                sendPortConnectionSignal(oldPhysicalName, newPhysicalName);
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
            {
                portMapInterface_->
                    setPhysicalLeftBound(abstractPortName, portMapIndex, value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
            {
                portMapInterface_->
                    setPhysicalRightBound(abstractPortName, portMapIndex, value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::TIEOFF)
            {
                portMapInterface_->
                    setLogicalTieOff(abstractPortName, portMapIndex, value.toString().toStdString());

                QModelIndex logicalTieoffIndex = index.parent().sibling(index.parent().row(), index.column());
                emit dataChanged(logicalTieoffIndex, logicalTieoffIndex);
            }
        }

        emit dataChanged(index, index);
        emit contentChanged();

        return true;
    }
    else if (index.parent().isValid() && role == Qt::CheckStateRole)
    {
        bool checkValue = (value == Qt::Checked);
        if (index.column() == PortMapsColumns::INVERT)
        {
            portMapInterface_->setInvertValue(abstractPortName, portMapIndex, checkValue);
        }
        else if (index.column() == PortMapsColumns::ISINFORMATIVE)
        {
            portMapInterface_->setIsInformativeValue(abstractPortName, portMapIndex, checkValue);
        }

        QModelIndex logicalInvertIndex = index.parent().sibling(index.parent().row(), index.column());

        emit dataChanged(index, index);
        emit dataChanged(logicalInvertIndex, logicalInvertIndex);
        emit contentChanged();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::switchMappedLogicalPort()
//-----------------------------------------------------------------------------
void PortMapTreeModel::switchMappedLogicalPort(QModelIndex const& index, std::string const& logicalPortName,
    int const& portMapIndex, QVariant const& value)
{
    if (index.parent().isValid())
    {
        std::string newLogicalName = value.toString().toStdString();

        QModelIndex sourceParentIndex = index.parent();
        QModelIndex destinationParentIndex = sourceParentIndex.sibling(
            portMapInterface_->mappingIndex(newLogicalName), PortMapsColumns::LOGICAL_PORT);

        int sourceRow = index.row();
        int destinationPortMapCount = portMapInterface_->portMapCount(newLogicalName);

        beginMoveRows(sourceParentIndex, sourceRow, sourceRow, destinationParentIndex,
            destinationPortMapCount);

        portMapInterface_->setLogicalPort(logicalPortName, portMapIndex, newLogicalName);

        endMoveRows();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::sendPortConnectionSignal()
//-----------------------------------------------------------------------------
void PortMapTreeModel::sendPortConnectionSignal(QString const& oldName, QString const& newName)
{
    if (!oldName.isEmpty())
    {
        emit portDisconnected(oldName);
    }

    emit portConnected(newName);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::indexIsValid()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::indexIsValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.row() >= 0 && index.row() < portMapInterface_->logicalPortCount())
        {
            return true;
        }
    }
    else
    {
        QModelIndex parentIndex = index.parent();
        int portMapCount = portMapInterface_->portMapCount(portMapInterface_->getIndexedItemName(parentIndex.row()));
        if (index.row() >= 0 && index.row() < portMapCount)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortMapTreeModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    QModelIndex parentIndex = index;
    if (index.parent().isValid())
    {
        parentIndex = index.parent();
    }

    std::string portName = portMapInterface_->getIndexedItemName(parentIndex.row());
    bool logicalPortExists = portMapInterface_->logicalPortExists(portName);
    if (logicalPortExists)
    {
        itemFlags |= Qt::ItemIsDropEnabled;
    }

    if ((index.column() == PortMapsColumns::INVERT || index.column() == PortMapsColumns::ISINFORMATIVE)
        && index.parent().isValid())
    {
        itemFlags |= Qt::ItemIsUserCheckable;
    }
    else if ((index.column() == PortMapsColumns::PHYSICAL_PORT && logicalPortExists) ||
        (index.parent().isValid() && index.column() != PortMapsColumns::LOGICAL_PRESENCE))
    {
        itemFlags |= Qt::ItemIsEditable;
    }

    return itemFlags;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::reset()
//-----------------------------------------------------------------------------
void PortMapTreeModel::reset()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::refresh()
//-----------------------------------------------------------------------------
void PortMapTreeModel::refresh()
{
    reset();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == PortMapsColumns::LOGICAL_LEFT || index.column() == PortMapsColumns::LOGICAL_RIGHT ||
        index.column() == PortMapsColumns::PHYSICAL_LEFT || index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
        index.column() == PortMapsColumns::TIEOFF;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_LEFT || index.column() == PortMapsColumns::LOGICAL_RIGHT ||
        index.column() == PortMapsColumns::PHYSICAL_LEFT || index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
        index.column() == PortMapsColumns::TIEOFF)
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::validateIndex()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::validateIndex(QModelIndex const& index) const
{
    QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(index);

    std::string abstractPortName = combinedIndex.first;
    int portMapIndex = combinedIndex.second;

    QModelIndex logicalIndex = index;
    if (index.parent().isValid())
    {
        logicalIndex = index.parent();
    }

    portMapInterface_->changeValidatorAbstractionDefinition();
    if (index.column() == PortMapsColumns::LOGICAL_PORT && !portMapInterface_->logicalPortExists(abstractPortName))
    {
        return false;
    }
    else if (!index.parent().isValid() || portMapIndex < 0)
    {
        return true;
    }

    QModelIndex physicalPortIndex = index.sibling(index.row(), PortMapsColumns::PHYSICAL_PORT);
    QString physicalPortName = physicalPortIndex.data(Qt::DisplayRole).toString();
    QSharedPointer<Port> physicalPort = component_->getPort(physicalPortName);

    if (index.column() == PortMapsColumns::LOGICAL_PORT || index.column() == PortMapsColumns::LOGICAL_LEFT ||
        index.column() == PortMapsColumns::LOGICAL_RIGHT || index.column() == PortMapsColumns::PHYSICAL_PORT ||
        index.column() == PortMapsColumns::PHYSICAL_LEFT || index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
        index.column() == PortMapsColumns::TIEOFF)
    {
        if (portMapInterface_->logicalPortIsValid(abstractPortName, portMapIndex))
        {
            if (portMapInterface_->physicalMappingIsValid(abstractPortName, portMapIndex))
            {
                if (index.column() != PortMapsColumns::TIEOFF &&
                    portMapInterface_->physicalPortIsValid(abstractPortName, portMapIndex))
                {
                    if (index.column() == PortMapsColumns::LOGICAL_PORT ||
                        index.column() == PortMapsColumns::PHYSICAL_PORT)
                    {
                        return portMapInterface_->connectedPortsHaveValidPortTypes(
                                abstractPortName, portMapIndex) &&
                            portMapInterface_->connectedPortsHaveValidDirections(
                                abstractPortName, portMapIndex) &&
                            portMapInterface_->connectedPortsHaveValidInitiatives(abstractPortName, portMapIndex);
                    }
                    else if (index.column() == PortMapsColumns::LOGICAL_LEFT ||
                        index.column() == PortMapsColumns::LOGICAL_RIGHT ||
                        index.column() == PortMapsColumns::PHYSICAL_LEFT ||
                        index.column() == PortMapsColumns::PHYSICAL_RIGHT)
                    {
                        return portMapInterface_->connectedPortsHaveSameRange(abstractPortName, portMapIndex);
                    }
                }
                else if (index.column() == PortMapsColumns::TIEOFF &&
                    portMapInterface_->tieOffIsValid(abstractPortName, portMapIndex))
                {
                    return true;
                }
            }

            if (index.column() == PortMapsColumns::LOGICAL_PORT)
            {
                return true;
            }
            else if (index.column() == PortMapsColumns::LOGICAL_LEFT ||
                index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                return portMapInterface_->logicalPortHasValidRange(abstractPortName, portMapIndex);
            }
        }

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onAddPortMap()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onAddPortMap(QModelIndex const& itemIndex)
{
    if (indexIsValid(itemIndex))
    {
        int newMapRow = 0;
        std::string logicalName = "";

        QModelIndex parentIndex;
        if (!itemIndex.parent().isValid())
        {
            parentIndex = itemIndex;
            logicalName = portMapInterface_->getIndexedItemName(parentIndex.row());
            newMapRow = portMapInterface_->portMapCount(logicalName);
        }
        else
        {
            parentIndex = itemIndex.parent();
            logicalName = portMapInterface_->getIndexedItemName(parentIndex.row());
            newMapRow = itemIndex.row();
        }

        if (parentIndex.column() != PortMapsColumns::LOGICAL_PORT)
        {
            parentIndex = parentIndex.sibling(parentIndex.row(), PortMapsColumns::LOGICAL_PORT);
        }

        beginInsertRows(parentIndex, newMapRow, newMapRow);

        portMapInterface_->addPortMap(newMapRow, logicalName);

        endInsertRows();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onRemovePort()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onRemovePort(QModelIndex const& itemIndex)
{
    if (itemIndex.parent().isValid() && indexIsValid(itemIndex))
    {
        QModelIndex parentIndex = itemIndex.parent();

        QPair<std::string, int> combinedIndex = getPortNamePortMapIndex(itemIndex);
        std::string abstractPortName = combinedIndex.first;
        int portMapIndex = combinedIndex.second;

        QString physicalPortName = "";
        if (portMapInterface_->hasPhysicalPort(abstractPortName, portMapIndex))
        {
            physicalPortName =
                QString::fromStdString(portMapInterface_->getPhysicalPortName(abstractPortName, portMapIndex));
        }

        beginRemoveRows(parentIndex, itemIndex.row(), itemIndex.row());

        portMapInterface_->removePortMap(abstractPortName, portMapIndex);

        endRemoveRows();

        if (!physicalPortName.isEmpty())
        {
            emit portDisconnected(physicalPortName);
        }

        emit dataChanged(parentIndex, parentIndex);

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onRemoveAllChildItemsFrom()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onRemoveAllChildItemsFrom(QModelIndex const& itemIndex)
{
    if (!itemIndex.parent().isValid() && indexIsValid(itemIndex))
    {
        QModelIndex logicalPortIndex = itemIndex;
        if (itemIndex.column() != PortMapsColumns::LOGICAL_PORT)
        {
            logicalPortIndex = itemIndex.sibling(itemIndex.row(), PortMapsColumns::LOGICAL_PORT);
        }

        std::string logicalPortName = portMapInterface_->getIndexedItemName(itemIndex.row());
        int portMapCount = portMapInterface_->portMapCount(logicalPortName);

        if (portMapCount > 0)
        {
            beginRemoveRows(logicalPortIndex, 0, portMapCount - 1);

            std::vector<std::string> connectedPhysicals =
                portMapInterface_->removeAllPortMapsFromLogicalPort(logicalPortName);

            endRemoveRows();

            for (auto physicalPort : connectedPhysicals)
            {
                emit portDisconnected(QString::fromStdString(physicalPort));
            }

            emit dataChanged(logicalPortIndex, logicalPortIndex);
            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList PortMapTreeModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());
    types << "text/plain";

    return types;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions PortMapTreeModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::dropMimeData()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int, int,
    const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    QString mimeText = data->text();

    QStringList physicalPorts = mimeText.split(';');

    QModelIndex physicalIndex = parent;
    if (parent.parent().isValid())
    {
        physicalIndex = parent.parent();
    }

    std::string portName = portMapInterface_->getIndexedItemName(physicalIndex.row());
    if (!portMapInterface_->logicalPortExists(portName))
    {
        return false;
    }

    if (physicalIndex.column() != PortMapsColumns::PHYSICAL_PORT)
    {
        physicalIndex = physicalIndex.sibling(physicalIndex.row(), PortMapsColumns::PHYSICAL_PORT);
    }

    foreach (QString currentPort, physicalPorts)
    {
        setData(physicalIndex, currentPort, Qt::EditRole);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onRemoveAllPortMappings()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onRemoveAllPortMappings()
{
    for (int i = 0; i < portMapInterface_->logicalPortCount(); ++i)
    {
        QModelIndex mappingIndex = index(i, PortMapsColumns::LOGICAL_PORT);
        onRemoveAllChildItemsFrom(mappingIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onAddAutoConnectedPortMaps()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onAddAutoConnectedPortMaps(QVector<QString> physicalPorts)
{
    beginResetModel();

    for (auto portName : physicalPorts)
    {
        emit portConnected(portName);
    }

    endResetModel();

    emit contentChanged();
}
