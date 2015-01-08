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
PortsModel::PortsModel(QSharedPointer<Model> model, QSharedPointer<ExpressionParser> expressionParser, 
    QObject *parent):
    QAbstractTableModel(parent),
    ParameterModelEquations(),
    model_(model), lockedIndexes_()
{
	Q_ASSERT(model_);
    setExpressionParser(expressionParser);
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

    if (role == Qt::DisplayRole) 
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole || role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }

    else if (Qt::ForegroundRole == role)
    {
        if ( isLocked(index) )
        {
            return QColor("gray");
        }

        else if (ports.at(index.row())->isValid(model_->hasViews()))
        {
            return blackForValidOrRedForInvalidIndex(index);
        }
        else
        {
            return QColor("red");
        }
    }
	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == PORT_COL_ROW)
        {
            return QColor("whiteSmoke");
        }
        else if (index.column() == PORT_COL_NAME || index.column() == PORT_COL_DIRECTION || 
            index.column() == PORT_COL_WIDTH || index.column() == PORT_COL_LEFT || 
            index.column() == PORT_COL_RIGHT)
        {
            return QColor("LemonChiffon");
        }
        else
        {
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
            if (section == PORT_COL_ROW)
            {
                return tr("#");
            }
            else if (section == PORT_COL_NAME)
            {
                return tr("Name");
            }
            else if (section == PORT_COL_DIRECTION)
            {
                return tr("Direction");
            }
            else if (section == PORT_COL_WIDTH)
            {
                return tr("Width");
            }
            else if (section == PORT_COL_LEFT)
            {
                QString leftBoundHeader = tr("Left\n(higher)\nbound") + getExpressionSymbol();
                return leftBoundHeader;
            }
            else if (section == PORT_COL_RIGHT)
            {
                QString rightBoundHeader = tr("Right\n(lower)\nbound") + getExpressionSymbol();
                return rightBoundHeader;
            }
            else if (section == PORT_COL_TYPENAME)
            {
                return tr("Type");
            }
            else if (section == PORT_COL_TYPEDEF)
            {
                return tr("Type\ndefinition");
            }
            else if (section == PORT_COL_DEFAULT)
            {
                QString defaultHeader = tr("Default\nvalue") + getExpressionSymbol();
                return defaultHeader;
            }
            else if (section == PORT_COL_DESC)
            {
                return tr("Description");
            }
            else if (section == PORT_COL_ADHOC_VISIBILITY)
            {
                return tr("Ad-hoc");
            }
            else
            {
                return QVariant();
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

        if (index.column() == PORT_COL_ROW)
        {
            return false;
        }
        else if (index.column() == PORT_COL_NAME)
        {
            ports.at(index.row())->setName(value.toString());
        }
        else if (index.column() == PORT_COL_DIRECTION)
        {
            General::Direction direction = General::str2Direction(value.toString(), General::DIRECTION_INVALID);

            ports.at(index.row())->setDirection(direction);
        }
        else if (index.column() == PORT_COL_WIDTH)
        {
            ports.at(index.row())->removeLeftBoundExpression();
            ports.at(index.row())->removeRightBoundExpression();

            int size = value.toInt();
            ports.at(index.row())->setPortSize(size);

            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) 
            {
                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) 
            {
                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(index, QAbstractTableModel::index(index.row(), index.column()+3, QModelIndex()));
            return true;
        }
        else if (index.column() == PORT_COL_LEFT)
        {
            // make sure left bound doesn't drop below right bound
            QString calculatedExpression = parseExpressionToDecimal(value.toString(), index);

            if(calculatedExpression.toInt() < ports.at(index.row())->getRightBound() ||
                calculatedExpression == "n/a")
            {
                return false;
            }

            // ok so make the change
            ports.at(index.row())->removeLeftBoundExpression();
            
            if (calculatedExpression != value.toString())
            {
                ports.at(index.row())->setLeftBoundExpression(value.toString());
            }

            ports.at(index.row())->setLeftBound(calculatedExpression.toInt());

            int size = ports.at(index.row())->getPortSize();
            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) 
            {
                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) 
            {
                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(
                QAbstractTableModel::index(index.row(), index.column()-1, QModelIndex()),
                QAbstractTableModel::index(index.row(), index.column()+2, QModelIndex()));

            return true;
        }
        else if (index.column() == PORT_COL_RIGHT)
        {
            // make sure right bound is not greater than left bound
            QString calculatedExpression = parseExpressionToDecimal(value.toString(), index);

            if (calculatedExpression.toInt() > ports.at(index.row())->getLeftBound() || 
                calculatedExpression == "n/a")
            {
                return false;
            }

            // ok so apply the change
            ports.at(index.row())->removeRightBoundExpression();
            
            if (calculatedExpression != value.toString())
            {
                ports.at(index.row())->setRightBoundExpression(value.toString());
            }

            ports.at(index.row())->setRightBound(calculatedExpression.toInt());

            int size = ports.at(index.row())->getPortSize();
            // if port is vectored and previous type was std_logic
            if (size > 1 && ports.at(index.row())->getTypeName() == QString("std_logic")) 
            {
                // change the type to vectored
                ports.at(index.row())->setTypeName("std_logic_vector");
            }

            // if port is not vectored but previous type was std_logic_vector
            else if (size < 2 && ports.at(index.row())->getTypeName() == QString("std_logic_vector")) 
            {
                ports.at(index.row())->setTypeName("std_logic");
            }

            emit dataChanged(
                QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()), 
                QAbstractTableModel::index(index.row(), index.column()-2, QModelIndex()));
            return true;
        }
        else if (index.column() == PORT_COL_TYPENAME)
        {
            QString typeName = value.toString();
            ports.at(index.row())->setTypeName(typeName);

            // update the type definition for the new type name
            ports.at(index.row())->setTypeDefinition(typeName,
                VhdlGeneral::getDefaultVhdlTypeDef(typeName));

            emit dataChanged(index,
                QAbstractTableModel::index(index.row(), index.column()+1, QModelIndex()));
            return true;
        }
        else if (index.column() == PORT_COL_TYPEDEF)
        {
            QString typeName = ports.at(index.row())->getTypeName();
            ports.at(index.row())->setTypeDefinition(typeName, value.toString());
        }
        else if (index.column() == PORT_COL_DEFAULT)
        {
            ports.at(index.row())->setDefaultValue(value.toString());
        }
        else if (index.column() == PORT_COL_DESC)
        {
            ports.at(index.row())->setDescription(value.toString());
        }
        else if (index.column() == PORT_COL_ADHOC_VISIBILITY)
        {
            ports.at(index.row())->setAdHocVisible(value.toBool());
        }
        else
        {
            return false;
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
    QModelIndex portIndex = index(port);
    QModelIndex nameIndex = index(port).sibling(portIndex.row(), PORT_COL_NAME);
    QModelIndex directionIndex = nameIndex.sibling(portIndex.row(), PORT_COL_DIRECTION);
    QModelIndex typeIndex = nameIndex.sibling(portIndex.row(), PORT_COL_TYPENAME);

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

//-----------------------------------------------------------------------------
// Function: portsmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::valueForIndex(QModelIndex const& index) const
{
    QList<QSharedPointer<Port> > ports = model_->getPorts();

    if (index.column() == PORT_COL_ROW)
    {
        return index.row() + 1;
    }
    else if (index.column() == PORT_COL_NAME)
    {
        return ports.at(index.row())->getName();
    }
    else if (index.column() == PORT_COL_DIRECTION)
    {
        return General::direction2Str(ports.at(index.row())->getDirection());
    }
    else if (index.column() == PORT_COL_WIDTH)
    {
        return ports.at(index.row())->getPortSize();
    }
    // Expression OK
    else if (index.column() == PORT_COL_LEFT)
    {
        //return ports.at(index.row())->getLeftBound();
        QString leftBound = QString::number(ports.at(index.row())->getLeftBound());
        return formattedValueFor(leftBound);
    }
    // Expression OK
    else if (index.column() == PORT_COL_RIGHT)
    {
        //return ports.at(index.row())->getRightBound();
        QString rightBound = QString::number(ports.at(index.row())->getRightBound());
        return formattedValueFor(rightBound);
    }
    else if (index.column() == PORT_COL_TYPENAME)
    {
        return ports.at(index.row())->getTypeName();
    }
    else if (index.column() == PORT_COL_TYPEDEF)
    {
        QString typeName = ports.at(index.row())->getTypeName();
        return ports.at(index.row())->getTypeDefinition(typeName);
    }
    // Expression OK
    else if (index.column() == PORT_COL_DEFAULT)
    {
        //return ports.at(index.row())->getDefaultValue();
        return formattedValueFor(ports.at(index.row())->getDefaultValue());
    }
    else if (index.column() == PORT_COL_DESC)
    {
        return ports.at(index.row())->getDescription();
    }
    else if (index.column() == PORT_COL_ADHOC_VISIBILITY)
    {
        return ports.at(index.row())->isAdHocVisible();
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PortsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == PORT_COL_LEFT || index.column() == PORT_COL_RIGHT || index.column() == PORT_COL_DEFAULT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QList<QSharedPointer<Port> > ports = model_->getPorts();

    if (index.column() == PORT_COL_LEFT)
    {
        return ports.at(index.row())->getLeftBoundExpression();
    }
    else if (index.column() == PORT_COL_RIGHT)
    {
        return ports.at(index.row())->getRightBoundExpression();
    }
    else if (index.column() == PORT_COL_DEFAULT)
    {
        return ports.at(index.row())->getDefaultValue();
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool PortsModel::validateColumnForParameter(QModelIndex const& index) const
{
    QList < QSharedPointer < Port > > ports = model_->getPorts();

    if (index.column() == PORT_COL_LEFT)
    {
        QString leftBound = QString::number(ports.at(index.row())->getLeftBound());
        return isValuePlainOrExpression(leftBound);
    }
    else if (index.column() == PORT_COL_RIGHT)
    {
        QString rightBound = QString::number(ports.at(index.row())->getRightBound());
        return isValuePlainOrExpression(rightBound);
    }
    else if (index.column() == PORT_COL_DEFAULT)
    {
        return isValuePlainOrExpression(ports.at(index.row())->getDefaultValue());
    }

    return true;
}