//-----------------------------------------------------------------------------
// File: AbstractionWirePortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// Data model for the wires within abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionWirePortsModel.h"
#include "LogicalPortColumns.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <common/KactusColors.h>

#include <QStringList>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::AbstractionWirePortsModel()
//-----------------------------------------------------------------------------
AbstractionWirePortsModel::AbstractionWirePortsModel(LibraryInterface* libraryAccess, QObject *parent):
QAbstractTableModel(parent),
absDef_(),
busDefinition_(),
table_(),
libraryAccess_(libraryAccess)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionWirePortsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_.size();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionWirePortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return LogicalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionWirePortsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    // Do not allow setting system groups for other than system mode.
    if (index.column() == LogicalPortColumns::SYSTEM_GROUP && 
        table_.at(index.row()).mode_ != General::SYSTEM &&
        table_.at(index.row()).wire_->getSystemGroup().isEmpty())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionWirePortsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionWirePortsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
    }
            
    AbstractionWirePortsModel::SignalRow const& port = table_.at(index.row());

	if (role == Qt::DisplayRole) 
    {
        if (index.column() == LogicalPortColumns::NAME) 
        {
            if (port.abstraction_->getLogicalName().isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return port.abstraction_->getLogicalName();
            }
        }
        else if (index.column() == LogicalPortColumns::QUALIFIER)
        {
            return toString(port.abstraction_->getWire()->getQualifier());
        }
        else if (index.column() == LogicalPortColumns::WIDTH)
        {
            return port.wire_->getWidth();
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            return DirectionTypes::direction2Str(port.wire_->getDirection());
        }
        else if (index.column() == LogicalPortColumns::MODE)
        {
            return General::interfaceMode2Str(port.mode_);
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            return port.wire_->getSystemGroup();
        }
        else if (index.column() == LogicalPortColumns::PRESENCE)
        {
            return presence2Str(port.wire_->getPresence());
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return port.abstraction_->getWire()->getDefaultValue();
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            return General::driverType2Str(port.abstraction_->getWire()->getDriverType());
        }
        else if (index.column() == LogicalPortColumns::DESCRIPTION)
        {
            return port.abstraction_->description();
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == LogicalPortColumns::NAME && port.abstraction_->getLogicalName().isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE && 
                (port.mode_ == General::INTERFACE_MODE_COUNT || table_.count(port) > 1)))
        {
            return  KactusColors::ERROR;
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            if (!busDefinition_ || (port.mode_ == General::SYSTEM &&
                !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).contains(
                    port.wire_->getSystemGroup())))
            {
                return  KactusColors::ERROR;
            }
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole ||
        oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    AbstractionWirePortsModel::SignalRow& port = table_[index.row()];
    if (index.column() == LogicalPortColumns::NAME)
    {		
        QString newName = value.toString();

        QSharedPointer<PortAbstraction> selectedPort = absDef_->getPort(newName);
        if (!selectedPort)
        {
            selectedPort = QSharedPointer<PortAbstraction>(new PortAbstraction());
            selectedPort->setLogicalName(newName);
            absDef_->getLogicalPorts()->append(selectedPort);
        }
        if (!selectedPort->getWire())
        {
            selectedPort->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
        }

        if (!portHasOtherSignals(port))
        {
            absDef_->getLogicalPorts()->removeOne(port.abstraction_);
        }

        port.abstraction_ = selectedPort;

    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        port.abstraction_->getWire()->setQualifier(toQualifier(value.toString()));
    }
    else if (index.column() == LogicalPortColumns::WIDTH)
    {
        port.wire_->setWidth(value.toString());            
    }
    else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        port.abstraction_->getWire()->setDefaultValue(value.toString());
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        port.mode_ = General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT);
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        port.wire_->setSystemGroup(value.toString());
    }
    else if (index.column() == LogicalPortColumns::DIRECTION)
    {
        port.wire_->setDirection(DirectionTypes::str2Direction(value.toString(),
            DirectionTypes::DIRECTION_INVALID)); 
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        port.wire_->setPresence(PresenceTypes::str2Presence(value.toString(), PresenceTypes::UNKNOWN));
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        port.abstraction_->getWire()->setDriverType(General::str2DriverType(value.toString(), General::NO_DRIVER));
    }
    else if (index.column() == LogicalPortColumns::DESCRIPTION)
    {
        port.abstraction_->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    //! Indexing doesn't work correctly (select other than first index).
    QModelIndexList indexList;
    indexList.append(index);

    if (index.column() == LogicalPortColumns::NAME || index.column() == LogicalPortColumns::DEFAULT_VALUE ||
        index.column() == LogicalPortColumns::DRIVER || index.column() == LogicalPortColumns::QUALIFIER ||
        index.column() == LogicalPortColumns::DESCRIPTION)
    {
        for (int i = 0; i < table_.size(); ++i)
        {
            AbstractionWirePortsModel::SignalRow signal = table_.at(i);

            if (signal.abstraction_->getLogicalName().compare(port.abstraction_->getLogicalName()) == 0 &&
                signal != port)
            {
                QModelIndex signalIndex = index.sibling(i, index.column());
                indexList.append(signalIndex);
            }
        }
    }

    sendDataChangeForAllChangedItems(indexList);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::portHasOtherSignals()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::portHasOtherSignals(AbstractionWirePortsModel::SignalRow const& portSignal) const
{
    General::InterfaceMode portMode = portSignal.mode_;
    QSharedPointer<WireAbstraction> portWire = portSignal.abstraction_->getWire();

    if (portMode == General::MASTER)
    {
        return portWire->hasSlavePort() || portWire->getSystemPorts()->count() > 0;
    }
    else if (portMode == General::SLAVE)
    {
        return portWire->hasMasterPort() || portWire->getSystemPorts()->count() > 0;
    }
    else
    {
        return portWire->hasMasterPort() || portWire->hasSlavePort() || portWire->getSystemPorts()->count() > 1;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
{
    QModelIndex firstIndex = changedIndexes.first();
    QModelIndex lastIndex = changedIndexes.last();

    foreach (QModelIndex currentIndex, changedIndexes)
    {
        if (currentIndex.row() < firstIndex.row())
        {
            firstIndex = currentIndex;
        }
        else if (currentIndex.row() > lastIndex.row())
        {
            lastIndex = currentIndex;
        }
    }

    emit dataChanged(firstIndex, lastIndex);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    Q_ASSERT_X(absDef, "BusPortsModel::setAbsDef", "Null Abstraction Definition given as parameter");

    absDef_ = absDef;

    beginResetModel();
    table_.clear();

    foreach (QSharedPointer<PortAbstraction> portAbs, *absDef_->getLogicalPorts()) 
    {
        if (portAbs->hasWire())
        {
            bool hasValidPort = false;

            if (portAbs->getWire()->hasMasterPort()) 
            {
                createRow(portAbs, portAbs->getWire()->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }

            if (portAbs->getWire()->hasSlavePort()) 
            {
                createRow(portAbs, portAbs->getWire()->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }

            foreach (QSharedPointer<WirePort> system, *portAbs->getWire()->getSystemPorts()) 
            {
                createRow(portAbs, system, General::SYSTEM);
                hasValidPort = true;
            }

            if (!hasValidPort)
            {
                createRow(portAbs, QSharedPointer<WirePort>(new WirePort()), General::INTERFACE_MODE_COUNT);
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::save()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::save()
{
    QVector<QSharedPointer<PortAbstraction> > savedPorts;

    for (int i = 0; i < table_.size(); i++) 
    {
        AbstractionWirePortsModel::SignalRow portOnRow = table_.at(i);

        QSharedPointer<PortAbstraction> portAbs = portOnRow.abstraction_;

        if (!savedPorts.contains(portAbs))
        {
            portAbs->getWire()->setMasterPort(QSharedPointer<WirePort>());
            portAbs->getWire()->setSlavePort(QSharedPointer<WirePort>());
            portAbs->getWire()->getSystemPorts()->clear();

            // Save the port for the first mode.
            savePort(portAbs, i);

            // Save different modes for the port abstraction.
            for (int j = i + 1; j < table_.size(); j++)
            {
                if (table_.at(j).abstraction_ == portAbs)
                {                    
                    savePort(portAbs, j);
                }
            }

            savedPorts.append(portAbs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSignal()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSignal()
{
    AbstractionWirePortsModel::SignalRow port;
    port.abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    port.abstraction_->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    port.wire_ = QSharedPointer<WirePort>(new WirePort());

    absDef_->getLogicalPorts()->append(port.abstraction_);

    beginInsertRows(QModelIndex(), table_.count(), table_.count());
    table_.append(port);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addMaster()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addMaster(QModelIndexList const& indexes)
{
    createNewSignal(General::MASTER, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSlave()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSlave(QModelIndexList const& indexes)
{
    createNewSignal(General::SLAVE, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::createNewSignal()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::createNewSignal(General::InterfaceMode newSignalMode, QModelIndexList const& selection)
{
    QVector<AbstractionWirePortsModel::SignalRow> targetPorts = getIndexedPorts(selection);

    beginResetModel();

    foreach(AbstractionWirePortsModel::SignalRow signal, targetPorts)
    {
        if (!modeExistsForPort(newSignalMode, signal.abstraction_->getLogicalName()))
        {
            AbstractionWirePortsModel::SignalRow newSignal = constructCopySignal(signal);
            newSignal.mode_ = newSignalMode;
            newSignal.wire_->setSystemGroup("");

            General::InterfaceMode opposingSignal = General::MASTER;
            if (newSignalMode == General::MASTER)
            {
                opposingSignal = General::SLAVE;
            }

            DirectionTypes::Direction mirroredDirection =
                getMirroredDirectionForSignal(signal.abstraction_->getLogicalName(), opposingSignal);
            if (mirroredDirection != DirectionTypes::DIRECTION_INVALID)
            {
                newSignal.wire_->setDirection(mirroredDirection);
            }

            table_.append(newSignal);
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::constructCopySignal()
//-----------------------------------------------------------------------------
AbstractionWirePortsModel::SignalRow AbstractionWirePortsModel::constructCopySignal(
    AbstractionWirePortsModel::SignalRow signal) const
{
    AbstractionWirePortsModel::SignalRow newSignal;
    newSignal.abstraction_ = signal.abstraction_;
    newSignal.wire_ = QSharedPointer<WirePort>(new WirePort());

    return newSignal;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addSystem(QModelIndexList const& indexes)
{
    QVector<AbstractionWirePortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);
    
    beginResetModel();

    foreach(AbstractionWirePortsModel::SignalRow signal, targetPorts)
    {
        AbstractionWirePortsModel::SignalRow newSystemSignal = constructCopySignal(signal);
        newSystemSignal.mode_ = General::SYSTEM;
        newSystemSignal.wire_->setSystemGroup("");

        table_.append(newSystemSignal);
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<AbstractionWirePortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);

    beginResetModel();

    foreach(AbstractionWirePortsModel::SignalRow signal, targetPorts)
    {
        QStringList systemGroups = getMissingSystemGroupsForSignal(signal);
        foreach(QString group, systemGroups)
        {
            AbstractionWirePortsModel::SignalRow newSystemSignal = constructCopySignal(signal);
            newSystemSignal.mode_ = General::SYSTEM;
            newSystemSignal.wire_->setSystemGroup(group);

            table_.append(newSystemSignal);
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionWirePortsModel::getMissingSystemGroupsForSignal(AbstractionWirePortsModel::SignalRow const& signal)
    const
{
    QStringList missingSystemGroups;
    if (busDefinition_)
    {
        missingSystemGroups = busDefinition_->getSystemGroupNames();
    }

    foreach(AbstractionWirePortsModel::SignalRow const& comparisonSignal, table_)
    {
        if (comparisonSignal.abstraction_->getLogicalName() == signal.abstraction_->getLogicalName() &&
            comparisonSignal.mode_ == General::SYSTEM)
        {
            missingSystemGroups.removeAll(comparisonSignal.wire_->getSystemGroup());
        }
    }

    return missingSystemGroups;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::getIndexedPorts()
//-----------------------------------------------------------------------------
QVector<AbstractionWirePortsModel::SignalRow> AbstractionWirePortsModel::getIndexedPorts(QModelIndexList const& selection)
{
    QVector<AbstractionWirePortsModel::SignalRow> targetPorts;

    // Find all ports that match given indexes.
    foreach(QModelIndex index, selection)
    {
        if (0 <= index.row() && index.row() <= table_.size())
        {
            AbstractionWirePortsModel::SignalRow temp = table_.at(index.row());
            if (!portHasBeenSelected(temp, targetPorts))
            {
                targetPorts.append(temp);
            }
        }
    }

    return targetPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::portHasBeenSelected()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::portHasBeenSelected(AbstractionWirePortsModel::SignalRow const& portSignal,
    QVector<AbstractionWirePortsModel::SignalRow> const& selectedPorts) const
{
    foreach(AbstractionWirePortsModel::SignalRow const& signal, selectedPorts)
    {
        if (signal.abstraction_->getLogicalName() == portSignal.abstraction_->getLogicalName())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::modeExistsForPort()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::modeExistsForPort(General::InterfaceMode const& mode, QString const& portName) const
{
    foreach(AbstractionWirePortsModel::SignalRow signal, table_)
    {
        if (signal.abstraction_->getLogicalName().compare(portName) == 0 && signal.mode_ == mode)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::getMirroredDirectionForSignal()
//-----------------------------------------------------------------------------
DirectionTypes::Direction AbstractionWirePortsModel::getMirroredDirectionForSignal(QString const& portName,
    General::InterfaceMode const& opposingMode) const
{
    foreach(AbstractionWirePortsModel::SignalRow signal, table_)
    {
        if (signal.mode_ == opposingMode && signal.abstraction_->getLogicalName().compare(portName) == 0)
        {
            return DirectionTypes::convert2Mirrored(signal.wire_->getDirection());
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::onRemoveItem(QModelIndex const& index)
{
    if (0 <= index.row() && index.row() <= table_.size()) 
    {
        AbstractionWirePortsModel::SignalRow portToRemove = table_.at(index.row());

        QString removedName = portToRemove.abstraction_->getLogicalName();
        General::InterfaceMode removedMode = portToRemove.mode_;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        
        bool removeAbstraction = true;
        foreach (AbstractionWirePortsModel::SignalRow const& row, table_)
        {
            if (row.abstraction_ == portToRemove.abstraction_)
            {
                removeAbstraction = false;
            }
        }

        if (removeAbstraction)
        {
            absDef_->getLogicalPorts()->removeOne(portToRemove.abstraction_);
        }

        endRemoveRows();

        emit contentChanged();
        emit portRemoved(removedName, removedMode);      
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::savePort()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i)
{
    AbstractionWirePortsModel::SignalRow sourcePort = table_.at(i);

    // Set the wirePort placement according to the mode in the table.
    if (sourcePort.mode_ == General::MASTER)
    {        
        portAbs->getWire()->setMasterPort(sourcePort.wire_);
    }
    else if (sourcePort.mode_ == General::SLAVE)
    {
        portAbs->getWire()->setSlavePort(sourcePort.wire_);
    }
    else if (sourcePort.mode_ == General::SYSTEM)
    {
        portAbs->getWire()->addSystemPort(sourcePort.wire_);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::createRow()
//-----------------------------------------------------------------------------
void AbstractionWirePortsModel::createRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<WirePort> modeSpesific, General::InterfaceMode mode) 
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");
    Q_ASSERT_X(modeSpesific, "BusPortsModel::createRow", "Null WirePort pointer given as parameter");

    AbstractionWirePortsModel::SignalRow port;
    port.mode_ = mode;
    port.abstraction_ = portAbs;
    port.wire_ = modeSpesific;
    
    table_.append(port);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::toString()
//-----------------------------------------------------------------------------
QString AbstractionWirePortsModel::toString(Qualifier qualifier) const
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
// Function: AbstractionWirePortsModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type AbstractionWirePortsModel::toQualifier(QString const& str) const
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
// Function: AbstractionWirePortsModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
AbstractionWirePortsModel::SignalRow::SignalRow():
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
mode_(General::INTERFACE_MODE_COUNT),
wire_(QSharedPointer<WirePort>(new WirePort()))
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::SignalRow::operator==()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::SignalRow::operator==(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
        mode_ == other.mode_ &&
        (mode_ != General::SYSTEM || wire_->getSystemGroup() == other.wire_->getSystemGroup());
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::SignalRow::operator!=(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        wire_->getSystemGroup() != other.wire_->getSystemGroup();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsModel::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsModel::SignalRow::operator<( const SignalRow& other ) const
{
	// Order by name, mode and system group.
	if (abstraction_->getLogicalName() == other.abstraction_->getLogicalName()) 
    {
        if (mode_ == other.mode_)
        {
            return wire_->getSystemGroup() < other.wire_->getSystemGroup();
        }

		return mode_ < other.mode_;
	}
	else
    {
		return abstraction_->getLogicalName() < other.abstraction_->getLogicalName();
    }
}
