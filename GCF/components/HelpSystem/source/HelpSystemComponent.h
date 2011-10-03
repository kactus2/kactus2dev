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

#ifndef HELP_SYSTEM_COMPONENT_H
#define HELP_SYSTEM_COMPONENT_H

#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>
#include "IHelpSystem.h"
#include <GCF/IConfigurationDialogPageProvider.h>

class QHelpEngine;
class QModelIndex;

namespace GCF
{
	namespace Components
	{
		class HelpView;
		struct HelpSystemComponentData;
		class HelpSystemComponent : public AbstractComponent, virtual public IAboutData,
									 virtual public IHelpSystem,
									 virtual public IConfigurationDialogPageProvider
		{
			Q_OBJECT
			Q_INTERFACES(IAboutData IHelpSystem IConfigurationDialogPageProvider)
			Q_PROPERTY(QString HelpCollectionFile READ helpCollectionFile WRITE setHelpCollectionFile)
			GCF_DECLARE_STD_COMPONENT(HelpSystemComponent)

		public:
			static HelpSystemComponent & instance();
			~HelpSystemComponent();

			HelpView* helpView() const;

		protected:
			HelpSystemComponent();
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;
			void addChildWidget(QWidget* parent, QWidget* child, const QString& hint);
			void removeChildWidget(QWidget* parent, QWidget* child, const QString& hint);
			void initializeComponent();
			void finalizeComponent();
			void creationAnnounced();

			bool eventFilter(QObject *obj, QEvent *event);

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

			// IHelpSystem implementation
			void setHelpCollectionFile(const QString& cf);
			QString helpCollectionFile() const;
			void registerDocumentation(const QString& qchFile);
			void unregisterDocumentation(const QString& namespaceName);
			QStringList registeredDocumentations();
			void showDocumentation(const QUrl& url);
			QUrl currentDocumentationUrl() const;
			void showDocumentationForKeyword(const QString& keyword);
			QHelpEngine* helpEngine() const;

			// IConfigurationDialogPageProvider implementation
			QString pageTitle() const;
			QIcon pageIcon() const;
			QWidget* createPage(QWidget* parent);
			void savePage(QWidget* page);

		protected slots:
			void slotBeginSearch();
			void slotContentItemClicked(const QModelIndex& index);

		private:
			HelpSystemComponentData* d;
		};
	}
}

#endif

