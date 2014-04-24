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

#include <designEditors/HWDesign/BusPortItem.h>

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/memorymap.h>

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

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
    if (component_)
    {
        component_->disconnect(this);
    }

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
                addressEntries_.append(QSharedPointer<AddressEntry>(new AddressEntry(component_, port)));
            }
        }

        connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
                this, SLOT(refresh()), Qt::UniqueConnection);

        connect(component_, SIGNAL(destroyed(ComponentItem*)),
                this, SLOT(clear()), Qt::UniqueConnection);
    }

    // Sort the entries by start address.
    qSort(addressEntries_.begin(), addressEntries_.end(), &addressEntrySortOp);

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
        QSharedPointer<AddressEntry> entry = addressEntries_.at(index.row());

        switch (index.column())
        {
        case ADDRESS_COL_INTERFACE_NAME:
            {
                return entry->getInterfaceName();
            }

        case ADDRESS_COL_MAP_NAME:
            {
                return entry->getMemoryMapName();
            }

        case ADDRESS_COL_MAP_AUB:
            {
                return entry->getMemoryMapAUB();
            }

        case ADDRESS_COL_RANGE_BYTES:
            {
                return entry->getRangeInBytes();
            }

        case ADDRESS_COL_RANGE_AUB:
            {
                return entry->getRange();
            }

        case ADDRESS_COL_END_ADDRESS:
            {
                if (!entry->hasValidConnection())
                {
                    return tr("unspecified");
                }

                return QString("0x") + QString("%1").arg(entry->getEndAddress(), 8, 16, QChar('0')).toUpper();
            }

        case ADDRESS_COL_START_ADDRESS:
            {
                return QString("0x") + QString("%1").arg(entry->getStartAddress(), 8, 16, QChar('0')).toUpper();
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
            QSharedPointer<AddressEntry> entry = addressEntries_.at(index.row());

            if (entry->isLocked())
            {
                return QIcon(":icons/graphics/lock-on.png");
            }
            else
            {
                return QIcon(":icons/graphics/lock-off.png");
            }
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        switch (index.column())
        {
        case ADDRESS_COL_START_ADDRESS:
        case ADDRESS_COL_END_ADDRESS:
            {
                if (!checkRangeOverlaps(index.row()))
                {
                    return QColor(Qt::red);
                }

                return QVariant();
            }

        default:
            return QVariant();
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
            return addressEntries_.at(index.row())->isLocked();
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
                
            case ADDRESS_COL_MAP_AUB:
                {
                    return tr("AUB");
                }
                
            case ADDRESS_COL_RANGE_BYTES:
                {
                    return tr("Range (bytes)");
                }

            case ADDRESS_COL_RANGE_AUB:
                {
                    return tr("Range (AUB)");
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
                QSharedPointer<AddressEntry> entry = addressEntries_[index.row()];

                disconnect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
                           this, SLOT(refresh()));

                entry->setStartAddress(Utils::str2Uint(value.toString()));

                connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
                    this, SLOT(refresh()), Qt::UniqueConnection);

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
        return true;
    }
    else if (role == Qt::UserRole)
    {
        if (index.column() == ADDRESS_COL_LOCKED)
        {
            QSharedPointer<AddressEntry> entry = addressEntries_[index.row()];

            disconnect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
                       this, SLOT(refresh()));

            entry->setLocked(value.toBool());

            connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
                    this, SLOT(refresh()), Qt::UniqueConnection);

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
            QSharedPointer<AddressEntry> entry = addressEntries_[index.row()];
            
            if (entry->isLocked())
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

//-----------------------------------------------------------------------------
// Function: AddressModel::isEntryValid()
//-----------------------------------------------------------------------------
bool AddressModel::checkRangeOverlaps(int index) const
{
    for (int i = 0; i < addressEntries_.size(); ++i)
    {
        if (i == index)
        {
            continue;
        }

        // Check if the address boundaries overlap.
        if (addressEntries_.at(index)->overlaps(*addressEntries_.at(i)))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressModel::autoAssignAddresses()
//-----------------------------------------------------------------------------
void AddressModel::autoAssignAddresses()
{
    beginResetModel();

    if (!addressEntries_.empty())
    {
        addressEntries_[0]->setStartAddress(addressEntries_[0]->getStartAddress());

        for (int i = 0; i < addressEntries_.size() - 1; ++i)
        {
            if (!addressEntries_[i + 1]->isLocked())
            {
                addressEntries_[i + 1]->setStartAddress(addressEntries_[i]->getEndAddress() + 1);
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::refresh()
//-----------------------------------------------------------------------------
void AddressModel::refresh()
{
    emit dataChanged(index(0, ADDRESS_COL_INTERFACE_NAME), index(addressEntries_.size(), ADDRESS_COL_END_ADDRESS));
}

//-----------------------------------------------------------------------------
// Function: AddressModel::importCSV()
//-----------------------------------------------------------------------------
void AddressModel::importCSV(QString const& filename)
{
    if (!filename.isEmpty())
    {
        QFileInfo info(filename);

        if (!info.exists() || !info.isFile())
        {
            return;
        }

        QFile file(filename);

        // Try to open the file for reading.
        if (!file.open(QFile::Truncate | QFile::ReadOnly))
        {
            //emit errorMessage(tr("Could not open file %1 for reading.").arg(filename));
            return;
        }

        QTextStream stream(&file);

        // Read the headers from the stream and make sure the file is of correct format.
        QString header = stream.readLine(100);
        QStringList headers = header.split(";", QString::KeepEmptyParts);
        if (headers.size() != ADDRESS_COL_COUNT)
        {
            //emit errorMessage(tr("File %1 was not appropriate format to import ports.").arg(filename));
            file.close();
            return;
        }

        beginResetModel();

        while (!stream.atEnd())
        {
            QString line = stream.readLine(0);
            QStringList settings = line.split(";", QString::KeepEmptyParts);

            // If not all settings are found then skip the entry.
            if (settings.size() != ADDRESS_COL_COUNT)
            {
                //emit noticeMessage(tr("Found invalid port within csv-file import, skipping..."));
                continue;
            }

            // Find the address entry with the corresponding name.
            for (int i = 0; i < addressEntries_.size(); ++i)
            {
                QSharedPointer<AddressEntry> entry = addressEntries_[i];

                if (entry->getInterfaceName() == settings.at(0))
                {
                    entry->setStartAddress(settings.at(2).toUInt());
                    entry->setLocked(settings.at(3) == "true");
                }
            }
        }
        endResetModel();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressModel::exportCSV()
//-----------------------------------------------------------------------------
void AddressModel::exportCSV(QString const& filename)
{
    if (!filename.isEmpty())
    {
        QFile file(filename);

        // Try to open the file for writing.
        if (!file.open(QFile::Truncate | QFile::WriteOnly))
        {
            //emit errorMessage(tr("Could not open file %1 for writing.").arg(filename));
            return;
        }

        QTextStream stream(&file);

        // Write the headers for the columns.
        stream << "Interface name;Memory map;Start address;Locked;Range;End address" << endl;

        // Write each entry
        foreach (QSharedPointer<AddressEntry> entry, addressEntries_)
        {
            stream << entry->getInterfaceName() << ";";
            stream << entry->getMemoryMapName() << ";";
            stream << entry->getStartAddress() << ";";
            stream << entry->isLocked() << ";";
            stream << entry->getRange() << ";";
            stream << entry->getEndAddress() << endl;
        }

        // close the file
        file.close();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressModel::addressEntrySortOp()
//-----------------------------------------------------------------------------
bool AddressModel::addressEntrySortOp(QSharedPointer<AddressEntry> lhs, QSharedPointer<AddressEntry> rhs)
{
    return lhs->getStartAddress() < rhs->getStartAddress();
}

//-----------------------------------------------------------------------------
// Function: AddressModel::onMoveItem()
//-----------------------------------------------------------------------------
void AddressModel::onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos)
{
    // If there was no item in the starting point.
    if (!originalPos.isValid())
    {
        return;
    }
    // If the indexes are the same.
    else if (originalPos == newPos)
    {
        return;
    }
    else if (originalPos.row() < 0 || originalPos.row() >= addressEntries_.size())
    {
        return;
    }

    // if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= addressEntries_.size())
    {
        beginResetModel();

        QSharedPointer<AddressEntry> entry = addressEntries_.at(originalPos.row());
        addressEntries_.removeAt(originalPos.row());
        addressEntries_.append(entry);

        endResetModel();
    }
    // if both indexes were valid
    else
    {
        beginResetModel();

        QSharedPointer<AddressEntry> entry = addressEntries_.at(originalPos.row());
        addressEntries_.removeAt(originalPos.row());
        addressEntries_.insert(newPos.row(), entry);

        endResetModel();
    }
}