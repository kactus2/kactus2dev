//-----------------------------------------------------------------------------
// File: swbuildcommandmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 02.04.2013
//
// Description:
// The model class to manage SW build commands in SW Build Command editor.
//-----------------------------------------------------------------------------

#include "swbuildcommandmodel.h"

#include <editors/ComponentEditor/fileBuilders/FileBuilderColumns.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::SWBuildCommandModel()
//-----------------------------------------------------------------------------
SWBuildCommandModel::SWBuildCommandModel(QSharedPointer<Component> component,
	QSharedPointer<QList<QSharedPointer<SWFileBuilder> > > swBuildCommands, 
	QObject *parent):
QAbstractTableModel(parent),
component_(component),
swBuildCommands_(swBuildCommands)
{

}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::~SWBuildCommandModel()
//-----------------------------------------------------------------------------
SWBuildCommandModel::~SWBuildCommandModel()
{
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::rowCount()
//-----------------------------------------------------------------------------
int SWBuildCommandModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return swBuildCommands_->count();
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::columnCount()
//-----------------------------------------------------------------------------
int SWBuildCommandModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return FileBuilderColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags SWBuildCommandModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	// the replace default column is boolean and uses check box
	if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
		flags |= Qt::ItemIsUserCheckable;
	}
	// others can be freely edited by user
	else
    {
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::headerData()
//-----------------------------------------------------------------------------
QVariant SWBuildCommandModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == FileBuilderColumns::FILETYPE_COLUMN)
    {
        return tr("File type");
    }
    else if (section == FileBuilderColumns::COMMAND_COLUMN)
    {
        return tr("Command");
    }
    else if (section == FileBuilderColumns::FLAGS_COLUMN)
    {
        return tr("Flags");
    }
    else if (section == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
        return tr("Replace default flags");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::data()
//-----------------------------------------------------------------------------
QVariant SWBuildCommandModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= swBuildCommands_->size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole )
    {
        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            return swBuildCommands_->at(index.row())->getFileType();
        }
        else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
        {
            return swBuildCommands_->at(index.row())->getCommand();
        }
        else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
        {
            return swBuildCommands_->at(index.row())->getFlags();
        }
        else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            return swBuildCommands_->at(index.row())->getReplaceDefaultFlags();
        }
        else
        {
            return QVariant();					
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            if (swBuildCommands_->at(index.row())->getReplaceDefaultFlags() == QLatin1String("true"))
            {
                return Qt::Checked;
            }
            else 
            {
                return Qt::Unchecked;
            }
        }
        else 
        {
            return QVariant();
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (swBuildCommands_->at(index.row())->isValid())
        {
            return QColor("black");
        }
        else
        {
            return QColor("red");
        }
    }
    // if unsupported role
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::setData()
//-----------------------------------------------------------------------------
bool SWBuildCommandModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= swBuildCommands_->size())
    {
		return false;
	}

    if (role == Qt::EditRole)
    {
        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            swBuildCommands_->at(index.row())->setFileType(value.toString());
        }
        else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
        {
            swBuildCommands_->at(index.row())->setCommand(value.toString());
        }
        else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
        {
            swBuildCommands_->at(index.row())->setFlags(value.toString());
        }
        else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            swBuildCommands_->at(index.row())->setReplaceDefaultFlags(value.toString());
        }
        else
        {
            return false;
        }

        emit contentChanged();
		emit dataChanged(index, index);
		return true;
	}

	else if (role == Qt::CheckStateRole) 
    {
        if (value == Qt::Checked)
        {
            swBuildCommands_->at(index.row())->setReplaceDefaultFlags(QStringLiteral("true"));
        }
        else
        {
            swBuildCommands_->at(index.row())->setReplaceDefaultFlags(QStringLiteral("false"));
        }

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
// Function: SWBuildCommandModel::isValid()
//-----------------------------------------------------------------------------
bool SWBuildCommandModel::isValid() const
{
	// if at least one SW build command is invalid
	foreach (QSharedPointer<SWFileBuilder> buildCom, *swBuildCommands_)
    {
		if (!buildCom->isValid())
        {
			return false;
		}
	}

	// all were valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::onAddItem()
//-----------------------------------------------------------------------------
void SWBuildCommandModel::onAddItem(QModelIndex const& index)
{
	int row = swBuildCommands_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	swBuildCommands_->insert(row, QSharedPointer<SWFileBuilder>(new SWFileBuilder()));
	endInsertRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWBuildCommandModel::onRemoveItem()
//-----------------------------------------------------------------------------
void SWBuildCommandModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= swBuildCommands_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	swBuildCommands_->removeAt(index.row());
	endRemoveRows();

	emit contentChanged();
}
