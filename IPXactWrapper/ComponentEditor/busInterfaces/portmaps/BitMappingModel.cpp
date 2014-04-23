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

#include <QStringList>
#include <QColor>
#include <QIcon>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionEndpoint.h>
#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/models/PortGenerationRow.h>
#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/component.h>
#include <models/generaldeclarations.h>
#include "models/port.h"
#include <models/portabstraction.h>

#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitSelectionDialog.h>

namespace
{
    QString const HEADER_NAMES[] =
    {
        "Logical\nIndex",
        "Bit Field(s)"
    }; 

    QString const EDIT_ROW_NAME = "Drop port to add rows.";
}

Q_DECLARE_METATYPE(General::PortBounds)

    //-----------------------------------------------------------------------------
    // Function: BitMappingModel()
    //-----------------------------------------------------------------------------
    BitMappingModel::BitMappingModel(BusInterface* busif,
    QSharedPointer<Component> component,
    LibraryInterface* libHandler, 
    QObject* parent)
    : QAbstractTableModel(parent), 
    busif_(busif),
    mode_(General::INTERFACE_MODE_COUNT),
    logicalPort_(),
    portMaps_(busif->getPortMaps()),
    component_(component),
    handler_(libHandler),        
    rows_(),
    mappings_(),
    canEdit_(false)
{
    Q_ASSERT(busif);
    Q_ASSERT(component);
    Q_ASSERT(libHandler);
}

//-----------------------------------------------------------------------------
// Function: ~BitMappingModel()
//-----------------------------------------------------------------------------
BitMappingModel::~BitMappingModel()
{
}

//-----------------------------------------------------------------------------
// Function: rowCount()
//-----------------------------------------------------------------------------
int BitMappingModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: columnCount()
//-----------------------------------------------------------------------------
int BitMappingModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: data()
//-----------------------------------------------------------------------------
QVariant BitMappingModel::data(const QModelIndex& index, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( !index.isValid() )
    {
        return QVariant();
    }

    if ( index.row() > rows_.size() || index.column() >= COLUMN_COUNT )
    {
        return QVariant();
    }

    if ( role == Qt::DisplayRole ) 
    {                  

        switch (index.column()) {
        case INDEX :
            {
                return index.row();
            }
        case BIT : 
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
                        int left = component_->getPortLeftBound(row.portName_);
                        int right = component_->getPortRightBound(row.portName_);
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
        default :
            {
                return QVariant();
            }
        }
    }

    else if ( role == Qt::ForegroundRole )
    {      
        if ((index.column() == INDEX && !canEdit_) ||
            (canEdit_ && index.row()  == rows_.size() - 1) ||          
            (index.column() == BIT && rows_.at(index.row()).isEmpty()))
        {
            return QColor("gray");
        }  
        return QColor("black");
    }

    else if ( role == Qt::BackgroundRole )
    {
        return QColor("white");
    }

    else if (role == Qt::TextAlignmentRole)
    {
        return (Qt::AlignLeft + Qt::AlignVCenter);     
    }

    else if ( role == Qt::EditRole )
    {
        if (index.column() == BIT)
        {
            return data(index, Qt::DisplayRole);
        }
    }

    else if ( role == Qt::ToolTipRole )
    {
        // TODO
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: headerData()
//-----------------------------------------------------------------------------
QVariant BitMappingModel::headerData(int section, Qt::Orientation orientation, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( orientation != Qt::Horizontal )
    {
        return QVariant();  
    }

    if ( role == Qt::DisplayRole ) 
    {
        if ( section < COLUMN_COUNT )
        {
            return HEADER_NAMES[section];
        }               
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: setData()
//-----------------------------------------------------------------------------
bool BitMappingModel::setData(const QModelIndex& index, const QVariant& value, 
    int role /*Qt::EditRole*/ )
{
    if (!index.isValid())
    {
        return false;
    }
    // if row is invalid
    else if ( index.row() < 0 || rows_.size() < index.row() ){
        return false;
    }

    else if ( index.column() != BIT )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        switch (index.column()) 
        {

        case BIT :
            {                
                if(value.canConvert<General::PortBounds>())
                {
                    int lastRow = addMapping(value.value<General::PortBounds>(), index.row());
                    emit dataChanged(index, createIndex(lastRow, BIT));                    
                    return true;
                }
                // Clear cell.
                else if (value.toString().isEmpty() && !(canEdit_ && index.row() == rows_.size() - 1))
                {
                    rows_[index.row()].clear();       
                    emit dataChanged(index,index);
                    return true;
                }

                return false;

            }        
        default :
            {
                return false;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: setHeaderData()
//-----------------------------------------------------------------------------
bool BitMappingModel::setHeaderData( int section, Qt::Orientation orientation, 
    const QVariant & value, int role /*= Qt::EditRole*/ )
{
    if( orientation != Qt::Horizontal )
    {
        return false;
    }

    if ( !value.isValid() )
    {
        return false;
    }

    if( role == Qt::EditRole && section < COLUMN_COUNT )
    {
        QAbstractTableModel::setHeaderData(section,orientation, value, role);
        emit headerDataChanged(orientation,section,section);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BitMappingModel::flags(const QModelIndex& index) const
{
    if ( !index.isValid() )
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
// Function: isValid()
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
bool BitMappingModel::dropMimeData(const QMimeData *data, 
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if ( action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if ( row != -1 || column != -1 || !parent.isValid() )
    {
        return false;
    }

    if (data->hasText())
    {
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
        foreach (QString portName, portNames)
        {
            if (!component_->hasPort(portName))
            {
                continue;
            }

            if (targetRow >= rowCount() && !canEdit_)
            {
                break;
            }

            int left = component_->getPort(portName)->getLeftBound();
            int right = component_->getPort(portName)->getRightBound();
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
            General::PortBounds portData(portName, left, right);
            addMapping(portData, targetRow);            
            targetRow += abs(portData.left_ - portData.right_) + 1;
        }

        if (canEdit_)
        {
            addEditRow();
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: BitMappingModel::mapLast()
//-----------------------------------------------------------------------------
void BitMappingModel::mapToEnd(QStringList portNames)
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
        canEdit_ = (absDef_->getPortSize(logicalName, mode_) == -1);

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
void BitMappingModel::setAbsType(const VLNV& absDefVlnv, General::InterfaceMode mode)
{
    if (!absDefVlnv.isValid())
    {
        return;
    }

    // ask library to parse the model for abstraction definition
    QSharedPointer<LibraryComponent> libComb;
    if (handler_->contains(absDefVlnv))
    { 
        libComb = handler_->getModel(absDefVlnv);
    }
    // if library did not contain the vlnv
    else
    {
        return;
    }
    // make sure the model is for abstraction definition
    if (handler_->getDocumentType(absDefVlnv) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef_ = libComb.staticCast<AbstractionDefinition>();
    }

    mode_ = mode;

    // Clear the table.
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
QList< QSharedPointer<General::PortMap> > BitMappingModel::getPortMaps() const
{                    
    QList<QSharedPointer<General::PortMap> > mappings;

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
                QSharedPointer<General::PortMap> map = createPortMapForPin(pin, logIndex);
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
    if (canEdit_){
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
        if (lastConnection.size() != 0 && 
            lastConnection.first().portName_ == EDIT_ROW_NAME)
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
        if ( pins.left_ == connection.left_ && pins == connection)
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
    foreach (QString portName, ports)
    {
        if (component_->getPortDirection(portName) != General::INOUT &&
            absDef_->getPortDirection(logicalPort_, mode_) != General::INOUT &&
            component_->getPortDirection(portName) != absDef_->getPortDirection(logicalPort_, mode_))
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
    int logicalPortSize = absDef_->getPortSize(logicalPort_, mode_);

    QList<QSharedPointer<General::PortMap> > logicPortMaps;

    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {   
        if (portMap->logicalPort_ == logicalPort_)
        {
            logicPortMaps.append(portMap);
        }
    }

    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (logicalPortSize == -1)
    {
        foreach (QSharedPointer<General::PortMap> portMap, logicPortMaps)
        {   
            General::PortBounds logicalRange = portMap->getLogicalRange(component_->getPort(portMap->physicalPort_));
            int higherBound = qMax(logicalRange.left_,logicalRange.right_);
            logicalPortSize = qMax(logicalPortSize, higherBound);
        }
        logicalPortSize++;
    }

    for (int index = 0; index < logicalPortSize; index++)
    {
        QList<General::PortBounds> pins;
        // Search for previous mappings.
        foreach(QSharedPointer<General::PortMap> portMap, logicPortMaps)
        {

            General::PortBounds logicalRange = portMap->getLogicalRange(component_->getPort(portMap->physicalPort_));
            int logLower = qMin(logicalRange.left_, logicalRange.right_);
            int logHigher = qMax(logicalRange.left_, logicalRange.right_);
            if (logLower <= index && index <= logHigher)
            {
                General::PortBounds physBounds = portMap->getPhysicalRange(component_->getPort(portMap->physicalPort_));
                int physLower = qMin(physBounds.left_, physBounds.right_);
                General::PortBounds toAdd(portMap->physicalPort_);     
                if (abs(physBounds.left_ - physBounds.right_) + 1 > 1)
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
bool BitMappingModel::isMapped(QList<QSharedPointer<General::PortMap> > mappings, 
    General::PortBounds const& pin, int logicalIndex) const
{
    foreach (QSharedPointer<General::PortMap> portMap, mappings)
    {
        if (portMap->physicalPort_ == pin.portName_)
        {
            General::PortBounds logicalRange = portMap->getLogicalRange(component_->getPort(portMap->physicalPort_));
            int logLowerBound = qMin(logicalRange.left_, logicalRange.right_);
            int logHigherBound = qMax(logicalRange.left_, logicalRange.right_);
            General::PortBounds physicalRange = portMap->getPhysicalRange(component_->getPort(portMap->physicalPort_));

            int phyLowerBound = qMin(physicalRange.left_, physicalRange.right_);
            int phyHigherBound = qMax(physicalRange.left_, physicalRange.right_);

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
// Function: BitMappingModel::MyMethod()
//-----------------------------------------------------------------------------
QSharedPointer<General::PortMap> BitMappingModel::createPortMapForPin(General::PortBounds const& pin, 
    int firstLogicalIndex) const
{
    bool ascending = component_->getPortLeftBound(pin.portName_) >= component_->getPortRightBound(pin.portName_);

    // Create a new port map.      
    int logLeft = firstLogicalIndex;
    int logRight = firstLogicalIndex;
    int physLeft = pin.left_;
    int physRight = pin.left_;

    // Check how many contiguous bits are connected  in ascending order.
    if (firstLogicalIndex != rows_.size() - 1)
    {                    
        bool stretch = true;
        for (int index = firstLogicalIndex + 1; index < rows_.size() && stretch; index++)
        {
            stretch = false;
            foreach (General::PortBounds conn, rows_[index])
            {
                if (conn == pin )
                { 
                    if (ascending && conn.left_ == physLeft + 1)                                
                    {
                        physLeft++;
                        logLeft++;
                        stretch = true;
                        break;
                    }
                    else if (!ascending && conn.left_ == physRight + 1)
                    {
                        physRight++;
                        logRight++;
                        stretch = true;
                        break;
                    }
                }
            }                   
        } 
    }

    QSharedPointer<General::PortMap> map(new General::PortMap());
    map->logicalPort_ = logicalPort_;
    map->logicalVector_->setLeft(logLeft);
    map->logicalVector_->setRight(logRight);
    map->physicalPort_ = pin.portName_;
    map->physicalVector_->setLeft(physLeft);
    map->physicalVector_->setRight(physRight);
    return map;
}








