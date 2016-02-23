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

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <kactusGenerators/vhdlGenerator/vhdlgeneral.h>

#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PortsModel::PortsModel()
//-----------------------------------------------------------------------------
PortsModel::PortsModel(QSharedPointer<Model> model, QSharedPointer<ExpressionParser> expressionParser,
                       QSharedPointer<ParameterFinder> parameterFinder,
                       QSharedPointer<ExpressionFormatter> expressionFormatter,
                       QSharedPointer<PortValidator> portValidator, QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
model_(model),
lockedIndexes_(),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
portValidator_(portValidator)
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
int PortsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return model_->getPorts()->count();
}   

//-----------------------------------------------------------------------------
// Function: PortsModel::columnCount()
//-----------------------------------------------------------------------------
int PortsModel::columnCount(QModelIndex const& parent) const
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
QVariant PortsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == PortColumns::DESCRIPTION)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        if (index.column() == PortColumns::DESCRIPTION)
        {
            return valueForIndex(index);
        }
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if (isLocked(index))
        {
            return QColor("gray");
        }
        /*else if (portOnRow(index.row())->isValid(model_->hasViews()))
        {
            return blackForValidOrRedForInvalidIndex(index);
        }*/
        else
        {
            return blackForValidOrRedForInvalidIndex(index);
            //return QColor("red");
        }
    }
	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == PortColumns::ROW_NUMBER ||
            (index.column() == PortColumns::WIDTH && hasExpressionInLeftOrRightBound(portOnRow(index.row()))) ||
            (index.column() == PortColumns::TYPE_DEF && !portOnRow(index.row())->hasType()))
        {
            return QColor("whiteSmoke");
        }
        else if (index.column() == PortColumns::NAME || index.column() == PortColumns::DIRECTION || 
            index.column() == PortColumns::WIDTH)
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
QVariant PortsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                QString leftBoundHeader = tr("Left (higher)\nbound") + getExpressionSymbol();
                return leftBoundHeader;
            }
            else if (section == PortColumns::RIGHT_BOUND)
            {
                QString rightBoundHeader = tr("Right (lower)\nbound") + getExpressionSymbol();
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
            else if (section == PortColumns::ARRAY_LEFT)
            {
                QString arrayLeftHeader = tr("Array\nleft") + getExpressionSymbol();
                return arrayLeftHeader;
            }
            else if (section == PortColumns::ARRAY_RIGHT)
            {
                QString arrayRightHeader = tr("Array\nright") + getExpressionSymbol();
                return arrayRightHeader;
            }
            else if (section == PortColumns::TAG_GROUP)
            {
                return tr("Port\ntags");
            }
            else if (section == PortColumns::ADHOC_VISIBILITY)
            {
                return tr("Ad-hoc");
            }
            else if (section == PortColumns::DESCRIPTION)
            {
                return tr("Description");
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
bool PortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{	
	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count())
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
            DirectionTypes::Direction direction = DirectionTypes::str2Direction(value.toString(),
                DirectionTypes::DIRECTION_INVALID);

            port->setDirection(direction);
        }
        else if (index.column() == PortColumns::WIDTH)
        {
            if (hasExpressionInLeftOrRightBound(port))
            {
                return false;
            }

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
                removeReferencesFromSingleExpression(port->getLeftBound());
            }

            port->setLeftBound(value.toString());

            setTypeNameAndDefinitionOnRow(port, index.row());

            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == PortColumns::RIGHT_BOUND)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getRightBound());
            }

            port->setRightBound(value.toString());

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
        else if (index.column() == PortColumns::ARRAY_LEFT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getArrayLeft());
            }

            port->setArrayLeft(value.toString());

            if (value.isValid() && port->getArrayRight().isEmpty() && !port->getArrayLeft().isEmpty())
            {
                port->setArrayRight(QString::number(0));
            }
        }
        else if (index.column() == PortColumns::ARRAY_RIGHT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getArrayRight());
            }

            port->setArrayRight(value.toString());

            if (value.isValid() && port->getArrayLeft().isEmpty() && !port->getArrayRight().isEmpty())
            {
                port->setArrayLeft(QString::number(0));
            }
        }
        else if (index.column() == PortColumns::TAG_GROUP)
        {
            QString tagGroup = value.toString();
            port->setPortTags(tagGroup);
        }
        else if (index.column() == PortColumns::ADHOC_VISIBILITY)
        {
            port->setAdHocVisible(value.toBool());
        }
        else if (index.column() == PortColumns::DESCRIPTION)
        {
            port->setDescription(value.toString());
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
Qt::ItemFlags PortsModel::flags(QModelIndex const& index) const
{
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
	foreach (QSharedPointer<Port> port, *model_->getPorts())
    {
        if (!portValidator_->validate(port))
        {
			return false;
        }
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveRow(int row)
{
	// if row is invalid
	if (row < 0 || row >=model_->getPorts()->count() || rowIsLocked(row))
    {
		return;
    }

	beginRemoveRows(QModelIndex(), row, row);
    model_->getPorts()->removeAt(row);
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PortsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= model_->getPorts()->count() ||
        rowIsLocked(index.row()))
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    QSharedPointer<QList<QSharedPointer<Port> > > ports = model_->getPorts();

    removeReferencesInItemOnRow(index.row());

	ports->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddRow()
//-----------------------------------------------------------------------------
void PortsModel::onAddRow()
{
    int lastRow = model_->getPorts()->count();
	beginInsertRows(QModelIndex(), lastRow, lastRow);

	QSharedPointer<Port> port(new Port());
	model_->getPorts()->append(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::onAddItem()
//-----------------------------------------------------------------------------
void PortsModel::onAddItem(QModelIndex const& index)
{
	int row = model_->getPorts()->count();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	model_->getPorts()->insert(row, QSharedPointer<Port>(new Port()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::addPort()
//-----------------------------------------------------------------------------
void PortsModel::addPort(QSharedPointer<Port> port)
{
    int lastRow = model_->getPorts()->count(); 
	beginInsertRows(QModelIndex(), lastRow, lastRow);

	model_->getPorts()->append(port);
    lockPort(port);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortsModel::index(QSharedPointer<Port> port) const
{
	// find the correct row
	int row = model_->getPorts()->indexOf(port);

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

    foreach(QSharedPointer<Port> port, *model_->getPorts())
    {
        if (port->getDirection() != DirectionTypes::DIRECTION_PHANTOM)
        {
            lockPort(port);
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortsModel::portOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsModel::portOnRow(int row) const
{
    return model_->getPorts()->at(row);
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
    QModelIndex nameIndex = QAbstractTableModel::index(row, PortColumns::NAME, QModelIndex());
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
        return port->name();
    }
    else if (index.column() == PortColumns::DIRECTION)
    {
        return DirectionTypes::direction2Str(port->getDirection());
    }
    else if (index.column() == PortColumns::WIDTH)
    {
        int calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toInt();
        int calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toInt();

        int portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
        return portWidth;
    }
    else if (index.column() == PortColumns::LEFT_BOUND)
    {
        QString leftBound = port->getLeftBound();

        if (leftBound == "n/a")
        {
            port->setLeftBound(0);
        }

        return leftBound;
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
    {
        QString rightBound = port->getRightBound();

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
        return port->getDefaultValue();
    }
    else if (index.column() == PortColumns::ARRAY_LEFT)
    {
        return port->getArrayLeft();
    }
    else if (index.column() == PortColumns::ARRAY_RIGHT)
    {
        return port->getArrayRight();
    }
    else if (index.column() == PortColumns::TAG_GROUP)
    {
        return port->getPortTags();
    }
    else if (index.column() == PortColumns::ADHOC_VISIBILITY)
    {
        return port->isAdHocVisible();
    }
    else if (index.column() == PortColumns::DESCRIPTION)
    {
        return port->description();
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
        index.column() == PortColumns::DEFAULT_VALUE || index.column() == PortColumns::ARRAY_LEFT ||
        index.column() == PortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: portsmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == PortColumns::LEFT_BOUND)
    {
        return port->getLeftBound();
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
    {
        return port->getRightBound();
    }
    else if (index.column() == PortColumns::DEFAULT_VALUE)
    {
        return port->getDefaultValue();
    }
    else if (index.column() == PortColumns::ARRAY_LEFT)
    {
        return port->getArrayLeft();
    }
    else if (index.column() == PortColumns::ARRAY_RIGHT)
    {
        return port->getArrayRight();
    }
    else if (index.column() == PortColumns::DESCRIPTION)
    {
        return port->description();
    }
    else
    {
        return data(index, Qt::DisplayRole);
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::validateIndex()
//-----------------------------------------------------------------------------
bool PortsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());

    if (index.column() == PortColumns::NAME)
    {
        return portValidator_->hasValidName(port->name());
    }
    if (index.column() == PortColumns::LEFT_BOUND)
    {
		if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
		{
			return true;
		}

        return portValidator_->portBoundIsValid(port->getLeftBound());
    }
    else if (index.column() == PortColumns::RIGHT_BOUND)
	{
		if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
		{
			return true;
		}

        return portValidator_->portBoundIsValid(port->getRightBound());
    }
    else if (index.column() == PortColumns::DEFAULT_VALUE)
    {
        return isValuePlainOrExpression(port->getDefaultValue());
    }
    else if (index.column() == PortColumns::ARRAY_LEFT)
    {
        return portValidator_->arrayValueIsValid(port->getArrayLeft());
    }
    else if (index.column() == PortColumns::ARRAY_RIGHT)
    {
        return portValidator_->arrayValueIsValid(port->getArrayRight());
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: portsmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PortsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Port> port = model_->getPorts()->at(row);

    int referencesInLeftBound = port->getLeftBound().count(valueID);
    int referencesInRightbount = port->getRightBound().count(valueID);
    int referencesInDefaultValue = port->getDefaultValue().count(valueID);
    int referencesInArrayLeft = port->getArrayLeft().count(valueID);
    int referencesInArrayRight = port->getArrayRight().count(valueID);

    int  totalReferences = referencesInLeftBound + referencesInRightbount + referencesInDefaultValue +
        referencesInArrayLeft + referencesInArrayRight;
    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: PortsModel::updatePortType()
//-----------------------------------------------------------------------------
void PortsModel::setTypeNameAndDefinitionOnRow(QSharedPointer<Port> port, int row)
{
    int calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toInt();
    int calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toInt();

    int portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
    // if port is vectored and previous type was std_logic
    if (portWidth > 1 && port->getTypeName() == QString("std_logic")) 
    {
        // change the type to vectored
        port->setTypeName("std_logic_vector");
        port->setTypeDefinition("std_logic_vector", VhdlGeneral::getDefaultVhdlTypeDef("std_logic_vector"));

        emit dataChanged(QAbstractTableModel::index(row, PortColumns::TYPE_NAME), 
            QAbstractTableModel::index(row, PortColumns::TYPE_DEF));
    }
    // if port is not vectored but previous type was std_logic_vector
    else if (portWidth < 2 && port->getTypeName() == QString("std_logic_vector")) 
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
	if ( port->getLeftBound().isEmpty() && port->getRightBound().isEmpty() )
	{
		return true;
	}

    bool leftNumber = false;
    bool rightNumber = false;

    port->getLeftBound().toInt(&leftNumber);
    port->getRightBound().toInt(&rightNumber);

    return !leftNumber || !rightNumber;
}
