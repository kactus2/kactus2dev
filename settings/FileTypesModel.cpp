//-----------------------------------------------------------------------------
// File: FileTypesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 20.02.2013
//
// Description:
// Model for file types data.
//-----------------------------------------------------------------------------

#include "FileTypesModel.h"
#include "FileTypeColumns.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileTypesModel::FileTypesModel()
//-----------------------------------------------------------------------------
FileTypesModel::FileTypesModel(QSettings& settings, QObject *parent): QAbstractTableModel(parent)
{
    // Parse existing file types from settings.
    beginResetModel();
    settings.beginGroup("FileTypes");

    for (QString const& typeName : settings.childGroups())
    {
        settings.beginGroup(typeName);
        FileTypeEntry entry;
		entry.name = typeName;
		entry.editInKactus = settings.value("EditInKactus").toBool();
        entry.extensions = settings.value("Extensions").toString();
		entry.executable = settings.value("Executable").toString();

        entries_.append(entry);
        settings.endGroup();
    }

    settings.endGroup();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::rowCount()
//-----------------------------------------------------------------------------
int FileTypesModel::rowCount(QModelIndex const& parent) const
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
int FileTypesModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return FileTypeColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::data()
//-----------------------------------------------------------------------------
QVariant FileTypesModel::data(QModelIndex const& index, int role) const
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        FileTypeEntry const& entry = entries_.at(index.row());

        if (index.column() == FileTypeColumns::NAME)
        {
            return entry.name;
        }
        else if (index.column() == FileTypeColumns::EDITABLE_IN_KACTUS)
        {
            return entry.editInKactus;
        }
        else if (index.column() == FileTypeColumns::EXTENSIONS)
        {
            return entry.extensions;
        }
        else if (index.column() == FileTypeColumns::EXECUTABLE)
        {
            return entry.executable;
        }
        else
        {
            return QVariant();
        }
    }

	else if (role == Qt::UserRole)
    {
        QStringList specifiedFileTypes;
        for (FileTypeEntry const& entry : entries_) 
        {
            specifiedFileTypes.append(entry.name);
        }

        return specifiedFileTypes;
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == FileTypeColumns::NAME)
        {
            return tr("File type");
        }
        else if (section == FileTypeColumns::EDITABLE_IN_KACTUS)
        {
            return tr("Edit in Kactus2");
        }
        else if (section == FileTypeColumns::EXTENSIONS)
        {
            return tr("Extensions");
        }
        else if (section == FileTypeColumns::EXECUTABLE)
        {
            return tr("Execute with");
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::setData()
//-----------------------------------------------------------------------------
bool FileTypesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Check for invalid index.
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == FileTypeColumns::NAME)
        {
            QString name = value.toString();

            // if the file type has already been defined
            for (const FileTypesModel::FileTypeEntry& entry : entries_)
            {
                if (entry.name.compare(name, Qt::CaseInsensitive) == 0)
                {
                    return false;
                }
            }

            entries_[index.row()].name = name;
        }
        else if (index.column() == FileTypeColumns::EDITABLE_IN_KACTUS)
        {
            entries_[index.row()].editInKactus = value.toBool();
        }
        else if (index.column() == FileTypeColumns::EXTENSIONS)
        {
            entries_[index.row()].extensions = value.toString();
        }
        else if (index.column() ==  FileTypeColumns::EXECUTABLE)
        {
            entries_[index.row()].executable = value.toString();
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileTypesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileTypesModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
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
	// first clear all previous defined file types to remove duplicates
	settings.remove("FileTypes");

    settings.beginGroup("FileTypes");
    for (FileTypeEntry const& entry : entries_)
    {
		settings.beginGroup(entry.name);
		settings.setValue("EditInKactus", entry.editInKactus);
        settings.setValue("Extensions", entry.extensions);
		settings.setValue("Executable", entry.executable);
        settings.endGroup();
    }
    settings.endGroup();
}
