//-----------------------------------------------------------------------------
// File: EnumerationModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing Enumeration within a Choice element.
//-----------------------------------------------------------------------------

#include "EnumerationModel.h"

#include <IPXACTmodels/Enumeration.h>

#include <QColor>

namespace
{
    enum COLUMNS
    {
        ENUMERATION = 0,
        TEXT,
        HELP,
        COLUMN_COUNT
    };
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::EnumerationModel()
//-----------------------------------------------------------------------------
EnumerationModel::EnumerationModel(QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations, 
    QObject *parent)
    : QAbstractTableModel (parent),
    enumerations_(enumerations)
{

}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::~EnumerationModel()
//-----------------------------------------------------------------------------
EnumerationModel::~EnumerationModel()
{
    
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::columnCount()
//-----------------------------------------------------------------------------
int EnumerationModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::rowCount()
//-----------------------------------------------------------------------------
int EnumerationModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    // Additional row for inserting new rows.
    return enumerations_->size() + 1;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::data()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::data(QModelIndex const& index, int role) const
{
    if (isNotValidIndex(index))
    {
        return QVariant();
    }
    
    if (index.column() == ENUMERATION)
    {
        return getEnumerationColumnData(index.row(), role);
    } 
    else if (index.column() == TEXT)
    {
        return getTextColumnData(index.row(), role);
    }
    else if (index.column() == HELP)
    {
        return getHelpColumnData(index.row(), role);
    }
    else
    {
        Q_ASSERT_X(false, "data()", "Undefined column requested.");
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::headerData()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == ENUMERATION)
        {
            return tr("Enumeration");
        }
        else if (section == TEXT)
        {
            return tr("Text");
        } 
        else if (section == HELP)
        {
            return tr("Description");
        } 
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::setData()
//-----------------------------------------------------------------------------
bool EnumerationModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (isNotValidIndex(index))
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        if (isLastRow(index.row()) && !value.toString().isEmpty())
        {
            onAddItem(index);
        }

        if (hasEnumerationOnRow(index.row()))
        {
            if (index.column() == ENUMERATION)
            {
                enumerations_->at(index.row())->setValue(value.toString());
            }
            else if (index.column() == TEXT)
            {
                enumerations_->at(index.row())->setText(value.toString());
            } 
            else if (index.column() == HELP)
            {
                enumerations_->at(index.row())->setHelp(value.toString());
            } 
            else
            {
                Q_ASSERT_X(false, "setData()", "Undefined column requested.");
                return false;
            }
        }        
    } 

    emit dataChanged(index, index);
    return true;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags EnumerationModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags validFlags = QAbstractTableModel::flags(index);
    validFlags |= Qt::ItemIsEditable;
    return validFlags;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::onAddItem()
//-----------------------------------------------------------------------------
void EnumerationModel::onAddItem(QModelIndex const& index)
{
    int row = rowCount();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    enumerations_->insert(row, QSharedPointer<Enumeration>(new Enumeration()));
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::onRemoveItem()
//-----------------------------------------------------------------------------
void EnumerationModel::onRemoveItem(QModelIndex const& index)
{
    int row = index.row();
    if (hasEnumerationOnRow(row))
    {
        beginRemoveRows(QModelIndex(), row, row);
        enumerations_->removeAt(row);
        endRemoveRows();

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::isNotValidIndex()
//-----------------------------------------------------------------------------
bool EnumerationModel::isNotValidIndex(QModelIndex const& index) const
{
    return !index.isValid() ||  index.row() > enumerations_->size() || index.column() >= COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::hasEnumerationOnRow()
//-----------------------------------------------------------------------------
bool EnumerationModel::hasEnumerationOnRow(int row) const
{
    return row < enumerations_->size();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::isLastRow()
//-----------------------------------------------------------------------------
bool EnumerationModel::isLastRow(int row)
{
    return row == enumerations_->size();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::getEnumerationColumnData()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::getEnumerationColumnData(int row, int role) const
{
    if (hasEnumerationOnRow(row))
    { 
        QSharedPointer<Enumeration> enumeration = enumerations_->at(row);
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            return enumeration->getValue();
        }
        else if (role == Qt::ForegroundRole)
        { 
            return getForegroundColorForEnumeration(enumeration);
        }
        else if (role == Qt::BackgroundRole)
        { 
            return QColor("lemonChiffon");
        }
    }
    else
    {
        if (role == Qt::DisplayRole)
        {
            return tr("Add new enumeration");
        }
        else if (role == Qt::ForegroundRole)
        { 
            return QColor("gray");
        }        
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::getForegroundColorForEnumeration()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::getForegroundColorForEnumeration(QSharedPointer<Enumeration> enumeration) const
{
    if (enumeration->getValue().isEmpty())
    {
        return QColor("red");
    }
    else
    {
        return QColor("black");
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::getTextColumnData()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::getTextColumnData(int row, int role) const
{
   if (hasEnumerationOnRow(row))
   {
       QSharedPointer<Enumeration> enumeration = enumerations_->at(row);

       if (role == Qt::DisplayRole || role == Qt::EditRole)
       {
           return enumeration->getText();
       }
       else if (role == Qt::ForegroundRole)
       { 
           return getForegroundColorForEnumeration(enumeration);
       }
   }

   return QVariant();
}

//-----------------------------------------------------------------------------
// Function: EnumerationModel::getHelpColumnData()
//-----------------------------------------------------------------------------
QVariant EnumerationModel::getHelpColumnData(int row, int role) const
{
    if (hasEnumerationOnRow(row))
    {
        QSharedPointer<Enumeration> enumeration = enumerations_->at(row);

        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            return enumeration->getHelp();
        }
        else if (role == Qt::ForegroundRole)
        { 
            return getForegroundColorForEnumeration(enumeration);
        }
    }

    return QVariant();
}
