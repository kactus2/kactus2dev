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

#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::TransactionalPortsModel()
//-----------------------------------------------------------------------------
TransactionalPortsModel::TransactionalPortsModel(QSharedPointer<Model> model,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<PortValidator> portValidator,
    QSortFilterProxyModel* filter, QObject *parent):
PortsModel(model, expressionParser, parameterFinder, expressionFormatter, portValidator, filter, parent)
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

    QSharedPointer<Port> port = portOnRow(index.row());

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
                removeReferencesFromSingleExpression(port->getTransactional()->getBusWidth());
            }

            port->getTransactional()->setBusWidth(value.toString());
        }
        else if (index.column() == TransactionalPortColumns::INITIATIVE)
        {
            port->getTransactional()->setInitiative(value.toString());
        }
        else if (index.column() == TransactionalPortColumns::KIND)
        {
            port->getTransactional()->setKind(value.toString());
        }
        else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getTransactional()->getMaxConnections());
            }

            port->getTransactional()->setMaxConnections(value.toString());
        }
        else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(port->getTransactional()->getMinConnections());
            }

            port->getTransactional()->setMinConnections(value.toString());
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
    QSharedPointer<Port> port = portOnRow(index.row());
    QSharedPointer<Transactional> portTransactional = port->getTransactional();
    if (portTransactional)
    {
        if (index.column() == TransactionalPortColumns::WIDTH)
        {
            return portTransactional->getBusWidth();
        }
        else if (index.column() == TransactionalPortColumns::INITIATIVE)
        {
            return portTransactional->getInitiative();
        }
        else if (index.column() == TransactionalPortColumns::KIND)
        {
            return portTransactional->getKind();
        }
        else if (index.column() == TransactionalPortColumns::PROTOCOL)
        {
            if (portTransactional->getProtocol())
            {
                return portTransactional->getProtocol()->getProtocolType();
            }
            else
            {
                return QVariant();
            }
        }
        else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
        {
            return portTransactional->getMaxConnections();
        }
        else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
        {
            return portTransactional->getMinConnections();
        }
    }
    
    return PortsModel::valueForIndex(index);
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
    QSharedPointer<Port> port = portOnRow(index.row());
    QSharedPointer<Transactional> portTransactional = port->getTransactional();
    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return portTransactional->getBusWidth();
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return portTransactional->getMaxConnections();
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return portTransactional->getMinConnections();
    }
    else
    {
        return PortsModel::expressionOrValueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::validateIndex()
//-----------------------------------------------------------------------------
bool TransactionalPortsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Port> port = portOnRow(index.row());
    QSharedPointer<Transactional> transactional = port->getTransactional();
    if (!transactional)
    {
        return false;
    }

    if (index.column() == TransactionalPortColumns::WIDTH)
    {
        return getValidator()->hasValidTransactionalWidth(transactional);
    }
    else if (index.column() == TransactionalPortColumns::INITIATIVE)
	{
        return getValidator()->hasValidTransactionalInitiative(transactional);
    }
    else if (index.column() == TransactionalPortColumns::KIND)
    {
        return getValidator()->hasValidTransactionalKind(transactional);
    }
    else if (index.column() == TransactionalPortColumns::PROTOCOL)
    {
        return getValidator()->hasValidTransactionalProtocol(transactional);
    }
    else if (index.column() == TransactionalPortColumns::MAX_CONNECTIONS)
    {
        return getValidator()->hasValidTransactionalMaxConnections(transactional);
    }
    else if (index.column() == TransactionalPortColumns::MIN_CONNECTIONS)
    {
        return getValidator()->hasValidTransactionalMinConnections(transactional);
    }
    else
    {
        return PortsModel::validateIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int TransactionalPortsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Port> port = portOnRow(row);
    QSharedPointer<Transactional> transactional = port->getTransactional();

    int refrencesInWidth = transactional->getBusWidth().count(valueID);
    int referencesInMaxConnections = transactional->getMaxConnections().count(valueID);
    int referencesInMinConnections = transactional->getMinConnections().count(valueID);

    int  totalReferences = refrencesInWidth + referencesInMaxConnections + referencesInMinConnections +
        PortsModel::getAllReferencesToIdInItemOnRow(row, valueID);
    return totalReferences;
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
bool TransactionalPortsModel::indexedItemIsDisabled(QModelIndex const& index, QSharedPointer<Port>) const
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
// Function: TransactionalPortsModel::finalizePort()
//-----------------------------------------------------------------------------
void TransactionalPortsModel::finalizePort(QSharedPointer<Port> port)
{
    QSharedPointer<Transactional> newTransactional(new Transactional());

    port->setTransactional(newTransactional);
}
