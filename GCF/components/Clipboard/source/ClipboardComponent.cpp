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

#include "ClipboardComponent.h"
#include "ClipboardModel.h"
#include "ClipboardWidget.h"
#include "IXmlUtils.h"

#include <GCF/IComponentPlugin.h>
#include <GCF/ComponentFactory.h>
#include <GCF/Message.h>
#include <QByteArray>
#include <QDataStream>
#include <QFile>

GCF_DEFINE_STD_COMPONENT(ClipboardComponent)

typedef QList<ClipboardData> ClipboardDataList;

Q_DECLARE_METATYPE(ClipboardDataList)

const QString defaultContextName = "Default";

struct GCF::Components::ClipboardComponentData
{
    ClipboardComponentData() : clipboardModel(0), clipboardWidget(0) { }

    QMap<QString, ClipboardDataList> contextStore;
	QStringList contextStoreKeys;

    GCF::Components::ClipboardModel* clipboardModel;
    QString activeContext;
    GCF::Components::ClipboardWidget* clipboardWidget;
	IXmlUtils* xmlUtils;
};

GCF::Components::ClipboardComponent & GCF::Components::ClipboardComponent::instance()
{
    static GCF::Components::ClipboardComponent* theInstance = GCF_CREATE_COMPONENT(ClipboardComponent);
    return *theInstance;
}

GCF::Components::ClipboardComponent::ClipboardComponent()
{
    d = new GCF::Components::ClipboardComponentData;
    d->clipboardModel = new GCF::Components::ClipboardModel(this);

    // add a default context
    addContext(defaultContextName);
}

GCF::Components::ClipboardComponent::~ClipboardComponent()
{
    delete d;
}

QObject* GCF::Components::ClipboardComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');
	GCF::Components::ClipboardComponent* that = const_cast<GCF::Components::ClipboardComponent*>(this);

    if(comps.last() == "controller")
        return that;

    if(comps.last() == "clipboardModel")
        return d->clipboardModel;

    return 0;
}

QWidget* GCF::Components::ClipboardComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "clipboardWidget")
    {
        if(!d->clipboardWidget)
            d->clipboardWidget = new GCF::Components::ClipboardWidget( qobject_cast<IClipboard*>(this) );
        return d->clipboardWidget;
    }

    return 0;
}

void GCF::Components::ClipboardComponent::initializeComponent()
{
    // Do nothing.
}

void GCF::Components::ClipboardComponent::finalizeComponent()
{
    // Store the current clipboard items into a XML file.
	// We prefer not to store it into GCF::Components::ClipboardComponent.xml because, that would
	// cause too many changes. Each change to SVN would then become expensive.

	// Lets save the data into a file within the GUIXML directory
	QString clipboardFile = QString("%1/Clipboard.xml").arg( GCF::AbstractComponent::guiXmlFileDirectory() );

	// Create a QDomDocument and store clipboard information into it
	QDomDocument doc("Clipboard");
	QDomElement rootE = doc.createElement("Clipboard");
	doc.appendChild(rootE);

	for(int i=0; i<d->contextStoreKeys.count(); i++)
	{
		QString ctx = d->contextStoreKeys[i];

		// Store a context element
		QDomElement contextE = doc.createElement("Context");
		rootE.appendChild(contextE);
		contextE.setAttribute("Name", ctx);

		// Store data items in it.
		ClipboardDataList dataList = d->contextStore[ctx];
		for(int i=0; i<dataList.count(); i++)
		{
			ClipboardData data = dataList[i];

			QVariantList list = QVariantList() << data.Data << data.Description << data.Icon.pixmap(64,64);
			QByteArray listBa;
			QDataStream ds(&listBa, QIODevice::WriteOnly);
			GCF::Message::encodeVariantList(ds, list);

			QDomElement dataE = doc.createElement("Data");
			contextE.appendChild(dataE);

			d->xmlUtils->saveData(doc, dataE, listBa);
		}
	}

	// Now save the clipboard XML into the file
	QFile file(clipboardFile);
	file.open(QFile::WriteOnly);

	QString xml = doc.toString();
	file.write(xml.toAscii());
	file.close();
}

void GCF::Components::ClipboardComponent::creationAnnounced()
{
	GCF::AbstractComponent* comp = 0;
	QString name;
	QObject* xmlUtilsObj = findObject("IXmlUtils", name, &comp);
	if(xmlUtilsObj)
	{
		d->xmlUtils = qobject_cast<IXmlUtils*>(xmlUtilsObj);
		GCF::AbstractComponent::addDependency(comp, this);
	}

	// Load the previously stored XML if possible
	QString clipboardFile = QString("%1/Clipboard.xml").arg( GCF::AbstractComponent::guiXmlFileDirectory() );
	QFile file(clipboardFile);
	file.open(QFile::ReadOnly);

	QDomDocument doc("Clipboard");
	bool success = doc.setContent(&file);
	file.close();

    if(!success)
        return;

	// Root element
	QDomElement rootE = doc.documentElement();

	// Now read contexts from the Clipboard XML document
	QList<QDomElement> contextEList = d->xmlUtils->findElements(rootE, "Context");
	for(int i=0; i<contextEList.count(); i++)
	{
		QDomElement contextE = contextEList[i];
		QString ctx = contextE.attribute("Name");
		if(ctx == defaultContextName)
			activateDefaultContext();
		else
			addContext(ctx);

		// Now add data elements into the context
		QList<QDomElement> dataEList = d->xmlUtils->findElements(contextE, "Data");
		for(int j=0; j<dataEList.count(); j++)
		{
			QDomElement dataE = dataEList[j];
			QByteArray ba;
			d->xmlUtils->loadData(dataE, ba);

			QDataStream ds(&ba, QIODevice::ReadOnly);
			QVariantList list;
			GCF::Message::extractVariantList(ds, list);

			ClipboardData data( QIcon(list[2].value<QPixmap>()), list[0], list[1].toString() );
			addData(data);
		}
	}
}

QObject* GCF::Components::ClipboardComponent::containerObject()
{
    return this;
}

QString GCF::Components::ClipboardComponent::productName() const
{
    return "Clipboard";
}

QString GCF::Components::ClipboardComponent::organization() const
{
    return "VCreate Logic Pvt. Ltd.";
}

QImage GCF::Components::ClipboardComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::ClipboardComponent::version() const
{
    return "1.0";
}

QString GCF::Components::ClipboardComponent::shortDescription() const
{
    return "Provides a multi-context, resizable clipboard";
}

QString GCF::Components::ClipboardComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::ClipboardComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/gcf/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::ClipboardComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, Vision, Design, Development");
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::ClipboardComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::ClipboardComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::ClipboardComponent::copyrightStatement() const
{
    return QString();
}

void GCF::Components::ClipboardComponent::addContext(const QString& contextName)
{
    if(!d->contextStore.contains(contextName))
    {
        ClipboardDataList list;
        d->contextStore[contextName] = list;
		d->contextStoreKeys.append(contextName);
    }

    emit contextAdded(contextName);

    activateContext(contextName);
}

void GCF::Components::ClipboardComponent::removeContext(const QString& contextName)
{
    if(!d->contextStore.contains(contextName) || contextName == defaultContextName)
        return;

    d->contextStore.remove(contextName);
	d->contextStoreKeys.removeAll(contextName);
    emit contextRemoved(contextName);

    // activate the default context
    activateDefaultContext();
}

QString GCF::Components::ClipboardComponent::context(int index) const
{
    if( index < 0 || index >= d->contextStoreKeys.count() )
        return QString();

    return d->contextStoreKeys[index];
}

int GCF::Components::ClipboardComponent::contextCount() const
{
    return d->contextStoreKeys.count();
}

void GCF::Components::ClipboardComponent::activateContext(const QString& name)
{
    if(name.isEmpty())
        return;

    if(d->activeContext == name)
        return;

    if(!d->contextStore.contains(name))
        return;

    d->activeContext = name;
    ClipboardDataList dataList = d->contextStore[name];
    d->clipboardModel->setClipboardDataList(dataList);

    emit activeContextChanged(name);
}

void GCF::Components::ClipboardComponent::activateContext(int contextIndex)
{
    QString ctxName = this->context(contextIndex);
    activateContext(ctxName);
}

void GCF::Components::ClipboardComponent::activateDefaultContext()
{
    activateContext(defaultContextName);
}

QString GCF::Components::ClipboardComponent::activeContextName() const
{
    return d->activeContext;
}

int GCF::Components::ClipboardComponent::activeContextIndex() const
{
    QStringList keys = d->contextStore.keys();

    return keys.indexOf(d->activeContext);
}

bool GCF::Components::ClipboardComponent::isDefaultContextActive() const
{
    return (d->activeContext == defaultContextName);
}

void GCF::Components::ClipboardComponent::addData(const ClipboardData& data)
{
    if(d->activeContext.isEmpty())
        activateDefaultContext();

    ClipboardDataList& dataList = d->contextStore[d->activeContext];
    dataList.prepend(data);
    d->clipboardModel->setClipboardDataList(dataList);

    emit dataAdded(data);
}

bool GCF::Components::ClipboardComponent::removeDataAt(int index)
{
    if(d->activeContext.isEmpty())
        activateDefaultContext();

    ClipboardDataList& dataList = d->contextStore[d->activeContext];
    if(index < 0 || index >= dataList.count())
        return false;

    dataList.removeAt(index);
    d->clipboardModel->setClipboardDataList(dataList);

    emit dataRemoved(index);
	return true;
}

bool GCF::Components::ClipboardComponent::removeAll()
{
    if(d->activeContext.isEmpty())
        activateDefaultContext();

    ClipboardDataList& dataList = d->contextStore[d->activeContext];
    dataList.clear();
    d->clipboardModel->setClipboardDataList(dataList);

    emit dataCleared();
	return true;
}

int GCF::Components::ClipboardComponent::dataCount() const
{
    if(d->activeContext.isEmpty())
        return 0;

    ClipboardDataList& dataList = d->contextStore[d->activeContext];
    return dataList.count();
}

ClipboardData GCF::Components::ClipboardComponent::data(int index) const
{
    if(d->activeContext.isEmpty())
        return ClipboardData();

    ClipboardDataList& dataList = d->contextStore[d->activeContext];
    if(index < 0 || index >= dataList.count())
        return ClipboardData();

    return dataList[index];
}

QAbstractListModel* GCF::Components::ClipboardComponent::activeClipboardModel() const
{
    return d->clipboardModel;
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(ClipboardComponent)
