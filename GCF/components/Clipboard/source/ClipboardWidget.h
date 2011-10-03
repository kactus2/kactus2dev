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

#ifndef CLIPBOARD_WIDGET_H
#define CLIPBOARD_WIDGET_H

#include <QWidget>
#include "IClipboardWidget.h"

class IClipboard;
class QItemSelection;
class QModelIndex;

namespace GCF
{
	namespace Components
	{
		struct ClipboardWidgetData;
		class ClipboardWidget : public QWidget, virtual public IClipboardWidget
		{
			Q_OBJECT
			Q_INTERFACES(IClipboardWidget)

		public:
			ClipboardWidget(IClipboard* clipboard, QWidget* parent=0);
			~ClipboardWidget();

			// IContainer implementation
			QObject* containerObject();

			// IClipboardWidget implementation
			IClipboard* clipboard() const;

		signals:
			void dataClicked(const QString& context, const ClipboardData& data, int dataIndex);
			void dataDoubleClicked(const QString& context, const ClipboardData& data, int dataIndex);
			void dataRightClicked(const QString& context, const ClipboardData& data, int dataIndex, QMenu* menu);

		protected slots:
			void on_cmdRemoveSelected_clicked();
			void on_cmdClearAll_clicked();
			void on_listView_clicked(const QModelIndex& index);
			void on_listView_doubleClicked(const QModelIndex& index);
			void selectionChanged(const QItemSelection& newSelection);

		protected:
			bool eventFilter(QObject* obj, QEvent* event);

		private:
			ClipboardWidgetData* d;
		};
	}
}

#endif
