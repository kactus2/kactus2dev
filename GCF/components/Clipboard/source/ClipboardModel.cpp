/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ClipboardModel.h"

struct GCF::Components::ClipboardModelData
{
    QList<ClipboardData> dataList;
};

GCF::Components::ClipboardModel::ClipboardModel(QObject* parent)
:QAbstractListModel(parent)
{
    d = new GCF::Components::ClipboardModelData;
}

GCF::Components::ClipboardModel::~ClipboardModel()
{
    delete d;
}

void GCF::Components::ClipboardModel::setClipboardDataList(const QList<ClipboardData>& dataList)
{
    if(d->dataList.count())
    {
		beginRemoveRows(QModelIndex(), 0, d->dataList.count());
        removeRows(0, d->dataList.count());
        endRemoveRows();
    }

    d->dataList = dataList;

    if(d->dataList.count())
    {
		beginInsertRows(QModelIndex(), 0, d->dataList.count());
        insertRows(0, d->dataList.count());
        endInsertRows();
    }
}

QList<ClipboardData>& GCF::Components::ClipboardModel::clipboardDataList() const
{
    return d->dataList;
}

int GCF::Components::ClipboardModel::rowCount(const QModelIndex& parentIndex) const
{
    if(parentIndex.isValid())
        return 0;

    return d->dataList.count();
}

QVariant GCF::Components::ClipboardModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= d->dataList.count())
        return QVariant();

    ClipboardData data = d->dataList[index.row()];

    switch(role)
    {
    case Qt::DisplayRole:
        return data.Description;
    case Qt::ToolTipRole:
		if(data.Data.type() == QVariant::String)
			return data.Data.toString().left(50);
		return QString("%1").arg(data.Data.typeName());
    case Qt::DecorationRole:
        return data.Icon;
    default:
        break;
    }

    return QVariant();
}

QVariant GCF::Components::ClipboardModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if(section)
        return QVariant();

    if(orient != Qt::Horizontal)
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    return "Clipboard Data";
}

