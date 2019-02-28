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
LibraryErrorModel::LibraryErrorModel(QObject* parent): QAbstractTableModel(parent), errors_(), errorSources_()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::addErrors()
//-----------------------------------------------------------------------------
void LibraryErrorModel::addErrors(QVector<QString> const& errorList, QString const& vlnv)
{
    beginResetModel();
    errors_ += errorList;

    int errorCount = errorList.count();
    for (int i = 0; i < errorCount; i++)
    {
        errorSources_.append(vlnv);
    }

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

    return errors_.count();
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
    if (!index.isValid() || index.row() < 0 || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (index.column() == COLUMN_NUMBER)
    {
        return index.row();
    }
    else if (index.column() == DESCRIPTION)
    {
        return errors_.at(index.row());
    }
    else if (index.column() == VLNV_COLUMN)
    {
        return errorSources_.at(index.row());
    }

    return QVariant();
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
    else if (section == DESCRIPTION)
    {
        return tr("Description");
    }
    else if (section == VLNV_COLUMN)
    {
        return tr("Document");
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
