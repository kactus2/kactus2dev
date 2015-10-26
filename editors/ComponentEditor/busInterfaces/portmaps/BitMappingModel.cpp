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

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/vlnv.h>

#include <QStringList>
#include <QColor>
#include <QIcon>

namespace
{
    QString const HEADER_NAMES[] =
    {
        "Logical\nIndex",
        "Bit Field(s)"
    }; 

    QString const EDIT_ROW_NAME("Drop port to add rows.");
}

Q_DECLARE_METATYPE(General::PortBounds)

//-----------------------------------------------------------------------------
// Function: BitMappingModel::BitMappingModel()
//-----------------------------------------------------------------------------
BitMappingModel::BitMappingModel(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component, 
    LibraryInterface* libHandler, QObject* parent) : 
      QAbstractTableModel(parent), 
      rows_(),
      mappings_(),
      busif_(busif),
      mode_(General::INTERFACE_MODE_COUNT),
      logicalPort_(),
      portMaps_(busif->getPortMaps()),
      component_(component),
      handler_(libHandler),                    
      canEdit_(false)
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
                QStringList ports;
                foreach (General::PortBounds row, rows_.at(index.row()))
                {
                    QSharedPointer<Port> port = component_->getPort(row.portName_);

                    int left = port->getLeftBound().toInt();
                    int right = port->getRightBound().toInt();
                    int portSize = abs(left - right) + 1;

                    if (portSize > 1)
                    {
                        ports.append(row.portName_ + "(" + QString::number(row.left_) + ")");
                    }
                    else
                    {
                        ports.append(row.portName_);
                    }                        
                }
                return ports.join(", ");
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
    if (orientation != Qt::Horizontal)
    {
        return QVariant();  
    }

    if (role == Qt::DisplayRole && section < COLUMN_COUNT) 
    {
        return HEADER_NAMES[section];              
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
// Function: BitMappingModel::setHeaderData()
//-----------------------------------------------------------------------------
bool BitMappingModel::setHeaderData(int section, Qt::Orientation orientation, QVariant const& value, int role)
{
    if (orientation != Qt::Horizontal || !value.isValid())
    {
        return false;
    }

    if (role == Qt::EditRole && section < COLUMN_COUNT)
    {
        QAbstractTableModel::setHeaderData(section, orientation, value, role);
        emit headerDataChanged(orientation, section, section);
        return true;
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
            int left = port->getLeftBound().toInt();
            int right = port->getRightBound().toInt();
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

    logicalPort_ = logicalName;
    // Reset the table.
    beginResetModel();
    rows_.clear();

    if (!logicalName.isEmpty())
    {
        // Indexes can be added/removed, if absDef does not define size.
        canEdit_ = (absDef_->getPort(logicalName)->getWire()->getWidth(mode_).isEmpty()); //Size(logicalName, mode_) == -1);

        if (mappings_.contains(logicalName))
        {
            restoreMappings();
        } 
        else
        {
            createInitialMappings();

            if (canEdit_)
            {
                addEditRow();
            }
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
    QSharedPointer<Document> libComb  = handler_->getModel(absDefVlnv);
    if (!libComb)
    { 
        return;
    }

    // make sure the model is for abstraction definition
    if (handler_->getDocumentType(absDefVlnv) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef_ = libComb.staticCast<AbstractionDefinition>();
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
        foreach(General::PortBounds pin, rows_[logIndex])
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
        emit errorMessage("Directions between logical port \"" + logicalPort_ + 
            "\" and physical port(s) " + incompatiblePorts.join(", ") + " did not match.");
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

    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {   
        if (portMap->getLogicalPort()->name_ == logicalPort_)
        {
            logicPortMaps.append(portMap);
        }
    }

    QString logicalPortSize = absDef_->getPort(logicalPort_)->getWire()->getWidth(mode_);

    int logicalSize = logicalPortSize.toInt();
    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (logicalPortSize.isEmpty())
    {
        foreach (QSharedPointer<PortMap> portMap, logicPortMaps)
        {   
            QSharedPointer<Range> logicalRange = portMap->getLogicalPort()->range_;
            int higherBound = qMax(logicalRange->getLeft().toInt(), logicalRange->getRight().toInt());
            logicalSize = qMax(logicalSize, higherBound);
        }
        logicalSize++;
    }

    for (int index = 0; index < logicalSize; index++)
    {
        QList<General::PortBounds> pins;
        // Search for previous mappings.
        foreach(QSharedPointer<PortMap> portMap, logicPortMaps)
        {
            QSharedPointer<Range> logicalRange = portMap->getLogicalPort()->range_;
            int logLower = qMin(logicalRange->getLeft().toInt(), logicalRange->getRight().toInt());
            int logHigher = qMax(logicalRange->getLeft().toInt(), logicalRange->getRight().toInt());
            if (logLower <= index && index <= logHigher)
            {
                int physicalRangeLeft = portMap->getPhysicalPort()->partSelect_->getLeftRange().toInt();
                int physicalRangeRight = portMap->getPhysicalPort()->partSelect_->getRightRange().toInt();
                int physLower = qMin(physicalRangeLeft, physicalRangeRight);

                General::PortBounds toAdd(portMap->getPhysicalPort()->name_);
                if (abs(physicalRangeLeft - physicalRangeRight) + 1 > 1)
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
            QSharedPointer<Range> logicalRange = portMap->getLogicalPort()->range_;
            int logLowerBound = qMin(logicalRange->getLeft().toInt(), logicalRange->getRight().toInt());
            int logHigherBound = qMax(logicalRange->getLeft().toInt(), logicalRange->getRight().toInt());
          
            int physicalLeft = portMap->getPhysicalPort()->partSelect_->getLeftRange().toInt();
            int physicalRight = portMap->getPhysicalPort()->partSelect_->getRightRange().toInt();

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

    bool ascending = port->getLeftBound().toInt() >= port->getRightBound().toInt();

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
    map->getLogicalPort()->name_ = logicalPort_;
    map->getLogicalPort()->range_->setLeft(QString::number(logLeft));
    map->getLogicalPort()->range_->setRight(QString::number(logRight));

    map->getPhysicalPort()->name_ = pin.portName_;
    map->getPhysicalPort()->partSelect_->setLeftRange(QString::number(physLeft));
    map->getPhysicalPort()->partSelect_->setRightRange(QString::number(physRight));

    return map;
}
