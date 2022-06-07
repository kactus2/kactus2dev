//-----------------------------------------------------------------------------
// File: filesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 1.6.2012
//
// Description:
// The model that contains the files to edit in files summary editor.
//-----------------------------------------------------------------------------

#include "filesmodel.h"
#include "FileColumns.h"

#include <KactusAPI/include/utils.h>
#include <common/KactusColors.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/FileInterface.h>

#include <QFileInfo>
#include <QMimeData>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: FilesModel::FilesModel()
//-----------------------------------------------------------------------------
FilesModel::FilesModel(LibraryInterface* handler, QSharedPointer<Component> component,
    FileInterface* fileInterface, QObject* parent):
QAbstractTableModel(parent),
handler_(handler),
component_(component),
fileInterface_(fileInterface)
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

    return fileInterface_->itemCount();
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
    if (!index.isValid() || index.row() < 0 || index.row() >= fileInterface_->itemCount())
    {
		return QVariant();
	}

    std::string fileName = fileInterface_->getIndexedItemName(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == FileColumns::NAME_COLUMN)
        {
            QFileInfo fileInfo(QString::fromStdString(fileName));
            QString fileName = fileInfo.fileName();
            if (fileName.isEmpty())
            {
                fileName = tr("unnamed");
            }

            return fileName;            
        }
        else if (index.column() == FileColumns::PATH_COLUMN)
        {
            QString filePath = QString::fromStdString(fileName);
            if (filePath.isEmpty() && role == Qt::DisplayRole)
            {
                filePath = tr("no path or URI defined");
            }

            return filePath;
        }
        else if (index.column() == FileColumns::TYPES_COLUMN)
        {
            return getFileTypesString(fileName);
        }
        else if (index.column() == FileColumns::DESCRIPTION)
        {
            QString fileDescription = QString::fromStdString(fileInterface_->getDescription(fileName));

            if (role == Qt::DisplayRole)
            {
                return fileDescription.replace(
                    QRegularExpression("\n.*$", QRegularExpression::DotMatchesEverythingOption), "...");
            }
            else
            {
                return fileDescription;
            }
        }
	}

	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == FileColumns::NAME_COLUMN)
        {
            if (fileName.empty())
            {
                return KactusColors::ERROR;
            }
            else
            {
                return KactusColors::REGULAR_TEXT;
            }
        }
        if (index.column() == FileColumns::PATH_COLUMN && !(filePathExists(fileName) || isValidURI(fileName)))
        {
            return KactusColors::ERROR;
        }
	}

    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == FileColumns::DESCRIPTION)
        {
            return QString::fromStdString(fileInterface_->getDescription(fileName));
        }
        else if (!filePathExists(fileName) && !isValidURI(fileName))
        {
            return tr("File path is not found or defines an invalid URI.");
        }
    }

	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == FileColumns::PATH_COLUMN || index.column() == FileColumns::TYPES_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filesmodel::getFileTypesString()
//-----------------------------------------------------------------------------
QString FilesModel::getFileTypesString(std::string const& fileName) const
{
    QStringList types;

    for (auto fileType : fileInterface_->getFileTypes(fileName))
    {
        types.append(QString::fromStdString(fileType));
    }

    return types.join("; ");
}

//-----------------------------------------------------------------------------
// Function: FilesModel::setData()
//-----------------------------------------------------------------------------
bool FilesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fileInterface_->itemCount())
    {
		return false;
	}

    std::string fileName = fileInterface_->getIndexedItemName(index.row());

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

            std::string newName = value.toString().toStdString();
            fileInterface_->setName(fileName, newName);

            //! Send information of the new name to the matching file editor...
            emit fileRenamed(fileName, newName);
        }
        else if (index.column() == FileColumns::TYPES_COLUMN)
        {
            QString str = value.toString();
            QStringList fileTypes = str.split("; ", QString::SkipEmptyParts);

            std::vector<std::string> newFileTypes;
            for (auto singleType : fileTypes)
            {
                newFileTypes.push_back(singleType.toStdString());
            }

            fileInterface_->setFileTypes(fileName, newFileTypes);
        }
        else if (index.column() == FileColumns::DESCRIPTION)
        {
            fileInterface_->setDescription(fileName, value.toString().toStdString());
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
    int row = fileInterface_->itemCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);

    fileInterface_->addFile(row, "unknown");

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
    int row = fileInterface_->itemCount();

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
    for (auto fileName : fileInterface_->getItemNames())
    {
        if (QString::fromStdString(fileName) == relPath)
        {
            return;
        }
    }

	beginInsertRows(QModelIndex(), row, row);
    fileInterface_->addFile(row, relPath.toStdString());
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
    if (!index.isValid() || index.row() < 0 || index.row() >= fileInterface_->itemCount())
    {
		return;
	}

    std::string removedFileName = fileInterface_->getIndexedItemName(index.row());

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    fileInterface_->removeFile(removedFileName);

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
	if (!originalPos.isValid() || originalPos == newPos || originalPos.row() < 0 ||
        originalPos.row() >= fileInterface_->itemCount())
    {
		return;
	}

	int source = originalPos.row();
	int target = 0;

	// if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= fileInterface_->itemCount())
    {
		beginResetModel();

        std::string fileName = fileInterface_->getIndexedItemName(originalPos.row());
        target = fileInterface_->itemCount() - 1;
        fileInterface_->swapFiles(originalPos.row(), target);

        endResetModel();
	}
	// if both indexes were valid
	else
    {
		beginResetModel();
        fileInterface_->swapFiles(originalPos.row(), newPos.row());
		target = newPos.row();
		endResetModel();
	}

	emit fileMoved(source, target);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filesmodel::fileExists()
//-----------------------------------------------------------------------------
bool FilesModel::filePathExists(std::string const& fileName) const
{
    if (fileName.empty())
    {
        return false;
    }

    QString xmlPath = handler_->getPath(component_->getVlnv());
    QString absFilePath = General::getAbsolutePath(xmlPath, QString::fromStdString(fileName));

    QFileInfo fileInfo(absFilePath);

    return fileInfo.exists();
}

//-----------------------------------------------------------------------------
// Function: filesmodel::isValidURI()
//-----------------------------------------------------------------------------
bool FilesModel::isValidURI(std::string const& fileName) const
{
    return Utils::URL_VALIDITY_REG_EXP.match(QString::fromStdString(fileName)).hasMatch();
}
