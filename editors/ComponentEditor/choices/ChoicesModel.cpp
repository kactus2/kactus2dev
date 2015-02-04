//-----------------------------------------------------------------------------
// File: ChoicesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing choices in a component.
//-----------------------------------------------------------------------------

#include "ChoicesModel.h"

#include <IPXACTmodels/choice.h>

#include <QColor>

namespace
{
    enum COLUMNS 
    {
        CHOICE_NAME = 0,
        CHOICE_ENUMERATIONS,
        COLUMN_COUNT
    };
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::ChoicesModel()
//-----------------------------------------------------------------------------
ChoicesModel::ChoicesModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject* parent ):
QAbstractTableModel(parent),
choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::~ChoicesModel()
//-----------------------------------------------------------------------------
ChoicesModel::~ChoicesModel()
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::rowCount()
//-----------------------------------------------------------------------------
int ChoicesModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
		return 0;
	}

	return choices_->count();
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::columnCount()
//-----------------------------------------------------------------------------
int ChoicesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
		return 0;
	}

	return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ChoicesModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::headerData()
//-----------------------------------------------------------------------------
QVariant ChoicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole == role && orientation == Qt::Horizontal)
    {
        if (section == CHOICE_NAME)
        {
            return tr("Name");
        }
        else if (section == CHOICE_ENUMERATIONS)
        {
            return tr("Enumeration(s)");
        }
        else
        {
            return QVariant();
        }
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::data()
//-----------------------------------------------------------------------------
QVariant ChoicesModel::data( const QModelIndex& index, int role) const
{
	if (isNotValidIndex(index))
    {
		return QVariant();
	}

    QSharedPointer<Choice> choice = choices_->at(index.row());

    if (Qt::DisplayRole == role)
    {
        switch (index.column()) {
        case CHOICE_NAME:
            {
                return choice->getName();
            }
        case CHOICE_ENUMERATIONS:
            {
                return choice->getEnumerationValues().join(", ");
            }
        default:
            {
                return QVariant();
            }
        }
	}
    else if (Qt::EditRole == role)
    {
        if (index.column() == CHOICE_NAME) 
        {
            return choice->getName();
        }
        else
        {
            return QVariant();
        }
    }
	else if (Qt::ForegroundRole == role)
    {
        if (choice->isValid())
        {
            return QColor(Qt::black);
        }
        else
        {
            return QColor(Qt::red);
        }
	}
    else if (Qt::BackgroundRole == role)
    {
        return QColor("lemonChiffon");
    }
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::setData()
//-----------------------------------------------------------------------------
bool ChoicesModel::setData( const QModelIndex& index, const QVariant& value, int role)
{
	if (isNotValidIndex(index))
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        if (index.column() == CHOICE_NAME)
        {       
            choices_->at(index.row())->setName(value.toString());
            emit dataChanged(index, index);
            emit contentChanged();
            return true;
        }
        else 
        {
            return false;
        }
    }
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::isValid()
//-----------------------------------------------------------------------------
bool ChoicesModel::isValid() const
{
	foreach (QSharedPointer<Choice> choice, *choices_)
    {
        if (!choice->isValid())
        {
            return false;
        }
    }
 
    return true;
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::onAddItem()
//-----------------------------------------------------------------------------
void ChoicesModel::onAddItem(QModelIndex const& index)
{
	int row = rowCount();

	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	choices_->insert(row, QSharedPointer<Choice>(new Choice()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ChoicesModel::onRemoveItem(QModelIndex const& index)
{
    if (isNotValidIndex(index))
    {
		return;
	}

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	choices_->removeAt(index.row());
	endRemoveRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ChoicesModel::isNotValidIndex()
//-----------------------------------------------------------------------------
bool ChoicesModel::isNotValidIndex(QModelIndex const& index) const
{
    return !index.isValid() || index.row() < 0 || index.row() >= rowCount();
}
