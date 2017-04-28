//-----------------------------------------------------------------------------
// File: AbsDefTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 27.04.2017
//
// Description:
// A model for displaying abstraction definitions.
//-----------------------------------------------------------------------------

#include "AbsDefTreeModel.h"
#include "LogicalPortColumns.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QIcon>
#include <QSize>
#include <QMimeData>

#include <common/KactusColors.h>

namespace
{
    QString const MULTIPLE_SELECTED = "[multiple]";
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::AbsDefTreeModel()
//-----------------------------------------------------------------------------
AbsDefTreeModel::AbsDefTreeModel(QObject *parent):
QAbstractItemModel(parent),
absDef_(),
busDefinition_(),
table_() 
{
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::~AbsDefTreeModel()
//-----------------------------------------------------------------------------
AbsDefTreeModel::~AbsDefTreeModel()
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::columnCount()
//-----------------------------------------------------------------------------
int AbsDefTreeModel::columnCount(const QModelIndex&) const
{
    return LogicalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::rowCount()
//-----------------------------------------------------------------------------
int AbsDefTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return table_.size();
    }
    else if (!parent.parent().isValid())
    {
        return table_.at(parent.row())->wirePorts_.size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbsDefTreeModel::flags(const QModelIndex& index) const 
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.parent().isValid())
    {
        QSharedPointer<PortAbstractionEntry> entry = table_.at(index.parent().row());

        QSharedPointer<WireAbstraction> chosen = entry->portAbstraction_->getWire();

        // Do not allow setting system groups for other than system mode.
        if (index.column() == LogicalPortColumns::SYSTEM_GROUP && 
            !chosen->getSystemPorts()->isEmpty())
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::index()
//-----------------------------------------------------------------------------
QModelIndex AbsDefTreeModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!parent.isValid() && 0 <= row && row < table_.size())
    {
        QSharedPointer<PortAbstraction> logicalPort = table_.at(row)->portAbstraction_;
        return createIndex(row, column, logicalPort.data());
    }
    else
    {
        QSharedPointer<WirePort> wirePort = getIndexedWirePort(parent, row);

        if (wirePort)
        {
            return createIndex(row, column, wirePort.data());
        }
        else
        {
            return QModelIndex();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::getIndexedWirePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> AbsDefTreeModel::getIndexedWirePort(QModelIndex const& parentIndex, int row) const
{
    QSharedPointer<WirePort> indexedPortMap;

    for (int treeIndex = 0; treeIndex < table_.size(); ++treeIndex)
    {
        QSharedPointer<PortAbstractionEntry> signal = table_.at(treeIndex);

        if (signal->portAbstraction_.data() == parentIndex.internalPointer())
        {
            if (row < signal->wirePorts_.size())
            {
                indexedPortMap = table_[treeIndex]->wirePorts_.at(row);
            }

            break;
        }
    }

    return indexedPortMap;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::parent()
//-----------------------------------------------------------------------------
QModelIndex AbsDefTreeModel::parent(QModelIndex const& child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    for (int portMappingIndex = 0; portMappingIndex < table_.size(); ++portMappingIndex)
    {
        if (table_[portMappingIndex]->portAbstraction_.data() == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    WirePort* childItem = static_cast<WirePort*>(child.internalPointer());
    return createParentIndexForPortMap(childItem);
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::createParentIndexForPortMap()
//-----------------------------------------------------------------------------
QModelIndex AbsDefTreeModel::createParentIndexForPortMap(WirePort* childItem) const
{
    for (int i = 0; i < table_.size(); ++i)
    {
        foreach (QSharedPointer<WirePort> wirePort, table_[i]->wirePorts_)
        {
            if (wirePort.data() == childItem)
            {
                return createIndex(i, 0, table_[i]->portAbstraction_.data());
            }
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbsDefTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return QVariant();
    }

    if (section == LogicalPortColumns::NAME)
    {		   
        return tr("Name");
    }
    else if (section == LogicalPortColumns::QUALIFIER)
    {
        return tr("Qualifier");
    }
    else if (section == LogicalPortColumns::WIDTH)
    {
        return tr("Width");
    }
    else if (section == LogicalPortColumns::DIRECTION)
    {
        return tr("Direction");
    }
    else if (section == LogicalPortColumns::MODE)
    {
        return tr("Mode");
    }
    else if (section == LogicalPortColumns::SYSTEM_GROUP)
    {
        return tr("System group");
    }
    else if (section == LogicalPortColumns::PRESENCE)
    {
        return tr("Presence");
    }
    else if (section == LogicalPortColumns::DEFAULT_VALUE)
    {
        return tr("Default value");
    }
    else if (section == LogicalPortColumns::DRIVER)
    {
        return tr("Driver");
    }
    else if (section == LogicalPortColumns::DESCRIPTION)
    {
        return tr("Description");
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::data()
//-----------------------------------------------------------------------------
QVariant AbsDefTreeModel::data(QModelIndex const& index, int role) const
{
    if (!indexIsValid(index))
    {
        return QVariant();
    }

    QSharedPointer<PortAbstraction> portAbstraction;
    QSharedPointer<WirePort> wirePort;
    QSharedPointer<WireAbstraction> wireAbstraction;
    QSharedPointer<AbsDefTreeModel::PortAbstractionEntry> entry;

    if (!index.parent().isValid())
    {
        entry = table_.at(index.row());

        portAbstraction = entry->portAbstraction_;

        /*if (table_.at(index.row())->wirePorts_.size() == 1)
        {
           wirePort = entry->wirePorts_.first();
        }*/
    }
    else
    {
        entry = table_.at(index.parent().row());

        portAbstraction = entry->portAbstraction_;
        wirePort = entry->wirePorts_.at(index.row());
    }

    wireAbstraction = portAbstraction->getWire();

    if (role == Qt::DisplayRole) 
    {
        if (!wirePort && index.column() == LogicalPortColumns::NAME) 
        {
            if (portAbstraction->getLogicalName().isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return portAbstraction->getLogicalName();
            }
        }
        else if (!wirePort && index.column() == LogicalPortColumns::QUALIFIER)
        {
            return toString(wireAbstraction->getQualifier());
        }
        else if (wirePort && index.column() == LogicalPortColumns::WIDTH)
        {
            return wirePort->getWidth();
        }
        else if (wirePort && index.column() == LogicalPortColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(wirePort->getDirection());
        }
        else if (wirePort && index.column() == LogicalPortColumns::MODE)
        {
            return General::interfaceMode2Str(wirePort->getMode());
        }
        else if (wirePort && index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            return wirePort->getSystemGroup();
        }
        else if (wirePort && index.column() == LogicalPortColumns::PRESENCE)
        {
            return presence2Str(wirePort->getPresence());
        }
        else if (!wirePort && index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return wireAbstraction->getDefaultValue();
        }
        else if (!wirePort && index.column() == LogicalPortColumns::DRIVER)
        {
            return General::driverType2Str(wireAbstraction->getDriverType());
        }
        else if (!wirePort && index.column() == LogicalPortColumns::DESCRIPTION)
        {
            return portAbstraction->description();
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::BackgroundRole) 
    {
        return getBackgroundColour(index, portAbstraction, wirePort);
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == LogicalPortColumns::NAME && portAbstraction->getLogicalName().isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE && 
            (wirePort && wirePort->getMode() == General::INTERFACE_MODE_COUNT || table_.count(entry) > 1)))
        {
            return KactusColors::ERROR;
        }
        else if (wirePort && index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            if (wirePort->getMode() != General::SYSTEM || !busDefinition_ || 
                !busDefinition_->getSystemGroupNames().contains(wirePort->getSystemGroup()))
            {
                return KactusColors::ERROR;
            }
        }
        else
        {
            return QVariant();
        }
    }
    else if (wirePort && role == Qt::DecorationRole)
    {
        if (index.column() == LogicalPortColumns::DIRECTION)
        {
            DirectionTypes::Direction direction = wirePort->getDirection();

            return getIconForDirection(direction);
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::getBackgroundColour()
//-----------------------------------------------------------------------------
QVariant AbsDefTreeModel::getBackgroundColour(QModelIndex const& index,
    QSharedPointer<PortAbstraction> logicalPort, QSharedPointer<WirePort> wirePort) const
{
    if (!index.parent().isValid())
    {
        if (index.column() == LogicalPortColumns::NAME)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::LOGICAL_PORT_BACKGROUND;
        }
    }
    else
    {
        if (index.column() == LogicalPortColumns::SYSTEM_GROUP && wirePort->getMode() == General::SYSTEM) 
        {
            return KactusColors::MANDATORY_FIELD;
        }
    }

    return KactusColors::REGULAR_FIELD;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::getIconForDirection()
//-----------------------------------------------------------------------------
QIcon AbsDefTreeModel::getIconForDirection(DirectionTypes::Direction direction) const
{
    QString directionPath = ":icons/common/graphics/cross.png";
    if (direction == DirectionTypes::IN)
    {
        directionPath = ":icons/common/graphics/input.png";
    }
    else if (direction == DirectionTypes::OUT)
    {
        directionPath = ":icons/common/graphics/output.png";
    }
    else if (direction == DirectionTypes::INOUT)
    {
        directionPath = ":icons/common/graphics/inout.png";
    }
    else if (direction == DirectionTypes::DIRECTION_PHANTOM)
    {
        directionPath = ":icons/common/graphics/phantom.png";
    }

    return QIcon(directionPath);
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::setData()
//-----------------------------------------------------------------------------
bool AbsDefTreeModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    if (!indexIsValid(index))
    {
        return false;
    }

    if (role != Qt::EditRole)
    {
        return false;
    }

    QSharedPointer<PortAbstraction> portAbstraction;
    QSharedPointer<WirePort> wirePort;
    QSharedPointer<WireAbstraction> wireAbstraction;
    QSharedPointer<AbsDefTreeModel::PortAbstractionEntry> entry;

    if (!index.parent().isValid())
    {
        entry = table_.at(index.row());

        portAbstraction = entry->portAbstraction_;

        /*if (table_.at(index.row())->wirePorts_.size() == 1)
        {
            wirePort = entry->wirePorts_.first();
        }*/
    }
    else
    {
        entry = table_.at(index.parent().row());

        portAbstraction = entry->portAbstraction_;
        wirePort = entry->wirePorts_.at(index.row());
    }

    wireAbstraction = portAbstraction->getWire();

    if (!wirePort && index.column() == LogicalPortColumns::NAME)
    {		
        QString newName = value.toString();
        QString oldName = portAbstraction->getLogicalName();
        portAbstraction->setLogicalName(newName);        
        //emit portRenamed(oldName, newName);
    }
    else if (!wirePort && index.column() == LogicalPortColumns::QUALIFIER)
    {
        wireAbstraction->setQualifier(toQualifier(value.toString()));
    }
    else if (wirePort && index.column() == LogicalPortColumns::WIDTH)
    {
        wirePort->setWidth(value.toString());            
    }
    else if (!wirePort && index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        portAbstraction->getWire()->setDefaultValue(value.toString());
    }
    /*else if (index.column() == LogicalPortColumns::MODE)
    {
        General::InterfaceMode oldMode = wirePort->getMode();
        General::InterfaceMode newMode = General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT);

        if ()
    }*/
    else if (wirePort && index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        wirePort->setSystemGroup(value.toString());
    }
    else if (wirePort && index.column() == LogicalPortColumns::DIRECTION)
    {
        wirePort->setDirection(DirectionTypes::str2Direction(value.toString(),
            DirectionTypes::DIRECTION_INVALID)); 
    }
    else if (wirePort && index.column() == LogicalPortColumns::PRESENCE)
    {
        wirePort->setPresence(PresenceTypes::str2Presence(value.toString(), PresenceTypes::UNKNOWN));
    }
    else if (!wirePort && index.column() == LogicalPortColumns::DRIVER)
    {
        portAbstraction->getWire()->setDriverType(General::str2DriverType(value.toString(), General::NO_DRIVER));
    }
    else if (!wirePort && index.column() == LogicalPortColumns::DESCRIPTION)
    {
        portAbstraction->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::sendPortConnectionSignal()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::sendPortConnectionSignal(QString const& oldName, QString const& newName)
{
    if (!oldName.isEmpty())
    {
        emit portDisconnected(oldName);
    }

    emit portConnected(newName);
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::indexIsValid()
//-----------------------------------------------------------------------------
bool AbsDefTreeModel::indexIsValid(QModelIndex const& index) const
{
    QModelIndex parentIndex = index.parent();

    if (index.isValid() && ((!parentIndex.isValid() && 0 <= index.row() && index.row() < table_.size()) ||
        (parentIndex.isValid() && 0 <= index.row() &&
        index.row() < table_.at(parentIndex.row())->wirePorts_.size())))
    {
        return true;
    }
 
    return false;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::setAbsDef(QSharedPointer<AbstractionDefinition> absDef) 
{
    Q_ASSERT_X(absDef, "AbsDefTreeModel::setAbsDef", "Null Abstraction Definition given as parameter");

    absDef_ = absDef;

    beginResetModel();
    table_.clear();

    foreach (QSharedPointer<PortAbstraction> portAbs, *absDef_->getLogicalPorts()) 
    {
        QSharedPointer<PortAbstractionEntry> entry(new PortAbstractionEntry);
        entry->portAbstraction_ = portAbs;

        if (portAbs->hasWire())
        {
            if (portAbs->getWire()->hasMasterPort()) 
            {
                entry->wirePorts_.append(portAbs->getWire()->getMasterPort());
            }

            if (portAbs->getWire()->hasSlavePort()) 
            {
                entry->wirePorts_.append(portAbs->getWire()->getSlavePort());
            }

            foreach (QSharedPointer<WirePort> system, *portAbs->getWire()->getSystemPorts()) 
            {
                entry->wirePorts_.append(system);
            }

        }

        table_.append(entry);
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::setBusDef()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel:::addSignalOptions()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::addSignalOptions(QModelIndexList const& indexes) 
{
    QList<QSharedPointer<PortAbstractionEntry> > targetPorts;

    // Find all ports that match given indexes.
    foreach (QModelIndex index, indexes) 
    {
        if (0 <= index.row() && index.row() <= table_.size()) 
        {
            QSharedPointer<PortAbstractionEntry> temp = table_.at(index.row());
            if (!targetPorts.contains(temp))
            {
                targetPorts.append(temp);
            }
        }
    }

    beginResetModel();

    // Make other port modes for the ports.
    foreach (QSharedPointer<PortAbstractionEntry> entry, targetPorts) 
    {
        QSharedPointer<WireAbstraction> wireAbstraction = entry->portAbstraction_->getWire();

        // Cannot do without at least one wireport.
        if (entry->wirePorts_.size() < 1)
        {
            continue;
        }

        // In practice, only the first wire port is relevant.
        QSharedPointer<WirePort> oldPort = entry->wirePorts_.first();

        if (oldPort->getMode() == General::SYSTEM)
        {
            QStringList groups;
            QStringList usedGroups;

            if (busDefinition_)
            {
                groups = busDefinition_->getSystemGroupNames();
            }
            else
            {
                groups.append("");
            }

            foreach(QString systemGroup, groups)
            {
                // Skip existing system modes.
                if (wireAbstraction->findSystemPort(systemGroup))
                {
                    continue;
                }

                // Make a copy, change the mode.
                QSharedPointer<WirePort> newPort(new WirePort(*oldPort));
                newPort->setSystemGroup(systemGroup);

                // Reverse the direction for convenience, if unidirectional.
                // It is assumed, however, that with more that two system groups, there should only one driver.
                if (groups.size() < 3 && oldPort->getDirection() == DirectionTypes::IN)
                {
                    oldPort->setDirection(DirectionTypes::OUT);
                }
                else
                {
                    oldPort->setDirection(DirectionTypes::IN);
                }

                // Add to the wire abstraction.
                wireAbstraction->addSystemPort(newPort);
            }
        }
        else if (oldPort->getMode() == General::MASTER)
        {
            // Skip if already has a slave.
            if (wireAbstraction->hasSlavePort())
            {
                continue;
            }

            // Create a new slave for the master.
            QSharedPointer<WirePort> newPort(new WirePort(*oldPort));
            newPort->setMode(General::SLAVE);

            // Reverse the direction for convenience, if unidirectional.
            if (oldPort->getDirection() == DirectionTypes::IN)
            {
                oldPort->setDirection(DirectionTypes::OUT);
            }
            else
            {
                oldPort->setDirection(DirectionTypes::IN);
            }

            // Add to the wire abstraction.
            wireAbstraction->setSlavePort(newPort);
        }
        else if (oldPort->getMode() == General::SLAVE)
        {
            // Skip if already has a slave.
            if (wireAbstraction->hasSlavePort())
            {
                continue;
            }

            // Create a new slave for the master.
            QSharedPointer<WirePort> newPort(new WirePort(*oldPort));
            newPort->setMode(General::MASTER);

            // Reverse the direction for convenience, if unidirectional.
            if (oldPort->getDirection() == DirectionTypes::IN)
            {
                oldPort->setDirection(DirectionTypes::OUT);
            }
            else
            {
                oldPort->setDirection(DirectionTypes::IN);
            }

            // Add to the wire abstraction.
            wireAbstraction->setSlavePort(newPort);
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::onAddItem()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::onAddItem(QModelIndex const& itemIndex)
{
    if (!indexIsValid(itemIndex))
    {
        return;
    }

    QModelIndex parentIndex = itemIndex.parent();
    if (parentIndex.isValid())
    {
        int parentRow = parentIndex.row();

        QSharedPointer<PortAbstractionEntry> entry = table_.at(parentRow);

        int newItemRow = entry->wirePorts_.size();

        // Create a new slave for the master.
        QSharedPointer<WirePort> newPort(new WirePort(General::SYSTEM));

        beginInsertRows(parentIndex, newItemRow, newItemRow);
        entry->wirePorts_.append(newPort);
        endInsertRows();

        entry->portAbstraction_->getWire()->addSystemPort(newPort);

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbsDefTreeModel::onRemoveItem(QModelIndex const& itemIndex)
{
    if (!indexIsValid(itemIndex))
    {
        return;
    }

    if (itemIndex.parent().isValid())
    {
        QModelIndex parentIndex = itemIndex.parent();

        QSharedPointer<PortAbstractionEntry> entry = table_.at(parentIndex.row());
        QSharedPointer<WirePort> removedWirePort = entry->wirePorts_.at(itemIndex.row());

        beginRemoveRows(parentIndex, itemIndex.row(), itemIndex.row());
        entry->wirePorts_.removeAll(removedWirePort);
        entry->portAbstraction_->getWire()->removeWirePort(removedWirePort);
        endRemoveRows();

        emit dataChanged(itemIndex, itemIndex);

        emit contentChanged();

        emit portRemoved(entry->portAbstraction_->name(), removedWirePort->getMode());      
    }
    else
    {
        QSharedPointer<PortAbstractionEntry> entry = table_.at(itemIndex.row());
            
        beginRemoveRows(QModelIndex(), itemIndex.row(), itemIndex.row());
        absDef_->getLogicalPorts()->removeAll(entry->portAbstraction_);
        table_.removeAll(entry);
        endRemoveRows();

        emit dataChanged(itemIndex, itemIndex);

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::toString()
//-----------------------------------------------------------------------------
QString AbsDefTreeModel::toString(Qualifier qualifier) const 
{
    if (qualifier.isData() && qualifier.isAddress())
    {
        return "data/address";        
    }
    else if (qualifier.isData())
    {
        return "data";
    }
    else if (qualifier.isAddress())
    {
        return "address";
    }
    else if (qualifier.isClock())
    {
        return "clock";
    }
    else if (qualifier.isReset())
    {
        return "reset";
    }
    else 
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type AbsDefTreeModel::toQualifier(QString const& str) const 
{
    if (str == "address")
    {
        return Qualifier::Address;
    }
    else if (str == "data")
    {
        return Qualifier::Data;
    }
    else if (str == "data/address")
    {
        return Qualifier::Data_Address;
    }
    else if (str == "clock")
    {
        return Qualifier::Clock;
    }
    else if (str == "reset")
    {
        return Qualifier::Reset;
    }
    else
    {
        return Qualifier::Any;
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
AbsDefTreeModel::PortAbstractionEntry::PortAbstractionEntry()
{
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
AbsDefTreeModel::PortAbstractionEntry::PortAbstractionEntry(PortAbstractionEntry const& other):
portAbstraction_(other.portAbstraction_),
wirePorts_(other.wirePorts_)
{
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::SignalRow::operator==()
//-----------------------------------------------------------------------------
/*bool AbsDefTreeModel::SignalRow::operator==(SignalRow const& other) const 
{
    return  portAbstraction_->getLogicalName() == other.portAbstraction_->getLogicalName() &&
        mode_ == other.mode_ &&
        (mode_ != General::SYSTEM || wirePort_->getSystemGroup() == other.wirePort_->getSystemGroup());
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool AbsDefTreeModel::SignalRow::operator!=(SignalRow const& other) const 
{
    return  portAbstraction_->getLogicalName() != other.portAbstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        wirePort_->getSystemGroup() != other.wirePort_->getSystemGroup();
}

//-----------------------------------------------------------------------------
// Function: AbsDefTreeModel::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool AbsDefTreeModel::SignalRow::operator<( const SignalRow& other ) const 
{
    // Order by name, mode and system group.
    if (portAbstraction_->getLogicalName() == other.portAbstraction_->getLogicalName()) 
    {
        if (mode_ == other.mode_)
        {
            return wirePort_->getSystemGroup() < other.wirePort_->getSystemGroup();
        }

        return mode_ < other.mode_;
    }
    else
    {
        return portAbstraction_->getLogicalName() < other.portAbstraction_->getLogicalName();
    }
}*/