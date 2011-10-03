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

#include "HelpSystemComponent.h"
#include "HelpView.h"
#include "HelpSystemConfigurationPage.h"
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>

#include <QtHelp>
#include <QVBoxLayout>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QCompleter>
#include <QMainWindow>
#include <QStatusBar>
#include <QKeyEvent>
#include <QModelIndex>
#include <QTabWidget>

GCF_DEFINE_STD_COMPONENT(HelpSystemComponent)

struct GCF::Components::HelpSystemComponentData
{
    HelpSystemComponentData() : helpEngine(0), contentModel(0),
        contentWidget(0), indexModel(0), indexWidget(0), helpPanel(0), helpSearchEngine(0),
        searchPanel(0), indexPanel(0), indexFilterLineEdit(0), helpView(0) { }

	QHelpEngine* helpEngine;
	QHelpContentModel* contentModel;
	QHelpContentWidget* contentWidget;
	QHelpIndexModel* indexModel;
	QHelpIndexWidget* indexWidget;
	QTabWidget* helpPanel;

	QHelpSearchEngine* helpSearchEngine;
	QWidget* searchPanel;
    QWidget* indexPanel;
    QLineEdit* indexFilterLineEdit;

    GCF::Components::HelpView* helpView;

    QString helpCollectionFile;
};

GCF::Components::HelpSystemComponent & GCF::Components::HelpSystemComponent::instance()
{
    static GCF::Components::HelpSystemComponent* theInstance = GCF_CREATE_COMPONENT(HelpSystemComponent);
    return *theInstance;
}

GCF::Components::HelpSystemComponent::HelpSystemComponent()
{
    d = new GCF::Components::HelpSystemComponentData;

    d->helpView = new GCF::Components::HelpView;
}

GCF::Components::HelpSystemComponent::~HelpSystemComponent()
{
    delete d;
}

GCF::Components::HelpView* GCF::Components::HelpSystemComponent::helpView() const
{
    return d->helpView;
}

QObject* GCF::Components::HelpSystemComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');
	GCF::Components::HelpSystemComponent* that = const_cast<GCF::Components::HelpSystemComponent*>(this);

    if(comps.last() == "controller")
        return that;

	if(comps.last() == "helpEngine")
    {
        if( !d->helpEngine )
        {
            qDebug("Help Collection File is: %s", qPrintable(d->helpCollectionFile));
            d->helpEngine = new QHelpEngine(d->helpCollectionFile, that);
            bool success = d->helpEngine->setupData();
            if( !success )
                qDebug("Could not initialize help engine");

            d->contentModel = d->helpEngine->contentModel();
            d->contentWidget = d->helpEngine->contentWidget();
            d->indexModel = d->helpEngine->indexModel();
            d->indexWidget = d->helpEngine->indexWidget();
            d->helpSearchEngine = d->helpEngine->searchEngine();
			d->helpSearchEngine->reindexDocumentation();

            d->searchPanel = new QWidget;
            QVBoxLayout* layout = new QVBoxLayout(d->searchPanel);
            layout->setMargin(5);
            layout->addWidget( d->helpSearchEngine->queryWidget() );
            layout->addWidget( d->helpSearchEngine->resultWidget() );

            d->indexPanel = new QWidget;
            QLabel* lf = new QLabel("&Look For");
            d->indexFilterLineEdit = new QLineEdit;
            lf->setBuddy(d->indexFilterLineEdit);
            d->indexFilterLineEdit->installEventFilter(that);

            layout = new QVBoxLayout(d->indexPanel);
            layout->setMargin(5);
            layout->addWidget(lf);
            layout->addWidget(d->indexFilterLineEdit);
            layout->addWidget(d->indexWidget);

            connect(d->indexWidget, SIGNAL(linkActivated(QUrl,QString)),
                d->helpView, SLOT(showHelp(QUrl)));
            connect(d->indexWidget, SIGNAL(linksActivated(QMap<QString, QUrl>,QString)),
                d->helpView, SLOT(showHelp(QMap<QString, QUrl>,QString)));
            connect(d->helpSearchEngine->resultWidget(), SIGNAL(requestShowLink(QUrl)),
                    d->helpView, SLOT(showHelp(QUrl)));
			connect(d->helpSearchEngine->queryWidget(), SIGNAL(search()),
                    this, SLOT(slotBeginSearch()));
			connect(d->indexFilterLineEdit, SIGNAL(textEdited(const QString&)),
					d->indexWidget, SLOT(filterIndices(const QString&)));
			connect(d->indexFilterLineEdit, SIGNAL(returnPressed()),
					d->indexWidget, SLOT(activateCurrentItem()));
            connect(d->contentWidget, SIGNAL(clicked(const QModelIndex&)),
                   this,SLOT(slotContentItemClicked(const QModelIndex&)));
        }

		return d->helpEngine;
    }

    if(comps.last() == "contentModel")
		return d->contentModel;

	if(comps.last() == "indexModel")
		return d->indexModel;

	if(comps.last() == "helpSearchEngine")
		return d->helpSearchEngine;

    return 0;
}

QWidget* GCF::Components::HelpSystemComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	if(comps.last() == "contentPanel")
		return d->contentWidget;

	if(comps.last() == "indexPanel")
		return d->indexPanel;

	if(comps.last() == "searchPanel")
		return d->searchPanel;

    if(comps.last() == "helpView")
        return d->helpView;

	if(comps.last() == "helpPanel")
	{
		if(!d->helpPanel)
			d->helpPanel = new QTabWidget;
		return d->helpPanel;
	}

    return 0;
}

void GCF::Components::HelpSystemComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
	if( !d->helpPanel || parent != d->helpPanel || !child )
		return;

	Q_UNUSED(hint);

	// Cant add a widget thats already in helpPanel
	if( d->helpPanel->indexOf(child) >= 0 )
		return;

	d->helpPanel->addTab(child, child->windowTitle());
}

void GCF::Components::HelpSystemComponent::removeChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
	if( !d->helpPanel || parent != d->helpPanel || !child )
		return;

	// Cant remove widget thats not in the helpPanel
	int index = d->helpPanel->indexOf(child);
	if( index < 0 )
		return;

	d->helpPanel->removeTab(index);
}

void GCF::Components::HelpSystemComponent::initializeComponent()
{
    // Do nothing.
}

void GCF::Components::HelpSystemComponent::finalizeComponent()
{
    QString guiXml = QString("%1/%2").arg(guiXmlFileDirectory()).arg(guiXmlFileName());
    QFile guiXmlFile(guiXml);
    guiXmlFile.open(QFile::ReadOnly);

    QDomDocument guiXmlDoc;
    guiXmlDoc.setContent( &guiXmlFile );
    guiXmlFile.close();

    QDomElement rootE = guiXmlDoc.documentElement();
    QDomNodeList nodes = rootE.childNodes();
    QStringList toDeleteKeys;
    toDeleteKeys << "GCF::Components::HelpSystemComponent.helpView::FontSize";
    toDeleteKeys << "GCF::Components::HelpSystemComponent.helpView::FontName";

    QList<QDomElement> toRemoveEList;

    for(int i=0; i<nodes.count(); i++)
    {
        QDomNode node = nodes.item(i);
        QDomElement nodeE = node.toElement();
        if( nodeE.isNull() )
            continue;

        if(nodeE.tagName() != "property")
            continue;

        QString key = nodeE.attribute("key");

        if( toDeleteKeys.contains(key) )
            toRemoveEList.append(nodeE);
    }

    for(int i=0; i<toRemoveEList.count(); i++)
        rootE.removeChild( toRemoveEList[i] );

    QDomElement e1 = guiXmlDoc.createElement("property");
    QDomElement e2 = guiXmlDoc.createElement("property");
    rootE.appendChild(e1);
    rootE.appendChild(e2);

    e1.setAttribute("key", "GCF::Components::HelpSystemComponent.helpView::FontSize");
    e1.setAttribute("value", d->helpView->fontSize());
    e2.setAttribute("key", "GCF::Components::HelpSystemComponent.helpView::FontName");
    e2.setAttribute("value", d->helpView->fontName());

    QString guiXmlStr = guiXmlDoc.toString();
    guiXmlFile.open(QFile::WriteOnly);
    guiXmlFile.write(guiXmlStr.toAscii());
}

void GCF::Components::HelpSystemComponent::creationAnnounced()
{
    // Do nothing
}


bool GCF::Components::HelpSystemComponent::eventFilter(QObject* object, QEvent* event)
{
    if( object != d->indexFilterLineEdit )
        return false;

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = (QKeyEvent*)(event);
        if(ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down)
        {
            qApp->sendEvent(d->indexWidget, event);
            return true;
        }
    }

    return false;
}

QObject* GCF::Components::HelpSystemComponent::containerObject()
{
    return this;
}

QString GCF::Components::HelpSystemComponent::productName() const
{
    return "Help System Component";
}

QString GCF::Components::HelpSystemComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::HelpSystemComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::HelpSystemComponent::version() const
{
    return "1.0";
}

QString GCF::Components::HelpSystemComponent::shortDescription() const
{
    return "Provides a help system for the application";
}

QString GCF::Components::HelpSystemComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::HelpSystemComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/gcf/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::HelpSystemComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Vasudha S. Mirji", "vasudha.mirji@vcreatelogic.com", "http://www.vcreatelogic.com", "Design/Development of the Help System");
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Wasting time ;-)");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::HelpSystemComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Vasudha S. Mirji", "vasudha.mirji@vcreatelogic.com", "http://www.vcreatelogic.com", "Design/Development of the Help System");
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Wasting time ;-)");
    }
    return retList;
}

QString GCF::Components::HelpSystemComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::HelpSystemComponent::copyrightStatement() const
{
    return QString();
}

void GCF::Components::HelpSystemComponent::setHelpCollectionFile(const QString& cf)
{
    if(d->helpCollectionFile == cf)
        return;

    if(!d->helpCollectionFile.isEmpty())
    {
        qDebug("Cannot change help collection file from \"%s\"", qPrintable(d->helpCollectionFile));
        return;
    }

    d->helpCollectionFile = cf;
}

QString GCF::Components::HelpSystemComponent::helpCollectionFile() const
{
    return d->helpCollectionFile;
}

void GCF::Components::HelpSystemComponent::registerDocumentation(const QString& qchFile)
{
    d->helpEngine->registerDocumentation(qchFile);
}

void GCF::Components::HelpSystemComponent::unregisterDocumentation(const QString& namespaceName)
{
    d->helpEngine->unregisterDocumentation(namespaceName);
}

QStringList GCF::Components::HelpSystemComponent::registeredDocumentations()
{
    return d->helpEngine->registeredDocumentations();
}

void GCF::Components::HelpSystemComponent::showDocumentation(const QUrl& url)
{
    d->helpView->showHelp(url);
}

QUrl GCF::Components::HelpSystemComponent::currentDocumentationUrl() const
{
    return d->helpView->source();
}

void GCF::Components::HelpSystemComponent::showDocumentationForKeyword(const QString& keyword)
{
    QMap<QString, QUrl> lfk = d->indexModel->linksForKeyword(keyword);
    if( !lfk.count() )
        return;

    if( lfk.count() == 1 )
        d->helpView->showHelp( lfk.begin().value() );
    else
        d->helpView->showHelp( lfk, keyword );
}

QHelpEngine* GCF::Components::HelpSystemComponent::helpEngine() const
{
    return d->helpEngine;
}

QString GCF::Components::HelpSystemComponent::pageTitle() const
{
    return "Help System";
}

QIcon GCF::Components::HelpSystemComponent::pageIcon() const
{
    return QIcon();
}

QWidget* GCF::Components::HelpSystemComponent::createPage(QWidget* parent)
{
    GCF::Components::HelpSystemConfigurationPage* page = new GCF::Components::HelpSystemConfigurationPage(parent);
    page->initFrom( this );
    return page;
}

void GCF::Components::HelpSystemComponent::savePage(QWidget* page)
{
    GCF::Components::HelpSystemConfigurationPage* page2 = qobject_cast<GCF::Components::HelpSystemConfigurationPage*>(page);
    if( !page2 )
        return;

    page2->saveInto( this );
}

void GCF::Components::HelpSystemComponent::slotBeginSearch()
{
	QList<QHelpSearchQuery> queryList = d->helpSearchEngine->queryWidget()->query();
	d->helpSearchEngine->search(queryList);
}

void GCF::Components::HelpSystemComponent::slotContentItemClicked(const QModelIndex& index)
{
    QHelpContentItem* item = d->contentModel->contentItemAt(index);
    d->helpView->showHelp(item->url());
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(HelpSystemComponent)

