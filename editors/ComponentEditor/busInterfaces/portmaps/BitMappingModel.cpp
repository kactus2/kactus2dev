//-----------------------------------------------------------------------------
// File: BitMappingModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.09.2013
//
// Description:
// Table model for bit field mapping.
//-----------------------------------------------------------------------------

#include "BitMappingModel.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/busInterfaces/portmaps/BitSelectionDialog.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QStringList>
#include <QColor>
#include <QIcon>

namespace
{
    QString const EDIT_ROW_NAME("Drop physical port to add rows.");
}

Q_DECLARE_METATYPE(General::PortBounds)

//-----------------------------------------------------------------------------
// Function: BitMappingModel::BitMappingModel()
//-----------------------------------------------------------------------------
BitMappingModel::BitMappingModel(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component,
LibraryInterface* libHandler, QSharedPointer<ExpressionParser> expressionParser, QObject* parent):
QAbstractTableModel(parent),
    rows_(),
    mappings_(),
    busif_(busif),
    mode_(General::INTERFACE_MODE_COUNT),
    logicalPort_(),
    component_(component),
    handler_(libHandler),
    canEdit_(false),
    expressionParser_(expressionParser)
{
    Q_ASSERT(component);
    Q_ASSERT(libHandler);
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::~BitMappingModel()
//-----------------------------------------------------------------------------
BitMappingModel::~BitMappingModel()
{

}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::rowCount()
//-----------------------------------------------------------------------------
int BitMappingModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::columnCount()
//-----------------------------------------------------------------------------
int BitMappingModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::data()
//-----------------------------------------------------------------------------
QVariant BitMappingModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() > rows_.size() || index.column() >= COLUMN_COUNT)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole) 
    {                  
        if (index.column() == INDEX)
        {
            return index.row();
        }
        else if (index.column() == BIT)
        {                
            if (rows_.at(index.row()).isEmpty())
            {
                return logicalPort_;
            }
            else
            {
                QStringList portsOnRow;
                foreach (General::PortBounds const& row, rows_.at(index.row()))
                {
                    int portSize = 1;
                    QSharedPointer<Port> port = component_->getPort(row.portName_);
                    if (port)
                    {
                        int left = expressionParser_->parseExpression(port->getLeftBound()).toInt();
                        int right = expressionParser_->parseExpression(port->getRightBound()).toInt();
                        portSize = abs(left - right) + 1;
                    }

                    if (portSize > 1)
                    {
                        portsOnRow.append(row.portName_ + "(" + QString::number(row.left_) + ")");
                    }
                    else
                    {
                        portsOnRow.append(row.portName_);
                    } 
                }
                return portsOnRow.join(", ");
            }
        }
        else 
        {
            return QVariant();
        }
    }

    else if (role == Qt::ForegroundRole)
    {      
        if ((index.column() == INDEX && !canEdit_) ||
            (canEdit_ && index.row()  == rows_.size() - 1) ||
            (index.column() == BIT && rows_.at(index.row()).isEmpty()))
        {
            return QColor("gray");
        }  

        return QColor("black");
    }

    else if (role == Qt::BackgroundRole)
    {
        return QColor("white");
    }

    else if (role == Qt::TextAlignmentRole)
    {
        return (Qt::AlignLeft + Qt::AlignVCenter);     
    }

    else if (role == Qt::EditRole && index.column() == BIT)
    {
        return data(index, Qt::DisplayRole);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::headerData()
//-----------------------------------------------------------------------------
QVariant BitMappingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
    {
        if (section == INDEX)
        {
            return QStringLiteral("Logical\nIndex");
        }   
        else if (section == BIT)
        {
            return QStringLiteral("Bit Field(s)");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::setData()
//-----------------------------------------------------------------------------
bool BitMappingModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || rows_.size() < index.row() || index.column() != BIT)
    {
        return false;
    }

    if (role == Qt::EditRole && index.column() == BIT)
    {
        if (value.canConvert<General::PortBounds>())
        {
            int lastRow = addMapping(value.value<General::PortBounds>(), index.row());
            emit dataChanged(index, createIndex(lastRow, BIT));                    
            return true;
        }
        // Clear cell any but last.
        else if (value.toString().isEmpty() && !(canEdit_ && index.row() == rows_.size() - 1))
        {
            rows_[index.row()].clear();       
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BitMappingModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == BIT)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }

    return Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::isValid()
//-----------------------------------------------------------------------------
bool BitMappingModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions BitMappingModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList BitMappingModel::mimeTypes() const
{
    QStringList types = QAbstractTableModel::mimeTypes();
    types << "text/plain";
    return types;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::dropMimeData()
//-----------------------------------------------------------------------------
bool BitMappingModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column,
    QModelIndex const& parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if (row != -1 || column != -1 || !parent.isValid() || !data->hasText())
    {
        return false;
    }

    QStringList portNames = data->text().split(";", QString::SkipEmptyParts);
    // Check direction compatibility.
    if (portNames.isEmpty() || !checkDirectionForPorts(portNames))
    {
        return false;
    }

    if (canEdit_)
    {
        removeEditRow();
    }

    int targetRow = parent.row(); 
    foreach (QString const& portName, portNames)
    {
        if (targetRow >= rowCount() && !canEdit_)
        {
            break;
        }

        QSharedPointer<Port> port = component_->getPort(portName);
        if (port)
        {
            int left = expressionParser_->parseExpression(port->getLeftBound()).toInt();
            int right = expressionParser_->parseExpression(port->getRightBound()).toInt();
            int portSize = abs(left - right) + 1;
            int rowsLeft = rowCount() - targetRow;

            if (portSize > rowsLeft)
            {
                if (canEdit_)
                {
                    addRows(portSize - rowsLeft);
                }
                else
                {
                    BitSelectionDialog dialog(logicalPort_, targetRow, portName, portSize, rowsLeft);
                    if (dialog.exec() != QDialog::Accepted)
                    {
                        return false;
                    }
                    left = dialog.getHigherBound();
                    right = dialog.getLowerBound();
                }
            }

            addMapping(General::PortBounds(portName, left, right), targetRow);            
            targetRow += portSize;
        }
    }

    if (canEdit_)
    {
        addEditRow();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::mapLast()
//-----------------------------------------------------------------------------
void BitMappingModel::mapToEnd(QStringList const& portNames)
{
    int endIndex = rows_.size();

    // Find the first unmapped index and insert ports.
    for (int logIndex = 0; logIndex < endIndex; logIndex++)
    {
        if (rows_[logIndex].size() == 0 || (canEdit_ && logIndex == endIndex - 1))
        {
            QMimeData mimeData;
            mimeData.setText(portNames.join(';'));
            dropMimeData(&mimeData, Qt::CopyAction, -1, -1, createIndex(logIndex, 0));
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::onSetLogicalSignal()
//-----------------------------------------------------------------------------
void BitMappingModel::onSetLogicalSignal(QString const& logicalName)
{
    saveCurrentMappings();

    beginResetModel();
    logicalPort_ = logicalName;
    rows_.clear();

    QSharedPointer<WireAbstraction> logicalWire;
    if (absDef_)
    {
        QSharedPointer<PortAbstraction> logicalAbstraction = absDef_->getPort(logicalName);
        if (logicalAbstraction)
        {
            logicalWire = logicalAbstraction->getWire();
        }        
    }

    if (!logicalName.isEmpty())
    {
        // Indexes can be added/removed, if absDef does not define size.
        canEdit_ = !logicalWire || logicalWire->getWidth(mode_).isEmpty();

        if (mappings_.contains(logicalName))
        {
            restoreMappings();
        } 
        else
        {
            createInitialMappings();
            addEditRow();
        }
    }
    endResetModel();

    emit logicalPortSet(logicalPort_);
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::onRemoveMapping()
//-----------------------------------------------------------------------------
void BitMappingModel::onRemoveMapping(QString const& logicalName)
{
    bool setPrevious = logicalPort_ == logicalName;
    if (setPrevious)
    {
        onSetLogicalSignal("");
    }

    if (mappings_.contains(logicalName))
    {
        mappings_.remove(logicalName);
    }

    if (setPrevious)
    {
        onSetLogicalSignal(logicalName);
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::setAbsType()
//-----------------------------------------------------------------------------
void BitMappingModel::setAbsType(VLNV const& absDefVlnv, General::InterfaceMode mode)
{
    if (!absDefVlnv.isValid())
    {
        return;
    }

    // ask library to parse the model for abstraction definition
    QSharedPointer<Document> document  = handler_->getModel(absDefVlnv);
    if (!document)
    { 
        return;
    }

    // make sure the model is for abstraction definition
    if (handler_->getDocumentType(absDefVlnv) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef_ = document.staticCast<AbstractionDefinition>();
    }

    mode_ = mode;

    beginResetModel();
    rows_.clear();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::getLogicalSignal()
//-----------------------------------------------------------------------------
QString BitMappingModel::getLogicalSignal() const
{
    return logicalPort_;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::getPortMaps()
//-----------------------------------------------------------------------------
QList< QSharedPointer<PortMap> > BitMappingModel::getPortMaps() const
{                    
    QList<QSharedPointer<PortMap> > mappings;

    // Omit editing row, if exists.
    int endIndex = rows_.size();
    if (canEdit_)
    {
        endIndex--;
    }

    for (int logIndex = 0; logIndex < endIndex; logIndex++)
    {
        foreach(General::PortBounds const& pin, rows_[logIndex])
        {           
            if (!isMapped(mappings, pin, logIndex)) 
            {               
                QSharedPointer<PortMap> map = createPortMapForPin(pin, logIndex);
                mappings.append(map);     
            }                               
        }
    }

    return mappings;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::addRows()
//-----------------------------------------------------------------------------
void BitMappingModel::addRows(int count)
{
    int lastRow = rows_.size() + count - 1;

    beginInsertRows(QModelIndex(), rows_.size(), lastRow);
    for (int row = rows_.size(); row <= lastRow; row++)
    {
        rows_.append(QList<General::PortBounds>());
    }
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::removeRows()
//-----------------------------------------------------------------------------
void BitMappingModel::removeRows(int count)
{
    int lastRemovedRow = rows_.size() - count;

    beginRemoveRows(QModelIndex(), lastRemovedRow, rows_.size() - 1);
    for (int row = rows_.size() - 1; row >= lastRemovedRow; row--)
    {
        rows_.removeLast();
    }
    endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::addEditRow()
//-----------------------------------------------------------------------------
void BitMappingModel::addEditRow()
{
    if (canEdit_)
    {
        QList<General::PortBounds> pins;
        General::PortBounds toAdd(EDIT_ROW_NAME);              
        pins.append(toAdd);
        addRows(1);
        rows_.last().append(pins);
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::removeEditRow()
//-----------------------------------------------------------------------------
void BitMappingModel::removeEditRow()
{
    if (canEdit_)
    {
        const QList<General::PortBounds> lastConnection = rows_.value(rows_.size() - 1);
        if (lastConnection.size() != 0 && lastConnection.first().portName_ == EDIT_ROW_NAME)
        {
            removeRows(1);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::hasDuplicates()
//-----------------------------------------------------------------------------
bool BitMappingModel::hasDuplicates(General::PortBounds connection, int row) const
{
    foreach (General::PortBounds pins, rows_.value(row))
    {
        if (pins.left_ == connection.left_ && pins == connection)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::checkDirectionForPorts()
//-----------------------------------------------------------------------------
bool BitMappingModel::checkDirectionForPorts(QStringList const& ports)
{
    QStringList incompatiblePorts;
    foreach (QString const& portName, ports)
    {
        QSharedPointer<Port> port = component_->getPort(portName);

        if (port->getDirection() != DirectionTypes::INOUT &&
            absDef_->getPortDirection(logicalPort_, mode_) != DirectionTypes::INOUT &&
            port->getDirection() != absDef_->getPortDirection(logicalPort_, mode_))
        {
            incompatiblePorts.append(portName);
        }
    }

    if (!incompatiblePorts.isEmpty())
    {
        emit errorMessage("Directions between logical port \"" + logicalPort_ + "\" and physical port(s) " + 
            incompatiblePorts.join(", ") + " did not match.");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::addMapping()
//-----------------------------------------------------------------------------
int BitMappingModel::addMapping(General::PortBounds mapping, int row)
{
    int targetRow = row;
    int lowerBound = qMin(mapping.left_, mapping.right_);
    int higherBound = qMax(mapping.left_, mapping.right_);

    for (int i = lowerBound; i <= higherBound; i++)
    {
        if (targetRow >= rows_.size())
        {
            break;
        }

        General::PortBounds conn(mapping.portName_, i, i);
        if (!hasDuplicates(conn, targetRow))
        {
            rows_[targetRow].append(conn);
        }
        targetRow++;
    }

    return targetRow;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::createInitialMappings()
//-----------------------------------------------------------------------------
void BitMappingModel::createInitialMappings()
{
    QList<QSharedPointer<PortMap> > logicPortMaps;

    if (busif_->getPortMaps())
    {
        foreach (QSharedPointer<PortMap> portMap, *busif_->getPortMaps())
        {   
            if (portMap->getLogicalPort()->name_ == logicalPort_)
            {
                logicPortMaps.append(portMap);
            }
        }
    }

    QString logicalPortSize = absDef_->getPort(logicalPort_)->getWire()->getWidth(mode_);
    int logicalSize = expressionParser_->parseExpression(logicalPortSize).toInt();

    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (logicalPortSize.isEmpty())
    {
        foreach (QSharedPointer<PortMap> portMap, logicPortMaps)
        {   
            QPair<int, int> logicalBounds = findLogicalBounds(portMap);
            int higherBound = qMax(logicalBounds.first, logicalBounds.second);

            logicalSize = qMax(logicalSize, higherBound + 1);
        }
    }

    for (int index = 0; index < logicalSize; index++)
    {
        QList<General::PortBounds> pins;
        // Search for previous mappings.
        foreach(QSharedPointer<PortMap> portMap, logicPortMaps)
        {
            QPair<int, int> logicalBounds = findLogicalBounds(portMap);
            int logicalLeft = logicalBounds.first;
            int logicalRight = logicalBounds.second;

            int logLower = qMin(logicalLeft, logicalRight);
            int logHigher = qMax(logicalLeft, logicalRight);

            if (logLower <= index && index <= logHigher)
            {
                int physicalRangeLeft = 0;
                int physicalRangeRight = 0;

                if (portMap->getPhysicalPort()->partSelect_)
                {
                    physicalRangeLeft = expressionParser_->parseExpression(
                        portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
                    physicalRangeRight = expressionParser_->parseExpression(
                        portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();
                }
                else
                {
                    physicalRangeLeft = expressionParser_->parseExpression(
                        component_->getPort(portMap->getPhysicalPort()->name_)->getLeftBound()).toInt();
                    physicalRangeRight = expressionParser_->parseExpression(
                        component_->getPort(portMap->getPhysicalPort()->name_)->getRightBound()).toInt();
                }

                int physLower = qMin(physicalRangeLeft, physicalRangeRight);

                General::PortBounds toAdd(portMap->getPhysicalPort()->name_);

                int physicalSize = abs(physicalRangeLeft - physicalRangeRight) + 1;
                if (physicalSize > 1)
                {
                    int physIndex = index - (logLower - physLower);
                    toAdd.left_ = physIndex;
                    toAdd.right_ = physIndex;
                }
                pins.append(toAdd);
            }
        }                
        rows_.append(pins);
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::saveCurrentMappings()
//-----------------------------------------------------------------------------
void BitMappingModel::saveCurrentMappings()
{
    if (!logicalPort_.isEmpty())
    {
        mappings_.insert(logicalPort_, rows_);
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::restoreMappings()
//-----------------------------------------------------------------------------
void BitMappingModel::restoreMappings()
{
    rows_ = mappings_.value(logicalPort_);
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::isMapped()
//-----------------------------------------------------------------------------
bool BitMappingModel::isMapped(QList<QSharedPointer<PortMap> > mappings, General::PortBounds const& pin, 
    int logicalIndex) const
{
    foreach (QSharedPointer<PortMap> portMap, mappings)
    {
        if (portMap->getPhysicalPort()->name_ == pin.portName_)
        {            
            QPair<int, int> logicalBounds = findLogicalBounds(portMap);
            int logLowerBound = qMin(logicalBounds.first, logicalBounds.second);
            int logHigherBound = qMax(logicalBounds.first, logicalBounds.second);

            int physicalLeft = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
            int physicalRight = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();

            int phyLowerBound = qMin(physicalLeft, physicalRight);
            int phyHigherBound = qMax(physicalLeft, physicalRight);

            if (logLowerBound <= logicalIndex && logicalIndex <= logHigherBound &&
                phyLowerBound <= pin.left_ && pin.left_ <= phyHigherBound )
            {
                return true;            
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::createPortMapForPin()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> BitMappingModel::createPortMapForPin(General::PortBounds const& pin, 
    int firstLogicalIndex) const
{
    QSharedPointer<Port> port = component_->getPort(pin.portName_);

    int portLeftBound = expressionParser_->parseExpression(port->getLeftBound()).toInt();
    int portRightBound = expressionParser_->parseExpression(port->getRightBound()).toInt();
    bool ascending = portLeftBound >= portRightBound;

    // Create a new port map.      
    int logLeft = firstLogicalIndex;
    int logRight = firstLogicalIndex;
    int physLeft = pin.left_;
    int physRight = pin.left_;

    // Check how many contiguous bits are connected in ascending order.
    if (firstLogicalIndex != rows_.size() - 1)
    {                    
        bool continueMapping = true;
        for (int index = firstLogicalIndex + 1; index < rows_.size() && continueMapping; index++)
        {
            continueMapping = false;
            foreach (General::PortBounds const& conn, rows_[index])
            {
                if (conn == pin)
                { 
                    if (ascending && conn.left_ == physLeft + 1)                                
                    {
                        physLeft++;
                        logLeft++;
                        continueMapping = true;
                        break;
                    }
                    else if (!ascending && conn.left_ == physRight + 1)
                    {
                        physRight++;
                        logRight++;
                        continueMapping = true;
                        break;
                    }
                }
            }                   
        } 
    }

    QSharedPointer<PortMap> map(new PortMap());
    QSharedPointer<PortMap::LogicalPort> logical(new PortMap::LogicalPort(logicalPort_));
    QSharedPointer<Range> logicalRange(new Range(QString::number(logLeft), QString::number(logRight)));    
    logical->range_ = logicalRange;
    map->setLogicalPort(logical);

    QSharedPointer<PortMap::PhysicalPort> physical(new PortMap::PhysicalPort(pin.portName_));

    QSharedPointer<PartSelect> physicalRange(new PartSelect(QString::number(physLeft), QString::number(physRight)));
    physical->partSelect_ = physicalRange;
    map->setPhysicalPort(physical);

    return map;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::findLogicalBounds()
//-----------------------------------------------------------------------------
QPair<int, int> BitMappingModel::findLogicalBounds(QSharedPointer<PortMap> portMap) const
{
    if (portMap->getLogicalPort()->range_)
    {
        int leftBound = expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
        int rightBound = expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
        return qMakePair(leftBound, rightBound);
    }
    else if (component_->hasPort(portMap->getPhysicalPort()->name_))
    {
        int leftBound = expressionParser_->parseExpression(
            component_->getPort(portMap->getPhysicalPort()->name_)->getLeftBound()).toInt();
        int rightBound = expressionParser_->parseExpression( 
            component_->getPort(portMap->getPhysicalPort()->name_)->getRightBound()).toInt();
        return qMakePair(abs(leftBound - rightBound), 0);
    }

    return qMakePair(0, 0);
}
