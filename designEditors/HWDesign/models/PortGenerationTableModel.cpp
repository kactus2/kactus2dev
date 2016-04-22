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

#include <QStringList>
#include <QColor>

#include <designEditors/HWDesign/models/PortGenerationRow.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <library/LibraryManager/libraryinterface.h>

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
// Function: PortGenerationTableModel::PortGenerationTableModel()
//-----------------------------------------------------------------------------
PortGenerationTableModel::PortGenerationTableModel(QObject* parent):
QAbstractTableModel(parent), 
    draftComponent_(0), 
    lockedColumns_(),
    rows_(),
    headers_()
{
    for(int i = 0; i < PortGenerationRow::COLUMNS; i++)
    {
        headers_.append(HEADER_DEFAULT_NAMES[i]);
    }
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::~PortGenerationTableModel()
//-----------------------------------------------------------------------------
PortGenerationTableModel::~PortGenerationTableModel()
{
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::rowCount()
//-----------------------------------------------------------------------------
int PortGenerationTableModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::columnCount()
//-----------------------------------------------------------------------------
int PortGenerationTableModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return PortGenerationRow::COLUMNS;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::data()
//-----------------------------------------------------------------------------
QVariant PortGenerationTableModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() >= rows_.size() || index.column() >= PortGenerationRow::COLUMNS)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole) 
    {
        QSharedPointer<PortGenerationRow> row = rows_.at(index.row());

        if (index.column() == SRC_DESCRIPTION ) 
        {
            return row->getSourceDescription();
        }   
        else if (index.column() == SRC_DIRECTION)
        {
            return DirectionTypes::direction2Str(row->getSourceDirection());
        }
        else if (index.column() == SRC_NAME)
        {
            return row->getSourceName();
        }
        else if (index.column() == SIZE)
        {
            return row->getSize();
        }
        else if (index.column() == TARGET_NAME)
        {
            return row->getDraftName();
        }
        else if (index.column() == TARGET_DIRECTION)
        {
            DirectionTypes::Direction dir = row->getDraftDirection();
            if (dir == DirectionTypes::DIRECTION_INVALID)
            {
                return QString(tr("undefined"));
            }

            return DirectionTypes::direction2Str(dir);
        }
        else if (index.column() == TARGET_DESCRIPTION)
        {
            return row->getDraftDescription();
        }
        else
        {
            return QVariant();
        }
    }

    else if ( role == Qt::ForegroundRole )
    {
        
        if (index.column() == TARGET_NAME && nameDuplicates(index.row()))
        {
            return QColor("red");
        } 
        else if (index.column() == TARGET_DIRECTION && 
            rows_.at(index.row())->getDraftDirection() == DirectionTypes::DIRECTION_INVALID)
        {
            return QColor("red");
        }
        else if (columnLocked(index.column()))
        {
            return QColor("gray");
        }

        return QColor("black");
    }

    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == TARGET_NAME)
        {
            return QColor("LemonChiffon");
        }

        return QColor("white");
    }

    else if (role == Qt::TextAlignmentRole)
    {
        if ( index.column() == SRC_DIRECTION || index.column() == SIZE || index.column() == TARGET_DIRECTION )
        {
            return Qt::AlignCenter;
        }
        else
        {
            return Qt::AlignLeft;
        }
    }

    else if (role == Qt::EditRole && !columnLocked(index.column()))
    {
        return data(index, Qt::DisplayRole);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == TARGET_NAME && nameDuplicates(index.row()))
        {
                return QString(tr("Name %1 already in use.").arg(data(index).toString()));
        }
        else if (index.column() == TARGET_DIRECTION &&
            rows_.at(index.row())->getDraftDirection() == DirectionTypes::DIRECTION_INVALID)
        {
            return QString(tr("Direction not defined in bus abstraction definition."));
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortGenerationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole && section < headers_.size()) 
    {
        return headers_.at(section);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: setData()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rows_.size() || columnLocked(index.column()))
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<PortGenerationRow> row = rows_.at(index.row());
        if (index.column() == TARGET_NAME)
        {
            row->setDraftName(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == TARGET_DIRECTION)
        {
            row->setDraftDirection(DirectionTypes::str2Direction(value.toString(), 
                DirectionTypes::DIRECTION_INVALID));
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == TARGET_DESCRIPTION)
        {
            row->setDraftDescription(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else
        {
            return false;
        }        
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::setHeaderData()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::setHeaderData( int section, Qt::Orientation orientation, QVariant const& value, 
    int role)
{
    if (orientation != Qt::Horizontal || !value.isValid())
    {
        return false;
    }

    if (role == Qt::EditRole && section < headers_.size())
    {
        headers_.replace(section, value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortGenerationTableModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (columnLocked( index.column()))
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::isValid()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::isValid() const
{
    for ( int i = 0; i < rows_.size(); i++ ) 
    {
        if (!rows_.at(i)->isValid() || nameDuplicates(i))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::initialize()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::initialize(QSharedPointer<Component> srcComponent, 
    QSharedPointer<BusInterface> busIf, 
    QSharedPointer<Component> targetComponent, 
    LibraryInterface* lh, 
    General::InterfaceMode selectedMode)
{
    draftComponent_ = targetComponent;

    // Get the abstract definition of the bus interface.
    QSharedPointer<Document> libComp = lh->getModel(*busIf->getAbstractionTypes()->first()->getAbstractionRef());
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

    beginResetModel();    
    
    bool createNew = rows_.empty();
    QSharedPointer<PortGenerationRow> row = QSharedPointer<PortGenerationRow>();
    int count = 0;

    // Create table rows based on ports in opposing interface. 
    foreach (QSharedPointer<PortMap> portMap, *busIf->getPortMaps())
    {
        QString portName = portMap->getPhysicalPort()->name_;

        QSharedPointer<Port> port = srcComponent->getPort(portName);
        if (port)
        {
            if (createNew)
            {
                int portSize = abs(port->getLeftBound().toInt() - port->getRightBound().toInt()) + 1;
                row = QSharedPointer<PortGenerationRow>(new PortGenerationRow(portName, port->getDirection(),
                    port->description(), portSize));
                rows_.append(row);
            }
            else
            {
                row = rows_.at(count);
            }

            DirectionTypes::Direction draftDir = absDef->getPortDirection(portMap->getLogicalPort()->name_,
                selectedMode);
            row->setDraftDirection(draftDir);
            row->setDraftName(generateName(portName, port->getDirection(), draftDir));

            count++;
        }
    }

    endResetModel();

    lockColumn(SRC_DESCRIPTION);
    lockColumn(SRC_DIRECTION);
    lockColumn(SRC_NAME);
    lockColumn(SIZE);
    lockColumn(TARGET_DIRECTION);
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::lockColumn()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::lockColumn(int const column)
{
    if (!lockedColumns_.contains(column))
    {
        lockedColumns_.append(column);
    }
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::openColumn()
//-----------------------------------------------------------------------------
void PortGenerationTableModel::openColumn(int const column)
{
    lockedColumns_.removeAll(column);
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::columnLocked()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::columnLocked(int column) const
{
    return lockedColumns_.contains(column);
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::nameDuplicates()
//-----------------------------------------------------------------------------
bool PortGenerationTableModel::nameDuplicates(int const row) const
{
    if (row < rows_.size())
    {
        QSharedPointer<PortGenerationRow> comparable = rows_.at(row);

        foreach (QSharedPointer<PortGenerationRow> portRow, rows_)
        {
            // Check draft component for identical port names.
            if (draftComponent_->getPort(comparable->getDraftName()) != 0)
            {
                return true;
            }

            else if( comparable == portRow )
            {
                continue;
            }

            else if( comparable->getDraftName() == portRow->getDraftName() )
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortGenerationTableModel::generateName()
//-----------------------------------------------------------------------------
QString PortGenerationTableModel::generateName(QString const& name, 
    DirectionTypes::Direction opposingDirection, 
    DirectionTypes::Direction draftDirection, 
    QString const& delimiter)
{
    if (name.isEmpty())
    {
        return QString("unnamed");
    }

    if (draftDirection == DirectionTypes::DIRECTION_INVALID)
    {
        return name;
    }

    QString ending = delimiter + DirectionTypes::direction2Str(opposingDirection); 
    QString replacement = delimiter + DirectionTypes::direction2Str(draftDirection); 
    QString nameCandidate = name + replacement;
    if (name.endsWith(ending))
    {
        nameCandidate = name.left(name.lastIndexOf(ending))+replacement;
    }

    QString uniqueName = nameCandidate;
    int count = 0;
    while (draftComponent_->getPort(uniqueName) != 0)
    {
        count++;
        uniqueName = nameCandidate + "_" + QString::number(count);
    }

    return uniqueName;
}
