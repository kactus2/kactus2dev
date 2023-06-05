//-----------------------------------------------------------------------------
// File: TagCompleterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2019
//
// Description:
// Model for tag completer.
//-----------------------------------------------------------------------------

#include "TagCompleterModel.h"

#include <QColor>

//-----------------------------------------------------------------------------
// Function: TagCompleterModel::TagCompleterModel()
//-----------------------------------------------------------------------------
TagCompleterModel::TagCompleterModel(QVector<TagData> tagData, QObject* parent):
QAbstractListModel(parent),
tags_(tagData)
{

}

//-----------------------------------------------------------------------------
// Function: TagCompleterModel::rowCount()
//-----------------------------------------------------------------------------
int TagCompleterModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return tags_.size();
}

//-----------------------------------------------------------------------------
// Function: TagCompleterModel::data()
//-----------------------------------------------------------------------------
QVariant TagCompleterModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        QString tagName = tags_.at(index.row()).name_;
        return tagName;
    }
    else if (role == Qt::DecorationRole)
    {
        QColor tagColor(tags_.at(index.row()).color_);
        return tagColor;
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: TagCompleterModel::itemSelected()
//-----------------------------------------------------------------------------
void TagCompleterModel::itemSelected(QModelIndex const& index)
{
    QVariant colorVariant = index.data(Qt::DecorationRole);
    if (colorVariant.canConvert<QColor>())
    {
        QColor tagColor = colorVariant.value<QColor>();
        emit selectedColor(tagColor);
    }
}

//-----------------------------------------------------------------------------
// Function: TagCompleterModel::unfilteredItemSelected()
//-----------------------------------------------------------------------------
void TagCompleterModel::unfilteredItemSelected(int itemIndex)
{
    TagData indexedTag = tags_.at(itemIndex);

    QColor tagColor = QColor(indexedTag.color_);
    emit selectedColor(tagColor);
}
