/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portsmodel.cpp
 */

#include "portsmodel.h"

#include <models/generaldeclarations.h>

#include <QTextStream>
#include <QString>
#include <QStringList>

PortsModel::PortsModel( void* dataPointer, QObject *parent ):
QAbstractTableModel(parent),
ports_(), table_() {

	Q_ASSERT_X(dataPointer, "PortsModel constructor",
		"Null dataPointer given as parameter");

	// set the pointer to the actual data structure containing the 
	// model parameters
	ports_ = static_cast<QMap<QString, QSharedPointer<Port> > *>(dataPointer);

	restore();
}

PortsModel::~PortsModel() {
}

int PortsModel::rowCount(const QModelIndex& parent /*= QModelIndex() */) const {

	if (parent.isValid())
		return 0;

	return table_.size();
}

int PortsModel::columnCount(const QModelIndex& parent /*= QModelIndex() */) const {

	if (parent.isValid())
		return 0;

	return 9;
}

QVariant PortsModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: {
				return table_.at(index.row())->getName();
					}
			case 1: {
				return General::direction2Str(table_.at(index.row())->getDirection());
					}
			case 2: {
				return table_.at(index.row())->getPortSize();
					}
			case 3: {
				return table_.at(index.row())->getLeftBound();
					}
			case 4: {
				return table_.at(index.row())->getRightBound();
					}
			case 5: {
				return table_.at(index.row())->getTypeName();
					}
			case 6: {
				QString typeName = table_.at(index.row())->getTypeName();
				return table_.at(index.row())->getTypeDefinition(typeName);
					}
			case 7: {
				return table_.at(index.row())->getDefaultValue();
					}
			case 8: {
				return table_.at(index.row())->getDescription();
					}
			default: {
				return QVariant();
					 }
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant PortsModel::headerData( int section, Qt::Orientation orientation, 
								int role /*= Qt::DisplayRole */ ) const {

	if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (section) {
			case 0: {
				return tr("Name");
					}
			case 1: {
				return tr("Direction");
					}
			case 2: {
				return tr("Width");
					}
			case 3: {
				return tr("Left\nbound");
					}
			case 4: {
				return tr("Right\nbound");
					}
			case 5: {
				return tr("Type");
					}
			case 6: {
				return tr("Type\ndefinition");
					}
			case 7: {
				return tr("Default\nvalue");
					}
			case 8: {
				return tr("Description");
					}
			default: {
				return QVariant();
					 }
		}
	}

	// if unsupported role
	else {
		return QVariant();
	}
}

bool PortsModel::setData( const QModelIndex& index, 
						 const QVariant& value, 
						 int role /*= Qt::EditRole */ ) {
	
	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {
		
		switch (index.column()) {
			case 0: {
				table_.at(index.row())->setName(value.toString());
				break;
					}
			case 1: {

				General::Direction direction = General::str2Direction(
					value.toString(), General::DIRECTION_INVALID);
				
				table_.at(index.row())->setDirection(direction);
				break;
					}
			case 2: {
				int size = value.toInt();
				table_.at(index.row())->setPortSize(size);
				
				// if port is vectored and previous type was std_logic
				if (size > 1 && table_.at(index.row())->getTypeName() == QString("std_logic")) {
					
					// change the type to vectored
					table_.at(index.row())->setTypeName("std_logic_vector");
				}

				// if port is not vectored but previous type was std_logic_vector
				else if (size < 2 && table_.at(index.row())->getTypeName() == QString("std_logic_vector")) {

					table_.at(index.row())->setTypeName("std_logic");
				}

				emit dataChanged(index, 
					QAbstractTableModel::index(index.row(), index.column()+3, QModelIndex()));
				return true;
					}
			case 3: {

				// make sure left bound doesn't drop below right bound
				if (value.toInt() < table_.at(index.row())->getRightBound())
					return false;

				// ok so make the change
				table_.at(index.row())->setLeftBound(value.toInt());

				int size = table_.at(index.row())->getPortSize();
				// if port is vectored and previous type was std_logic
				if (size > 1 && table_.at(index.row())->getTypeName() == QString("std_logic")) {

					// change the type to vectored
					table_.at(index.row())->setTypeName("std_logic_vector");
				}

				// if port is not vectored but previous type was std_logic_vector
				else if (size < 2 && table_.at(index.row())->getTypeName() == QString("std_logic_vector")) {

					table_.at(index.row())->setTypeName("std_logic");
				}

				emit dataChanged(
					QAbstractTableModel::index(index.row(), index.column()-1, QModelIndex()),
					QAbstractTableModel::index(index.row(), index.column()+2, QModelIndex()));

				return true;
					}
			case 4: {

				// make sure right bound is not greater than left bound
				if (value.toInt() > table_.at(index.row())->getLeftBound())
					return false;

				// ok so apply the change
				table_.at(index.row())->setRightBound(value.toInt());

				int size = table_.at(index.row())->getPortSize();
				// if port is vectored and previous type was std_logic
				if (size > 1 && table_.at(index.row())->getTypeName() == QString("std_logic")) {

					// change the type to vectored
					table_.at(index.row())->setTypeName("std_logic_vector");
				}

				// if port is not vectored but previous type was std_logic_vector
				else if (size < 2 && table_.at(index.row())->getTypeName() == QString("std_logic_vector")) {

					table_.at(index.row())->setTypeName("std_logic");
				}

				emit dataChanged(
					QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()), 
					QAbstractTableModel::index(index.row(), index.column()-2, QModelIndex()));
				return true;
					}
			case 5: {
				table_.at(index.row())->setTypeName(value.toString());
				break;
					}
			case 6: {
				QString typeName = table_.at(index.row())->getTypeName();
				table_.at(index.row())->setTypeDefinition(typeName, value.toString());
				break;
					}
			case 7: {
				table_.at(index.row())->setDefaultValue(value.toString());
				break;
					}
			case 8: {
				table_.at(index.row())->setDescription(value.toString());
				break;
					}
			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		return true;
	}

	// unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags PortsModel::flags( const QModelIndex& index ) const {
	
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool PortsModel::isValid() const {
	
	// check all ports in the table
	foreach (QSharedPointer<Port> port, table_) {

		// if one is in invalid state
		if (!port->isValid())
			return false;
	}

	return true;
}

void PortsModel::apply() {

	if (!isValid())
		return;

	// remove old ports
	ports_->clear();

	// add each port from the table
	for (int i = 0; i < table_.size(); ++i) {
		ports_->insert(table_.value(i)->getName(), table_.value(i));
	}
}

void PortsModel::restore() {

	beginResetModel();

	foreach (QSharedPointer<Port> port, *ports_) {
		table_.append(QSharedPointer<Port>(new Port(*port.data())));
	}

	endResetModel();
}

void PortsModel::onRemoveRow( int row ) {
	// if row is invalid
	if (row < 0 || row >= table_.size())
		return;

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
	table_.removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void PortsModel::onAddRow() {
	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	QSharedPointer<Port> port(new Port());
	port->setTypeName("std_logic");
	port->setTypeDefinition("std_logic", "IEEE.std_logic_1164.all");
	table_.append(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}


void PortsModel::exportPorts( QFile& file ) {
	
	// create a stream to write into
	QTextStream stream(&file);

	// write the headers to the file
	stream << tr("Name") << ";";
	stream << tr("Direction") << ";";
	stream << tr("Width") << ";";
	stream << tr("Left bound") << ";";
	stream << tr("Right bound") << ";";
	stream << tr("Type") << ";";
	stream << tr("Type definition") << ";";
	stream << tr("Default value") << ";";
	stream << tr("Description");
	stream << endl;

	// write each port
	foreach (QSharedPointer<Port> port, table_) {

		stream << port->getName() << ";";
		stream << General::direction2Str(port->getDirection()) << ";";
		stream << port->getPortSize() << ";";
		stream << port->getLeftBound() << ";";
		stream << port->getRightBound() << ";";
		stream << port->getTypeName() << ";";
		stream << port->getTypeDefinition(port->getTypeName()) << ";";
		stream << port->getDefaultValue() << ";";
		stream << port->getDescription();
		stream << endl;
	}
}

void PortsModel::importPorts( QFile& file ) {

	// create a stream to read from
	QTextStream stream (&file);

	// read the headers from the file
	QString headers = stream.readLine();
	QStringList headerList = headers.split(";");

	// if the file is not supported type (not same amount of lines)
	if (headerList.count() != 7)
		return;

	beginResetModel();

	table_.clear();
	
	// read as long as theres stuff left to read
	while (!stream.atEnd()) {

		QString line = stream.readLine();
		QStringList elements = line.split(";");
		
		QString name = elements.value(0);
		General::Direction direction = General::str2Direction(
			elements.value(1), General::DIRECTION_INVALID);
		int width = elements.value(2).toInt();
		int left = elements.value(3).toInt();
		int right = elements.value(4).toInt();
		QString typeName = elements.value(5);
		QString typeDefinition = elements.value(6);
		QString defaultValue = elements.value(7);
		QString description = elements.value(8);

		if (width != (left - right + 1))
			emit errorMessage(tr("Port %1 width does not match port bounds").arg(
			name));

		table_.append(QSharedPointer<Port>(new Port(name, direction, left, right,
			typeName, typeDefinition, defaultValue, description)));
	}
	
	endResetModel();
}
