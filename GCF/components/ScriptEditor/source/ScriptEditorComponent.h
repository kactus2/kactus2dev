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

#ifndef SCRIPT_EDITOR_COMPONENT_H
#define SCRIPT_EDITOR_COMPONENT_H

#include <GCF/AbstractComponent.h>
#include <GCF/IAboutData.h>
#include <GCF/IConfigurationDialogPageProvider.h>
#include "IProjectPart.h"

#include <QIcon>

class QIODevice;

namespace GCF
{
	namespace Components
	{
		class ScriptEngineManager;

		struct ScriptEditorComponentData;
		class ScriptEditorComponent : public AbstractComponent,
			virtual public IProjectPart, virtual public IAboutData,
			virtual public IConfigurationDialogPageProvider
		{
			Q_OBJECT
			Q_INTERFACES(IProjectPart IAboutData IConfigurationDialogPageProvider)
			GCF_DECLARE_STD_COMPONENT(ScriptEditorComponent)

		public:
			static ScriptEditorComponent& instance();
			~ScriptEditorComponent();

			void addScriptableObject(QObject* object, QString name, bool excludeChildren=true);
			void removeScriptableObject(QObject* object);
			void removeScriptableObject(QString name);

			ScriptEngineManager* scriptEngineManager();

			Q_INVOKABLE QWidget* loadForm(QString uiFile);
			Q_INVOKABLE QWidget* loadForm(QString uiFile, QString parentName);
			Q_INVOKABLE QWidget* loadForm(QString uiFile, QString parentName, QString hint);
			Q_INVOKABLE bool unloadForm(QString formName);
			Q_INVOKABLE bool unloadForm(QWidget* widget);
			QWidget* loadForm(QIODevice* uiFileDevice, QString uiFile, QString parentName, QString hint);

			Q_INVOKABLE QString getSaveFileName(QString caption=QString(), QString filter=QString(), QString directory=QString());
			Q_INVOKABLE QString getOpenFileName(QString caption=QString(), QString filter=QString(), QString directory=QString());
			Q_INVOKABLE void showMessageBox(QString message, QString title);
			Q_INVOKABLE QObject* addAction(QString actionName, QString text, QString iconName=QString(), int rowSpan=1, int colSpan=2);
			Q_INVOKABLE bool removeAction(QObject* actionObject);
			Q_INVOKABLE bool removeAction(QString actionName);

		public slots:
			void loadForm();
			void unloadForm();
			void showScriptEditor();
			void showScriptEditor(QString objectName, QString signal, QString eventName);
			void highlightScriptableObject(QObject* object);
			void executeMainScript();

		protected:
			ScriptEditorComponent();

			// Implemented from AbstractComponent.
			void initializeComponent();
			void finalizeComponent();
			void creationAnnounced();
			void componentCreated(AbstractComponent* comp);
			void componentDestroyed(AbstractComponent* comp);
			QObject* fetchObject(const QString& completeName) const;
			QWidget* fetchWidget(const QString& completeName) const;

			// Custom methods.
			void gatherScriptableObjectsFrom(AbstractComponent* comp);
			void releaseScriptableObjectsFrom(AbstractComponent* comp);

		protected slots:
			void on_workspaceWidgetActivated(QWidget* widget);
			void on_scriptableObjectExplorer_objectDoubleClicked(QObject* object, QString objectName);
			void on_scriptableObjectExplorer_signalDoubleClicked(QObject* object, QString objectName, QString event, QString signal);
			void on_scriptableObjectExplorer_propertyDoubleClicked(QObject* object, QString objectName, QString method, QString signature);
			void on_scriptableObjectExplorer_methodDoubleClicked(QObject* object, QString objectName, QString property, QString dataType);
			void on_scriptEngineManager_scriptError(QString msg, int lineNr);
			void on_formDestroyed(QObject* obj);

		public:
			// IContainer implementation
			QObject* containerObject();

			// IProjectPart implemnetation
			QString projectPartID();
			bool newProjectPart();
			bool loadProjectPart(QDomDocument doc, QDomElement partE);
			bool initializeProjectPart();
			bool saveProjectPart(QDomDocument doc, QDomElement partParentE);
			bool closeProjectPart();

		signals:
			void projectPartModified();

		public:
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

			// IConfigurationDialogPageProvider implementation
			QString pageTitle() const;
			QIcon pageIcon() const;
			QWidget* createPage(QWidget* parent);
			void savePage(QWidget* page);

		private:
			ScriptEditorComponentData* d;
		};
	}
}

#endif
