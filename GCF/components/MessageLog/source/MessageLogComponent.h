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

#ifndef MESSAGE_LOG_COMPONENT_H
#define MESSAGE_LOG_COMPONENT_H

#include <GCF/AbstractComponent.h>

namespace GCF
{
	namespace Components
	{
		struct MessageLogComponentData;
		class MessageLogComponent : public AbstractComponent
		{
			Q_OBJECT
			GCF_DECLARE_STD_COMPONENT(MessageLogComponent)

		public:
			static MessageLogComponent & instance();
			~MessageLogComponent();

		public slots:
			void debugMessage(const QString& msg);
			void warningMessage(const QString& msg);
			void criticalMessage(const QString& msg);
			void fatalMessage(const QString& msg);

		protected:
			MessageLogComponent();
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;
			void addChildWidget(QWidget* parent, QWidget* child, const QString& hint);
			void justActivated();
			void initializeComponent();
			void finalizeComponent();

		private:
			MessageLogComponentData* d;
		};
	}
}

#endif

