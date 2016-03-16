//-----------------------------------------------------------------------------
// File: LibraryErrorModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 04.09.2012
//
// Description:
// Library error model.
//-----------------------------------------------------------------------------

#include "LibraryErrorModel.h"

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::LibraryErrorModel()
//-----------------------------------------------------------------------------
LibraryErrorModel::LibraryErrorModel(QObject* parent): QAbstractTableModel(parent), errors_()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::~LibraryErrorModel()
//-----------------------------------------------------------------------------
LibraryErrorModel::~LibraryErrorModel()
{
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::addErrors()
//-----------------------------------------------------------------------------
void LibraryErrorModel::addErrors(QVector<QString> const& errorList)
{
    beginResetModel();
    errors_ = errorList;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::rowCount()
//-----------------------------------------------------------------------------
int LibraryErrorModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return errors_.size();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::columnCount()
//-----------------------------------------------------------------------------
int LibraryErrorModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::data()
//-----------------------------------------------------------------------------
QVariant LibraryErrorModel::data(QModelIndex const& index, int role) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= errors_.size() || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (index.column() == COLUMN_NUMBER)
    {
        return index.row();
    }
    else //if (index.column() == COLUMN_DESCRIPTION)
    {
        return errors_.at(index.row());
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::headerData()
//-----------------------------------------------------------------------------
QVariant LibraryErrorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && orientation != Qt::Vertical)
    {
        return QVariant();
    }

    if (section == COLUMN_NUMBER)
    {
        return "#";
    }
    else if (section == COLUMN_DESCRIPTION)
    {
        return tr("Description");
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LibraryErrorModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled;
}
