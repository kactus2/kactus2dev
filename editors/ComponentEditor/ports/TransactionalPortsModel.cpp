//-----------------------------------------------------------------------------
// File: TransactionalPortsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.05.2019
//
// Description:
// Table model for transactional ports.
//-----------------------------------------------------------------------------

#include "TransactionalPortsModel.h"

#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>
#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <QIcon>

using namespace std;

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::TransactionalPortsModel()
//-----------------------------------------------------------------------------
TransactionalPortsModel::TransactionalPortsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSortFilterProxyModel* filter, QObject *parent):
PortsModel(parameterFinder, portsInterface, filter, parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::columnCount()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

 	return TransactionalPortColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::headerData()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == TransactionalPortColumns::WIDTH)
        {
            QString widthHeader = tr("Width") + getExpressionSymbol();
            return widthHeader;
        }
        else if (section == TransactionalPortColumns::INITIATIVE)
        {
            return tr("Initiative");
        }
        else if (section == TransactionalPortColumns::KIND)
        {
            return tr("Kind");
        }
        else if (section == TransactionalPortColumns::PROTOCOL)
        {
            return tr("Protocol");
        }
        else if (section == TransactionalPortColumns::MAX_CONNECTIONS)
        {
            QString maxConnectionsHeader = tr("Max\nconnections") + getExpressionSymbol();
            return maxConnectionsHeader;
        }
        else if (section == TransactionalPortColumns::MIN_CONNECTIONS)
        {
            QString minConnectionsHeader = tr("Min\nconnections") + getExpressionSymbol();
            return minConnectionsHeader;
        }
    }

    return PortsModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::data()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= getInterface()->itemCount())
    {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == TransactionalPortColumns::INITIATIVE)
    {
        return QIcon(getIconPath(index.row()));
    }

    return PortsModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::setData()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (PortsModel::setData(index, value, role))
    {
        return true;
    }
    else if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    std::string portName = getInterface()->getIndexedItemName(index.row());

	if (role == Qt::EditRole)
    {
        if (isLocked(index))
        {
            return false;
        }
        else if (index.column() == TransactionalPortColumns::WIDTH)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getBusWidthExpression(portName)));
            }

            getInterface()->setBusWidth(portName, value.toString().toStdString());
        }
        else if (index.column() == TransactionalPortColumns::INITIATIVE)
        {
            QString newInitiative = value.toString();
            if (newInitiative.compare(TransactionalTypes::INITIATIVE_REQUIRES_PROVIDES, Qt::CaseInsensitive) == 0)
            {
                newInitiative = TransactionalTypes::INITIATIVE_BOTH;
            }

            getInterface()->setInitiative(portName, newInitiative.toStdString());
        }
        else if (index.column() == TransactionalPortColumns::KIND)
        {
            getInterface()->setKind(portName, value.toString().toStdString());
        }
        else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getMaxConnectionsExpression(portName)));
            }

            getInterface()->setMaxConnections(portName, value.toString().toStdString());
        }
        else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(getInterface()->getMinConnectionsExpression(portName)));
            }

            getInterface()->setMinConnections(portName, value.toString().toStdString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::valueForIndex(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return QString::fromStdString(getInterface()->getBusWidthValue(portName));
    }
    else if (index.column() == TransactionalPortColumns::INITIATIVE)
    {
        QString initiative = QString::fromStdString(getInterface()->getInitiative(portName));
        if (initiative.compare(TransactionalTypes::INITIATIVE_BOTH, Qt::CaseInsensitive) == 0)
        {
            initiative = TransactionalTypes::INITIATIVE_REQUIRES_PROVIDES;
        }

        return initiative;
    }
    else if (index.column() == TransactionalPortColumns::KIND)
    {
        return QString::fromStdString(getInterface()->getKind(portName));
    }
    else if (index.column() == TransactionalPortColumns::PROTOCOL)
    {
        return QString::fromStdString(getInterface()->getProtocolType(portName));
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMaxConnectionsValue(portName));
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMinConnectionsValue(portName));
    }
    
    return PortsModel::valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return QString::fromStdString(getInterface()->getBusWidthFormattedExpression(portName));
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMaxConnectionsFormattedExpression(portName));
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMinConnectionsFormattedExpression(portName));
    }
    else
    {
        return PortsModel::formattedExpressionForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::expressionForIndex(QModelIndex const& index) const
{
    string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return QString::fromStdString(getInterface()->getBusWidthExpression(portName));
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMaxConnectionsExpression(portName));
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return QString::fromStdString(getInterface()->getMinConnectionsExpression(portName));
    }
    else
    {
        return PortsModel::expressionForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == TransactionalPortColumns::WIDTH ||
        index.column() == TransactionalPortColumns::MAX_CONNECTIONS ||
        index.column() == TransactionalPortColumns::MIN_CONNECTIONS ||
        index.column() == TransactionalPortColumns::ARRAY_LEFT ||
        index.column() == TransactionalPortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant TransactionalPortsModel::expressionOrValueForIndex(QModelIndex const& index) const
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
// Function: TransactionalPortsModel::validateIndex()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::validateIndex(QModelIndex const& index) const
{
    std::string portName = getInterface()->getIndexedItemName(index.row());

    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return getInterface()->portHasValidBusWidth(portName);
    }
    else if (index.column() == TransactionalPortColumns::INITIATIVE)
	{
        return getInterface()->portHasValidInitiative(portName);
    }
    else if (index.column() == TransactionalPortColumns::KIND)
    {
        return getInterface()->portHasValidKind(portName);
    }
    else if (index.column() == TransactionalPortColumns::PROTOCOL)
    {
        return getInterface()->portHasValidProtocol(portName);
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return getInterface()->portHasValidMaxConnections(portName);
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return getInterface()->portHasValidMinConnections(portName);
    }
    else
    {
        return PortsModel::validateIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::rowNumberColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::rowNumberColumn() const
{
    return TransactionalPortColumns::ROW_NUMBER;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::nameColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::nameColumn() const
{
    return TransactionalPortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::typeColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::typeColumn() const
{
    return TransactionalPortColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::arrayLeftColumn() const
{
    return TransactionalPortColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::arrayRightColum()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::arrayRightColum() const
{
    return TransactionalPortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::tagColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::tagColumn() const
{
    return TransactionalPortColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::adHocColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::adHocColumn() const
{
    return TransactionalPortColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::descriptionColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::descriptionColumn() const
{
    return TransactionalPortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::indexedItemIsDisabled()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::indexedItemIsDisabled(QModelIndex const& index) const
{
    return index.column() == TransactionalPortColumns::ROW_NUMBER ||
        index.column() == TransactionalPortColumns::PROTOCOL;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::indexedItemIsMandatory()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::indexedItemIsMandatory(QModelIndex const& index) const
{
    return index.column() == TransactionalPortColumns::NAME ||
        index.column() == TransactionalPortColumns::INITIATIVE;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::indexedItemIsLocked()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::indexedItemIsLocked(QModelIndex const& index) const
{
    return index.column() == TransactionalPortColumns::PROTOCOL;
}

//-----------------------------------------------------------------------------
// Function: WirePortsModel::getLockedPortIndexes()
//-----------------------------------------------------------------------------
QModelIndexList TransactionalPortsModel::getLockedPortIndexes(QModelIndex const& portIndex) const
{
    QModelIndexList lockedIndexes;

    QModelIndex protocolIndex = portIndex.sibling(portIndex.row(), TransactionalPortColumns::PROTOCOL);

    return lockedIndexes;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::addNewPort()
//-----------------------------------------------------------------------------
void TransactionalPortsModel::addNewPort()
{
    getInterface()->addTransactionalPort();
}
