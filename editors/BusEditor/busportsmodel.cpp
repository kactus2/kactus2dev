//-----------------------------------------------------------------------------
// File: busportsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// Data model for the wires within abstraction definition.
//-----------------------------------------------------------------------------

#include "busportsmodel.h"
#include "LogicalPortColumns.h"

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QColor>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: BusPortsModel::BusPortsModel()
//-----------------------------------------------------------------------------
BusPortsModel::BusPortsModel(QObject *parent): 
QAbstractTableModel(parent),
absDef_(),
busDefinition_(),
table_() 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::~BusPortsModel()
//-----------------------------------------------------------------------------
BusPortsModel::~BusPortsModel() 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::rowCount()
//-----------------------------------------------------------------------------
int BusPortsModel::rowCount(QModelIndex const& parent) const 
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_.size();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::columnCount()
//-----------------------------------------------------------------------------
int BusPortsModel::columnCount(QModelIndex const& parent) const 
{
	if (parent.isValid())
    {
		return 0;
    }

	return LogicalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BusPortsModel::flags(const QModelIndex& index) const 
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
// Function: BusPortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant BusPortsModel::headerData(int section, Qt::Orientation orientation, int role) const 
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
// Function: BusPortsModel::data()
//-----------------------------------------------------------------------------
QVariant BusPortsModel::data(QModelIndex const& index, int role) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
    }
            
    BusPortsModel::SignalRow const& port = table_.at(index.row());

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
	else if (role == Qt::BackgroundRole) 
    {
        if (index.column() == LogicalPortColumns::NAME ||
            (index.column() == LogicalPortColumns::SYSTEM_GROUP && port.mode_ == General::SYSTEM)) 
        {
            return QColor("LemonChiffon");
        }
        
        int topIndex = index.row();

        // Find the index that is the first index for this port.
        while (0 < topIndex && topIndex < table_.size()) 
        {
            if (table_.at(topIndex - 1).abstraction_->getLogicalName() != 
                table_.at(index.row()).abstraction_->getLogicalName())
            {
                break;
            }
            else
            {
                topIndex--;
            }
        }

        // The first item has always white background.
        if (topIndex == 0)
        {
            return QColor("white");
        }

        QModelIndex previousPort = QAbstractTableModel::index(topIndex - 1, LogicalPortColumns::QUALIFIER,
            QModelIndex());

        QColor previousColor = data(previousPort, Qt::BackgroundRole).value<QColor>();

        // Return a color that is different from previous port.
        if (previousColor == QColor("white"))
        {
            return QColor("aliceblue");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == LogicalPortColumns::NAME && port.abstraction_->getLogicalName().isEmpty()) ||
            (index.column() == LogicalPortColumns::MODE && port.mode_ == General::INTERFACE_MODE_COUNT))
        {
            return QColor("red");
        }
        else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
        {
            if (port.mode_ != General::SYSTEM || !busDefinition_ || 
                !busDefinition_->getSystemGroupNames().contains(port.wire_->getSystemGroup()))
            {
                return QColor("red");
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
// Function: BusPortsModel::setData()
//-----------------------------------------------------------------------------
bool BusPortsModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole)
    {
        return false;
    }

    BusPortsModel::SignalRow& port = table_[index.row()];

    if (index.column() == LogicalPortColumns::NAME)
    {		
        QString newName = value.toString();

        // Make sure there is not already port with same name.
        if (!absDef_->getPort(newName).isNull())
        {
            return false;
        }

        QString oldName = port.abstraction_->getLogicalName();
        port.abstraction_->setLogicalName(newName);        

        emit portRenamed(oldName, newName);
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
        port.wire_->setDirection(DirectionTypes::str2Direction(value.toString(), DirectionTypes::DIRECTION_INVALID));    }
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

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void BusPortsModel::setAbsDef(QSharedPointer<AbstractionDefinition> absDef) 
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

    qSort(table_);
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setBusDef()
//-----------------------------------------------------------------------------
void BusPortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::save()
//-----------------------------------------------------------------------------
void BusPortsModel::save() 
{
    for (int i = 0; i < table_.size(); ++i) 
    {
        BusPortsModel::SignalRow portOnRow = table_.at(i);

        QSharedPointer<PortAbstraction> portAbs = portOnRow.abstraction_;
        portAbs->getWire()->setMasterPort(QSharedPointer<WirePort>());
        portAbs->getWire()->setSlavePort(QSharedPointer<WirePort>());
        portAbs->getWire()->getSystemPorts()->clear();

        // Save the port for the first mode.
        savePort(portAbs, i);

        // Save different modes for the port abstraction.
        while (i < table_.size() - 1 && table_.at(i + 1).abstraction_ == table_.at(i).abstraction_) 
        {
            i++;
            savePort(portAbs, i);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::addSignal()
//-----------------------------------------------------------------------------
void BusPortsModel::addSignal() 
{
    BusPortsModel::SignalRow port;
    port.abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    port.abstraction_->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    port.wire_ = QSharedPointer<WirePort>(new WirePort());

    absDef_->getLogicalPorts()->append(port.abstraction_);

    beginInsertRows(QModelIndex(), 0, 0);
    table_.prepend(port);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::addSignalOptions()
//-----------------------------------------------------------------------------
void BusPortsModel::addSignalOptions(QModelIndexList const& indexes) 
{
    QList<BusPortsModel::SignalRow> targetPorts;

    // Find all ports that match given indexes.
    foreach (QModelIndex index, indexes) 
    {
        if (0 <= index.row() && index.row() <= table_.size()) 
        {
            BusPortsModel::SignalRow temp = table_.at(index.row());
            if (!targetPorts.contains(temp))
            {
                targetPorts.append(temp);
            }
        }
    }

    beginResetModel();

    // Make other port modes for the ports.
    foreach (BusPortsModel::SignalRow port, targetPorts) 
    {
        // Make copies of the port with different modes.
        BusPortsModel::SignalRow master(port);
        master.mode_ = General::MASTER;
        master.wire_->setSystemGroup("");

        BusPortsModel::SignalRow slave(port);
        slave.mode_ = General::SLAVE;
        slave.wire_->setSystemGroup("");

        BusPortsModel::SignalRow system(port);
        system.mode_ = General::SYSTEM;
        system.wire_->setSystemGroup("");

        // add the ports that are not already in the table
        if (!table_.contains(master))
        {
            table_.append(master);
        }
        if (!table_.contains(slave))
        {
            table_.append(slave);
        }
        if (!table_.contains(system))
        {
            table_.append(system);
        }
    }

    qSort(table_);

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void BusPortsModel::onRemoveItem(QModelIndex const& index)
{
    if (0 <= index.row() && index.row() <= table_.size()) 
    {
        BusPortsModel::SignalRow portToRemove = table_.at(index.row());

        QString removedName = portToRemove.abstraction_->getLogicalName();
        General::InterfaceMode removedMode = portToRemove.mode_;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        
        bool removeAbstraction = true;
        foreach (BusPortsModel::SignalRow const& row, table_)
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
// Function: BusPortsModel::savePort()
//-----------------------------------------------------------------------------
void BusPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i) 
{
    BusPortsModel::SignalRow sourcePort = table_.at(i);

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
// Function: BusPortsModel::createRow()
//-----------------------------------------------------------------------------
void BusPortsModel::createRow(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
    General::InterfaceMode mode) 
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");
    Q_ASSERT_X(modeSpesific, "BusPortsModel::createRow", "Null WirePort pointer given as parameter");

	BusPortsModel::SignalRow port;
    port.mode_ = mode;
    port.abstraction_ = portAbs;
    port.wire_ = modeSpesific;
    
    table_.append(port);
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::toString()
//-----------------------------------------------------------------------------
QString BusPortsModel::toString(Qualifier qualifier) const 
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
// Function: BusPortsModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type BusPortsModel::toQualifier(QString const& str) const 
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
// Function: BusPortsModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
BusPortsModel::SignalRow::SignalRow():
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
    mode_(General::INTERFACE_MODE_COUNT),
    wire_(QSharedPointer<WirePort>(new WirePort()))
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
BusPortsModel::SignalRow::SignalRow(SignalRow const& other):
abstraction_(other.abstraction_),
    mode_(other.mode_),
    wire_(QSharedPointer<WirePort>(new WirePort(*other.wire_)))
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::SignalRow::operator==()
//-----------------------------------------------------------------------------
bool BusPortsModel::SignalRow::operator==(SignalRow const& other) const 
{
    return  abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
        mode_ == other.mode_ &&
        (mode_ != General::SYSTEM || wire_->getSystemGroup() == other.wire_->getSystemGroup());
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool BusPortsModel::SignalRow::operator!=(SignalRow const& other) const 
{
    return  abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        wire_->getSystemGroup() != other.wire_->getSystemGroup();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool BusPortsModel::SignalRow::operator<( const SignalRow& other ) const 
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
