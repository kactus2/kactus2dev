//-----------------------------------------------------------------------------
// File: portlistmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2011
//
// Description:
// The base class for logical list model and physical list model.
//-----------------------------------------------------------------------------

#include "portlistmodel.h"

#include <QPixmap>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: PortListModel::PortListModel()
//-----------------------------------------------------------------------------
PortListModel::PortListModel(QObject *parent): 
QAbstractListModel(parent), 
list_()
{
}

//-----------------------------------------------------------------------------
// Function: PortListModel::~PortListModel()
//-----------------------------------------------------------------------------
PortListModel::~PortListModel()
{
}

//-----------------------------------------------------------------------------
// Function: PortListModel::rowCount()
//-----------------------------------------------------------------------------
int PortListModel::rowCount(QModelIndex const& parent) const 
{
	if (parent.isValid())
    {
		return 0;
    }

	return list_.size();
}

//-----------------------------------------------------------------------------
// Function: PortListModel::data()
//-----------------------------------------------------------------------------
QVariant PortListModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= list_.size() || role != Qt::DisplayRole)
    {
		return QVariant();
    }

	return list_.at(index.row());
}

//-----------------------------------------------------------------------------
// Function: PortListModel::addPort()
//-----------------------------------------------------------------------------
void PortListModel::addPort(QString const& portName)
{
	// if the port is already on the list
	if (!list_.contains(portName))
    {
        beginInsertRows(QModelIndex(), list_.size(), list_.size());
        list_.append(portName);
        endInsertRows();
    }
}

//-----------------------------------------------------------------------------
// Function: PortListModel::addPort()
//-----------------------------------------------------------------------------
void PortListModel::removeItem(QModelIndex const& index)
{
	if (!index.isValid())
    {
		return;
    }

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	list_.removeAt(index.row());
	endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: PortListModel::removeItems()
//-----------------------------------------------------------------------------
void PortListModel::removeItems(QModelIndexList const& indexes)
{
	if (indexes.isEmpty())
    {
		return;
    }

	// first find the ports that were indexed
	QStringList portsToRemove;
    foreach (QModelIndex const& index, indexes)
    {
        if (index.isValid() && 0 <= index.row() && index.row() < list_.size())
        {
            portsToRemove.append(list_.at(index.row()));
        }
    }

    removePorts(portsToRemove);
}

//-----------------------------------------------------------------------------
// Function: PortListModel::removeItems()
//-----------------------------------------------------------------------------
void PortListModel::removePorts(QStringList const& portList)
{
    beginResetModel();
    foreach (QString port, portList)
    {
        list_.removeAll(port);
    }
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortListModel::removePort()
//-----------------------------------------------------------------------------
void PortListModel::removePort(QString const& port)
{
    beginResetModel();
    list_.removeAll(port);
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortListModel::onMoveItems()
//-----------------------------------------------------------------------------
void PortListModel::onMoveItems(QStringList const& portNames, QModelIndex const& targetIndex)
{
	bool append = !targetIndex.isValid();

	QString targetItem;
	// find the target item so items can be inserted before it even when it's index changes.
	if (!append)
    {
		targetItem = list_.at(targetIndex.row());
    }

	beginResetModel();
	// move each item
	foreach (QString const& port, portNames)
    {
		// if the source is the same as target
		if (port == targetItem)
        {
			continue;
        }

		// first remove the port from the list
		list_.removeAll(port);

		int index = 0;
		// find the new place for the item
		if (!append)
        {
			index = list_.indexOf(targetItem);
        }
		else 
        {
			index = list_.size();
        }

		// insert the port to the correct location
		list_.insert(index, port);
	}
	endResetModel();
}
