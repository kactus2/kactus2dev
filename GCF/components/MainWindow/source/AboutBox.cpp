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

#include "AboutBox.h"
#include "ui_AboutBox.h"

#include <GCF/ComponentFactory.h>
#include "MainWindowComponent.h"
#include "MainWindow.h"

#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

Q_DECLARE_METATYPE(IAboutData*);

/**
\ingroup stdmainwindowcomponent
\class GCF::Components::AboutBox GCF::Components::AboutBox.h
\brief This class provides functions to create about dialog box.

The dialog box shows "about" data from all objects that implement
the IAboutData interface.

It provides function to show information about the components in the application.

It gets information on "about" background and applicationLogo from GCF::Components::MainWindowComponent.

Shown below is screenshot of about dialog
\image html about.png
*/


struct GCF::Components::AboutBoxData
{
    Ui::AboutBox ui;
};

/**
Constructor
*/
GCF::Components::AboutBox::AboutBox(QWidget* parent):QDialog(parent)
{
    d = new GCF::Components::AboutBoxData;
    d->ui.setupUi(this);
    init();

    static QPixmap aboutBg( GCF::Components::MainWindowComponent::instance().aboutBackground() );
    QBrush bgBrush( palette().window().color(), aboutBg );
    QPalette pal = this->palette();

    if(!aboutBg.isNull())
    {
        pal.setBrush( backgroundRole(), bgBrush );
        pal.setBrush( QPalette::Base, bgBrush );
        pal.setBrush( QPalette::Window, bgBrush );
        pal.setBrush( QPalette::Background, bgBrush );
    }

    setPalette(pal);
    setAutoFillBackground(true);

    d->ui.lblLogo->setPixmap( QPixmap(GCF::Components::MainWindowComponent::instance().applicationLogo()) );
    d->ui.lstComponents->setSortingEnabled(true);

#ifdef GCF_APP_VERSION
    d->ui.lblVersion->setText( GCF_APP_VERSION );
#else
#if QT_VERSION >= 0x040400
    static QString versionStr = QString("%1 - %2").arg( qApp->applicationName() ).arg( qApp->applicationVersion() );
#else
    static QString versionStr = QString("%1 - 2.5").arg( qApp->applicationName() );
#endif
    d->ui.lblVersion->setText( versionStr );
#endif

    for(int i=0; i<d->ui.componentInfoTabWidget->count(); i++)
    {
        QWidget* widget = d->ui.componentInfoTabWidget->widget(i);
        if(widget->layout())
            widget->layout()->setMargin(0);
    }

    d->ui.txtGeneral->viewport()->installEventFilter(this);
    d->ui.txtCredits->viewport()->installEventFilter(this);
    d->ui.txtCopyright->viewport()->installEventFilter(this);
}

/**
Destructor
*/
GCF::Components::AboutBox::~AboutBox()
{
    delete d;
}

void GCF::Components::AboutBox::currentItemChanged(QListWidgetItem* item)
{
    if(!item)
        return;

    d->ui.txtGeneral->clear();
    d->ui.txtCredits->clear();
    d->ui.txtCopyright->clear();

    QString general, people, copyright;

    IAboutData* aboutData = item->data(Qt::UserRole).value<IAboutData*>();
    if(!aboutData)
        return;
    general = QString("<h2>%1</h2>\n"
            "<h4>%2</h4>\n"
            "<p>Version: %3</p>\n"
            "<p>%4</p>\n"
            "<p>URL: <a href=\"%5\">%5</a></p>\n"
            "<p>Bug Reports: <a href=\"%6\">%6</a></p>\n")
            .arg(aboutData->productName())
            .arg(aboutData->organization())
            .arg(aboutData->version())
            .arg(aboutData->shortDescription())
            .arg(aboutData->homepage())
            .arg(aboutData->bugAddress());

    QList<GCF::AboutPerson> authors = aboutData->authors();
    QList<GCF::AboutPerson> credits = aboutData->credits();

    if(authors.count() || credits.count())
    {
        people += "<table border=\"1\" width=\"100%\">\n";
        people += "<tr><td><b>Name</b></td><td><b>Contribution</b></td><td><b>Contact</b></td>\n";
    }
    for(int i=0; i<authors.count(); i++)
    {
        if(!i)
            people += "<tr><td colspan=\"3\" align=\"center\"><b>Authors</b></td></tr>\n";
        people += QString("<tr><td>%1</td><td>%2</td><td>%3<br>%4</td>\n")
            .arg(authors[i].Name).arg(authors[i].Task).arg(authors[i].EMail).arg(authors[i].URL);
    }
    for(int i=0; i<credits.count(); i++)
    {
        if(!i)
            people += "<tr><td colspan=\"3\" align=\"center\"><b>Credits</b></td></tr>\n";
        people += QString("<tr><td>%1</td><td>%2</td><td>%3<br>%4</td>\n")
            .arg(credits[i].Name).arg(credits[i].Task).arg(credits[i].EMail).arg(credits[i].URL);
    }
    if(authors.count() || credits.count())
        people += "</table>";

    copyright = QString("<p><b>License</b>: %1</p><p>%2</p>\n").arg(aboutData->license()).arg(aboutData->copyrightStatement());
    d->ui.txtCopyright->setHtml(copyright);

    d->ui.txtGeneral->setHtml(general);
    d->ui.txtCredits->setHtml(people);
    d->ui.txtCopyright->setHtml(copyright);
}

void GCF::Components::AboutBox::init()
{
    QStringList completeNames;
    QList<GCF::AbstractComponent*> compList;
    QList<QObject*> aboutDataObjList
        = GCF::Components::MainWindowComponent::instance().findObjects("IAboutData", completeNames, &compList);

    QListWidgetItem* initItem = 0;
    for(int i=0; i<aboutDataObjList.count(); i++)
    {
        IAboutData* aboutData = qobject_cast<IAboutData*>(aboutDataObjList[i]);
        QListWidgetItem* item = new QListWidgetItem(d->ui.lstComponents);
        item->setText( aboutData->productName() );
        item->setData( Qt::UserRole, qVariantFromValue<IAboutData*>(aboutData) );
        if(!initItem)
            initItem = item;
    }

    currentItemChanged(initItem);
    connect(d->ui.lstComponents, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentItemChanged(QListWidgetItem*)));

    d->ui.lblLicensee->setText("This version of the product is distrubuted the terms and conditions "
                               "as described License.txt accompanying the product.");
}

bool GCF::Components::AboutBox::eventFilter(QObject* obj, QEvent* event)
{
    if(!obj->isWidgetType())
        return false;

    if(event->type() != QEvent::MouseButtonPress)
        return false;

    QWidget* wid = (QWidget*)obj;
    QTextEdit* te = qobject_cast<QTextEdit*>(wid->parentWidget());
    if(!te)
        return false;

    QMouseEvent* me = (QMouseEvent*)event;
    QString anchor = te->anchorAt(me->pos());
    if(anchor.isEmpty())
        return false;

    QDesktopServices::openUrl( QUrl(anchor) );
    return false;
}


