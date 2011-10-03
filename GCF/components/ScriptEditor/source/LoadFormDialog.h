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

#ifndef PARENT_WIDGET_SELECTOR_DIALOG_H
#define PARENT_WIDGET_SELECTOR_DIALOG_H

#include <QDialog>

namespace GCF
{
	namespace Components
	{
		struct LoadFormDialogData;
		class LoadFormDialog : public QDialog
		{
			Q_OBJECT

		public:
			LoadFormDialog(QWidget* parent=0);
			~LoadFormDialog();

			void setWidgetInfo(const QStringList* widgetNames, const QList<QWidget*>* widgetList);

			QString formFileName() const;
			QString parentWidgetName() const;

		protected slots:
			void on_lstWidgetNames_currentRowChanged(int index);
			void on_cmdSelectFormFile_clicked();
			void on_txtParentWidget_textChanged(const QString& text);

		private:
			LoadFormDialogData* d;
		};
	}
}

#endif

