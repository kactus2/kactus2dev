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

#ifndef COMPONENT_GUI_H
#define COMPONENT_GUI_H

#include "Common.h"
#include "DataStore.h"
#include <QDomDocument>
#include <QDomElement>

class QAction;
class QWidget;
class QMenu;
class QToolBar;
class QStatusBar;

namespace GCF
{

    class AbstractComponent;
    class SignalMultiplexer;

    #ifdef GCF_KPARTS_AVAILABLE
    struct KPartsData;
    #endif

    class  ComponentGui;
    struct ComponentGuiNodeData;

    class GCF_EXPORT_CLASS ComponentGuiNode
    {
    public:
        enum Type {
            Object,             // 0
            Widget,             // 1
            Action,             // 2
            Menu,               // 3
            Toolbar,            // 4
            MenuItem,           // 5
            ToolbarItem,        // 6
            GroupItem,          // 7
            GroupActionItem,    // 8
            Connection,         // 9
            Property,           // 10
            Method,             // 11
            ActiveX,            // 12
            KPart,              // 13
            GroupDrawerWidgetItem, // 14
            ActionContainer,     // 15
            ActionContainerItem, // 16
            Other = 1000
        };

        ComponentGuiNode();
        ComponentGuiNode(const ComponentGuiNode & other);
        ~ComponentGuiNode();

        bool operator == (const ComponentGuiNode & other) const;
        ComponentGuiNode & operator = (const ComponentGuiNode & other);

        ComponentGuiNode parentNode() const;
        bool isValid() const;

        DataStore nodeData() const;
        QVariant & operator [] (const QString& key);
        bool hasKey(const QString& key);
        void removeKey(const QString& key);
        QStringList keys() const;

        void setType(int t);
        int type() const;

        void setGui(const ComponentGui & gui);
        const ComponentGui & gui() const;

        void setName(const QString& name) const;
        QString name() const;
        QString completeName() const;

        AbstractComponent* component() const;

        void addChild(ComponentGuiNode child);
        void removeChild(ComponentGuiNode child);
        int childCount();
        ComponentGuiNode child(int index) const;

        void showNode();
        void hideNode();
        void clear();

        ComponentGuiNode findNode(const QString& completeName) const;

    private:
        ComponentGuiNodeData* d;
    };

    struct ComponentGuiData;
    class GCF_EXPORT_CLASS ComponentGui
    {
    public:
        ComponentGui();
        ComponentGui(const ComponentGui & other);
        ~ComponentGui();

        bool operator == (const ComponentGui & other) const;
        ComponentGui & operator = (const ComponentGui & other);

        AbstractComponent* component() const;

        DataStore guiData();
        QVariant & operator [] (const QString& key);
        bool hasKey(const QString& key);
        void removeKey(const QString& key);
        QStringList keys() const;

        void addNode(ComponentGuiNode node);
        void removeNode(ComponentGuiNode node);
        int nodeCount() const;
        ComponentGuiNode node(int index) const;

        void showGui();
        void hideGui();
        void clear();

        ComponentGuiNode findNode(const QString& completeName, bool allComps=false) const;

        // for debugging purposes only
        void dumpGui(const QString& fileName);
        void dumpGuiNode(GCF::ComponentGuiNode node, QDomDocument doc, QDomElement parentE);

    protected:
        void setComponent(AbstractComponent* comp);

    private:
        friend class AbstractComponent;
        ComponentGuiData* d;
    };

    class GCF_EXPORT_INTERFACE AbstractNodeInitializer
    {
    public:
        AbstractNodeInitializer();
        virtual ~AbstractNodeInitializer();
        virtual int handledType() const = 0;
        virtual void initializeNode(ComponentGuiNode node) = 0;
        virtual void cleanupNode(ComponentGuiNode node) = 0;
        virtual void showNode(ComponentGuiNode node) = 0;
        virtual void hideNode(ComponentGuiNode node) = 0;

        virtual QAction* fetchAction(ComponentGuiNode node) const;
        virtual QObject* fetchObject(ComponentGuiNode node) const;
        virtual QWidget* fetchWidget(ComponentGuiNode node) const;
        virtual QMenu* fetchMenu(ComponentGuiNode node) const;
        virtual QToolBar* fetchToolBar(ComponentGuiNode node) const;
    #ifdef GCF_ACTIVEQT_AVAILABLE
        virtual QAxWidget* fetchActiveXWidget(ComponentGuiNode node) const;
    #endif
    #ifdef GCF_KPARTS_AVAILABLE
        virtual KParts::Part* fetchPart(ComponentGuiNode node) const;
    #endif
        QObject* findObject(ComponentGuiNode node, const QString& completeName, AbstractComponent** comp=0);
        void addChildObject(ComponentGuiNode node, QObject* parent, QObject* child, const QString& hint);
        void removeChildObject(ComponentGuiNode node, QObject* parent, QObject* child, const QString& hint);
        void addChildWidget(ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint);
        void removeChildWidget(ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint);
        void showChildWidget(ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint);
        void hideChildWidget(ComponentGuiNode node, QWidget* parent, QWidget* child, const QString& hint);
        void addDependency(AbstractComponent* comp1, AbstractComponent* comp2);
        void removeDependency(AbstractComponent* comp1, AbstractComponent* comp2);

        ComponentGuiNode resolveCompleteSlotName(ComponentGuiNode node, const QString& completeSlotName,
                                     QObject** object, QString& slotName) const;
        ComponentGuiNode resolveCompletePropertyName(ComponentGuiNode node, const QString& completePropertyName,
                                     QObject** object, QString& propertyName) const;
        ComponentGuiNode resolveCompleteMethodName(ComponentGuiNode node, const QString& completeMethodName,
                                     QObject** object, QString& methodName);
    };

    struct NodeInitializerFactoryData;
    class GCF_EXPORT_CLASS NodeInitializerFactory
    {
    public:
        static NodeInitializerFactory & instance();

        ~NodeInitializerFactory();

        QList<int> handledTypes() const;
        AbstractNodeInitializer* initializer(int type) const;

        void initializeNode(ComponentGuiNode node);
        void cleanupNode(ComponentGuiNode node);
        void showNode(ComponentGuiNode node);
        void hideNode(ComponentGuiNode node);

        void setInitializer(int type, AbstractNodeInitializer* i);

    protected:
        NodeInitializerFactory();

    private:
        NodeInitializerFactoryData* d;
    };

    class GCF_EXPORT_CLASS ObjectNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS WidgetNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ActionNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS MenuNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS MenuItemNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ToolBarNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ToolBarItemNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS MenuStripGroupNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS MenuStripGroupActionNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ConnectionNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS PropertyNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS MethodNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ActiveXNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS KPartNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    #ifdef GCF_KPARTS_AVAILABLE
    private:
        /*
        Why am I passing part and xmlGuiClient as separate parameters when KParts::Part is a subclass of KXMLGUIClient.
        GCF separates part (aka component) from guixml. So logically __i__ need them to be separate parameters.
        */
        virtual void mergePartGui(ComponentGuiNode partGuiNode, KParts::Part* part, KXMLGUIClient* xmlGuiClient, GCF::KPartsData& partData);
        virtual void initRootNode(ComponentGuiNode partGuiNode, KParts::Part* part, KXMLGUIClient* xmlGuiClient, GCF::KPartsData& partData);
    #endif
    };

    class GCF_EXPORT_CLASS GroupDrawerWidgetNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    class GCF_EXPORT_CLASS ActionContainerNodeInitializer : public WidgetNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);

    protected:
        virtual QWidget* fetchWidget(ComponentGuiNode node) const;
    };

    class GCF_EXPORT_CLASS ActionContainerItemNodeInitializer : public AbstractNodeInitializer
    {
    public:
        virtual int handledType() const;
        virtual void initializeNode(ComponentGuiNode node);
        virtual void cleanupNode(ComponentGuiNode node);
        virtual void showNode(ComponentGuiNode node);
        virtual void hideNode(ComponentGuiNode node);
    };

    struct ComponentGuiXmlParserData;
    class GCF_EXPORT_CLASS ComponentGuiXmlParser
    {
    public:
        ComponentGuiXmlParser();
        ~ComponentGuiXmlParser();

        bool parseGui(ComponentGui gui, const QString& file);

    protected:
        void parseDomDoc(QDomDocument doc);
        void parseObjectElement(QDomElement e);
        void parseWidgetElement(QDomElement e);
        void parseActionElement(QDomElement e);
        void parseMenuElement(QDomElement e);
        void parseToolbarElement(QDomElement e);
        void parseGroupElement(QDomElement e);
        void parseConnectionElement(QDomElement e);
        void parsePropertyElement(QDomElement e);
        void parseMethodElement(QDomElement e);
        void parseActiveXElement(QDomElement e);
        void parseKPartElement(QDomElement e);
        void parseActionContainerElement(QDomElement e);

    private:
        QList<QDomElement> elementList(QDomElement e, const QString& tagName);
        void storeAttributes(QDomElement e, DataStore dataStore);

    private:
        ComponentGuiXmlParserData* d;
    };

}

Q_DECLARE_METATYPE(GCF::ComponentGui)
Q_DECLARE_METATYPE(GCF::ComponentGuiNode)
Q_DECLARE_METATYPE(GCF::SignalMultiplexer*)

#endif


