//-----------------------------------------------------------------------------
// File: filesetsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The model class to manage the objects for FileSetsEditor.
//-----------------------------------------------------------------------------

#include "filesetsmodel.h"
#include "FileSetColumns.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <KactusAPI/include/FileSetInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <common/KactusColors.h>


#include <QStringList>
#include <QString>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: FileSetsModel::FileSetsModel()
//-----------------------------------------------------------------------------
FileSetsModel::FileSetsModel(FileSetInterface* fileSetInterface, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
QAbstractTableModel(parent),
parameterFinder_(parameterFinder),
fileSetInterface_(fileSetInterface)
{

}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::rowCount()
//-----------------------------------------------------------------------------
int FileSetsModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
    
    return fileSetInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::columnCount()
//-----------------------------------------------------------------------------
int FileSetsModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/ ) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return FileSetColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileSetsModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileSetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == FileSetColumns::NAME_COLUMN)
        {
            return tr("Name");
        }
        else if (section == FileSetColumns::GROUP_COLUMN)
        {
            return tr("Group identifiers");
        }
        else if (section == FileSetColumns::DESCRIPTION)
        {
            return tr("Description");
        }
	}
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::data()
//-----------------------------------------------------------------------------
QVariant FileSetsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fileSetInterface_->itemCount())
    {
		return QVariant();
	}

    std::string fileSetName = fileSetInterface_->getIndexedItemName(index.row());

    if (index.column() == FileSetColumns::DESCRIPTION && (role == Qt::EditRole || role == Qt::ToolTipRole))
    {
        return QString::fromStdString(fileSetInterface_->getDescription(fileSetName));
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == FileSetColumns::NAME_COLUMN)
        {
            return QString::fromStdString(fileSetName);
        }
        else if (index.column() == FileSetColumns::DESCRIPTION)
        {
            QString fileSetDescription = QString::fromStdString(fileSetInterface_->getDescription(fileSetName));

            return fileSetDescription.replace(
                QRegularExpression("\n.*$", QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else if (index.column() == FileSetColumns::GROUP_COLUMN)
        {
            std::vector<std::string> fileSetGroups = fileSetInterface_->getGroups(fileSetName);

            std::string combinedGroups;
            for (size_t i = 0; i < fileSetGroups.size(); ++i)
            {
                auto const& singleGroup = fileSetGroups.at(i);

                if (i > 0)
                {
                    combinedGroups += " ";
                }

                combinedGroups += singleGroup;
            }

            return QString::fromStdString(combinedGroups);
        }
        else
        {
            return QVariant();
        }
    }
	else if (Qt::BackgroundRole == role)
    {
        if (index.column() == FileSetColumns::NAME_COLUMN)
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
// Function: FileSetsModel::setData()
//-----------------------------------------------------------------------------
bool FileSetsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fileSetInterface_->itemCount())
    {
		return false;
	}

    std::string fileSetName = fileSetInterface_->getIndexedItemName(index.row());

    if (Qt::EditRole == role)
    {
        if (index.column() == FileSetColumns::NAME_COLUMN)
        {
            fileSetInterface_->setName(fileSetName, value.toString().toStdString());
        }
        else if (index.column() == FileSetColumns::DESCRIPTION)
        {
            fileSetInterface_->setDescription(fileSetName, value.toString().toStdString());
        }
        else if(index.column() == FileSetColumns::GROUP_COLUMN)
        {
            QString str = value.toString();
            QStringList groupNames = str.split(' ', Qt::SkipEmptyParts);

            std::vector<std::string> newGroups;
            for (auto singleGroup : groupNames)
            {
                newGroups.push_back(singleGroup.toStdString());
            }

            fileSetInterface_->setGroups(fileSetName, newGroups);
        }
        else
        {
            return false;
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
// Function: FileSetsModel::onAddItem()
//-----------------------------------------------------------------------------
void FileSetsModel::onAddItem(QModelIndex const& index)
{
    int row = fileSetInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);

    fileSetInterface_->addFileSet(row);

	endInsertRows();

	// inform navigation tree that file set is added
	emit fileSetAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FileSetsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= fileSetInterface_->itemCount())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    std::string removedName = fileSetInterface_->getIndexedItemName(index.row());

    decreaseReferencesWithRemovedFileSet(removedName);

    fileSetInterface_->removeFileSet(removedName);

    endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fileSetRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filesetsmodel::decreaseReferencesWithRemovedFileSet()
//-----------------------------------------------------------------------------
void FileSetsModel::decreaseReferencesWithRemovedFileSet(std::string const& fileSetName)
{
    QStringList expressionList;
    std::vector<std::string> selectedFileSetNames;
    selectedFileSetNames.push_back(fileSetName);

    for (auto expression : fileSetInterface_->getExpressionsInSelectedFileSets(selectedFileSetNames))
    {
        expressionList.append(QString::fromStdString(expression));
    }

    ReferenceCalculator referenceCalculator(parameterFinder_);
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);
    foreach (QString referencedID, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedID); ++i)
        {
            emit decreaseReferences(referencedID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::refresh()
//-----------------------------------------------------------------------------
void FileSetsModel::refresh()
{
    beginResetModel();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: FileSetsModel::onFileSetAdded()
//-----------------------------------------------------------------------------
void FileSetsModel::onFileSetAdded(FileSet* fileSet)
{
    beginResetModel();
    endResetModel();

    // Find out the corresponding index and signal fileSetAdded().
    int fileSetIndex = fileSetInterface_->getItemIndex(fileSet->name().toStdString());
    if (fileSetIndex >= 0)
    {
        emit fileSetAdded(fileSetIndex);
        emit contentChanged();
    }
}
