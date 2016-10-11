//-----------------------------------------------------------------------------
// File: filesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 1.6.2012
//
// Description:
// The model that contains the files to edit in files summary editor.
//-----------------------------------------------------------------------------

#include "filesmodel.h"
#include "FileColumns.h"

#include <common/utils.h>
#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>

#include <QColor>
#include <QFileInfo>
#include <QMimeData>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: FilesModel::FilesModel()
//-----------------------------------------------------------------------------
FilesModel::FilesModel(LibraryInterface* handler, QSharedPointer<Component> component,
                       QSharedPointer<FileSet> fileSet, QObject* parent):
QAbstractTableModel(parent),
handler_(handler),
component_(component),
fileSet_(fileSet), 
files_(fileSet->getFiles())
{

}

//-----------------------------------------------------------------------------
// Function: FilesModel::~FilesModel()
//-----------------------------------------------------------------------------
FilesModel::~FilesModel()
{

}

//-----------------------------------------------------------------------------
// Function: FilesModel::rowCount()
//-----------------------------------------------------------------------------
int FilesModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return files_->size();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::columnCount()
//-----------------------------------------------------------------------------
int FilesModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return FileColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FilesModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::ItemIsDropEnabled;
	}
	
    if (index.column() == FileColumns::NAME_COLUMN)
    {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::headerData()
//-----------------------------------------------------------------------------
QVariant FilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole)
    {
        if (section == FileColumns::NAME_COLUMN)
        {
            return tr("File name");
        }
        else if (section == FileColumns::PATH_COLUMN)
        {
            return tr("File path");
        }
        else if (section == FileColumns::TYPES_COLUMN)
        {
            return tr("File types");
        }
        else if (section == FileColumns::DESCRIPTION)
        {
            return tr("Description");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::data()
//-----------------------------------------------------------------------------
QVariant FilesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= files_->size())
    {
		return QVariant();
	}

    QSharedPointer<File> file = files_->at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == FileColumns::NAME_COLUMN)
        {
            QFileInfo fileInfo(files_->at(index.row())->name());
            QString fileName = fileInfo.fileName();
            if (fileName.isEmpty())
            {
                fileName = tr("unnamed");
            }

            return fileName;            
        }
        else if (index.column() == FileColumns::PATH_COLUMN)
        {
            QString filePath = file->name();
            if (filePath.isEmpty() && role == Qt::DisplayRole)
            {
                filePath = tr("no path or URI specified");
            }

            return filePath;
        }
        else if (index.column() == FileColumns::TYPES_COLUMN)
        {
            return file->getFileTypes()->join(' ');
        }
        else if (index.column() == FileColumns::DESCRIPTION)
        {
            if (role == Qt::DisplayRole)
            {
                return file->getDescription().replace(QRegularExpression("\n.*$",
                    QRegularExpression::DotMatchesEverythingOption), "...");
            }
            else
            {
                return file->getDescription();
            }
        }
	}

	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == FileColumns::NAME_COLUMN)
        {
            if (file->name().isEmpty())
            {
                return QColor("red");
            }
            else
            {
                return QColor("gray");
            }
        }
        if (index.column() == FileColumns::PATH_COLUMN && !(filePathExists(file) || isValidURI(file)))
        {
            return QColor("red");
        }

        return QColor("black");
	}

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == FileColumns::DESCRIPTION)
        {
            return file->getDescription();
        }
        else if (!filePathExists(file) && !isValidURI(file))
        {
            return tr("File path is not found or defines an invalid URI.");
        }
    }

	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == FileColumns::PATH_COLUMN || index.column() == FileColumns::TYPES_COLUMN)
        {
            return QColor("LemonChiffon");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::setData()
//-----------------------------------------------------------------------------
bool FilesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= files_->size())
    {
		return false;
	}

    if (role == Qt::EditRole)
    {
        if (index.column() == FileColumns::NAME_COLUMN)
        {
            // User can not edit the file name, it must be done through file path.
            return false;
        }
        else if (index.column() == FileColumns::PATH_COLUMN)
        {
            QString filePath = value.toString();
            if (filePath.isEmpty())
            {
                return false;
            }

            files_->at(index.row())->setName(filePath);
        }
        else if (index.column() == FileColumns::TYPES_COLUMN)
        {
            QString str = value.toString();
            QSharedPointer<QStringList> fileTypes(new QStringList(str.split(' ', QString::SkipEmptyParts)));

            files_->at(index.row())->setFileTypes(fileTypes);
        }
        else if (index.column() == FileColumns::DESCRIPTION)
        {
            files_->at(index.row())->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}

    return false;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList FilesModel::mimeTypes() const
{
    return QStringList(QStringLiteral("text/uri-list"));
}

//-----------------------------------------------------------------------------
// Function: FilesModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions FilesModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::dropMimeData()
//-----------------------------------------------------------------------------
bool FilesModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int /*row*/, int /*column*/,
    QModelIndex const& /*parent*/)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    foreach(QUrl const& filePath, data->urls())
    {
        if (filePath.isLocalFile())
        {
            onAddItem(QModelIndex(), filePath.toLocalFile());
        }      
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::onAddItem()
//-----------------------------------------------------------------------------
void FilesModel::onAddItem(QModelIndex const& index)
{
    int row = files_->size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    QSettings settings;
    QSharedPointer<File> file(new File());    

    beginInsertRows(QModelIndex(), row, row);
    files_->insert(row, file);
    endInsertRows();

    // inform navigation tree that file set is added
    emit fileAdded(row);

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::onAddItem()
//-----------------------------------------------------------------------------
void FilesModel::onAddItem(QModelIndex const& index, const QString& filePath)
{
	int row = files_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// convert the given path into relative path
	const QString xmlPath = handler_->getPath(component_->getVlnv());
	const QString relPath = General::getRelativePath(xmlPath, filePath);

	// if relative path could not be created.
	if (relPath.isEmpty())
    {
		return;
	}

	// if the file is already contained in the list
	foreach (QSharedPointer<File> file, *files_)
    {
		if (file->name() == relPath)
        {
			return;
		}
	}

	QSettings settings;
    QSharedPointer<File> file(new File(relPath));
	file->setFileTypes(settings);

	beginInsertRows(QModelIndex(), row, row);
	files_->insert(row, file);
	endInsertRows();

	// inform navigation tree that file set is added
	emit fileAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FilesModel::onRemoveItem(QModelIndex const& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= files_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	files_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fileRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::onMoveItem()
//-----------------------------------------------------------------------------
void FilesModel::onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos )
{
	// if there was no item in the starting point
    // if the indexes are the same
    // if the indexes are outside the table
	if (!originalPos.isValid() || originalPos == newPos || originalPos.row() < 0 ||
        originalPos.row() >= files_->size())
    {
		return;
	}

	int source = originalPos.row();
	int target = 0;

	// if the new position is invalid index then put the item last in the table
	if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= files_->size())
    {
		beginResetModel();
		QSharedPointer<File> file = files_->takeAt(originalPos.row());
		files_->append(file);
		target = files_->size() - 1;
		endResetModel();
	}
	// if both indexes were valid
	else
    {
		beginResetModel();
		files_->swap(originalPos.row(), newPos.row());
		target = newPos.row();
		endResetModel();
	}

	emit fileMoved(source, target);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filesmodel::fileExists()
//-----------------------------------------------------------------------------
bool FilesModel::filePathExists(QSharedPointer<File> file) const
{
    if (file->name().isEmpty())
    {
        return false;
    }

    QString xmlPath = handler_->getPath(component_->getVlnv());
    QString absFilePath = General::getAbsolutePath(xmlPath, file->name());

    QFileInfo fileInfo(absFilePath);

    return fileInfo.exists();
}

//-----------------------------------------------------------------------------
// Function: filesmodel::isValidURI()
//-----------------------------------------------------------------------------
bool FilesModel::isValidURI(QSharedPointer<File> file) const
{
    return Utils::URL_VALIDITY_REG_EXP.match(file->name()).hasMatch();
}
