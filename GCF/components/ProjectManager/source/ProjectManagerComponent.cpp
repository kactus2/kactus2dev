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

#include "ProjectManagerComponent.h"
#include <GCF/ComponentFactory>
#include <GCF/IComponentPlugin>
#include <GCF/ComponentSettings>

#include "Project.h"
#include "IProjectPart.h"
#include <QMainWindow>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QFile>

struct GCF::Components::ProjectManagerComponentData
{
    ProjectManagerComponentData() : recentFilesMenu(0) { }

    GCF::Components::Project visProject;
    QMenu* recentFilesMenu;

    QString formatRecentFileName(const QString& name);
};

GCF_DEFINE_STD_COMPONENT(ProjectManagerComponent)

GCF::Components::ProjectManagerComponent& GCF::Components::ProjectManagerComponent::instance()
{
    static GCF::Components::ProjectManagerComponent* theInstance = GCF_CREATE_COMPONENT(ProjectManagerComponent);
    return *theInstance;
}

GCF::Components::ProjectManagerComponent::ProjectManagerComponent()
{
    d = new GCF::Components::ProjectManagerComponentData;
}

GCF::Components::ProjectManagerComponent::~ProjectManagerComponent()
{
    delete d;
}

void GCF::Components::ProjectManagerComponent::initializeComponent()
{
    // Do nothing
}

void GCF::Components::ProjectManagerComponent::finalizeComponent()
{
    if(!d->visProject.isRecentFileListModified())
        return;

    // Store the file names into the settings object
    QStringList rfList = d->visProject.recentFilesList();
    settings()["RecentFiles"] = rfList;
}

QAction* GCF::Components::ProjectManagerComponent::fetchAction(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "loadProject")
        return d->visProject.recentFilesMenu()->menuAction();

    return GCF::AbstractComponent::fetchAction(completeName);
}

QObject* GCF::Components::ProjectManagerComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::Components::ProjectManagerComponent* that = const_cast<GCF::Components::ProjectManagerComponent*>(this);

    if(comps.last() == "controller")
        return that;

    if(comps.last() == "visProject")
        return &d->visProject;

    return 0;
}

QWidget* GCF::Components::ProjectManagerComponent::fetchWidget(const QString& completeName) const
{
    Q_UNUSED(completeName);
    return 0;
}

void GCF::Components::ProjectManagerComponent::creationAnnounced()
{
    // Find out all objects that implement IProjectPart and register them with
    // the project object owned by this component.
    QStringList compNames;
    QList<QObject*> projectParts = findObjects("IProjectPart", compNames);
    for(int i=0; i<projectParts.count(); i++)
    {
        IProjectPart* projectPart = qobject_cast<IProjectPart*>(projectParts[i]);
        d->visProject.addProjectPart(projectPart);
    }

    QStringList rfList = settings()["RecentFiles"].toStringList();
    for(int i=0; i<rfList.count(); i++)
        d->visProject.addRecentFile( rfList[i] );

    connect(&d->visProject, SIGNAL(projectModified(bool)), this, SLOT(projectModified(bool)));
    projectModified(false);

    d->visProject.setRecentFileListUnModified();
}

void GCF::Components::ProjectManagerComponent::componentCreated(GCF::AbstractComponent* comp)
{
    QString ifaceName = QString("%1.IProjectPart").arg(comp->componentName());
    QStringList compNames;
    QList<QObject*> projectParts = findObjects(ifaceName, compNames);
    for(int i=0; i<projectParts.count(); i++)
    {
        IProjectPart* projectPart = qobject_cast<IProjectPart*>(projectParts[i]);
        d->visProject.addProjectPart(projectPart);
    }

    if(comp->componentName() == "ComponentLoaderComponent")
    {
        // all components have just been loaded. We ensure that the modified flag is set as false now
        d->visProject.newProject();
    }
}

void GCF::Components::ProjectManagerComponent::componentDestroyed(GCF::AbstractComponent* comp)
{
    QString ifaceName = QString("%1.IProjectPart").arg(comp->componentName());
    QStringList compNames;
    QList<QObject*> projectParts = findObjects(ifaceName, compNames);
    for(int i=0; i<projectParts.count(); i++)
    {
        IProjectPart* projectPart = qobject_cast<IProjectPart*>(projectParts[i]);
        d->visProject.removeProjectPart(projectPart);
    }
}

void GCF::Components::ProjectManagerComponent::justActivated()
{
    projectModified(false); // ###: FIXME
}

void GCF::Components::ProjectManagerComponent::projectModified(bool val)
{
    if( !this->isActive() )
        return;

    QString title;
    if(d->visProject.currentProjectFileName().isEmpty())
        title = QString("%1[*] - %2").arg("NONAME").arg(qApp->applicationName());
    else
        title = QString("%1[*] - %2").arg(d->visProject.currentProjectFileName()).arg(qApp->applicationName());

    GCF::AbstractComponent::componentHostWidget()->setWindowTitle(title);
    GCF::AbstractComponent::componentHostWidget()->setWindowModified(val);
}

QObject* GCF::Components::ProjectManagerComponent::containerObject()
{
    return this;
}

QString GCF::Components::ProjectManagerComponent::productName() const
{
    return "Project Management System";
}

QString GCF::Components::ProjectManagerComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::ProjectManagerComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::ProjectManagerComponent::version() const
{
    return "1.0";
}

QString GCF::Components::ProjectManagerComponent::shortDescription() const
{
    return "This component provides the basic functionality required to manage (as in "
           "load and save) projects.";
}

QString GCF::Components::ProjectManagerComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::ProjectManagerComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/gcf/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::ProjectManagerComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, ion, Design, Development");
        retList << GCF::AboutPerson("Lohith D. S", "NA", "http://www.vcreatelogic.com", "Design, Development");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::ProjectManagerComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::ProjectManagerComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::ProjectManagerComponent::copyrightStatement() const
{
    return QString();
}

QString GCF::Components::ProjectManagerComponentData::formatRecentFileName(const QString& name)
{
    static QDir appDir( QApplication::applicationDirPath() );

    QString ret = name;
    if( name.startsWith( appDir.absolutePath() ) )
    {
        ret = appDir.relativeFilePath(name);
        ret = "${APPDIR}/" + ret;
    }

    return ret;
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(ProjectManagerComponent)

