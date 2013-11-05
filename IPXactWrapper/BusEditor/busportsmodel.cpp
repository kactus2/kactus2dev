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

#include <QDebug>

BusPortsModel::BusPortsModel(QObject *parent): 
QAbstractTableModel(parent),
absDef_(),
ports_(),
table_() {

}

BusPortsModel::~BusPortsModel() {
}

int BusPortsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {

	if (parent.isValid())
		return 0;
	return table_.size();
}

int BusPortsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {

	if (parent.isValid())
		return 0;
	return 9;
}

QVariant BusPortsModel::data( const QModelIndex& index, 
							 int role /*= Qt::DisplayRole*/ ) const {

	if (!index.isValid())
		return QVariant();
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
		case 0: {
			return table_.at(index.row()).name_;
				}
		case 1: {
			return portQualifier2Str(table_.at(index.row()).qualifier_);
				}
		case 2: {
			int width = table_.at(index.row()).width_;
			
			// if width is unspecified
			if (width <= 0) {
				return QString();
			}
			return width;
				}
		case 3: {
			return table_.at(index.row()).defaultValue_;
				}
		case 4: {
			return portMode2Str(table_.at(index.row()).mode_);
				}
		case 5: {
			return portDirection2Str(table_.at(index.row()).direction_);
				}
		case 6: {
			return General::presence2Str(table_.at(index.row()).presence_);
				}
		case 7: {
			return portDriver2Str(table_.at(index.row()).driver_);
				}
		case 8: {
			return table_.at(index.row()).comment_;
				}
		default: {
			return QVariant();
					}
		}
	}
	else if (role == Qt::BackgroundRole) {
        if (index.column() == 0) {
            return QColor("LemonChiffon");
        }

        int topIndex = index.row();

        // find the index that is the first index for this port
        while (topIndex > 0 && topIndex < table_.size()) {

            // if the next port is different port
            if (table_.at(topIndex - 1).name_ != table_.at(index.row()).name_)
                break;

            // if the next port is the same port
            else
                --topIndex;
        }

        // the first item has white back ground
        if (topIndex == 0)
            return QColor("white");

        // index for the previous port
        QModelIndex previous = QAbstractTableModel::index(--topIndex, 1, QModelIndex());

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

    else
    {
        return QVariant();
    }
}

QVariant BusPortsModel::headerData( int section,
								   Qt::Orientation orientation,
								   int role /*= Qt::DisplayRole*/ ) const {

   if (role != Qt::DisplayRole)
	   return QVariant();
   else if (section < 0)
	   return QVariant();

   if (role == Qt::DisplayRole) {
	   if (orientation == Qt::Horizontal) {

		   switch (section) {
		   case 0: {
			   return tr("Name");
				   }
		   case 1: {
			   return tr("Qualifier");
				   }
		   case 2: {
			   return tr("Width");
				   }
		   case 3: {
			   return tr("Default");
				   }
		   case 4: {
			   return tr("Mode");
				   }
		   case 5: {
			   return tr("Direction");
				   }
		   case 6: {
			   return tr("Presence");
				   }
		   case 7: {
			   return tr("Driver");
				   }
		   case 8: {
			   return tr("Comment");
				   }
		   default: {
			   return QVariant();
					}
		   }
	   }
	   // vertical headers
	   else {

		   // provide horizontal headers only for each defined port
		   if (section > table_.size())
			   return QVariant();

		   return section + 1;
	   }
   }
   // if unsupported role
   else
	   return QVariant();
}

QString BusPortsModel::portMode2Str( PortMode mode ) const {
	switch (mode) {
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

BusPortsModel::PortMode BusPortsModel::str2PortMode( const QString& str ) const {

	if (str == QString("master"))
		return BusPortsModel::MODE_MASTER;
	else if (str == QString("slave"))
		return BusPortsModel::MODE_SLAVE;
	else if (str == QString("system"))
		return BusPortsModel::MODE_SYSTEM;
	else
		return BusPortsModel::MODE_ANY;
}

QString BusPortsModel::portDirection2Str( PortDirection direction ) const {

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

BusPortsModel::PortDirection BusPortsModel::str2PortDirection( const QString& str ) const {

	if (str == QString("in"))
		return BusPortsModel::DIRECTION_IN;
	else if (str == QString("out"))
		return BusPortsModel::DIRECTION_OUT;
	else
		return BusPortsModel::DIRECTION_INOUT;
}

QString BusPortsModel::portDriver2Str( PortDriver driver ) const {

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

BusPortsModel::PortDriver BusPortsModel::str2PortDriver( const QString& str ) const {

	if (str == QString("any"))
		return BusPortsModel::DRIVER_ANY;
	else if (str == QString("clock"))
		return BusPortsModel::DRIVER_CLOCK;
	else if (str == QString("singleshot"))
		return BusPortsModel::DRIVER_SINGLESHOT;
	else
		return BusPortsModel::DRIVER_NONE;
}

QString BusPortsModel::portQualifier2Str( PortQualifier qualifier ) const {

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

BusPortsModel::PortQualifier BusPortsModel::str2PortQualifier( const QString& str ) const {

	if (str == QString("address"))
		return BusPortsModel::QUALIFIER_ADDRESS;
	else if (str == QString("data"))
		return BusPortsModel::QUALIFIER_DATA;
	else if (str == QString("clock"))
		return BusPortsModel::QUALIFIER_CLOCK;
	else if (str == QString("reset"))
		return BusPortsModel::QUALIFIER_RESET;
	else
		return BusPortsModel::QUALIFIER_ANY;
}

void BusPortsModel::save() {

	// remove previous ports
	ports_->clear();

	for (int i = 0; i < table_.size(); ++i) {

		// do not add ports with empty name
		if (table_.at(i).name_.isEmpty())
			continue;

		QSharedPointer<PortAbstraction> portAbs = QSharedPointer<PortAbstraction>(
			new PortAbstraction());

		// set the logical name
		portAbs->setLogicalName(table_.at(i).name_);

		// set the qualifier for the port
		switch (table_.at(i).qualifier_) {
			case BusPortsModel::QUALIFIER_ADDRESS: {
				portAbs->setAsAddress();
				break;
												   }
			case BusPortsModel::QUALIFIER_DATA: {
				portAbs->setAsData();
				break;
												   }
			case BusPortsModel::QUALIFIER_CLOCK: {
				portAbs->setAsClock();
				break;
												   }
			case BusPortsModel::QUALIFIER_RESET: {
				portAbs->setAsReset();
				break;
												   }
			 // qualifier is any
			default: {
				portAbs->clearQualifier();
				break;
					 }

		}

		// set the default value element
		if (!table_.at(i).defaultValue_.isEmpty()) {
			portAbs->setDefaultValue(table_.at(i).defaultValue_.toInt());
		}

		// set the driver settings for the port
		switch (table_.at(i).driver_) {
			case BusPortsModel::DRIVER_ANY: {
				portAbs->setDriverType(General::ANY);
				break;
											}
			case BusPortsModel::DRIVER_CLOCK: {
				portAbs->setDriverType(General::CLOCK);
				break;
											  }
			case BusPortsModel::DRIVER_SINGLESHOT: {
				portAbs->setDriverType(General::SINGLESHOT);
				break;
												   }
			// BusPortsModel::DRIVER_NONE
			default: {
				portAbs->setRequiresDriver(false);
					 }
		}

		// save the current port
		savePort(portAbs, i);

		// while there are ports defined for same port abstraction but with 
		// different modes
		while (i < table_.size()-1 && table_.at(i + 1).name_ == table_.at(i).name_) {
			++i;
			savePort(portAbs, i);
		}

		// set the description for the port
		portAbs->setDescription(table_.at(i).comment_);

		// append the created port to the list
		ports_->append(portAbs);
	}
}

void BusPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i) {
	// set the width of the port for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->setMasterWidth(table_.at(i).width_);
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->setSlaveWidth(table_.at(i).width_);
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->setSystemWidth(table_.at(i).width_);
				break;
											 }
			// MODE_ANY
			default: {
				portAbs->setAllWidths(table_.at(i).width_);
				break;
					 }
	}

	// set the direction of the port for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->setMasterDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->setSlaveDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->setSystemDirection(portDirection2Direction(table_.at(i).direction_));
				break;
											 }
			// MODE_ANY
			default: {
				portAbs->setAllDirections(portDirection2Direction(table_.at(i).direction_));
				break;
					 }
	}

	// set the presence for the specified element
	switch (table_.at(i).mode_) {
			case BusPortsModel::MODE_MASTER: {
				portAbs->setMasterPresence(table_.at(i).presence_);
				break;
											 }
			case BusPortsModel::MODE_SLAVE: {
				portAbs->setSlavePresence(table_.at(i).presence_);
				break;
											}
			case BusPortsModel::MODE_SYSTEM: {
				portAbs->setSystemPresence(table_.at(i).presence_);
				break;
											 }
			// MODE_ANY
			default: {
				portAbs->setAllPresences(table_.at(i).presence_);
				break;
					 }
	}
}

void BusPortsModel::readPorts() {

	beginResetModel();
	for (int i = 0; i < ports_->size(); ++i) {
		QSharedPointer<PortAbstraction> portAbs = ports_->at(i);
		if (!portAbs)
			continue;

		// create port for master
		if (portAbs->hasMaster()) {
			readPort(portAbs, portAbs->getMaster(), BusPortsModel::MODE_MASTER);
		}

		// create port for slave
		if (portAbs->hasSlave()) {
			readPort(portAbs, portAbs->getSlave(), BusPortsModel::MODE_SLAVE);
		}

		// create port for system
		if (portAbs->hasSystem()) {
			readPort(portAbs, portAbs->getSystem(), BusPortsModel::MODE_SYSTEM);
		}
	}

	// sort the table
	qSort(table_);

	endResetModel();
}

void BusPortsModel::readPort( QSharedPointer<PortAbstraction> portAbs, 
							 WireAbstraction::WirePort* modeSpesific,
							 BusPortsModel::PortMode mode) {

	Q_ASSERT_X(portAbs, "BusPortsModel::readPort",
		"Null Port Abstraction pointer given as parameter");
	Q_ASSERT_X(modeSpesific, "BusPortsModel::readPort",
		"Null WirePort pointer given as parameter");

	BusPortsModel::Port port(portAbs->getLogicalName());

	// set the qualifier
	if (portAbs->isAddress())
		port.qualifier_ = BusPortsModel::QUALIFIER_ADDRESS;
	else if (portAbs->isData())
		port.qualifier_ = BusPortsModel::QUALIFIER_DATA;
	else if (portAbs->isClock())
		port.qualifier_ = BusPortsModel::QUALIFIER_CLOCK;
	else if (portAbs->isReset())
		port.qualifier_ = BusPortsModel::QUALIFIER_RESET;
	else
		port.qualifier_ = BusPortsModel::QUALIFIER_ANY;

	// set the width
	port.width_ = modeSpesific->width_;

	// if default value has not been defined
	if (portAbs->getDefaultValue() < 0)
		port.defaultValue_ = QString();
	else
		port.defaultValue_ = QString::number(portAbs->getDefaultValue());

	// set the mode
	port.mode_ = mode;

	// set the direction of the port
	switch (modeSpesific->direction_) {
		case General::IN: {
			port.direction_ = BusPortsModel::DIRECTION_IN;
			break;
						  }
		case General::OUT: {
			port.direction_ = BusPortsModel::DIRECTION_OUT;
			break;
						   }
		default: {
			port.direction_ = BusPortsModel::DIRECTION_INOUT;
			break;
				 }
	}

	// set the presence for the port
	port.presence_ = modeSpesific->presence_;

	// set the driver setting for the port

	// if port does not require driver
	if (!portAbs->getRequiresDriver()) 
		port.driver_ = BusPortsModel::DRIVER_NONE;
	
	// if driver is needed then find out it's type
	else {
		switch (portAbs->getDriverType()) {
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

	port.comment_ = portAbs->getDescription();
	table_.append(port);
}

void BusPortsModel::addSignal() {

	BusPortsModel::Port port;

	beginInsertRows(QModelIndex(), table_.size(), table_.size());
	table_.prepend(port);
	endInsertRows();

	emit contentChanged();
}

void BusPortsModel::addSignalOptions(const QModelIndexList& indexes ) {
	QList<BusPortsModel::Port> ports;

	// find all ports that match given indexes
	foreach (QModelIndex index, indexes) {

		if (index.row() >= 0 && index.row() <= table_.size()) {

			// find the port for the index
			BusPortsModel::Port temp = table_.at(index.row());

			// if port is not yet added to the list
			if (!ports.contains(temp))
				ports.append(temp);
		}
	}

	beginResetModel();

	// make other port modes for the port
	foreach (BusPortsModel::Port port, ports) {

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
			table_.append(master);
		if (!table_.contains(slave))
			table_.append(slave);
		if (!table_.contains(system))
			table_.append(system);
	}

	// sort the table to keep ports in order
	qSort(table_);

	endResetModel();

	emit contentChanged();
}

bool BusPortsModel::setData(const QModelIndex& index, 
							const QVariant& value, 
							int role /*= Qt::EditRole*/) {

	if (!index.isValid())
		return false;
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {

		switch (index.column()) {
			case 0: {
				
				// make sure there is not already port with same name and mode
				BusPortsModel::Port temp = table_.at(index.row());
				temp.name_ = value.toString();
				if (table_.contains(temp))
					return false;

				// don't rename all empty signal names if renaming was done for
				// empty port name
				if (table_.at(index.row()).name_.isEmpty()) {
					table_[index.row()].name_ = value.toString();
					emit dataChanged(index, index);
					return true;
				}

				renamePort(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case 1: {
				setQualifier(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case 2: {
				table_[index.row()].width_ = value.toInt();
				emit dataChanged(index, index);
				return true;
					}
			case 3: {
				setDefaultValue(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case 4: {
				table_[index.row()].mode_ = str2PortMode(value.toString());
				emit dataChanged(index, index);
				return true;
					}
			case 5: {
				table_[index.row()].direction_ = str2PortDirection(value.toString());
				emit dataChanged(index, index);
				return true;
					}
			case 6: {
				table_[index.row()].presence_ = General::str2Presence(value.toString(), General::REQUIRED);
				emit dataChanged(index, index);
				return true;
					}
			case 7: {
				setDriver(table_.at(index.row()).name_, value.toString());
				return true;
					}
			case 8: {
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
		return false;
}

Qt::ItemFlags BusPortsModel::flags(const QModelIndex& index) const {

	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BusPortsModel::renamePort( const QString oldPort, const QString newPort ) {

	// change all ports with given name
	for (int i = 0; i < table_.size(); ++i) {

		if (table_.at(i).name_ == oldPort) {
			table_[i].name_ = newPort;
			QModelIndex ind = index(i, 0, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}

	// sort the model to keep it ordered
	beginResetModel();
	qSort(table_);
	endResetModel();
}

void BusPortsModel::setQualifier( const QString& portName, const QString& qualifier ) {

	// change all qualifiers that have the given port name
	for (int i = 0; i < table_.size(); ++i) {

		if (table_.at(i).name_ == portName) {
			table_[i].qualifier_ = str2PortQualifier(qualifier);
			QModelIndex ind = index(i, 1, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

void BusPortsModel::setDefaultValue( const QString& portName, const QString& defaultValue ) {

	// change all default values for given ports
	for (int i = 0; i < table_.size(); ++i) {

		if (table_.at(i).name_ == portName) {
			table_[i].defaultValue_ = defaultValue;
			QModelIndex ind = index(i, 3, QModelIndex());
			emit dataChanged(ind, ind);

			// default value and driver are mutually exclusive
			table_[i].driver_ = BusPortsModel::DRIVER_NONE;
			QModelIndex ind2 = index(i, 7, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

void BusPortsModel::setDriver(const QString& portName, const QString& driverValue) {

	// change all drivers for given ports
	for (int i = 0; i < table_.size(); ++i) {

		if (table_.at(i).name_ == portName) {
			table_[i].driver_ = str2PortDriver(driverValue);
			QModelIndex ind = index(i, 7, QModelIndex());
			emit dataChanged(ind, ind);

			// default value and driver are mutually exclusive
			table_[i].defaultValue_ = QString();
			QModelIndex ind2 = index(i, 3, QModelIndex());
			emit dataChanged(ind2, ind2);
		}
	}
}

void BusPortsModel::setComment(const QString& portName, const QString& comment) {

	// change comments for the given ports
	for (int i = 0; i < table_.size(); ++i) {

		if (table_.at(i).name_ == portName) {
			table_[i].comment_ = comment;
			QModelIndex ind = index(i, 8, QModelIndex());
			emit dataChanged(ind, ind);
		}
	}
}

void BusPortsModel::setAbsDef( QSharedPointer<AbstractionDefinition> absDef ) {

	Q_ASSERT_X(absDef, "BusPortsModel constructor",
		"Null Abstraction Definition pointer given as parameter");

	absDef_ = absDef;

	ports_ = &absDef_->getPorts();

	readPorts();
}

General::Direction BusPortsModel::portDirection2Direction( PortDirection direction ) const {
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

void BusPortsModel::removeIndexes(const QModelIndexList& indexes ) {

	QList<BusPortsModel::Port> ports;

	// find all ports that match given indexes
	foreach (QModelIndex index, indexes) {
		
		if (index.row() >= 0 && index.row() <= table_.size()) {
			
			// find the port for the index
			BusPortsModel::Port temp = table_.at(index.row());

			// if port is not yet added to the list
			if (!ports.contains(temp))
				ports.append(temp);
		}
	}

	beginResetModel();

	foreach (BusPortsModel::Port port, ports) {
		if (table_.contains(port)) {
			int index = table_.indexOf(port);
			table_.removeAt(index);
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
    if (index.row() >= 0 && index.row() <= table_.size()) {
        // find the port for the index
        BusPortsModel::Port port = table_.at(index.row());

        if (table_.contains(port)) {
            beginRemoveRows(QModelIndex(), index.row(), index.row());
                int index = table_.indexOf(port);
            table_.removeAt(index);
            endRemoveRows();

            emit contentChanged();        
        }       
    }
}

void BusPortsModel::copyIndexes( const QModelIndexList& indexes ) {
	QList<BusPortsModel::Port> ports;

	// find all ports that match given indexes
	foreach (QModelIndex index, indexes) {

		if (index.row() >= 0 && index.row() <= table_.size()) {

			// find the port for the index
			BusPortsModel::Port temp = table_.at(index.row());

			// if port is not yet added to the list
			if (!ports.contains(temp))
				ports.append(temp);
		}
	}

	beginResetModel();

	// make copy of each port
	foreach (BusPortsModel::Port port, ports) {
		
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

void BusPortsModel::exportCSV( const QString& path ) {

	if (path.isEmpty())
		return;

	QFile file(path);
	 
	// if file can't be opened then quit
	if (!file.open(QFile::Truncate | QFile::WriteOnly)) {
		emit errorMessage(tr("Could not open file %1 for writing.").arg(path));
		return;
	}

	QTextStream stream(&file);

	// write the headers for the columns
	stream << "Name;Qualifier;Width;Default;Mode;Direction;Presence;Driver;Comment" << endl;

	// write each port
	foreach (BusPortsModel::Port port, table_) {
		stream << port.name_ << ";";
		stream << portQualifier2Str(port.qualifier_) << ";";
		stream << port.width_ << ";";
		stream << port.defaultValue_ << ";";
		stream << portMode2Str(port.mode_) << ";";
		stream << portDirection2Str(port.direction_) << ";";
		stream << General::presence2Str(port.presence_) << ";";
		stream << portDriver2Str(port.driver_) << ";";
		stream << port.comment_ << endl;
	}

	// close the file
	file.close();
}

void BusPortsModel::importCSV( const QString& path ) {

	if (path.isEmpty())
		return;

	// make sure the path exists and it is to a file, not a directory
	QFileInfo info(path);
	if (!info.exists() || !info.isFile())
		return;

	QFile file(path);

	// if file can't be opened for reading
	if (!file.open(QFile::Truncate | QFile::ReadOnly)) {
		emit errorMessage(tr("Could not open file %1 for reading.").arg(path));
		return;
	}

	QTextStream stream(&file);

	// read the headers from the stream and make sure the file is of correct format
	QString header = stream.readLine(100);
	QStringList headers = header.split(";", QString::KeepEmptyParts);
	if (headers.size() != 9) {
		emit errorMessage(tr("File %1 was not appropriate format to import ports.").arg(
			path));
		file.close();
		return;
	}

	beginResetModel();

	// read all lines
	while (!stream.atEnd()) {

		QString line = stream.readLine(0);
		QStringList settings = line.split(";", QString::KeepEmptyParts);
		// if not all settings are found then skip port
		if (settings.size() != 9) {
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
		port.presence_ = General::str2Presence(settings.at(6), General::REQUIRED);

		// set the driver
		port.driver_ = str2PortDriver(settings.at(7));

		// set the comment
		port.comment_ = settings.at(8);

		// before adding port make sure it does not already exist
		if (!table_.contains(port))
			table_.append(port);
	}

	// last sort the table
	qSort(table_);

	endResetModel();
	
	// close the file
	file.close();

	emit contentChanged();
}

BusPortsModel::Port::Port():
name_(),
qualifier_(BusPortsModel::QUALIFIER_ANY),
width_(0),
defaultValue_(),
mode_(BusPortsModel::MODE_ANY),
direction_(BusPortsModel::DIRECTION_INOUT),
presence_(General::REQUIRED),
driver_(BusPortsModel::DRIVER_NONE),
comment_() {
}

BusPortsModel::Port::Port( const QString& name ):
name_(name),
qualifier_(BusPortsModel::QUALIFIER_ANY),
width_(0),
defaultValue_(),
mode_(BusPortsModel::MODE_ANY),
direction_(BusPortsModel::DIRECTION_INOUT),
presence_(General::REQUIRED),
driver_(BusPortsModel::DRIVER_NONE),
comment_() {
}

BusPortsModel::Port::Port( const Port& other ):
name_(other.name_),
qualifier_(other.qualifier_),
width_(other.width_),
defaultValue_(other.defaultValue_),
mode_(other.mode_),
direction_(other.direction_),
presence_(other.presence_),
driver_(other.driver_),
comment_(other.comment_) {
}

BusPortsModel::Port& BusPortsModel::Port::operator=( const Port& other ) {
	if (this != &other) {

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

bool BusPortsModel::Port::operator==( const Port& other ) const {
	
	// if names don't match
	if (name_ != other.name_)
		return false;
	
	// if either port has mode ANY and the names matched
	else if (mode_ == BusPortsModel::MODE_ANY || other.mode_ == BusPortsModel::MODE_ANY)
		return true;

	// if modes don't match and 
	else if (mode_ != other.mode_)
		return false;

	// if names matched and ports matched
	else
		return true;
}

bool BusPortsModel::Port::operator!=( const Port& other ) const {

	// if names don't match
	if (name_ != other.name_)
		return true;

	// if either port has mode ANY and the names matched
	else if (mode_ == BusPortsModel::MODE_ANY || other.mode_ == BusPortsModel::MODE_ANY)
		return false;

	// if modes don't match and 
	else if (mode_ != other.mode_)
		return true;

	// if names matched and ports matched
	else
		return false;
}

bool BusPortsModel::Port::operator<( const Port& other ) const {

	// if names match then order by mode
	if (name_ == other.name_) {
		return mode_ < other.mode_;
	}
	else
		return name_ < other.name_;
}
