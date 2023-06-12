//-----------------------------------------------------------------------------
// File: PeripheralTemplateModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// The model to manage the peripheral templates.
//-----------------------------------------------------------------------------

#include "PeripheralTemplateModel.h"

#include <common/KactusColors.h>

#include <QColor>
#include <QCoreApplication>
#include <QFile>
#include <QIcon>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeColumns.h>

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::PeripheralTemplateModel()
//-----------------------------------------------------------------------------
PeripheralTemplateModel::PeripheralTemplateModel(QObject *parent):
QAbstractTableModel(parent)
{
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::setupPeripherals()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate>> newTemplates)
{
    beginResetModel();

    templates_.clear();
    templates_ = newTemplates;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::getTemplates()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > PeripheralTemplateModel::getTemplates() const
{
    return templates_;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::rowCount()
//-----------------------------------------------------------------------------
int PeripheralTemplateModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return static_cast<int>(templates_.count());
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::columnCount()
//-----------------------------------------------------------------------------
int PeripheralTemplateModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return PeripheralTemplateColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PeripheralTemplateModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable |= Qt::ItemIsEditable;
    return flags;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::headerData()
//-----------------------------------------------------------------------------
QVariant PeripheralTemplateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == PeripheralTemplateColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == PeripheralTemplateColumns::TEMPLATE)
        {
            return tr("Template path");
        }
        else if (section == PeripheralTemplateColumns::PATH)
        {
            return tr("File path");
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::data()
//-----------------------------------------------------------------------------
QVariant PeripheralTemplateModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= templates_.size())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tooltipForIndex(index);
    }
    else if (role == Qt::ForegroundRole)
    {
        return blackForValidRedForInvalidIndex(index);
    }
    else if (role == Qt::DecorationRole &&
        (index.column() == PeripheralTemplateColumns::TEMPLATE || index.column() == PeripheralTemplateColumns::PATH))
    {
        return QIcon(QString(":/icons/common/graphics/opened-folder.png"));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::setData()
//-----------------------------------------------------------------------------
bool PeripheralTemplateModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= templates_.size())
    {
		return false;
	}

    QSharedPointer<RenodeStructs::peripheralTemplate> indexedTemplate = templates_.at(index.row());
    if (Qt::EditRole == role)
    {
        if (index.column() == PeripheralTemplateColumns::NAME)
        {
            indexedTemplate->identifier_ = value.toString();
        }
        else if (index.column() == PeripheralTemplateColumns::TEMPLATE)
        {
            indexedTemplate->template_ = value.toString();
        }
        else if (index.column() == PeripheralTemplateColumns::PATH)
        {
            indexedTemplate->path_ = value.toString();
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant PeripheralTemplateModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RenodeStructs::peripheralTemplate> indexedTemplate = templates_.at(index.row());

	if (index.column() == PeripheralTemplateColumns::NAME)
	{
		return indexedTemplate->identifier_;
	}
	else if (index.column() == PeripheralTemplateColumns::TEMPLATE)
	{
		return indexedTemplate->template_;
	}
    else if (index.column() == PeripheralTemplateColumns::PATH)
    {
        return indexedTemplate->path_;
    }
   
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QVariant PeripheralTemplateModel::tooltipForIndex(QModelIndex const& index) const
{
	if (index.column() == PeripheralTemplateColumns::NAME)
	{
        return QString("Name of the template");
    }
	else if (index.column() == PeripheralTemplateColumns::TEMPLATE)
	{
        return QString("The template python file path relative to the installation folder.\n" + data(index).toString());
	}
	else if (index.column() == PeripheralTemplateColumns::PATH)
	{
		return QString("The folder path for the created python file relative to the generation destination folder.\n" + data(index).toString());
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::blackForValidRedForInvalidIndex()
//-----------------------------------------------------------------------------
QVariant PeripheralTemplateModel::blackForValidRedForInvalidIndex(QModelIndex const& index) const
{
    if (index.column() == PeripheralTemplateColumns::TEMPLATE)
    {
        QString templatePath = QCoreApplication::applicationDirPath() + "/" + index.data().toString();
        if (!QFile::exists(templatePath))
        {
            return KactusColors::ERROR;
        }
    }

    return KactusColors::REGULAR_TEXT;
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::onAddRow()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::onAddRow()
{
	int lastRow = static_cast<int>(templates_.count());
    addItem(lastRow);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::addItem()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::addItem(int newItemRow)
{
	beginInsertRows(QModelIndex(), newItemRow, newItemRow);

	QSharedPointer<RenodeStructs::peripheralTemplate> newTemplate(new RenodeStructs::peripheralTemplate());
	templates_.insert(newItemRow, newTemplate);

	endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::onAddItem()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::onAddItem(const QModelIndex& index)
{
	int row = static_cast<int>(templates_.count());
	if (index.isValid())
	{
		row = index.row();
	}

    addItem(row);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::onRemoveItem(const QModelIndex& index)
{
    onRemoveRow(index.row());
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PeripheralTemplateModel::onRemoveRow(int row)
{
	if (row < 0 || row >= templates_.count())
	{
		return;
	}

	beginRemoveRows(QModelIndex(), row, row);

    templates_.removeAt(row);

	endRemoveRows();
}
