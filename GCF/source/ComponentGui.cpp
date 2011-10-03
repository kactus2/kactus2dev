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

#include <GCF/ComponentGui.h>

#include <QUrl>
#include <QDir>
#include <QList>
#include <QFile>
#include <QMenu>
#include <QVector>
#include <QString>
#include <QAction>
#include <QWidget>
#include <QObject>
#include <QToolBar>
#include <QDomAttr>
#include <QMenuBar>
#include <QMetaObject>
#include <QMetaMethod>
#include <QDomElement>
#include <QDomDocument>
#include <QMainWindow>
#include <QMetaType>

#include <GCF/MenuStrip.h>
#include <GCF/AbstractComponent.h>
#include <GCF/IComponentHostWindow.h>
#include <GCF/ComponentFactory.h>
#include <GCF/SignalMultiplexer.h>
#include <GCF/ActionContainerWidget.h>
#include <GCF/ActionDocumentMapper.h>

#ifdef GCF_ACTIVEQT_AVAILABLE
#include <QAxWidget>
#include "GCF/IActiveXFactory.h"
#endif

#ifdef GCF_KPARTS_AVAILABLE
#include "GCF/IKPartsFactory.h"
#include "kparts/part.h"
#include "kmimetypetrader.h"
#include "kservicetypetrader.h"
#include "kxmlguiclient.h"
#include "kactioncollection.h"
#include "kcomponentdata.h"
#include "kurl.h"

namespace GCF
{
    struct KPartsData
    {
        KPartsData() : part(0), xmlGuiClient(0),
            partRootNodeCreated(false) { }

        KParts::Part* part;
        KXMLGUIClient* xmlGuiClient;
        bool partRootNodeCreated;
        GCF::ComponentGuiNode partRootNode; // for menu-strip or menu-bar
        QList<GCF::ComponentGuiNode> partNodes;
    };
}

Q_DECLARE_METATYPE(GCF::KPartsData);

#endif

Q_DECLARE_METATYPE(GCF::MenuStrip*)
Q_DECLARE_METATYPE(GCF::MenuStripGroup*)

namespace GCF
{
    struct ComponentGuiNodeData : public AbstractSharedData
    {
        ComponentGuiNodeData() : parentNode(0) { }

        ~ComponentGuiNodeData() {
            if(parentNode)
                delete parentNode;
        }

        ComponentGuiNode* parentNode;
        ComponentGui gui;
        DataStore dataStore;
        QList<ComponentGuiNode> nodeList;
        int type;
        bool inDestructor;
        QString name;
    };

}

GCF::ComponentGuiNode::ComponentGuiNode()
{
    d = new GCF::ComponentGuiNodeData;
    d->type = Other;
}

GCF::ComponentGuiNode::ComponentGuiNode(const GCF::ComponentGuiNode & other)
{
    d = other.d;
    d->ref();
}

GCF::ComponentGuiNode::~ComponentGuiNode()
{
    d->deref();
}

bool GCF::ComponentGuiNode::operator == (const GCF::ComponentGuiNode & other) const
{
    return d == other.d;
}

GCF::ComponentGuiNode & GCF::ComponentGuiNode::operator = (const GCF::ComponentGuiNode & other)
{
    d->deref();
    d = other.d;
    d->ref();
    return *this;
}

GCF::ComponentGuiNode GCF::ComponentGuiNode::parentNode() const
{
    if(d->parentNode)
        return *(d->parentNode);
    return GCF::ComponentGuiNode();
}

bool GCF::ComponentGuiNode::isValid() const
{
    return d->gui.component() != 0;
}

GCF::DataStore GCF::ComponentGuiNode::nodeData() const
{
    return d->dataStore;
}

QVariant & GCF::ComponentGuiNode::operator [] (const QString& key)
{
    return d->dataStore[key];
}

bool GCF::ComponentGuiNode::hasKey(const QString& key)
{
    return d->dataStore.hasKey(key);
}

void GCF::ComponentGuiNode::removeKey(const QString& key)
{
    return d->dataStore.removeKey(key);
}

QStringList GCF::ComponentGuiNode::keys() const
{
    return d->dataStore.keys();
}

void GCF::ComponentGuiNode::setType(int t)
{
    d->type = t;
}

int GCF::ComponentGuiNode::type() const
{
    return d->type;
}

void GCF::ComponentGuiNode::setGui(const GCF::ComponentGui & gui)
{
    d->gui = gui;
}

const GCF::ComponentGui & GCF::ComponentGuiNode::gui() const
{
    return d->gui;
}

void GCF::ComponentGuiNode::setName(const QString& name) const
{
    d->name = name;
}

QString GCF::ComponentGuiNode::name() const
{
    return d->name;
}

QString GCF::ComponentGuiNode::completeName() const
{
    QString ret;
    GCF::ComponentGuiNode node = *this;

    if(!isValid())
        return ret;

    while(node.isValid())
    {
        if(ret.isEmpty())
            ret = node.name();
        else
            ret = QString("%1.%2").arg(node.name()).arg(ret);
        node = node.parentNode();
    }

    ret = QString("%1.%2").arg(component()->componentName()).arg(ret);
    return ret;
}

GCF::AbstractComponent* GCF::ComponentGuiNode::component() const
{
    return d->gui.component();
}

void GCF::ComponentGuiNode::addChild(GCF::ComponentGuiNode child)
{
    d->nodeList.append(child);
    child.d->parentNode = new GCF::ComponentGuiNode(*this);
    child.setGui(d->gui);
    GCF::NodeInitializerFactory::instance().initializeNode(child);
}

void GCF::ComponentGuiNode::removeChild(GCF::ComponentGuiNode child)
{
    child.clear();
    if(child.parentNode() == *this)
    {
        delete child.d->parentNode;
        child.d->parentNode = 0;
    }
    child.setGui(GCF::ComponentGui());
    d->nodeList.removeAll(child);
}

int GCF::ComponentGuiNode::childCount()
{
    return d->nodeList.count();
}

GCF::ComponentGuiNode GCF::ComponentGuiNode::child(int index) const
{
    return d->nodeList[index];
}

void GCF::ComponentGuiNode::showNode()
{
    GCF::NodeInitializerFactory::instance().showNode(*this);
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].showNode();
}

void GCF::ComponentGuiNode::hideNode()
{
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].hideNode();
    GCF::NodeInitializerFactory::instance().hideNode(*this);
}

void GCF::ComponentGuiNode::clear()
{
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].clear();
    d->nodeList.clear();
    GCF::NodeInitializerFactory::instance().cleanupNode(*this);
    d->dataStore.clear();
}

GCF::ComponentGuiNode GCF::ComponentGuiNode::findNode(const QString& completeName) const
{
    if(this->completeName() == completeName)
        return* this;

    QStringList scopes = completeName.split('.');
    if(!scopes.contains(d->name))
        return GCF::ComponentGuiNode();

    for(int i=0; i<d->nodeList.count(); i++)
    {
        GCF::ComponentGuiNode node = d->nodeList[i].findNode(completeName);
        if(node.isValid())
            return node;
    }
    return GCF::ComponentGuiNode();
}

/****************************************************************************
GCF::ComponentGui Implementation
****************************************************************************/

struct GCF::ComponentGuiData : public GCF::AbstractSharedData
{
    ComponentGuiData() : component(0) { }

    GCF::AbstractComponent* component;
    QList<GCF::ComponentGuiNode> nodeList;
    GCF::DataStore dataStore;
};

GCF::ComponentGui::ComponentGui()
{
    d = new GCF::ComponentGuiData;
}

GCF::ComponentGui::ComponentGui(const GCF::ComponentGui & other)
{
    d = other.d;
    d->ref();
}

GCF::ComponentGui::~ComponentGui()
{
    d->deref();
}

bool GCF::ComponentGui::operator == (const GCF::ComponentGui & other) const
{
    return d == other.d;
}

GCF::ComponentGui & GCF::ComponentGui::operator = (const GCF::ComponentGui & other)
{
    d->deref();
    d = other.d;
    d->ref();
    return *this;
}

void GCF::ComponentGui::setComponent(GCF::AbstractComponent* comp)
{
    d->component = comp;
}

GCF::AbstractComponent* GCF::ComponentGui::component() const
{
    return d->component;
}

GCF::DataStore GCF::ComponentGui::guiData()
{
    return d->dataStore;
}

QVariant & GCF::ComponentGui::operator [] (const QString& key)
{
    return d->dataStore[key];
}

bool GCF::ComponentGui::hasKey(const QString& key)
{
    return d->dataStore.hasKey(key);
}

void GCF::ComponentGui::removeKey(const QString& key)
{
    return d->dataStore.removeKey(key);
}

QStringList GCF::ComponentGui::keys() const
{
    return d->dataStore.keys();
}

void GCF::ComponentGui::addNode(GCF::ComponentGuiNode node)
{
    d->nodeList.append(node);
    node.setGui(*this);
    GCF::NodeInitializerFactory::instance().initializeNode(node);
}

void GCF::ComponentGui::removeNode(GCF::ComponentGuiNode node)
{
    GCF::NodeInitializerFactory::instance().cleanupNode(node);
    d->nodeList.removeAll(node);
    node.setGui(GCF::ComponentGui());
}

int GCF::ComponentGui::nodeCount() const
{
    return d->nodeList.count();
}

GCF::ComponentGuiNode GCF::ComponentGui::node(int index) const
{
    return d->nodeList[index];
}

void GCF::ComponentGui::showGui()
{
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].showNode();
}

void GCF::ComponentGui::hideGui()
{
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].hideNode();
}

void GCF::ComponentGui::clear()
{
    // for(int i=d->nodeList.count()-1; i >= 0; i--)
    for(int i=0; i<d->nodeList.count(); i++)
        d->nodeList[i].clear();
    d->nodeList.clear();
}

GCF::ComponentGuiNode GCF::ComponentGui::findNode(const QString& completeName, bool allComps) const
{
    if(!d->component)
        return GCF::ComponentGuiNode();

    QStringList scopes = completeName.split('.');
    if(scopes.first() == d->component->componentName())
    {
        for(int i=0; i<d->nodeList.count(); i++)
        {
            GCF::ComponentGuiNode node2 = d->nodeList[i];
            GCF::ComponentGuiNode node = node2.findNode(completeName);
            if(node.isValid())
                return node;
        }
    }

    if(allComps)
    {
        QList<GCF::AbstractComponent*> comps = GCF::ComponentFactory::instance().components();
        for(int i=0; i<comps.count(); i++)
        {
            GCF::AbstractComponent* comp = comps[i];
            if(comp == d->component)
                continue;

            GCF::ComponentGui gui = comp->componentGui();
            GCF::ComponentGuiNode ret = gui.findNode(completeName);
            if(ret.isValid())
                return ret;
        }
    }

    return GCF::ComponentGuiNode();
}

void GCF::ComponentGui::dumpGui(const QString& fileName)
{
    QDomDocument doc("ComponentGuiDump");
    QDomElement rootE = doc.createElement("component");
    doc.appendChild(rootE);
    rootE.setAttribute("name", d->component->componentName());

    for(int i=0; i<d->nodeList.count(); i++)
    {
        GCF::ComponentGuiNode node = d->nodeList[i];
        dumpGuiNode(node, doc, rootE);
    }

    QString xmlStr = doc.toString(2);
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    file.write(xmlStr.toAscii());
    file.close();
}

void GCF::ComponentGui::dumpGuiNode(GCF::ComponentGuiNode node, QDomDocument doc, QDomElement parentE)
{
    QString eName;
    switch(node.type())
    {
    case GCF::ComponentGuiNode::Object:
        eName = "object"; break;
    case GCF::ComponentGuiNode::Widget:
        eName = "widget"; break;
    case GCF::ComponentGuiNode::Action:
        eName = "action"; break;
    case GCF::ComponentGuiNode::Menu:
        eName = "menu"; break;
    case GCF::ComponentGuiNode::Toolbar:
        eName = "toolbar"; break;
    case GCF::ComponentGuiNode::MenuItem:
        eName = "menuitem"; break;
    case GCF::ComponentGuiNode::ToolbarItem:
        eName = "toolbaritem"; break;
    case GCF::ComponentGuiNode::GroupItem:
        eName = "group"; break;
    case GCF::ComponentGuiNode::GroupActionItem:
        eName = "groupitem"; break;
    case GCF::ComponentGuiNode::Connection:
        eName = "connection"; break;
    case GCF::ComponentGuiNode::Property:
        eName = "property"; break;
    case GCF::ComponentGuiNode::Method:
        eName = "method"; break;
    case GCF::ComponentGuiNode::ActiveX:
        eName = "activex"; break;
    case GCF::ComponentGuiNode::KPart:
        eName = "kpart"; break;
    case GCF::ComponentGuiNode::ActionContainer:
        eName = "actioniontainer"; break;
    case GCF::ComponentGuiNode::ActionContainerItem:
        eName = "actioniontaineritem"; break;
    default:
        eName = "unknown";
    }

    QDomElement nodeE = doc.createElement(eName);
    parentE.appendChild(nodeE);

    if(!node.name().isEmpty())
        nodeE.setAttribute("name", node.name());

    QStringList keys = node.keys();
    for(int i=0; i<keys.count(); i++)
    {
        QString key = keys[i];
        QString value = node[key].toString();
        if(value.isEmpty())
            continue;

        nodeE.setAttribute(key, value);
    }

    for(int i=0; i<node.childCount(); i++)
    {
        GCF::ComponentGuiNode childNode = node.child(i);
        dumpGuiNode(childNode, doc, nodeE);
    }
}

/****************************************************************************
GCF::AbstractNodeInitializer Implementation
****************************************************************************/

GCF::AbstractNodeInitializer::AbstractNodeInitializer()
{

}

GCF::AbstractNodeInitializer::~AbstractNodeInitializer()
{

}

QAction* GCF::AbstractNodeInitializer::fetchAction(GCF::ComponentGuiNode node) const
{
    if(!node.component())
        return 0;

#ifdef GCF_KPARTS_AVAILABLE
    if(node.hasKey("KXMLGUIClient"))
    {
        KXMLGUIClient* xmlGuiClient = node["KXMLGUIClient"].value<KXMLGUIClient*>();
        QString actualName = node["actualName"].toString();
        QAction* action = xmlGuiClient->action( qPrintable(actualName) );
        node["FactoryEnable"] = action ? action->isEnabled() : true;
        if(action)
        {
            QString text = action->text();
            // Note from Prashanth: I know that the next line is stupid.
            // But, the & seems to show up on the menu instead of a _ under the
            // character next to it.
            text = text.remove('&');
            if(!text.isEmpty())
                action->setIconText(text);
        }
        return action;
    }
#endif

    return node.component()->fetchAction(node.completeName());
}

QObject* GCF::AbstractNodeInitializer::fetchObject(GCF::ComponentGuiNode node) const
{
    if(!node.component())
        return 0;

    return node.component()->fetchObject(node.completeName());
}

QWidget* GCF::AbstractNodeInitializer::fetchWidget(GCF::ComponentGuiNode node) const
{
    if(!node.component())
        return 0;

    return node.component()->fetchWidget(node.completeName());
}

QMenu* GCF::AbstractNodeInitializer::fetchMenu(GCF::ComponentGuiNode node) const
{
    if(!node.component())
        return 0;

    return node.component()->fetchMenu(node.completeName());
}

QToolBar* GCF::AbstractNodeInitializer::fetchToolBar(GCF::ComponentGuiNode node) const
{
    if(!node.component())
        return 0;

    return node.component()->fetchToolBar(node.completeName());
}

#ifdef GCF_ACTIVEQT_AVAILABLE

QAxWidget* GCF::AbstractNodeInitializer::fetchActiveXWidget(GCF::ComponentGuiNode node) const
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return 0;

    QString control = node["control"].toString();
    QString compName = node.completeName();

    // First try the activeX factory implementation
    IActiveXFactory* axFactory = qobject_cast<IActiveXFactory*>(comp);
    if(axFactory)
        return axFactory->createActiveXWidget(compName, control);

    // Create it our-selves from the control string
    QAxWidget* widget = new QAxWidget;
    widget->setControl(control);
    return widget;
}

#endif

#ifdef GCF_KPARTS_AVAILABLE

KParts::Part* GCF::AbstractNodeInitializer::fetchPart(GCF::ComponentGuiNode node) const
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return 0;

    IKPartsFactory::PartCreationType type = IKPartsFactory::PartCreationType(node["PartCreationType"].toInt());
    QString typeStr = node["typeStr"].toString();
    QString compName = node.completeName();
    bool readOnly = node["readOnly"].toBool();
    if(type == IKPartsFactory::UnknownType || typeStr.isEmpty())
        return 0;

    IKPartsFactory* kpFactory = qobject_cast<IKPartsFactory*>(comp);
    if(kpFactory)
        return kpFactory->createPart(compName, type, typeStr, readOnly);

    KParts::Part* part = 0;
    QWidget* parentWidget = GCF::AbstractComponent::componentHostWidget();
    QString errorString;

    switch(type)
    {
    case IKPartsFactory::MimeType:
        if(readOnly)
            part = KMimeTypeTrader::createPartInstanceFromQuery<KParts::ReadOnlyPart>(typeStr, parentWidget, parentWidget /*comp*/,
                                                                                       QString(), QVariantList(), &errorString);
        else
            part = KMimeTypeTrader::createPartInstanceFromQuery<KParts::ReadWritePart>(typeStr, parentWidget, parentWidget /*comp*/,
                                                                                       QString(), QVariantList(), &errorString);
        if(!part || !errorString.isEmpty())
            qDebug("%s", qPrintable(errorString));
        break;
    case IKPartsFactory::ServiceType:
        if(readOnly)
            part = KServiceTypeTrader::createInstanceFromQuery<KParts::ReadOnlyPart>(typeStr, parentWidget, parentWidget /*comp*/,
                                                                                       QString(), QVariantList(), &errorString);
        else
            part = KServiceTypeTrader::createInstanceFromQuery<KParts::ReadWritePart>(typeStr, parentWidget, parentWidget /*comp*/,
                                                                                       QString(), QVariantList(), &errorString);
        if(!part || !errorString.isEmpty())
            qDebug("%s", qPrintable(errorString));
        break;
    case IKPartsFactory::LibraryName:
        qDebug("Loading KParts from library is not supported yet");
    }

    return part;
}

#endif

QObject* GCF::AbstractNodeInitializer::findObject(ComponentGuiNode node, const QString& completeName, AbstractComponent** comp)
{
    if(!node.component())
        return 0;

    QObject* ret = node.component()->findObject(completeName, comp);
    return ret;
}

void GCF::AbstractNodeInitializer::addChildObject(GCF::ComponentGuiNode node, QObject* parent, QObject* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->addChildObject(parent, child, hint);
}

void GCF::AbstractNodeInitializer::removeChildObject(GCF::ComponentGuiNode node, QObject* parent, QObject* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->removeChildObject(parent, child, hint);
}

void GCF::AbstractNodeInitializer::addChildWidget(GCF::ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->addChildWidget(parent, child, hint);
}

void GCF::AbstractNodeInitializer::removeChildWidget(GCF::ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->removeChildWidget(parent, child, hint);
}

void GCF::AbstractNodeInitializer::showChildWidget(GCF::ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->showChildWidget(parent, child, hint);
}

void GCF::AbstractNodeInitializer::hideChildWidget(GCF::ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint)
{
    if(!node.component())
        return;
    node.component()->hideChildWidget(parent, child, hint);
}

void GCF::AbstractNodeInitializer::addDependency(GCF::AbstractComponent* comp1, GCF::AbstractComponent* comp2)
{
    if(!comp1 || !comp2)
        return;

    comp1->addDependentComponent(comp2);
    comp2->addDependingComponent(comp1);
}

void GCF::AbstractNodeInitializer::removeDependency(GCF::AbstractComponent* comp1, GCF::AbstractComponent* comp2)
{
    if(!comp1 || !comp2)
        return;

    comp1->removeDependentComponent(comp2);
    comp2->removeDependingComponent(comp1);
}

GCF::ComponentGuiNode GCF::AbstractNodeInitializer::resolveCompleteSlotName(GCF::ComponentGuiNode node, const QString& completeSlotName, QObject** object, QString& slotName) const
{
    GCF::ComponentGuiNode nullNode;
    QString completeName = completeSlotName.section("::", 0, 0);
    slotName = completeSlotName.section("::", 1, 1);

    if(completeName.isEmpty() || slotName.isEmpty() || !object)
        return nullNode;

    *object = 0;
    GCF::ComponentGuiNode cnNode = node.gui().findNode(completeName, true);
    if(!cnNode.isValid())
        return nullNode;

    switch(cnNode.type())
    {
    case GCF::ComponentGuiNode::Object:
        if(cnNode.hasKey("QObject"))
            *object = cnNode["QObject"].value<QObject*>();
        break;
    case GCF::ComponentGuiNode::Widget:
        if(cnNode.hasKey("QWidget"))
            *object = cnNode["QWidget"].value<QWidget*>();
        break;
    case GCF::ComponentGuiNode::Action:
        if(cnNode.hasKey("QAction"))
            *object = cnNode["QAction"].value<QAction*>();
        break;
    case GCF::ComponentGuiNode::Menu:
        if(cnNode.hasKey("QMenu"))
            *object = cnNode["QMenu"].value<QMenu*>();
        break;
    case GCF::ComponentGuiNode::Toolbar:
        if(cnNode.hasKey("QToolBar"))
            *object = cnNode["QToolBar"].value<QToolBar*>();
        break;
#ifdef GCF_ACTIVEQT_AVAILABLE
    case GCF::ComponentGuiNode::ActiveX:
        if(cnNode.hasKey("QAxWidget"))
            *object = cnNode["QAxWidget"].value<QAxWidget*>();
        break;
#endif
#ifdef GCF_KPARTS_AVAILABLE
    case GCF::ComponentGuiNode::KPart:
        if(cnNode.hasKey("KParts::Part"))
            *object = cnNode["KParts::Part"].value<KParts::Part*>();
        break;
#endif
    }

    if(*object)
        return cnNode;

    slotName.clear();
    return nullNode;
}

GCF::ComponentGuiNode GCF::AbstractNodeInitializer::resolveCompletePropertyName(GCF::ComponentGuiNode node,
const QString& completePropertyName, QObject** object, QString& propertyName) const
{
    return resolveCompleteSlotName(node, completePropertyName, object, propertyName);
}

GCF::ComponentGuiNode GCF::AbstractNodeInitializer::resolveCompleteMethodName(GCF::ComponentGuiNode node,
const QString& completeMethodName, QObject** object, QString& methodName)
{
    return resolveCompleteSlotName(node, completeMethodName, object, methodName);
}

/****************************************************************************
GCF::NodeInitializerFactory Implementation
****************************************************************************/

struct GCF::NodeInitializerFactoryData
{
    QMap<int, GCF::AbstractNodeInitializer*> initMap;
};

GCF::NodeInitializerFactory & GCF::NodeInitializerFactory::instance()
{
    static GCF::NodeInitializerFactory theInstance;
    return theInstance;
}

GCF::NodeInitializerFactory::NodeInitializerFactory()
{
    d = new GCF::NodeInitializerFactoryData;

    setInitializer(GCF::ComponentGuiNode::Object, new GCF::ObjectNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Widget, new GCF::WidgetNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Action, new GCF::ActionNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Menu, new GCF::MenuNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::MenuItem, new GCF::MenuItemNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Toolbar, new GCF::ToolBarNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::ToolbarItem, new GCF::ToolBarItemNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::GroupItem, new GCF::MenuStripGroupNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::GroupActionItem, new GCF::MenuStripGroupActionNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Connection, new GCF::ConnectionNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Property, new GCF::PropertyNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::Method, new GCF::MethodNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::ActiveX, new GCF::ActiveXNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::KPart, new GCF::KPartNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::GroupDrawerWidgetItem, new GCF::GroupDrawerWidgetNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::ActionContainer, new GCF::ActionContainerNodeInitializer);
    setInitializer(GCF::ComponentGuiNode::ActionContainerItem, new GCF::ActionContainerItemNodeInitializer);
}

GCF::NodeInitializerFactory::~NodeInitializerFactory()
{
    qDeleteAll(d->initMap.values());
    delete d;
}

QList<int> GCF::NodeInitializerFactory::handledTypes() const
{
    return d->initMap.keys();
}

GCF::AbstractNodeInitializer* GCF::NodeInitializerFactory::initializer(int type) const
{
    if(d->initMap.contains(type))
        return d->initMap[type];
    return 0;
}

void GCF::NodeInitializerFactory::initializeNode(GCF::ComponentGuiNode node)
{
    /*
    QString nodeName = node.completeName();
    printf("Initializing Node \"%s\" of type %d\n", qPrintable(nodeName), node.type());
    fflush(stdout);
    */

    if(!GCF::AbstractComponent::componentHostWindow())
        return;
    GCF::AbstractNodeInitializer* i = initializer(node.type());
    if(i)
        i->initializeNode(node);
}

void GCF::NodeInitializerFactory::cleanupNode(GCF::ComponentGuiNode node)
{
    /*
    QString nodeName = node.completeName();
    printf("Cleaning Up Node \"%s\" of type %d\n", qPrintable(nodeName), node.type());
    fflush(stdout);
    */

    if(!GCF::AbstractComponent::componentHostWindow())
        return;
    GCF::AbstractNodeInitializer* i = initializer(node.type());
    if(i)
        i->cleanupNode(node);
}

void GCF::NodeInitializerFactory::showNode(GCF::ComponentGuiNode node)
{
    if(!GCF::AbstractComponent::componentHostWindow())
        return;
    GCF::AbstractNodeInitializer* i = initializer(node.type());
    if(i)
        i->showNode(node);
}

void GCF::NodeInitializerFactory::hideNode(GCF::ComponentGuiNode node)
{
    if(!GCF::AbstractComponent::componentHostWindow())
        return;
    GCF::AbstractNodeInitializer* i = initializer(node.type());
    if(i)
        i->hideNode(node);
}

void GCF::NodeInitializerFactory::setInitializer(int type, GCF::AbstractNodeInitializer* i)
{
    if(d->initMap.contains(type))
        delete d->initMap[type];

    if(!i)
    {
        d->initMap.remove(type);
        return;
    }

    d->initMap[type] = i;
}

/****************************************************************************
GCF::ObjectNodeInitializer Implementation
****************************************************************************/

int GCF::ObjectNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Object;
}

void GCF::ObjectNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Create the QObject
    QString objectName = node.completeName();
    QObject* objectPtr = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : fetchObject(node);
    if(!objectPtr)
    {
        qWarning("Node %s cannot be initialized", objectName.toAscii().data());
        return;
    }

    if(!node.hasKey("QObject"))
        node["QObject"] = qVariantFromValue<QObject*>(objectPtr);

    // Set the object name
    objectPtr->setObjectName(objectName);

    // Store the title in the object.
    QString objectTitle = node["title"].toString();
    objectPtr->setProperty("_title_", objectTitle);

    // Now find out the parent object.
    GCF::ComponentGuiNode parentObjectNode;
    QString parentObjectName = node["parentObject"].toString();
    QString hint = node["parentObjectHint"].toString();
    if(!parentObjectName.isEmpty())
    {
        parentObjectNode = node.gui().findNode(parentObjectName, true);

        GCF::AbstractComponent* parentObjectComp = 0;
        QObject* parentObject = findObject(node, parentObjectName, &parentObjectComp);

        if(parentObject && parentObjectComp)
        {
            addChildObject(parentObjectNode, parentObject, objectPtr, hint);
            addDependency(parentObjectComp, comp);
            node["parentObjectNode"] = qVariantFromValue<GCF::ComponentGuiNode>(parentObjectNode);
        }
        else
            qWarning("Parent object or its component was not found for %s", objectName.toAscii().data());
    }

    // Store the node as a property value in QObject
    objectPtr->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    QObject::connect(objectPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
}

void GCF::ObjectNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch the pointer to the QObject in the node
    if(!node.hasKey("QObject"))
        return;
    QObject* objectPtr = node["QObject"].value<QObject*>();
    if(!objectPtr)
        return;

    // Find out the parent object node and drop dependencies.
    QString hint = node["parentObjectHint"].toString();
    if(node.hasKey("parentObjectNode"))
    {
        GCF::ComponentGuiNode parentObjectNode = node["parentObjectNode"].value<GCF::ComponentGuiNode>();
        GCF::AbstractComponent* parentObjectComp = 0;
        QString parentObjectName = node["parentObject"].toString();
        QObject* parentObject = findObject(node, parentObjectName, &parentObjectComp);
        if(parentObject && parentObjectComp)
        {
            removeChildObject(parentObjectNode, parentObject, objectPtr, hint);
            removeDependency(parentObjectComp, comp);
        }
        node["parentObjectNode"] = qVariantFromValue<GCF::ComponentGuiNode>(GCF::ComponentGuiNode());
    }

    // Delete the object
    node["QObject"] = qVariantFromValue<QObject*>(0);
    QObject::disconnect(objectPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
    if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
        return;

    if(objectPtr != node.component())
        delete objectPtr;
}

void GCF::ObjectNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    // Does not make sense here
    Q_UNUSED(node);
}

void GCF::ObjectNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    // Does not make sense here
    Q_UNUSED(node);
}

/****************************************************************************
GCF::WidgetNodeInitializer Implementation
****************************************************************************/

int GCF::WidgetNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Widget;
}

void GCF::WidgetNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Create the QWidget
    QString widgetName = node.completeName();
    QWidget* widgetPtr = fetchWidget(node);
    if(!widgetPtr)
    {
        qWarning("Node %s cannot be initialized", widgetName.toAscii().data());
        return;
    }
    node["QWidget"] = qVariantFromValue<QWidget*>(widgetPtr);

    // Set the widget name
    widgetPtr->setObjectName(widgetName);

    // Store the title in the widget.
    QString widgetTitle = node["title"].toString();
    widgetPtr->setProperty("_title_", widgetTitle);
    widgetPtr->setWindowTitle(widgetTitle);

    // Now find out the parent widget.
    GCF::ComponentGuiNode parentWidgetNode;
    QString parentWidgetName = node["parentWidget"].toString();
    QString hint = node["parentWidgetHint"].toString();
    if(!parentWidgetName.isEmpty())
    {
        parentWidgetNode = node.gui().findNode(parentWidgetName, true);
        if(parentWidgetNode.isValid())
        {
            QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
            GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
            if(parentWidget && parentWidgetComp)
            {
                addChildWidget(parentWidgetNode, parentWidget, widgetPtr, hint);
                addDependency(parentWidgetComp, comp);
                node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(parentWidgetNode);
            }
            else
                qWarning("Parent widget or its component was not found for %s", widgetName.toAscii().data());
        }
        else
            qWarning("Invalid parent widget specified for %s", widgetName.toAscii().data());
    }

    // Store the node as a property value in QObject
    widgetPtr->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    QObject::connect(widgetPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));

    // Show the widget
    if(node.hasKey("visible") && !node["visible"].toBool())
        return;
    widgetPtr->show();
}

void GCF::WidgetNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch the pointer to the QWidget in the node
    if(!node.hasKey("QWidget"))
        return;
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    // Find out the parent widget node and drop dependencies.
    QString hint = node["parentWidgetHint"].toString();
    if(node.hasKey("parentWidgetNode"))
    {
        GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
        QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
        GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
        if(parentWidgetComp)
        {
            removeChildWidget(parentWidgetNode, parentWidget, widgetPtr, hint);
            removeDependency(parentWidgetComp, comp);
        }
        node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(GCF::ComponentGuiNode());
    }

    // Delete the widget
    node["QWidget"] = qVariantFromValue<QWidget*>(0);
    QObject::disconnect(widgetPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
    if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
        return;

    delete widgetPtr;
}

void GCF::WidgetNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    if(node.hasKey("UsedInMenuStrip") && node["UsedInMenuStrip"].toBool())
        widgetPtr->setEnabled(true);
    else
    {
        QString hint = node["parentWidgetHint"].toString();
        GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
        QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
        if(parentWidgetNode.isValid() && parentWidgetPtr)
            showChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
        else
            widgetPtr->show();
    }
}

void GCF::WidgetNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    if(node.hasKey("UsedInMenuStrip") && node["UsedInMenuStrip"].toBool())
        widgetPtr->setEnabled(false);
    else
    {
        QString hint = node["parentWidgetHint"].toString();
        GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
        QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
        if(parentWidgetNode.isValid() && parentWidgetPtr)
            hideChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
        else
            widgetPtr->hide();
    }
}

/****************************************************************************
GCF::ActionNodeInitializer Implementation
****************************************************************************/

int GCF::ActionNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Action;
}

void GCF::ActionNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    GCF::SignalMultiplexer* actionMx = 0;
    QAction* actionPtr = 0;

    // Find out if the action node needs to be multiplexed with any other action.
    QString mergeWith = node["mergeWith"].toString();
    if(!mergeWith.isNull())
    {
        GCF::ComponentGuiNode mergeWithNode = node.gui().findNode(mergeWith, true);
        if(mergeWithNode.isValid())
        {
            actionPtr = mergeWithNode["QAction"].value<QAction*>();
            actionMx = mergeWithNode["GCF::SignalMultiplexer"].value<GCF::SignalMultiplexer*>();
        }
        if(!actionMx)
            actionPtr = 0;
    }

    if(!actionPtr)
    {
        QString actionName = node.completeName();
        actionPtr = fetchAction(node);
        if(!actionPtr)
        {
            qWarning("Cannot initialize node %s", actionName.toAscii().data());
            return;
        }

        actionPtr->setObjectName(actionName);
        actionPtr->setText(node["title"].toString());
        if(node.hasKey("icon"))
        {
            actionPtr->setIcon(QIcon(node["icon"].toString()));
            actionPtr->setIconText(node["title"].toString());
        }
        if(node.hasKey("shortcut"))
            actionPtr->setShortcut(QKeySequence(node["shortcut"].toString()));
        if(node.hasKey("checkState"))
        {
            actionPtr->setCheckable(true);
            actionPtr->setChecked(node["checkState"].toBool());
        }

        actionMx = new GCF::SignalMultiplexer(actionPtr, actionPtr);
        actionPtr->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));

        if(node.hasKey("hideWhenNotInUse"))
            actionPtr->setProperty("hideWhenNotInUse", node["hideWhenNotInUse"].toBool());
        else
            actionPtr->setProperty("hideWhenNotInUse", true);
    }
    else
    {
        GCF::ComponentGuiNode actionNode = actionPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!actionNode.isValid())
            actionNode = node.gui().findNode(actionPtr->objectName(), true);
        if(!actionNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* actionNodeComp = actionNode.component();
        addDependency(actionNodeComp, comp);
    }

    if(!actionPtr || !actionMx)
        return;

    // Find out if the action has a URL attribute
    QString url = node.hasKey("url") ? node["url"].toString() : QString();

    // If there is a url attribute, get hold of the ActionDocumentMapper object
    GCF::ActionDocumentMapper* mapper = 0;
    if(!url.isEmpty())
    {
        QString admNodeName = QString("%1.actionDocumentMapper").arg( node.gui().component()->componentName() );

        // First look for actionDocumentMapper
        GCF::ComponentGuiNode admNode = node.gui().findNode(admNodeName);
        if(admNode.isValid())
        {
            QObject* obj = admNode["QObject"].value<QObject*>();
            mapper = qobject_cast<GCF::ActionDocumentMapper*>(obj);
        }
        else
        {
            // If actionDocumentMapper doesnt already exist for the component,
            // then add it now!
            mapper = new GCF::ActionDocumentMapper(node.gui().component());
            admNode.setType(GCF::ComponentGuiNode::Object);
            admNode.setName("actionDocumentMapper");
            admNode["autoDelete"] = false;
            admNode["QObject"] = qVariantFromValue<QObject*>( (QObject*)(mapper) );
            
            GCF::ComponentGui gui = admNode.gui();
            gui.addNode(admNode);
        }
    }

    // Store the default property of the action into the QAction object.
    // This is used while creating drop down menu actions.
    if( node.hasKey("default") )
        actionPtr->setProperty( "_default_", node["default"].toBool() );

    // Store action and mx pointers in the node.
    node["QAction"] = qVariantFromValue<QAction*>(actionPtr);
    node["GCF::SignalMultiplexer"] = qVariantFromValue<GCF::SignalMultiplexer*>(actionMx);

    // Perform signal/slot connections
    QString triggerSlot = node["triggerSlot"].toString();
    QString toggleSlot = node["toggleSlot"].toString();
    QObject* triggerRx = 0, *toggleRx = 0;
    GCF::ComponentGuiNode triggerSlotNode = resolveCompleteSlotName(node, triggerSlot, &triggerRx, triggerSlot);
    GCF::ComponentGuiNode toggleSlotNode = resolveCompleteSlotName(node, toggleSlot, &toggleRx, toggleSlot);
    GCF::AbstractComponent* triggerSlotComp = triggerSlotNode.component();
    GCF::AbstractComponent* toggleSlotComp = toggleSlotNode.component();

    // Make connections and make a note of the receivers
    if(triggerRx)
    {
        actionMx->makeConnection(SIGNAL(triggered(bool)), triggerRx, triggerSlot.toAscii().data());
        node["triggerRx"] = qVariantFromValue<QObject*>(triggerRx);
        if(triggerSlotComp)
        {
            addDependency(triggerSlotComp, comp);
            node["triggerSlotNode"] = qVariantFromValue<GCF::ComponentGuiNode>(triggerSlotNode);
        }
    }

    if(toggleRx)
    {
        actionMx->makeConnection(SIGNAL(toggled(bool)), toggleRx, toggleSlot.toAscii().data());
        node["toggleRx"] = qVariantFromValue<QObject*>(toggleRx);
        if(toggleSlotComp)
        {
            addDependency(toggleSlotComp, comp);
            node["toggleSlotNode"] = qVariantFromValue<GCF::ComponentGuiNode>(toggleSlotNode);
        }
    }

    if(mapper)
    {
        mapper->mapAction(actionPtr, url);
        actionMx->makeConnection(SIGNAL(triggered(bool)), mapper, SLOT(map()));
    }

    QObject::connect(actionPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
}

void GCF::ActionNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* actionPtr = node["QAction"].value<QAction*>();
    GCF::SignalMultiplexer* actionMx = node["GCF::SignalMultiplexer"].value<GCF::SignalMultiplexer*>();
    QObject* triggerRx = node.hasKey("triggerRx") ? node["triggerRx"].value<QObject*>() : 0;
    QObject* toggleRx = node.hasKey("toggleRx") ? node["toggleRx"].value<QObject*>() : 0;
    GCF::ComponentGuiNode triggerSlotNode = node["triggerSlotNode"].value<GCF::ComponentGuiNode>();
    GCF::ComponentGuiNode toggleSlotNode = node["toggleSlotNode"].value<GCF::ComponentGuiNode>();
    GCF::AbstractComponent* triggerSlotComp = triggerSlotNode.component();
    GCF::AbstractComponent* toggleSlotComp = toggleSlotNode.component();

    node["QAction"] = qVariantFromValue<QAction*>(0);
    node["GCF::SignalMultiplexer"] = qVariantFromValue<GCF::SignalMultiplexer*>(0);
    node["triggerRx"] = qVariantFromValue<QObject*>(0);
    node["toggleRx"] = qVariantFromValue<QObject*>(0);
    node["triggerSlotNode"] = "";
    node["toggleSlotNode"] = "";

    // Check for sanity
    if(!actionPtr || !actionMx)
        return;

    // First break connections with receivers.
    actionMx->breakConnection(triggerRx);
    if(triggerSlotComp)
    {
        removeDependency(triggerSlotComp, comp);
    }
    actionMx->breakConnection(toggleRx);
    if(toggleSlotComp)
    {
        removeDependency(toggleSlotComp, comp);
    }

    // Now delete/release the action pointer.
    if(actionPtr->objectName() == node.completeName())
    {
        // action was created for this node and was not merged
        // with anyone else.
        QObject::disconnect(actionPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
        if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
            return;
        delete actionPtr;
    }
    else
    {
        GCF::ComponentGuiNode actionNode = actionPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!actionNode.isValid())
            actionNode = node.gui().findNode(actionPtr->objectName(), true);
        if(!actionNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* actionNodeComp = actionNode.component();
        removeDependency(actionNodeComp, comp);
    }
}

void GCF::ActionNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* actionPtr = node["QAction"].value<QAction*>();
    GCF::SignalMultiplexer* actionMx = node["GCF::SignalMultiplexer"].value<GCF::SignalMultiplexer*>();
    QObject* triggerRx = node.hasKey("triggerRx") ? node["triggerRx"].value<QObject*>() : 0;
    QObject* toggleRx = node.hasKey("toggleRx") ? node["toggleRx"].value<QObject*>() : 0;

    if(!actionPtr || !actionMx)
        return;

    if(triggerRx)
        actionMx->setActiveReceiver(triggerRx, true);
    if(toggleRx)
        actionMx->setActiveReceiver(toggleRx, true);
}

void GCF::ActionNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    QAction* actionPtr = node["QAction"].value<QAction*>();
    GCF::SignalMultiplexer* actionMx = node["GCF::SignalMultiplexer"].value<GCF::SignalMultiplexer*>();
    QObject* triggerRx = node.hasKey("triggerRx") ? node["triggerRx"].value<QObject*>() : 0;
    QObject* toggleRx = node.hasKey("toggleRx") ? node["toggleRx"].value<QObject*>() : 0;

    if(!actionPtr || !actionMx)
        return;

    if(triggerRx)
        actionMx->setActiveReceiver(triggerRx, false);
    if(toggleRx)
        actionMx->setActiveReceiver(toggleRx, false);
}

/****************************************************************************
GCF::MenuNodeInitializer Implementation
****************************************************************************/

int GCF::MenuNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Menu;
}

void GCF::MenuNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString menuName = node.completeName();
    QString title = node["title"].toString();
    QString parentMenu = node["parentMenu"].toString();
    QString mergeWith = node["mergeWith"].toString();
    QString before = node["before"].toString();
    QMenu* menuPtr = 0;
    QMenu* parentMenuPtr = 0;
    QAction* beforeAction = 0;

    if(!mergeWith.isEmpty())
    {
        GCF::ComponentGuiNode mergeWithNode = node.gui().findNode(mergeWith, true);
        if(mergeWithNode.isValid())
            menuPtr = mergeWithNode["QMenu"].value<QMenu*>();
    }

    if(!before.isEmpty())
    {
        GCF::ComponentGuiNode beforeNode = node.gui().findNode(before, true);
        if(beforeNode.isValid())
        {
            beforeAction = beforeNode["QAction"].value<QAction*>();
            if(!beforeAction)
            {
                QMenu* beforeMenu = beforeNode["QMenu"].value<QMenu*>();
                if(beforeMenu)
                    beforeAction = beforeMenu->menuAction();
            }
        }
    }

    if(!menuPtr)
    {
        if(!parentMenu.isEmpty())
        {
            GCF::ComponentGuiNode parentMenuNode = node.gui().findNode(parentMenu, true);
            if(parentMenuNode.isValid())
                parentMenuPtr = parentMenuNode["QMenu"].value<QMenu*>();
        }

        menuPtr = fetchMenu(node);
        if(parentMenuPtr)
        {
            if(beforeAction)
                parentMenuPtr->insertMenu(beforeAction, menuPtr);
            else
                parentMenuPtr->addMenu(menuPtr);
        }
        else if(!GCF::AbstractComponent::isMenuStripEnabled())
        {
            if(beforeAction)
                GCF::AbstractComponent::componentHostWindow()->windowMenuBar()->insertMenu(beforeAction, menuPtr);
            else
                GCF::AbstractComponent::componentHostWindow()->windowMenuBar()->addMenu(menuPtr);
        }
        menuPtr->setObjectName(menuName);
        menuPtr->setTitle(title);
        if(node.hasKey("icon") && (parentMenuPtr || AbstractComponent::isMenuStripEnabled()))
        {
            menuPtr->setIcon(QIcon(node["icon"].toString()));
            menuPtr->menuAction()->setIconText(title);
        }
        if(node.hasKey("tearOff"))
            menuPtr->setTearOffEnabled(node["tearOff"].toBool());

        menuPtr->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    }
    else
    {
        GCF::ComponentGuiNode menuNode = menuPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!menuNode.isValid())
            menuNode = node.gui().findNode(menuPtr->objectName(), true);
        if(!menuNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* menuNodeComp = menuNode.component();
        addDependency(menuNodeComp, comp);
    }

    if(!menuPtr)
        return;

    // Store menu pointer in the node
    node["QMenu"] = qVariantFromValue<QMenu*>(menuPtr);
    node["QAction"] = qVariantFromValue<QAction*>(menuPtr->menuAction());
    QObject::connect(menuPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
}

void GCF::MenuNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QMenu* menuPtr = node["QMenu"].value<QMenu*>();
    if(!menuPtr)
        return;

    node["QMenu"] = qVariantFromValue<QMenu*>(0);;

    if(menuPtr && menuPtr->objectName() == node.completeName())
    {
        // menu was created for this node and was not merged with anyone else
        QObject::disconnect(menuPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
        if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
            return;
        delete menuPtr;
    }
    else
    {
        GCF::ComponentGuiNode menuNode = menuPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!menuNode.isValid())
            menuNode = node.gui().findNode(menuPtr->objectName(), true);
        if(!menuNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* menuNodeComp = menuNode.component();
        removeDependency(menuNodeComp, comp);
    }
}

void GCF::MenuNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QMenu* menuPtr = node["QMenu"].value<QMenu*>();
    if(!menuPtr || menuPtr->objectName() != node.completeName())
    {
        // We are using a merged menu. So we do not have the right to
        // show/hide the menu here.
        return;
    }

    QAction* menuActionPtr = menuPtr->menuAction();
    if(menuActionPtr)
        // menuActionPtr->setVisible(true);
        menuActionPtr->setEnabled(true);
}

void GCF::MenuNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    QMenu* menuPtr = node["QMenu"].value<QMenu*>();
    if(!menuPtr || menuPtr->objectName() != node.completeName())
    {
        // We are using a merged menu. So we do not have the right to
        // show/hide the menu here.
        return;
    }

    QAction* menuActionPtr = menuPtr->menuAction();
    if(menuActionPtr)
        // menuActionPtr->setVisible(false);
        menuActionPtr->setEnabled(false);
}

/****************************************************************************
GCF::MenuItemNodeInitializer Implementation
****************************************************************************/

int GCF::MenuItemNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::MenuItem;
}

void GCF::MenuItemNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString actionName = node.hasKey("actionName") ? node["actionName"].toString() : "";
    bool sep = node.hasKey("separator") ? node["separator"].toBool() : false;
    GCF::ComponentGuiNode parentNode = node.parentNode();
    QString before = node["before"].toString();
    QString menuName;
    QMenu* menuPtr = 0;
    QAction* actionPtr = 0;
    QAction* beforeAction = 0;

    if(!parentNode.isValid() || !parentNode.hasKey("QMenu"))
        return;

    menuPtr = parentNode["QMenu"].value<QMenu*>();
    if(!menuPtr)
        return;

    if(actionName.isEmpty() && !sep)
        actionName = node.hasKey("menuName") ? node["menuName"].toString() : "";

    if(!before.isEmpty())
    {
        GCF::ComponentGuiNode beforeNode = node.gui().findNode(before, true);
        if(beforeNode.isValid())
        {
            beforeAction = beforeNode["QAction"].value<QAction*>();
            if(!beforeAction)
            {
                QMenu* beforeMenu = beforeNode["QMenu"].value<QMenu*>();
                if(beforeMenu)
                    beforeAction = beforeMenu->menuAction();
            }
        }
    }

    if(sep || actionName.isEmpty())
    {
        if(beforeAction)
            actionPtr = menuPtr->insertSeparator(beforeAction);
        else
            actionPtr = menuPtr->addSeparator();
    }
    else
    {
        // The menu items must be from this component only, so the second
        // parameter to findNode is not true. This is intentional.
        GCF::ComponentGuiNode actionNode = node.gui().findNode(actionName);
        if(!actionNode.isValid() || !actionNode.hasKey("QAction"))
            return;

        actionPtr = actionNode["QAction"].value<QAction*>();
        if(!actionPtr)
            return;

        if(beforeAction)
            menuPtr->insertAction(beforeAction, actionPtr);
        else
            menuPtr->addAction(actionPtr);

        if(actionPtr->menu() && !GCF::AbstractComponent::isMenuStripEnabled())
        {
            QMenuBar* menuBar = GCF::AbstractComponent::componentHostWindow()->windowMenuBar();
            menuBar->removeAction(actionPtr); // just incase the action

            // Fix to ensure that icon is infact set for such menu items, if
            // specified in the guixml file.
            if(actionNode.type() == GCF::ComponentGuiNode::Menu)
            {
                QMenu* menuPtr = actionPtr->menu();
                menuPtr->setIcon(QIcon(actionNode["icon"].toString()));
                actionPtr->setIconText(actionNode["title"].toString());
            }
        }
    }

    node["actionPtr"] = qVariantFromValue<QAction*>(actionPtr);
}

void GCF::MenuItemNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString actionName = node.hasKey("actionName") ? node["actionName"].toString() : "";
    GCF::ComponentGuiNode parentNode = node.parentNode();
    QMenu* menuPtr = 0;
    QAction* actionPtr = 0;

    if(!parentNode.isValid() || !parentNode.hasKey("QMenu"))
        return;
    menuPtr = parentNode["QMenu"].value<QMenu*>();
    if(!menuPtr)
        return;

    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();

    if(!actionPtr || !menuPtr)
        return;

    menuPtr->removeAction(actionPtr);
    node["actionPtr"] = qVariantFromValue<QAction*>(0);
}

void GCF::MenuItemNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* actionPtr = 0;
    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();
    if(!actionPtr)
        return;

    // actionPtr->setVisible(true);
    actionPtr->setEnabled(true);
}

void GCF::MenuItemNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    QAction* actionPtr = 0;
    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();
    if(!actionPtr)
        return;

    // actionPtr->setVisible(false);
    actionPtr->setEnabled(false);
}

/****************************************************************************
GCF::ToolBarNodeInitializer Implementation
****************************************************************************/

int GCF::ToolBarNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Toolbar;
}

void GCF::ToolBarNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString toolBarName = node.completeName();
    QString title = node["title"].toString();
    QString mergeWith = node.hasKey("mergeWith") ? node["mergeWith"].toString() : "";
    QToolBar* toolBarPtr = 0;

    if(!mergeWith.isEmpty())
    {
        GCF::ComponentGuiNode mergeWithNode = node.gui().findNode(mergeWith, true);
        if(mergeWithNode.isValid())
            toolBarPtr = mergeWithNode["QToolBar"].value<QToolBar*>();
    }

    if(!toolBarPtr)
    {
        toolBarPtr = fetchToolBar(node);
        toolBarPtr->setObjectName(toolBarName);
        toolBarPtr->setWindowTitle(title);
        toolBarPtr->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));

        GCF::AbstractComponent::componentHostWindow()->addWindowToolBar(toolBarPtr);
    }
    else
    {
        GCF::ComponentGuiNode toolBarNode = toolBarPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!toolBarNode.isValid())
            toolBarNode = node.gui().findNode(toolBarPtr->objectName(), true);
        if(!toolBarNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* toolBarNodeComp = toolBarNode.component();
        addDependency(toolBarNodeComp, comp);
    }

    // Store toolbar pointer in the node
    node["QToolBar"] = qVariantFromValue<QToolBar*>(toolBarPtr);

    QObject::connect(toolBarPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
}

void GCF::ToolBarNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QToolBar* toolBarPtr = node["QToolBar"].value<QToolBar*>();
    if(!toolBarPtr)
        return;

    node["QToolBar"] = qVariantFromValue<QToolBar*>(0);;

    if(toolBarPtr && toolBarPtr->objectName() == node.completeName())
    {
        // toolBar was created for this node and was not merged with anyone else
        QObject::disconnect(toolBarPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
        if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
            return;
        delete toolBarPtr;
    }
    else
    {
        GCF::ComponentGuiNode toolBarNode = toolBarPtr->property("_node_").value<GCF::ComponentGuiNode>();
        if(!toolBarNode.isValid())
            toolBarNode = node.gui().findNode(toolBarPtr->objectName(), true);
        if(!toolBarNode.isValid())
        {
            qFatal("Error at %s:%d", __FILE__, __LINE__);
            return;
        }

        GCF::AbstractComponent* toolBarNodeComp = toolBarNode.component();
        removeDependency(toolBarNodeComp, comp);
    }
}

void GCF::ToolBarNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QToolBar* toolBarPtr = node["QToolBar"].value<QToolBar*>();
    if(!toolBarPtr || toolBarPtr->objectName() != node.completeName())
    {
        // We are using a merged toolBar. So we do not have the right to
        // show/hide the toolBar here.
        return;
    }

    if(toolBarPtr)
        toolBarPtr->setEnabled(true);
}

void GCF::ToolBarNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    QToolBar* toolBarPtr = node["QToolBar"].value<QToolBar*>();
    if(!toolBarPtr || toolBarPtr->objectName() != node.completeName())
    {
        // We are using a merged toolBar. So we do not have the right to
        // show/hide the toolBar here.
        return;
    }

    if(toolBarPtr)
        toolBarPtr->setEnabled(false);
}

/****************************************************************************
GCF::ToolBarItemNodeInitializer Implementation
****************************************************************************/

int GCF::ToolBarItemNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::ToolbarItem;
}

void GCF::ToolBarItemNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString actionName = node.hasKey("actionName") ? node["actionName"].toString() : "";
    bool sep = node.hasKey("separator") ? node["separator"].toBool() : false;
    GCF::ComponentGuiNode parentNode = node.parentNode();
    QToolBar* toolBarPtr = 0;
    QAction* actionPtr = 0;

    if(!parentNode.isValid() || !parentNode.hasKey("QToolBar"))
        return;

    toolBarPtr = parentNode["QToolBar"].value<QToolBar*>();
    if(!toolBarPtr)
        return;

    if(actionName.isEmpty() && !sep)
        actionName = node.hasKey("menuName") ? node["menuName"].toString() : "";

    if(sep || actionName.isEmpty())
    {
        actionPtr = toolBarPtr->addSeparator();
    }
    else
    {
        // The toolBar items must be from this component only, so the second
        // parameter to findNode is not true. This is intentional.
        GCF::ComponentGuiNode actionNode = node.gui().findNode(actionName);
        if(!actionNode.isValid() || !actionNode.hasKey("QAction"))
            return;

        actionPtr = actionNode["QAction"].value<QAction*>();
        if(!actionPtr)
            return;

        toolBarPtr->addAction(actionPtr);

        // Fix to ensure that icon is infact set for such menu items, if
        // specified in the guixml file.
        if(actionNode.type() == GCF::ComponentGuiNode::Menu)
        {
            QMenu* menuPtr = actionPtr->menu();
            menuPtr->setIcon(QIcon(actionNode["icon"].toString()));
            actionPtr->setIconText(actionNode["title"].toString());
        }
    }

    node["actionPtr"] = qVariantFromValue<QAction*>(actionPtr);
}

void GCF::ToolBarItemNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString actionName = node.hasKey("actionName") ? node["actionName"].toString() : "";
    GCF::ComponentGuiNode parentNode = node.parentNode();
    QToolBar* toolBarPtr = 0;
    QAction* actionPtr = 0;

    if(!parentNode.isValid() || !parentNode.hasKey("QToolBar"))
        return;
    toolBarPtr = parentNode["QToolBar"].value<QToolBar*>();
    if(!toolBarPtr)
        return;

    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();

    if(!actionPtr || !toolBarPtr)
        return;

    toolBarPtr->removeAction(actionPtr);
    node["actionPtr"] = qVariantFromValue<QAction*>(0);;
}

void GCF::ToolBarItemNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* actionPtr = 0;
    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();
    if(!actionPtr)
        return;

    // actionPtr->setVisible(true);
    actionPtr->setEnabled(true);
}

void GCF::ToolBarItemNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    QAction* actionPtr = 0;
    if(node.hasKey("actionPtr"))
        actionPtr = node["actionPtr"].value<QAction*>();
    if(!actionPtr)
        return;

    // actionPtr->setVisible(false);
    actionPtr->setEnabled(false);
}

/****************************************************************************
GCF::MenuStripGroupNodeInitializer Implementation
****************************************************************************/

int GCF::MenuStripGroupNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::GroupItem;
}

void GCF::MenuStripGroupNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    GCF::MenuStrip* menuStrip = qobject_cast<GCF::MenuStrip*>(GCF::AbstractComponent::componentHostWindow()->windowMenuWidget());
    if(!menuStrip)
    {
        qFatal("Menu strip not present, even though it is enabled");
        return;
    }

    QString name = node.name();
    QString title = node["title"].toString();
    QString mergeWith = node["mergeWith"].toString();
    QString beforeName = node["before"].toString();
    QString iconName = node["icon"].toString();
    GCF::MenuStripGroup* group = 0;
    GCF::MenuStripGroup* before = 0;
    QIcon icon = iconName.isEmpty() ? QIcon() : QIcon(iconName);

    if(!mergeWith.isEmpty())
    {
        GCF::ComponentGuiNode mergeWithNode = node.gui().findNode(mergeWith, true);
        if(mergeWithNode.isValid())
            group = mergeWithNode["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();
        else
            node.removeKey("mergeWith");
    }

    if(!beforeName.isEmpty())
    {
        GCF::ComponentGuiNode beforeNode = node.gui().findNode(beforeName, true);
        if(beforeNode.isValid())
            before = beforeNode["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();
    }

    if(!group)
    {
        if(before)
            group = menuStrip->insertGroup(title, before);
        else
            group = menuStrip->addGroup(title);
        group->setIcon(icon);
    }

    node["GCF::MenuStripGroup"] = qVariantFromValue<GCF::MenuStripGroup*>(group);
}

void GCF::MenuStripGroupNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("mergeWith"))
        return;

    GCF::MenuStripGroup* group = node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();
    if(group)
        delete group;
    node.removeKey("GCF::MenuStripGroup");
}

void GCF::MenuStripGroupNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("mergeWith"))
        return;

    GCF::MenuStripGroup* group = node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();
    if(group)
        group->show();
}

void GCF::MenuStripGroupNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("mergeWith"))
        return;

    GCF::MenuStripGroup* group = node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();
    if(group)
        group->hide();
}

/****************************************************************************
GCF::ConnectionNodeInitializer Implementation
****************************************************************************/

int GCF::MenuStripGroupActionNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::GroupActionItem;
}

void GCF::MenuStripGroupActionNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    GCF::ComponentGuiNode parentNode = node.parentNode();
    QString actionName = node["actionName"].toString();
    QString menuName = node["menuName"].toString();
    GCF::MenuStripGroup* parentGroup = 0;
    QAction* action = 0;
    bool hasRowSpan = node.hasKey("rowSpan");
    bool hasColSpan = node.hasKey("colSpan");
    int rowSpan = hasRowSpan ? node["rowSpan"].toInt() : 1;
    int colSpan = hasColSpan ? node["colSpan"].toInt() : 1;

    if(actionName.isEmpty() && menuName.isEmpty())
        return;

    if(parentNode.isValid() && parentNode.hasKey("GCF::MenuStripGroup"))
        parentGroup = parentNode["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();

    if(!parentGroup)
        return;

    if(!actionName.isEmpty())
    {
        GCF::ComponentGuiNode actionNode = node.gui().findNode(actionName);
        if(actionNode.isValid())
            action = actionNode.hasKey("QAction") ? actionNode["QAction"].value<QAction*>() : 0;
    }
    else
    {
        GCF::ComponentGuiNode menuNode = node.gui().findNode(menuName);
        if(menuNode.isValid())
        {
            QMenu* menu = menuNode.hasKey("QMenu") ? menuNode["QMenu"].value<QMenu*>() : 0;
            action = menu ? menu->menuAction() : 0;
        }
    }

    if(!action)
        return;

    if(action->dynamicPropertyNames().contains("rowSpan"))
        return; // This action has already been inserted somewhere.

    if(action->icon().isNull())
    {
        if(!hasRowSpan)
            rowSpan = 2;
        if(!hasColSpan)
            colSpan = 2;
    }
    action->setProperty("rowSpan", rowSpan);
    action->setProperty("colSpan", colSpan);
    parentGroup->addAction(action);

    node["QAction"] = qVariantFromValue<QAction*>(action);
    node["GCF::MenuStripGroup"] = qVariantFromValue<GCF::MenuStripGroup*>(parentGroup);
}

void GCF::MenuStripGroupActionNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::MenuStripGroup* group = node.hasKey("GCF::MenuStripGroup") ? node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>() : 0;
    if(!action || !group)
        return;

    group->removeAction(action);
    node.removeKey("QAction");
    node.removeKey("GCF::MenuStripGroup");
}

void GCF::MenuStripGroupActionNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node["hideWhenInactive"].toBool() == false)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::MenuStripGroup* group = node.hasKey("GCF::MenuStripGroup") ? node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>() : 0;
    if(!action || !group)
        return;

    // action->setVisible(true);
    action->setEnabled(true);
}

void GCF::MenuStripGroupActionNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node["hideWhenInactive"].toBool() == false)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::MenuStripGroup* group = node.hasKey("GCF::MenuStripGroup") ? node["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>() : 0;
    if(!action || !group)
        return;

    // action->setVisible(false);
    action->setEnabled(false);
}

/****************************************************************************
GCF::ConnectionNodeInitializer Implementation
****************************************************************************/

int GCF::ConnectionNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Connection;
}

void GCF::ConnectionNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString sender = node["sender"].toString();
    QString receiver = node["receiver"].toString();
    QString signal, slot;
    QObject* senderPtr = 0;
    QObject* receiverPtr = 0;
    GCF::ComponentGuiNode senderNode = resolveCompleteSlotName(node, sender, &senderPtr, signal);
    GCF::ComponentGuiNode receiverNode = resolveCompleteSlotName(node, receiver, &receiverPtr, slot);

    if(senderNode.component() != comp && receiverNode.component() != comp)
    {
        qWarning("Either the sender or the receiver in the connection has to belong to this component");
        return;
    }

    if(!senderPtr || !receiverPtr)
        return;

    // Currently signal to signal connections are not supported.
    signal = QString("2%1").arg(signal);
    slot   = QString("1%1").arg(slot);
    bool success = QObject::connect(senderPtr, signal.toAscii().data(), receiverPtr, slot.toAscii().data());
    if(!success)
    {
        qWarning("Could not connect %s::%s to %s::%s",
                 senderPtr->metaObject()->className(),
                 signal.toAscii().data()+1,
                 receiverPtr->metaObject()->className(),
                 slot.toAscii().data()+1);
        return;
    }

    node["senderPtr"] = qVariantFromValue<QObject*>(senderPtr);
    node["signal"] = signal;
    node["receiverPtr"] = qVariantFromValue<QObject*>(receiverPtr);
    node["slot"] = slot;
    addDependency(senderNode.component(), receiverNode.component());
}

void GCF::ConnectionNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString sender = node["sender"].toString();
    QString receiver = node["receiver"].toString();
    QString signal, slot;
    QObject* senderPtr = 0;
    QObject* receiverPtr = 0;
    GCF::ComponentGuiNode senderNode = resolveCompleteSlotName(node, sender, &senderPtr, signal);
    GCF::ComponentGuiNode receiverNode = resolveCompleteSlotName(node, receiver, &receiverPtr, slot);

    if(senderNode.component() != comp && receiverNode.component() != comp)
    {
        qWarning("Either the sender or the receiver in the connection has to belong to this component");
        return;
    }

    if(!senderPtr || !receiverPtr)
        return;

    // Currently signal to signal connections are not supported.
    signal = QString("2%1").arg(signal);
    slot   = QString("1%1").arg(slot);
    QObject::disconnect(senderPtr, signal.toAscii().data(), receiverPtr, slot.toAscii().data());

    node["senderPtr"] = qVariantFromValue<QAction*>(0);;
    node["signal"] = "";
    node["receiverPtr"] = qVariantFromValue<QAction*>(0);;
    node["slot"] = "";
    removeDependency(senderNode.component(), receiverNode.component());
}

void GCF::ConnectionNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(!node.hasKey("senderPtr"))
        return;

    QObject* senderPtr = node["senderPtr"].value<QObject*>();
    QString signal = node["signal"].toString();
    QObject* receiverPtr = node["receiverPtr"].value<QObject*>();
    QString slot = node["slot"].toString();

    if(!senderPtr || !receiverPtr || signal.isEmpty() || slot.isEmpty())
        return;
    QObject::connect(senderPtr, signal.toAscii().data(), receiverPtr, slot.toAscii().data());
}

void GCF::ConnectionNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("disconnectWhenInactive") && !node["disconnectWhenInactive"].toBool())
        return;

    if(!node.hasKey("senderPtr"))
        return;

    QObject* senderPtr = node["senderPtr"].value<QObject*>();
    QString signal = node["signal"].toString();
    QObject* receiverPtr = node["receiverPtr"].value<QObject*>();
    QString slot = node["slot"].toString();

    if(!senderPtr || !receiverPtr || signal.isEmpty() || slot.isEmpty())
        return;
    QObject::disconnect(senderPtr, signal.toAscii().data(), receiverPtr, slot.toAscii().data());
}

/****************************************************************************
GCF::ComponentGuiXmlParser Implementation
****************************************************************************/

int GCF::PropertyNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Property;
}

void GCF::PropertyNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString completePropName = node["key"].toString();
    QObject* propObject = 0;
    QString propName;

    GCF::ComponentGuiNode propNode = resolveCompletePropertyName(node, completePropName, &propObject, propName);
    if(propNode.component() != comp || !propObject)
        return;

    propObject->setProperty(propName.toAscii().data(), node["value"]);
}

void GCF::PropertyNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}

void GCF::PropertyNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}

void GCF::PropertyNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}


/****************************************************************************
GCF::MethodNodeInitializer Implementation
****************************************************************************/

int GCF::MethodNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::Method;
}

void GCF::MethodNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QString completeMethodName = node["methodName"].toString();
    QObject* methodOwnerObject = 0;
    QString methodName;

    GCF::ComponentGuiNode methodOwnerNode
        = resolveCompleteMethodName(node, completeMethodName, &methodOwnerObject, methodName);
    if(methodOwnerNode.component() != comp || !methodOwnerObject)
        return;

    const QMetaObject* metaObject = methodOwnerObject->metaObject();
    int methodIndex = -1;
    QMetaMethod method;

    for(int i=0; i<metaObject->methodCount(); i++)
    {
        method = metaObject->method(i);
        QString methodSig = method.signature();
        if(methodSig.section('(', 0, 0) == methodName)
        {
            methodIndex = i;
            break;
        }
    }

    if(methodIndex < 0)
    {
        qWarning("Cannot find method %s", completeMethodName.toAscii().data());
        return;
    }

    QVector<QGenericArgument> args(10);
    QList<QByteArray> paramTypes = method.parameterTypes();
    int argIndex=0;

    QList<QString*> strings;
    QList<int*> ints;
    QList<double*> doubles;
    QList<bool*> bools;

    while(1)
    {
        if(argIndex >= paramTypes.count())
            break;

        QString argName = QString("param%1").arg(argIndex+1);
        if(!node.hasKey(argName))
            break;

        QVariant argValue = node[argName];
        QString paramType = paramTypes[argIndex];
        /*
        paramType = paramType.remove("const");
        paramType = paramType.remove("&");
        paramType = paramType.remove("*");
        paramType = paramType.simplified();
        */

        if(paramType == "QString")
        {
            QString* str = new QString(argValue.toString());
            strings.push_back(str);
            args[argIndex] = Q_ARG(QString, *str);
        }
        else if(paramType == "int")
        {
            int* intVal = new int;
            *intVal = argValue.toInt();
            ints.push_back(intVal);
            args[argIndex] = Q_ARG(int, *intVal);
        }
        else if(paramType == "double")
        {
            double* doubleVal = new double;
            *doubleVal = argValue.toDouble();
            doubles.push_back(doubleVal);
            args[argIndex] = Q_ARG(double, *doubleVal);
        }
        else if(paramType == "bool")
        {
            bool* boolVal = new bool;
            *boolVal = argValue.toBool();
            bools.push_back(boolVal);
            args[argIndex] = Q_ARG(bool, *boolVal);
        }
        else
        {
            qWarning("Cannot work with parameter datatype %s", paramType.toAscii().data());
            return;
        }

        ++argIndex;
    }

    QMetaObject::invokeMethod(methodOwnerObject, methodName.toAscii().data(), Qt::DirectConnection,
        args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);

    qDeleteAll(strings);
    qDeleteAll(ints);
    qDeleteAll(doubles);
    qDeleteAll(bools);
}

void GCF::MethodNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}

void GCF::MethodNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}

void GCF::MethodNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
    Q_UNUSED(node);
}

/****************************************************************************
GCF::ActiveXNodeInitializer Implementation
****************************************************************************/

int GCF::ActiveXNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::ActiveX;
}

void GCF::ActiveXNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_ACTIVEQT_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Create the QWidget
    QString widgetName = node.completeName();
    QAxWidget* axWidget = fetchActiveXWidget(node);
    if(!axWidget)
    {
        qWarning("Node %s cannot be initialized", qPrintable(widgetName));
        return;
    }
    node["QWidget"] = qVariantFromValue<QWidget*>(axWidget);
    node["QAxWidget"] = qVariantFromValue<QAxWidget*>(axWidget);

    // Set the widget name
    axWidget->setObjectName(widgetName);

    // Store the title in the widget.
    QString widgetTitle = node["title"].toString();
    axWidget->setProperty("_title_", widgetTitle);
    axWidget->setWindowTitle(widgetTitle);

    // Now find out the parent widget.
    GCF::ComponentGuiNode parentWidgetNode;
    QString parentWidgetName = node["parentWidget"].toString();
    QString hint = node["parentWidgetHint"].toString();
    if(!parentWidgetName.isEmpty())
    {
        parentWidgetNode = node.gui().findNode(parentWidgetName, true);
        if(parentWidgetNode.isValid())
        {
            QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
            GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
            if(parentWidget && parentWidgetComp)
            {
                addChildWidget(parentWidgetNode, parentWidget, axWidget, hint);
                addDependency(parentWidgetComp, comp);
                node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(parentWidgetNode);
            }
            else
                qWarning("Parent widget or its component was not found for %s", widgetName.toAscii().data());
        }
        else
            qWarning("Invalid parent widget specified for %s", widgetName.toAscii().data());
    }

    // Store the node as a property value in QObject
    axWidget->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    QObject::connect(axWidget, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));

    // Show the widget
    axWidget->show();
#else
    qDebug("ActiveQt Module not available");
#endif
}

void GCF::ActiveXNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_ACTIVEQT_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch the activex factory
    IActiveXFactory* axFactory = qobject_cast<IActiveXFactory*>(comp);

    // Fetch the pointer to the QAxWidget in the node
    if(!node.hasKey("QAxWidget"))
        return;
    QAxWidget* widgetPtr = node["QAxWidget"].value<QAxWidget*>();
    if(!widgetPtr)
        return;

    QString compName = node.completeName();
    QString control = node["control"].toString();

    if(axFactory)
        axFactory->finalizeActiveXWidget(widgetPtr, compName, control);

    // Find out the parent widget node and drop dependencies.
    QString hint = node["parentWidgetHint"].toString();
    if(node.hasKey("parentWidgetNode"))
    {
        GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
        QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
        GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
        if(parentWidgetComp)
        {
            removeChildWidget(parentWidgetNode, parentWidget, widgetPtr, hint);
            removeDependency(parentWidgetComp, comp);
        }
        node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(GCF::ComponentGuiNode());
    }

    // Delete the widget
    node["QAxWidget"] = qVariantFromValue<QAxWidget*>(0);
    node["QWidget"] = qVariantFromValue<QWidget*>(0);
    QObject::disconnect(widgetPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
    if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
        return;

    // Ask the IActiveXFactory to destroy the node if needed
    if(axFactory)
        axFactory->destroyActiveXWidget(widgetPtr, compName, control);
    else
        delete widgetPtr;
#else
    qDebug("ActiveQt Module not available");
#endif
}

void GCF::ActiveXNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_ACTIVEQT_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    QString hint = node["parentWidgetHint"].toString();
    GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
    QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
    if(parentWidgetNode.isValid() && parentWidgetPtr)
        showChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
    else
        widgetPtr->show();
#else
    qDebug("ActiveQt Module not available");
#endif
}

void GCF::ActiveXNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_ACTIVEQT_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    QString hint = node["parentWidgetHint"].toString();
    GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
    QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
    if(parentWidgetNode.isValid() && parentWidgetPtr)
        hideChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
    else
        widgetPtr->hide();
#else
    qDebug("ActiveQt Module not available");
#endif
}

/****************************************************************************
GCF::KPartNodeInitializer Implementation
****************************************************************************/

int GCF::KPartNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::ActiveX;
}

void GCF::KPartNodeInitializer::initializeNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_KPARTS_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Basic stuff
    IKPartsFactory::PartCreationType type = IKPartsFactory::UnknownType;
    QString typeStr;
    if(node.hasKey("mimeType"))
    {
        type = IKPartsFactory::MimeType;
        typeStr = node["mimeType"].toString();;
    }
    else if(node.hasKey("serviceType"))
    {
        type = IKPartsFactory::ServiceType;
        typeStr = node["serviceType"].toString();
    }
    else if(node.hasKey("libraryName"))
    {
        type = IKPartsFactory::LibraryName;
        typeStr = node["libraryName"].toString();
    }
    node["PartCreationType"] = int(type);
    node["typeStr"] = typeStr;

    // Create the KPart
    QString kpartName = node.completeName();
    KParts::Part* part = fetchPart(node);
    QWidget* partWidget = part ? part->widget() : 0;
    if(!part || !partWidget)
    {
        qWarning("Node %s cannot be initialized", qPrintable(kpartName));
        return;
    }
    qDebug("Loaded %s for %s", part->metaObject()->className(), qPrintable(kpartName));
    node["QWidget"] = qVariantFromValue<QWidget*>(partWidget);
    node["KParts::Part"] = qVariantFromValue<KParts::Part*>(part);

    // Set the widget name
    part->setObjectName(kpartName); // FIXME: Does a KPart use its objectName() for something?
    partWidget->setObjectName(kpartName);

    // Store the title in the widget.
    QString widgetTitle = node["title"].toString();
    partWidget->setProperty("_title_", widgetTitle);
    partWidget->setWindowTitle(widgetTitle);

    // Now find out the parent widget.
    GCF::ComponentGuiNode parentWidgetNode;
    QString parentWidgetName = node["parentWidget"].toString();
    QString hint = node["parentWidgetHint"].toString();
    if(!parentWidgetName.isEmpty())
    {
        parentWidgetNode = node.gui().findNode(parentWidgetName, true);
        if(parentWidgetNode.isValid())
        {
            QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
            GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
            if(parentWidget && parentWidgetComp)
            {
                addChildWidget(parentWidgetNode, parentWidget, partWidget, hint);
                addDependency(parentWidgetComp, comp);
                node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(parentWidgetNode);
            }
            else
                qWarning("Parent widget or its component was not found for %s", qPrintable(kpartName));
        }
        else
            qWarning("Invalid parent widget specified for %s", qPrintable(kpartName));
    }

    // Store the node as a property value in QObject
    partWidget->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    part->setProperty("_node_", qVariantFromValue<GCF::ComponentGuiNode>(node));
    QObject::connect(part, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));

    // Create a structure into which we are going to store the part data.
    GCF::KPartsData partData;
    partData.part = part;
    partData.xmlGuiClient = (KXMLGUIClient*)(part);

    // Merge GUI of the top-level part with GCF
    mergePartGui(node, part, part, partData);

    // Now merge the GUI of the child XMLGUIClients.
    QList<KXMLGUIClient*> childClients = part->childClients();
    for(int i=0; i<childClients.count(); i++)
    {
        KXMLGUIClient* childClient = childClients[i];
        mergePartGui(node, part, childClient, partData);
    }

    // Now lets store the part data for future use
    node["GCF::KPartsData"] = qVariantFromValue<GCF::KPartsData>(partData);

    // Show the widget
    partWidget->show();

    // Submit the URL if the GUI-XML provided any
    QString url = node.hasKey("url") ? node["url"].toString() : QString();
    if(!url.isEmpty())
    {
        QUrl qurl(url);
        KUrl kurl(qurl);
        KParts::ReadOnlyPart* roPart = qobject_cast<KParts::ReadOnlyPart*>(part);
        roPart->openUrl(kurl);
    }

#else
    qDebug("KParts integration not available");
#endif
}

void GCF::KPartNodeInitializer::cleanupNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_KPARTS_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Catch hold if the part factory interface.
    IKPartsFactory* kpFactory = qobject_cast<IKPartsFactory*>(comp);

    // Fetch the pointer to the KParts::Part in the node
    if(!node.hasKey("KParts::Part"))
        return;
    KParts::Part* part = node["KParts::Part"].value<KParts::Part*>();
    if(!part)
        return;
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();

    QString compName = node.completeName();
    IKPartsFactory::PartCreationType type = IKPartsFactory::PartCreationType(node["PartCreationType"].toInt());
    QString typeStr = node["typeStr"].toString();
    bool readOnly = node["readOnly"].toBool();

    // We are about to remove the part, lets give the part factory a chance to
    // do some finalization if needed.
    if(kpFactory)
        kpFactory->finalizePart(part, compName, type, typeStr, readOnly);

    // Find out the parent widget node and drop dependencies.
    QString hint = node["parentWidgetHint"].toString();
    if(node.hasKey("parentWidgetNode"))
    {
        GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
        QWidget* parentWidget = parentWidgetNode["QWidget"].value<QWidget*>();
        GCF::AbstractComponent* parentWidgetComp = parentWidgetNode.component();
        if(parentWidgetComp)
        {
            removeChildWidget(parentWidgetNode, parentWidget, widgetPtr, hint);
            removeDependency(parentWidgetComp, comp);
        }
        node["parentWidgetNode"] = qVariantFromValue<GCF::ComponentGuiNode>(GCF::ComponentGuiNode());
    }

    // Disconnect from the widget
    node["KParts::Part"] = qVariantFromValue<KParts::Part*>(0);
    node["QWidget"] = qVariantFromValue<QWidget*>(0);
    QObject::disconnect(widgetPtr, SIGNAL(destroyed(QObject*)), comp, SLOT(objectDestroyed(QObject*)));
    if(node.hasKey("autoDelete") && !node["autoDelete"].toBool())
        return;

    // Remove the merged GUI for this part.
    GCF::KPartsData partData = node["GCF::KPartsData"].value<GCF::KPartsData>();
    GCF::ComponentGui gui = node.gui();
    for(int i=0; i<partData.partNodes.count(); i++)
    {
        GCF::ComponentGuiNode partNode = partData.partNodes[i];
        qDebug("Removing node of type %d", partNode.type());
        gui.removeNode(partNode);
    }

    // Ask the node to close what-ever URL it is viewing
    KParts::ReadOnlyPart* roPart = qobject_cast<KParts::ReadOnlyPart*>(part);
    roPart->closeUrl();

    // Ask the IKPartsFactory to destroy the node if needed
    if(kpFactory)
        kpFactory->destroyPart(part, compName, type, typeStr, readOnly);
    else
        delete part;

    node.removeKey("GCF::KPartsData");
    node.removeKey("KParts::Part");

#else
    qDebug("KParts integration not available");
#endif
}

void GCF::KPartNodeInitializer::showNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_KPARTS_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    QString hint = node["parentWidgetHint"].toString();
    GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
    QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
    if(parentWidgetNode.isValid() && parentWidgetPtr)
        showChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
    else
        widgetPtr->show();

#else
    qDebug("KParts integration not available");
#endif
}

void GCF::KPartNodeInitializer::hideNode(GCF::ComponentGuiNode node)
{
#ifdef GCF_KPARTS_AVAILABLE
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node.hasKey("hideWhenInactive") && !node["hideWhenInactive"].toBool())
        return;

    // Fetch pointer to the QWidget in the node
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    QString hint = node["parentWidgetHint"].toString();
    GCF::ComponentGuiNode parentWidgetNode = node["parentWidgetNode"].value<GCF::ComponentGuiNode>();
    QWidget* parentWidgetPtr = parentWidgetNode.hasKey("QWidget") ? parentWidgetNode["QWidget"].value<QWidget*>() : 0;
    if(parentWidgetNode.isValid() && parentWidgetPtr)
        hideChildWidget(parentWidgetNode, parentWidgetPtr, widgetPtr, hint);
    else
        widgetPtr->hide();

#else
    qDebug("KParts integration not available");
#endif
}

#ifdef GCF_KPARTS_AVAILABLE
void GCF::KPartNodeInitializer::mergePartGui(GCF::ComponentGuiNode partGuiNode, KParts::Part* part, KXMLGUIClient* xmlGuiClient, GCF::KPartsData& partData)
{
    if(!part || !xmlGuiClient)
        return;

    // Catch hold of the component gui into which we are going to
    // merge the part's GUI
    GCF::ComponentGui gui = partGuiNode.gui();

    // Get hold of the Part's XMLGUI
    QDomDocument domDoc = xmlGuiClient->domDocument();
    QDomElement rootE = domDoc.documentElement();
    if(rootE.tagName() != "kpartgui")
        return;

    QVariant xgcv = qVariantFromValue<KXMLGUIClient*>(xmlGuiClient);

    /*
    The way we merge the part GUI is like this

    If we are in MenuStrip mode then
        - We create a new menu strip group for the part
        - For each menu/toolbar in the part's GUI, we create a menu button
          and add it within the menu strip group.
        - The menu itself is populated with the part's actions as specified
          within the part's XMLGUI document.

    If we are in normal menubar/toolbar mode
        - We create a new menu-bar menu for the part
        - All menus are added within this top level menu
        - Toolbars are created independently.

    Some more notes:
        - All actions created by GCF for KParts are marked with autoDelete=false.
          This is because the part has to take care of deleting the actions
        - All the top level nodes created are stored in partData (instance of GCF::KPartsData)
    */

    // Note that the order in which the nodes are created is very important.
    // Lets create the top-level menu-strip group or menu-bar menu into which
    // all of the part's menus and actions will be placed
    GCF::ComponentGuiNode partNode;
    if(!GCF::AbstractComponent::isMenuStripEnabled())
    {
        if(!partData.partRootNodeCreated)
            initRootNode(partGuiNode, part, xmlGuiClient, partData);
        partNode = partData.partRootNode;
    }

    // Look for the menu bars and create menu nodes first.
    QDomElement menuBarE = rootE.firstChildElement("MenuBar");
    QDomElement menuE = menuBarE.firstChildElement("Menu");
    while(1)
    {
        if(menuE.isNull())
            break;

        /*
        The rule in GCF is that menus and actions __have__ to be created
        as top level nodes. They are then inserted as items within their
        parent nodes.
        */
        QList<GCF::ComponentGuiNode> miNodes;

        QDomElement actionE = menuE.firstChildElement();
        while(1)
        {
            if(actionE.isNull())
                break;

            if(actionE.tagName() == "Action")
            {
                QString actualName = actionE.attribute("name");
                QString mangledName = QString("kpart_action_%1").arg(actualName);

                // Create an action node
                GCF::ComponentGuiNode actionNode;
                actionNode.setType(GCF::ComponentGuiNode::Action);
                actionNode.setName(mangledName);
                actionNode["actualName"] = actualName;
                actionNode["KXMLGUIClient"] = xgcv;
                actionNode["autoDelete"] = false;
                gui.addNode(actionNode);
                partData.partNodes.append(actionNode);

                QAction* action = actionNode["QAction"].value<QAction*>();
                if(action)
                    action->setEnabled( actionNode["FactoryEnable"].toBool() );

                // Create a menuitem node
                GCF::ComponentGuiNode miNode;
                miNode.setType(GCF::ComponentGuiNode::MenuItem);
                miNode["actionName"] = actionNode.completeName();
                miNodes.append(miNode);

                // qDebug("   Created Node for %s", qPrintable(actualName));
            }
            else if(actionE.tagName() == "Separator")
            {
                // Create a menuitem node
                GCF::ComponentGuiNode miNode;
                miNode.setType(GCF::ComponentGuiNode::MenuItem);
                miNode["separator"] = "true";
                miNodes.append(miNode);
            }

            actionE = actionE.nextSiblingElement();
        }

        // Now create the menu node
        QString menuName = QString("kparts_menu_%1").arg(menuE.attribute("name"));
        GCF::ComponentGuiNode menuNode;
        menuNode.setType(GCF::ComponentGuiNode::Menu);
        menuNode.setName(menuName);
        menuNode["title"] = menuE.text();
        menuNode["KXMLGUIClient"] = xgcv;
        if(!GCF::AbstractComponent::isMenuStripEnabled())
            menuNode["parentMenu"] = partNode.completeName();
        gui.addNode(menuNode);
        partData.partNodes.append(menuNode);

        // Insert the menu items into the menu node
        for(int i=0; i<miNodes.count(); i++)
        {
            GCF::ComponentGuiNode miNode = miNodes[i];
            menuNode.addChild(miNode);
        }

        // be done with the menu nodes
        miNodes.clear();

        menuE = menuE.nextSiblingElement();
    }

    if(GCF::AbstractComponent::isMenuStripEnabled())
    {
        if(!partData.partRootNodeCreated)
            initRootNode(partGuiNode, part, xmlGuiClient, partData);
        partNode = partData.partRootNode;

        for(int i=0; i<partData.partNodes.count(); i++)
        {
            GCF::ComponentGuiNode menuNode = partData.partNodes[i];
            if(menuNode.type() != GCF::ComponentGuiNode::Menu)
                continue;

            GCF::ComponentGuiNode partItemNode;
            partItemNode.setType(GCF::ComponentGuiNode::GroupActionItem);
            partItemNode["menuName"] = menuNode.completeName();
            partItemNode["rowSpan"] = 1;
            partItemNode["colSpan"] = 2;
            partNode.addChild(partItemNode);
        }
    }
    // Look for toolbars and create toolbar nodes if menu strip is disabled,
    // create toolbars if menu strip is disabled.
    // QDomElement menuBarE = rootE.firstChildElement("MenuBar");
    // Do this later
}

void GCF::KPartNodeInitializer::initRootNode(ComponentGuiNode partGuiNode, KParts::Part* part, KXMLGUIClient* xmlGuiClient, GCF::KPartsData& partData)
{
    if(!part || !xmlGuiClient)
        return;

    GCF::ComponentGui gui = partGuiNode.gui();
    QVariant xgcv = qVariantFromValue<KXMLGUIClient*>(xmlGuiClient);

    GCF::ComponentGuiNode partNode;
    if(!partData.partRootNodeCreated)
    {
        partNode.setName( QString("%1_kpartgui").arg(partGuiNode.name()) );
        if(GCF::AbstractComponent::isMenuStripEnabled())
            partNode.setType(GCF::ComponentGuiNode::GroupItem);
        else
            partNode.setType(GCF::ComponentGuiNode::Menu);
        if(partGuiNode.hasKey("mergeWith"))
            partNode["mergeWith"] = partGuiNode["mergeWith"];
        if(partGuiNode.hasKey("before"))
            partNode["before"] = partGuiNode["before"];
        partNode["title"] = partGuiNode["title"];
        partNode["KXMLGUIClient"] = xgcv;
        partNode["RootPartNode"] = true;
        gui.addNode(partNode);
        partData.partNodes.append(partNode);
        partData.partRootNode = partNode;
        partData.partRootNodeCreated = true;
    }
}

#endif

/****************************************************************************
GCF::GroupDrawerWidgetNodeInitializer Implementation
****************************************************************************/

int GCF::GroupDrawerWidgetNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::GroupDrawerWidgetItem;
}

void GCF::GroupDrawerWidgetNodeInitializer::initializeNode(ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Catch hold of the widget to show in the drawer
    QString widgetName = node["widgetName"].toString();
    GCF::ComponentGuiNode widgetNode = node.gui().findNode(widgetName);
    if(!widgetNode.isValid())
    {
        qWarning("Drawer Widget Name %s not valid", qPrintable(widgetName));
        return;
    }
    QWidget* widgetPtr = widgetNode["QWidget"].value<QWidget*>();
    if(!widgetPtr)
    {
        qWarning("Component %s did not provide drawer widget for %s",
            qPrintable(comp->componentName()), qPrintable(widgetName));
        return;
    }

    // Mark that this widget is being used within the menu strip.
    // So that the hideNode() and showNode() methods for widget node initializer
    // ensure that menu strip widgets are enabled/disabled and not shown/hidden.
    widgetNode["UsedInMenuStrip"] = true;

    // Configure the widget
    if(node.hasKey("height"))
    {
        int height = node["height"].toInt();
        widgetPtr->setMinimumHeight(height);
    }

    if(node.hasKey("width"))
    {
        int width = node["width"].toInt();
        widgetPtr->setMinimumWidth(width);
    }

    // Store the widget into the node for future reference
    node["QWidget"] = qVariantFromValue<QWidget*>(widgetPtr);

    // Get the MenuStripGroup into which this drawer widget should be inserted.
    GCF::ComponentGuiNode parentNode = node.parentNode();
    GCF::MenuStripGroup* group = parentNode["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();

    // Insert the drawer widget into that group now.
    group->addDrawerWidget(widgetPtr);

    // All done.
}

void GCF::GroupDrawerWidgetNodeInitializer::cleanupNode(ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Get pointer to the widget that needs to be removed
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(!widgetPtr)
        return;

    // Get the MenuStripGroup from which this widget should be removed.
    GCF::ComponentGuiNode parentNode = node.parentNode();
    GCF::MenuStripGroup* group = parentNode["GCF::MenuStripGroup"].value<GCF::MenuStripGroup*>();

    // Remove the widget from the group
    group->removeDrawerWidget(widgetPtr);

    // Update the node data
    node["QWidget"] = qVariantFromValue<QWidget*>(0);

    // All done.
}

void GCF::GroupDrawerWidgetNodeInitializer::showNode(ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Get pointer to the widget that needs to be enabled
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(widgetPtr)
        widgetPtr->setEnabled(true);
}

void GCF::GroupDrawerWidgetNodeInitializer::hideNode(ComponentGuiNode node)
{
    // Check for validity
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    // Get pointer to the widget that needs to be enabled
    QWidget* widgetPtr = node["QWidget"].value<QWidget*>();
    if(widgetPtr)
        widgetPtr->setDisabled(true);
}

/****************************************************************************
GCF::ActionContainerNodeInitializer Implementation
****************************************************************************/

int GCF::ActionContainerNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::ActionContainer;
}

void GCF::ActionContainerNodeInitializer::initializeNode(ComponentGuiNode node)
{
    GCF::WidgetNodeInitializer::initializeNode(node);

    // Ensure that the widget pointer is also available under GCF::ActionContainerWidget key
    QWidget* w = node["QWidget"].value<QWidget*>();
    GCF::ActionContainerWidget* acw = qobject_cast<GCF::ActionContainerWidget*>(w);
    node["GCF::ActionContainerWidget"] = qVariantFromValue<GCF::ActionContainerWidget*>(acw);
}

void GCF::ActionContainerNodeInitializer::cleanupNode(ComponentGuiNode node)
{
    GCF::WidgetNodeInitializer::cleanupNode(node);
}

void GCF::ActionContainerNodeInitializer::showNode(ComponentGuiNode node)
{
    GCF::WidgetNodeInitializer::showNode(node);
}

void GCF::ActionContainerNodeInitializer::hideNode(ComponentGuiNode node)
{
    GCF::WidgetNodeInitializer::hideNode(node);
}

QWidget* GCF::ActionContainerNodeInitializer::fetchWidget(ComponentGuiNode node) const
{
    if(node.type() == GCF::ComponentGuiNode::ActionContainer)
    {
        int maxRows = 2;
        if( node.hasKey("maxRows") )
            maxRows = node["maxRows"].toInt();

        return new GCF::ActionContainerWidget(maxRows);
    }

    return GCF::WidgetNodeInitializer::fetchWidget(node);
}

/****************************************************************************
GCF::ActionContainerItemNodeInitializer Implementation
****************************************************************************/

int GCF::ActionContainerItemNodeInitializer::handledType() const
{
    return GCF::ComponentGuiNode::ActionContainerItem;
}

void GCF::ActionContainerItemNodeInitializer::initializeNode(ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    GCF::ComponentGuiNode parentNode = node.parentNode();
    QString actionName = node["actionName"].toString();
    QString menuName = node["menuName"].toString();
    GCF::ActionContainerWidget* parentActionContainer = 0;
    QAction* action = 0;
    bool hasRowSpan = node.hasKey("rowSpan");
    bool hasColSpan = node.hasKey("colSpan");

    int rowSpan = hasRowSpan ? node["rowSpan"].toInt() : 1;
    int colSpan = hasColSpan ? node["colSpan"].toInt() : 1;

    if(actionName.isEmpty() && menuName.isEmpty())
        return;

    if(parentNode.isValid() && parentNode.hasKey("GCF::ActionContainerWidget"))
        parentActionContainer = parentNode["GCF::ActionContainerWidget"].value<GCF::ActionContainerWidget*>();

    if(!parentActionContainer)
        return;

    if(!actionName.isEmpty())
    {
        GCF::ComponentGuiNode actionNode = node.gui().findNode(actionName);
        if(actionNode.isValid())
            action = actionNode.hasKey("QAction") ? actionNode["QAction"].value<QAction*>() : 0;
    }
    else
    {
        GCF::ComponentGuiNode menuNode = node.gui().findNode(menuName);
        if(menuNode.isValid())
        {
            QMenu* menu = menuNode.hasKey("QMenu") ? menuNode["QMenu"].value<QMenu*>() : 0;
            action = menu ? menu->menuAction() : 0;
        }
    }

    if(!action)
        return;

    if(action->icon().isNull())
    {
        if(!hasRowSpan)
            rowSpan = 2;
        if(!hasColSpan)
            colSpan = 2;
    }

    parentActionContainer->addAction(action, rowSpan, colSpan);

    node["QAction"] = qVariantFromValue<QAction*>(action);
    node["GCF::ActionContainerWidget"] = qVariantFromValue<GCF::ActionContainerWidget*>(parentActionContainer);
}

void GCF::ActionContainerItemNodeInitializer::cleanupNode(ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::ActionContainerWidget* actionContainer = node.hasKey("GCF::ActionContainerWidget") ? node["GCF::ActionContainerWidget"].value<GCF::ActionContainerWidget*>() : 0;
    if(!action || !actionContainer)
        return;

    actionContainer->removeAction(action);
    node.removeKey("QAction");
    node.removeKey("GCF::ActionContainerWidget");
}

void GCF::ActionContainerItemNodeInitializer::showNode(ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node["hideWhenInactive"].toBool() == false)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::ActionContainerWidget* actionContainer = node.hasKey("GCF::ActionContainer") ? node["GCF::ActionContainerWidget"].value<GCF::ActionContainerWidget*>() : 0;
    if(!action || !actionContainer)
        return;

    // action->setVisible(true);
    action->setEnabled(true);
}

void GCF::ActionContainerItemNodeInitializer::hideNode(ComponentGuiNode node)
{
    GCF::AbstractComponent* comp = node.component();
    if(!comp)
        return;

    if(node["hideWhenInactive"].toBool() == false)
        return;

    QAction* action = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
    GCF::ActionContainerWidget* actionContainer = node.hasKey("GCF::ActionContainerWidget") ? node["GCF::ActionContainerWidget"].value<GCF::ActionContainerWidget*>() : 0;
    if(!action || !actionContainer)
        return;

    // action->setVisible(true);
    action->setEnabled(false);
}

/****************************************************************************
GCF::ComponentGuiXmlParser Implementation
****************************************************************************/

struct GCF::ComponentGuiXmlParserData
{
    GCF::ComponentGui gui;
    int connectionIndex;
    int methodIndex;
};

GCF::ComponentGuiXmlParser::ComponentGuiXmlParser()
{
    d = new GCF::ComponentGuiXmlParserData;
}

GCF::ComponentGuiXmlParser::~ComponentGuiXmlParser()
{
    delete d;
}

bool GCF::ComponentGuiXmlParser::parseGui(GCF::ComponentGui gui, const QString& file)
{
    // Unless a main window is specified, the component cannot be loaded.
    if(!GCF::AbstractComponent::componentHostWindow())
    {
        qWarning("No host window specified. Use GCF::AbstractComponent::setComponentHostWindow()");
        return false;
    }

    // cannot parse for invalid gui's
    if(!gui.component())
    {
        qWarning("No component associated with the GUI.");
        return false;
    }

    // Second initialization not possible.
    if(gui.nodeCount())
    {
        qWarning("Initialization of the GUI can be done only once for every component");
        return false;
    }

    QFile f(file);
    if(!f.open(QFile::ReadOnly))
    {
        qWarning("Cannot open GUIXML \"%s\"", qPrintable(file));
        return false;
    }

    QString fContents(f.readAll());
    f.close();

    QDomDocument doc;
    QString errMsg;
    if(!doc.setContent(fContents, &errMsg))
    {
        qWarning("GUIXML parse error: %s", qPrintable(errMsg));
        return false;
    }

    d->gui = gui;
    d->connectionIndex = 0;
    d->methodIndex = 0;
    parseDomDoc(doc);

    return true;
}

void GCF::ComponentGuiXmlParser::parseDomDoc(QDomDocument doc)
{
    QDomElement root = doc.documentElement();
    if(root.attribute("name") != d->gui.component()->componentName())
    {
        qWarning("Component name mismatch %s [Expecting %s]",
            qPrintable(root.attribute("name")), qPrintable(d->gui.component()->componentName()));
        return;
    }
    storeAttributes(root, d->gui.guiData());

    QDomNodeList nodes = root.childNodes();
    for(int i=0; i<nodes.count(); i++)
    {
        QDomNode node = nodes.item(i);
        QDomElement nodeE = node.toElement();
        if(nodeE.isNull())
            continue;

        QString tag = nodeE.tagName().toLower();

        if(tag == "object")
            parseObjectElement(nodeE);
        else if(tag == "widget")
            parseWidgetElement(nodeE);
        else if(tag == "action")
            parseActionElement(nodeE);
        else if(tag == "menu")
            parseMenuElement(nodeE);
        else if(tag == "toolbar")
            parseToolbarElement(nodeE);
        else if(tag == "group")
            parseGroupElement(nodeE);
        else if(tag == "connection")
            parseConnectionElement(nodeE);
        else if(tag == "property")
            parsePropertyElement(nodeE);
        else if(tag == "method")
            parseMethodElement(nodeE);
        else if(tag == "activex")
            parseActiveXElement(nodeE);
        else if(tag == "kpart")
            parseKPartElement(nodeE);
        else if(tag == "actioncontainer")
            parseActionContainerElement(nodeE);
    }
}

void GCF::ComponentGuiXmlParser::parseObjectElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Object);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseWidgetElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Widget);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseActionElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Action);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseMenuElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Menu);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);

    QList<QDomElement> menuItems = elementList(e, "menuitem");
    for(int i=0; i<menuItems.count(); i++)
    {
        GCF::ComponentGuiNode itemNode;
        itemNode.setType(GCF::ComponentGuiNode::MenuItem);
        itemNode.setName(menuItems[i].attribute("name"));
        storeAttributes(menuItems[i], itemNode.nodeData());
        node.addChild(itemNode);
    }
}

void GCF::ComponentGuiXmlParser::parseToolbarElement(QDomElement e)
{
    if(GCF::AbstractComponent::isMenuStripEnabled())
        return;

    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Toolbar);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);

    QList<QDomElement> toolbarItems = elementList(e, "toolbaritem");
    for(int i=0; i<toolbarItems.count(); i++)
    {
        GCF::ComponentGuiNode itemNode;
        itemNode.setType(GCF::ComponentGuiNode::ToolbarItem);
        itemNode.setName(toolbarItems[i].attribute("name"));
        storeAttributes(toolbarItems[i], itemNode.nodeData());
        node.addChild(itemNode);
    }
}

void GCF::ComponentGuiXmlParser::parseGroupElement(QDomElement e)
{
    /*
    With GCF 2.0, group elements are parsed as menu and toolbar elements
    when menu strip is disabled. This is to allow safe and quick switching
    between UI schemes.
    */

    GCF::ComponentGuiNode node;
    if(GCF::AbstractComponent::isMenuStripEnabled())
        node.setType(GCF::ComponentGuiNode::GroupItem);
    else
        node.setType(GCF::ComponentGuiNode::Menu);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);

    QList<QDomElement> groupActionItems = elementList(e, "groupitem");
    for(int i=0; i<groupActionItems.count(); i++)
    {
        GCF::ComponentGuiNode itemNode;
        if(GCF::AbstractComponent::isMenuStripEnabled())
            itemNode.setType(GCF::ComponentGuiNode::GroupActionItem);
        else
            itemNode.setType(GCF::ComponentGuiNode::MenuItem);
        itemNode.setName(groupActionItems[i].attribute("name"));
        storeAttributes(groupActionItems[i], itemNode.nodeData());
        node.addChild(itemNode);
    }

    if(GCF::AbstractComponent::isMenuStripEnabled())
    {
        QList<QDomElement> drawerItems = elementList(e, "draweritem");
        for(int i=0; i<drawerItems.count(); i++)
        {
            QDomElement itemE = drawerItems[i];

            GCF::ComponentGuiNode itemNode;
            itemNode.setType(GCF::ComponentGuiNode::GroupDrawerWidgetItem);
            itemNode.setName( itemE.attribute("name") );
            storeAttributes( itemE, itemNode.nodeData() );
            node.addChild(itemNode);
        }

        return;
    }

    { // Replicate the menu as toolbar also.
        GCF::ComponentGuiNode node;
        node.setType(GCF::ComponentGuiNode::Toolbar);
        QString tbName = QString("%1_toolbar").arg(e.attribute("name"));
        node.setName(tbName);
        storeAttributes(e, node.nodeData());
        if( node.hasKey("mergeWith") )
        {
            QString mw = node["mergeWith"].toString();
            mw = QString("%1_toolbar").arg(mw);
            node["mergeWith"] = mw;
        }
        if( node.hasKey("before") )
        {
            QString before = node["before"].toString();
            before = QString("%1_toolbar").arg(before);
            node["before"] = before;
        }
        d->gui.addNode(node);

        QList<QDomElement> groupActionItems = elementList(e, "groupitem");
        for(int i=0; i<groupActionItems.count(); i++)
        {
            GCF::ComponentGuiNode itemNode;
            itemNode.setType(GCF::ComponentGuiNode::ToolbarItem);
            itemNode.setName(groupActionItems[i].attribute("name"));
            storeAttributes(groupActionItems[i], itemNode.nodeData());
            node.addChild(itemNode);
        }
    }
}

void GCF::ComponentGuiXmlParser::parseConnectionElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Connection);
    node.setName(QString("connection%1").arg(++d->connectionIndex));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parsePropertyElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Property);
    node.setName(QString("property%1").arg(++d->connectionIndex));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseMethodElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::Method);
    node.setName(QString("method%1").arg(++d->methodIndex));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseActiveXElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::ActiveX);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseKPartElement(QDomElement e)
{
#ifdef GCF_KPARTS_AVAILABLE
    static KComponentData compData("GCF");
#endif

    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::KPart);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);
}

void GCF::ComponentGuiXmlParser::parseActionContainerElement(QDomElement e)
{
    GCF::ComponentGuiNode node;
    node.setType(GCF::ComponentGuiNode::ActionContainer);
    node.setName(e.attribute("name"));
    storeAttributes(e, node.nodeData());
    d->gui.addNode(node);

    QList<QDomElement> actionItems = elementList(e, "actioncontaineritem");
    for(int i=0; i<actionItems.count(); i++)
    {
        QDomElement itemE = actionItems[i];

        GCF::ComponentGuiNode itemNode;
        itemNode.setType(GCF::ComponentGuiNode::ActionContainerItem);
        itemNode.setName(itemE.attribute("name"));
        storeAttributes(itemE, itemNode.nodeData());
        node.addChild(itemNode);
    }
}

QList<QDomElement> GCF::ComponentGuiXmlParser::elementList(QDomElement e, const QString& tagName)
{
    QList<QDomElement>  ret;

    if(e.isNull())
        return ret;

    QString tagNameLower = tagName.toLower();

    QDomNodeList nodeList = e.childNodes();
    for(int i=0; i<nodeList.count(); i++)
    {
        QDomNode node = nodeList.item(i);
        QDomElement nodeE = node.toElement();
        if(nodeE.isNull())
            continue;

        if(tagName.isEmpty())
            ret.append(nodeE);
        else if(nodeE.tagName().toLower() == tagNameLower)
            ret.append(nodeE);
    }

    return ret;
}

void GCF::ComponentGuiXmlParser::storeAttributes(QDomElement e, GCF::DataStore dataStore)
{
    QDomNamedNodeMap map = e.attributes();
    for(int i=0; i<map.count(); i++)
    {
        QDomNode node = map.item(i);
        QDomAttr attr = node.toAttr();
        if(attr.isNull() || attr.name()=="name")
            continue;
        QString attrValue = attr.value();
        if(attrValue.contains("${PWD}"))
            attrValue.replace("${PWD}", QDir::currentPath());
        if(attrValue.contains("${APPDIR}"))
            attrValue.replace("${APPDIR}", QApplication::applicationDirPath());
        if(attrValue.contains("${HOMEDIR}"))
            attrValue.replace("${HOMEDIR}", QDir::homePath());
        if(attrValue.contains("${TEMPDIR}"))
            attrValue.replace("${TEMPDIR}", QDir::tempPath());
        if(attrValue.contains("${ROOTDIR}"))
            attrValue.replace("${ROOTDIR}", QDir::tempPath());
        dataStore[attr.name()] = attrValue;
    }
    dataStore["autoDelete"] = false;
}


