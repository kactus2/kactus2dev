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

#ifndef TOPIC_CHOOSER_DIALOG_H
#define TOPIC_CHOOSER_DIALOG_H

#include <QDialog>

class QString;
class QUrl;

namespace GCF
{
	namespace Components
	{
		struct TopicChooserDialogData;
		class TopicChooserDialog: public QDialog
		{
			Q_OBJECT
		public:
			TopicChooserDialog(QWidget * parent = 0);
			~TopicChooserDialog();

			void showTopics(const QMap<QString, QUrl> & links, const QString & keyword);

		public slots:
			void slotShowUrl();

		signals:
			void topicSelected(const QUrl& url);

		private:
			TopicChooserDialogData* d;
		};
	}
}

#endif
