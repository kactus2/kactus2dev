//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Data model for the transactional ports within abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionTransactionalPortsModel.h"

#include <common/KactusColors.h>

#include <editors/BusDefinitionEditor/AbstractionTransactionalPortColumns.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::AbstractionTransactionalPortsModel()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsModel::AbstractionTransactionalPortsModel(LibraryInterface* libraryAccess,
    QObject *parent):
QAbstractTableModel(parent),
absDef_(),
busDefinition_(),
table_(),
libraryAccess_(libraryAccess)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionTransactionalPortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return AbstractionTransactionalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionTransactionalPortsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    // Do not allow setting system groups for other than system mode.
    if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP && 
        table_.at(index.row()).mode_ != General::SYSTEM &&
        table_.at(index.row()).transactionalPort_->getSystemGroup().isEmpty())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionTransactionalPortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return QVariant();
    }
    if (section == AbstractionTransactionalPortColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == AbstractionTransactionalPortColumns::MODE)
    {
        return tr("Mode");
    }
    else if (section == AbstractionTransactionalPortColumns::PRESENCE)
    {
        return tr("Presence");
    }
    else if (section == AbstractionTransactionalPortColumns::INITIATIVE)
    {
        return tr("Initiative");
    }
    else if (section == AbstractionTransactionalPortColumns::KIND)
    {
        return tr("Kind");
    }
    else if (section == AbstractionTransactionalPortColumns::BUSWIDTH)
    {
        return tr("Bus width");
    }
    else if (section == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
    {
        return tr("System\ngroup");
    }
    else if (section == AbstractionTransactionalPortColumns::QUALIFIER)
    {
        return tr("Qualifier");
    }
    else if (section == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
    {
        return tr("Protocol\ntype");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADNAME)
    {
        return tr("Payload\nname");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADTYPE)
    {
        return tr("Payload\ntype");
    }
    else if (section == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
    {
        return tr("Payload\nextension");
    }
    else if (section == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionTransactionalPortsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_.size())
    {
		return QVariant();
    }
            
    AbstractionTransactionalPortsModel::SignalRow const& port = table_.at(index.row());

	if (role == Qt::DisplayRole) 
    {
        if (index.column() == AbstractionTransactionalPortColumns::NAME) 
        {
            if (port.abstraction_->getLogicalName().isEmpty())
            {
                return "unnamed";
            }
            else
            {
                return port.abstraction_->getLogicalName();
            }
        }
        else if (index.column() == AbstractionTransactionalPortColumns::MODE)
        {
            return General::interfaceMode2Str(port.mode_);
        }
        else if (index.column() == AbstractionTransactionalPortColumns::PRESENCE)
        {
            return PresenceTypes::presence2Str(port.transactionalPort_->getPresence());
        }
        else if (index.column() == AbstractionTransactionalPortColumns::INITIATIVE)
        {
            return getInitiativeForData(port.transactionalPort_);
        }
        else if (index.column() == AbstractionTransactionalPortColumns::KIND)
        {
            return port.transactionalPort_->getKind();
        }
        else if (index.column() == AbstractionTransactionalPortColumns::BUSWIDTH)
        {
            return port.transactionalPort_->getBusWidth();
        }
        else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
        {
            return port.transactionalPort_->getSystemGroup();
        }
        else if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER)
        {
            return qualifierToString(port.abstraction_->getTransactional()->getQualifier());
        }
        else if ((index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE ||
            index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION) &&
            port.transactionalPort_->getProtocol())
        {
            QSharedPointer<Protocol> portProtocol = port.transactionalPort_->getProtocol();
            if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
            {
                return getProtocolTypeText(portProtocol);
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME)
            {
                return portProtocol->getPayloadName();
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE)
            {
                return portProtocol->getPayloadType();
            }
            else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
            {
                return portProtocol->getPayloadExtension();
            }
        }

        else if (index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
        {
            return port.abstraction_->description();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if ((index.column() == AbstractionTransactionalPortColumns::NAME &&
                port.abstraction_->getLogicalName().isEmpty()) ||
            (index.column() == AbstractionTransactionalPortColumns::MODE && 
                (port.mode_ == General::INTERFACE_MODE_COUNT || table_.count(port) > 1)))
        {
            return KactusColors::ERROR;
        }
        else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
        {
            if (!busDefinition_ || (port.mode_ == General::SYSTEM &&
                !BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_).contains(
                    port.transactionalPort_->getSystemGroup())))
            {
                return KactusColors::ERROR;
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    QString oldData = data(index, Qt::DisplayRole).toString();

    if (!index.isValid() || index.row() < 0 || index.row() >= table_.size() || role != Qt::EditRole ||
        oldData.compare(value.toString()) == 0)
    {
        return false;
    }

    AbstractionTransactionalPortsModel::SignalRow& port = table_[index.row()];
    if (index.column() == AbstractionTransactionalPortColumns::NAME)
    {
        QString newName = value.toString();

        QSharedPointer<PortAbstraction> selectedPort = absDef_->getPort(newName);
        if (!selectedPort)
        {
            selectedPort = QSharedPointer<PortAbstraction>(new PortAbstraction());
            selectedPort->setLogicalName(newName);
            absDef_->getLogicalPorts()->append(selectedPort);
        }
        if (!selectedPort->getTransactional())
        {
            selectedPort->setTransactional(
                QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction()));
        }

        if (!portHasOtherSignals(port))
        {
            absDef_->getLogicalPorts()->removeOne(port.abstraction_);
        }

        port.abstraction_ = selectedPort;

    }
    else if (index.column() == AbstractionTransactionalPortColumns::MODE)
    {
        port.mode_ = General::str2Interfacemode(value.toString(), General::INTERFACE_MODE_COUNT);
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PRESENCE)
    {
        port.transactionalPort_->setPresence(
            PresenceTypes::str2Presence(value.toString(), PresenceTypes::UNKNOWN));
    }
    else if (index.column() == AbstractionTransactionalPortColumns::INITIATIVE)
    {
        port.transactionalPort_->setInitiative(getInitiativeForSetData(value.toString()));
    }
    else if (index.column() == AbstractionTransactionalPortColumns::KIND)
    {
        port.transactionalPort_->setKind(value.toString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::BUSWIDTH)
    {
        port.transactionalPort_->setBusWidth(value.toString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::SYSTEM_GROUP)
    {
        port.transactionalPort_->setSystemGroup(value.toString());
    }
    else if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER)
    {
        port.abstraction_->getTransactional()->setQualifier(toQualifier(value.toString()));
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE ||
        index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME ||
        index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE ||
        index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
    {
        QSharedPointer<Protocol> portProtocol = port.transactionalPort_->getProtocol();
        if (!portProtocol)
        {
            if (value.toString().isEmpty())
            {
                return false;
            }
            else
            {
                portProtocol = QSharedPointer<Protocol>(new Protocol());
                port.transactionalPort_->setProtocol(portProtocol);
            }
        }

        if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE)
        {
            portProtocol->setProtocolType(value.toString());
        }
        else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADNAME)
        {
            portProtocol->setPayloadName(value.toString());
        }
        else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE)
        {
            QString payloadType = value.toString();
            if (payloadType.compare("none", Qt::CaseInsensitive) == 0)
            {
                payloadType = "";
            }
            portProtocol->setPayloadType(payloadType);
        }
        else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADEXTENSION)
        {
            portProtocol->setPayloadExtension(value.toString(), false);
        }

        if (portProtocol && portProcotolTypeIsEmpty(portProtocol) &&
            portProtocol->getPayloadName().isEmpty() && portProtocol->getPayloadType().isEmpty() &&
            portProtocol->getPayloadExtension().isEmpty())
        {
            port.transactionalPort_->setProtocol(QSharedPointer<Protocol>());
        }
    }
    else if (index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        port.abstraction_->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    QModelIndexList indexList;
    indexList.append(index);

    if (index.column() == AbstractionTransactionalPortColumns::QUALIFIER ||
        index.column() == AbstractionTransactionalPortColumns::DESCRIPTION)
    {
        for (int i = 0; i < table_.size(); ++i)
        {
            AbstractionTransactionalPortsModel::SignalRow signal = table_.at(i);

            if (signal.abstraction_->getLogicalName().compare(port.abstraction_->getLogicalName()) == 0 &&
                signal != port)
            {
                QModelIndex signalIndex = index.sibling(i, index.column());
                indexList.append(signalIndex);
            }
        }
    }

    sendDataChangeForAllChangedItems(indexList);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::portHasOtherSignals()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::portHasOtherSignals(AbstractionTransactionalPortsModel::SignalRow const& portSignal) const
{
    General::InterfaceMode portMode = portSignal.mode_;
    QSharedPointer<TransactionalAbstraction> portTransaction = portSignal.abstraction_->getTransactional();

    if (portMode == General::MASTER)
    {
        return portTransaction->hasSlavePort() || portTransaction->getSystemPorts()->count() > 0;
    }
    else if (portMode == General::SLAVE)
    {
        return portTransaction->hasMasterPort() || portTransaction->getSystemPorts()->count() > 0;
    }
    else
    {
        return portTransaction->hasMasterPort() || portTransaction->hasSlavePort() ||
            portTransaction->getSystemPorts()->count() > 1;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getInitiativeForData()
//-----------------------------------------------------------------------------
QString AbstractionTransactionalPortsModel::getInitiativeForData(QSharedPointer<TransactionalPort> port) const
{
    QString portInitiative = port->getInitiative();
    if (portInitiative.compare(QStringLiteral("both"), Qt::CaseInsensitive) == 0)
    {
        return QStringLiteral("requires/provides");
    }
    else
    {
        return portInitiative;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getInitiative()
//-----------------------------------------------------------------------------
QString AbstractionTransactionalPortsModel::getInitiativeForSetData(QString const& newInitiativeValue) const
{
    if (newInitiativeValue.compare(QStringLiteral("requires"), Qt::CaseInsensitive) == 0 ||
        newInitiativeValue.compare(QStringLiteral("provides"), Qt::CaseInsensitive) == 0)
    {
        return newInitiativeValue;
    }
    else if (newInitiativeValue.compare(QStringLiteral("requires/provides"), Qt::CaseInsensitive) == 0)
    {
        return QStringLiteral("both");
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::sendDataChangeForAllChangedItems()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::sendDataChangeForAllChangedItems(QModelIndexList changedIndexes)
{
    QModelIndex firstIndex = changedIndexes.first();
    QModelIndex lastIndex = changedIndexes.last();

    foreach (QModelIndex currentIndex, changedIndexes)
    {
        if (currentIndex.row() < firstIndex.row())
        {
            firstIndex = currentIndex;
        }
        else if (currentIndex.row() > lastIndex.row())
        {
            lastIndex = currentIndex;
        }
    }

    emit dataChanged(firstIndex, lastIndex);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::setAbsDef()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    Q_ASSERT_X(absDef, "BusPortsModel::setAbsDef", "Null Abstraction Definition given as parameter");

    absDef_ = absDef;

    beginResetModel();
    table_.clear();

    foreach (QSharedPointer<PortAbstraction> portAbs, *absDef_->getLogicalPorts()) 
    {
        if (portAbs->hasTransactional())
        {
            QSharedPointer<TransactionalAbstraction> transactional = portAbs->getTransactional();

            bool hasValidPort = false;

            if (transactional->hasMasterPort())
            {
                createRow(portAbs, transactional->getMasterPort(), General::MASTER);
                hasValidPort = true;
            }

            if (transactional->hasSlavePort()) 
            {
                createRow(portAbs, transactional->getSlavePort(), General::SLAVE);
                hasValidPort = true;
            }

            foreach (QSharedPointer<TransactionalPort> system, *transactional->getSystemPorts()) 
            {
                createRow(portAbs, system, General::SYSTEM);
                hasValidPort = true;
            }

            if (!hasValidPort)
            {
                createRow(portAbs, QSharedPointer<TransactionalPort>(new TransactionalPort()),
                    General::INTERFACE_MODE_COUNT);
            }
        }
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::save()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::save()
{
    QVector<QSharedPointer<PortAbstraction> > savedPorts;

    for (int i = 0; i < table_.size(); i++) 
    {
        AbstractionTransactionalPortsModel::SignalRow portOnRow = table_.at(i);

        QSharedPointer<PortAbstraction> portAbs = portOnRow.abstraction_;

        if (!savedPorts.contains(portAbs))
        {
            QSharedPointer<TransactionalAbstraction> transactional = portAbs->getTransactional();
            transactional->setMasterPort(QSharedPointer<TransactionalPort>());
            transactional->setSlavePort(QSharedPointer<TransactionalPort>());
            transactional->getSystemPorts()->clear();

            // Save the port for the first mode.
            savePort(portAbs, i);

            // Save different modes for the port abstraction.
            for (int j = i + 1; j < table_.size(); j++)
            {
                if (table_.at(j).abstraction_ == portAbs)
                {                    
                    savePort(portAbs, j);
                }
            }

            savedPorts.append(portAbs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addSignal()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSignal()
{
    AbstractionTransactionalPortsModel::SignalRow port;
    port.abstraction_ = QSharedPointer<PortAbstraction>(new PortAbstraction());
    port.abstraction_->setTransactional(QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction()));
    port.transactionalPort_ = QSharedPointer<TransactionalPort>(new TransactionalPort());

    absDef_->getLogicalPorts()->append(port.abstraction_);

    beginInsertRows(QModelIndex(), table_.count(), table_.count());
    table_.append(port);
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addMaster()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addMaster(QModelIndexList const& indexes)
{
    createNewSignal(General::MASTER, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addSlave()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSlave(QModelIndexList const& indexes)
{
    createNewSignal(General::SLAVE, indexes);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::createNewSignal()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::createNewSignal(General::InterfaceMode newSignalMode,
    QModelIndexList const& selection)
{
    QVector<AbstractionTransactionalPortsModel::SignalRow> targetPorts = getIndexedPorts(selection);

    beginResetModel();

    foreach(AbstractionTransactionalPortsModel::SignalRow signal, targetPorts)
    {
        if (!modeExistsForPort(newSignalMode, signal.abstraction_->getLogicalName()))
        {
            AbstractionTransactionalPortsModel::SignalRow newSignal = constructSignalCopy(signal);
            newSignal.mode_ = newSignalMode;
            newSignal.transactionalPort_->setSystemGroup("");

            table_.append(newSignal);
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::constructSignalCopy()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsModel::SignalRow AbstractionTransactionalPortsModel::constructSignalCopy(
    AbstractionTransactionalPortsModel::SignalRow signal)
{
    AbstractionTransactionalPortsModel::SignalRow newSignal;
    newSignal.abstraction_ = signal.abstraction_;
    newSignal.transactionalPort_ = QSharedPointer<TransactionalPort>(new TransactionalPort());

    return newSignal;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addSystem()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addSystem(QModelIndexList const& indexes)
{
    QVector<AbstractionTransactionalPortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);
    
    beginResetModel();

    foreach(AbstractionTransactionalPortsModel::SignalRow signal, targetPorts)
    {
        AbstractionTransactionalPortsModel::SignalRow newSystemSignal = constructSignalCopy(signal);
        newSystemSignal.mode_ = General::SYSTEM;
        newSystemSignal.transactionalPort_->setSystemGroup("");

        table_.append(newSystemSignal);
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::addAllSystems()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::addAllSystems(QModelIndexList const& indexes)
{
    QVector<AbstractionTransactionalPortsModel::SignalRow> targetPorts = getIndexedPorts(indexes);

    beginResetModel();

    foreach(AbstractionTransactionalPortsModel::SignalRow signal, targetPorts)
    {
        QStringList systemGroups = getMissingSystemGroupsForSignal(signal);
        foreach(QString group, systemGroups)
        {
            AbstractionTransactionalPortsModel::SignalRow newSystemSignal = constructSignalCopy(signal);
            newSystemSignal.mode_ = General::SYSTEM;
            newSystemSignal.transactionalPort_->setSystemGroup(group);

            table_.append(newSystemSignal);
        }
    }

    endResetModel();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getMissingSystemGroupsForSignal()
//-----------------------------------------------------------------------------
QStringList AbstractionTransactionalPortsModel::getMissingSystemGroupsForSignal(
    AbstractionTransactionalPortsModel::SignalRow const& signal) const
{
    QStringList missingSystemGroups;
    if (busDefinition_)
    {
        missingSystemGroups = busDefinition_->getSystemGroupNames();
    }

    foreach(AbstractionTransactionalPortsModel::SignalRow const& comparisonSignal, table_)
    {
        if (comparisonSignal.abstraction_->getLogicalName() == signal.abstraction_->getLogicalName() &&
            comparisonSignal.mode_ == General::SYSTEM)
        {
            missingSystemGroups.removeAll(comparisonSignal.transactionalPort_->getSystemGroup());
        }
    }

    return missingSystemGroups;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getIndexedPorts()
//-----------------------------------------------------------------------------
QVector<AbstractionTransactionalPortsModel::SignalRow> AbstractionTransactionalPortsModel::getIndexedPorts(
    QModelIndexList const& selection)
{
    QVector<AbstractionTransactionalPortsModel::SignalRow> targetPorts;

    foreach(QModelIndex index, selection)
    {
        if (0 <= index.row() && index.row() <= table_.size())
        {
            AbstractionTransactionalPortsModel::SignalRow temp = table_.at(index.row());
            if (!portHasBeenSelected(temp, targetPorts))
            {
                targetPorts.append(temp);
            }
        }
    }

    return targetPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::portHasBeenSelected()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::portHasBeenSelected(
    AbstractionTransactionalPortsModel::SignalRow const& portSignal,
    QVector<AbstractionTransactionalPortsModel::SignalRow> const& selectedPorts) const
{
    foreach(AbstractionTransactionalPortsModel::SignalRow const& signal, selectedPorts)
    {
        if (signal.abstraction_->getLogicalName() == portSignal.abstraction_->getLogicalName())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::modeExistsForPort()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::modeExistsForPort(General::InterfaceMode const& mode,
    QString const& portName) const
{
    foreach(AbstractionTransactionalPortsModel::SignalRow signal, table_)
    {
        if (signal.abstraction_->getLogicalName().compare(portName) == 0 && signal.mode_ == mode)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::onRemoveItem(QModelIndex const& index)
{
    if (0 <= index.row() && index.row() <= table_.size()) 
    {
        AbstractionTransactionalPortsModel::SignalRow portToRemove = table_.at(index.row());

        QString removedName = portToRemove.abstraction_->getLogicalName();
        General::InterfaceMode removedMode = portToRemove.mode_;

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        table_.removeAt(table_.indexOf(portToRemove));
        
        bool removeAbstraction = true;
        foreach (AbstractionTransactionalPortsModel::SignalRow const& row, table_)
        {
            if (row.abstraction_ == portToRemove.abstraction_)
            {
                removeAbstraction = false;
            }
        }

        if (removeAbstraction)
        {
            absDef_->getLogicalPorts()->removeOne(portToRemove.abstraction_);
        }

        endRemoveRows();

        emit contentChanged();
        emit portRemoved(removedName, removedMode);      
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::savePort()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::savePort(QSharedPointer<PortAbstraction> portAbs, int i)
{
    AbstractionTransactionalPortsModel::SignalRow sourcePort = table_.at(i);
    QSharedPointer<TransactionalAbstraction> transactional = portAbs->getTransactional();

    // Set the wirePort placement according to the mode in the table.
    if (sourcePort.mode_ == General::MASTER)
    {        
        transactional->setMasterPort(sourcePort.transactionalPort_);
    }
    else if (sourcePort.mode_ == General::SLAVE)
    {
        transactional->setSlavePort(sourcePort.transactionalPort_);
    }
    else if (sourcePort.mode_ == General::SYSTEM)
    {
        transactional->addSystemPort(sourcePort.transactionalPort_);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::createRow()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsModel::createRow(QSharedPointer<PortAbstraction> portAbs,
    QSharedPointer<TransactionalPort> modeSpesific, General::InterfaceMode mode) 
{
    Q_ASSERT_X(portAbs, "BusPortsModel::createRow", "Null Port Abstraction pointer given as parameter");
    Q_ASSERT_X(modeSpesific, "BusPortsModel::createRow", "Null TransactionalPort pointer given as parameter");

    AbstractionTransactionalPortsModel::SignalRow port;
    port.mode_ = mode;
    port.abstraction_ = portAbs;
    port.transactionalPort_ = modeSpesific;
    
    table_.append(port);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::toString()
//-----------------------------------------------------------------------------
QString AbstractionTransactionalPortsModel::qualifierToString(Qualifier qualifier) const
{
    if (qualifier.isData() && qualifier.isAddress())
    {
        return "data/address";
    }
    else if (qualifier.isData())
    {
        return "data";
    }
    else if (qualifier.isAddress())
    {
        return "address";
    }
    else 
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::toQualifier()
//-----------------------------------------------------------------------------
Qualifier::Type AbstractionTransactionalPortsModel::toQualifier(QString const& str) const
{
    if (str == "address")
    {
        return Qualifier::Address;
    }
    else if (str == "data")
    {
        return Qualifier::Data;
    }
    else if (str == "data/address")
    {
        return Qualifier::Data_Address;
    }
    else
    {
        return Qualifier::Any;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::getProtocolTypeText()
//-----------------------------------------------------------------------------
QString AbstractionTransactionalPortsModel::getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const
{
    QString protocolType = portProtocol->getProtocolType();
    if (protocolType.compare("tlm", Qt::CaseSensitive) != 0)
    {
        protocolType = portProtocol->getCustomProtocolType();
    }

    return protocolType;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::portProcotolTypeIsEmpty()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const
{
    return portProtocol->getProtocolType().isEmpty() ||
        (portProtocol->getProtocolType().compare("custom", Qt::CaseInsensitive) == 0 &&
            portProtocol->getCustomProtocolType().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::SignalRow::SignalRow()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsModel::SignalRow::SignalRow():
abstraction_(QSharedPointer<PortAbstraction>(new PortAbstraction())),
mode_(General::INTERFACE_MODE_COUNT),
transactionalPort_(QSharedPointer<TransactionalPort>(new TransactionalPort()))
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::SignalRow::operator==()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::SignalRow::operator==(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() == other.abstraction_->getLogicalName() &&
        mode_ == other.mode_ && (mode_ != General::SYSTEM ||
            transactionalPort_->getSystemGroup() == other.transactionalPort_->getSystemGroup());
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::SignalRow::operator!=()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::SignalRow::operator!=(SignalRow const& other) const
{
    return  abstraction_->getLogicalName() != other.abstraction_->getLogicalName() ||
        mode_ != other.mode_ ||
        transactionalPort_->getSystemGroup() != other.transactionalPort_->getSystemGroup();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsModel::SignalRow::operator<()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsModel::SignalRow::operator<( const SignalRow& other ) const
{
	// Order by name, mode and system group.
	if (abstraction_->getLogicalName() == other.abstraction_->getLogicalName()) 
    {
        if (mode_ == other.mode_)
        {
            return transactionalPort_->getSystemGroup() < other.transactionalPort_->getSystemGroup();
        }

		return mode_ < other.mode_;
	}
	else
    {
		return abstraction_->getLogicalName() < other.abstraction_->getLogicalName();
    }
}
