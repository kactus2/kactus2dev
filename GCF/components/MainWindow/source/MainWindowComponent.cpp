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

#include "MainWindowComponent.h"
#include "SplashScreen.h"
#include "MainWindow.h"

#include <GCF/ComponentFactory.h>
#include <GCF/RemoteAppAccessPoint.h>
#include <GCF/AppAccessPointComponent.h>
#include <GCF/RemoteAppDiscovery.h>
#include <GCF/AppAccessPoint.h>

#include <QStatusBar>
#include <QPointer>
#include <QMenu>
#include <QDockWidget>
#include <QTabWidget>
#include <QDialog>
#include <QApplication>
#include <QSplitter>


/**
\ingroup gcfcomponents
\defgroup stdmainwindowcomponent Main Window Component
\brief Provides information about classes and interfaces belonging to the Main Window component.

The Main Window Component provides main window for your application which becomes host for menu strip,
dock windows, central widget.

Find snapshot of the Main window below:

\image html stdMainWindow.png
*/

struct GCF::Components::MainWindowComponentData
{
    MainWindowComponentData() : mainWindow(0) { }

    GCF::Components::MainWindow* mainWindow;
    QString aboutBg;
    QString appLogo;
    QString appIcon;
    QString splashImage;
    QPointer<GCF::Components::SplashScreen> splashScreen;
};

GCF_DEFINE_STD_COMPONENT(MainWindowComponent)

/**
\ingroup stdmainwindowcomponent
\class GCF::Components::MainWindowComponent GCF::Components::MainWindowComponent.h
\brief Provides functionality to provide Main window for your application.

This component class provides functions to have following in your
main application widnow.

\li Menu strip
\li Dock windows
	- Left Dock Window
	- Bottom Dock Window
	- Right Dock Window

\li Central Windget.
You can add as many widgets as you want to the central widget. Each widget will be added as
a new tab in the central widget.

\li Message Area
Provides area to display messages.
*/
GCF::Components::MainWindowComponent & GCF::Components::MainWindowComponent::instance()
{
    static GCF::Components::MainWindowComponent* mainWindowComp = GCF_CREATE_COMPONENT(MainWindowComponent);
    return *mainWindowComp;
}

/**
Constructor
*/
GCF::Components::MainWindowComponent::MainWindowComponent()
{
    d = new GCF::Components::MainWindowComponentData;
    d->mainWindow = new GCF::Components::MainWindow;
    d->mainWindow->showMaximized();
    d->mainWindow->setWindowTitle("Standard Main Window for GCF Applications.");
    d->mainWindow->statusBar()->showMessage("Copyright VCreate Logic (P) Ltd. www.vcreatelogic.com");
    GCF::AbstractComponent::setMainWindow( d->mainWindow );

    d->appIcon = ":/MainWindow/appIcon.png";
    d->aboutBg = ":/MainWindow/aboutBackground.png";
    d->appLogo = ":/MainWindow/appLogo.png";
    d->splashImage = ":/MainWindow/splash.png";

    qApp->setWindowIcon( QIcon(d->appIcon) );
    qApp->installEventFilter(this);
}

/**
Destructor
*/
GCF::Components::MainWindowComponent::~MainWindowComponent()
{
    delete d;
}

/**
Returns pointer to the main window
*/
GCF::Components::MainWindow* GCF::Components::MainWindowComponent::mainWindow() const
{
    return d->mainWindow;
}

/**
Sets about backgroud
*/
void GCF::Components::MainWindowComponent::setAboutBackground(const QString& pmFile)
{
    if( d->aboutBg == pmFile )
        return;

    d->aboutBg = pmFile;
}

/**
Returns about backgorund
*/
QString GCF::Components::MainWindowComponent::aboutBackground() const
{
    return d->aboutBg;
}

/**
Sets logo for the application
*/
void GCF::Components::MainWindowComponent::setApplicationLogo(const QString& pmFile)
{
    if( d->appLogo == pmFile )
        return;

    d->appLogo = pmFile;
}

/**
Returns application's logo
*/
QString GCF::Components::MainWindowComponent::applicationLogo() const
{
    return d->appLogo;
}

/**
Sets the application Icon
*/
void GCF::Components::MainWindowComponent::setApplicationIcon(const QString& appIcon)
{
    if( d->appIcon == appIcon )
        return;

    d->appIcon = appIcon;
    qApp->setWindowIcon( QIcon(d->appIcon) );
}

/**
Returns the application Icon
*/
QString GCF::Components::MainWindowComponent::applicationIcon() const
{
    return d->appIcon;
}

/**
Sets the splash image
*/
void GCF::Components::MainWindowComponent::setSplashImage(const QString& pmFile)
{
    if( d->splashImage == pmFile )
        return;

    d->splashImage = pmFile;
}

/**
Returns the splash image set to the application
*/
QString GCF::Components::MainWindowComponent::splashImage() const
{
    return d->splashImage;
}

QAction* GCF::Components::MainWindowComponent::fetchAction(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "togglePanelAction")
        return d->mainWindow->createPopupMenu()->menuAction();

    return GCF::AbstractComponent::fetchAction(completeName);
}

QObject* GCF::Components::MainWindowComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::Components::MainWindowComponent* that = const_cast<GCF::Components::MainWindowComponent*>(this);
    if(comps.last() == "controller")
        return that;

    if(comps.last() == "application")
        return qApp;

    return 0;
}

QWidget* GCF::Components::MainWindowComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "window")
        return d->mainWindow;

    if(comps.last() == "statusBar")
        return d->mainWindow->statusBar();

    if(comps.last() == "leftDockArea")
        return d->mainWindow->dockWidget(Qt::LeftDockWidgetArea);

    if(comps.last() == "rightDockArea")
        return d->mainWindow->dockWidget(Qt::RightDockWidgetArea);

    if(comps.last() == "topDockArea")
        return d->mainWindow->dockWidget(Qt::TopDockWidgetArea);

    if(comps.last() == "bottomDockArea")
        return d->mainWindow->dockWidget(Qt::BottomDockWidgetArea);

    if(comps.last() == "workspace")
        return d->mainWindow->workspace();

    if(comps.last() == "centralWidget")
        return d->mainWindow->centralWidget();

    return 0;
}

void GCF::Components::MainWindowComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(parent == d->mainWindow)
    {
        QDockWidget* dockWidget = new QDockWidget(d->mainWindow);
        // dockWidget->setTitleBarWidget(new CDockWidgetTitleBar(dockWidget));
        Qt::DockWidgetArea area;
        if(hint == "LeftDockWidgetArea")
            area = Qt::LeftDockWidgetArea;
        else if(hint == "RightDockWidgetArea")
            area = Qt::RightDockWidgetArea;
        else if(hint == "TopDockWidgetArea")
            area = Qt::TopDockWidgetArea;
        else if(hint == "BottomDockWidgetArea")
            area = Qt::BottomDockWidgetArea;
        else
            area = Qt::AllDockWidgetAreas;
        dockWidget->setAllowedAreas(area);
        dockWidget->setWidget(child);
        dockWidget->setWindowTitle(child->windowTitle());
        child->setProperty("QDockWidget", qVariantFromValue<QWidget*>(dockWidget));
        if(area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
            d->mainWindow->addDockWidget(area, dockWidget, Qt::Vertical);
        else if(area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea)
            d->mainWindow->addDockWidget(area, dockWidget, Qt::Horizontal);
        else
        {
            d->mainWindow->addDockWidget(area, dockWidget);
            dockWidget->setFloating(true);
        }
        dockWidget->show();
    }
    else if(parent == d->mainWindow->statusBar())
    {
        if(hint.toLower() == "permanent")
            d->mainWindow->statusBar()->addPermanentWidget(child);
        else 
            d->mainWindow->statusBar()->addWidget(child);
    }
    else if(parent == d->mainWindow->centralWidget())
    {
        if( !hint.isEmpty() )
            child->setMaximumHeight( hint.toInt() );
        d->mainWindow->centralWidget()->addWidget(child);
    }
    else
        d->mainWindow->addChildWidget(parent, child);
}

void GCF::Components::MainWindowComponent::removeChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(parent == d->mainWindow)
    {
        QWidget* dockWidgetProp = child->property("QDockWidget").value<QWidget*>();
        QDockWidget* dockWidget = qobject_cast<QDockWidget*>(dockWidgetProp);
        if(dockWidget)
        {
            child->setProperty("QDockWidget", qVariantFromValue<QWidget*>(0));
            dockWidget->deleteLater();
        }
    }
    else if(parent == d->mainWindow->statusBar())
    {
        d->mainWindow->statusBar()->removeWidget(child);
    }
    else if(parent == d->mainWindow->centralWidget() && child != d->mainWindow->workspace())
    {
        // Do nothing
    }
    else
        d->mainWindow->removeChildWidget(parent, child);
}

void GCF::Components::MainWindowComponent::showChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(parent == d->mainWindow)
    {
        QWidget* dockWidgetProp = child->property("QDockWidget").value<QWidget*>();
        QDockWidget* dockWidget = qobject_cast<QDockWidget*>(dockWidgetProp);
        if(dockWidget)
            dockWidget->show();
    }
    else if(parent == d->mainWindow->statusBar())
    {
        child->setEnabled(true);
    }
    else if(parent == d->mainWindow->centralWidget() && child != d->mainWindow->workspace())
    {
        child->show();
    }
    else
        d->mainWindow->showChildWidget(parent, child);
}

void GCF::Components::MainWindowComponent::hideChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(parent == d->mainWindow)
    {
        QWidget* dockWidgetProp = child->property("QDockWidget").value<QWidget*>();
        QDockWidget* dockWidget = qobject_cast<QDockWidget*>(dockWidgetProp);
        if(dockWidget)
            dockWidget->hide();
    }
    else if(parent == d->mainWindow->statusBar())
    {
        child->setEnabled(false);
    }
    else if(parent == d->mainWindow->centralWidget() && child != d->mainWindow->workspace())
    {
        child->hide();
    }
    else
        d->mainWindow->hideChildWidget(parent, child);
}

void GCF::Components::MainWindowComponent::initializeComponent()
{
    // Do nothing in here for now.
}

void GCF::Components::MainWindowComponent::finalizeComponent()
{
    // Do nothing in here for now.
}

void GCF::Components::MainWindowComponent::creationAnnounced()
{
     d->splashScreen = new GCF::Components::SplashScreen( QPixmap(d->splashImage) );
     d->splashScreen->finish(d->mainWindow);
     d->splashScreen->show();
}

void GCF::Components::MainWindowComponent::componentCreated(GCF::AbstractComponent* comp)
{
    if( d->splashScreen.isNull() )
        return;

    if( comp )
        d->splashScreen->showMessage( comp->componentName() );
    else
        d->splashScreen->showMessage( QString() );
}

QObject* GCF::Components::MainWindowComponent::containerObject()
{
    return this;
}

QString GCF::Components::MainWindowComponent::productName() const
{
    return "Standard Main Window for GCF Applications";
}

QString GCF::Components::MainWindowComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::MainWindowComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::MainWindowComponent::version() const
{
    return "2.0";
}

QString GCF::Components::MainWindowComponent::shortDescription() const
{
    return "The standard main window of GCF based applications";
}

QString GCF::Components::MainWindowComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::MainWindowComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/vtkdesigner/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::MainWindowComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, Vision, Design, Development");
        retList << GCF::AboutPerson("Lohith D S", "NA", "http://www.vcreatelogic.com", "VTK System Development");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::MainWindowComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::MainWindowComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::MainWindowComponent::copyrightStatement() const
{
    return QString();
}

void GCF::Components::MainWindowComponent::shutdown()
{
    GCF::RemoteAppDiscovery::instance().stopDiscovery(true);
    GCF::RemoteAppAccessPoint::closeAllRemoteAccessPoints();
    GCF::AppAccessPoint::instance().setAppAccessEnabled(false);
    GCF::ComponentFactory::instance().finalize();
}

bool GCF::Components::MainWindowComponent::eventFilter(QObject* obj, QEvent* event)
{
    if( event->type() != QEvent::Show )
        return false;

    QDialog* dialog = qobject_cast<QDialog*>(obj);
    if( !dialog )
        return false;

    QString title = dialog->windowTitle();
    if( title.contains( qApp->applicationName() ) )
        return false;

    title += QString(" - %1").arg( qApp->applicationName() );
    dialog->setWindowTitle( title );
    return false;
}

