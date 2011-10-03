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

#include "ScriptEditorComponent.h"
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>
#include "IXmlUtils.h"
#include <GCF/MenuStrip.h>
#include "ScriptEditor.h"
#include <GCF/ComponentGui.h>
#include <GCF/ComponentGui.h>
#include "ScriptEngineManager.h"
#include "ScriptableObjectExplorer.h"
#include "LoadFormDialog.h"
#include "ScriptEditorConfigurationPage.h"
#include "ui_UnloadFormDialog.h"

#include <QMap>
#include <QFile>
#include <QAction>
#include <QPointer>
#include <QUiLoader>
#include <QTextEdit>
#include <QTabWidget>
#include <QScrollBar>
#include <QMenu>
#include <QToolBar>
#include <QResource>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QMetaMethod>
#include <QFile>
#include <QDomDocument>

#ifdef GCF_ACTIVEQT_AVAILABLE
#include <QAxWidget>
#endif

#ifdef GCF_KPARTS_AVAILABLE
#include "kparts/part.h"
#endif

struct GCF::Components::ScriptEditorComponentData
{
    ScriptEditorComponentData() : scriptEditor(0), workspaceTabWidget(0), soExplorer(0), xmlUtils(0),
        scriptActionsGroup(0), configPageSheet(0) { }

    GCF::Components::ScriptEditor* scriptEditor;
    QTabWidget* workspaceTabWidget;
    GCF::Components::ScriptEngineManager scriptEngineManager;
    GCF::Components::ScriptableObjectExplorer* soExplorer;
    IXmlUtils* xmlUtils;
    GCF::MenuStripGroup* scriptActionsGroup;
    QList<QAction*> scriptActions;
    GCF::Components::ScriptEditorConfigurationPage* configPageSheet;

    QStringList widgetNames;
    QList<QWidget*> widgetList;

    QMap<QString, GCF::ComponentGuiNode> loadedFormsMap;
    QMap<QString, QString> formUiStringMap;

    QString formatArgs(QString objectName, QString signal);

};

GCF_DEFINE_STD_COMPONENT(ScriptEditorComponent)

GCF::Components::ScriptEditorComponent& GCF::Components::ScriptEditorComponent::instance()
{
    GCF::Components::ScriptEditorComponent* theInstance = GCF_CREATE_COMPONENT(ScriptEditorComponent);
    return *theInstance;
}

GCF::Components::ScriptEditorComponent::ScriptEditorComponent()
{
    d = new GCF::Components::ScriptEditorComponentData;
    d->scriptEditor = new GCF::Components::ScriptEditor;
    d->soExplorer = new GCF::Components::ScriptableObjectExplorer;
    d->scriptEditor->setScriptingEngineManager(&d->scriptEngineManager);
    d->scriptEngineManager.setScriptEditor(d->scriptEditor);
    d->soExplorer->setScriptEngineManager(&d->scriptEngineManager);
    d->scriptEngineManager.setScriptableObjectExplorer(d->soExplorer);

    connect(d->soExplorer, SIGNAL(objectDoubleClicked(QObject*, QString)),
            this, SLOT(on_scriptableObjectExplorer_objectDoubleClicked(QObject*, QString)));
    connect(d->soExplorer, SIGNAL(signalDoubleClicked(QObject*, QString, QString, QString)),
            this, SLOT(on_scriptableObjectExplorer_signalDoubleClicked(QObject*, QString, QString, QString)));
    connect(&d->scriptEngineManager, SIGNAL(scriptError(QString,int)), this,
            SLOT(on_scriptEngineManager_scriptError(QString,int)));
}

GCF::Components::ScriptEditorComponent::~ScriptEditorComponent()
{
    delete d;
}

void GCF::Components::ScriptEditorComponent::addScriptableObject(QObject* object, QString name, bool excludeChildren)
{
    d->scriptEngineManager.addScriptableObject(object, name, excludeChildren);
}

void GCF::Components::ScriptEditorComponent::removeScriptableObject(QObject* object)
{
    d->scriptEngineManager.removeScriptableObject(object);
}

void GCF::Components::ScriptEditorComponent::removeScriptableObject(QString name)
{
    d->scriptEngineManager.removeScriptableObject(name);
}

GCF::Components::ScriptEngineManager* GCF::Components::ScriptEditorComponent::scriptEngineManager()
{
    return &d->scriptEngineManager;
}

QWidget* GCF::Components::ScriptEditorComponent::loadForm(QString uiFile)
{
    return loadForm(uiFile, "MainWindowComponent.rightDockArea", QString());
}

QWidget* GCF::Components::ScriptEditorComponent::loadForm(QString uiFile, QString parentName)
{
    return loadForm(uiFile, parentName, QString());
}

QWidget* GCF::Components::ScriptEditorComponent::loadForm(QString uiFile, QString parName, QString hint)
{
    QFile file(uiFile);
    if(!file.open(QFile::ReadOnly))
        return 0;

    return loadForm(&file, uiFile, parName, hint);
}

bool GCF::Components::ScriptEditorComponent::unloadForm(QString formName)
{
    if(!d->loadedFormsMap.contains(formName))
        return false;

    GCF::ComponentGuiNode widgetNode = d->loadedFormsMap[formName];
    QWidget* widget = widgetNode["_QWidget_"].value<QWidget*>();
    d->scriptEngineManager.removeScriptableObject(widget);
    d->soExplorer->monitorEvents(widget, false);
    componentGui().removeNode(widgetNode);
    d->loadedFormsMap.remove(formName);
    d->formUiStringMap.remove(formName);

    emit projectPartModified();

    return true;
}

bool GCF::Components::ScriptEditorComponent::unloadForm(QWidget* widget)
{
    if(!widget)
        return false;

    return unloadForm(widget->objectName());
}

QWidget* GCF::Components::ScriptEditorComponent::loadForm(QIODevice* uiFileDevice, QString uiFile, QString parName, QString hint)
{
    QUiLoader uiLoader;
    QWidget* widget = 0;
    QString parentName = parName;

    if(parentName.isEmpty())
        parentName = "MainWindowComponent.rightDockArea";

    widget = uiLoader.load(uiFileDevice);
    if(!widget)
        return 0;

    // Unload the old form if any
    unloadForm(widget->objectName());

    // Register the widget with the script engine and my map.
    d->scriptEngineManager.addScriptableObject(widget, widget->objectName(), false);
    d->soExplorer->monitorEvents(widget, true);

    // Now add the widget as a GUI node of this component.
    GCF::ComponentGuiNode widgetNode;
    widgetNode.setType(GCF::ComponentGuiNode::Widget);
    widgetNode.setName(widget->objectName());
    widgetNode["name"] = widget->objectName();
    widgetNode["parentWidget"] = parentName;
    widgetNode["hint"] = hint;
    widgetNode["title"] = widget->windowTitle();
    widgetNode["hideWhenInactive"] = false;
    widgetNode["uiFile"] = uiFile;
    widgetNode["_QWidget_"] = qVariantFromValue<QWidget*>(widget);
	widgetNode["_QObject_"] = qVariantFromValue<QObject*>((QObject*)widget);
    d->loadedFormsMap[widget->objectName()] = widgetNode;
    componentGui().addNode(widgetNode);

	QObject::connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(on_formDestroyed(QObject*)));

    emit projectPartModified();

    return widget;
}

QString GCF::Components::ScriptEditorComponent::getSaveFileName(QString caption, QString filter, QString directory)
{
    QString dir = directory;
    QString cap = caption;

    if(dir.isEmpty())
        dir = QDir::homePath();
    if(cap.isEmpty())
        cap = "System Save File Dialog";

    return QFileDialog::getSaveFileName(GCF::AbstractComponent::mainWindow(), cap, dir, filter);
}

QString GCF::Components::ScriptEditorComponent::getOpenFileName(QString caption, QString filter, QString directory)
{
    QString dir = directory;
    QString cap = caption;

    if(dir.isEmpty())
        dir = QDir::homePath();
    if(cap.isEmpty())
        cap = "System Open File Dialog";

    return QFileDialog::getOpenFileName(GCF::AbstractComponent::mainWindow(), cap, dir, filter);
}

void GCF::Components::ScriptEditorComponent::showMessageBox(QString message, QString title)
{
    QMessageBox::information(GCF::AbstractComponent::mainWindow(), message, title);
}

QObject* GCF::Components::ScriptEditorComponent::addAction(QString actionName, QString text, QString iconName, int rowSpan, int colSpan)
{
    if(!GCF::AbstractComponent::isMenuStripEnabled())
        return 0;

    GCF::MenuStrip* menuStrip = qobject_cast<GCF::MenuStrip*>(GCF::AbstractComponent::mainWindow()->menuWidget());
    if(!menuStrip)
        return 0;

    if(!d->scriptActionsGroup)
    {
        int groupCount = menuStrip->groupCount();
        GCF::MenuStripGroup* beforeGroup = menuStrip->group(groupCount-1);
        d->scriptActionsGroup = menuStrip->insertGroup("Script Actions", beforeGroup);
        d->scriptActionsGroup->setIcon( QIcon(":/ScriptEditor/scriptingenv.png") );
    }

    if(!d->scriptActionsGroup)
        return 0;

    static int count=0;
    QAction* action = 0;

    for(int i=0; i<d->scriptActions.count(); i++)
    {
        action = d->scriptActions[i];
        if(action->iconText() == text)
            break;
        action = 0;
    }

    if(!action)
    {

        int rows = rowSpan;
        int cols = colSpan;
        if(rows < 1)
            rows = 1;
        if(cols < 1)
            cols = 1;
        if(rows > 2)
            rows = 2;
        if(cols > 3)
            cols = 3;

        action = new QAction(text, this);
        action->setIconText(text);
        action->setIcon( QIcon(iconName) );
        action->setProperty("rowSpan", rows);
        action->setProperty("colSpan", cols);
        action->setObjectName(actionName);

        d->scriptActionsGroup->addAction(action);
        d->scriptActions.append(action);
        d->scriptEngineManager.addScriptableObject(action, actionName);
    }

    return action;
}

bool GCF::Components::ScriptEditorComponent::removeAction(QObject* actionObject)
{
    if(!actionObject)
        return false;

    QAction* action = qobject_cast<QAction*>(actionObject);
    if(!action || !d->scriptActionsGroup)
        return false;

    if(!d->scriptActions.contains(action))
        return false;

    d->scriptActionsGroup->removeAction(action);
    d->scriptEngineManager.removeScriptableObject(action);
    d->scriptActions.removeAll(action);
    action->deleteLater();

    if(!d->scriptActions.count())
    {
        d->scriptActionsGroup->deleteLater();
        d->scriptActionsGroup = 0;
    }

    return true;
}

bool GCF::Components::ScriptEditorComponent::removeAction(QString actionName)
{
    if(actionName.isEmpty())
        return false;

    QAction* action = 0;
    for(int i=0; i<d->scriptActions.count(); i++)
    {
        action = d->scriptActions[i];
        if(action->objectName() == actionName)
            break;
        action = 0;
    }

    return this->removeAction(action);
}

void GCF::Components::ScriptEditorComponent::loadForm()
{
    LoadFormDialog dialog(GCF::AbstractComponent::mainWindow());
    dialog.setWidgetInfo(&d->widgetNames, &d->widgetList);
    if(dialog.exec() == QDialog::Accepted)
    {
        QString formFile = dialog.formFileName();
        QString parentWidget = dialog.parentWidgetName();
        loadForm(formFile, parentWidget);
    }
}

void GCF::Components::ScriptEditorComponent::unloadForm()
{
    // This is a very simple dialog, so I wont bother implementing the dialog
    // in a UnloadFormDialog.h/cpp file

    QDialog dialog(GCF::AbstractComponent::mainWindow());
    Ui::UnloadFormDialog ui;

    ui.setupUi(&dialog);
    ui.lstLoadedForms->setHeaderLabels(QStringList() << "Form Name" << "UiFile");

    QList<GCF::ComponentGuiNode> formNodes = d->loadedFormsMap.values();
    for(int i=0; i<formNodes.count(); i++)
    {
        GCF::ComponentGuiNode formNode = formNodes[i];
        QString uiFile = formNode["uiFile"].toString();
        QString formName = formNode["name"].toString();
        new QTreeWidgetItem(ui.lstLoadedForms, QStringList() << formName << uiFile);
    }
    ui.lstLoadedForms->resizeColumnToContents(0);

    if(dialog.exec() != QDialog::Accepted)
        return;

    QList<QTreeWidgetItem*> items = ui.lstLoadedForms->selectedItems();
    for(int i=0; i<items.count(); i++)
    {
        QString formName = items[i]->text(0);
        unloadForm(formName);
    }
}

void GCF::Components::ScriptEditorComponent::showScriptEditor()
{
    if(d->workspaceTabWidget)
        d->workspaceTabWidget->setCurrentWidget(d->scriptEditor);
}

void GCF::Components::ScriptEditorComponent::showScriptEditor(QString objectName, QString signal, QString eventName)
{
    if(!objectName.isEmpty() && !eventName.isEmpty() && !signal.isEmpty())
    {
        QString code;
        QString fnObjName = objectName;
        fnObjName = fnObjName.replace('.', '_');
        QString eventCode = QString("function %1_%2").arg(fnObjName).arg(eventName);
        bool success = d->scriptEditor->find(eventCode);
        if(!success)
        {
            QString args = d->formatArgs(objectName, signal);

            eventCode = QString(
                "// Event handler for %1 event of %2\n"
                "function %2_%1(%3)\n"
                "{\n"
                "\n"
                "}\n"
                ).arg(eventName).arg(fnObjName).arg(args);
            eventCode += QString(
                "%1.%2.connect(%4_%3);\n"
                ).arg(objectName).arg(signal.section('(', 0, 0)).arg(eventName).arg(fnObjName);
            code = d->scriptEditor->toPlainText();
            code = code + "\n" + eventCode + "\n\n";
            d->scriptEditor->setPlainText(code);
            d->scriptEditor->verticalScrollBar()->setValue(d->scriptEditor->verticalScrollBar()->maximum());
        }
    }
    showScriptEditor();
}

void GCF::Components::ScriptEditorComponent::highlightScriptableObject(QObject* object)
{
    d->soExplorer->highlightScriptableObject(object);
}

void GCF::Components::ScriptEditorComponent::executeMainScript()
{
    QString code = d->scriptEditor->code();
    d->scriptEngineManager.setUpdateEditor(false);
    d->scriptEngineManager.setScript(code);
    d->scriptEngineManager.setUpdateEditor(true);
    d->scriptEditor->unmarkAllLines();
    d->scriptEngineManager.executeScript();
}

void GCF::Components::ScriptEditorComponent::initializeComponent()
{
    // Do nothing
}

void GCF::Components::ScriptEditorComponent::finalizeComponent()
{
    d->soExplorer->setScriptEngineManager(0);
    d->scriptEngineManager.setScriptableObjectExplorer(0);

    QString guiXml = QString("%1/%2").arg(guiXmlFileDirectory()).arg(guiXmlFileName());
    QFile guiXmlFile(guiXml);
    guiXmlFile.open(QFile::ReadOnly);

    QDomDocument guiXmlDoc;
    guiXmlDoc.setContent( &guiXmlFile );
    guiXmlFile.close();

    QDomElement rootE = guiXmlDoc.documentElement();
    QDomNodeList nodes = rootE.childNodes();
    QStringList toDeleteKeys;
    toDeleteKeys << "ScriptEditorComponent.scriptEditor::TabSpace";
    toDeleteKeys << "ScriptEditorComponent.scriptEditor::FontSize";
    toDeleteKeys << "ScriptEditorComponent.scriptEditor::FontName";

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
    QDomElement e3 = guiXmlDoc.createElement("property");
    rootE.appendChild(e1);
    rootE.appendChild(e2);
    rootE.appendChild(e3);
    e1.setAttribute("key", "ScriptEditorComponent.scriptEditor::TabSpace");
    e1.setAttribute("value", d->scriptEditor->tabSpace());
    e2.setAttribute("key", "ScriptEditorComponent.scriptEditor::FontSize");
    e2.setAttribute("value", d->scriptEditor->fontSize());
    e3.setAttribute("key", "ScriptEditorComponent.scriptEditor::FontName");
    e3.setAttribute("value", d->scriptEditor->fontName());

    QString guiXmlStr = guiXmlDoc.toString();
    guiXmlFile.open(QFile::WriteOnly);
    guiXmlFile.write(guiXmlStr.toAscii());
}

void GCF::Components::ScriptEditorComponent::creationAnnounced()
{
    {
        QString completeVisName;
        GCF::AbstractComponent* xmlUtilsComp = 0;
        QObject* xmlUtilsObject = 0;

        xmlUtilsObject = findObject("IXmlUtils", completeVisName, &xmlUtilsComp);
        if(xmlUtilsObject)
            d->xmlUtils = qobject_cast<IXmlUtils*>(xmlUtilsObject);
    }

    {
        QTabWidget* tabWidget = 0;
        QWidget* seParent = d->scriptEditor->parentWidget();
        while(!tabWidget && seParent)
        {
            tabWidget = qobject_cast<QTabWidget*>(seParent);
            seParent = seParent->parentWidget();
        }

        if(tabWidget)
            d->workspaceTabWidget = tabWidget;
    }

    {
        // Connect to main window to find out when to make this component
        // active and inactive.
        QWidget* widget = GCF::AbstractComponent::componentHostWidget();
        if(widget)
            connect(widget, SIGNAL(workspaceWidgetActivated(QWidget*)),
                    this, SLOT(on_workspaceWidgetActivated(QWidget*)));
        setActive(false);
    }

    GCF::ComponentFactory& compFactory = GCF::ComponentFactory::instance();
    QList<GCF::AbstractComponent*> components = compFactory.components();
    for(int i=0; i<components.count(); i++)
    {
        GCF::AbstractComponent* component = components[i];
        QString compName = component->componentName();
        gatherScriptableObjectsFrom(component);
    }

    QStringList compNames;
    QList<QObject*> types = findObjects("IScriptDataType", compNames);
    for(int i=0; i<types.count(); i++)
    {
        IScriptDataType* type = qobject_cast<IScriptDataType*>(types[i]);
        if(type && type->containerObject() != this)
            d->scriptEngineManager.addScriptDataType(type);
    }

    d->soExplorer->setScriptEngineManager(&d->scriptEngineManager);
    d->scriptEngineManager.setScriptableObjectExplorer(d->soExplorer);

    // Make a list of all widget names for use with loadForm().
    d->widgetNames.clear();
    d->widgetList.clear();
    QStringList widgetNames;
    QList<QObject*> widgetObjects = findObjects("QWidget", widgetNames);
    for(int i=0; i<widgetObjects.count(); i++)
    {
        QWidget* widget = dynamic_cast<QWidget*>(widgetObjects[i]);
        if(qobject_cast<QMenu*>(widget) || qobject_cast<QToolBar*>(widget))
            continue;
        d->widgetNames.append(widgetNames[i]);
        d->widgetList.append(widget);
    }
}

void GCF::Components::ScriptEditorComponent::componentCreated(GCF::AbstractComponent* comp)
{
    gatherScriptableObjectsFrom(comp);

    QString ifaceName = QString("%1.IScriptDataType").arg(comp->componentName());
    QStringList compNames;
    QList<QObject*> types = findObjects(ifaceName, compNames);
    for(int i=0; i<types.count(); i++)
    {
        IScriptDataType* type = qobject_cast<IScriptDataType*>(types[i]);
        if(type)
            d->scriptEngineManager.addScriptDataType(type);
    }

    // Gather new widgets into the widget list.
    QStringList widgetNames;
    QList<QObject*> widgetObjects = findObjects("QWidget", widgetNames);
    for(int i=0; i<widgetObjects.count(); i++)
    {
        QWidget* widget = dynamic_cast<QWidget*>(widgetObjects[i]);
        if(qobject_cast<QMenu*>(widget) || qobject_cast<QToolBar*>(widget))
            continue;
        d->widgetNames.append(widgetNames[i]);
        d->widgetList.append(widget);
    }
}

void GCF::Components::ScriptEditorComponent::componentDestroyed(GCF::AbstractComponent* comp)
{
    releaseScriptableObjectsFrom(comp);

    QString ifaceName = QString("%1.IScriptDataType").arg(comp->componentName());
    QStringList compNames;
    QList<QObject*> types = findObjects(ifaceName, compNames);
    for(int i=0; i<types.count(); i++)
    {
        IScriptDataType* type = qobject_cast<IScriptDataType*>(types[i]);
        if(type)
            d->scriptEngineManager.removeScriptDataType(type);
    }

    QStringList widgetNames;
    QList<QObject*> widgetObjects = findObjects("QWidget", widgetNames);
    for(int i=0; i<widgetObjects.count(); i++)
    {
        QWidget* widget = dynamic_cast<QWidget*>(widgetObjects[i]);
        if(qobject_cast<QMenu*>(widget) || qobject_cast<QToolBar*>(widget))
            continue;
        int index = d->widgetList.indexOf(widget);
        d->widgetList.removeAt(index);
        d->widgetNames.removeAt(index);
    }
}

QObject* GCF::Components::ScriptEditorComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

	GCF::Components::ScriptEditorComponent* that = const_cast<GCF::Components::ScriptEditorComponent*>(this);

    if(comps.last() == "controller")
    {
        return that;
    }

    if(comps.last() == "scriptEngineManager")
        return &d->scriptEngineManager;

    return 0;
}

QWidget* GCF::Components::ScriptEditorComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "scriptEditor")
    {
        return d->scriptEditor;
    }

    if(comps.last() == "scriptableObjectExplorer")
    {
        return d->soExplorer;
    }

    if(comps.last() == "configPageSheet")
    {
        if(!d->configPageSheet)
            d->configPageSheet = new GCF::Components::ScriptEditorConfigurationPage;
        d->configPageSheet->layout()->setMargin(0);
        d->configPageSheet->initFrom(d->scriptEditor, true);
        return d->configPageSheet;
    }

    if(d->loadedFormsMap.contains(comps.last()))
    {
        GCF::ComponentGuiNode widgetNode = d->loadedFormsMap[comps.last()];
        return widgetNode["_QWidget_"].value<QWidget*>();
    }

    return 0;
}

void GCF::Components::ScriptEditorComponent::gatherScriptableObjectsFrom(GCF::AbstractComponent* comp)
{
    GCF::ComponentGui gui = comp->componentGui();
    for(int i=0; i<gui.nodeCount(); i++)
    {
        GCF::ComponentGuiNode node = gui.node(i);
        if(!node.hasKey("scriptName"))
            continue;

        QString scriptName = node["scriptName"].toString();
        bool excludeChildren = node.hasKey("excludeChildren") ? node["receiverObject"].toBool() : true;
        if(scriptName.isEmpty())
            continue;

        switch(node.type())
        {
        case GCF::ComponentGuiNode::Object: {
                QObject* object = node["QObject"].value<QObject*>();
                addScriptableObject(object, scriptName, excludeChildren);
            } break;
        case GCF::ComponentGuiNode::Widget: {
                QWidget* widget = node["QWidget"].value<QWidget*>();
                addScriptableObject(widget, scriptName, excludeChildren);
            } break;
#ifdef GCF_ACTIVEQT_AVAILABLE
        case GCF::ComponentGuiNode::ActiveX: {
                QAxWidget* axWidget = node["QAxWidget"].value<QAxWidget*>();
                addScriptableObject(axWidget, scriptName, excludeChildren);
            } break;
#endif
#ifdef GCF_KPARTS_AVAILABLE
        case GCF::ComponentGuiNode::KPart: {
                KParts::Part* part = node["KParts::Part"].value<KParts::Part*>();
                addScriptableObject(part, scriptName, excludeChildren);
            } break;
#endif
        default:
            break;
        }
    }
}

void GCF::Components::ScriptEditorComponent::releaseScriptableObjectsFrom(GCF::AbstractComponent* comp)
{
    GCF::ComponentGui gui = comp->componentGui();
    for(int i=0; i<gui.nodeCount(); i++)
    {
        GCF::ComponentGuiNode node = gui.node(i);
        if(!node.hasKey("scriptName"))
            continue;

        QString scriptName = node["scriptName"].toString();
        if(scriptName.isEmpty())
            continue;

        switch(node.type())
        {
        case GCF::ComponentGuiNode::Object: {
                QObject* object = node["QObject"].value<QObject*>();
                removeScriptableObject(object);
            } break;
        case GCF::ComponentGuiNode::Widget: {
                QWidget* widget = node["QWidget"].value<QWidget*>();
                removeScriptableObject(widget);
            } break;
        default:
            break;
        }
    }
}

void GCF::Components::ScriptEditorComponent::on_workspaceWidgetActivated(QWidget* widget)
{
    setActive((widget == d->scriptEditor));
}

void GCF::Components::ScriptEditorComponent::on_scriptableObjectExplorer_objectDoubleClicked(QObject* object, QString objectName)
{
    showScriptEditor(objectName, QString(), QString());
    Q_UNUSED(object);
}

void GCF::Components::ScriptEditorComponent::on_scriptableObjectExplorer_signalDoubleClicked(QObject* object, QString objectName, QString event, QString signal)
{
    showScriptEditor(objectName, signal, event);
    Q_UNUSED(object);
}

void GCF::Components::ScriptEditorComponent::on_scriptableObjectExplorer_propertyDoubleClicked(QObject* object, QString objectName, QString method, QString signature)
{
    Q_UNUSED(object);
    Q_UNUSED(objectName);
    Q_UNUSED(method);
    Q_UNUSED(signature);
}

void GCF::Components::ScriptEditorComponent::on_scriptableObjectExplorer_methodDoubleClicked(QObject* object, QString objectName, QString property, QString dataType)
{
    Q_UNUSED(object);
    Q_UNUSED(objectName);
    Q_UNUSED(property);
    Q_UNUSED(dataType);
}

void GCF::Components::ScriptEditorComponent::on_scriptEngineManager_scriptError(QString msg, int lineNr)
{
    d->scriptEditor->markLine(lineNr-1, msg);
    d->scriptEditor->gotoLine(lineNr-1);
    Q_UNUSED(msg);
}

void GCF::Components::ScriptEditorComponent::on_formDestroyed(QObject* obj)
{
	if(!obj)
		return;

	QString objName = obj->objectName();
	unloadForm( obj->objectName() );
}

QObject* GCF::Components::ScriptEditorComponent::containerObject()
{
    return this;
}

QString GCF::Components::ScriptEditorComponent::projectPartID()
{
    return "ScriptEnvironment";
}

bool GCF::Components::ScriptEditorComponent::newProjectPart()
{
    QList<QString> formNames = d->loadedFormsMap.keys();
    for(int i=0; i<formNames.count(); i++)
        unloadForm(formNames[i]);
    d->loadedFormsMap.clear();
    d->formUiStringMap.clear();
    for(int i=0; i<d->scriptActions.count(); i++)
        d->scriptEngineManager.removeScriptableObject(d->scriptActions[i]);
    qDeleteAll(d->scriptActions);
    d->scriptActions.clear();
    delete d->scriptActionsGroup;
    d->scriptActionsGroup = 0;
    return true;
}

bool GCF::Components::ScriptEditorComponent::loadProjectPart(QDomDocument doc, QDomElement partE)
{
    this->newProjectPart();
    if(!d->xmlUtils)
        return false;

    QList<QDomElement> uiFileEList = d->xmlUtils->findElements(partE, "UiFile");
    for(int i=0; i<uiFileEList.count(); i++)
    {
        QDomElement uiFileE = uiFileEList[i];
        QDomNodeList childNodes = uiFileE.childNodes();
        QString name = uiFileE.attribute("Name");
        QString parentWidget = uiFileE.attribute("ParentWidget");
        QString hint = uiFileE.attribute("Hint");
        QString uiFile = uiFileE.attribute("UiFile");

        for(int i=0; i<childNodes.count(); i++)
        {
            QDomCDATASection cdata = childNodes.item(i).toCDATASection();
            if(cdata.isNull())
                continue;
            QByteArray ba = cdata.data().toAscii();
            QBuffer uiBuf(&ba);
            loadForm(&uiBuf, uiFile, parentWidget, hint);
            d->formUiStringMap[name] = cdata.data();
            break;
        }
    }

    return true;
}

bool GCF::Components::ScriptEditorComponent::initializeProjectPart()
{
    return true;
}

bool GCF::Components::ScriptEditorComponent::saveProjectPart(QDomDocument doc, QDomElement partParentE)
{
    QMap<QString, GCF::ComponentGuiNode>::const_iterator it = d->loadedFormsMap.constBegin();
    QMap<QString, GCF::ComponentGuiNode>::const_iterator end = d->loadedFormsMap.constEnd();

    while(it != end)
    {
        QString name = it.key();
        GCF::ComponentGuiNode widgetNode = it.value();

        QDomElement uiFileE = doc.createElement("UiFile");
        partParentE.appendChild(uiFileE);
        uiFileE.setAttribute("Name", name);
        uiFileE.setAttribute("ParentWidget", widgetNode["parentWidget"].toString());
        uiFileE.setAttribute("Hint", widgetNode["hint"].toString());
        uiFileE.setAttribute("HideWhenInactive", widgetNode["hideWhenInactive"].toString());

        QString uiFileName = widgetNode["uiFile"].toString();
        uiFileE.setAttribute("UiFile", uiFileName);

        QString uiFileString;

        if(d->formUiStringMap.contains(name))
            uiFileString = d->formUiStringMap[name];
        else
        {
            QFile file(uiFileName);
            file.open(QFile::ReadOnly);
            uiFileString = file.readAll();
            file.close();
        }

        QDomCDATASection uiFileStringE = doc.createCDATASection(uiFileString);
        uiFileE.appendChild(uiFileStringE);

        ++it;
    }

    return true;
}

bool GCF::Components::ScriptEditorComponent::closeProjectPart()
{
    this->newProjectPart();
    return true;
}

QString GCF::Components::ScriptEditorComponent::productName() const
{
    return "Scripting Environment";
}

QString GCF::Components::ScriptEditorComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::ScriptEditorComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::ScriptEditorComponent::version() const
{
    return "1.0";
}

QString GCF::Components::ScriptEditorComponent::shortDescription() const
{
    return "This component provides the scripting environment for the application. It "
           "provides a script editor, object explorer among others to help script "
           "functionality.";
}

QString GCF::Components::ScriptEditorComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/vtkdesigner";
}

QString GCF::Components::ScriptEditorComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/vtkdesigner/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::ScriptEditorComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, Vision, Design, Development");
        retList << GCF::AboutPerson("Lohith D. S", "NA", "http://www.vcreatelogic.com", "Design, Development");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::ScriptEditorComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::ScriptEditorComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::ScriptEditorComponent::copyrightStatement() const
{
    return QString();
}

QString GCF::Components::ScriptEditorComponent::pageTitle() const
{
    return "Script Editor";
}

QIcon GCF::Components::ScriptEditorComponent::pageIcon() const
{
    return QIcon();
}

QWidget* GCF::Components::ScriptEditorComponent::createPage(QWidget* parent)
{
    ScriptEditorConfigurationPage* page = new ScriptEditorConfigurationPage(parent);
    page->initFrom( d->scriptEditor );
    return page;
}

void GCF::Components::ScriptEditorComponent::savePage(QWidget* page)
{
    ScriptEditorConfigurationPage* page2 = qobject_cast<ScriptEditorConfigurationPage*>(page);
    if( !page2 )
        return;

    page2->saveInto( d->scriptEditor );
}

QString GCF::Components::ScriptEditorComponentData::formatArgs(QString objectName, QString signal)
{
    QObject* obj = 0;
    for(int i=0; i<scriptEngineManager.scriptableObjectCount(); i++)
    {
        if(scriptEngineManager.scriptableObjectName(i) == objectName)
        {
            obj = scriptEngineManager.scriptableObject(i);
            break;
        }
    }

    if(!obj)
        return 0;

    int signalIndex = obj->metaObject()->indexOfSignal( qPrintable(signal) );
    if(signalIndex < 0)
        return 0;

    QMetaMethod method = obj->metaObject()->method( signalIndex );
    QList<QByteArray> paramNames = method.parameterNames();
    QList<QByteArray> paramTypes = method.parameterTypes();
    QString args = paramNames.count() ? QString("\n") : QString();
    for(int i=0; i<paramNames.count(); i++)
    {
        if(i)
            args += ",\n";
        args += QString("    %1 /* %2 */").arg( QString(paramNames[i]) ).arg( QString(paramTypes[i]) );
    }

    if(paramNames.count())
        args += "    ";

    return args;
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(ScriptEditorComponent)

