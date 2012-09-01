//-----------------------------------------------------------------------------
// File: AddressModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// Data model for the address editor.
//-----------------------------------------------------------------------------

#include "AddressModel.h"

#include "AddressDelegate.h"

#include <common/utils.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designwidget/BusPortItem.h>

#include <models/businterface.h>
#include <models/slaveinterface.h>
#include <models/memorymap.h>

//-----------------------------------------------------------------------------
// Function: AddressModel::AddressModel()
//-----------------------------------------------------------------------------
AddressModel::AddressModel(QObject *parent)
    : QAbstractTableModel(parent),
      component_(0),
      editProvider_(0)
{

}

//-----------------------------------------------------------------------------
// Function: AddressModel::~AddressModel()
//-----------------------------------------------------------------------------
AddressModel::~AddressModel()
{
}

//-----------------------------------------------------------------------------
// Function: AddressModel::setDataSource()
//-----------------------------------------------------------------------------
void AddressModel::setComponent(ComponentItem* component)
{
    beginResetModel();

    component_ = component;
    addressEntries_.clear();

    if (component_ != 0)
    {
        foreach (ConnectionEndpoint* endpoint, component_->getEndpoints())
        {
            BusPortItem* port = dynamic_cast<BusPortItem*>(endpoint);

            if (port != 0 && port->getBusInterface() != 0 &&
                port->getBusInterface()->getInterfaceMode() == General::MIRROREDSLAVE)
            {
                AddressEntry entry(component_, port);

                if (port->isConnected())
                {
                    // Determine the connected port.
                    GraphicsConnection const* conn = port->getConnections().first();

                    if (conn->endpoint1() != port)
                    {
                        entry.connectedPort = conn->endpoint1();
                    }
                    else
                    {
                        entry.connectedPort = conn->endpoint2();
                    }

                    // Retrieve the size of the memory map.
                    ComponentItem* connectedComp = entry.connectedPort->encompassingComp();

                    if (connectedComp != 0)
                    {
                        QString mapName = entry.connectedPort->getBusInterface()->getSlave()->getMemoryMapRef();
                        MemoryMap const* map = connectedComp->componentModel()->getMemoryMap(mapName);

                        entry.baseEndAddress = map->getLastAddress();
                        entry.range = entry.baseEndAddress * map->getAddressUnitBits() / 8;
                    }
                }


                addressEntries_.append(entry);
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::rowCount()
//-----------------------------------------------------------------------------
int AddressModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return addressEntries_.size();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::columnCount()
//-----------------------------------------------------------------------------
int AddressModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return ADDRESS_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressModel::data()
//-----------------------------------------------------------------------------
QVariant AddressModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= addressEntries_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        AddressEntry const& entry = addressEntries_.at(index.row());

        switch (index.column())
        {
        case ADDRESS_COL_INTERFACE_NAME:
            {
                return entry.component->name() + "." + entry.mirSlavePort->name();
            }

        case ADDRESS_COL_MAP_NAME:
            {
                if (entry.connectedPort == 0)
                {
                    return tr("undefined");
                }

                return entry.connectedPort->encompassingComp()->name() + "." +
                       entry.connectedPort->getBusInterface()->getSlave()->getMemoryMapRef();
            }

        case ADDRESS_COL_RANGE:
            {
                if (entry.connectedPort == 0)
                {
                    return tr("unspecified");
                }

                return entry.range;
            }

        case ADDRESS_COL_END_ADDRESS:
            {
                if (entry.connectedPort == 0)
                {
                    return tr("unspecified");
                }

                // Retrieve the start address from the component parameters.
                unsigned int startAddress = 
                    entry.component->getConfigurableElements().value(entry.mirSlavePort->name() + "_start_addr", "0").toUInt();

                return QString("0x") +
                       QString("%1").arg(startAddress + entry.baseEndAddress, 8, 16, QChar('0')).toUpper();
            }

        case ADDRESS_COL_START_ADDRESS:
            {
                unsigned int startAddress = 
                    entry.component->getConfigurableElements().value(entry.mirSlavePort->name() + "_start_addr", "0").toUInt();
                return QString("0x") + QString("%1").arg(startAddress, 8, 16, QChar('0')).toUpper();
            }

        case ADDRESS_COL_LOCKED:
            {
                return QVariant();
            }

        default:
            {
                return QVariant();
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == ADDRESS_COL_LOCKED)
        {
            AddressEntry const& entry = addressEntries_.at(index.row());

            QMap<QString, QString> const& elements = entry.component->getConfigurableElements();
            bool locked = elements.value(entry.mirSlavePort->name() + "_addr_locked", "false") == "true";

            if (locked)
            {
                return QIcon(":icons/graphics/lock-on.png");
            }
            else
            {
                return QIcon(":icons/graphics/lock-off.png");
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        if (index.column() == ADDRESS_COL_LOCKED)
        {
            return QSize(16, 16);
        }
    }
    else if (role == Qt::UserRole)
    {
        if (index.column() == ADDRESS_COL_LOCKED)
        {
            AddressEntry const& entry = addressEntries_.at(index.row());
            QMap<QString, QString> const& elements = entry.component->getConfigurableElements();
            return elements.value(entry.mirSlavePort->name() + "_addr_locked", "false") == "true";
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::headerData()
//-----------------------------------------------------------------------------
QVariant AddressModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case ADDRESS_COL_INTERFACE_NAME:
                {
                    return tr("Bus interface");
                }

            case ADDRESS_COL_MAP_NAME:
                {
                    return tr("Memory map");
                }
                
            case ADDRESS_COL_START_ADDRESS:
                {
                    return tr("Start address");
                }

            case ADDRESS_COL_LOCKED:
                {
                    return tr("Locked");
                }

            case ADDRESS_COL_RANGE:
                {
                    return tr("Range");
                }

            case ADDRESS_COL_END_ADDRESS:
                {
                    return tr("End address");
                }

            default:
                {
                    return QVariant();
                }
            }
        } 
        // Vertical headers get running numbers.
        else
        {
            return section + 1;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::setData()
//-----------------------------------------------------------------------------
bool AddressModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= addressEntries_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case ADDRESS_COL_START_ADDRESS:
            {
                AddressEntry const& entry = addressEntries_.at(index.row());

                QMap<QString, QString> elements = entry.component->getConfigurableElements();
                elements.insert(entry.mirSlavePort->name() + "_start_addr", QString::number(Utils::str2Int(value.toString())));
                entry.component->setConfigurableElements(elements);
                
                QModelIndex endAddressIndex(this->index(index.row(), index.column() + 2));
                emit dataChanged(endAddressIndex, endAddressIndex);
                break;
            }

        default:
            {
                return false;
            }
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else if (role == Qt::UserRole)
    {
        if (index.column() == ADDRESS_COL_LOCKED)
        {
            AddressEntry const& entry = addressEntries_.at(index.row());

            QMap<QString, QString> elements = entry.component->getConfigurableElements();
            elements.insert(entry.mirSlavePort->name() + "_addr_locked", value.toString().toLower());
            entry.component->setConfigurableElements(elements);

            QModelIndex startAddressIndex(this->index(index.row(), index.column() - 1));
            emit dataChanged(startAddressIndex, index);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AddressModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    switch (index.column())
    {
    case ADDRESS_COL_START_ADDRESS:
        {
            AddressEntry const& entry = addressEntries_.at(index.row());
            QMap<QString, QString> const& elements = entry.component->getConfigurableElements();
            bool locked = elements.value(entry.mirSlavePort->name() + "_addr_locked", "false") == "true";

            if (locked)
            {
                return Qt::ItemIsEnabled;
            }
            else
            {
                return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
            }
        }
    
    case ADDRESS_COL_LOCKED:
        return Qt::ItemIsEnabled;

    default:
        return Qt::ItemIsEnabled;
    }
}

//-----------------------------------------------------------------------------
// Function: AddressModel::clear()
//-----------------------------------------------------------------------------
void AddressModel::clear()
{
    setComponent(0);
}
