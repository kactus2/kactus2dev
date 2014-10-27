//-----------------------------------------------------------------------------
// File: portsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------

#include "portsmodel.h"

#include "portsdelegate.h"
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/port.h>

#include <kactusGenerators/vhdlGenerator/vhdlgeneral.h>

#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: PortsModel::PortsModel()
//-----------------------------------------------------------------------------
PortsModel::PortsModel(QSharedPointer<Model> model, QObject *parent ): QAbstractTableModel(parent), 
    model_(model), lockedIndexes_()
{
	Q_ASSERT(model_);
}

//-----------------------------------------------------------------------------
// Function: PortsModel::~PortsModel()
//-----------------------------------------------------------------------------
PortsModel::~PortsModel()
{

}

//-----------------------------------------------------------------------------
// Function: PortsModel::rowCount()
//-----------------------------------------------------------------------------
int PortsModel::rowCount(const QModelIndex& parent /*= QModelIndex() */) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return model_->portCount();
}   

//-----------------------------------------------------------------------------
// Function: PortsModel::columnCount()
//-----------------------------------------------------------------------------
int PortsModel::columnCount(const QModelIndex& parent /*= QModelIndex() */) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return PORT_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::data()
//-----------------------------------------------------------------------------
QVariant PortsModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

	// if row is invalid
	if (index.row() < 0 || index.row() >= model_->portCount())
    {
        return QVariant();
    }

    QList<QSharedPointer<Port> > ports = model_->getPorts();

    if (role == Qt::DisplayRole) {

        switch (index.column())
        {
        case PORT_COL_ROW: 
            {
                return index.row() + 1;
            }
        case PORT_COL_NAME:
            {
                return ports.at(index.row())->getName();
            }
        case PORT_COL_DIRECTION:
            {
                return General::direction2Str(ports.at(index.row())->getDirection());
            }
        case PORT_COL_WIDTH:
            {
                return ports.at(index.row())->getPortSize();
            }
        case PORT_COL_LEFT:
            {
                return ports.at(index.row())->getLeftBound();
            }
        case PORT_COL_RIGHT:
            {
                return ports.at(index.row())->getRightBound();
            }
        case PORT_COL_TYPENAME:
            {
                return ports.at(index.row())->getTypeName();
            }
        case PORT_COL_TYPEDEF:
            {
                QString typeName = ports.at(index.row())->getTypeName();
                return ports.at(index.row())->getTypeDefinition(typeName);
            }
        case PORT_COL_DEFAULT:
            {
                return ports.at(index.row())->getDefaultValue();
            }
        case PORT_COL_DESC:
            {
                return ports.at(index.row())->getDescription();
            }
        case PORT_COL_ADHOC_VISIBILITY:
            {
                return ports.at(index.row())->isAdHocVisible();
            }
        default:
            {
                return QVariant();
            }
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if ( isLocked(index) )
        {
            return QColor("gray");
        }
        else if (ports.at(index.row())->isValid(model_->hasViews()))
        {
            return QColor("black");
        }
        else
        {
            return QColor("red");
        }
    }
	else if (Qt::BackgroundRole == role)
    {

        switch (index.column()) 
        {
        case PORT_COL_NAME:
        case PORT_COL_DIRECTION:
        case PORT_COL_WIDTH:
        case PORT_COL_LEFT:
        case PORT_COL_RIGHT:
            {
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
            if (ports.at(index.row())->isAdHocVisible())
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
    else if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == PORT_COL_ROW)
        {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
    }
	// if unsupported role	
    return QVariant();	
}

//-----------------------------------------------------------------------------
// Function: PortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case PORT_COL_ROW:
                {
                    return tr("#");
                }
            case PORT_COL_NAME:
                {
                    return tr("Name");
                }
            case PORT_COL_DIRECTION:
                {
                    return tr("Direction");
                }
            case PORT_COL_WIDTH:
                {
                    return tr("Width");
                }
            case PORT_COL_LEFT:
                {
                    return tr("Left\n(higher)\nbound");
                }
            case PORT_COL_RIGHT:
                {
                    return tr("Right\n(lower)\nbound");
                }
            case PORT_COL_TYPENAME:
                {
                    return tr("Type");
                }
            case PORT_COL_TYPEDEF:
                {
                    return tr("Type\ndefinition");
                }
            case PORT_COL_DEFAULT:
                {
                    return tr("Default\nvalue");
                }
            case PORT_COL_DESC:
                {
                    return tr("Description");
                }
            case PORT_COL_ADHOC_VISIBILITY:
                {
                    return tr("Ad-hoc");
                }
            default:
                {
                    return QVariant();
                }
            }
        } 
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::setData()
//-----------------------------------------------------------------------------
bool PortsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */ )
{	
	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= model_->portCount())
		return false;

    QList<QSharedPointer<Port> > ports = model_->getPorts();

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }

        switch (index.column()) {
        case PORT_COL_ROW: {
            return false;
                           }
        case PORT_COL_NAME: {
            ports.at(index.row())->setName(value.toString());

            break;
                            }
        case PORT_COL_DIRECTION: {

            General::Direction direction = General::str2Direction(
                value.toString(), General::DIRECTION_INVALID);

            ports.at(index.row())->setDirection(direction);
            break;
                                 }
        case PORT_COL_WIDTH: {
            int size = value.toInt();
            ports.at(index.row())->setPortSize(size);

            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) {

                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) {

                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(index, 
                QAbstractTableModel::index(index.row(), index.column()+3, QModelIndex()));
            return true;
                             }
        case PORT_COL_LEFT: {

            // make sure left bound doesn't drop below right bound
            if (value.toInt() < ports.at(index.row())->getRightBound())
                return false;

            // ok so make the change
            ports.at(index.row())->setLeftBound(value.toInt());

            int size = ports.at(index.row())->getPortSize();
            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) {

                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) {

                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(
                QAbstractTableModel::index(index.row(), index.column()-1, QModelIndex()),
                QAbstractTableModel::index(index.row(), index.column()+2, QModelIndex()));

            return true;
                            }
        case PORT_COL_RIGHT: {

            // make sure right bound is not greater than left bound
            if (value.toInt() > ports.at(index.row())->getLeftBound())
                return false;

            // ok so apply the change
            ports.at(index.row())->setRightBound(value.toInt());

            int size = ports.at(index.row())->getPortSize();
            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) {

                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) {

                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(
                QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()), 
                QAbstractTableModel::index(index.row(), index.column()-2, QModelIndex()));
            return true;
                             }
        case PORT_COL_TYPENAME: {
            QString typeName = value.toString();
            ports.at(index.row())->setTypeName(typeName);

            // update the type definition for the new type name
            ports.at(index.row())->setTypeDefinition(typeName,
                VhdlGeneral::getDefaultVhdlTypeDef(typeName));

            emit dataChanged(index,
                QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()));
            return true;
                                }
        case PORT_COL_TYPEDEF: {
            QString typeName = ports.at(index.row())->getTypeName();
            ports.at(index.row())->setTypeDefinition(typeName, value.toString());
            break;
                               }
        case PORT_COL_DEFAULT: {
            ports.at(index.row())->setDefaultValue(value.toString());
            break;
                               }
        case PORT_COL_DESC: {
            ports.at(index.row())->setDescription(value.toString());
            break;
                            }
        case PORT_COL_ADHOC_VISIBILITY: {
            ports.at(index.row())->setAdHocVisible(value.toBool());
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
        ports.at(index.row())->setAdHocVisible(value == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }
    // unsupported role
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortsModel::flags( const QModelIndex& index ) const {
	
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

    if (index.column() == PORT_COL_ROW)
    {
        return Qt::NoItemFlags;
    }

    if ( isLocked(index) )
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

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

//-----------------------------------------------------------------------------
// Function: PortsModel::isValid()
//-----------------------------------------------------------------------------
bool PortsModel::isValid() const
{	
	// check all ports in the table
	foreach (QSharedPointer<Port> port, model_->getPorts())
    {	
		if (!port->isValid(model_->hasViews()))
        {
			return false;
        }
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveRow(int row) {
	// if row is invalid
	if (row < 0 || row >=model_->portCount() || rowIsLocked(row))
		return;

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
    QList<QSharedPointer<Port> >& ports = model_->getPorts();
	ports.removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	if (index.row() < 0 || index.row() >= model_->portCount() || rowIsLocked(index.row()))
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    QList<QSharedPointer<Port> >& ports = model_->getPorts();
	ports.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddRow()
//-----------------------------------------------------------------------------
void PortsModel::onAddRow()
{
	beginInsertRows(QModelIndex(), model_->portCount(), model_->portCount());

	QSharedPointer<Port> port(new Port());
	model_->addPort(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddItem()
//-----------------------------------------------------------------------------
void PortsModel::onAddItem( const QModelIndex& index )
{
	int row = model_->portCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    QList<QSharedPointer<Port> >& ports = model_->getPorts();
	ports.insert(row, QSharedPointer<Port>(new Port()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::addPort()
//-----------------------------------------------------------------------------
void PortsModel::addPort( QSharedPointer<Port> port )
{
	beginInsertRows(QModelIndex(), model_->portCount(), model_->portCount());

	model_->addPort(port);
    lockPort(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortsModel::index( QSharedPointer<Port> port ) const
{
	// find the correct row
	int row = model_->getPorts().indexOf(port);

	// if the named port is not found
	if (row < 0)
    {
		return QModelIndex();
	}

	// the base class creates the index for the row
	return QAbstractTableModel::index(row, 0, QModelIndex());
}

//-----------------------------------------------------------------------------
// Function: PortsModel::setModelAndLockCurrentPorts()
//-----------------------------------------------------------------------------
void PortsModel::setModelAndLockCurrentPorts(QSharedPointer<Model> model)
{
    beginResetModel();

    lockedIndexes_.clear();

    model_ = model;
    
    endResetModel();

    foreach(QSharedPointer<Port> port, model_->getPorts())
    {
        lockPort(port);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: lockPort()
//-----------------------------------------------------------------------------
void PortsModel::lockPort(QSharedPointer<Port> port)
{
    QModelIndex nameIndex = index(port);
    QModelIndex directionIndex = nameIndex.sibling(nameIndex.row(), PORT_COL_DIRECTION );
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(), PORT_COL_TYPENAME);

    if (nameIndex.isValid() && typeIndex.isValid() && typeIndex.isValid())
    {     
        lockIndex(nameIndex);  
        lockIndex(directionIndex);  
        lockIndex(typeIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: unlockPort()
//-----------------------------------------------------------------------------
void PortsModel::unlockPort(QSharedPointer<Port> port)
{
    QModelIndex nameIndex = index(port);
    QModelIndex directionIndex = nameIndex.sibling(nameIndex.row(), PORT_COL_DIRECTION);
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(), PORT_COL_TYPENAME);
   
    if (nameIndex.isValid() && typeIndex.isValid() && typeIndex.isValid())
    {     
        unlockIndex(nameIndex);  
        unlockIndex(directionIndex);  
        unlockIndex(typeIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: lockIndex()
//-----------------------------------------------------------------------------
void PortsModel::lockIndex(QModelIndex const& index)
{
    if(!isLocked(index))
    {
        lockedIndexes_.append(QPersistentModelIndex(index));
    }
}

//-----------------------------------------------------------------------------
// Function: unlockIndex()
//-----------------------------------------------------------------------------
void PortsModel::unlockIndex(QModelIndex const& index)
{
    lockedIndexes_.removeAll(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: isLocked()
//-----------------------------------------------------------------------------
bool PortsModel::isLocked(QModelIndex const& index) const
{
    return lockedIndexes_.contains(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: PortsModel::rowIsLocked()
//-----------------------------------------------------------------------------
bool PortsModel::rowIsLocked(int row)
{
    QModelIndex nameIndex = QAbstractTableModel::index(row, 0, QModelIndex());
    return nameIndex.isValid() && isLocked(nameIndex);
}
