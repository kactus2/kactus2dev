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

#ifndef HELP_SYSTEM_CONFIGURATION_PAGE_H
#define HELP_SYSTEM_CONFIGURATION_PAGE_H

#include <QWidget>

namespace GCF
{
	namespace Components
	{
		class HelpSystemComponent;
		struct HelpSystemConfigurationPageData;
		class HelpSystemConfigurationPage : public QWidget
		{
			Q_OBJECT

		public:
			HelpSystemConfigurationPage(QWidget* parent=0);
			~HelpSystemConfigurationPage();

			void initFrom(HelpSystemComponent* helpSystemComponent);
			void saveInto(HelpSystemComponent* helpSystemComponent);

		public slots:
			void on_cmdRemove_clicked();
			void on_cmdAdd_clicked();


		private:
			HelpSystemConfigurationPageData* d;
		};
	}
}

#endif

