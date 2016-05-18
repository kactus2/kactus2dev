//-----------------------------------------------------------------------------
// File: InterfacePortMapModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.05.2016
//
// Description:
// Table model for visualizing interface port maps.
//-----------------------------------------------------------------------------

#include "InterfacePortMapModel.h"
#include "InterfacePortMapColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionEndpoint.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::InterfacePortMapModel()
//-----------------------------------------------------------------------------
InterfacePortMapModel::InterfacePortMapModel(LibraryInterface* library, QObject *parent):
QAbstractTableModel(parent),
libraryHandler_(library),
mappingItems_(),
componentFinder_(new ComponentParameterFinder(QSharedPointer<Component>())),
parser_(new IPXactSystemVerilogParser(componentFinder_)),
activeInterfaces_(),
endPoint_()
{

}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::~InterfacePortMapModel()
//-----------------------------------------------------------------------------
InterfacePortMapModel::~InterfacePortMapModel()
{

}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::setData()
//-----------------------------------------------------------------------------
void InterfacePortMapModel::setInterfaceData(ConnectionEndpoint* busItem,
    QList<QSharedPointer<ActiveInterface> > activeInterfaces)
{
    beginResetModel();

    endPoint_ = busItem;
    mappingItems_.clear();
    activeInterfaces_.clear();

    if (busItem->isBus() && !busItem->isHierarchical())
    {
        activeInterfaces_ = activeInterfaces;
    }

    if (busItem && busItem->isBus() && busItem->getOwnerComponent() && busItem->getBusInterface() &&
        busItem->getBusInterface()->getPortMaps())
    {
        QSharedPointer<BusInterface> busInterface = busItem->getBusInterface();
        QList<QSharedPointer<PortMap> > portMaps = *busInterface->getPortMaps();

        // get the abstraction def for the interface
        VLNV absDefVLNV;
        if (busInterface->getAbstractionTypes() && !busInterface->getAbstractionTypes()->isEmpty() &&
            busInterface->getAbstractionTypes()->first()->getAbstractionRef())
        {
            absDefVLNV = *busInterface->getAbstractionTypes()->first()->getAbstractionRef();
        }

        QSharedPointer<AbstractionDefinition> absDef;
        if (libraryHandler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION)
        {
            QSharedPointer<Document> libComp = libraryHandler_->getModel(absDefVLNV);
            absDef = libComp.staticCast<AbstractionDefinition>();
        }

        // get the component that contains the selected interface
        QSharedPointer<Component> component = busItem->getOwnerComponent();
        componentFinder_->setComponent(component);

        foreach (QSharedPointer<PortMap> portMap, portMaps)
        {
            MappingItem newItem;

            QString logicalPortName = portMap->getLogicalPort()->name_;

            newItem.logicalIsOk_ = absDef && absDef->hasPort(logicalPortName, busInterface->getInterfaceMode());

            int logicalSize = 1;
            // if the logical port is vectored
            if (portMap->getLogicalPort() && portMap->getLogicalPort()->range_)
            {
                QString logicalLeft = parser_->parseExpression(portMap->getLogicalPort()->range_->getLeft());
                QString logicalRight = parser_->parseExpression(portMap->getLogicalPort()->range_->getRight());

                logicalSize = abs(logicalLeft.toInt() - logicalRight.toInt()) + 1;

                logicalPortName += "[" + logicalLeft + ".." + logicalRight + "]";
            }

            // display at least the name of physical port
            QString physicalPortName = portMap->getPhysicalPort()->name_;

            newItem.physicalPortName_ = physicalPortName;

            newItem.physicalIsOk_ = (component && component->hasPort(physicalPortName));

            int physicalSize = 1;

            int physicalLeft = 0;
            int physicalRight = 0;
            if (portMap->getPhysicalPort())
            {
                QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();
                if (portMap->getPhysicalPort()->partSelect_)
                {
                    physicalLeft = parser_->parseExpression(physicalPort->partSelect_->getLeftRange()).toInt();
                    physicalRight = parser_->parseExpression(physicalPort->partSelect_->getRightRange()).toInt();

                    physicalSize = abs(physicalLeft - physicalRight) + 1;
                }
            }
            // if port map does not contain physical vector but port is found on the component
            else if (component->hasPort(physicalPortName))
            {
                QSharedPointer<Port> componentPort = component->getPort(physicalPortName);

                physicalLeft = parser_->parseExpression(componentPort->getLeftBound()).toInt();
                physicalRight = parser_->parseExpression(componentPort->getRightBound()).toInt();

                physicalSize = abs(physicalLeft - physicalRight) + 1;
            }

            physicalPortName += "[" + QString::number(physicalLeft) + ".." + QString::number(physicalRight) + "]";

            if (logicalSize != physicalSize)
            {
                newItem.logicalIsOk_ = false;
                newItem.physicalIsOk_ = false;
            }

            newItem.logicalPort_ = logicalPortName;
            newItem.physicalPort_ = physicalPortName;

            newItem.isExcluded_ = portIsExcluded(newItem.physicalPortName_);

            mappingItems_.append(newItem);
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::portIsExcluded()
//-----------------------------------------------------------------------------
bool InterfacePortMapModel::portIsExcluded(QString const& portName) const
{
    foreach (QSharedPointer<ActiveInterface> currentInterface, activeInterfaces_)
    {
        if (currentInterface->getExcludePorts()->contains(portName))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::rowCount()
//-----------------------------------------------------------------------------
int InterfacePortMapModel::rowCount(QModelIndex const& ) const
{
    return mappingItems_.size();
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::columnCount()
//-----------------------------------------------------------------------------
int InterfacePortMapModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return InterfacePortMapColumns::INTERFACE_COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::data()
//-----------------------------------------------------------------------------
QVariant InterfacePortMapModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mappingItems_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == InterfacePortMapColumns::INTERFACE_LOGICAL_NAME)
        {
            return mappingItems_.at(index.row()).logicalPort_;
        }
        else if (index.column() == InterfacePortMapColumns::INTERFACE_PHYSICAL_NAME)
        {
            return mappingItems_.at(index.row()).physicalPort_;
        }
    }
    else if (role == Qt::CheckStateRole && index.column() == InterfacePortMapColumns::INTERFACE_EXCLUDE)
    {
        if (mappingItems_.at(index.row()).isExcluded_)
        {
            return Qt::Checked;
        }
        else
        {
            return Qt::Unchecked;
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == InterfacePortMapColumns::INTERFACE_LOGICAL_NAME &&
            !mappingItems_.at(index.row()).logicalIsOk_) ||
            (index.column() == InterfacePortMapColumns::INTERFACE_PHYSICAL_NAME &&
            !mappingItems_.at(index.row()).physicalIsOk_))
        {
            return QColor("red");
        }

        return QColor("black");
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == InterfacePortMapColumns::INTERFACE_EXCLUDE &&
            (endPoint_->isHierarchical() || activeInterfaces_.isEmpty()))
        {
            return QColor("whiteSmoke");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == InterfacePortMapColumns::INTERFACE_LOGICAL_NAME)
        {
            return mappingItems_.at(index.row()).logicalPort_;
        }
        else if (index.column() == InterfacePortMapColumns::INTERFACE_PHYSICAL_NAME)
        {
            return mappingItems_.at(index.row()).physicalPort_;
        }
        else
        {
            return QString("");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::headerData()
//-----------------------------------------------------------------------------
QVariant InterfacePortMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == InterfacePortMapColumns::INTERFACE_LOGICAL_NAME)
            {
                return tr("Logical name");
            }
            else if (section == InterfacePortMapColumns::INTERFACE_PHYSICAL_NAME)
            {
                return tr("Physical name");
            }
            else if (section == InterfacePortMapColumns::INTERFACE_EXCLUDE)
            {
                return tr("Exclude");
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::setData()
//-----------------------------------------------------------------------------
bool InterfacePortMapModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mappingItems_.size())
    {
        return false;
    }

    if (role == Qt::CheckStateRole)
    {
        bool excludeValue = value == Qt::Checked;

        mappingItems_[index.row()].isExcluded_ = excludeValue;

        foreach (QSharedPointer<ActiveInterface> currentInterface, activeInterfaces_)
        {
            currentInterface->getExcludePorts()->removeAll(mappingItems_.at(index.row()).physicalPortName_);
            
            if (mappingItems_.at(index.row()).isExcluded_)
            {
                currentInterface->getExcludePorts()->append(mappingItems_.at(index.row()).physicalPortName_);
            }
        }

        emit contentChanged();

        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags InterfacePortMapModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled;

    if (index.column() == InterfacePortMapColumns::INTERFACE_EXCLUDE)
    {
        if (endPoint_->isHierarchical() || activeInterfaces_.isEmpty())
        {
            itemFlags = Qt::NoItemFlags;
        }
        else
        {
            itemFlags |= Qt::ItemIsUserCheckable;
        }
    }

    return itemFlags;
}
