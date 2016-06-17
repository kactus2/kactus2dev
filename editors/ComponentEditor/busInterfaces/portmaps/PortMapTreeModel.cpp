//-----------------------------------------------------------------------------
// File: PortMapTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeModel.h"
#include "PortMapsColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <QIcon>
#include <QSize>
#include <QMimeData>

namespace
{
    QString const MULTIPLEPHYSICALS = "[multiple]";
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::PortMapTreeModel()
//-----------------------------------------------------------------------------
PortMapTreeModel::PortMapTreeModel(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component,
    LibraryInterface* handler, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortMapValidator> portMapValidator, QObject *parent):
QAbstractItemModel(parent),
ParameterizableTable(parameterFinder),
component_(component),
handler_(handler),
containingBusInterface_(busif),
absDef_(),
interfaceMode_(General::MASTER),
formatter_(expressionFormatter),
portMappings_(),
portMapValidator_(portMapValidator)
{
    setExpressionParser(expressionParser);
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
        return portMappings_.size();
    }
    else if (!parent.parent().isValid())
    {
        return portMappings_.at(parent.row()).portMaps_.size();
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
    if (!parent.isValid() && 0 <= row && row < portMappings_.size())
    {
        QSharedPointer<PortAbstraction> logicalPort = portMappings_.at(row).logicalPort_;
        return createIndex(row, column, logicalPort.data());
    }
    else
    {
        QSharedPointer<PortMap> portMap = getIndexedPortMap(parent, row);

        if (portMap)
        {
            return createIndex(row, column, portMap.data());
        }
        else
        {
            return QModelIndex();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getIndexedPortMap()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> PortMapTreeModel::getIndexedPortMap(QModelIndex const& parentIndex, int row) const
{
    QSharedPointer<PortMap> indexedPortMap;

    QString portName = parentIndex.sibling(row, PortMapsColumns::LOGICAL_PORT).data().toString();

    for (int portMappingIndex = 0; portMappingIndex < portMappings_.size(); ++portMappingIndex)
    {
        QString portMappingLogicalPortName = portMappings_.at(portMappingIndex).logicalPort_->name();

        if (portMappings_[portMappingIndex].logicalPort_.data() == parentIndex.internalPointer())
        {
            if (row < portMappings_[portMappingIndex].portMaps_.size())
            {
                indexedPortMap = portMappings_[portMappingIndex].portMaps_.at(row);
            }

            break;
        }
    }

    return indexedPortMap;
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

    for (int portMappingIndex = 0; portMappingIndex < portMappings_.size(); ++portMappingIndex)
    {
        if (portMappings_[portMappingIndex].logicalPort_.data() == child.internalPointer())
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
    for (int i = 0; i < portMappings_.size(); ++i)
    {
        foreach (QSharedPointer<PortMap> portMap, portMappings_[i].portMaps_)
        {
            if (portMap.data() == childItem)
            {
                return createIndex(i, 0, portMappings_[i].logicalPort_.data());
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

    QSharedPointer<PortAbstraction> abstractPort;
    QSharedPointer<PortMap> portMap;

    if (!index.parent().isValid())
    {
        abstractPort = portMappings_.at(index.row()).logicalPort_;

        if (portMappings_.at(index.row()).portMaps_.size() == 1)
        {
            portMap = portMappings_.at(index.row()).portMaps_.first();
        }
    }
    else
    {
        abstractPort = portMappings_.at(index.parent().row()).logicalPort_;
        portMap = portMappings_.at(index.parent().row()).portMaps_.at(index.row());
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == PortMapsColumns::LOGICAL_PORT)
        {
            return getLogicalPortName(index, portMap, abstractPort);
        }
        else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            return getPhysicalPortName(index, portMap);
        }
        else if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PRESENCE)
        {
            PresenceTypes::Presence requirement = abstractPort->getPresence(interfaceMode_);
            if (requirement == PresenceTypes::UNKNOWN)
            {
                requirement = PresenceTypes::OPTIONAL;
            }

            return PresenceTypes::presence2Str(requirement);
        }
        else
        {
            return formatter_->formatReferringExpression(valueForIndex(index, abstractPort, portMap).toString());
        }
    }
    else if (index.parent().isValid() && role == Qt::CheckStateRole && index.column() == PortMapsColumns::INVERT)
    {
        if (portMap->getInvert().toBool())
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
        return valueForIndex(index, abstractPort, portMap);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index, abstractPort, portMap).toString());
        }
        else
        {
            return QString("");
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == PortMapsColumns::LOGICAL_PRESENCE)
        {
            return QColor("Grey");
        }
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        return getBackgroundColour(index, abstractPort, portMap);
    }
    else if (role == Qt::DecorationRole)
    {
        if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PORT)
        {
            DirectionTypes::Direction direction = DirectionTypes::DIRECTION_INVALID;
            if (absDef_ && abstractPort)
            {
                direction = absDef_->getPortDirection(abstractPort->name(), interfaceMode_);
            }

            return getIconForDirection(direction);
        }
        else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            QString physicalPortName = getPhysicalPortName(index, portMap).toString();
            if (!physicalPortName.isEmpty() && physicalPortName.compare(MULTIPLEPHYSICALS, Qt::CaseSensitive) != 0)
            {
                DirectionTypes::Direction direction = DirectionTypes::DIRECTION_INVALID;
                QSharedPointer<Port> physicalPort = component_->getPort(physicalPortName);
                if (physicalPort)
                {
                    direction = physicalPort->getDirection();
                }

                return getIconForDirection(direction);
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(100, 20);
    }
    else if (role == Qt::UserRole + 1)
    {
        if (absDef_)
        {
            bool definitionHasPort = absDef_->hasPort(abstractPort->name(), interfaceMode_);
            return definitionHasPort;
        }
    }

    // Unsupported role.
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getBackgroundColour()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::getBackgroundColour(QModelIndex const& index,
    QSharedPointer<PortAbstraction> logicalPort, QSharedPointer<PortMap> portMap) const
{
    if (!index.parent().isValid())
    {
        if (logicalPort->getPresence(interfaceMode_) == PresenceTypes::REQUIRED &&
            index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            return QColor("lemonChiffon");
        }
        else
        {
            return QColor("aliceblue");
        }
    }
    else
    {
        if (index.column() == PortMapsColumns::LOGICAL_PORT ||
            (index.column() == PortMapsColumns::PHYSICAL_PORT && portMap->getLogicalTieOff().isEmpty()) ||
            (index.column() == PortMapsColumns::TIEOFF && !portMap->getPhysicalPort()))
        {
            return QColor("lemonChiffon");
        }
    }
    // return QVariant();
    return QColor("white");
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getIconForDirection()
//-----------------------------------------------------------------------------
QIcon PortMapTreeModel::getIconForDirection(DirectionTypes::Direction direction) const
{
    QString directionPath = ":icons/common/graphics/cross.png";
    if (direction == DirectionTypes::IN)
    {
        directionPath = ":icons/common/graphics/control-180.png";
    }
    else if (direction == DirectionTypes::OUT)
    {
        directionPath = ":icons/common/graphics/control.png";
    }
    else if (direction == DirectionTypes::INOUT)
    {
        directionPath = ":icons/common/graphics/control-dual.png";
    }

    return QIcon(directionPath);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getLogicalPortName()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::getLogicalPortName(QModelIndex const& index, QSharedPointer<PortMap> portMap,
    QSharedPointer<PortAbstraction> logicalPort) const
{
    QString logicalPortName = logicalPort->name();

    if (index.parent().isValid())
    {
        if (portMap && !portMap->getLogicalPort() ||
            (portMap->getLogicalPort() && portMap->getLogicalPort()->name_.isEmpty()))
        {
            logicalPortName = "Undefined";
        }

        QString numberText = " (" + QString::number(index.row()) + ")";
        logicalPortName.append(numberText);
    }

    return logicalPortName;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getPhysicalPortName()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::getPhysicalPortName(QModelIndex const& itemIndex, QSharedPointer<PortMap> portMap) const
{
    QString physicalName = "";

    if (portMap)
    {
        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (physicalPort)
        {
            physicalName = physicalPort->name_;
        }
    }
    else
    {
        QStringList physicalNames;
        foreach (QSharedPointer<PortMap> portMap, portMappings_.at(itemIndex.row()).portMaps_)
        {
            if (portMap->getPhysicalPort() && !physicalNames.contains(portMap->getPhysicalPort()->name_))
            {
                physicalNames.append(portMap->getPhysicalPort()->name_);
            }
        }
        if (physicalNames.size() == 1)
        {
            physicalName = physicalNames.first();
        }
        else if (physicalNames.size() > 1)
        {
            physicalName = MULTIPLEPHYSICALS;
        }
    }

    return physicalName;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::valueForIndex(QModelIndex const& index, QSharedPointer<PortAbstraction> logicalPort,
    QSharedPointer<PortMap> portMap) const
{
    if (!index.parent().isValid())
    {
        QVariant logicalLeft = getLogicalLeftBound(logicalPort);
        if (index.column() == PortMapsColumns::LOGICAL_LEFT)
        {
            return logicalLeft;
        }
        else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
        {
            if (logicalLeft.toString().isEmpty())
            {
                return logicalLeft;
            }
            else
            {
                return 0;
            }
        }
    }

    if (portMap)
    {
        QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();
        if (logicalPort && logicalPort->range_)
        {
            if (index.column() == PortMapsColumns::LOGICAL_LEFT)
            {
                return logicalPort->range_->getLeft();
            }
            else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                return logicalPort->range_->getRight();
            }
        }

        QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
        if (index.column() == PortMapsColumns::PHYSICAL_LEFT && physicalPort && physicalPort->partSelect_)
        {
            return physicalPort->partSelect_->getLeftRange();
        }
        else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT && physicalPort && physicalPort->partSelect_)
        {
            return physicalPort->partSelect_->getRightRange();
        }
        else if (index.column() == PortMapsColumns::TIEOFF)
        {
            return portMap->getLogicalTieOff();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::getLogicalLeftBound()
//-----------------------------------------------------------------------------
QVariant PortMapTreeModel::getLogicalLeftBound(QSharedPointer<PortAbstraction> logicalPort) const
{
    if (logicalPort)
    {
        QSharedPointer<WireAbstraction> abstractWire = logicalPort->getWire();
        if (abstractWire)
        {
            QString logicalWidth = abstractWire->getWidth(interfaceMode_);
            if (!logicalWidth.isEmpty())
            {
                int logicalLeft = parseExpressionToDecimal(logicalWidth).toInt() - 1;
                return logicalLeft;
            }
        }
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

    if (role == Qt::EditRole)
    {
        if (!index.parent().isValid() && PortMapsColumns::PHYSICAL_PORT &&
            !value.toString().isEmpty() && value.toString().compare(index.data(Qt::DisplayRole).toString()) != 0)
        {
            QSharedPointer<PortMap> newPortMap (new PortMap());

            QSharedPointer<PortMap::PhysicalPort> newPhysicalPort (new PortMap::PhysicalPort(value.toString()));
            newPortMap->setPhysicalPort(newPhysicalPort);

            QModelIndex logicalColumnIndex = index.sibling(index.row(), PortMapsColumns::LOGICAL_PORT);
            QString logicalName = portMappings_.at(logicalColumnIndex.row()).logicalPort_->name();
            if (absDef_->hasPort(logicalName, interfaceMode_))
            {
                QSharedPointer<PortMap::LogicalPort> newLogicalPort (new PortMap::LogicalPort(logicalName));
                newPortMap->setLogicalPort(newLogicalPort);
            }

            int portMapCountInIndex = portMappings_.at(logicalColumnIndex.row()).portMaps_.size();

            beginInsertRows(logicalColumnIndex, portMapCountInIndex, portMapCountInIndex);

            portMappings_[logicalColumnIndex.row()].portMaps_.append(newPortMap);
            containingBusInterface_->getPortMaps()->append(newPortMap);

            endInsertRows();

            sendPortConnectionSignal("", value.toString());
        }
        else if (index.parent().isValid())
        {
            QSharedPointer<PortMap> changedPortMap = getIndexedPortMap(index.parent(), index.row());

            if (index.column() == PortMapsColumns::LOGICAL_PORT)
            {
                switchMappedLogicalPort(index, value, changedPortMap);
            }
            else if (index.column() == PortMapsColumns::LOGICAL_LEFT ||
                index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                setLogicalValue(index, value, changedPortMap);
            }
            else if ((index.column() == PortMapsColumns::PHYSICAL_LEFT ||
                index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
                index.column() == PortMapsColumns::PHYSICAL_PORT))
            {
                if (!changedPortMap->getPhysicalPort())
                {
                    if (value.toString().isEmpty())
                    {
                        return false;
                    }
                    else
                    {
                        QSharedPointer<PortMap::PhysicalPort> newPhysicalPort =
                            QSharedPointer<PortMap::PhysicalPort>(new PortMap::PhysicalPort());
                        changedPortMap->setPhysicalPort(newPhysicalPort);
                    }
                }

                setPhysicalValue(index, value, changedPortMap);
            }
            else if (index.column() == PortMapsColumns::TIEOFF)
            {
                changedPortMap->setLogicalTieOff(value.toString());
            }
        }

        emit dataChanged(index, index);
        emit contentChanged();

        return true;
    }
    else if (index.parent().isValid() && role == Qt::CheckStateRole)
    {
        bool inverValue = value == Qt::Checked;

        QSharedPointer<PortMap> portMap = getIndexedPortMap(index.parent(), index.row());
        portMap->setInvert(inverValue);

        emit dataChanged(index, index);
        emit contentChanged();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::switchMappedLogicalPort()
//-----------------------------------------------------------------------------
void PortMapTreeModel::switchMappedLogicalPort(QModelIndex const& index, const QVariant& value,
    QSharedPointer<PortMap> switchedPortMap)
{
    if (index.parent().isValid())
    {
        QString newLogicalName = value.toString();

        QSharedPointer<PortMap::LogicalPort> logicalPort = switchedPortMap->getLogicalPort();
        if (!logicalPort)
        {
            logicalPort = QSharedPointer<PortMap::LogicalPort> (new PortMap::LogicalPort());
            switchedPortMap->setLogicalPort(logicalPort);
        }

        if (logicalPort->name_.compare(newLogicalName, Qt::CaseInsensitive) != 0)
        {
            logicalPort->name_ = newLogicalName;

            QModelIndex sourceParentIndex = index.parent();

            QModelIndex destinationParentIndex =
                sourceParentIndex.sibling(portMappings_.size() - 1, PortMapsColumns::LOGICAL_PORT);

            for (int logicalIndex = 0; logicalIndex  < portMappings_.size(); ++logicalIndex)
            {
                if (portMappings_[logicalIndex].logicalPort_->name().compare(newLogicalName, Qt::CaseInsensitive)
                    == 0)
                {
                    destinationParentIndex =
                        sourceParentIndex.sibling(logicalIndex, PortMapsColumns::LOGICAL_PORT);
                    break;
                }
            }
            int sourceRow = index.row();

            int destinationPortMapCount = portMappings_[destinationParentIndex.row()].portMaps_.size();

            beginMoveRows(sourceParentIndex, sourceRow, sourceRow, destinationParentIndex,
                destinationPortMapCount);

            portMappings_[sourceParentIndex.row()].portMaps_.removeAll(switchedPortMap);
            portMappings_[destinationParentIndex.row()].portMaps_.append(switchedPortMap);

            endMoveRows();

            if (logicalPort && logicalPort->name_.isEmpty() && (!logicalPort->range_ || (logicalPort->range_ &&
                logicalPort->range_->getLeft().isEmpty() && logicalPort->range_->getRight().isEmpty())))
            {
                logicalPort->range_.clear();
                switchedPortMap->setLogicalPort(QSharedPointer<PortMap::LogicalPort>());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::setLogicalValue()
//-----------------------------------------------------------------------------
void PortMapTreeModel::setLogicalValue(QModelIndex const& index, const QVariant& value,
    QSharedPointer<PortMap> changedPortMap)
{
    QSharedPointer<PortMap::LogicalPort> logicalPort = changedPortMap->getLogicalPort();
    if (!logicalPort)
    {
        logicalPort = QSharedPointer<PortMap::LogicalPort>(new PortMap::LogicalPort());
        changedPortMap->setLogicalPort(logicalPort);
    }

    if (!value.toString().isEmpty() && !logicalPort->range_)
    {
        QSharedPointer<Range> newRange (new Range("", ""));
        logicalPort->range_ = newRange;
    }

    if (index.column() == PortMapsColumns::LOGICAL_LEFT && logicalPort->range_)
    {
        logicalPort->range_->setLeft(value.toString());
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT && logicalPort->range_)
    {
        logicalPort->range_->setRight(value.toString());
    }

    if (logicalPort && logicalPort->name_.isEmpty() && (!logicalPort->range_ || (logicalPort->range_ &&
        logicalPort->range_->getLeft().isEmpty() && logicalPort->range_->getRight().isEmpty())))
    {
        logicalPort->range_.clear();
        changedPortMap->setLogicalPort(QSharedPointer<PortMap::LogicalPort>());
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::setPhysicalValue()
//-----------------------------------------------------------------------------
void PortMapTreeModel::setPhysicalValue(QModelIndex const& index, const QVariant& value,
    QSharedPointer<PortMap> changedPortMap)
{
    QSharedPointer<PortMap::PhysicalPort> physicalPort = changedPortMap->getPhysicalPort();
    if (physicalPort)
    {
        if (index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            QString oldPortName = physicalPort->name_;

            physicalPort->name_ = value.toString();

            sendPortConnectionSignal(oldPortName, value.toString());
        }
        else
        {
            if (!physicalPort->partSelect_)
            {
                QSharedPointer<PartSelect> newPartSelect (new PartSelect());
                physicalPort->partSelect_ = newPartSelect;
            }

            if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
            {
                physicalPort->partSelect_->setLeftRange(value.toString());
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
            {
                physicalPort->partSelect_->setRightRange(value.toString());
            }

            if (physicalPort->partSelect_->getLeftRange().isEmpty() &&
                physicalPort->partSelect_->getRightRange().isEmpty())
            {
                physicalPort->partSelect_.clear();
            }
        }

        if (physicalPort && physicalPort->name_.isEmpty() && (!physicalPort->partSelect_ ||
            (physicalPort->partSelect_ && physicalPort->partSelect_->getLeftRange().isEmpty() &&
            physicalPort->partSelect_->getRightRange().isEmpty())))
        {
            changedPortMap->getPhysicalPort().clear();
            changedPortMap->setPhysicalPort(QSharedPointer<PortMap::PhysicalPort>());
        }
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
    QModelIndex parentIndex = index.parent();

    if (index.isValid() && ((!parentIndex.isValid() && 0 <= index.row() && index.row() < portMappings_.size()) ||
        (parentIndex.isValid() && 0 <= index.row() &&
        index.row() < portMappings_.at(parentIndex.row()).portMaps_.size())))
    {
        return true;
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

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

    if (index.column() == PortMapsColumns::INVERT && index.parent().isValid())
    {
        itemFlags |= Qt::ItemIsUserCheckable;
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT ||
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

    portMappings_.clear();

    QSharedPointer<PortAbstraction> unconnectedPort (new PortAbstraction());
    unconnectedPort->setLogicalName("Unknown");

    PortMapping unconnectedMapping;
    unconnectedMapping.logicalPort_ = unconnectedPort;

    if (absDef_)
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *absDef_->getLogicalPorts())
        {
            if (logicalPort->hasMode(interfaceMode_))
            {
                PortMapping newMapping;
                newMapping.logicalPort_ = logicalPort;

                portMappings_.append(newMapping);
            }
        }
    }

    foreach (QSharedPointer<PortMap> currentMap, *containingBusInterface_->getPortMaps())
    {
        bool logicalPortFound = false;

        for (int mappingIndex = 0; mappingIndex< portMappings_.size(); ++mappingIndex)
        {
            if (portMappings_.at(mappingIndex).logicalPort_->name() == currentMap->getLogicalPort()->name_)
            {
                portMappings_[mappingIndex].portMaps_.append(currentMap);
                logicalPortFound = true;
                break;;
            }
        }

        if (!logicalPortFound)
        {
            if (currentMap->getLogicalPort() && !currentMap->getLogicalPort()->name_.isEmpty())
            {
                QSharedPointer<PortAbstraction> newAbstractionPort (new PortAbstraction());
                newAbstractionPort->setLogicalName(currentMap->getLogicalPort()->name_);

                PortMapping newMapping;
                newMapping.logicalPort_ = newAbstractionPort;
                newMapping.portMaps_.append(currentMap);

                portMappings_.append(newMapping);
            }
            else
            {
                unconnectedMapping.portMaps_.append(currentMap);
            }
        }
    }

    portMappings_.append(unconnectedMapping);

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::setAbsType()
//-----------------------------------------------------------------------------
void PortMapTreeModel::setAbsType(VLNV const& vlnv, General::InterfaceMode mode)
{
    interfaceMode_ = mode;

    absDef_ = QSharedPointer<AbstractionDefinition>();

    if (vlnv.isValid())
    {
        QSharedPointer<Document> libComb = handler_->getModel(vlnv);
        if (libComb)
        {
            if (handler_->getDocumentType(vlnv) == VLNV::ABSTRACTIONDEFINITION)
            {
                absDef_ = libComb.dynamicCast<AbstractionDefinition>();
            }
        }
    }

    // Add existing mappings.
    reset();
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
    QSharedPointer<PortAbstraction> abstractPort;
    QSharedPointer<PortMap> portMap;

    if (!index.parent().isValid())
    {
        abstractPort = portMappings_.at(index.row()).logicalPort_;
    }
    else
    {
        abstractPort = portMappings_.at(index.parent().row()).logicalPort_;
        portMap = portMappings_.at(index.parent().row()).portMaps_.at(index.row());
    }

    if (index.column() == PortMapsColumns::LOGICAL_PORT)
    {
        return getLogicalPortName(index, portMap, abstractPort);
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        return getPhysicalPortName(index, portMap);
    }
    else if (index.column() == PortMapsColumns::LOGICAL_PRESENCE)
    {
        return PresenceTypes::presence2Str(abstractPort->getPresence(interfaceMode_));
    }
    else
    {
        return valueForIndex(index, abstractPort, portMap).toString();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::validateIndex()
//-----------------------------------------------------------------------------
bool PortMapTreeModel::validateIndex(QModelIndex const& index) const
{
    QModelIndex logicalIndex = index;
    QSharedPointer<PortMap> currentPortMap;
    if (index.parent().isValid())
    {
        logicalIndex = index.parent();
        currentPortMap = portMappings_.at(logicalIndex.row()).portMaps_.at(index.row());
    }

    portMapValidator_->abstractionDefinitionChanged(absDef_, interfaceMode_);

    QSharedPointer<PortAbstraction> logicalPort = portMappings_.at(logicalIndex.row()).logicalPort_;
    if (!absDef_->hasPort(logicalPort->name(), interfaceMode_))
    {
        return false;
    }

    QModelIndex physicalPortIndex = index.sibling(index.row(), PortMapsColumns::PHYSICAL_PORT);
    QString physicalPortName = physicalPortIndex.data(Qt::DisplayRole).toString();
    QSharedPointer<Port> physicalPort = component_->getPort(physicalPortName);

    if ((index.column() == PortMapsColumns::LOGICAL_PORT || index.column() == PortMapsColumns::PHYSICAL_PORT) &&
        (!physicalPortName.isEmpty() || physicalPortName.compare(MULTIPLEPHYSICALS) != 0) && physicalPort &&
        logicalPort->getWire())
    {
        DirectionTypes::Direction logicalDirection = logicalPort->getWire()->getDirection(interfaceMode_);
        DirectionTypes::Direction physicalDirection = physicalPort->getDirection();
        if (logicalDirection != physicalDirection)
        {
            return false;
        }
    }

    if (index.parent().isValid() && currentPortMap &&
        index.column() != PortMapsColumns::LOGICAL_PRESENCE && index.column() != PortMapsColumns::INVERT)
    {
        if (index.column() == PortMapsColumns::LOGICAL_PORT ||
            index.column() == PortMapsColumns::LOGICAL_LEFT || index.column() == PortMapsColumns::LOGICAL_RIGHT)
        {
            if (portMapValidator_->hasValidLogicalPort(currentPortMap))
            {
                if (index.column() == PortMapsColumns::LOGICAL_LEFT || 
                    index.column() == PortMapsColumns::LOGICAL_RIGHT)
                {
                    return portMapValidator_->connectedPortsHaveSameRange(currentPortMap);
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (currentPortMap->getPhysicalPort() && !currentPortMap->getLogicalTieOff().isEmpty())
            {
                return false;
            }

            if (index.column() == PortMapsColumns::TIEOFF)
            {
                return portMapValidator_->hasValidTieOff(currentPortMap);
            }

            if (currentPortMap->getPhysicalPort())
            {
                QSharedPointer<Port> physicalPort = component_->getPort(currentPortMap->getPhysicalPort()->name_);
                if (portMapValidator_->hasValidPhysicalPort(currentPortMap, physicalPort))
                {
                    if (index.column() == PortMapsColumns::PHYSICAL_LEFT ||
                        index.column() == PortMapsColumns::PHYSICAL_RIGHT)
                    {
                        return portMapValidator_->connectedPortsHaveSameRange(currentPortMap);
                    }
                }
                else
                {
                    return false;
                }
            }
        }
    }

    else
    {
        QSharedPointer<PortAbstraction> logicalPort = portMappings_.at(index.row()).logicalPort_;
        bool hasPortMaps = !portMappings_.at(index.row()).portMaps_.isEmpty();
        if (logicalPort && (logicalPort->getPresence(interfaceMode_) == PresenceTypes::ILLEGAL) && hasPortMaps)
        {
            return false;
        }
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
        QModelIndex parentIndex;
        if (!itemIndex.parent().isValid())
        {
            parentIndex = itemIndex;
        }
        else
        {
            parentIndex = itemIndex.parent();
        }

        if (parentIndex.column() != PortMapsColumns::LOGICAL_PORT)
        {
            parentIndex = parentIndex.sibling(parentIndex.row(), PortMapsColumns::LOGICAL_PORT);
        }

        int parentRow = parentIndex.row();
        QString logicalPortName = portMappings_.at(parentRow).logicalPort_->name();

        int newItemRow = portMappings_.at(parentRow).portMaps_.size();

        QSharedPointer<PortMap::LogicalPort> logicalPort (new PortMap::LogicalPort(logicalPortName));
        QSharedPointer<PortMap> newPortMap(new PortMap());
        newPortMap->setLogicalPort(logicalPort);

        beginInsertRows(parentIndex, newItemRow, newItemRow);
        portMappings_[parentRow].portMaps_.insert(newItemRow, newPortMap);
        endInsertRows();

        containingBusInterface_->getPortMaps()->append(newPortMap);

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

        QSharedPointer<PortMap> removedPortMap = portMappings_.at(parentIndex.row()).portMaps_.at(itemIndex.row());

        beginRemoveRows(parentIndex, itemIndex.row(), itemIndex.row());
        portMappings_[parentIndex.row()].portMaps_.removeAll(removedPortMap);
        endRemoveRows();


        containingBusInterface_->getPortMaps()->removeAll(removedPortMap);

        if (removedPortMap->getPhysicalPort())
        {
            emit portDisconnected(removedPortMap->getPhysicalPort()->name_);
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

        QList<QSharedPointer<PortMap> > removedMaps = portMappings_[logicalPortIndex.row()].portMaps_;

        if (!removedMaps.isEmpty())
        {
            beginRemoveRows(logicalPortIndex, 0, removedMaps.size()-1);

            foreach (QSharedPointer<PortMap> portMap, removedMaps)
            {
                portMappings_[logicalPortIndex.row()].portMaps_.removeAll(portMap);
                containingBusInterface_->getPortMaps()->removeAll(portMap);

                if (portMap->getPhysicalPort())
                {
                    emit portDisconnected(portMap->getPhysicalPort()->name_);
                }
            }

            endRemoveRows();

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
    for (int i = 0; i < portMappings_.size(); ++i)
    {
        QModelIndex mappingIndex = index(i, PortMapsColumns::LOGICAL_PORT);
        onRemoveAllChildItemsFrom(mappingIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::onAddConnectedPortMap()
//-----------------------------------------------------------------------------
void PortMapTreeModel::onAddConnectedPortMap(QSharedPointer<PortMap> newPortMap)
{
    if (newPortMap->getLogicalPort() && newPortMap->getPhysicalPort())
    {
        for (int portMapIndex = 0; portMapIndex < portMappings_.size(); ++portMapIndex)
        {
            if (portMappings_[portMapIndex].logicalPort_->name() == newPortMap->getLogicalPort()->name_)
            {
                QModelIndex logicalIndex = index(portMapIndex, PortMapsColumns::LOGICAL_PORT);
                int portMapCount = portMappings_[portMapIndex].portMaps_.size();

                beginInsertRows(logicalIndex, portMapCount, portMapCount);

                portMappings_[portMapIndex].portMaps_.append(newPortMap);
                containingBusInterface_->getPortMaps()->append(newPortMap);

                endInsertRows();

                emit portConnected(newPortMap->getPhysicalPort()->name_);
                emit contentChanged();

                return;
            }
        }
    }
}
