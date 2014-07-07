//-----------------------------------------------------------------------------
// File: PortMapsTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.10.2013
//
// Description:
// A model for displaying port maps in component.
//-----------------------------------------------------------------------------

#include "PortMapsTreeModel.h"

#include "PortMapsTreeItem.h"
#include "PortMapsLogicalItem.h"
#include "PortMapsBitMapItem.h"

#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QSize>


//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::PortMapsTreeModel()
//-----------------------------------------------------------------------------
PortMapsTreeModel::PortMapsTreeModel(BusInterface* busif,
    QSharedPointer<Component> component,
    LibraryInterface* handler,
    QObject *parent)
    : QAbstractItemModel(parent),    
    component_(component),
    handler_(handler),
    root_(new PortMapsTreeItem()),
    busif_(busif),    
    absDef_(),    
    interfaceMode_(General::MASTER),
    portMaps_(busif->getPortMaps())
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::~PortMapsTreeModel()
//-----------------------------------------------------------------------------
PortMapsTreeModel::~PortMapsTreeModel()
{
    delete root_;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortMapsTreeModel::headerData(int section, Qt::Orientation /*orientation*/,
                                         int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case COLUMN_TREE:
            {
                return tr("Logical Port");
            }
        case COLUMN_PHYSICAL:
            {
                return tr("Physical Port(s)");
            }
        case  COLUMN_WIDTH_LOGICAL:
            {
                return tr("Logical Width");
            }
        case  COLUMN_WIDTH_PHYSICAL:
            {
                return tr("Physical Width");
            }
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::columnCount()
//-----------------------------------------------------------------------------
int PortMapsTreeModel::columnCount(const QModelIndex& /*parent = QModelIndex()*/) const
{
    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::rowCount()
//-----------------------------------------------------------------------------
int PortMapsTreeModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return static_cast<PortMapsTreeItem*>(parent.internalPointer())->getChildCount();
    }
    else
    {
        return root_->getChildCount();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortMapsTreeModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    PortMapsTreeItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<PortMapsTreeItem*>(parent.internalPointer());
    }

    PortMapsTreeItem* child = parentItem->getChild(row);
    
    if (child == 0)
    {
        return QModelIndex();
    }

    return createIndex(row, column, child);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::parent()
//-----------------------------------------------------------------------------
QModelIndex PortMapsTreeModel::parent(QModelIndex const& child) const
{
    if (!child.isValid() || child.column() > COLUMN_COUNT)
    {
        return QModelIndex();
    }

    PortMapsTreeItem* childItem = static_cast<PortMapsTreeItem*>(child.internalPointer());
    PortMapsTreeItem* parent = childItem->getParent();

    if (parent == 0 || parent == root_)
    {
        return QModelIndex();
    }

    int index = parent->getIndex();

    if (index < 0)
    {
        return QModelIndex();
    }

    return createIndex(index, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::setData()
//-----------------------------------------------------------------------------
bool PortMapsTreeModel::setData(QModelIndex const&, QVariant const&, int  /* = Qt::EditRole */)
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::data()
//-----------------------------------------------------------------------------
QVariant PortMapsTreeModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    PortMapsTreeItem* item = static_cast<PortMapsTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        return item->data(index.column());  
    }          

    else if (role == Qt::DecorationRole)
    {        
        if (index.column() == COLUMN_TREE && item->getType() == PortMapsTreeItem::ITEM_LOGICAL_PORT)
        {
            if (!absDef_)
            {
                return QIcon(":icons/common/graphics/cross.png");
            }

            General::Direction direction = absDef_->getPortDirection(item->getName(), interfaceMode_);      
            switch( direction )
            {
            case General::IN :
                return QIcon(":icons/common/graphics/control-180.png");

            case General::OUT :
                return QIcon(":icons/common/graphics/control.png");

            case General::INOUT :
                return QIcon(":icons/common/graphics/control-dual.png");

            default:
                return QIcon(":icons/common/graphics/cross.png");
            }
        }
    }    
    else if (role == Qt::ForegroundRole)
    {
        // Unmapped bit items are shown in gray.
        if (index.column() == COLUMN_PHYSICAL && item->getType() == PortMapsTreeItem::ITEM_BIT_MAP && 
            !dynamic_cast<PortMapsBitMapItem*>(item)->isConnected())
        {
            return QColor(Qt::gray);
        }

        // All invalid items are marked with red.
        if (!item->isValid())
        {
            return QColor(Qt::red);
        }
        else
        {
            return QColor(Qt::black);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item->getType() == PortMapsTreeItem::ITEM_LOGICAL_PORT)
        {
            return QColor(230, 230, 230);
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch (index.column())
        {

        case COLUMN_WIDTH_LOGICAL:
        case COLUMN_WIDTH_PHYSICAL:
            {
                return Qt::AlignRight + Qt::AlignVCenter;
            }
        default:
            {
                return Qt::AlignLeft + Qt::AlignVCenter;
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(100, 16);
    }   
    else if (role == Qt::ToolTipRole)
    {
        // Show all connected bits in tooltip.
        if (index.column() == COLUMN_PHYSICAL && item->getType() == PortMapsTreeItem::ITEM_BIT_MAP && 
            dynamic_cast<PortMapsBitMapItem*>(item)->isConnected())
        {
            QString portNames = "<p>";
            portNames.append(item->data(COLUMN_PHYSICAL).toString());
            portNames.append("</p>");           
            return portNames;
        }
    }

    // Unsupported role.
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::hasChildren()
//-----------------------------------------------------------------------------
bool PortMapsTreeModel::hasChildren(const QModelIndex& parent /*= QModelIndex()*/) const
{
    PortMapsTreeItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<PortMapsTreeItem*>(parent.internalPointer());
    }

    return (parentItem->getChildCount() > 0);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortMapsTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::reset()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::reset()
{
    beginResetModel();

    delete root_;
    root_ = new PortMapsTreeItem();

    // Add existing mappings.
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {

        PortMapsLogicalItem* logicalItem = dynamic_cast<PortMapsLogicalItem*>(findItem(portMap->logicalPort_));

        if (!logicalItem)
        {            
            logicalItem = new PortMapsLogicalItem(root_, portMap->logicalPort_, component_, busif_, absDef_);
            root_->addChild(logicalItem);
        }

        logicalItem->refresh();       
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::createMap()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::createMap(QSharedPointer<General::PortMap> portMap)
{
    if (!portMaps_.contains(portMap))
    {
        portMaps_.append(portMap);
        emit contentChanged();
    }
    
    PortMapsLogicalItem* logicalItem = dynamic_cast<PortMapsLogicalItem*>(findItem(portMap->logicalPort_));

    if (!logicalItem)
    {
        beginInsertRows(QModelIndex(), root_->getChildCount(), root_->getChildCount());
        logicalItem = new PortMapsLogicalItem(root_, portMap->logicalPort_, component_, busif_, absDef_);
        root_->addChild(logicalItem);
        endInsertRows();
    }

    beginInsertRows(getItemIndex(logicalItem, 0), 0, logicalItem->getWidth());
    logicalItem->refresh();
    endInsertRows();    
}


//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::logicalPorts()
//-----------------------------------------------------------------------------
QStringList PortMapsTreeModel::logicalPorts() const
{
    QStringList list;
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
        list.append(portMap->logicalPort_);
    }
    return list;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::setAbsType()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::setAbsType(const VLNV& vlnv, General::InterfaceMode mode)
{
    interfaceMode_ = mode;

    if (!vlnv.isValid()) {
        absDef_ = QSharedPointer<AbstractionDefinition>();
        return;
    }

    // ask library to parse the model for abstraction definition
    QSharedPointer<LibraryComponent> libComb;
    if (handler_->contains(vlnv)) 
    {
        libComb = handler_->getModel(vlnv);
    }
    // if library did not contain the vlnv
    else
    {
        return;
    }
    // make sure the model is for abstraction definition
    if (handler_->getDocumentType(vlnv) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef_ = libComb.staticCast<AbstractionDefinition>();     
    }

    // Add existing mappings.
    reset();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::refresh()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::refresh()
{
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        PortMapsLogicalItem* logicalItem = dynamic_cast<PortMapsLogicalItem*>(root_->getChild(i));
        Q_ASSERT(logicalItem);
        logicalItem->refresh();
    }

    emit dataChanged(getItemIndex(root_->getChild(0), 0),
                     getItemIndex(root_->getChild(root_->getChildCount() - 1), 0));
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::findItem()
//-----------------------------------------------------------------------------
PortMapsTreeItem* PortMapsTreeModel::findItem(QString const& name)
{
    PortMapsTreeItem* item = 0;
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        item = root_->getChild(i);
        if (item->getName() == name)
        {
            return item;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::getItemIndex()
//-----------------------------------------------------------------------------
QModelIndex PortMapsTreeModel::getItemIndex(PortMapsTreeItem* item, int column) const
{
    if (item == 0)
    {
        return QModelIndex();
    }

    PortMapsTreeItem* parent = item->getParent();

    if (parent == 0)
    {
        return QModelIndex();
    }

    return createIndex(item->getIndex(), column, item);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::isValid()
//-----------------------------------------------------------------------------
bool PortMapsTreeModel::isValid() const
{
    return root_->isValid();
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::isValid()
//-----------------------------------------------------------------------------
bool PortMapsTreeModel::isValid(QStringList& errorList) const
{
    return root_->isValid(errorList);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::canCreateMap()
//-----------------------------------------------------------------------------
bool PortMapsTreeModel::canCreateMap( const QString& physicalPort, const QString& logicalPort ) const {
    // if port maps has been defined then check that signal directions match
    if (absDef_) 
    {
        General::Direction logicalDirection = 
            absDef_->getPortDirection(logicalPort, interfaceMode_);

        General::Direction physDirection = component_->getPortDirection(physicalPort);

        // if directions don't match
        if (logicalDirection != physDirection && 
            physDirection != General::INOUT && logicalDirection != General::INOUT) 
        {
                emit errorMessage(tr("Directions between logical port \"%1\" and"
                    " physical port \"%2\" did not match.").arg(logicalPort).arg(physicalPort));
                return false;
        }
        else
        {
            return true;
        }
    }
    // if abstraction definition is not used ports can always be mapped
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::restoreItem()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::restoreItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    // Only logical items can be removed.
    if (index.parent().isValid())
    {
        return;
    }

    PortMapsTreeItem* item = root_->getChild(index.row());
    QString logicalName = item->getName();
        
    emit logicalRestored(logicalName);
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::removeItem()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::removeItem(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    // Only logical items can be removed.
    if (index.parent().isValid())
    {
        return;
    }

    PortMapsTreeItem* item = root_->getChild(index.row());
    QString logicalName = item->getName();
    if (item->getType() == PortMapsTreeItem::ITEM_LOGICAL_PORT)
    {
        beginRemoveRows(getItemIndex(root_,0), item->getIndex(), item->getIndex());
        root_->removeChild(item);
        delete item;
        endRemoveRows();
        
        emit logicalRemoved(logicalName);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsTreeModel::removeMapping()
//-----------------------------------------------------------------------------
void PortMapsTreeModel::removeMapping(QModelIndex const& index)
{
    if (!index.isValid())
    {
        return;
    }

    // Only logical items can be removed.
    if (index.parent().isValid())
    {
        return;
    }

    PortMapsTreeItem* item = root_->getChild(index.row());
    QString logicalName = item->getName();
    QStringList physPorts;
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort_ == logicalName)
        {
            if (!physPorts.contains(portMap->physicalPort_))
            {
                physPorts.append(portMap->physicalPort_);
            }
            portMaps_.removeAll(portMap);
        }
    }    

    // Search for disconnected physical ports.    
    foreach (QString physicalName, physPorts)
    {
        bool connected = false;
        foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
        {
            if (portMap->physicalPort_ == physicalName)
            {
                connected = true;
                break;
            }
        }

        if (!connected)
        {
            emit physicalRemoved(physicalName);
        }
    }

    removeItem(index);
}