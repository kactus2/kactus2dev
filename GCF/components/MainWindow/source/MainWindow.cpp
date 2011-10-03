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

#include "MainWindow.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QWorkspace>
#include <QDockWidget>
#include <QTabWidget>
#include <QStatusBar>
#include <QApplication>
#include <QDialog>
#include <QContextMenuEvent>
#include <QLabel>
#include <QMenuBar>
#include <QStyle>
#include <QIcon>
#include <QSplitter>
#include <QTabBar>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

#include "MainWindowComponent.h"
#include "AboutBox.h"
#include "IWorkspaceTabBarPaintHelper.h"

#include <GCF/ConfigurationDialogComponent.h>
#include <GCF/MenuStrip.h>

// We want the tabs to be drawn in a custom way, if required. This class is a bunch of tricks.
// Its a good thing that nobody outside MainWindow.cpp actually uses it!
class TabWidgetStyle : public QStyle
{
public:
    TabWidgetStyle(QTabWidget* workspaceTabWidget) : m_workspaceTabWidget(workspaceTabWidget),
                                                     m_tabBar(0),
                                                     m_oldStyle(workspaceTabWidget->style())
    { 
        m_workspaceTabWidget->setStyle(this);
        this->setParent(m_workspaceTabWidget);

        m_tabBar = m_workspaceTabWidget->findChild<QTabBar*>("qt_tabwidget_tabbar");
        if(m_tabBar)
        {
            m_tabBar->installEventFilter(this);
            m_tabBar->setStyle(this);
        }
    }

    virtual ~TabWidgetStyle() { }

    void drawControl(QStyle::ControlElement element, 
                     const QStyleOption* option, 
                     QPainter* painter, 
                     const QWidget* widget) const;

    // Reimplement the other QStyle interface methods to call corresponding ones in the
    // old style
    virtual void polish(QWidget * w) { m_oldStyle->polish(w); }
    virtual void unpolish(QWidget *w) { m_oldStyle->unpolish(w); }
    virtual void polish(QApplication * a) { m_oldStyle->polish(a); }
    virtual void unpolish(QApplication *a) { m_oldStyle->unpolish(a); }
    virtual void polish(QPalette & p) { m_oldStyle->polish(p); }

    virtual QRect itemTextRect(const QFontMetrics &fm, const QRect &r,
                           int flags, bool enabled,
                           const QString &text) const 
    { return m_oldStyle->itemTextRect(fm, r, flags, enabled, text); }

    virtual QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
    { return m_oldStyle->itemPixmapRect(r, flags, pixmap); }

    virtual void drawItemText(QPainter *painter, const QRect &rect,
                              int flags, const QPalette &pal, bool enabled,
                              const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
    { m_oldStyle->drawItemText(painter, rect, flags, pal, enabled, text, textRole); }

    virtual void drawItemPixmap(QPainter *painter, const QRect &rect,
                                int alignment, const QPixmap &pixmap) const
    { m_oldStyle->drawItemPixmap(painter, rect, alignment, pixmap); }

    virtual QPalette standardPalette() const
    { return m_oldStyle->standardPalette(); }

    virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                               const QWidget *w = 0) const
    { m_oldStyle->drawPrimitive(pe, opt, p, w); }

    virtual QRect subElementRect(SubElement subElement, const QStyleOption *option,
                                 const QWidget *widget = 0) const
    { return m_oldStyle->subElementRect(subElement, option, widget); }

    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *widget = 0) const
    { m_oldStyle->drawComplexControl(cc, opt, p, widget); }

    virtual SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                             const QPoint &pt, const QWidget *widget = 0) const
    { return m_oldStyle->hitTestComplexControl(cc, opt, pt, widget); }

    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                                 SubControl sc, const QWidget *widget = 0) const
    { return m_oldStyle->subControlRect(cc, opt, sc, widget); }

    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const
    { return m_oldStyle->pixelMetric(metric, option, widget); }

    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                                   const QSize &contentsSize, const QWidget *w = 0) const
    { return m_oldStyle->sizeFromContents(ct, opt, contentsSize, w); }

    virtual int styleHint(StyleHint stylehint, const QStyleOption *opt = 0,
                          const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const
    { return m_oldStyle->styleHint(stylehint, opt, widget, returnData); }

    virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const 
    { return m_oldStyle->generatedIconPixmap(iconMode, pixmap, opt); }

    virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0,
                                   const QWidget *widget = 0) const
    { return m_oldStyle->standardPixmap(standardPixmap, opt, widget); }

private:
    QTabWidget* m_workspaceTabWidget;
    QTabBar* m_tabBar;
    QStyle* m_oldStyle;
};

void TabWidgetStyle::drawControl(QStyle::ControlElement element, const QStyleOption* option, 
                                 QPainter* painter, const QWidget* widget) const
{
    if(element != QStyle::CE_TabBarTab)
        return;

    const QTabBar* tabBar = qobject_cast<const QTabBar*>(widget);
    if(!m_tabBar || !tabBar || tabBar != m_tabBar)
    {
        m_oldStyle->drawControl(element, option, painter, widget);
        return;
    }

    int tabIndex = -1;
    for(int i=0; i<m_tabBar->count(); i++)
    {
        QRect rect = m_tabBar->tabRect(i);
        if(rect.contains(option->rect))
        {
            tabIndex = i;
            break;
        }
    }

    QWidget* w = m_workspaceTabWidget->widget(tabIndex);
    IWorkspaceTabBarPaintHelper* helper = qobject_cast<IWorkspaceTabBarPaintHelper*>(w);
    if(!helper)
    {
        m_oldStyle->drawControl(element, option, painter, widget);
        return;
    }

    helper->drawTabBar(option, painter, m_workspaceTabWidget);
}

struct GCF::Components::MainWindowData
{
    MainWindowData() : centralWidget(0), workspace(0), shutdownFlag(false) { }

    QSplitter* centralWidget;
    QTabWidget* workspace;
    QMap<Qt::DockWidgetArea, QDockWidget*> dockWidgetMap;
    bool shutdownFlag;
};

/**
\ingroup mainwindowcomponent
\class GCF::Components::MainWindow GCF::Components::MainWindow.h
\brief This class provides functions to create Main window for your application.

Class provides functions to create main window, and the contents of the main window like central widget,
dock widgets, adding/removing the widgets to the central widgets etc.
*/

/**
Returns the pointer to only instance of GCF::Components::MainWindow.
If there is no instance, one is created and returned.
*/
GCF::Components::MainWindow & GCF::Components::MainWindow::instance()
{
    return *(GCF::Components::MainWindowComponent::instance().mainWindow());
}

GCF::Components::MainWindow::MainWindow()
:QMainWindow(0)
{
    d = new GCF::Components::MainWindowData;
    setAnimated(false);
    setMinimumSize(800, 600);

    d->centralWidget = new QSplitter(Qt::Vertical, this);
    setCentralWidget(d->centralWidget);

    d->workspace = new QTabWidget(d->centralWidget);
    d->workspace->setTabPosition(QTabWidget::North);
    d->workspace->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    d->centralWidget->addWidget(d->workspace);
    connect(d->workspace, SIGNAL(currentChanged(int)), this, SLOT(on_workspace_currentChanged(int)));

    new TabWidgetStyle(d->workspace);
}

/**
Destructor
*/
GCF::Components::MainWindow::~MainWindow()
{
    delete d;
}

/**
Returns pointer to the central widget
*/
QSplitter* GCF::Components::MainWindow::centralWidget() const
{
    return d->centralWidget;
}

/**
Returns pointer to the workspace.
*/
QTabWidget* GCF::Components::MainWindow::workspace()
{
    return d->workspace;
}


/**
Returns the pointer to the dock widget for the given dock area.

If dockwidget corresponding to the dock area is not found, new dockwidget for that
dock area is created.

\param area DockWidgetArea for which corresponding dockwidget is returned.

*/
QDockWidget* GCF::Components::MainWindow::dockWidget(Qt::DockWidgetArea area)
{
    QDockWidget* ret = 0;
    if(d->dockWidgetMap.contains(area))
        ret = d->dockWidgetMap[area];

    if(!ret)
    {
        // create the dock widget
        ret = new QDockWidget(this);
        ret->setAllowedAreas(area);

        // place a tab widget on the dock widget
        QTabWidget* tabWidget = new QTabWidget(ret);
        ret->setWidget(tabWidget);

        // position the tabs in the right place.
        switch(area)
        {
        case Qt::LeftDockWidgetArea:
            tabWidget->setTabPosition(QTabWidget::West);
            break;
        case Qt::RightDockWidgetArea:
            tabWidget->setTabPosition(QTabWidget::East);
            break;
        case Qt::TopDockWidgetArea:
            tabWidget->setTabPosition(QTabWidget::North);
            // tabWidget->setFixedHeight(150);
            // ret->setFixedHeight(150);
            break;
        case Qt::BottomDockWidgetArea:
            tabWidget->setTabPosition(QTabWidget::South);
            // tabWidget->setFixedHeight(150);
            // ret->setFixedHeight(150);
            break;
        default:
            break;
        }
    }

    addDockWidget(area, ret);
    d->dockWidgetMap[area] = ret;
    ret->hide();
    return ret;
}

void GCF::Components::MainWindow::addChildWidget(QWidget* parent, QWidget* child)
{
    if(!parent || !child)
        return;

    if(parent == d->workspace)
    {
        d->workspace->addTab(child, child->windowTitle());
        child->installEventFilter(this);
        child->setAutoFillBackground(true);
        emit workspaceWidgetActivated(d->workspace->currentWidget());
    }
    else
    {
        QList<QDockWidget*> dockWidgets = d->dockWidgetMap.values();
        QDockWidget* dw = qobject_cast<QDockWidget*>(parent);
        if(dw && dockWidgets.contains(dw))
        {
            QTabWidget* tw = qobject_cast<QTabWidget*>(dw->widget());
            tw->addTab(child, child->windowTitle());
            child->setAutoFillBackground(true);
            if(tw->count())
                dw->show();
            else
                dw->hide();
        }
    }
}

void GCF::Components::MainWindow::removeChildWidget(QWidget* parent, QWidget* child)
{
    if(!parent || !child)
        return;

    if(parent == d->workspace)
    {
        d->workspace->removeTab(d->workspace->indexOf(child));
        child->removeEventFilter(this);
    }
    else
    {
        QList<QDockWidget*> dockWidgets = d->dockWidgetMap.values();
        QDockWidget* dw = qobject_cast<QDockWidget*>(parent);
        if(dw && dockWidgets.contains(dw))
        {
            QTabWidget* tw = qobject_cast<QTabWidget*>(dw->widget());
            tw->removeTab(tw->indexOf(child));
            if(tw->count())
                dw->show();
            else
                dw->hide();
        }
    }
}

void GCF::Components::MainWindow::showChildWidget(QWidget* parent, QWidget* child)
{
    if(!parent || !child)
        return;

    if(parent == d->workspace)
    {
        d->workspace->setCurrentWidget(child);
    }
    else
    {
        QList<QDockWidget*> dockWidgets = d->dockWidgetMap.values();
        QDockWidget* dw = qobject_cast<QDockWidget*>(parent);
        if(dw && dockWidgets.contains(dw))
        {
            QTabWidget* tw = qobject_cast<QTabWidget*>(dw->widget());
            int index = tw->indexOf(child);
            if(index < 0)
                child->show();
            else
            {
                child->setEnabled(true);
                tw->setCurrentWidget(child);
            }
        }
    }
}

void GCF::Components::MainWindow::hideChildWidget(QWidget* parent, QWidget* child)
{
    if(!parent || !child)
        return;

    if(parent == d->workspace)
    {
        // widgets on the workspace are not hidden or shown
    }
    else
    {
        QList<QDockWidget*> dockWidgets = d->dockWidgetMap.values();
        QDockWidget* dw = qobject_cast<QDockWidget*>(parent);
        if(dw && dockWidgets.contains(dw))
        {
            QTabWidget* tw = qobject_cast<QTabWidget*>(dw->widget());
            int index = tw->indexOf(child);
            if(index < 0)
                child->hide();
            else
                child->setEnabled(false);
        }
    }
}

void GCF::Components::MainWindow::showConfigurationDialog()
{
    GCF::ConfigurationDialogComponent* comp = new GCF::ConfigurationDialogComponent;
    comp->initialize();

    // The dialog box will execute in the meantime.
    // It will also load pages from objects that implement the
    // IConfigurationDialogPageProvider interface

    comp->finalize();
}

void GCF::Components::MainWindow::on_workspace_currentChanged(int index)
{
    QWidget* widget = d->workspace->widget(index);
    emit workspaceWidgetActivated(widget);
}

void GCF::Components::MainWindow::about()
{
    GCF::Components::AboutBox aboutDialog(this);
    aboutDialog.exec();
}

void GCF::Components::MainWindow::closeEvent(QCloseEvent* ce)
{
    if(d->shutdownFlag)
    {
        QMainWindow::closeEvent(ce);
        return;
    }

    QMessageBox::StandardButton ret =
    QMessageBox::information(this, "Quit Confirmation", "Are you sure you want to quit?",
        QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    if(ret == QMessageBox::No)
        ce->setAccepted(false);
    else
    {
        ce->setAccepted(true);
        d->shutdownFlag = true;
        QMetaObject::invokeMethod(&GCF::Components::MainWindowComponent::instance(), "shutdown", Qt::QueuedConnection);
    }
}

bool GCF::Components::MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::WindowTitleChange)
    {
        QWidget* w = qobject_cast<QWidget*>(obj);
        int index = d->workspace->indexOf(w);
        if(index < 0)
            return false;

        QString title = w->windowTitle();

        d->workspace->setTabText(index, title);
    }

    return false;
}

