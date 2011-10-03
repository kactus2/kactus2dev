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

#include <GCF/ComponentLoaderComponent.h>
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>

#include <QList>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QFileDialog>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QCoreApplication>


struct GCF::ComponentLoaderComponentData
{
	QStringList compDirPaths;
	GCF::ComponentLoaderComponent* loaderComponent;

	void registerFromDirectory(const QString& dir);
	void registerFromFile(const QString& file);
};


GCF_DEFINE_COMPONENT(GCF::ComponentLoaderComponent);

GCF::ComponentLoaderComponent& GCF::ComponentLoaderComponent::instance()
{
    static GCF::ComponentLoaderComponent* theInstance = GCF_CREATE_COMPONENT(GCF::ComponentLoaderComponent);
    return *theInstance;
}

GCF::ComponentLoaderComponent::ComponentLoaderComponent()
{
    d = new GCF::ComponentLoaderComponentData;
    d->loaderComponent = this;
}

GCF::ComponentLoaderComponent::~ComponentLoaderComponent()
{
    delete d;
}

QStringList GCF::ComponentLoaderComponent::componentDirectories() const
{
    return d->compDirPaths;
}

void GCF::ComponentLoaderComponent::loadComponent(const QString& compName)
{
    emit loadingComponent(compName);
	GCF::AbstractComponent* comp = GCF::ComponentFactory::instance().createComponent(compName);
    if(!comp)
        qWarning("Could not create component \"%s\"", compName.toAscii().data());
    else
    {
        qDebug("Loading component \"%s\"", compName.toAscii().data());
        comp->initialize();
    }
}

void GCF::ComponentLoaderComponent::loadComponent2(const QString& compName, const QString& libraryFile)
{
    d->registerFromFile(libraryFile);
    
    emit loadingComponent(compName);
	GCF::AbstractComponent* comp = GCF::ComponentFactory::instance().createComponent(compName);
    if(!comp)
        qWarning("Could not create component \"%s\"", compName.toAscii().data());
    else
    {
        qDebug("Loading component \"%s\"", compName.toAscii().data());
        comp->initialize();
    }
}

void GCF::ComponentLoaderComponent::addComponentDirectory(const QString& dir)
{
    d->compDirPaths.append(dir);
    qDebug("Added %s to component directory list", dir.toAscii().data());
}

void GCF::ComponentLoaderComponent::removeComponentDirectory(const QString& dir)
{
    d->compDirPaths.removeAll(dir);
}

void GCF::ComponentLoaderComponent::registerComponents()
{
    if(!d->compDirPaths.count())
        return;
    for(int i=0; i<d->compDirPaths.count(); i++)
        d->registerFromDirectory(d->compDirPaths[i]);
}

QObject* GCF::ComponentLoaderComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::ComponentLoaderComponent* that = const_cast<GCF::ComponentLoaderComponent*>(this);
    if(comps.last() == "controller")
        return that;
    return 0;
}

void GCF::ComponentLoaderComponent::initializeComponent()
{
    // Do nothing for now
}

void GCF::ComponentLoaderComponent::finalizeComponent()
{
    // Do nothing for now
}

void GCF::ComponentLoaderComponent::componentCreated(GCF::AbstractComponent* comp)
{
    GCF::ComponentLoaderComponent::instance(); // make sure that the instance pointer is
                                           // initialized
    if(comp->componentName() == "CConfigurationDialogComponent")
    {
        GCF::ComponentLoaderConfigComponent* c = new GCF::ComponentLoaderConfigComponent;
        c->initialize();
    }
}

void GCF::ComponentLoaderComponentData::registerFromDirectory(const QString& dirPath)
{
    QDir dir(dirPath);
    QStringList nameFilters;
#if(defined(Q_OS_WIN32) || defined(Q_OS_WIN64))
    nameFilters << "*.dll";
#else
    nameFilters << "*.so";
#endif

    QStringList libFileList = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    for(int i=0; i<libFileList.count(); i++)
    {
        QString libFile = QString("%1/%2").arg(dirPath).arg(libFileList[i]);
        registerFromFile(libFile);
    }
}

void GCF::ComponentLoaderComponentData::registerFromFile(const QString& file)
{
    QFileInfo fi(file);
    QString filePath = fi.absolutePath();
    QString fileName = fi.baseName();

    QCoreApplication::addLibraryPath(filePath);
    qWarning("Loading components from %s", file.toAscii().data());
    QLibrary library(fileName);
    if(!library.load())
    {
        qWarning("Could not load \"%s\": %s", file.toAscii().data(), library.errorString().toAscii().data());
        return;
    }

    CreateComponentPluginFunction fn = (CreateComponentPluginFunction)library.resolve("GetComponentPlugin");
    if(!fn)
    {
        library.unload();
        return;
    }

    IComponentPlugin* plugin = fn();
#ifdef GCF_USE_BUILD_KEY
    if( plugin->buildKey() != IComponentPlugin::systemBuildKey() )
    {
        qDebug("Build key of %s does not match with this system.", qPrintable(plugin->componentName()));
        delete plugin;
        library.unload();
        return;
    }
#endif
    QString compName = plugin->componentName();
    const QMetaObject* existingCompMeta = GCF::ComponentFactory::instance().componentMeta(compName);
    const QMetaObject* newCompMeta = plugin->componentMeta();

    if(!existingCompMeta)
    {
        plugin->registerComponent();
        qDebug("Registering %s component", compName.toAscii().data());
    }
    else if(newCompMeta)
    {
        int exIndex = existingCompMeta->indexOfClassInfo("Version");
        int newIndex = newCompMeta->indexOfClassInfo("Version");

        if(exIndex >= 0 && newIndex >= 0)
        {
            QMetaClassInfo existingVersion = existingCompMeta->classInfo(exIndex);
            QMetaClassInfo newVersion = newCompMeta->classInfo(newIndex);
            if(QString(existingVersion.value()).toInt() < QString(newVersion.value()).toInt())
            {
                plugin->registerComponent();
                qDebug("Registering %s component", compName.toAscii().data());
            }
            else
                library.unload();
        }
        else
            library.unload();
    }
    else
        library.unload();
}

/****************************************************************************
GCF::ComponentLoaderConfigPage Implementation
****************************************************************************/

#include "ui_ComponentLoaderConfigPage.h"

struct GCF::ComponentLoaderConfigPageData
{
    Ui::ComponentLoaderConfigPage ui;
};

GCF::ComponentLoaderConfigPage::ComponentLoaderConfigPage(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::ComponentLoaderConfigPageData;
    d->ui.setupUi(this);
}

GCF::ComponentLoaderConfigPage::~ComponentLoaderConfigPage()
{
    delete d;
}

void GCF::ComponentLoaderConfigPage::on_cmdAddPath_clicked()
{
    static QString lastPath;
    QString path = QFileDialog::getExistingDirectory(this, "Choose an existing directory", lastPath);
    if(path.isEmpty())
        return;

    lastPath = path;
    d->ui.pathList->addItem(path);
    GCF::ComponentLoaderComponent::instance().addComponentDirectory(path);
}

void GCF::ComponentLoaderConfigPage::on_cmdRemovePath_clicked()
{
    if(!d->ui.pathList->currentItem())
        return;

    QString path = d->ui.pathList->currentItem()->text();
    GCF::ComponentLoaderComponent::instance().removeComponentDirectory(path);
    delete d->ui.pathList->currentItem();
}

void GCF::ComponentLoaderConfigPage::showEvent(QShowEvent* se)
{
    if(d->ui.pathList->count())
        return;

    QStringList pathList = GCF::ComponentLoaderComponent::instance().componentDirectories();
    for(int i=0; i<pathList.count(); i++)
        d->ui.pathList->addItem(pathList[i]);

    Q_UNUSED(se);
}

/****************************************************************************
GCF::ComponentLoaderConfigComponent Implementation
****************************************************************************/

GCF_DEFINE_COMPONENT(GCF::ComponentLoaderConfigComponent)

GCF::ComponentLoaderConfigComponent::ComponentLoaderConfigComponent() 
{ 
}

GCF::ComponentLoaderConfigComponent::~ComponentLoaderConfigComponent() 
{ 
}

void GCF::ComponentLoaderConfigComponent::initializeComponent()
{
    setGuiXmlFileName(":/GCF/guixml/GCF::ComponentLoaderConfigComponent.xml");
}
void GCF::ComponentLoaderConfigComponent::finalizeComponent() { }

QWidget* GCF::ComponentLoaderConfigComponent::createWidget(const QString& completeName)
{
    QStringList comps = completeName.split('.');
    if(comps.last() == "configPage")
        return new GCF::ComponentLoaderConfigPage;
    return 0;
}

