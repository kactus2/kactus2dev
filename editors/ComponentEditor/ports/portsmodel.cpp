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
#include "PortColumns.h"

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
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
ParameterizableTable(parameterFinder, parent),
model_(model),
lockedIndexes_(),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter)
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

	return PortColumns::COLUMN_COUNT;
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

    if (role == Qt::DisplayRole) 
    {
        return valueForIndex(index);
    }
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return expressionFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
    }
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }
    else if (Qt::ForegroundRole == role)
    {
        if (isLocked(index))
        {
            return QColor("gray");
        }
        else if (portOnRow(index.row())->isValid(model_->hasViews()))
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
        if (index.column() == PortColumns::ROW_NUMBER ||
            index.column() == PortColumns::WIDTH && hasExpressionInLeftOrRightBound(portOnRow(index.row())) ||
            index.column() == PortColumns::TYPE_DEF && !portOnRow(index.row())->hasType())
        {
            return QColor("whiteSmoke");
        }
        else if (index.column() == PortColumns::NAME || index.column() == PortColumns::DIRECTION || 
            index.column() == PortColumns::WIDTH || index.column() == PortColumns::LEFT_BOUND || 
            index.column() == PortColumns::RIGHT_BOUND)
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
        if (index.column() == PortColumns::ADHOC_VISIBILITY)
        {
            if (portOnRow(index.row())->isAdHocVisible())
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
    else if (role == Qt::TextAlignmentRole && index.column() == PortColumns::ROW_NUMBER)
    {
        return Qt::AlignRight + Qt::AlignVCenter;
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
            if (section == PortColumns::ROW_NUMBER)
            {
                return tr("#");
            }
            else if (section == PortColumns::NAME)
            {
                return tr("Name");
            }
            else if (section == PortColumns::DIRECTION)
            {
                return tr("Direction");
            }
            else if (section == PortColumns::WIDTH)
            {
                return tr("Width");
            }
            else if (section == PortColumns::LEFT_BOUND)
            {
                QString leftBoundHeader = tr("Left\n(higher)\nbound") + getExpressionSymbol();
                return leftBoundHeader;
            }
            else if (section == PortColumns::RIGHT_BOUND)
            {
                QString rightBoundHeader = tr("Right\n(lower)\nbound") + getExpressionSymbol();
                return rightBoundHeader;
            }
            else if (section == PortColumns::TYPE_NAME)
            {
                return tr("Type");
            }
            else if (section == PortColumns::TYPE_DEF)
            {
                return tr("Type\ndefinition");
            }
            else if (section == PortColumns::DEFAULT_VALUE)
            {
                QString defaultHeader = tr("Default\nvalue") + getExpressionSymbol();
                return defaultHeader;
            }
            else if (section == PortColumns::PORT_COL_DESC)
            {
                return tr("Description");
            }
            else if (section == PortColumns::ADHOC_VISIBILITY)
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
bool PortsModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole */ )
{	
	if (!index.isValid() || index.row() < 0 || index.row() >= model_->portCount())
    {
		return false;
    }

    QSharedPointer<Port> port = portOnRow(index.row());

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }

        if (index.column() == PortColumns::ROW_NUMBER)
        {
            return false;
        }
        else if (index.column() == PortColumns::NAME)
        {
            port->setName(value.toString());
        }
        else if (index.column() == PortColumns::DIRECTION)
        {
            General::Direction direction = General::str2Direction(value.toString(), General::DIRECTION_INVALID);

            port->setDirection(direction);
        }
        else if (index.column() == PortColumns::WIDTH)
        {
            if (hasExpressionInLeftOrRightBound(port))
            {
                return false;
            }

            port->removeLeftBoundExpression();
            port->removeRightBoundExpression();

            int size = value.toInt();
            port->setPortSize(size);

            QModelIndex widhtIndex = QAbstractTableModel::index(index.row(), PortColumns::WIDTH);
            emit dataChanged(widhtIndex, widhtIndex);

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == PortColumns::LEFT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getLeftBoundExpression());
            }

            // make sure left bound doesn't drop below right bound.
            QString calculatedExpression = parseExpressionToDecimal(value.toString());

            if(calculatedExpression.toInt() < port->getRightBound() || calculatedExpression == "n/a")
            {
                return false;
            }

            // ok so make the change.
            port->removeLeftBoundExpression();
            
            if (calculatedExpression != value.toString())
            {
                port->setLeftBoundExpression(value.toString());
            }

            port->setLeftBound(calculatedExpression.toInt());

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == PortColumns::RIGHT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getRightBoundExpression());
            }

            // make sure right bound is not greater than left bound.
            QString calculatedExpression = parseExpressionToDecimal(value.toString());

            if (calculatedExpression.toInt() > port->getLeftBound() ||  calculatedExpression == "n/a")
            {
                return false;
            }

            // ok so apply the change.
            port->removeRightBoundExpression();
            
            if (calculatedExpression != value.toString())
            {
                port->setRightBoundExpression(value.toString());
            }

            port->setRightBound(calculatedExpression.toInt());

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == PortColumns::TYPE_NAME)
        {
            QString typeName = value.toString();
            port->setTypeName(typeName);

            // update the type definition for the new type name.
            port->setTypeDefinition(typeName, VhdlGeneral::getDefaultVhdlTypeDef(typeName));

            emit dataChanged(index, QAbstractTableModel::index(index.row(), PortColumns::TYPE_DEF));
            return true;
        }
        else if (index.column() == PortColumns::TYPE_DEF)
        {
            port->setTypeDefinition(port->getTypeName(), value.toString());
        }
        else if (index.column() == PortColumns::DEFAULT_VALUE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getDefaultValue());
            }

            port->setDefaultValue(value.toString());
        }
        else if (index.column() == PortColumns::PORT_COL_DESC)
        {
            port->setDescription(value.toString());
        }
        else if (index.column() == PortColumns::ADHOC_VISIBILITY)
        {
            port->setAdHocVisible(value.toBool());
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
        port->setAdHocVisible(value == Qt::Checked);
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
	
	if (!index.isValid() || index.column() == PortColumns::ROW_NUMBER)
    {
		return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (isLocked(index) ||
        (index.column() == PortColumns::WIDTH && hasExpressionInLeftOrRightBound(portOnRow(index.row()))))
    {
        return flags;
    }
    else if (index.column() == PortColumns::ADHOC_VISIBILITY)
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

    removeReferencesInItemOnRow(index.row());

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
// Function: PortsModel::portOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsModel::portOnRow(int row) const
{
    return model_->getPorts().at(row);
}

//-----------------------------------------------------------------------------
// Function: lockPort()
//-----------------------------------------------------------------------------
void PortsModel::lockPort(QSharedPointer<Port> port)
{
    QModelIndex portIndex = index(port);
    QModelIndex nameIndex = index(port).sibling(portIndex.row(), PortColumns::NAME);
    QModelIndex directionIndex = nameIndex.sibling(portIndex.row(), PortColumns::DIRECTION);
    QModelIndex typeIndex = nameIndex.sibling(portIndex.row(), PortColumns::TYPE_NAME);

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
    QModelIndex directionIndex = nameIndex.sibling(nameIndex.row(), PortColumns::DIRECTION);
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(), PortColumns::TYPE_NAME);
   
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
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == PortColumns::ROW_NUMBER)
    {
        return index.row() + 1;
    }
    else if (index.column() == PortColumns::NAME)
    {
        return port->getName();
    }
    else if (index.column() == PortColumns::DIRECTION)
    {
        return General::direction2Str(port->getDirection());
    }
    else if (index.column() == PortColumns::WIDTH)
    {
        return port->getPortSize();
    }
    else if (index.column() == PortColumns::LEFT_BOUND)
    {
        QString leftBound = formattedValueFor(port->getLeftBoundExpression());

        if (leftBound == "n/a")
        {
            port->setLeftBound(0);
        }

        return leftBound;
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
    {
        QString rightBound = formattedValueFor(port->getRightBoundExpression());

        if (rightBound == "n/a")
        {
            port->setRightBound(0);
        }

        return rightBound;
    }
    else if (index.column() == PortColumns::TYPE_NAME)
    {
        return port->getTypeName();
    }
    else if (index.column() == PortColumns::TYPE_DEF)
    {
        QString typeName = port->getTypeName();
        return port->getTypeDefinition(typeName);
    }
    else if (index.column() == PortColumns::DEFAULT_VALUE)
    {
        return formattedValueFor(port->getDefaultValue());
    }
    else if (index.column() == PortColumns::PORT_COL_DESC)
    {
        return port->getDescription();
    }
    else if (index.column() == PortColumns::ADHOC_VISIBILITY)
    {
        return port->isAdHocVisible();
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
    return index.column() == PortColumns::LEFT_BOUND || index.column() == PortColumns::RIGHT_BOUND || 
        index.column() == PortColumns::DEFAULT_VALUE;
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == PortColumns::LEFT_BOUND)
    {
        return port->getLeftBoundExpression();
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
    {
        return port->getRightBoundExpression();
    }
    else if (index.column() == PortColumns::DEFAULT_VALUE)
    {
        return port->getDefaultValue();
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
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == PortColumns::LEFT_BOUND)
    {
        QString leftBound = QString::number(port->getLeftBound());
        return isValuePlainOrExpression(leftBound);
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
    {
        QString rightBound = QString::number(port->getRightBound());
        return isValuePlainOrExpression(rightBound);
    }
    else if (index.column() == PortColumns::DEFAULT_VALUE)
    {
        return isValuePlainOrExpression(port->getDefaultValue());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PortsModel::getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const
{
    QSharedPointer<Port> port = model_->getPorts().at(row);

    int referencesInLeftBound = port->getLeftBoundExpression().count(valueID);
    int referencesInRightbount = port->getRightBoundExpression().count(valueID);
    int referencesInDefaultValue = port->getDefaultValue().count(valueID);

    int  totalReferences = referencesInLeftBound + referencesInRightbount + referencesInDefaultValue;
    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::updatePortType()
//-----------------------------------------------------------------------------
void PortsModel::setTypeNameAndDefinitionOnRow(QSharedPointer<Port> port, int row)
{
    int size = port->getPortSize();
    // if port is vectored and previous type was std_logic
    if (size > 1 && port->getTypeName() == QString("std_logic")) 
    {
        // change the type to vectored
        port->setTypeName("std_logic_vector");
        port->setTypeDefinition("std_logic_vector", VhdlGeneral::getDefaultVhdlTypeDef("std_logic_vector"));

        emit dataChanged(QAbstractTableModel::index(row, PortColumns::TYPE_NAME), 
            QAbstractTableModel::index(row, PortColumns::TYPE_DEF));
    }
    // if port is not vectored but previous type was std_logic_vector
    else if (size < 2 && port->getTypeName() == QString("std_logic_vector")) 
    {
        port->setTypeName("std_logic");
        port->setTypeDefinition("std_logic", VhdlGeneral::getDefaultVhdlTypeDef("std_logic"));

        emit dataChanged(QAbstractTableModel::index(row, PortColumns::TYPE_NAME), 
            QAbstractTableModel::index(row, PortColumns::TYPE_DEF));
    }
}

//-----------------------------------------------------------------------------
// Function: PortsModel::hasExpressionInLeftOrRightBound()
//-----------------------------------------------------------------------------
bool PortsModel::hasExpressionInLeftOrRightBound(QSharedPointer<Port> port) const
{
    bool leftNumber = false;
    bool rightNumber = false;

    port->getLeftBoundExpression().toInt(&leftNumber);
    port->getRightBoundExpression().toInt(&rightNumber);

    return !leftNumber || !rightNumber;
}
