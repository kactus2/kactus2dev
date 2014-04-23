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
LibraryErrorModel::LibraryErrorModel(QObject* parent)
    : QAbstractTableModel(parent),
      errors_()
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
void LibraryErrorModel::addErrors(QStringList const& errorList)
{
    beginResetModel();
    errors_ = errorList;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::rowCount()
//-----------------------------------------------------------------------------
int LibraryErrorModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
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
int LibraryErrorModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
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
QVariant LibraryErrorModel::data(const QModelIndex& index,
                                 int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= errors_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case COLUMN_NUMBER:
            return index.row();

        case COLUMN_DESCRIPTION:
            return errors_.at(index.row());
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::headerData()
//-----------------------------------------------------------------------------
QVariant LibraryErrorModel::headerData(int section, Qt::Orientation orientation,
                                       int role /*= Qt::DisplayRole */) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case COLUMN_NUMBER:
                {
                    return "#";
                }

            case COLUMN_DESCRIPTION:
                {
                    return tr("Description");
                }
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: LibraryErrorModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags LibraryErrorModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled;
}
