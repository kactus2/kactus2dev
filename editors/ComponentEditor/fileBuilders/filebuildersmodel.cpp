//-----------------------------------------------------------------------------
// File: filebuildersmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.05.2012
//
// Description:
// Model that contains the items to edit file builders.
//-----------------------------------------------------------------------------

#include "filebuildersmodel.h"
#include "FileBuilderColumns.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <KactusAPI/include/FileBuilderInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/common/FileBuilder.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::FileBuildersModel()
//-----------------------------------------------------------------------------
FileBuildersModel::FileBuildersModel(FileBuilderInterface* builderInterface,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QObject* parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
fileBuilderInterface_(builderInterface)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::~FileBuildersModel()
//-----------------------------------------------------------------------------
FileBuildersModel::~FileBuildersModel()
{

}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::rowCount()
//-----------------------------------------------------------------------------
int FileBuildersModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return fileBuilderInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::columnCount()
//-----------------------------------------------------------------------------
int FileBuildersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return FileBuilderColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::data()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

	// if row is invalid
    else if (index.row() < 0 || index.row() >= fileBuilderInterface_->itemCount())
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
        }
        else
        {
            return valueForIndex(index);
        }
	}
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
	else if (Qt::BackgroundRole == role)
    {
		if (index.column() == 0)
        {
			return KactusColors::MANDATORY_FIELD;
		}
		else
        {
			return KactusColors::REGULAR_FIELD;
		}
	}
	else if (Qt::ForegroundRole == role)
    {
// 		if (fileBuilders_->at(index.row())->isValid())
//         {
			return KactusColors::REGULAR_TEXT;
// 		}
// 		else
//         {
// 			return KactusColors::ERROR;
// 		}
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::headerData()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ )
    const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
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
            QString replaceFlags = tr("Replace default flags") + getExpressionSymbol();
            return replaceFlags;
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::setData()
//-----------------------------------------------------------------------------
bool FileBuildersModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole */ )
{
	if (!index.isValid())
    {
		return false;
	}

	// if row is invalid
    else if (index.row() < 0 || index.row() >= fileBuilderInterface_->itemCount())
    {
		return false;
	}

	if (Qt::EditRole == role)
    {
        std::string builderType = fileBuilderInterface_->getIndexedFileType(index.row());

        if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
        {
            fileBuilderInterface_->setFileType(builderType, value.toString().toStdString());
        }
        else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
        {
            fileBuilderInterface_->setCommand(builderType, value.toString().toStdString());
        }
        else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
        {
            fileBuilderInterface_->setFlags(builderType, value.toString().toStdString());
        }
        else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(fileBuilderInterface_->getReplaceDefaultFlagsExpression(builderType)));
            }
            fileBuilderInterface_->setReplaceDefaultFlags(builderType, value.toString().toStdString());
        }

		emit contentChanged();
		emit dataChanged(index, index);
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileBuildersModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

	return flags;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::isValid()
//-----------------------------------------------------------------------------
bool FileBuildersModel::isValid() const
{
	// check all file builders
/*
	foreach (QSharedPointer<FileBuilder> fileBuilder, *fileBuilders_)
    {
		// if one is invalid
// 		if (!fileBuilder->isValid()) {
// 			return false;
// 		}
	}
*/

	return true;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void FileBuildersModel::onRemoveItem( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid())
    {
		return;
	}
	// make sure the row number if valid
    else if (index.row() < 0 || index.row() >= fileBuilderInterface_->itemCount())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesInItemOnRow(index.row());
    fileBuilderInterface_->removeFileBuilder(fileBuilderInterface_->getIndexedFileType(index.row()));

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::onAddItem()
//-----------------------------------------------------------------------------
void FileBuildersModel::onAddItem( const QModelIndex& index )
{
    int row = fileBuilderInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);

    fileBuilderInterface_->addFileBuilder(row);

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool FileBuildersModel::isValidExpressionColumn(QModelIndex const& index) const
{
    if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuildersModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
        std::string fileType = fileBuilderInterface_->getIndexedFileType(index.row());
        return QString::fromStdString(fileBuilderInterface_->getReplaceDefaultFlagsFormattedExpression(fileType));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: FileBuildersModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::expressionForIndex(QModelIndex const& index) const
{
    if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
        std::string fileType = fileBuilderInterface_->getIndexedFileType(index.row());
        return QString::fromStdString(fileBuilderInterface_->getReplaceDefaultFlagsExpression(fileType));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: FileBuildersModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant FileBuildersModel::valueForIndex(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    std::string builderType = fileBuilderInterface_->getIndexedFileType(index.row());

    if (index.column() == FileBuilderColumns::FILETYPE_COLUMN)
    {
        return QString::fromStdString(builderType);
    }
    else if (index.column() == FileBuilderColumns::COMMAND_COLUMN)
    {
        return QString::fromStdString(fileBuilderInterface_->getCommand(builderType));
    }
    else if (index.column() == FileBuilderColumns::FLAGS_COLUMN)
    {
        return QString::fromStdString(fileBuilderInterface_->getFlags(builderType));
    }
    else if (index.column() == FileBuilderColumns::REPLACE_DEFAULT_COLUMN)
    {
        return QString::fromStdString(fileBuilderInterface_->getReplaceDefaultFlagsValue(builderType));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::validateIndex()
//-----------------------------------------------------------------------------
bool FileBuildersModel::validateIndex(QModelIndex const& /*index*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: filebuildersmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int FileBuildersModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return fileBuilderInterface_->getAllReferencesToIdInItem(
        fileBuilderInterface_->getIndexedFileType(row), valueID.toStdString());
}
