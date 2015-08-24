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

#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: BusPortsModel::BusPortsModel()
//-----------------------------------------------------------------------------
BusPortsModel::BusPortsModel(QObject *parent): 
QAbstractTableModel(parent),
absDef_(),
ports_(),
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
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant BusPortsModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) 
    {
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
        else if (section == LogicalPortColumns::GROUP)
        {
            return tr("System group");
        }
        else if (section == LogicalPortColumns::PRESENCE)
        {
            return tr("Presence");
        }
        else if (section == LogicalPortColumns::DEFAULT_VALUE)
        {
            return tr("Default");
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
    else // vertical headers.
    {
        if (section > table_.size())
        {
            return QVariant();
        }

        return section + 1;
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

	if (role == Qt::DisplayRole) 
    {
        BusPortsModel::Port const& port = table_.at(index.row());

        if (index.column() == LogicalPortColumns::NAME) 
        {
            if (port.name_.isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return port.name_;
            }
        }
        else if (index.column() == LogicalPortColumns::QUALIFIER)
        {
            return toString(port.qualifier_);
        }
        else if (index.column() == LogicalPortColumns::WIDTH)
        {
            int width = port.width_;

            // if width is unspecified
            if (width <= 0) 
            {
                return QString();
            }
            return width;
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            return General::direction2Str(port.direction_);
        }
        else if (index.column() == LogicalPortColumns::MODE)
        {
            return General::interfaceMode2Str(port.mode_);
        }
        else if (index.column() == LogicalPortColumns::GROUP)
        {
            return port.group_;
        }
        else if (index.column() == LogicalPortColumns::PRESENCE)
        {
            return presence2Str(port.presence_);
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            return port.defaultValue_;
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            return General::driverType2Str(port.driver_);
        }
        else if (index.column() == LogicalPortColumns::DESCRIPTION)
        {
            return port.description_;
        }
        else
        {
            return QVariant();
        }

    }
	else if (role == Qt::BackgroundRole) 
    {
        if (index.column() == LogicalPortColumns::NAME) 
        {
            return QColor("LemonChiffon");
        }

        int topIndex = index.row();

        // find the index that is the first index for this port
        while (topIndex > 0 && topIndex < table_.size()) 
        {
            // if the next port is different port
            if (table_.at(topIndex - 1).name_ != table_.at(index.row()).name_)
            {
                break;
            }

            // if the next port is the same port
            else
            {
                --topIndex;
            }
        }

        // the first item has white back ground
        if (topIndex == 0)
        {
            return QColor("white");
        }

        // index for the previous port
        QModelIndex previous = QAbstractTableModel::index(topIndex - 1, LogicalPortColumns::QUALIFIER,
            QModelIndex());

        // color of the previous port
        QColor previousColor = data(previous, Qt::BackgroundRole).value<QColor>();

        // return a color that is different from previous port
        if (previousColor == QColor("white"))
        {
            return QColor("aliceblue");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ForegroundRole && index.column() == LogicalPortColumns::NAME && 
        table_.at(index.row()).name_.isEmpty())
    {
        return QColor("red");
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setData()
//-----------------------------------------------------------------------------
bool BusPortsModel::setData(QModelIndex const& index, QVariant const& value, int role) 
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }

    if (role == Qt::EditRole) 
    {
        if (index.column() == LogicalPortColumns::NAME)
        {		
            // Make sure there is not already port with same name and mode.
            BusPortsModel::Port temp = table_.at(index.row());
            temp.name_ = value.toString();
            if (table_.contains(temp))
            {
                return false;
            }

            // Don't rename all empty signal names if renaming was done for empty port name.
            if (table_.at(index.row()).name_.isEmpty()) 
            {
                table_[index.row()].name_ = value.toString();
                emit dataChanged(index, index);
            }
            else
            {
                renamePort(table_.at(index.row()).name_, value.toString());
            }

            return true;
        }
        else if (index.column() == LogicalPortColumns::QUALIFIER)
        {
            setQualifier(table_.at(index.row()).name_, value.toString());
            return true;
        }
        else if (index.column() == LogicalPortColumns::WIDTH)
        {
            setWidth(table_.at(index.row()).name_, value.toInt(), table_.at(index.row()).mode_);				
            return true;
        }
        else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
        {
            setDefaultValue(table_.at(index.row()).name_, value.toString());
            return true;
        }
        else if (index.column() == LogicalPortColumns::MODE)
        {
            table_[index.row()].mode_ = General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT);
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == LogicalPortColumns::GROUP)
        {
            table_[index.row()].group_ = value.toString();
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == LogicalPortColumns::DIRECTION)
        {
            table_[index.row()].direction_ = General::str2Direction(value.toString(), General::DIRECTION_INVALID);
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == LogicalPortColumns::PRESENCE)
        {
            table_[index.row()].presence_ = General::str2Presence(value.toString(), General::REQUIRED);
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == LogicalPortColumns::DRIVER)
        {
            setDriver(table_.at(index.row()).name_, value.toString());
            return true;
        }
        else if (index.column() == LogicalPortColumns::DESCRIPTION)
        {
            setDescription(table_.at(index.row()).name_, value.toString());
            return true;
        }
        else
        {
            return false;
        }
    }
    // unsupported role
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void BusPortsModel::setAbsDef( QSharedPointer<AbstractionDefinition> absDef ) 
{
    Q_ASSERT_X(absDef, "BusPortsModel constructor", "Null Abstraction Definition given as parameter");

    absDef_ = absDef;
    ports_ = absDef_->getLogicalPorts();

    readPorts();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::save()
//-----------------------------------------------------------------------------
void BusPortsModel::save() 
{
    ports_->clear();

    for (int i = 0; i < table_.size(); ++i) 
    {
        BusPortsModel::Port portOnRow = table_.at(i);
        if (!table_.at(i).name_.isEmpty())
        {
            QSharedPointer<PortAbstraction> portAbs = QSharedPointer<PortAbstraction>(new PortAbstraction());
            portAbs->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));

            // set the logical name
            portAbs->setName(portOnRow.name_);

            portAbs->getWire()->setQualifier(portOnRow.qualifier_);

            // set the default value element
            if (!portOnRow.defaultValue_.isEmpty()) 
            {
                portAbs->setDefaultValue(portOnRow.defaultValue_);
            }
            if (portOnRow.driver_ != General::NO_DRIVER)
            {
                portAbs->getWire()->setDriverType(portOnRow.driver_);
            }
            else 
            {
                portAbs->getWire()->setRequiresDriver(false);
            }

            // save the current port
            savePort(portAbs, i);

            // Save different modes for the port abstraction.
            while (i < table_.size() - 1 && table_.at(i + 1).name_ == table_.at(i).name_) 
            {
                i++;
                savePort(portAbs, i);
            }

            // set the description for the port
            portAbs->setDescription(portOnRow.description_);

            // append the created port to the list
            ports_->append(portAbs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::addSignal()
//-----------------------------------------------------------------------------
void BusPortsModel::addSignal() 
{
    BusPortsModel::Port port;

    beginInsertRows(QModelIndex(), table_.size(), table_.size());
    table_.prepend(port);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::addSignalOptions()
//-----------------------------------------------------------------------------
void BusPortsModel::addSignalOptions(const QModelIndexList& indexes ) 
{
    QList<BusPortsModel::Port> ports;

    // find all ports that match given indexes
    foreach (QModelIndex index, indexes) 
    {
        if (index.row() >= 0 && index.row() <= table_.size()) 
        {
            // find the port for the index
            BusPortsModel::Port temp = table_.at(index.row());

            // if port is not yet added to the list
            if (!ports.contains(temp))
            {
                ports.append(temp);
            }
        }
    }

    beginResetModel();

    // make other port modes for the port
    foreach (BusPortsModel::Port port, ports) 
    {
        // make copies of the port with different modes
        BusPortsModel::Port master(port);
        master.mode_ = General::MASTER;

        BusPortsModel::Port slave(port);
        slave.mode_ = General::SLAVE;

        BusPortsModel::Port system(port);
        system.mode_ = General::SYSTEM;

        // remove the original port to avoid duplicates
        int index = table_.indexOf(port);
        table_.removeAt(index);

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
// Function: BusPortsModel::removeIndexes()
//-----------------------------------------------------------------------------
void BusPortsModel::removeIndexes(QModelIndexList const& indexes) 
{
    QList<BusPortsModel::Port> portsToRemove;

    // find all ports that match given indexes
    foreach (QModelIndex index, indexes) 
    {
        if (index.row() >= 0 && index.row() <= table_.size()) 
        {
            // find the port for the index
            BusPortsModel::Port temp = table_.at(index.row());

            // if port is not yet added to the list
            if (!portsToRemove.contains(temp))
            {
                portsToRemove.append(temp);
            }
        }
    }

    beginResetModel();

    foreach (BusPortsModel::Port portToRemove, portsToRemove) 
    {
        if (table_.contains(portToRemove)) 
        {
            int index = table_.indexOf(portToRemove);
            table_.removeAt(index);
            emit portRemoved(portToRemove.name_, portToRemove.mode_);
        }
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void BusPortsModel::onRemoveItem(QModelIndex const& index)
{
    if (index.row() >= 0 && index.row() <= table_.size()) 
    {
        BusPortsModel::Port portToRemove = table_.at(index.row());

        QString removedName = portToRemove.name_;
        General::InterfaceMode removedMode = portToRemove.mode_;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        endRemoveRows();

        emit contentChanged();        
        emit portRemoved(removedName, removedMode);      
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::copyIndexes()
//-----------------------------------------------------------------------------
void BusPortsModel::copyIndexes( const QModelIndexList& indexes ) 
{
    QList<BusPortsModel::Port> portsToCopy;

    foreach (QModelIndex index, indexes) 
    {
        if (index.row() >= 0 && index.row() <= table_.size()) 
        {
            BusPortsModel::Port temp = table_.at(index.row());

            // if port is not yet added to the list
            if (!portsToCopy.contains(temp))
            {
                portsToCopy.append(temp);
            }
        }
    }

    beginResetModel();

    foreach (BusPortsModel::Port copiedPort, portsToCopy) 
    {
        BusPortsModel::Port copyPort = copiedPort;
        copyPort.name_ += QString("-copy");

        table_.append(copyPort);
    }

    qSort(table_);

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::exportCSV()
//-----------------------------------------------------------------------------
void BusPortsModel::exportCSV( QString const& path ) 
{
    if (path.isEmpty())
    {
        return;
    }

    QFile file(path);

    // If file can't be opened then cancel.
    if (!file.open(QFile::Truncate | QFile::WriteOnly)) 
    {
        emit errorMessage(tr("Could not open file %1 for writing.").arg(path));
        return;
    }

    QTextStream stream(&file);

    // write the headers for the columns
    stream << "Name;Qualifier;Width;Direction;Mode;Presence;Default;Driver;Comment" << endl;

    // write each port
    foreach (BusPortsModel::Port port, table_) 
    {
        stream << port.name_ << ";";
        stream << toString(port.qualifier_) << ";";
        stream << port.width_ << ";";
        stream << General::direction2Str(port.direction_) << ";";
        stream << General::interfaceMode2Str(port.mode_) << ";";
        stream << port.group_ << ";";
        stream << General::presence2Str(port.presence_) << ";";
        stream << port.defaultValue_ << ";";
        stream << General::driverType2Str(port.driver_) << ";";
        stream << port.description_ << endl;
    }

    // close the file
    file.close();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::importCSV()
//-----------------------------------------------------------------------------
void BusPortsModel::importCSV( QString const& path ) 
{
    if (path.isEmpty())
    {
        return;
    }

    // make sure the path exists and it is to a file, not a directory
    QFileInfo info(path);
    if (!info.exists() || !info.isFile())
    {
        return;
    }

    QFile file(path);

    // if file can't be opened for reading
    if (!file.open(QFile::Truncate | QFile::ReadOnly)) 
    {
        emit errorMessage(tr("Could not open file %1 for reading.").arg(path));
        return;
    }

    QTextStream stream(&file);

    // read the headers from the stream and make sure the file is of correct format
    QString header = stream.readLine(100);
    QStringList headers = header.split(";", QString::KeepEmptyParts);
    if (headers.size() != LogicalPortColumns::COLUMN_COUNT) 
    {
        emit errorMessage(tr("File %1 was not appropriate format to import ports.").arg(path));
        file.close();
        return;
    }

    beginResetModel();

    // read all lines
    while (!stream.atEnd()) 
    {
        QStringList columnsOnLine = stream.readLine().split(";", QString::KeepEmptyParts);
        
        if (columnsOnLine.size() != LogicalPortColumns::COLUMN_COUNT)
        {
            emit noticeMessage(tr("Found invalid port within csv-file import, skipping..."));
            continue;
        }

        BusPortsModel::Port createdPort;
        
        createdPort.name_ = columnsOnLine.at(0);

        createdPort.qualifier_ = toQualifier(columnsOnLine.at(1));

        createdPort.width_ = columnsOnLine.at(2).toInt();

        createdPort.direction_ = General::str2Direction(columnsOnLine.at(3), General::DIRECTION_INVALID);

        createdPort.mode_ = General::str2Interfacemode(columnsOnLine.at(4), General::INTERFACE_MODE_COUNT);

        createdPort.group_ = columnsOnLine.at(5);

        createdPort.presence_ = General::str2Presence(columnsOnLine.at(6), General::REQUIRED);

        createdPort.defaultValue_ = columnsOnLine.at(7);

        createdPort.driver_ = General::str2DriverType(columnsOnLine.at(8), General::ANY);

        createdPort.description_ = columnsOnLine.at(9);

        // before adding port make sure it does not already exist
        if (!table_.contains(createdPort))
        {
            table_.append(createdPort);
        }
    }

    qSort(table_);

    endResetModel();

    file.close();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::savePort()
//-----------------------------------------------------------------------------
void BusPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i) 
{
    BusPortsModel::Port sourcePort = table_.at(i);

    // set the width of the port for the specified element
    if (sourcePort.mode_ == General::MASTER)
    {
        if (!portAbs->getWire()->hasMasterPort())
        {
            portAbs->getWire()->setMasterPort(QSharedPointer<WirePort>(new WirePort()));
        }
        portAbs->getWire()->getMasterPort()->setWidth(QString::number(sourcePort.width_));
        portAbs->getWire()->getMasterPort()->setDirection(sourcePort.direction_);
        portAbs->getWire()->getMasterPort()->setPresence(sourcePort.presence_);

    }
    else if (sourcePort.mode_ == General::SLAVE)
    {
        if (!portAbs->getWire()->hasSlavePort())
        {
            portAbs->getWire()->setSlavePort(QSharedPointer<WirePort>(new WirePort()));
        }
        portAbs->getWire()->getSlavePort()->setWidth(QString::number(sourcePort.width_));
        portAbs->getWire()->getSlavePort()->setDirection(sourcePort.direction_);
        portAbs->getWire()->getSlavePort()->setPresence(sourcePort.presence_);
    }
    else if (sourcePort.mode_ == General::SYSTEM )
    {
        QSharedPointer<WirePort> systemWire(new WirePort());
        systemWire->setSystemGroup(sourcePort.group_);
        systemWire->setWidth(QString::number(sourcePort.width_));
        systemWire->setDirection(sourcePort.direction_);
        systemWire->setPresence(sourcePort.presence_);

        portAbs->getWire()->addSystemPort(systemWire);
    }
    // MODE_ANY
    else
    {
        //portAbs->setAllWidths(table_.at(i).width_);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::readPorts()
//-----------------------------------------------------------------------------
void BusPortsModel::readPorts() 
{
	beginResetModel();
    table_.clear();

	foreach (QSharedPointer<PortAbstraction> portAbs, *ports_) 
    {
		// create port for master
		if (portAbs->getWire()->hasMasterPort()) 
        {
			readPort(portAbs, portAbs->getWire()->getMasterPort(), General::MASTER);
		}

		// create port for slave
		if (portAbs->getWire()->hasSlavePort()) 
        {
			readPort(portAbs, portAbs->getWire()->getSlavePort(), General::SLAVE);
		}

		// create port for system
		foreach (QSharedPointer<WirePort> system, *portAbs->getWire()->getSystemPorts()) 
        {
			readPort(portAbs, system, General::SYSTEM);
		}
	}

	qSort(table_);

	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::readPort()
//-----------------------------------------------------------------------------
void BusPortsModel::readPort(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
    General::InterfaceMode mode) 
{
	Q_ASSERT_X(portAbs, "BusPortsModel::readPort", "Null Port Abstraction pointer given as parameter");
	Q_ASSERT_X(modeSpesific, "BusPortsModel::readPort",	"Null WirePort pointer given as parameter");

	BusPortsModel::Port port;
    
    port.name_ = portAbs->name();

    if (mode == General::SYSTEM)
    {
        port.group_ = modeSpesific->getSystemGroup();
    }

    if (portAbs->getQualifier().isData() && portAbs->getQualifier().isAddress())
    {
        port.qualifier_ = Qualifier::Data_Address;
    }
    else if (portAbs->getQualifier().isAddress())
    {
		port.qualifier_ = Qualifier::Address;
    }
	else if (portAbs->getQualifier().isData())
    {
		port.qualifier_ = Qualifier::Data;
    }
	else if (portAbs->getQualifier().isClock())
    {
		port.qualifier_ = Qualifier::Clock;
    }
	else if (portAbs->getQualifier().isReset())
    {
		port.qualifier_ = Qualifier::Reset;
    }
	else
    {
		port.qualifier_ = Qualifier::Any;
    }

	port.width_ = modeSpesific->getWidth().toInt();

    port.defaultValue_ = portAbs->getDefaultValue();
    
	port.mode_ = mode;

    port.direction_ = modeSpesific->getDirection();

	port.presence_ = modeSpesific->getPresence();

	if (portAbs->requiresDriver()) 
    {
        port.driver_ = portAbs->getWire()->getDriverType();
    }
	else
    {
		port.driver_ = General::NO_DRIVER;        
	}

	port.description_ = portAbs->description();

    // Check for no duplicates.
    if (!table_.contains(port))
    {
        table_.append(port);
    }	
}


//-----------------------------------------------------------------------------
// Function: BusPortsModel::renamePort()
//-----------------------------------------------------------------------------
void BusPortsModel::renamePort(QString const& oldPort, QString const& newPort) 
{
	// Change all ports with given name.
	for (int row = 0; row < table_.size(); row++) 
    {
		if (table_.at(row).name_ == oldPort) 
        {
			table_[row].name_ = newPort;
			QModelIndex ind = index(row, LogicalPortColumns::NAME, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}

    emit portRenamed(oldPort, newPort);

	beginResetModel();
	qSort(table_);
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setQualifier()
//-----------------------------------------------------------------------------
void BusPortsModel::setQualifier( QString const& portName, QString const& qualifier ) 
{
	// Change all qualifiers that have the given port name.
	for (int row = 0; row < table_.size(); row++) 
    {
		if (table_.at(row).name_ == portName) 
        {
			table_[row].qualifier_ = toQualifier(qualifier);
			QModelIndex ind = index(row, LogicalPortColumns::QUALIFIER, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setDefaultValue()
//-----------------------------------------------------------------------------
void BusPortsModel::setDefaultValue(QString const& portName, QString const& defaultValue) 
{
	// Change all default values for given ports.
	for (int row = 0; row < table_.size(); row++) 
    {
        if (table_.at(row).name_ == portName) 
        {
			table_[row].defaultValue_ = defaultValue;
			QModelIndex ind = index(row, LogicalPortColumns::DEFAULT_VALUE, QModelIndex());
			emit dataChanged(ind, ind);

			// Default value and driver are mutually exclusive.
			table_[row].driver_ = General::NO_DRIVER;
			QModelIndex ind2 = index(row, LogicalPortColumns::DRIVER, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setDriver()
//-----------------------------------------------------------------------------
void BusPortsModel::setDriver(QString const& portName, QString const& driverValue) 
{
	// change all drivers for given ports
	for (int row = 0; row < table_.size(); row++) 
    {
		if (table_.at(row).name_ == portName) 
        {
			table_[row].driver_ = General::str2DriverType(driverValue, General::NO_DRIVER);
			QModelIndex ind = index(row, LogicalPortColumns::DRIVER, QModelIndex());
			emit dataChanged(ind, ind);

			// Default value and driver are mutually exclusive.
			table_[row].defaultValue_ = QString();
			QModelIndex ind2 = index(row, LogicalPortColumns::DEFAULT_VALUE, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setDescription()
//-----------------------------------------------------------------------------
void BusPortsModel::setDescription(QString const& portName, QString const& description) 
{
	// Change description for the given ports.
	for (int row = 0; row < table_.size(); row++) 
    {
		if (table_.at(row).name_ == portName) 
        {
			table_[row].description_ = description;
			QModelIndex ind = index(row, LogicalPortColumns::DESCRIPTION, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setWidth()
//-----------------------------------------------------------------------------
void BusPortsModel::setWidth(QString const& portName, const int width, General::InterfaceMode mode)
{
    // change width for the given ports.
    for (int row = 0; row < table_.size(); row++) 
    {
        if (table_.at(row).name_ == portName && table_.at(row).mode_ == mode) 
        {
            table_[row].width_ = width;
            QModelIndex ind = index(row, LogicalPortColumns::WIDTH, QModelIndex());
            emit dataChanged(ind, ind);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::toString()
//-----------------------------------------------------------------------------
QString BusPortsModel::toString(Qualifier::Type qualifier) const 
{
    if (qualifier == Qualifier::Address)
    {
        return "address";
    }
    else if (qualifier == Qualifier::Data)
    {
        return "data";
    }
    else if (qualifier == Qualifier::Data_Address)
    {
        return "data/address";
    }
    else if (qualifier == Qualifier::Clock)
    {
        return "clock";
    }
    else if (qualifier == Qualifier::Reset)
    {
        return "reset";
    }
    else if (qualifier == Qualifier::Any)
    {
        return "any";
    }
    else 
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type BusPortsModel::toQualifier( QString const& str ) const 
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
// Function: BusPortsModel::Port::Port()
//-----------------------------------------------------------------------------
BusPortsModel::Port::Port():
name_(),
    group_(),
    qualifier_(Qualifier::Any),
    width_(0),
    defaultValue_(),
    mode_(General::INTERFACE_MODE_COUNT),
    direction_(General::INOUT),
    presence_(General::REQUIRED),
    driver_(General::NO_DRIVER),
    description_() 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::Port()
//-----------------------------------------------------------------------------
BusPortsModel::Port::Port( const Port& other ):
name_(other.name_),
    group_(other.group_),
    qualifier_(other.qualifier_),
    width_(other.width_),
    defaultValue_(other.defaultValue_),
    mode_(other.mode_),
    direction_(other.direction_),
    presence_(other.presence_),
    driver_(other.driver_),
    description_(other.description_) 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::operator=()
//-----------------------------------------------------------------------------
BusPortsModel::Port& BusPortsModel::Port::operator=( const Port& other ) 
{
	if (this != &other) 
    {
		name_ = other.name_;
        group_ = other.group_;
		qualifier_ = other.qualifier_;
		width_ = other.width_;
		defaultValue_ = other.defaultValue_;
		mode_ = other.mode_;
		direction_ = other.direction_;
		presence_ = other.presence_;
		driver_ = other.driver_;
		description_ = other.description_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::operator==()
//-----------------------------------------------------------------------------
bool BusPortsModel::Port::operator==( const Port& other ) const 
{
	// if names don't match
	if (name_ != other.name_)
    {
		return false;
    }
	else if (mode_ == General::INTERFACE_MODE_COUNT || other.mode_ == General::INTERFACE_MODE_COUNT)
    {
		return true;
    }
    else if (group_ != other.group_)
    {
        return false;
    }
	else if (mode_ != other.mode_)
    {
		return false;
    }
	else
    {
		return true;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::operator!=()
//-----------------------------------------------------------------------------
bool BusPortsModel::Port::operator!=( const Port& other ) const 
{
	if (name_ != other.name_)
    {
		return true;
    }
	else if (mode_ == General::INTERFACE_MODE_COUNT || other.mode_ == General::INTERFACE_MODE_COUNT)
    {
		return false;
    }
    else if (group_ != other.group_)
    {
        return true;
    }
	else if (mode_ != other.mode_)
    {
		return true;
    }
	else
    {
		return false;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::operator<()
//-----------------------------------------------------------------------------
bool BusPortsModel::Port::operator<( const Port& other ) const 
{
	// if names match then order by mode
	if (name_ == other.name_) 
    {
		return mode_ < other.mode_;
	}
	else
    {
		return name_ < other.name_;
    }
}
