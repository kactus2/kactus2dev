/* 
 *
 *  Created on: 12.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsmodel.cpp
 */

#include "portmapsmodel.h"

#include <models/generaldeclarations.h>
#include <LibraryManager/libraryinterface.h>

#include <QtGlobal>
#include <QStringList>
#include <QSize>
#include <QColor>

#include <QDebug>

// the struct constructor
PortMapsModel::Mapping::Mapping( const QString& logicalPort,
								const QString& physicalPort ):
physPort_(physicalPort), physLeft_(-1), physRight_(-1),
logicalPort_(logicalPort), logicalLeft_(-1), logicalRight_(-1) {
}

PortMapsModel::PortMapsModel(QSortFilterProxyModel* proxy, 
							 QSharedPointer<Component> component,
							 LibraryInterface* handler,
							 QObject *parent): 
QAbstractTableModel(parent),
portMaps_(NULL), 
table_(),
proxy_(proxy),
component_(component),
handler_(handler),
absDef_(),
interfaceMode_(General::MASTER) {

	Q_ASSERT_X(proxy, "PortMapsModel constructor",
		"Null proxy pointer given as parameter");
	Q_ASSERT_X(component, "PortMapsModel constructor",
		"Null component pointer given as parameter");
	Q_ASSERT_X(handler, "PortMapsModel constructor",
		"Null LibraryInterface pointer given as parameter");
}

PortMapsModel::~PortMapsModel() {
}

void PortMapsModel::setPortMaps( QList<QSharedPointer<General::PortMap> >* portMaps ) {

	// if pointer is null then nothing is done
	if (!portMaps)
		return;

	portMaps_ = portMaps;

	restore();
}

int PortMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	
	if (parent.isValid())
		return 0;

	return table_.size();
}

int PortMapsModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	if (parent.isValid())
		return 0;

	return 6;
}

QVariant PortMapsModel::data(const QModelIndex& index, 
							 int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			
			case 0: 
				if (table_.at(index.row()).logicalLeft_ >= 0)
					return table_.at(index.row()).logicalLeft_;
				return QString();

			case 1:
				if (table_.at(index.row()).logicalRight_ >= 0)
					return table_.at(index.row()).logicalRight_;
				return QString();

			case 2: 
				return table_.at(index.row()).logicalPort_;

			case 3: 

				return table_.at(index.row()).physPort_;

			case 4:
				if (table_.at(index.row()).physLeft_ >= 0)
					return table_.at(index.row()).physLeft_;
				return QString();

			case 5:
				if (table_.at(index.row()).physRight_ >= 0)
					return table_.at(index.row()).physRight_;
				return QString();

			default:
				return QVariant();
		}
	}
	// the hint for sizes
	else if (Qt::SizeHintRole == role) {

		// if item is for port name
		if (index.column() == 2 || index.column() == 3) 
			return QSize(200, 10);
		else
			return QSize(40, 10);
	}
	else if (Qt::ForegroundRole == role) {

		// if invalid item
		if (!isValid(table_.at(index.row())))
			return QColor("red");
		else
			return QColor("black");
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant PortMapsModel::headerData(int section, 
								   Qt::Orientation orientation, 
								   int role /*= Qt::DisplayRole */ ) const {

	if (role == Qt::DisplayRole) {

		// the horizontal headers
		if ( orientation == Qt::Horizontal) {
			switch (section) {
				case 0: 
					return tr("Logical\nleft");
				case 1:
					return tr("Logical\nright");
				case 2:
					return tr("Logical\nname");
				case 3:
					return tr("Physical\nname");
				case 4:
					return tr("Physical\nleft");
				case 5:
					return tr("Physical\nright");
				default:
					return QVariant();
			}
		}

		// if orientation is not horizontal
		return QVariant();
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

bool PortMapsModel::setData( const QModelIndex& index, 
							const QVariant& value, 
							int role /*= Qt::EditRole */ ) {
	
	if (!index.isValid())
		return false;
	

	if (index.row() < 0 || index.row() >= table_.size() )
		return false;
	
	if (Qt::EditRole == role) {

		switch (index.column()) {
			case 0: {
				// if the value is defined
				if (!value.toString().isEmpty())
					table_[index.row()].logicalLeft_ = value.toInt();
				// if value was empty
				else
					table_[index.row()].logicalLeft_ = -1;
				break;
					}
			case 1: {
				// if the value is defined
				if (!value.toString().isEmpty())
					table_[index.row()].logicalRight_ = value.toInt();
				// if value was empty
				else
					table_[index.row()].logicalRight_ = -1;
				break;
					}
			case 2: {
				table_[index.row()].logicalPort_ = value.toString();
				break;
					}
			case 3: {
				table_[index.row()].physPort_ = value.toString();
				break;
					}
			case 4: {
				// if the value is defined
				if (!value.toString().isEmpty())
					table_[index.row()].physLeft_ = value.toInt();
				// if value was empty
				else
					table_[index.row()].physLeft_ = -1;
				break;
					}
			case 5: {
				// if the value is defined
				if (!value.toString().isEmpty())
					table_[index.row()].physRight_ = value.toInt();
				// if value was empty
				else
					table_[index.row()].physRight_ = -1;
				break;
					}
		}

		emit dataChanged(index, index);
		return true;
	}
	// if unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags PortMapsModel::flags( const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;

	// port names can't be edited
	else if (index.column() == 2 || index.column() == 3)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool PortMapsModel::isValid() const {

	foreach (PortMapsModel::Mapping map, table_) {
		if (!isValid(map))
			return false;
	}

	// all ports were valid
	return true;
}

bool PortMapsModel::isValid( const Mapping& map ) const {
	// if either port name is empty
	if (map.logicalPort_.isEmpty() || map.physPort_.isEmpty())
		return false;

	// if abstraction def is set and logical port is illegal 
	else if (absDef_ && absDef_->isIllegal(map.logicalPort_, interfaceMode_))
		return false;

	// if abstraction def is set but port is not defined as optional or
	// required
	else if (absDef_ && 
		!absDef_->isRequired(map.logicalPort_, interfaceMode_) &&
		!absDef_->isOptional(map.logicalPort_, interfaceMode_))
		return false;

	// if port is not found in component
	else if (!component_->hasPort(map.physPort_))
		return false;

	// if one bound for logical port is defined but not the other
	else if (map.logicalLeft_ < 0 && map.logicalRight_ >= 0)
		return false;
	else if (map.logicalLeft_ >= 0 && map.logicalRight_ < 0)
		return false;

	// if one bound for physical port is defined but not the other
	else if (map.physLeft_ < 0 && map.physRight_ >= 0)
		return false;
	else if (map.physLeft_ >= 0 && map.physRight_ < 0)
		return false;

	// if port sizes don't match
	else if ((map.logicalLeft_ - map.logicalRight_) != (map.physLeft_- map.physRight_))
		return false;

	// everything was ok
	else
		return true;
}

void PortMapsModel::apply() {

	if (!portMaps_)
		return;

	// remove the previous portmaps
	portMaps_->clear();

	foreach (PortMapsModel::Mapping mapping, table_) {

		// create the port map
		QSharedPointer<General::PortMap> map = QSharedPointer<General::PortMap>(new General::PortMap);
		map->logicalPort_ = mapping.logicalPort_;
		map->physicalPort_ = mapping.physPort_;

		// if logical port should be vectored
		if (mapping.logicalLeft_ >= 0 && mapping.logicalRight_ >= 0) {
			map->logicalVector_ = QSharedPointer<Vector>(new Vector(
				mapping.logicalLeft_, mapping.logicalRight_));
		}

		// if physical port should be vectored
		if (mapping.physLeft_ >= 0 && mapping.physRight_ >= 0) {
			map->physicalVector_ = QSharedPointer<Vector>(new Vector(
				mapping.physLeft_, mapping.physRight_));
		}

		// save the map to port maps
		portMaps_->append(map);
	}

}

void PortMapsModel::restore() {
	
	if (!portMaps_)
		return;

	beginResetModel();

	// remove the previous items
	table_.clear();

	for (int i = 0; i < portMaps_->size(); ++i) {
		
		// create the mapping
		PortMapsModel::Mapping mapping(portMaps_->at(i)->logicalPort_,
			portMaps_->at(i)->physicalPort_);

		// if logical port is vectored
		if (portMaps_->at(i)->logicalVector_) {
			mapping.logicalLeft_ = portMaps_->at(i)->logicalVector_->getLeft();
			mapping.logicalRight_ = portMaps_->at(i)->logicalVector_->getRight();
		}

		// if physical port is vectored
		if (portMaps_->at(i)->physicalVector_) {
			mapping.physLeft_ = portMaps_->at(i)->physicalVector_->getLeft();
			mapping.physRight_ = portMaps_->at(i)->physicalVector_->getRight();
		}

		// append the mapping to the table
		table_.append(mapping);

	}

	endResetModel();
	emit contentChanged();
}

void PortMapsModel::onRemoveItems( const QModelIndex& index ) {

	if (!index.isValid())
		return;
	else if (index.row() < 0 || index.row() >= table_.size())
		return;

	QString physical = table_.at(index.row()).physPort_;
	bool physicalFound = false;
	QString logical = table_.at(index.row()).logicalPort_;
	bool logicalFound = false;

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	
	// remove port map
	table_.removeAt(index.row());
	endRemoveRows();

	// check if either port is still found in another port map
	foreach (PortMapsModel::Mapping map, table_) {
		if (map.logicalPort_ == logical)
			logicalFound = true;
		if (map.physPort_ == physical)
			physicalFound = true;
	}

	// inform other models that ports that dont have mappings can be removed
	if (!logicalFound)
		emit logicalRemoved(logical);
	if (!physicalFound)
		emit physicalRemoved(physical);
	
	emit contentChanged();
}

void PortMapsModel::createMap( const QString& physicalPort, const QString& logicalPort ) {
	
	// both port names must be non-empty
	if (physicalPort.isEmpty() || logicalPort.isEmpty())
		return;	

	beginInsertRows(QModelIndex(), table_.size(), table_.size());
	PortMapsModel::Mapping map(logicalPort, physicalPort);

	// if abs def is used
	if (absDef_) {
		int size = absDef_->getPortSize(logicalPort, interfaceMode_);

		// if size was specified
		if (size >= 0) {
			map.logicalLeft_ = size - 1;
			map.logicalRight_ = 0;
		}
	}
	// if the port is found on the component
	if (component_->hasPort(physicalPort)) {
		int size = component_->getPortWidth(physicalPort);

		if (size >= 0) {
			map.physLeft_ = size - 1;
			map.physRight_ = 0;
		}
	}

	table_.append(map);
	endInsertRows();
	emit contentChanged();
}

QStringList PortMapsModel::logicalPorts() const {

	QStringList list;
	foreach (PortMapsModel::Mapping map, table_) {
		list.append(map.logicalPort_);
	}
	return list;
}

QStringList PortMapsModel::physicalPorts() const {

	QStringList list;
	foreach (PortMapsModel::Mapping map, table_) {
		list.append(map.physPort_);
	}
	return list;
}

void PortMapsModel::onRestoreItem( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	if (index.column() == 2) {
		emit logicalRemoved(table_.at(index.row()).logicalPort_);
	}
	else if (index.column() == 3) {
		emit physicalRemoved(table_.at(index.row()).physPort_);
	}
}

void PortMapsModel::setAbsType( const VLNV& vlnv, General::InterfaceMode mode ) {

	interfaceMode_ = mode;

	if (!vlnv.isValid()) {
		absDef_ = QSharedPointer<AbstractionDefinition>();
		return;
	}

	// ask library to parse the model for abstraction definition
	QSharedPointer<LibraryComponent> libComb;
	if (handler_->contains(vlnv)) 
		libComb = handler_->getModel(vlnv);

	// if library did not contain the vlnv
	else
		return;

	// make sure the model is for abstraction definition
	if (handler_->getDocumentType(vlnv) == VLNV::ABSTRACTIONDEFINITION)
		absDef_ = libComb.staticCast<AbstractionDefinition>();
}

bool PortMapsModel::canCreateMap( const QString& physicalPort, const QString& logicalPort ) const {
	// if port maps has been defined then check that signal directions match
	if (absDef_) {
		General::Direction logicalDirection = 
			absDef_->getPortDirection(logicalPort, interfaceMode_);

		General::Direction physDirection = component_->getPortDirection(physicalPort);

		// if directions don't match
		if (logicalDirection != physDirection) {
			emit errorMessage(tr("Directions between logical port \"%1\" and"
				" physical port \"%2\" didn't match.").arg(logicalPort).arg(physicalPort));
			return false;
		}
		else
			return true;
	}
	// if abstraction definition is not used ports can always be mapped
	else
		return true;
}
