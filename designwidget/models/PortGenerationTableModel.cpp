//-----------------------------------------------------------------------------
// File: PortGenerationTableModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.5.2013
//
// Description:
// Table model for draft port generation.
//-----------------------------------------------------------------------------

#include "PortGenerationTableModel.h"

#include <QStringlist>
#include <QColor>

#include <common/graphicsItems/ComponentItem.h>
#include <designwidget/BusPortItem.h>
#include <designwidget/HWComponentItem.h>
#include <designwidget/models/PortGenerationRow.h>
#include <models/component.h>
#include "models/port.h"
#include <models/businterface.h>
#include <models/abstractiondefinition.h>
#include <models/portabstraction.h>

#include <LibraryManager/libraryinterface.h>

namespace
{
    QString const HEADER_DEFAULT_NAMES[PortGenerationRow::COLUMNS] =
    {
        "Description",
        "Direction",
        "Name",
        "Size",
        "Name",
        "Direction",
        "Description"
    }; 
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel()
//-----------------------------------------------------------------------------
PortGenerationTableModel::PortGenerationTableModel(QObject* parent)
    : QAbstractTableModel(parent), draftComponent_(0), 
    lockedColumns_(), rows_(),
    headers_()
{
    for( int i = 0; i < PortGenerationRow::COLUMNS; i++ )
    {
        headers_.append(HEADER_DEFAULT_NAMES[i]);
    }
}

//-----------------------------------------------------------------------------
// Function: ~PortGenerationTableModel()
//-----------------------------------------------------------------------------
PortGenerationTableModel::~PortGenerationTableModel()
{
}

//-----------------------------------------------------------------------------
// Function: rowCount()
//-----------------------------------------------------------------------------
int PortGenerationTableModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: columnCount()
//-----------------------------------------------------------------------------
int PortGenerationTableModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return PortGenerationRow::COLUMNS;
}

//-----------------------------------------------------------------------------
// Function: data()
//-----------------------------------------------------------------------------
QVariant PortGenerationTableModel::data(const QModelIndex& index, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( !index.isValid() )
    {
        return QVariant();
    }
    
    if ( index.row() >= rows_.size() || index.column() >= PortGenerationRow::COLUMNS )
    {
        return QVariant();
    }

    if ( role == Qt::DisplayRole ) 
    {
        QSharedPointer<PortGenerationRow> row = rows_.at(index.row());
        switch (index.column()) {

        case SRC_DESCRIPTION : 
            {
                return row->getSourceDescription();
            }   
        case SRC_DIRECTION : 
            {
                return General::direction2Str(row->getSourceDirection());
            }
        case SRC_NAME : 
            {
                return row->getSourceName();
            }
        case SIZE : 
            {
                return row->getSize();
            }
        case DRAFT_NAME :
            {
                return row->getDraftName();
            }
        case DRAFT_DIRECTION :
            {
                General::Direction dir = row->getDraftDirection();
                if ( dir == General::DIRECTION_INVALID )
                {
                    return QString(tr("undefined"));
                }
                return General::direction2Str(dir);
            }
        case DRAFT_DESCRIPTION : 
            {
                return row->getDraftDescription();
            }
        default :
            {
                return QVariant();
            }
        }
    }

    else if ( role == Qt::ForegroundRole )
    {
        
        if ( index.column() == DRAFT_NAME && nameDuplicates(index.row()) )
        {
            return QColor("red");
        } 
        else if ( index.column() == DRAFT_DIRECTION && 
            rows_.at(index.row())->getDraftDirection() == General::DIRECTION_INVALID )
        {
            return QColor("red");
        }
        else if ( columnLocked(index.column()) )
        {
            return QColor("gray");
        }

        return QColor("black");
    }

    else if ( role == Qt::BackgroundRole )
    {
        if ( index.column() == DRAFT_NAME )
        {
            return QColor("LemonChiffon");
        }

        return QColor("white");
    }

    else if (role == Qt::TextAlignmentRole)
    {
        if ( index.column() == SRC_DIRECTION || index.column() == SIZE || 
            index.column() == DRAFT_DIRECTION )
        {
            return Qt::AlignCenter;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }

    else if ( role == Qt::EditRole )
    {
        if ( !columnLocked(index.column()) )
        {
            return data(index, Qt::DisplayRole);
        }
    }

    else if ( role == Qt::ToolTipRole )
    {
        if ( index.column() == DRAFT_NAME )
        {
            if ( nameDuplicates(index.row()) )
            {
                return QString(tr("Name %1 already in use.").arg(data(index).toString()));
            } 
        }
        else if ( index.column() == DRAFT_DIRECTION )
        {
            if ( rows_.at(index.row())->getDraftDirection() == General::DIRECTION_INVALID )
            {
                return QString(tr("Direction not defined in bus abstraction definition."));
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: headerData()
//-----------------------------------------------------------------------------
QVariant PortGenerationTableModel::headerData(int section, Qt::Orientation orientation, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( orientation != Qt::Horizontal )
    {
        return QVariant();
    }

    if ( role == Qt::DisplayRole ) 
    {
        if ( section < headers_.size() )
        {
            return headers_.at(section);
        }
        else{
            return QVariant();
        }

        // if unsupported role
    }
    else 
    {
        return QVariant();
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: setData()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::setData(const QModelIndex& index, const QVariant& value, 
    int role /*Qt::EditRole*/ )
{
    if (!index.isValid())
    {
        return false;
    }
    // if row is invalid
    else if ( index.row() < 0 || index.row() >= rows_.size() ){
        return false;
    }
    else if ( columnLocked(index.column()) )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        QSharedPointer<PortGenerationRow> row = rows_.at(index.row());
        switch (index.column()) 
        {

        case DRAFT_NAME :
            {
                row->setDraftName(value.toString());
                emit dataChanged(index,index);
                return true;
            }
        case DRAFT_DIRECTION :
            {
                row->setDraftDirection(General::str2Direction(value.toString(),General::DIRECTION_INVALID));
                emit dataChanged(index,index);
                return true;
            }
        case DRAFT_DESCRIPTION : 
            {
                row->setDraftDescription(value.toString());
                emit dataChanged(index,index);
                return true;
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
bool PortGenerationTableModel::setHeaderData( int section, Qt::Orientation orientation, 
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

    if( role == Qt::EditRole && section < headers_.size() )
    {
        headers_.replace(section,value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortGenerationTableModel::flags(const QModelIndex& index) const
{
    if ( !index.isValid() )
    {
        return Qt::NoItemFlags;
    }

    if ( columnLocked( index.column()) )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::isValid() const
{
    for ( int i = 0; i < rows_.size(); i++ ) 
    {
        if ( !rows_.at(i)->isValid() || nameDuplicates(i) )
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: initialize()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::initialize(LibraryInterface* lh, BusPortItem const* sourceBusPort,
    BusPortItem const* draftBusPort, General::InterfaceMode selectedMode)
{
    // Store pointer to draft component.
    draftComponent_ = draftBusPort->encompassingComp()->componentModel();

    // Get the abstract definition of the bus interface.
    QSharedPointer<BusInterface> busIf = sourceBusPort->getBusInterface();
    QSharedPointer<LibraryComponent> libComp = lh->getModel(busIf->getAbstractionType());
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

    beginResetModel();    
    //rows_.clear();

    bool createNew = rows_.empty();
    QSharedPointer<PortGenerationRow> row = QSharedPointer<PortGenerationRow>();
    int count = 0;
    // Create table rows based on ports in opposing interface. 
    foreach ( QSharedPointer<General::PortMap> portMap, busIf->getPortMaps() )
    {
        QString portName = portMap->physicalPort_;          
        QSharedPointer<Port> port = sourceBusPort->encompassingComp()->componentModel()->getPort(portName);

        if ( createNew )
        {
            row = QSharedPointer<PortGenerationRow>(new PortGenerationRow(portName, port->getDirection(),
                port->getDescription(), port->getPortSize()));
        }
        else
        {
            row = rows_.at(count);
        }

        General::Direction draftDir = absDef->getPortDirection(portMap->logicalPort_, selectedMode);
        row->setDraftDirection(draftDir);
        row->setDraftName(generateName(portName, port->getDirection(), draftDir));
        if ( createNew )
        {
            rows_.append(QSharedPointer<PortGenerationRow>(row));
        }

        count++;
    }

    endResetModel();

    lockColumn(SRC_DESCRIPTION);
    lockColumn(SRC_DIRECTION);
    lockColumn(SRC_NAME);
    lockColumn(SIZE);
    lockColumn(DRAFT_DIRECTION);

}

//-----------------------------------------------------------------------------
// Function: lockColumn()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::lockColumn(int const column)
{
    if( !lockedColumns_.contains(column) )
    {
        lockedColumns_.append(column);
    }
}

//-----------------------------------------------------------------------------
// Function: openColumn()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::openColumn(int const column)
{
    lockedColumns_.removeAll(column);
}

//-----------------------------------------------------------------------------
// Function: columnLocked()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::columnLocked(int column) const
{
    return lockedColumns_.contains(column);
}

//-----------------------------------------------------------------------------
// Function: nameDuplicates()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::nameDuplicates(int const row) const
{
    if ( row < rows_.size() ){

        QSharedPointer<PortGenerationRow> comparable = rows_.at(row);

        foreach ( QSharedPointer<PortGenerationRow> portRow, rows_ )
        {
            if( comparable == portRow )
            {
                continue;
            }

            else if( comparable->getDraftName() == portRow->getDraftName() )
            {
                return true;
            }

            // Check also draft component for identical port names.
            else if ( draftComponent_->getPort(comparable->getDraftName()) != 0 )
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: generateName()
//-----------------------------------------------------------------------------
QString PortGenerationTableModel::generateName(QString name, 
    General::Direction opposingDirection, General::Direction draftDirection, QString delimiter)
{
    if ( name.isEmpty() )
    {
        return QString("unnamed");
    }

    if ( draftDirection == General::DIRECTION_INVALID )
    {
        return name;
    }

    QString ending = delimiter+General::direction2Str(opposingDirection); 
    QString replacement = delimiter+General::direction2Str(draftDirection); 
    QString nameCandidate = name + replacement;
    if ( name.endsWith(ending) )
    {
        nameCandidate = name.left(name.lastIndexOf(ending))+replacement;
    }

    QString uniqueName = nameCandidate;
    int count = 0;
    while( draftComponent_->getPort(uniqueName) != 0 )
    {
        count++;
        uniqueName = nameCandidate + "_" + QString::number(count);
    }

    return uniqueName;
}