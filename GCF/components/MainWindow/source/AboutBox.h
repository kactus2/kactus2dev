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

#ifndef ABOUT_BOX_H
#define ABOUT_BOX_H

#include <GCF/Common.h>
#include <QDialog>

class QListWidgetItem;

namespace GCF
{
	namespace Components
	{
		struct AboutBoxData;
		class GCF_EXPORT_CLASS2 AboutBox : public QDialog
		{
			Q_OBJECT

		public:
			AboutBox(QWidget* parent=0);
			~AboutBox();

		protected slots:
			void currentItemChanged(QListWidgetItem* item);

		protected:
			void init();
			bool eventFilter(QObject* obj, QEvent* event);

		private:
			AboutBoxData* d;
		};
	}
}

#endif
