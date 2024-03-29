//-----------------------------------------------------------------------------
// File: channelsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The model class to manage the objects for channels editor.
//-----------------------------------------------------------------------------

#include "channelsmodel.h"

#include "ChannelColumns.h"

#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/validators/ChannelValidator.h>

#include <common/KactusColors.h>

#include <QStringList>


//-----------------------------------------------------------------------------
// Function: ChannelsModel::ChannelsModel()
//-----------------------------------------------------------------------------
ChannelsModel::ChannelsModel(QSharedPointer<QList<QSharedPointer<Channel> > > channels, 
    QSharedPointer<ChannelValidator> validator, QObject *parent ) :
QAbstractTableModel(parent),
    channels_(channels),
    validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::rowCount()
//-----------------------------------------------------------------------------
int ChannelsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return channels_->size();
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::columnCount()
//-----------------------------------------------------------------------------
int ChannelsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return ChannelColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ChannelsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == ChannelColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == ChannelColumns::DISPLAY_NAME)
    {
        return tr("Display name");
    }
    else if (section == ChannelColumns::INTERFACES) 
    {
        return tr("Interface references");
    }
    else if (section == ChannelColumns::SHORT_DESCRIPTION)
    {
        return tr("Short description");
    }
    else if (section == ChannelColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::data()
//-----------------------------------------------------------------------------
QVariant ChannelsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= channels_->size())
    {
		return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ChannelColumns::NAME)
        {
            return channels_->at(index.row())->name();
        }
        else if (index.column() == ChannelColumns::DISPLAY_NAME)
        {
            return channels_->at(index.row())->displayName();
        }
        else if (index.column() == ChannelColumns::INTERFACES)
        {
            QStringList interfaceNames = channels_->at(index.row())->getInterfaceNames();
            return interfaceNames.join(' ');
        }
        else if (index.column() == ChannelColumns::SHORT_DESCRIPTION)
        {
            return channels_->at(index.row())->shortDescription();
        }
        else if (index.column() == ChannelColumns::DESCRIPTION)
        {
            return channels_->at(index.row())->description();
        }
        else
        {
            return QVariant();
        }

    }

	// user display role for interface column returns a QStringList
	else if (role == ChannelColumns::USER_DISPLAY_ROLE && index.column() == ChannelColumns::INTERFACES)
    {
		return channels_->at(index.row())->getInterfaceNames();
	}

	else if (role == Qt::ForegroundRole)
    {

        if (index.column() == ChannelColumns::INTERFACES && 
            !validator_->hasValidBusInterfaceReferences(channels_->at(index.row())))
        {
            return KactusColors::ERROR;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
	}

	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ChannelColumns::NAME || index.column() == ChannelColumns::INTERFACES)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::setData()
//-----------------------------------------------------------------------------
bool ChannelsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= channels_->size())
    {
		return false;
	}

	if (role == Qt::EditRole)
    {
        if (index.column() ==  ChannelColumns::NAME)
        {
            channels_->at(index.row())->setName(value.toString());
        }
        else if (index.column() == ChannelColumns::DISPLAY_NAME)
        {
            channels_->at(index.row())->setDisplayName(value.toString());		
        }
        else if (index.column() == ChannelColumns::INTERFACES)
        {
            QString str = value.toString();
            QStringList interfaceNames = str.split(' ', Qt::SkipEmptyParts);
            channels_->at(index.row())->setInterfaces(interfaceNames);
        }
        else if (index.column() == ChannelColumns::SHORT_DESCRIPTION)
        {
            channels_->at(index.row())->setShortDescription(value.toString());
        }
        else if (index.column() == ChannelColumns::DESCRIPTION)
        {
            channels_->at(index.row())->setDescription(value.toString());
        }
        else
        {
            return false;
        }


		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	// user edit role for interface column operates on QStringList
	else if (role == ChannelColumns::USER_EDIT_ROLE && ChannelColumns::INTERFACES == index.column())
    {
		channels_->at(index.row())->setInterfaces(value.toStringList());
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::onAddItem()
//-----------------------------------------------------------------------------
void ChannelsModel::onAddItem(QModelIndex const& index)
{
	int row = channels_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	channels_->insert(row, QSharedPointer<Channel>(new Channel()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit channelAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ChannelsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ChannelsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= channels_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	channels_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit channelRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
