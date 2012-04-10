/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portsmodel.cpp
 */

#include "portsmodel.h"

#include "portsdelegate.h"
#include <models/generaldeclarations.h>

#include <vhdlGenerator/vhdlgeneral.h>

#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QColor>

PortsModel::PortsModel(QSharedPointer<Component> component,
					   void* dataPointer,
					   QObject *parent ):
QAbstractTableModel(parent),
ports_(), 
table_(),
component_(component) {

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

	return PORT_COL_COUNT;
}

QVariant PortsModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid()) {
		return QVariant();
	}

	// if row is invalid
	if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case PORT_COL_NAME: {
				return table_.at(index.row())->getName();
					}
			case PORT_COL_DIRECTION: {
				return General::direction2Str(table_.at(index.row())->getDirection());
					}
			case PORT_COL_WIDTH: {
				return table_.at(index.row())->getPortSize();
					}
			case PORT_COL_LEFT: {
				return table_.at(index.row())->getLeftBound();
					}
			case PORT_COL_RIGHT: {
				return table_.at(index.row())->getRightBound();
					}
			case PORT_COL_TYPENAME: {
				return table_.at(index.row())->getTypeName();
					}
			case PORT_COL_TYPEDEF: {
				QString typeName = table_.at(index.row())->getTypeName();
				return table_.at(index.row())->getTypeDefinition(typeName);
					}
			case PORT_COL_DEFAULT: {
				return table_.at(index.row())->getDefaultValue();
					}
			case PORT_COL_DESC: {
				return table_.at(index.row())->getDescription();
					}
			case PORT_COL_ADHOC_VISIBILITY: {
				return table_.at(index.row())->isAdHocVisible();
															  }

			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (table_.at(index.row())->isValid(component_->hasViews())) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case PORT_COL_NAME:
			case PORT_COL_DIRECTION:
			case PORT_COL_WIDTH:
			case PORT_COL_LEFT:
			case PORT_COL_RIGHT: {
				return QColor("LemonChiffon");
					}
			default:
				return QColor("white");
		}
	}
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == PORT_COL_ADHOC_VISIBILITY)
        {
            if (table_.at(index.row())->isAdHocVisible())
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        else
        {
            return QVariant();
        }
    }
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant PortsModel::headerData( int section, Qt::Orientation orientation, 
								int role /*= Qt::DisplayRole */ ) const {

	if (role == Qt::DisplayRole) {

		if (orientation == Qt::Horizontal) {
			switch (section) {
				case PORT_COL_NAME: {
					return tr("Name");
						}
				case PORT_COL_DIRECTION: {
					return tr("Direction");
						}
				case PORT_COL_WIDTH: {
					return tr("Width");
						}
				case PORT_COL_LEFT: {
					return tr("Left\n(higher)\nbound");
						}
				case PORT_COL_RIGHT: {
					return tr("Right\n(lower)\nbound");
						}
				case PORT_COL_TYPENAME: {
					return tr("Type");
						}
				case PORT_COL_TYPEDEF: {
					return tr("Type\ndefinition");
						}
				case PORT_COL_DEFAULT: {
					return tr("Default\nvalue");
						}
				case PORT_COL_DESC: {
					return tr("Description");
						}

                case PORT_COL_ADHOC_VISIBILITY:
                    {
                        return tr("Ad-hoc");
                        break;
                    }

				default: {
					return QVariant();
						 }
			}
		} 

		// if vertical headers
		else {
			return section + 1;
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
			case PORT_COL_NAME: {
				table_.at(index.row())->setName(value.toString());
				break;
					}
			case PORT_COL_DIRECTION: {

				General::Direction direction = General::str2Direction(
					value.toString(), General::DIRECTION_INVALID);
				
				table_.at(index.row())->setDirection(direction);
				break;
					}
			case PORT_COL_WIDTH: {
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
			case PORT_COL_LEFT: {

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
			case PORT_COL_RIGHT: {

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
			case PORT_COL_TYPENAME: {
				QString typeName = value.toString();
				table_.at(index.row())->setTypeName(typeName);

				// update the type definition for the new type name
				table_.at(index.row())->setTypeDefinition(typeName,
					VhdlGeneral::getDefaultVhdlTypeDef(typeName));

				emit dataChanged(index,
					QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()));
				return true;
					}
			case PORT_COL_TYPEDEF: {
				QString typeName = table_.at(index.row())->getTypeName();
				table_.at(index.row())->setTypeDefinition(typeName, value.toString());
				break;
					}
			case PORT_COL_DEFAULT: {
				table_.at(index.row())->setDefaultValue(value.toString());
				break;
					}
			case PORT_COL_DESC: {
				table_.at(index.row())->setDescription(value.toString());
				break;
					}
			case PORT_COL_ADHOC_VISIBILITY: {
				table_.at(index.row())->setAdHocVisible(value.toBool());
				break;
											}

			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		return true;
	}
    else if (role == Qt::CheckStateRole)
    {
        table_.at(index.row())->setAdHocVisible(value == Qt::Checked);
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

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == PORT_COL_ADHOC_VISIBILITY)
    {
        flags |= Qt::ItemIsUserCheckable;
    }
    else
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool PortsModel::isValid() const {
	
	// check all ports in the table
	foreach (QSharedPointer<Port> port, table_) {

		// if one is in invalid state
		if (!port->isValid(component_->hasViews()))
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

		QSharedPointer<Port> tablePort(new Port(*port.data()));

		// the default port types are not used anymore
		// if theres no specified type for the port
// 		if (!tablePort->hasType()) {
// 			tablePort->useDefaultVhdlTypes();
// 		}
		table_.append(tablePort);
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

void PortsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void PortsModel::onAddRow() {
	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	QSharedPointer<Port> port(new Port());
	table_.append(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}


void PortsModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QSharedPointer<Port>(new Port()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

