
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

#ifndef PROJECT_MANAGER_COMPONENT_H
#define PROJECT_MANAGER_COMPONENT_H

#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>

namespace GCF
{
	namespace Components
	{
		struct ProjectManagerComponentData;
		class ProjectManagerComponent : public AbstractComponent, virtual public IAboutData
		{
			Q_OBJECT
			Q_INTERFACES(IAboutData)
			GCF_DECLARE_STD_COMPONENT(ProjectManagerComponent)

		public:
			static ProjectManagerComponent& instance();
			~ProjectManagerComponent();

		protected:
			ProjectManagerComponent();
			void initializeComponent();
			void finalizeComponent();
			QAction* fetchAction(const QString& completeName) const;
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;
			void creationAnnounced();
			void componentCreated(AbstractComponent* comp);
			void componentDestroyed(AbstractComponent* comp);
			void justActivated();

		protected slots:
			void projectModified(bool val);

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


		private:
			ProjectManagerComponentData* d;
		};
	}
}

#endif


