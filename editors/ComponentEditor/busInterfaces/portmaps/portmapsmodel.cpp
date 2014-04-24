/* 
 *
 *  Created on: 12.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsmodel.cpp
 */

#include "portmapsmodel.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QtGlobal>
#include <QStringList>
#include <QSize>
#include <QColor>

PortMapsModel::PortMapsModel(BusInterface* busif,
							 QSharedPointer<Component> component,
							 LibraryInterface* handler,
							 QObject *parent): 
QAbstractTableModel(parent),
busif_(busif),
portMaps_(busif->getPortMaps()),
component_(component),
handler_(handler),
absDef_(),
interfaceMode_(General::MASTER) {

	Q_ASSERT_X(component, "PortMapsModel constructor",
		"Null component pointer given as parameter");
	Q_ASSERT_X(handler, "PortMapsModel constructor",
		"Null LibraryInterface pointer given as parameter");
	Q_ASSERT(busif);
}

PortMapsModel::~PortMapsModel() {
}

int PortMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	
	if (parent.isValid())
		return 0;

	return portMaps_.size();
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
	else if (index.row() < 0 || index.row() >= portMaps_.size())
		return QVariant();

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			
			case 0: 
				if (portMaps_.at(index.row())->logicalVector_) {
					return portMaps_.at(index.row())->logicalVector_->getLeft();
				}
				return QString();

			case 1:
				if (portMaps_.at(index.row())->logicalVector_) {
					return portMaps_.at(index.row())->logicalVector_->getRight();
				}
				return QString();

			case 2: 
				return portMaps_.at(index.row())->logicalPort_;

			case 3: 

				return portMaps_.at(index.row())->physicalPort_;

			case 4:
				if (portMaps_.at(index.row())->physicalVector_) {
					return portMaps_.at(index.row())->physicalVector_->getLeft();
				}
				return QString();

			case 5:
				if (portMaps_.at(index.row())->physicalVector_) {
					return portMaps_.at(index.row())->physicalVector_->getRight();
				}
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

		// the physical port bounds are needed to 
		QList<General::PortBounds> bounds = component_->getPortBounds();

		// if the physical port's bounds don't match the actual port size
		if (!portMaps_.at(index.row())->isValid(bounds)) {
			return QColor("red");
		}
		// if abstraction def is set and logical port is illegal 
		else if (absDef_ && absDef_->isIllegal(portMaps_.at(index.row())->logicalPort_, interfaceMode_)) {
			return QColor("red");
		}

		// if abstraction def is set but port is not defined as optional or
		// required
		else if (absDef_ && 
			!absDef_->isRequired(portMaps_.at(index.row())->logicalPort_, interfaceMode_) &&
			!absDef_->isOptional(portMaps_.at(index.row())->logicalPort_, interfaceMode_)) {
			return QColor("red");
		}
		// everything was fine
		else {
			return QColor("black");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 2:
			case 3: {
				return QColor("LemonChiffon");
					}
			default: {
				return QColor("white");
					 }
		}
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
	
	if (!index.isValid()) {
		return false;
	}

	if (index.row() < 0 || index.row() >= portMaps_.size() ) {
		return false;
	}
	
	if (Qt::EditRole == role) {

		switch (index.column()) {
			case 0: {
				
				// if there is no logical vector then add it
				if (!portMaps_[index.row()]->logicalVector_) {
					portMaps_[index.row()]->logicalVector_ = QSharedPointer<Vector>(new Vector());
				}

				// if the value is defined
				if (!value.toString().isEmpty()) {
					portMaps_[index.row()]->logicalVector_->setLeft(value.toInt());
				}
				// if value was empty
				else {
					portMaps_[index.row()]->logicalVector_->setLeft(-1);
				}
				break;
					}
			case 1: {

				// if there is no logical vector then add it
				if (!portMaps_[index.row()]->logicalVector_) {
					portMaps_[index.row()]->logicalVector_ = QSharedPointer<Vector>(new Vector());
				}

				// if the value is defined
				if (!value.toString().isEmpty()) {
					portMaps_[index.row()]->logicalVector_->setRight(value.toInt());
				}
				// if value was empty
				else {
					portMaps_[index.row()]->logicalVector_->setRight(-1);
				}
				break;
					}
			case 2: {
				portMaps_[index.row()]->logicalPort_ = value.toString();
				break;
					}
			case 3: {
				portMaps_[index.row()]->physicalPort_ = value.toString();
				break;
					}
			case 4: {

				// if there is no physical vector then add it
				if (!portMaps_[index.row()]->physicalVector_) {
					portMaps_[index.row()]->physicalVector_ = QSharedPointer<Vector>(new Vector());
				}

				// if the value is defined
				if (!value.toString().isEmpty())
					portMaps_[index.row()]->physicalVector_->setLeft(value.toInt());
				// if value was empty
				else
					portMaps_[index.row()]->physicalVector_->setLeft(-1);
				break;
					}
			case 5: {

				// if there is no physical vector then add it
				if (!portMaps_[index.row()]->physicalVector_) {
					portMaps_[index.row()]->physicalVector_ = QSharedPointer<Vector>(new Vector());
				}

				// if the value is defined
				if (!value.toString().isEmpty())
					portMaps_[index.row()]->physicalVector_->setRight(value.toInt());
				// if value was empty
				else
					portMaps_[index.row()]->physicalVector_->setRight(-1);
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

	// physical bounds are needed to check that port bounds match the physical port bounds
	QList<General::PortBounds> bounds = component_->getPortBounds();

	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		if (!portMap->isValid(bounds)) {
			return false;
		}
		// if abstraction def is set and logical port is illegal 
		else if (absDef_ && absDef_->isIllegal(portMap->logicalPort_, interfaceMode_)) {
			return false;
		}

		// if abstraction def is set but port is not defined as optional or
		// required
		else if (absDef_ && 
			!absDef_->isRequired(portMap->logicalPort_, interfaceMode_) &&
			!absDef_->isOptional(portMap->logicalPort_, interfaceMode_)) {
				return false;
		}
	}

	// all ports were valid
	return true;
}

void PortMapsModel::onRemoveItems( const QModelIndex& index ) {

	if (!index.isValid())
		return;
	else if (index.row() < 0 || index.row() >= portMaps_.size())
		return;

	QString physical = portMaps_.at(index.row())->physicalPort_;
	bool physicalFound = false;
	QString logical = portMaps_.at(index.row())->logicalPort_;
	bool logicalFound = false;

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	
	// remove port map
	portMaps_.removeAt(index.row());
	endRemoveRows();

	// check if either port is still found in another port map
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		if (portMap->logicalPort_ == logical) {
			logicalFound = true;
		}
		if (portMap->physicalPort_ == physical) {
			physicalFound = true;
		}
	}

	// inform other models that ports that don't have mappings can be removed
	if (!logicalFound) {
		emit logicalRemoved(logical);
	}
	if (!physicalFound) {
		emit physicalRemoved(physical);
	}
	
	emit contentChanged();
}

void PortMapsModel::createMap( const QString& physicalPort, const QString& logicalPort ) {
	
	// both port names must be non-empty
	if (physicalPort.isEmpty() || logicalPort.isEmpty())
		return;	

	beginInsertRows(QModelIndex(), portMaps_.size(), portMaps_.size());

	QSharedPointer<General::PortMap> portMap(new General::PortMap());
	portMap->logicalPort_ = logicalPort;
	portMap->physicalPort_ = physicalPort;

	// if abs def is used
	if (absDef_) {
		int size = absDef_->getPortSize(logicalPort, interfaceMode_);

        // If size is not specified in logical, use physical port size.
        if (size < 0 && component_->hasPort(physicalPort))
        {
            size = component_->getPortWidth(physicalPort);
        }

		// if size was specified.
		if (size >= 0) {
			portMap->logicalVector_->setLeft(size - 1);
			portMap->logicalVector_->setRight(0);
        }
	}
	// if the port is found on the component
	if (component_->hasPort(physicalPort)) {
		int size = component_->getPortWidth(physicalPort);

		if (size >= 0) {
			portMap->physicalVector_->setLeft(size - 1);
			portMap->physicalVector_->setRight(0);
		}
	}

	portMaps_.append(portMap);
	endInsertRows();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortMapsModel::createMap()
//-----------------------------------------------------------------------------
void PortMapsModel::createMap(QSharedPointer<General::PortMap> portMap)
{

    beginInsertRows(QModelIndex(), portMaps_.size(), portMaps_.size());
    portMaps_.append(portMap);
    endInsertRows();
    emit contentChanged();
}


QStringList PortMapsModel::logicalPorts() const {

	QStringList list;
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		list.append(portMap->logicalPort_);
	}
	return list;
}

QStringList PortMapsModel::physicalPorts() const {

	QStringList list;
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		list.append(portMap->physicalPort_);
	}
	return list;
}

void PortMapsModel::onRestoreItem( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	if (index.column() == 2) {
		emit logicalRemoved(portMaps_.at(index.row())->logicalPort_);
	}
	else if (index.column() == 3) {
		emit physicalRemoved(portMaps_.at(index.row())->physicalPort_);
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
		if (logicalDirection != physDirection && physDirection != General::INOUT) {
			emit errorMessage(tr("Directions between logical port \"%1\" and"
				" physical port \"%2\" did not match.").arg(logicalPort).arg(physicalPort));
			return false;
		}
		else
			return true;
	}
	// if abstraction definition is not used ports can always be mapped
	else
		return true;
}
