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

#ifndef CLIPBOARD_MODEL_H
#define CLIPBOARD_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include "IClipboard.h"

namespace GCF
{
	namespace Components
	{
		struct ClipboardModelData;
		class ClipboardModel : public QAbstractListModel
		{
			Q_OBJECT

		public:
			ClipboardModel(QObject* parent=0);
			~ClipboardModel();

			void setClipboardDataList(const QList<ClipboardData>& dataList);
			QList<ClipboardData>& clipboardDataList() const;

			int rowCount(const QModelIndex& parentIndex=QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;
			QVariant headerData(int section, Qt::Orientation orient, int role=Qt::DisplayRole) const;

		private:
			ClipboardModelData* d;
		};
	}
}

#endif

