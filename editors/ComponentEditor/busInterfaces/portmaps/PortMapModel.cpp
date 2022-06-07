//-----------------------------------------------------------------------------
// File: PortMapModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.05.2022
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapModel.h"

#include "PortMapsColumns.h"

#include <common/KactusColors.h>

#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>

#include <IPXACTmodels/common/PresenceTypes.h>

#include <QIcon>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: PortMapModel::PortMapModel()
//-----------------------------------------------------------------------------
PortMapModel::PortMapModel(QSharedPointer<ParameterFinder> parameterFinder, PortMapInterface* portMapInterface,
    QObject* parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
portMapInterface_(portMapInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapModel::columnCount()
//-----------------------------------------------------------------------------
int PortMapModel::columnCount(const QModelIndex&) const
{
    return PortMapsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::rowCount()
//-----------------------------------------------------------------------------
int PortMapModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return portMapInterface_->itemCount();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == PortMapsColumns::LOGICAL_PORT)
            {
                return tr("Logical signal");
            }
            else if (section == PortMapsColumns::LOGICAL_PRESENCE)
            {
                return tr("Requirement");
            }
            else if (section == PortMapsColumns::LOGICAL_LEFT)
            {
                QString logicalLeft = tr("Logical left") + getExpressionSymbol();
                return logicalLeft;
            }
            else if (section == PortMapsColumns::LOGICAL_RIGHT)
            {
                QString logicalRight = tr("Logical right") + getExpressionSymbol();
                return logicalRight;
            }
            else if (section == PortMapsColumns::PHYSICAL_PORT)
            {
                return tr("Physical port");
            }
            else if (section == PortMapsColumns::PHYSICAL_LEFT)
            {
                QString physicalLeft = tr("Physical left") + getExpressionSymbol();
                return physicalLeft;
            }
            else if (section == PortMapsColumns::PHYSICAL_RIGHT)
            {
                QString physicalRight = tr("Physical right") + getExpressionSymbol();
                return physicalRight;
            }
            else if (section == PortMapsColumns::INVERT)
            {
                return tr("Invert");
            }
            else if (section == PortMapsColumns::ISINFORMATIVE)
            {
                return tr("Informative");
            }
            else if (section == PortMapsColumns::TIEOFF)
            {
                QString tieOff = tr("Tie Off") + getExpressionSymbol();
                return tieOff;
            }
        }

        else if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::data()
//-----------------------------------------------------------------------------
QVariant PortMapModel::data(QModelIndex const& index, int role) const
{
    if (!indexIsValid(index))
    {
        return QVariant();
    }

    int portMapIndex = index.row();
    if (role == Qt::DisplayRole)
    {
        return formattedExpressionForIndex(index);
    }

    else if (role == Qt::CheckStateRole &&
        (index.column() == PortMapsColumns::INVERT || index.column() == PortMapsColumns::ISINFORMATIVE))
    {
        if ((index.column() == PortMapsColumns::INVERT && portMapInterface_->getInvertValue(portMapIndex).toBool())
            ||
            (index.column() == PortMapsColumns::ISINFORMATIVE &&
                portMapInterface_->getIsInformativeValue(portMapIndex)))
        {
            return Qt::Checked;
        }
        else
        {
            return Qt::Unchecked;
        }
    }

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == PortMapsColumns::LOGICAL_PRESENCE)
        {
            return KactusColors::DISABLED_TEXT;
        }
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::DecorationRole)
    {
        if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PORT)
        {
            return QIcon(QString::fromStdString(portMapInterface_->getLogicalPortIconPath(portMapIndex)));
        }
        else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
        {
            return QIcon(QString::fromStdString(portMapInterface_->getPhysicalPortIconPath(portMapIndex)));
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(100, 20);
    }
    else if (role == Qt::UserRole + 1)
    {
        return portMapInterface_->logicalPortExists(portMapIndex);
    }

    // Unsupported role.
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalLeftBoundFormattedExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalRightBoundFormattedExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalLeftBoundFormattedExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalRightBoundFormattedExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(portMapInterface_->getLogicalTieOffFormattedExpression(index.row()));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapModel::expressionForIndex(QModelIndex const& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalLeftBoundExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalRightBoundExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalLeftBoundExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalRightBoundExpression(index.row()));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(portMapInterface_->getLogicalTieOffExpression(index.row()));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalPortName(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalPortName(index.row()));
    }
    else if (!index.parent().isValid() && index.column() == PortMapsColumns::LOGICAL_PRESENCE)
    {
        return QString::fromStdString(portMapInterface_->getLogicalPresence(index.row()));
    }
    else if (index.column() == PortMapsColumns::INVERT)
    {
        return QString::fromStdString(portMapInterface_->getInvertString(index.row()));
    }
    else if (index.column() == PortMapsColumns::ISINFORMATIVE)
    {
        return QString::fromStdString(portMapInterface_->getIsInformativeString(index.row()));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(portMapInterface_->getLogicalTieOffValue(index.row()));
    }
    if (index.column() == PortMapsColumns::LOGICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalLeftBoundValue(index.row()));
    }
    else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getLogicalRightBoundValue(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalLeftBoundValue(index.row()));
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
    {
        return QString::fromStdString(portMapInterface_->getPhysicalRightBoundValue(index.row()));
    }
    else if (index.column() == PortMapsColumns::TIEOFF)
    {
        return QString::fromStdString(portMapInterface_->getLogicalTieOffValue(index.row()));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::setData()
//-----------------------------------------------------------------------------
bool PortMapModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
    bool success = false;

    if (indexIsValid(index) && index.data(Qt::EditRole) != value)
    {
        if (role == Qt::EditRole)
        {
            if (index.column() == PortMapsColumns::LOGICAL_PORT)
            {
                portMapInterface_->setLogicalPort(index.row(), value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::LOGICAL_LEFT)
            {
                portMapInterface_->setLogicalLeftBound(index.row(), value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                portMapInterface_->setLogicalRightBound(index.row(), value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
            {
                QString oldPhysicalName = QString::fromStdString(portMapInterface_->getPhysicalPortName(index.row()));
                QString newPhysicalName = value.toString();

                portMapInterface_->setPhysicalPort(index.row(), newPhysicalName.toStdString());
                sendPortConnectionSignal(oldPhysicalName, newPhysicalName);
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_LEFT)
            {
                portMapInterface_->setPhysicalLeftBound(index.row(), value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::PHYSICAL_RIGHT)
            {
                portMapInterface_->setPhysicalRightBound(index.row(), value.toString().toStdString());
            }
            else if (index.column() == PortMapsColumns::TIEOFF)
            {
                portMapInterface_->setLogicalTieOff(index.row(), value.toString().toStdString());
            }
        }
        else if (role == Qt::CheckStateRole)
        {
            bool checkValue = (value == Qt::Checked);
            if (index.column() == PortMapsColumns::INVERT)
            {
                portMapInterface_->setInvertValue(index.row(), checkValue);
            }
            else if (index.column() == PortMapsColumns::ISINFORMATIVE)
            {
                portMapInterface_->setIsInformativeValue(index.row(), checkValue);
            }
        }

        emit dataChanged(index, index);
        emit contentChanged();

        success = true;
    }

    return success;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::sendPortConnectionSignal()
//-----------------------------------------------------------------------------
void PortMapModel::sendPortConnectionSignal(QString const& oldName, QString const& newName)
{
    if (!oldName.isEmpty())
    {
        emit portDisconnected(oldName);
    }

    emit portConnected(newName);
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::indexIsValid()
//-----------------------------------------------------------------------------
bool PortMapModel::indexIsValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.row() >= 0 && index.row() < portMapInterface_->itemCount())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortMapModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    bool logicalPortExists = portMapInterface_->logicalPortExists(index.row());
    if (logicalPortExists)
    {
        itemFlags |= Qt::ItemIsDropEnabled;
    }

    if (index.column() == PortMapsColumns::INVERT || index.column() == PortMapsColumns::ISINFORMATIVE)
    {
        itemFlags |= Qt::ItemIsUserCheckable;
    }
    else if (index.column() != PortMapsColumns::LOGICAL_PRESENCE)
    {
        itemFlags |= Qt::ItemIsEditable;
    }

    return itemFlags;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::reset()
//-----------------------------------------------------------------------------
void PortMapModel::reset()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::refresh()
//-----------------------------------------------------------------------------
void PortMapModel::refresh()
{
    reset();
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PortMapModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == PortMapsColumns::LOGICAL_LEFT || index.column() == PortMapsColumns::LOGICAL_RIGHT ||
        index.column() == PortMapsColumns::PHYSICAL_LEFT || index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
        index.column() == PortMapsColumns::TIEOFF;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PortMapModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeModel::validateIndex()
//-----------------------------------------------------------------------------
bool PortMapModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT && !portMapInterface_->logicalPortExists(index.row()))
    {
        return false;
    }

    if (index.column() == PortMapsColumns::LOGICAL_PORT || index.column() == PortMapsColumns::LOGICAL_LEFT ||
        index.column() == PortMapsColumns::LOGICAL_RIGHT || index.column() == PortMapsColumns::PHYSICAL_PORT ||
        index.column() == PortMapsColumns::PHYSICAL_LEFT || index.column() == PortMapsColumns::PHYSICAL_RIGHT ||
        index.column() == PortMapsColumns::TIEOFF)
    {
        if (portMapInterface_->logicalPortIsValid(index.row()))
        {
            if (portMapInterface_->physicalMappingIsValid(index.row()))
            {
                if (index.column() != PortMapsColumns::TIEOFF &&
                    portMapInterface_->physicalPortIsValid(index.row()))
                {
                    if (index.column() == PortMapsColumns::LOGICAL_PORT ||
                        index.column() == PortMapsColumns::PHYSICAL_PORT)
                    {
                        return portMapInterface_->connectedPortsHaveValidPortTypes(index.row()) &&
                            portMapInterface_->connectedPortsHaveValidDirections(index.row()) &&
                            portMapInterface_->connectedPortsHaveValidInitiatives(index.row());
                    }
                    else if (index.column() == PortMapsColumns::LOGICAL_LEFT ||
                        index.column() == PortMapsColumns::LOGICAL_RIGHT ||
                        index.column() == PortMapsColumns::PHYSICAL_LEFT ||
                        index.column() == PortMapsColumns::PHYSICAL_RIGHT)
                    {
                        return portMapInterface_->connectedPortsHaveSameRange(index.row());
                    }
                }
                else if (index.column() == PortMapsColumns::TIEOFF &&
                    portMapInterface_->tieOffIsValid(index.row()))
                {
                    return true;
                }
            }

            if (index.column() == PortMapsColumns::LOGICAL_PORT)
            {
                return true;
            }
            else if (index.column() == PortMapsColumns::LOGICAL_LEFT ||
                index.column() == PortMapsColumns::LOGICAL_RIGHT)
            {
                return portMapInterface_->logicalPortHasValidRange(index.row());
            }
        }

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::onAddPortMap()
//-----------------------------------------------------------------------------
void PortMapModel::onAddPortMap(QModelIndex const& itemIndex)
{
    int additionRow = itemIndex.row();
    if (!itemIndex.isValid())
    {
        additionRow = portMapInterface_->itemCount();
    }

    beginInsertRows(QModelIndex(), additionRow, additionRow);

    portMapInterface_->addPortMap(additionRow);

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::onRemovePortMap()
//-----------------------------------------------------------------------------
void PortMapModel::onRemovePortMap(QModelIndex const& itemIndex)
{
    if (!indexIsValid(itemIndex))
    {
        return;
    }

    QString physicalPortName(QString::fromStdString(portMapInterface_->getPhysicalPortName(itemIndex.row())));

    beginRemoveRows(QModelIndex(), itemIndex.row(), itemIndex.row());

    portMapInterface_->removePortMap(itemIndex.row());

    endRemoveRows();

    if (!physicalPortName.isEmpty())
    {
        emit portDisconnected(physicalPortName);
    }

    emit dataChanged(itemIndex, itemIndex);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList PortMapModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());
    types << "text/plain";

    return types;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions PortMapModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::dropMimeData()
//-----------------------------------------------------------------------------
bool PortMapModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int, int,
    const QModelIndex &parent)
{
    if (!parent.isValid())
    {
        return false;
    }

    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    QString mimeText = data->text();

    QStringList physicalPorts = mimeText.split(';');

    beginResetModel();

    QString firstPort = physicalPorts.takeFirst();
    portMapInterface_->setPhysicalPort(parent.row(), firstPort.toStdString());

    endResetModel();

    if (physicalPorts.size() > 0)
    {
        std::string logicalName = portMapInterface_->getLogicalPortName(parent.row());

        int portMapIndex = portMapInterface_->itemCount();

        beginInsertRows(QModelIndex(), portMapIndex, portMapIndex + physicalPorts.size() - 1);

        foreach(QString currentPort, physicalPorts)
        {
            //         setData(physicalIndex, currentPort, Qt::EditRole);
            portMapInterface_->addPortMap(portMapIndex);
            portMapInterface_->setPhysicalPort(portMapIndex, currentPort.toStdString());
            portMapInterface_->setLogicalPort(portMapIndex, logicalName);

            portMapIndex = portMapInterface_->itemCount();
        }

        endInsertRows();
    }

    emit contentChanged();

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::onRemoveAllPortMappings()
//-----------------------------------------------------------------------------
void PortMapModel::onRemoveAllPortMappings()
{
    beginRemoveRows(QModelIndex(), 0, portMapInterface_->itemCount() - 1);

    for (int i = 0; i < portMapInterface_->itemCount(); ++i)
    {
        portMapInterface_->removePortMap(0);
    }

    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::onAddAutoConnectedPortMaps()
//-----------------------------------------------------------------------------
void PortMapModel::onAddAutoConnectedPortMaps(QVector<QString> physicalPorts)
{
    beginResetModel();

    for (auto portName : physicalPorts)
    {
        emit portConnected(portName);
    }

    endResetModel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortMapModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PortMapModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return portMapInterface_->getAllReferencesToIdInItem(
        portMapInterface_->getIndexedItemName(row), valueID.toStdString());
}
