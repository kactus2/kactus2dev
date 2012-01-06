//-----------------------------------------------------------------------------
// File: EndpointModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint data model for the endpoint editor.
//-----------------------------------------------------------------------------

#include "EndpointModel.h"

#include <QDebug>

EndpointModel::EndpointModel(QSharedPointer<Component> component,
                             void* dataPointer, QObject *parent): 
QAbstractTableModel(parent), endpoints_(), table_(), component_(component)
{
    Q_ASSERT_X(dataPointer, "EndpointModel constructor",
        "Null dataPointer given as parameter");

    // Set the pointer to the actual data structure containing the endpoints.
    endpoints_ = static_cast<QMap<QString, QSharedPointer<Port> > *>(dataPointer);

    restore();
}

EndpointModel::~EndpointModel() {
}

int EndpointModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const {

    // not hierarchical model
    if (parent.isValid())
        return 0;

    return table_.size();
}

int EndpointModel::columnCount( const QModelIndex & parent /*= QModelIndex() */ ) const {

    // not hierarchical model
    if (parent.isValid())
        return 0;

    return 4;
}

QVariant EndpointModel::data(const QModelIndex & index, 
                             int role /*= Qt::DisplayRole */) const
{
    if (!index.isValid())
        return QVariant();

    // if row is invalid
    if (index.row() < 0 || index.row() >= table_.size())
        return QVariant();

    if (role == Qt::DisplayRole) {

        switch (index.column()) {
            case 0: 
                return table_.value(index.row())->getName();
            case 1:
                return table_.value(index.row())->getTypeName();
            case 2:
                return General::direction2Str(table_.value(index.row())->getDirection());
            case 3:
                return table_.value(index.row())->getRemoteEndpointName();
            default:
                return QVariant();
        }
    }

    // if unsupported role
    else {
        return QVariant();
    }
}

QVariant EndpointModel::headerData(int section, Qt::Orientation orientation, 
                                   int role /*= Qt::DisplayRole */ ) const
{
    // only horizontal headers are supported
    if (orientation != Qt::Horizontal) 
        return QVariant();

    if (role == Qt::DisplayRole) {

        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Data type");
            case 2:
                return tr("Direction");
            case 3:
                return tr("Remote endpoint name");
            default:
                return QVariant();
        }
    }

    // if unsupported role
    else {
        return QVariant();
    }
}

bool EndpointModel::setData(const QModelIndex& index, const QVariant& value, 
                            int role /*= Qt::EditRole */ )
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {

        switch (index.column()) {
            case 0:
                {
                    // if name changes then the map has to change the indexing also
                    QString oldKey = table_.value(index.row())->getName();
                    table_.value(index.row())->setName(value.toString());
                    break;
                }

            case 1:
                {
                    table_.value(index.row())->setTypeName(value.toString());
                    break;
                }

            case 2:
                {
                    table_.value(index.row())->setDirection(General::str2Direction(value.toString(), General::IN));
                    break;
                }

            case 3:
                {
                    table_.value(index.row())->setRemoteEndpointName(value.toString());
                    break;
                }
            default:
                return false;

        }
        emit dataChanged(index, index);
        return true;
    }
    // if role is not supported
    else {
        return false;
    }
}

Qt::ItemFlags EndpointModel::flags(const QModelIndex& index) const {

    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void EndpointModel::onRemoveRow( int row ) {

    // if row is invalid
    if (row < 0 || row >= table_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);

    // remove the object from the map
    table_.removeAt(row);

    endRemoveRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

void EndpointModel::onAddRow() {

    beginInsertRows(QModelIndex(), table_.size(), table_.size());

    table_.append(QSharedPointer<Port>(new Port()));

    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

bool EndpointModel::isValid() const {

    for (int i = 0; i < table_.size(); ++i) {

        // if one model parameter is not valid
        if (!table_.value(i)->isValid(component_->hasViews())) {

            emit errorMessage(tr("Endpoint %1 is not valid").arg(
                table_.value(i)->getName()));
            return false;
        }
    }
    return true;
}

void EndpointModel::apply() {

    if (!isValid())
        return;

    // remove old model parameters from original model
    endpoints_->clear();

    for (int i = 0; i < table_.size(); ++i) {
        endpoints_->insert(table_.value(i)->getName(), table_.value(i));
    }
}

void EndpointModel::restore() {

    beginResetModel();

    // remove old model parameters
    table_.clear();

    // add all model parameters to the table
    foreach (QSharedPointer<Port> port, *endpoints_) {

        // create a copy of the model parameter
        table_.append(QSharedPointer<Port>(
            new Port(*port.data())));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: importEndpoints()
//-----------------------------------------------------------------------------
bool EndpointModel::importEndpoints(Component const& sourceComp, ImportEndpointsDialog::ImportOption option)
{
    // Check that the source component is of a valid type.
    if (sourceComp.getComponentImplementation() != KactusAttribute::KTS_SW ||
        (sourceComp.getComponentSWType() != KactusAttribute::KTS_SW_APPLICATION &&
         sourceComp.getComponentSWType() != KactusAttribute::KTS_SW_ENDPOINTS))
    {
        return false;
    }

    beginResetModel();

    // If the import option is synchronize, remove all ports that are not part of the source component.
    if (option == ImportEndpointsDialog::IMPORT_SYNCHRONIZE)
    {
        for (int i = 0; i < table_.size(); ++i)
        {
            if (sourceComp.getPort(table_[i]->getName()) == 0)
            {
                table_.removeAt(i);
                --i;
            }
        }
    }

    // Copy the ports representing the endpoints from the source to the target.
    QMap< QString, QSharedPointer<Port> > const& ports = sourceComp.getPorts();
    QMapIterator< QString, QSharedPointer<Port> > portIter(ports);

    while (portIter.hasNext())
    {
        portIter.next();

        QSharedPointer<Port> newPort;
        
        // Try to find an existing port with the same name.
        foreach (QSharedPointer<Port> port, table_)
        {
            if (port->getName() == portIter.key())
            {
                newPort = port;
                break;
            }
        }

        // If an existing port was not found, create a new one.
        if (newPort == 0)
        {
            newPort = QSharedPointer<Port>(new Port());
            newPort->setName(portIter.key());
            table_.append(newPort);
        }

        // Update the port settings.
        newPort->setDirection(portIter.value()->getDirection());
        newPort->setTypeName(portIter.value()->getTypeName());

        // Reset the remote endpoint name if the target component is an endpoints component.
        if (component_->getComponentSWType() == KactusAttribute::KTS_SW_ENDPOINTS)
        {
            newPort->setRemoteEndpointName("");
        }
        // Otherwise the target is an application. The remote endpoint name can be automatically
        // assigned only if the source component is also an application and we are using synchronization.
        else if (sourceComp.getComponentSWType() == KactusAttribute::KTS_SW_APPLICATION)
        {
            newPort->setRemoteEndpointName(portIter.value()->getRemoteEndpointName());
        }
    }

    endResetModel();

    return true;
}
