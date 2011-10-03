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

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include "IProject.h"
#include "IProjectExt.h"

class IXmlUtils;
class QMenu;

namespace GCF
{
	namespace Components
	{
		class ProjectManagerComponent;

		struct ProjectData;
		class Project : public QObject, virtual public IProject, virtual public IProjectExt
		{
			Q_OBJECT
			Q_PROPERTY(QString DefaultProjectPath READ defaultProjectPath WRITE setDefaultProjectPath)
			Q_PROPERTY(QString ProjectRootTagName READ projectRootTagName WRITE setProjectRootTagName)
			Q_PROPERTY(QString ProjectFileSuffix READ projectFileSuffix WRITE setProjectFileSuffix)
			Q_INTERFACES(IProject IProjectExt)

		public:
			Project(QObject* parent=0);
			~Project();

			void setXmlUtils(IXmlUtils* utils);
			IXmlUtils* xmlUtils() const;

			void setDefaultProjectPath(QString path);
			void setProjectRootTagName(QString tagName);
			void setProjectFileSuffix(QString suffix);
			QMenu* recentFilesMenu() const;

			// IContainer implementation
			QObject* containerObject();

			// IProject implementation
			bool newProject();
			bool loadProject(QString projectFile);
			bool saveProject(QString projectFile);
			bool closeProject();
			bool isModified();
			void addProjectPart(IProjectPart* part);
			void removeProjectPart(IProjectPart* part);
			int projectPartCount();
			IProjectPart* projectPart(int index);

		signals:
			void projectModified(bool val);

		public:
			// IProjectExt implementation
			QString currentProjectFileName() const;
			QString currentProjectFilePath() const;
			QStringList recentFilesList() const;
			QString defaultProjectPath() const;
			QString projectRootTagName() const;
			QString projectFileSuffix() const;

		signals:
			void currentProjectFileNameChanged(const QString& newName);
            void projectLoadBegin();
            void projectLoadEnd();
            void projectCleared();

		public slots: // custom slots.
			void slotNewProject();
			void slotLoadProject();
			void slotSaveProject();
			void slotCloseProject();
			void slotSaveProjectAs();
			void addRecentFile(QString recentFile);

		protected slots:
			void slotProjectPartModified();
			void slotOpenRecentFile();

		private:
			void setRecentFileListUnModified();
			bool isRecentFileListModified() const;

		protected:
			bool eventFilter(QObject* obj, QEvent* event);

		private:
			friend class ProjectManagerComponent;
			ProjectData* d;
		};
	}
}

#endif
