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

#include "Project.h"
#include "IProjectPart.h"
#include "IXmlUtils.h"
#include <GCF/AbstractComponent.h>
#include <GCF/Common.h>

#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QCloseEvent>

struct GCF::Components::ProjectData
{
    ProjectData() : xmlUtils(0), modified(false), recentFilesMenu(0),
                     recentFilesModified(false) { }

    QList<IProjectPart*> projectPartList;
    IXmlUtils* xmlUtils;
    QString projectFileName;
    bool modified;
    QString defaultProjectPath;
    QString projectRootTagName;
    QMenu* recentFilesMenu;
    QStringList recentFiles;
    bool recentFilesModified;
    QString projectFileSuffix;

    IProjectPart* projectPart(QString id);
};

GCF::Components::Project::Project(QObject* parent)
:QObject(parent)
{
    d = new GCF::Components::ProjectData;
    d->defaultProjectPath = QDir::homePath();
    d->projectRootTagName = "Project";
    d->projectFileSuffix = "gcfproj";
    GCF::AbstractComponent::mainWindow()->installEventFilter(this);
}

GCF::Components::Project::~Project()
{
    delete d;
}

void GCF::Components::Project::setXmlUtils(IXmlUtils* utils)
{
    d->xmlUtils = utils;
}

IXmlUtils* GCF::Components::Project::xmlUtils() const
{
    return d->xmlUtils;
}

void GCF::Components::Project::setDefaultProjectPath(QString path)
{
    d->defaultProjectPath = path;
}

void GCF::Components::Project::setProjectRootTagName(QString tagName)
{
    d->projectRootTagName = tagName;
}

void GCF::Components::Project::setProjectFileSuffix(QString suffix)
{
    d->projectFileSuffix = suffix;
}

QMenu* GCF::Components::Project::recentFilesMenu() const
{
    if(d->recentFilesMenu)
        return d->recentFilesMenu;

    d->recentFilesMenu = new QMenu;
    for(int i=d->recentFiles.count()-1; i>=0; i--)
    {
        QString fileName = d->recentFiles[i];

        QFileInfo fi(fileName);
        QAction* action = d->recentFilesMenu->addAction( fi.fileName() );
        action->setProperty("_FilePath_", fi.absoluteFilePath() );
        connect(action, SIGNAL(triggered()), this, SLOT(slotOpenRecentFile()));
    }

    return d->recentFilesMenu;
}

void GCF::Components::Project::addRecentFile(QString recentFile)
{
    if(recentFile.isEmpty() || d->recentFiles.count() && d->recentFiles.last() == recentFile)
        return;

    int removeIndex = -1;
    if(d->recentFiles.count())
    {
        removeIndex = d->recentFiles.indexOf(recentFile);
        d->recentFiles.removeAt(removeIndex);
    }

    d->recentFiles.append(recentFile);

    if(d->recentFilesMenu)
    {
        if(removeIndex >= 0)
        {
            QList<QAction*> actions = d->recentFilesMenu->actions();
            removeIndex = actions.count()-removeIndex-1;
            QAction* remAction = actions.takeAt(removeIndex);
            delete remAction;
        }

        QFileInfo fi(recentFile);
        QAction* action = new QAction(fi.fileName(), d->recentFilesMenu);
        action->setProperty("_FilePath_", fi.absoluteFilePath() );
        connect(action, SIGNAL(triggered()), this, SLOT(slotOpenRecentFile()));

        QList<QAction*> actions = d->recentFilesMenu->actions();
        if(actions.count())
            d->recentFilesMenu->insertAction(actions.first(), action);
        else
            d->recentFilesMenu->addAction(action);
    }

    d->recentFilesModified = true;
}

QObject* GCF::Components::Project::containerObject()
{
    return this;
}

bool GCF::Components::Project::newProject()
{
    bool success = true;

    for(int i=0; i<d->projectPartList.count(); i++)
    {
        IProjectPart* part = d->projectPartList[i];
        success &= part->newProjectPart();
    }

    d->modified = false;
    emit projectModified(d->modified);
    emit projectCleared();

    return success;
}

bool GCF::Components::Project::loadProject(QString projectFile)
{
    HourGlass hourGlass;

    emit projectLoadBegin();

    QDomDocument doc;
    QFile file(projectFile);
    if(!file.open(QFile::ReadOnly))
    {
        emit projectLoadEnd();
        return false;
    }

    QString docStr = file.readAll();
    if(!doc.setContent(docStr))
    {
        emit projectLoadEnd();
        return false;
    }

    QDomElement rootE = doc.documentElement();
    if(rootE.tagName() != d->projectRootTagName)
    {
        emit projectLoadEnd();
        return false;
    }

    QList<IProjectPart*> loadedParts;
    QDomNodeList childNodes = rootE.childNodes();
    for(int i=0; i<childNodes.count(); i++)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement childE = childNode.toElement();

        if(childE.isNull() || childE.tagName() != "ProjectPart")
            continue;

        QString id = childE.attribute("ID");
        if(id.isEmpty())
            continue;

        IProjectPart* part = d->projectPart(id);
        if(!part)
            continue;

        if(part->loadProjectPart(doc, childE))
            loadedParts.append(part);
    }

    for(int i=0; i<loadedParts.count(); i++)
        loadedParts[i]->initializeProjectPart();

    emit projectLoadEnd();

    return true;
}

bool GCF::Components::Project::saveProject(QString projectFile)
{
    HourGlass hourGlass;

    QFile file(projectFile);
    if(!file.open(QFile::WriteOnly))
        return false;

    QDomDocument doc(d->projectRootTagName);
    QDomElement rootE = doc.createElement(d->projectRootTagName);
    doc.appendChild(rootE);

    for(int i=0; i<d->projectPartList.count(); i++)
    {
        IProjectPart* part = d->projectPartList[i];
        QDomElement partE = doc.createElement("ProjectPart");
        partE.setAttribute("ID", part->projectPartID());
        if(part->saveProjectPart(doc, partE))
            rootE.appendChild(partE);
    }

    QString docStr = doc.toString(2);
    file.write(docStr.toAscii());
    file.close();

    return true;
}

bool GCF::Components::Project::closeProject()
{
    bool success = true;

    for(int i=0; i<d->projectPartList.count(); i++)
    {
        IProjectPart* part = d->projectPartList[i];
        success &= part->closeProjectPart();
    }

    d->modified = false;
    emit projectModified(d->modified);

    return success;
}

bool GCF::Components::Project::isModified()
{
    return d->modified;
}

void GCF::Components::Project::addProjectPart(IProjectPart* part)
{
    if(!part || d->projectPartList.contains(part))
        return;

    d->projectPartList.append(part);
    QObject* obj = part->containerObject();
    connect(obj, SIGNAL(projectPartModified()), this, SLOT(slotProjectPartModified()));
}

void GCF::Components::Project::removeProjectPart(IProjectPart* part)
{
    if(!part || !d->projectPartList.contains(part))
        return;

    d->projectPartList.removeAll(part);
    QObject* obj = part->containerObject();
    disconnect(obj, SIGNAL(projectPartModified()), this, SLOT(slotProjectPartModified()));
}

int GCF::Components::Project::projectPartCount()
{
    return d->projectPartList.count();
}

IProjectPart* GCF::Components::Project::projectPart(int index)
{
    if(index < 0 || index >= d->projectPartList.count())
        return 0;

    return d->projectPartList[index];
}

QString GCF::Components::Project::currentProjectFileName() const
{
    QFileInfo fi(d->projectFileName);
    if(fi.isFile())
        return fi.absoluteFilePath();

    return "noname";
}

QString GCF::Components::Project::currentProjectFilePath() const
{
    QFileInfo fi(d->projectFileName);
    if(fi.isFile())
        return fi.absolutePath();

    return QString();
}

QStringList GCF::Components::Project::recentFilesList() const
{
    return d->recentFiles;
}

QString GCF::Components::Project::defaultProjectPath() const
{
    return d->defaultProjectPath;
}

QString GCF::Components::Project::projectRootTagName() const
{
    return d->projectRootTagName;
}

QString GCF::Components::Project::projectFileSuffix() const
{
    return d->projectFileSuffix;
}

void GCF::Components::Project::slotNewProject()
{
    if( d->modified )
    {
        QMessageBox::StandardButton btn = QMessageBox::question( GCF::AbstractComponent::mainWindow(),
            "Save modifications?",
            "The project has been modified. Do you want to save the modifications?",
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
        if( btn == QMessageBox::Cancel )
            return;

        if( btn == QMessageBox::Yes )
            slotSaveProject();
    }

    blockSignals(true);
    this->newProject();
    d->projectFileName.clear();
    d->modified = false;
    blockSignals(false);
    emit projectModified(d->modified);
    emit projectCleared();
    QMetaObject::invokeMethod(this, "currentProjectFileNameChanged", Qt::QueuedConnection, Q_ARG(QString, d->projectFileName));
}

void GCF::Components::Project::slotLoadProject()
{
    slotNewProject();

    QString fileType = QString("%1 Project (*.%2)").arg(qApp->applicationName()).arg(d->projectFileSuffix);
    d->projectFileName = QFileDialog::getOpenFileName(GCF::AbstractComponent::mainWindow(), "Load Project",
        d->defaultProjectPath, fileType, 0);
    QMetaObject::invokeMethod(this, "currentProjectFileNameChanged", Qt::QueuedConnection, Q_ARG(QString, d->projectFileName));
    if(d->projectFileName.isEmpty())
        return;

    emit projectLoadBegin();
    blockSignals(true);
    closeProject();
    loadProject(d->projectFileName);
    addRecentFile(d->projectFileName);
    d->modified = false;
    blockSignals(false);
    emit projectModified(d->modified);
    emit projectLoadEnd();
}

void GCF::Components::Project::slotSaveProject()
{
    blockSignals(true);
    if(d->projectFileName.isEmpty())
        slotSaveProjectAs();
    else
    {
        saveProject(d->projectFileName);
        addRecentFile(d->projectFileName);
    }
    d->modified = false;
    blockSignals(false);
    emit projectModified(d->modified);
}

void GCF::Components::Project::slotCloseProject()
{
	this->slotNewProject();
}

void GCF::Components::Project::slotSaveProjectAs()
{
    QString fileType = QString("%1 Project (*.%2)").arg(qApp->applicationName()).arg(d->projectFileSuffix);
    d->projectFileName = QFileDialog::getSaveFileName(GCF::AbstractComponent::mainWindow(), "Save Project",
        d->defaultProjectPath, fileType, 0);
    if(d->projectFileName.isEmpty())
    {
        QMetaObject::invokeMethod(this, "currentProjectFileNameChanged", Qt::QueuedConnection, Q_ARG(QString, d->projectFileName));
        return;
    }

    QString ending = QString(".%1").arg(d->projectFileSuffix);
    if(!d->projectFileName.endsWith(ending))
    {
        d->projectFileName += ending;
        QMetaObject::invokeMethod(this, "currentProjectFileNameChanged", Qt::QueuedConnection, Q_ARG(QString, d->projectFileName));
    }

    blockSignals(true);
    saveProject(d->projectFileName);
    addRecentFile(d->projectFileName);
    d->modified = false;
    blockSignals(false);
    emit projectModified(d->modified);
}

void GCF::Components::Project::slotProjectPartModified()
{
    d->modified = true;
    emit projectModified(d->modified);
}

void GCF::Components::Project::slotOpenRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if(!action)
        return;

    QString fileName = action->property("_FilePath_").toString();
    if(fileName.isEmpty())
        return;

    emit projectLoadBegin();
    blockSignals(true);
    closeProject();
    loadProject(fileName);
    addRecentFile(fileName);
    d->modified = false;
    d->projectFileName = fileName;
    blockSignals(false);
    emit projectModified(d->modified);
    emit projectLoadEnd();
    QMetaObject::invokeMethod(this, "currentProjectFileNameChanged", Qt::QueuedConnection, Q_ARG(QString, d->projectFileName));
}

void GCF::Components::Project::setRecentFileListUnModified()
{
    d->recentFilesModified = false;
}

bool GCF::Components::Project::isRecentFileListModified() const
{
    return d->recentFilesModified;
}

bool GCF::Components::Project::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == GCF::AbstractComponent::mainWindow() && event->type() == QEvent::Close)
    {
        QCloseEvent* ce = (QCloseEvent*)event;

        if( d->modified )
        {
            QMessageBox::StandardButton btn = QMessageBox::question( GCF::AbstractComponent::mainWindow(),
                "Save modifications?",
                "The project has been modified. Do you want to save the modifications?",
                QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
            if( btn == QMessageBox::Cancel )
            {
                ce->setAccepted(false);
                return true;
            }

            if( btn == QMessageBox::Yes )
                slotSaveProject();
        }

		this->newProject();
    }

    return false;
}

IProjectPart* GCF::Components::ProjectData::projectPart(QString id)
{
    for(int i=projectPartList.count()-1; i>=0; i--)
    {
        IProjectPart* part = projectPartList[i];
        if(part->projectPartID() == id)
            return part;
    }
    return 0;
}
