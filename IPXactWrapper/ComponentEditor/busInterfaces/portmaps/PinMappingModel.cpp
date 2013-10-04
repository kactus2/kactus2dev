//-----------------------------------------------------------------------------
// File: PinMappingModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.09.2013
//
// Description:
// Table model for bit field mapping.
//-----------------------------------------------------------------------------

#include "PinMappingModel.h"

#include <QStringList>
#include <QColor>
#include <QIcon>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionEndpoint.h>
#include <designwidget/HWComponentItem.h>
#include <designwidget/models/PortGenerationRow.h>
#include <models/component.h>
#include "models/port.h"
#include <models/businterface.h>
#include <models/abstractiondefinition.h>
#include <models/portabstraction.h>

#include <LibraryManager/libraryinterface.h>
#include "models/generaldeclarations.h"
#include "LibraryManager/vlnv.h"
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitSelectionDialog.h>

namespace
{
    QString const HEADER_NAMES[] =
    {
        "Logical\nIndex",
        "Bit field(s)"
    }; 
}

Q_DECLARE_METATYPE(PinMappingModel::PinMapDropData)

//-----------------------------------------------------------------------------
// Function: PinMappingModel()
//-----------------------------------------------------------------------------
PinMappingModel::PinMappingModel(BusInterface* busif,
    QSharedPointer<Component> component,
    LibraryInterface* libHandler, 
    QObject* parent)
    : QAbstractTableModel(parent), 
    parentWidget_(dynamic_cast<QWidget*>(parent)),
    busif_(busif),
    mode_(General::INTERFACE_MODE_COUNT),
    logicalPort_(),
    portMaps_(busif->getPortMaps()),
    component_(component),
    handler_(libHandler),    
    rows_(),
    canChange_(false)
{
    Q_ASSERT(parentWidget_);
    Q_ASSERT(busif);
    Q_ASSERT(component);
    Q_ASSERT(libHandler);
}

//-----------------------------------------------------------------------------
// Function: ~PinMappingModel()
//-----------------------------------------------------------------------------
PinMappingModel::~PinMappingModel()
{
}

//-----------------------------------------------------------------------------
// Function: rowCount()
//-----------------------------------------------------------------------------
int PinMappingModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: columnCount()
//-----------------------------------------------------------------------------
int PinMappingModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: data()
//-----------------------------------------------------------------------------
QVariant PinMappingModel::data(const QModelIndex& index, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( !index.isValid() )
    {
        return QVariant();
    }
    
    if ( !rows_.contains(index.row()) || index.column() >= COLUMN_COUNT )
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
        case PIN : 
            {                
                if (rows_.value(index.row()).isEmpty())
                {
                    return logicalPort_;
                }
                else
                {
                    QStringList ports;
                    foreach (PinMappingModel::PinConnection row, rows_.value(index.row()))
                    {
                        if (component_->getPortWidth(row.physicalName) > 1)
                        {
                            ports.append(row.physicalName + "(" + QString::number(row.physicalIndex) + ")");
                        }
                        else
                        {
                            ports.append(row.physicalName);
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
        if ((index.column() == INDEX && !canChange_) ||
            (index.column() == PIN && rows_.value(index.row()).isEmpty()))
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
        if (index.column() == PIN)
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
QVariant PinMappingModel::headerData(int section, Qt::Orientation orientation, 
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
bool PinMappingModel::setData(const QModelIndex& index, const QVariant& value, 
    int role /*Qt::EditRole*/ )
{
    if (!index.isValid())
    {
        return false;
    }
    // if row is invalid
    else if ( index.row() < 0 || !rows_.contains(index.row()) ){
        return false;
    }

    else if ( index.column() != PIN )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        switch (index.column()) 
        {

        case PIN :
            {
                if(value.canConvert<PinMapDropData>())
                {
                    PinMapDropData data = value.value<PinMapDropData>();
                    int targetRow = index.row();
                    for (int i = data.right; i <= data.left; i++)
                    {
                        if (targetRow > rows_.size())
                        {
                            break;
                        }

                        PinConnection conn(i, data.physName);
                        if (!hasDuplicates(conn, targetRow))
                        {
                            rows_[targetRow].append(conn);
                        }
                        targetRow++;
                    }
                    emit dataChanged(index,index);
                    return true;
                }
                else if (value.toString().isEmpty())
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
bool PinMappingModel::setHeaderData( int section, Qt::Orientation orientation, 
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
Qt::ItemFlags PinMappingModel::flags(const QModelIndex& index) const
{
    if ( !index.isValid() )
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == PIN)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }

    return Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool PinMappingModel::isValid() const
{
    return true;
}


//-----------------------------------------------------------------------------
// Function: PinMappingModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions PinMappingModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


//-----------------------------------------------------------------------------
// Function: PinMappingModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList PinMappingModel::mimeTypes() const
{
   QStringList types = QAbstractTableModel::mimeTypes();
    types << "text/plain";
    return types;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::dropMimeData()
//-----------------------------------------------------------------------------
bool PinMappingModel::dropMimeData(const QMimeData *data, 
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
       
        QList<QVariant> dropData;

        int targetRow = parent.row();        
        foreach (QString portName, portNames)
        {
            if (!component_->hasPort(portName))
            {
                continue;
            }

            if (targetRow >= rowCount())
            {
                break;
            }

            int portSize = component_->getPortWidth(portName);
            int left = portSize - 1;
            int right = 0;
            int rowsLeft = rowCount() - targetRow;

        if (portSize > rowsLeft)
        {
            BitSelectionDialog* dialog = new BitSelectionDialog(portSize, rowsLeft, parentWidget_ );
            if (dialog->exec() != QDialog::Accepted)
            {
                return false;
            }
            left = dialog->getLeft();
            right = dialog->getRight();
        }

        PinMappingModel::PinMapDropData pinData;        
        pinData.physName = portName;
        pinData.left = left;
        pinData.right = right;

        setData(index(targetRow, PIN), QVariant::fromValue(pinData));
        targetRow += left - right + 1;
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::setAbsType()
//-----------------------------------------------------------------------------
void PinMappingModel::setAbsType(const VLNV& absDefVlnv, General::InterfaceMode mode)
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
// Function: PinMappingModel::setLogicalSignal()
//-----------------------------------------------------------------------------
void PinMappingModel::setLogicalSignal(QString& logicalName)
{
    logicalPort_ = logicalName;
    // Reset the table.
    beginResetModel();
    rows_.clear();
    if (!logicalName.isEmpty())
    {
        int logicalPortSize = absDef_->getPortSize(logicalName, mode_);

        // Indexes can be added/removed, if absDef does not define size.
        canChange_ = (logicalPortSize == -1);

        for (int index = 0; index < logicalPortSize; index++)
        {

            QList<PinMappingModel::PinConnection> pins;

            // Search for previous mappings.
            foreach(QSharedPointer<General::PortMap> portMap, portMaps_)
            {
                if (portMap->logicalPort_ == logicalName)
                {
                    int logLeft = portMap->logicalVector_->getLeft();
                    int logRight = portMap->logicalVector_->getRight();
                    if ((logLeft >= index && index >= logRight) || (logLeft <= index && index <= logRight))
                    {
                        PinConnection toAdd(0, portMap->physicalPort_);                    
                        //toAdd.physicalName = portMap->physicalPort_;
                        if (component_->getPortWidth(portMap->physicalPort_) > 1)
                        {
                            toAdd.physicalIndex = index - (logRight - portMap->physicalVector_->getRight());
                        }                    
                        pins.append(toAdd);
                    }
                }
            }
            rows_.insert(index, pins);
        }
    }
    endResetModel();

    emit canModify(canChange_);
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::getPortMaps()
//-----------------------------------------------------------------------------
QList< QSharedPointer<General::PortMap> > PinMappingModel::getPortMaps() const
{                    
    QList<QSharedPointer<General::PortMap> > mappings;
    
    for (int logIndex = 0; logIndex < rows_.size(); logIndex++)
    {
        foreach(PinConnection pin, rows_[logIndex])
        {           
            // Check if physical port is already mapped.
            bool alreadyMapped = false;            
            foreach (QSharedPointer<General::PortMap> portMap, mappings)
            {
                if (portMap->physicalPort_ == pin.physicalName &&
                    portMap->logicalVector_->getRight() <= logIndex &&
                    logIndex <= portMap->logicalVector_->getLeft() &&
                    portMap->physicalVector_->getRight() <= pin.physicalIndex &&
                    pin.physicalIndex <= portMap->physicalVector_->getLeft() )
                {
                    alreadyMapped = true;
                    break;
                }
            }
            
            if (!alreadyMapped) 
            {               
                // Create a new port map.      
                int logLeft = logIndex;
                int logRight = logIndex;
                int physLeft = pin.physicalIndex;
                int physRight = pin.physicalIndex;
    
                // Check how many contiguous bits are connected.
                bool stretch = true;
                for (int index = logIndex + 1; index < rows_.size() && stretch; index++)
                {
                    stretch = false;
                    foreach (PinConnection conn, rows_[index])
                    {
                        if (conn.physicalName == pin.physicalName &&
                            conn.physicalIndex == physLeft + 1)
                        {
                            physLeft++;
                            logLeft++;
                            stretch = true;
                            break;
                        }
                    }
                }
                      
                QSharedPointer<General::PortMap> map(new General::PortMap());
                map->logicalPort_ = logicalPort_;
                map->logicalVector_->setLeft(logLeft);
                map->logicalVector_->setRight(logRight);
                map->physicalPort_ = pin.physicalName;
                map->physicalVector_->setLeft(physLeft);
                map->physicalVector_->setRight(physRight);
                mappings.append(map);     
            }                               
        }
    }

    return mappings;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::hasDuplicates()
//-----------------------------------------------------------------------------
bool PinMappingModel::hasDuplicates(PinConnection connection, int row) const
{

    // Check that no duplicates are added.
    foreach (PinConnection pins, rows_.value(row))
    {
        if ( pins.physicalIndex == connection.physicalIndex && pins.physicalName == connection.physicalName)
        {
            return true;
        }
    }

    return false;
}



//-----------------------------------------------------------------------------
// Function: PinMappingModel::checkDirectionForPorts()
//-----------------------------------------------------------------------------
bool PinMappingModel::checkDirectionForPorts(QStringList const& ports)
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

