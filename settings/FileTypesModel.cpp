//-----------------------------------------------------------------------------
// File: FileTypesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.02.2013
//
// Description:
// Model for file types data.
//-----------------------------------------------------------------------------

#include "FileTypesModel.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileTypesModel::FileTypesModel()
//-----------------------------------------------------------------------------
FileTypesModel::FileTypesModel(QSettings& settings, QObject *parent)
    : QAbstractTableModel(parent)
{
    // Parse existing file types from settings.
    beginResetModel();
    settings.beginGroup("FileTypes");

    QStringList typeNames = settings.childKeys();

    foreach (QString const& typeName, typeNames)
    {
        FileTypeEntry entry;
        entry.name = typeName;
        entry.extensions = settings.value(typeName).toString();

        entries_.append(entry);
    }

    settings.endGroup();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::~FileTypesModel()
//-----------------------------------------------------------------------------
FileTypesModel::~FileTypesModel()
{
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::rowCount()
//-----------------------------------------------------------------------------
int FileTypesModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return entries_.size();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::columnCount()
//-----------------------------------------------------------------------------
int FileTypesModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return FILE_TYPES_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::data()
//-----------------------------------------------------------------------------
QVariant FileTypesModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        FileTypeEntry const& entry = entries_.at(index.row());

        switch (index.column())
        {
        case FILE_TYPES_COL_NAME:
            {
                return entry.name;
            }

        case FILE_TYPES_COL_EXTENSIONS:
            {
                return entry.extensions;
            }

        default:
            {
                return QVariant();
            }
        }
    }
	 else if (Qt::UserRole == role) {
		 QStringList specifiedFileTypes;
		 foreach (FileTypeEntry const& entry, entries_) {
			 specifiedFileTypes.append(entry.name);
		 }

		 return specifiedFileTypes;
	 }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileTypesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case FILE_TYPES_COL_NAME:
                {
                    return tr("File type");
                }

            case FILE_TYPES_COL_EXTENSIONS:
                {
                    return tr("Extensions");
                }

            default:
                {
                    return QVariant();
                }
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::setData()
//-----------------------------------------------------------------------------
bool FileTypesModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case FILE_TYPES_COL_NAME:
            {
                entries_[index.row()].name = value.toString();
                break;
            }

        case FILE_TYPES_COL_EXTENSIONS:
            {
                entries_[index.row()].extensions = value.toString();
                break;
            }

        default:
            {
                return false;
            }
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileTypesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    switch (index.column())
    {
    case FILE_TYPES_COL_NAME:
    case FILE_TYPES_COL_EXTENSIONS:
        {
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }

    default:
        return Qt::ItemIsEnabled;
    }
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::onAddRow()
//-----------------------------------------------------------------------------
void FileTypesModel::onAddRow()
{
    beginInsertRows(QModelIndex(), entries_.size(), entries_.size());
    entries_.append(FileTypeEntry());
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::onRemoveRow()
//-----------------------------------------------------------------------------
void FileTypesModel::onRemoveRow(int row)
{
    // Check if the row is invalid.
    if (row < 0 || row >= entries_.size())
    {
        return;
    }

    // Remove the entry at the given row.
    beginRemoveRows(QModelIndex(), row, row);
    entries_.removeAt(row);
    endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::onAddItem()
//-----------------------------------------------------------------------------
void FileTypesModel::onAddItem(const QModelIndex& index)
{
    int row = entries_.size();

    // If the index is valid then add the item to the correct position.
    if (index.isValid())
    {
        row = index.row();
    }

    // Insert a new empty file type entry.
    beginInsertRows(QModelIndex(), row, row);
    entries_.insert(row, FileTypeEntry());
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FileTypesModel::onRemoveItem(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    // Make sure the row number if valid.
    if (index.row() < 0 || index.row() >= entries_.size())
    {
        return;
    }

    // Remove the entry at the row specified by the index.
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    entries_.removeAt(index.row());
    endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::apply()
//-----------------------------------------------------------------------------
void FileTypesModel::apply(QSettings& settings)
{
    foreach (FileTypeEntry const& entry, entries_)
    {
        settings.setValue("FileTypes/" + entry.name, entry.extensions);
    }
}
