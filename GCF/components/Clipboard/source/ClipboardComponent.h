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

#ifndef CLIPBOARD_COMPONENT_H
#define CLIPBOARD_COMPONENT_H

#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>
#include "IClipboard.h"

namespace GCF
{
	namespace Components
	{
		struct ClipboardComponentData;
		class ClipboardComponent : public AbstractComponent, virtual public IAboutData,
									virtual public IClipboard
		{
			Q_OBJECT
			Q_INTERFACES(IAboutData IClipboard)
			GCF_DECLARE_STD_COMPONENT(ClipboardComponent)

		public:
			static ClipboardComponent & instance();
			~ClipboardComponent();

		protected:
			ClipboardComponent();
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;
			void initializeComponent();
			void finalizeComponent();
			void creationAnnounced();

		public:
			// IContainer implementation
			QObject* containerObject();

			// IAboutData implementation
			QString productName() const;
			QString organization() const;
			QImage programLogo() const;
			QString version() const;
			QString shortDescription() const;
			QString homepage() const;
			QString bugAddress() const;
			const QList<AboutPerson> authors() const;
			const QList<AboutPerson> credits() const;
			QString license() const;
			QString copyrightStatement() const;

			// IClipboard implementation
			void addContext(const QString& contextName);
			void removeContext(const QString& contextName);
			QString context(int index) const;
			int contextCount() const;
			void activateContext(const QString& name);
			void activateContext(int contextIndex);
			void activateDefaultContext();
			QString activeContextName() const;
			int activeContextIndex() const;
			bool isDefaultContextActive() const;
			void addData(const ClipboardData& data);
			bool removeDataAt(int index);
			bool removeAll();
			int dataCount() const;
			ClipboardData data(int index) const;
			QAbstractListModel* activeClipboardModel() const;

		signals:
			void activeContextChanged(const QString& ctxName);
			void contextAdded(const QString& name);
			void contextRemoved(const QString& name);
			void dataAdded(const ClipboardData& data);
			void dataRemoved(int index);
			void dataCleared();

		private:
			ClipboardComponentData* d;
		};
	}
}

#endif

