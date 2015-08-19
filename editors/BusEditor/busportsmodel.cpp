/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsmodel.cpp
 */

#include "busportsmodel.h"

#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

namespace
{
    //! Enumeration for table columns.
    enum Columns
    {
        NAME_COLUMN = 0,
        QUALIFIER_COLUMN,
        WIDTH_COLUMN,
        DEFAULT_COLUMN,
        MODE_COLUMN,
        DIRECTION_COLUMN,
        PRESENCE_COLUMN,
        DRIVER_COLUMN,
        COMMENT_COLUMN,
        COLUMN_COUNT
    };
}

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

	return COLUMN_COUNT;
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
        if (index.column() == NAME_COLUMN) 
        {
            if (table_.at(index.row()).name_.isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return table_.at(index.row()).name_;
            }
        }
        else if (index.column() == QUALIFIER_COLUMN)
        {
            return portQualifier2Str(table_.at(index.row()).qualifier_);
        }
        else if (index.column() == WIDTH_COLUMN)
        {
            int width = table_.at(index.row()).width_;

            // if width is unspecified
            if (width <= 0) 
            {
                return QString();
            }
            return width;
        }
        else if (index.column() == DEFAULT_COLUMN)
        {
            return table_.at(index.row()).defaultValue_;
        }
        else if (index.column() == MODE_COLUMN)
        {
            return portMode2Str(table_.at(index.row()).mode_);
        }
        else if (index.column() == DIRECTION_COLUMN)
        {
            return portDirection2Str(table_.at(index.row()).direction_);
        }
        else if (index.column() == PRESENCE_COLUMN)
        {
            return presence2Str(table_.at(index.row()).presence_);
        }
        else if (index.column() == DRIVER_COLUMN)
        {
            return portDriver2Str(table_.at(index.row()).driver_);
        }
        else if (index.column() == COMMENT_COLUMN)
        {
            return table_.at(index.row()).comment_;
        }
        else
        {
            return QVariant();
        }

    }
	else if (role == Qt::BackgroundRole) 
    {
        if (index.column() == NAME_COLUMN) 
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
        QModelIndex previous = QAbstractTableModel::index(--topIndex, QUALIFIER_COLUMN, QModelIndex());

        // color of the previous port
        QColor previousColor = data(previous, Qt::BackgroundRole).value<QColor>();

        // return a color that is different from previous port
        if (previousColor == QColor("white"))
        {
            return QColor("lightgrey");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == NAME_COLUMN && table_.at(index.row()).name_.isEmpty())
        {
            return QColor("red");
        }
    }

    return QVariant();
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
       if (section == NAME_COLUMN)
       {		   
           return tr("Name");
       }
       else if (section == QUALIFIER_COLUMN)
       {
           return tr("Qualifier");
       }
       else if (section ==  WIDTH_COLUMN)
       {
           return tr("Width");
       }
       else if (section ==  DEFAULT_COLUMN)
       {
           return tr("Default");
       }
       else if (section ==  MODE_COLUMN)
       {
           return tr("Mode");
       }
       else if (section == DIRECTION_COLUMN)
       {
           return tr("Direction");
       }
       else if (section == PRESENCE_COLUMN)
       {
           return tr("Presence");
       }
       else if (section == DRIVER_COLUMN)
       {
           return tr("Driver");
       }
       else if (section == COMMENT_COLUMN)
       {
           return tr("Description");
       }
       else
       {
           return QVariant();
       }
   }
   else // vertical headers
   {
       if (section > table_.size())
       {
           return QVariant();
       }

       return section + 1;
   }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portMode2Str()
//-----------------------------------------------------------------------------
QString BusPortsModel::portMode2Str( PortMode mode ) const 
{
	switch (mode)
    {
		case BusPortsModel::MODE_MASTER: 
			return tr("master");
		case BusPortsModel::MODE_SLAVE: 
			return tr("slave");
		case BusPortsModel::MODE_SYSTEM:
			return tr("system");
		case  BusPortsModel::MODE_ANY:
			return tr("any");
		default:
			return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::str2PortMode()
//-----------------------------------------------------------------------------
BusPortsModel::PortMode BusPortsModel::str2PortMode( const QString& str ) const 
{
	if (str == QString("master"))
    {
		return BusPortsModel::MODE_MASTER;
    }
	else if (str == QString("slave"))
    {
		return BusPortsModel::MODE_SLAVE;
    }
	else if (str == QString("system"))
    {
		return BusPortsModel::MODE_SYSTEM;
    }
	else
    {
		return BusPortsModel::MODE_ANY;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portMode2Mode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusPortsModel::portMode2Mode(const PortMode mode) const
{
    switch (mode)
    {
    case BusPortsModel::MODE_MASTER:
        return General::MASTER;
    case BusPortsModel::MODE_SLAVE:
        return General::SLAVE;
    case BusPortsModel::MODE_SYSTEM:
        return General::SYSTEM;
    case BusPortsModel::MODE_ANY:
    default:
        return General::INTERFACE_MODE_COUNT;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portDirection2Str()
//-----------------------------------------------------------------------------
QString BusPortsModel::portDirection2Str( PortDirection direction ) const 
{
	switch (direction) {
		case BusPortsModel::DIRECTION_IN:
			return tr("in");
		case BusPortsModel::DIRECTION_OUT:
			return tr("out");
		case BusPortsModel::DIRECTION_INOUT:
			return tr("inout");
		default:
			return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::str2PortDirection()
//-----------------------------------------------------------------------------
BusPortsModel::PortDirection BusPortsModel::str2PortDirection( const QString& str ) const 
{
	if (str == QString("in"))
    {
		return BusPortsModel::DIRECTION_IN;
    }
	else if (str == QString("out"))
    {
		return BusPortsModel::DIRECTION_OUT;
    }
	else
    {
		return BusPortsModel::DIRECTION_INOUT;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portDriver2Str()
//-----------------------------------------------------------------------------
QString BusPortsModel::portDriver2Str( PortDriver driver ) const 
{
	switch (driver) {
		case BusPortsModel::DRIVER_NONE:
			return tr("none");
		case BusPortsModel::DRIVER_ANY:
			return tr("any");
		case BusPortsModel::DRIVER_CLOCK:
			return tr("clock");
		case BusPortsModel::DRIVER_SINGLESHOT:
			return tr("singleshot");
		default:
			return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::str2PortDriver()
//-----------------------------------------------------------------------------
BusPortsModel::PortDriver BusPortsModel::str2PortDriver( const QString& str ) const 
{
	if (str == QString("any"))
    {
		return BusPortsModel::DRIVER_ANY;
    }
	else if (str == QString("clock"))
    {
		return BusPortsModel::DRIVER_CLOCK;
    }
	else if (str == QString("singleshot"))
    {
		return BusPortsModel::DRIVER_SINGLESHOT;
    }
	else
    {
		return BusPortsModel::DRIVER_NONE;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portQualifier2Str()
//-----------------------------------------------------------------------------
QString BusPortsModel::portQualifier2Str( PortQualifier qualifier ) const 
{
	switch ( qualifier) {
		case BusPortsModel::QUALIFIER_ADDRESS:
			return tr("address");
		case BusPortsModel::QUALIFIER_DATA:
			return tr("data");
		case BusPortsModel::QUALIFIER_CLOCK:
			return tr("clock");
		case BusPortsModel::QUALIFIER_RESET:
			return tr("reset");
		case BusPortsModel::QUALIFIER_ANY:
			return tr("any");
		default:
			return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::str2PortQualifier()
//-----------------------------------------------------------------------------
BusPortsModel::PortQualifier BusPortsModel::str2PortQualifier( const QString& str ) const 
{
	if (str == QString("address"))
    {
		return BusPortsModel::QUALIFIER_ADDRESS;
    }
	else if (str == QString("data"))
    {
		return BusPortsModel::QUALIFIER_DATA;
    }
	else if (str == QString("clock"))
    {
		return BusPortsModel::QUALIFIER_CLOCK;
    }
	else if (str == QString("reset"))
    {
		return BusPortsModel::QUALIFIER_RESET;
    }
	else
    {
		return BusPortsModel::QUALIFIER_ANY;
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::save()
//-----------------------------------------------------------------------------
void BusPortsModel::save() 
{
	// remove previous ports
	ports_->clear();

	for (int i = 0; i < table_.size(); ++i) 
    {
		// do not add ports with empty name
		if (table_.at(i).name_.isEmpty())
        {
			continue;
        }

		QSharedPointer<PortAbstraction> portAbs = QSharedPointer<PortAbstraction>(new PortAbstraction());

		// set the logical name
		portAbs->setName(table_.at(i).name_);

		// set the qualifier for the port
		switch (table_.at(i).qualifier_) {
			case BusPortsModel::QUALIFIER_ADDRESS: {
				portAbs->setQualifier(Qualifier::Address);
				break;
												   }
			case BusPortsModel::QUALIFIER_DATA: {
				portAbs->setQualifier(Qualifier::Data);
				break;
												   }
			case BusPortsModel::QUALIFIER_CLOCK: {
				portAbs->setQualifier(Qualifier::Clock);
				break;
												   }
			case BusPortsModel::QUALIFIER_RESET: {
				portAbs->setQualifier(Qualifier::Reset);
				break;
												   }
			 // qualifier is any
			default: {
				portAbs->setQualifier(Qualifier::Reset);
				break;
					 }

		}

		// set the default value element
		if (!table_.at(i).defaultValue_.isEmpty()) 
        {
			portAbs->setDefaultValue(table_.at(i).defaultValue_);
		}

		// set the driver settings for the port
		switch (table_.at(i).driver_) {
			case BusPortsModel::DRIVER_ANY: {
				portAbs->getWire()->setDriverType(General::ANY);
				break;
											}
			case BusPortsModel::DRIVER_CLOCK: {
				portAbs->getWire()->setDriverType(General::CLOCK);
				break;
											  }
			case BusPortsModel::DRIVER_SINGLESHOT: {
				portAbs->getWire()->setDriverType(General::SINGLESHOT);
				break;
												   }
			// BusPortsModel::DRIVER_NONE
			default: {
				portAbs->getWire()->setRequiresDriver(false);
					 }
		}

		// save the current port
		savePort(portAbs, i);

		// while there are ports defined for same port abstraction but with 
		// different modes
		while (i < table_.size()-1 && table_.at(i + 1).name_ == table_.at(i).name_) 
        {
			++i;
			savePort(portAbs, i);
		}

		// set the description for the port
		portAbs->setDescription(table_.at(i).comment_);

		// append the created port to the list
		ports_->append(portAbs);
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::savePort()
//-----------------------------------------------------------------------------
void BusPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i) 
{
	// set the width of the port for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->getWire()->getMasterPort()->setWidth(QString::number(table_.at(i).width_));
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->getWire()->getSlavePort()->setWidth(QString::number(table_.at(i).width_));
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->getWire()->getSystemPorts()->first()->setWidth(QString::number(table_.at(i).width_));
				break;
											 }
			// MODE_ANY
			default: {
				//portAbs->setAllWidths(table_.at(i).width_);
				break;
					 }
	}

	// set the direction of the port for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->getWire()->getMasterPort()->setDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->getWire()->getSlavePort()->setDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->getWire()->getSystemPorts()->first()->setDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											 }
			// MODE_ANY
			default: {
				//portAbs->setAllDirections(portDirection2Direction(table_.at(i).direction_));
				break;
					 }
	}

	// set the presence for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->getWire()->getMasterPort()->setPresence(table_.at(i).presence_);
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->getWire()->getSlavePort()->setPresence(table_.at(i).presence_);
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->getWire()->getSystemPorts()->first()->setPresence(table_.at(i).presence_);
				break;
											 }
			// MODE_ANY
			default: {
				//portAbs->setAllPresences(table_.at(i).presence_);
				break;
					 }
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
			readPort(portAbs, portAbs->getWire()->getMasterPort(), BusPortsModel::MODE_MASTER);
		}

		// create port for slave
		if (portAbs->getWire()->hasSlavePort()) 
        {
			readPort(portAbs, portAbs->getWire()->getSlavePort(), BusPortsModel::MODE_SLAVE);
		}

		// create port for system
		if (!portAbs->getWire()->getSystemPorts()->isEmpty()) 
        {
			readPort(portAbs, portAbs->getWire()->getSystemPorts()->first(), BusPortsModel::MODE_SYSTEM);
		}
	}

	// sort the table
	qSort(table_);

	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::readPort()
//-----------------------------------------------------------------------------
void BusPortsModel::readPort(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
    BusPortsModel::PortMode mode) 
{
	Q_ASSERT_X(portAbs, "BusPortsModel::readPort", "Null Port Abstraction pointer given as parameter");
	Q_ASSERT_X(modeSpesific, "BusPortsModel::readPort",	"Null WirePort pointer given as parameter");

	BusPortsModel::Port port(portAbs->name());
    
	// set the qualifier
	if (portAbs->getQualifier().isAddress())
    {
		port.qualifier_ = BusPortsModel::QUALIFIER_ADDRESS;
    }
	else if (portAbs->getQualifier().isData())
    {
		port.qualifier_ = BusPortsModel::QUALIFIER_DATA;
    }
	else if (portAbs->getQualifier().isClock())
    {
		port.qualifier_ = BusPortsModel::QUALIFIER_CLOCK;
    }
	else if (portAbs->getQualifier().isReset())
    {
		port.qualifier_ = BusPortsModel::QUALIFIER_RESET;
    }
	else
    {
		port.qualifier_ = BusPortsModel::QUALIFIER_ANY;
    }

	// set the width
	port.width_ = modeSpesific->getWidth().toInt();

	// if default value has not been defined
	if (portAbs->getDefaultValue() < 0)
    {
		port.defaultValue_ = QString();
    }
	else
    {
		port.defaultValue_ = portAbs->getDefaultValue();
    }

	// set the mode
	port.mode_ = mode;

    // set the direction of the port
    if (modeSpesific->getDirection() == General::IN)
    {
        port.direction_ = BusPortsModel::DIRECTION_IN;
    }
    else if (modeSpesific->getDirection() == General::OUT)
    {
        port.direction_ = BusPortsModel::DIRECTION_OUT;
    }
    else
    {
        port.direction_ = BusPortsModel::DIRECTION_INOUT;
    }


	// set the presence for the port
	port.presence_ = modeSpesific->getPresence();

	// set the driver setting for the port

	// if port does not require driver
	if (!portAbs->requiresDriver()) 
    {
		port.driver_ = BusPortsModel::DRIVER_NONE;
    }
	
	// if driver is needed then find out it's type
	else {
		switch (portAbs->getWire()->getDriverType()) {
			case General::CLOCK: {
				port.driver_ = BusPortsModel::DRIVER_CLOCK;
				break;
								 }
			case General::SINGLESHOT: {
				port.driver_ = BusPortsModel::DRIVER_SINGLESHOT;
				break;
									  }
			default: {
				port.driver_ = BusPortsModel::DRIVER_ANY;
				break;
					 }
		}
	}

	port.comment_ = portAbs->description();

    // Check for no duplicates.
    if (!table_.contains(port))
    {
        table_.append(port);
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
		master.mode_ = BusPortsModel::MODE_MASTER;
		BusPortsModel::Port slave(port);
		slave.mode_ = BusPortsModel::MODE_SLAVE;
		BusPortsModel::Port system(port);
		system.mode_ = BusPortsModel::MODE_SYSTEM;

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

	// sort the table to keep ports in order
	qSort(table_);

	endResetModel();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setData()
//-----------------------------------------------------------------------------
bool BusPortsModel::setData(const QModelIndex& index, 
							const QVariant& value, 
							int role /*= Qt::EditRole*/) 
{
	if (!index.isValid())
    {
		return false;
    }
	else if (index.row() < 0 || index.row() >= table_.size())
    {
		return false;
    }

	if (role == Qt::EditRole) 
    {
		switch (index.column()) {
			case NAME_COLUMN: {
				
				// make sure there is not already port with same name and mode
				BusPortsModel::Port temp = table_.at(index.row());
				temp.name_ = value.toString();
				if (table_.contains(temp))
                {
					return false;
                }

				// don't rename all empty signal names if renaming was done for
				// empty port name
				if (table_.at(index.row()).name_.isEmpty()) 
                {
					table_[index.row()].name_ = value.toString();
					emit dataChanged(index, index);
					return true;
				}

				renamePort(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case QUALIFIER_COLUMN: {
				setQualifier(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case WIDTH_COLUMN: {
                setWidth(table_.at(index.row()).name_, value.toInt(), table_.at(index.row()).mode_);				
				return true;
					}
			case DEFAULT_COLUMN: {
				setDefaultValue(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case MODE_COLUMN: {
				table_[index.row()].mode_ = str2PortMode(value.toString());
				emit dataChanged(index, index);
				return true;
					}
			case DIRECTION_COLUMN: {
				table_[index.row()].direction_ = str2PortDirection(value.toString());
				emit dataChanged(index, index);
				return true;
					}
			case PRESENCE_COLUMN: {
				table_[index.row()].presence_ = str2Presence(value.toString(), General::REQUIRED);
				emit dataChanged(index, index);
				return true;
					}
			case DRIVER_COLUMN: {
				setDriver(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case COMMENT_COLUMN: {
				setComment(table_.at(index.row()).name_, value.toString());
				return true;
					}
			default: {
				return false;
					 }
		}

		
	}
	// unsupported role
	else
    {
		return false;
    }
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
// Function: BusPortsModel::renamePort()
//-----------------------------------------------------------------------------
void BusPortsModel::renamePort( const QString oldPort, const QString newPort ) 
{
	// change all ports with given name
	for (int i = 0; i < table_.size(); ++i) 
    {
		if (table_.at(i).name_ == oldPort) 
        {
			table_[i].name_ = newPort;
			QModelIndex ind = index(i, NAME_COLUMN, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}

    emit portRenamed(oldPort, newPort);

	// sort the model to keep it ordered
	beginResetModel();
	qSort(table_);
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setQualifier()
//-----------------------------------------------------------------------------
void BusPortsModel::setQualifier( const QString& portName, const QString& qualifier ) 
{
	// change all qualifiers that have the given port name
	for (int i = 0; i < table_.size(); ++i) 
    {
		if (table_.at(i).name_ == portName) 
        {
			table_[i].qualifier_ = str2PortQualifier(qualifier);
			QModelIndex ind = index(i, QUALIFIER_COLUMN, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setDefaultValue()
//-----------------------------------------------------------------------------
void BusPortsModel::setDefaultValue( const QString& portName, const QString& defaultValue) 
{
	// change all default values for given ports
	for (int i = 0; i < table_.size(); ++i) 
    {
        if (table_.at(i).name_ == portName) 
        {
			table_[i].defaultValue_ = defaultValue;
			QModelIndex ind = index(i, DEFAULT_COLUMN, QModelIndex());
			emit dataChanged(ind, ind);

			// default value and driver are mutually exclusive
			table_[i].driver_ = BusPortsModel::DRIVER_NONE;
			QModelIndex ind2 = index(i, DRIVER_COLUMN, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setDriver()
//-----------------------------------------------------------------------------
void BusPortsModel::setDriver(const QString& portName, const QString& driverValue) 
{
	// change all drivers for given ports
	for (int i = 0; i < table_.size(); ++i) 
    {
		if (table_.at(i).name_ == portName) 
        {
			table_[i].driver_ = str2PortDriver(driverValue);
			QModelIndex ind = index(i, DRIVER_COLUMN, QModelIndex());
			emit dataChanged(ind, ind);

			// default value and driver are mutually exclusive
			table_[i].defaultValue_ = QString();
			QModelIndex ind2 = index(i, DEFAULT_COLUMN, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setComment()
//-----------------------------------------------------------------------------
void BusPortsModel::setComment(const QString& portName, const QString& comment) 
{
	// change comments for the given ports
	for (int i = 0; i < table_.size(); ++i) 
    {
		if (table_.at(i).name_ == portName) 
        {
			table_[i].comment_ = comment;
			QModelIndex ind = index(i, COMMENT_COLUMN, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setWidth()
//-----------------------------------------------------------------------------
void BusPortsModel::setWidth(const QString& portName, const int width, PortMode mode)
{
    // change width for the given ports.
    for (int i = 0; i < table_.size(); ++i) 
    {
        if (table_.at(i).name_ == portName && table_.at(i).mode_ == mode) 
        {
            table_[i].width_ = width;
            QModelIndex ind = index(i, WIDTH_COLUMN, QModelIndex());
            emit dataChanged(ind, ind);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void BusPortsModel::setAbsDef( QSharedPointer<AbstractionDefinition> absDef ) 
{
	Q_ASSERT_X(absDef, "BusPortsModel constructor",
		"Null Abstraction Definition pointer given as parameter");

	absDef_ = absDef;

	ports_ = absDef_->getLogicalPorts();

	readPorts();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::portDirection2Direction()
//-----------------------------------------------------------------------------
General::Direction BusPortsModel::portDirection2Direction( PortDirection direction ) const 
{
	switch (direction) {
		case BusPortsModel::DIRECTION_IN: {
			return General::IN;
										  }
		case BusPortsModel::DIRECTION_OUT: {
			return General::OUT;
										   }
		// BusPortsModel::DIRENTION:INOUT
		default: {
			return General::INOUT;
				 }
	}
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::removeIndexes()
//-----------------------------------------------------------------------------
void BusPortsModel::removeIndexes(const QModelIndexList& indexes ) 
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

	foreach (BusPortsModel::Port port, ports) 
    {
		if (table_.contains(port)) 
        {
			int index = table_.indexOf(port);
			table_.removeAt(index);
            emit portRemoved(port.name_, portMode2Mode(port.mode_));
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
        // find the port for the index
        BusPortsModel::Port port = table_.at(index.row());

        if (table_.contains(port)) 
        {
            QString portName = port.name_;
            BusPortsModel::PortMode portMode = port.mode_;
            beginRemoveRows(QModelIndex(), index.row(), index.row());
                int index = table_.indexOf(port);
            table_.removeAt(index);
            endRemoveRows();

            emit contentChanged();        
            emit portRemoved(portName, portMode2Mode(portMode));
        }       
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::copyIndexes()
//-----------------------------------------------------------------------------
void BusPortsModel::copyIndexes( const QModelIndexList& indexes ) 
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

	// make copy of each port
	foreach (BusPortsModel::Port port, ports) 
    {
		// create a copy of the port and change the name to contain "-copy"
		BusPortsModel::Port copyPort = port;
		copyPort.name_ += QString("-copy");

		table_.append(copyPort);
	}

	// sort the table to keep ports in order
	qSort(table_);

	endResetModel();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::exportCSV()
//-----------------------------------------------------------------------------
void BusPortsModel::exportCSV( const QString& path ) 
{
	if (path.isEmpty())
    {
		return;
    }

	QFile file(path);
	 
	// if file can't be opened then quit
	if (!file.open(QFile::Truncate | QFile::WriteOnly)) 
    {
		emit errorMessage(tr("Could not open file %1 for writing.").arg(path));
		return;
	}

	QTextStream stream(&file);

	// write the headers for the columns
	stream << "Name;Qualifier;Width;Default;Mode;Direction;Presence;Driver;Comment" << endl;

	// write each port
	foreach (BusPortsModel::Port port, table_) 
    {
		stream << port.name_ << ";";
		stream << portQualifier2Str(port.qualifier_) << ";";
		stream << port.width_ << ";";
		stream << port.defaultValue_ << ";";
		stream << portMode2Str(port.mode_) << ";";
		stream << portDirection2Str(port.direction_) << ";";
		stream << presence2Str(port.presence_) << ";";
		stream << portDriver2Str(port.driver_) << ";";
		stream << port.comment_ << endl;
	}

	// close the file
	file.close();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::importCSV()
//-----------------------------------------------------------------------------
void BusPortsModel::importCSV( const QString& path ) 
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
	if (headers.size() != 9) 
    {
		emit errorMessage(tr("File %1 was not appropriate format to import ports.").arg(
			path));
		file.close();
		return;
	}

	beginResetModel();

	// read all lines
	while (!stream.atEnd()) 
    {
		QString line = stream.readLine(0);
		QStringList settings = line.split(";", QString::KeepEmptyParts);
		// if not all settings are found then skip port
		if (settings.size() != 9) 
        {
			emit noticeMessage(tr("Found invalid port within csv-file import, skipping..."));
			continue;
		}

		// create port with a name
		BusPortsModel::Port port(settings.at(0));

		// set the qualifier
		port.qualifier_ = str2PortQualifier(settings.at(1));

		// set width
		port.width_ = settings.at(2).toInt();

		// set default value
		port.defaultValue_ = settings.at(3);

		// set mode
		port.mode_ = str2PortMode(settings.at(4));

		// set direction
		port.direction_ = str2PortDirection(settings.at(5));

		// set presence
		port.presence_ = str2Presence(settings.at(6), General::REQUIRED);

		// set the driver
		port.driver_ = str2PortDriver(settings.at(7));

		// set the comment
		port.comment_ = settings.at(8);

		// before adding port make sure it does not already exist
		if (!table_.contains(port))
        {
			table_.append(port);
        }
	}

	// last sort the table
	qSort(table_);

	endResetModel();
	
	// close the file
	file.close();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::Port()
//-----------------------------------------------------------------------------
BusPortsModel::Port::Port():
name_(),
qualifier_(BusPortsModel::QUALIFIER_ANY),
width_(0),
defaultValue_(),
mode_(BusPortsModel::MODE_ANY),
direction_(BusPortsModel::DIRECTION_INOUT),
presence_(General::REQUIRED),
driver_(BusPortsModel::DRIVER_NONE),
comment_() 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::Port()
//-----------------------------------------------------------------------------
BusPortsModel::Port::Port( const QString& name ):
name_(name),
qualifier_(BusPortsModel::QUALIFIER_ANY),
width_(0),
defaultValue_(),
mode_(BusPortsModel::MODE_ANY),
direction_(BusPortsModel::DIRECTION_INOUT),
presence_(General::REQUIRED),
driver_(BusPortsModel::DRIVER_NONE),
comment_() 
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsModel::Port::Port()
//-----------------------------------------------------------------------------
BusPortsModel::Port::Port( const Port& other ):
name_(other.name_),
qualifier_(other.qualifier_),
width_(other.width_),
defaultValue_(other.defaultValue_),
mode_(other.mode_),
direction_(other.direction_),
presence_(other.presence_),
driver_(other.driver_),
comment_(other.comment_) 
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
		qualifier_ = other.qualifier_;
		width_ = other.width_;
		defaultValue_ = other.defaultValue_;
		mode_ = other.mode_;
		direction_ = other.direction_;
		presence_ = other.presence_;
		driver_ = other.driver_;
		comment_ = other.comment_;
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
	
	// if either port has mode ANY and the names matched
	else if (mode_ == BusPortsModel::MODE_ANY || other.mode_ == BusPortsModel::MODE_ANY)
    {
		return true;
    }

	// if modes don't match and 
	else if (mode_ != other.mode_)
    {
		return false;
    }

	// if names matched and ports matched
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
	// if names don't match
	if (name_ != other.name_)
    {
		return true;
    }

	// if either port has mode ANY and the names matched
	else if (mode_ == BusPortsModel::MODE_ANY || other.mode_ == BusPortsModel::MODE_ANY)
    {
		return false;
    }

	// if modes don't match and 
	else if (mode_ != other.mode_)
    {
		return true;
    }

	// if names matched and ports matched
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


General::Presence BusPortsModel::str2Presence(QString str, General::Presence defaultValue) {

        // identify the correct enum value
        if (str == QString("illegal")) {
            return General::ILLEGAL;
        }

        else if (str == QString("required")) {
            return General::REQUIRED;
        }

        else if (str == QString("optional")) {
            return General::OPTIONAL;
        }
        else {
            return defaultValue;
        }
}

QString BusPortsModel::presence2Str(General::Presence presence) const {
    switch (presence) {
    case WirePort::ILLEGAL: {
        return QString("illegal");
                           }
    case WirePort::REQUIRED: {
        return QString("required");
                            }
    case WirePort::OPTIONAL: {
        return QString("optional");
                            }
    default: {
        return QString();
             }
    }
}