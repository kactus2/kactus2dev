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

#include <GCF/AbstractComponent.h>
#include <GCF/ComponentGui.h>
#include <GCF/ComponentFactory.h>
#include <GCF/Menu.h>
#include <GCF/MenuStrip.h>
#include <GCF/IComponentHostWindow.h>
#include <GCF/ComponentHostMainWindow.h>
#include <GCF/ComponentSettings.h>

#include <QAction>
#include <QWidget>
#include <QObject>
#include <QMenu>
#include <QToolBar>
#include <QMap>
#include <QMainWindow>
#include <QPointer>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#ifdef GCF_ACTIVEQT_AVAILABLE
#include <QAxWidget>
#endif

#ifdef GCF_KPARTS_AVAILABLE
#include "kparts/part.h"
#endif

/**
\class GCF::AbstractComponent GCF::AbstractComponent.h
\brief Base class for all components.

The GCF::AbstractComponent is the base class for all components to inherit from.
This class defines the rules and protocols for components to interact with the host
application and with other components.

GCF::AbstractComponent is derived from QObject to enable dynamic query capabilities
for components:

\li Dynamic propeties
\li Run-Time-Class identification
\li Signal and Slot capabilities

\note Before creating components, you will need to ensure that the following global
settings are configured.
\li GUI XML File Directory: Configure via setGuiXmlFileDirectory(). All GUI XML files will
be looked up in this directory
\li Component Host Window: Configure via setComponentHostWindow() or setMainWindow(). While
the former allows you to make any widget the component host, the latter will allow you
to make any normal QMainWindow the component host.
\li Menu Strip Usage: Configure via setMenuStripEnabled() method.

\note A valid component host (either a QMainWindow or a IComponentHostWindow) must be
specified. Otherwise GCF's GUI XML system will not work and your application will crash.
*/

struct GCF::AbstractComponentData
{
    AbstractComponentData() : initialized(false), active(false), inFinalize(false),
        inSetActive(false) { }

    QString componentName;
    QString guiXmlFile;
    bool initialized;
    GCF::ComponentGui gui;
    bool active;
    GCF::DataStore componentData;
    bool inFinalize;
    bool inSetActive;
    GCF::ComponentSettings* componentSettings;

    void saveComponentSettings();
    void loadComponentSettings();

    QMap<GCF::AbstractComponent*, int> dependentComps;
    QMap<GCF::AbstractComponent*, int> dependingComps;
};

static QString & guiXmlFileDir()
{
    static QString ret(".");
    return ret;
}

GCF::ComponentHostMainWindow & guiHostWindow(QMainWindow* mainWindow)
{
    static GCF::ComponentHostMainWindow theInstance(mainWindow);
    return theInstance;
}

static IComponentHostWindow* ComponentHostWindow = 0;
static bool MenuStripEnabled = false;

/**
The setGuiXmlFileDirectory() static function can be used to specify a common directory
where all the GUI XML files for the application can be found. By default CF assumes
this to be the current working directory. The guiXmlFileDirectory() method can be used
to query this information.
*/
void GCF::AbstractComponent::setGuiXmlFileDirectory(const QString& absDirPath)
{
    guiXmlFileDir() = absDirPath;
}

/**
The setGuiXmlFileDirectory() static function can be used to specify a common directory
where all the GUI XML files for the application can be found. By default CF assumes
this to be the current working directory. The guiXmlFileDirectory() method can be used
to query this information.
*/
QString GCF::AbstractComponent::guiXmlFileDirectory()
{
    return guiXmlFileDir();
}

/**
Sets the main window for use with all the components.

\warning Do not call this function more than once
\warning Do not call setComponentHostWindow() after calling this function
\warning Do not call this function with a null pointer parameter
*/
void GCF::AbstractComponent::setMainWindow(QMainWindow* mainWindow)
{
    setComponentHostWindow( &(guiHostWindow(mainWindow)) );
}

/**
Returns the main window for use with all the components.
*/
QMainWindow* GCF::AbstractComponent::mainWindow()
{
    if( !componentHostWindow() )
        return 0;

    return qobject_cast<QMainWindow*>( componentHostWindow()->windowWidget() );
}

/**
Sets the host window on to which all components will be placed. All GUI merging
(involving actions, menus, toolbars) will happen on the host window.

\warning Do not call this function more than once
\warning Do not call setMainWindow() after calling this function
\warning Do not call this function with a null pointer parameter
*/
void GCF::AbstractComponent::setComponentHostWindow(IComponentHostWindow* hostWindow)
{
    ComponentHostWindow = hostWindow;
    if(hostWindow && MenuStripEnabled)
        hostWindow->setWindowMenuWidget(new GCF::MenuStrip(hostWindow->windowWidget()));
}

/**
Returns a pointer to the component host window.
*/
IComponentHostWindow* GCF::AbstractComponent::componentHostWindow()
{
    return ComponentHostWindow;
}

/**
Returns the component's host window's widget.
*/
QWidget* GCF::AbstractComponent::componentHostWidget()
{
    if( ComponentHostWindow )
        return ComponentHostWindow->windowWidget();

    return 0;
}

/**
Enabled menu strip usage in the main window. This flag has to be enabled before you
create the main window object and associate it with the component framework via the
\ref setMainWindow() method.

If menu strips are disabled, then XML elements "group" and "groupAction" in the GUI XML file
will be ignored. Otherwise they will be processed.

\note TopLevel menu items may not be shown. They have to be inserted into a group.
\note Toolbars will not be allowed if menu strip is enabled.
\note This method has to be called before calling setMainWindow() or setComponentHostWindow()
methods.
*/
void GCF::AbstractComponent::setMenuStripEnabled(bool val)
{
    if(mainWindow())
        return;

    MenuStripEnabled = val;
}

/**
Returns true if menu strip has been enabled, false otherwise.
*/
bool GCF::AbstractComponent::isMenuStripEnabled()
{
    return MenuStripEnabled;
}

/**
Component constructor. The constructor is protected because components must be created
using the component factory method only.
*/
GCF::AbstractComponent::AbstractComponent():QObject(&GCF::ComponentFactory::instance())
{
    d = new GCF::AbstractComponentData;
    d->componentSettings = new GCF::ComponentSettings(this);
    d->gui.setComponent(this);

    GCF::ComponentFactory::instance().registerComponent(this);
}

/**
Component destructor
*/
GCF::AbstractComponent::~AbstractComponent()
{
    delete d->componentSettings;
    delete d;
    GCF::ComponentFactory::instance().unregisterComponent(this);
}

/**
The componentName() method is used to determine the name of the component. While the
component name can be queried by any class in the system, the component name can
only be set by the component itself. Hence setComponentName() is declared as protected.
Subclasses of GCF::AbstractComponent must set their component name in their constructor.

\note Component names must be unique within the system. This rule has to be enforced
by developers themselves, the CF does not provide this functionality.
*/
QString GCF::AbstractComponent::componentName() const
{
    return d->componentName;
}

/**
This method returns the GUI of the component. The GCF::ComponentGui class, discussed in
a later section, shall provide information about how the component would like to have
its UI placed in the host application's main window. The function constructs GCF::ComponentGui
object by reading the XML file returned by the function guiXmlFileName().
*/
GCF::ComponentGui GCF::AbstractComponent::componentGui() const
{
    return d->gui;
}

/**
The guiXmlFileName() method is used to figure out the name of the GUI XML file to read
while constructing the GCF::ComponentGui object for this component. This function is used by
the componentGui() method. The file name returned by this method is assumed to be relative
to guiXmlFileDirectory().

The setGuiXmlFileName() method can be used by subclasses to specify the name of the XML
file that should be referred to while constructing the GCF::ComponentGui object for the component.
If no name was specified by the subclass then the Gui XML File name is assumed to be the
class name of the component itself followed by .xml extension.
*/
QString GCF::AbstractComponent::guiXmlFileName() const
{
    return d->guiXmlFile;
}

/**
These methods can be used by components or other modules in the system to activate/deactivate
the component. GUI of all the active components is shown whereas those of inactive components
are hidden.
*/
bool GCF::AbstractComponent::isActive() const
{
    return d->active;
}

/**
The dependentComponents() method returns a list of pointers to components that depend on this
component. The dependingComponents() method returns a list of pointers to components the this
component depends on.
*/
QList<GCF::AbstractComponent*> GCF::AbstractComponent::dependentComponents() const
{
    return d->dependentComps.keys();
}

/**
The dependentComponents() method returns a list of pointers to components that depend on this
component. The dependingComponents() method returns a list of pointers to components the this
component depends on.
*/
QList<GCF::AbstractComponent*> GCF::AbstractComponent::dependingComponents() const
{
    return d->dependingComps.keys();
}

/**
Generic data storage for the component. You can store any custom data about the component in this
data store.
*/
GCF::DataStore GCF::AbstractComponent::componentData() const
{
    return d->componentData;
}

/**
Initializes this component if it has not already been initialized
*/
void GCF::AbstractComponent::initialize()
{
    if(d->initialized)
        return;

    this->initializeComponent();

    if(d->componentName.isEmpty())
        setComponentName( QString("%1").arg(metaObject()->className()) );

    if(d->guiXmlFile.isEmpty())
        setGuiXmlFileName( QString("%1.xml").arg(d->componentName) );

    d->loadComponentSettings();

    GCF::ComponentFactory::instance().announceComponentCreation(this);
    this->creationAnnounced();

    d->initialized = true;
}

/**
Finalizes this component. By the time function returns, the component will be deleted
and the UI created by this component will be released.

\note All depending components are finalized first before finalizing this component.
*/
void GCF::AbstractComponent::finalize()
{
    if(d->inFinalize)
        return;

    d->inFinalize = true;

    GCF::ComponentFactory::instance().announceComponentDestruction(this);
    this->destructionAnnounced();

    if(d->dependentComps.count())
    {
        QList<GCF::AbstractComponent*> depComps = d->dependentComps.keys();
        for(int i=0; i<depComps.count(); i++)
        {
            GCF::AbstractComponent* comp = depComps[i];
            if(GCF_CHECK_COMPONENT_PTR(comp))
                comp->finalize();
        }
    }

    this->finalizeComponent();
    d->gui.clear();

    d->saveComponentSettings();

    ::delete this;
}


/**
These methods can be used by components or other modules in the system to activate/deactivate
the component. GUI of all the active components is shown whereas those of inactive components
are hidden.
*/
void GCF::AbstractComponent::setActive(bool val)
{
    if(d->inSetActive)
        return;

    if(d->active == val)
        return;

    d->inSetActive = true;

    if(val)
    {
        this->aboutToActivate();
        d->gui.showGui();
        d->active = val;
        this->justActivated();
    }
    else
    {
        this->aboutToDeactivate();
        d->gui.hideGui();
        d->active = val;
        this->justDeactivated();
    }

    d->inSetActive = false;
}


/**
The componentName() method is used to figure out the name of the component. The
setComponentName() method will be used to set the name of the component. While the
component name can be queried by any class in the system, the component name can
only be set by the component itself. Hence setComponentName() is declared as protected.
Subclasses of GCF::AbstractComponent must set their component name in their constructor.

\note Component names must be unique within the system. This rule has to be enforced
by developers themselves, the CF does not provide this functionality.
\note Component name cannot contain namespaces.
*/
void GCF::AbstractComponent::setComponentName(const QString & name)
{
    if(name.contains("::"))
    {
        QStringList comps = name.split("::");
        d->componentName = comps.last();
    }
    else
        d->componentName = name;
}

/**
The guiXmlFileName() method is used to determine the name of the GUI XML file to read
while constructing the GCF::ComponentGui object for this component. This function is used by
the componentGui() method. The file name returned by this method is assumed to be relative
to guiXmlFileDirectory().

The setGuiXmlFileName() method can be used by subclasses to specify the name of the XML
file that should be referred to while constructing the GCF::ComponentGui object for the component.
If no name was specified by the subclass then the Gui XML File name is assumed to be the
class name of the component itself followed by the .xml extension.

The GUI will be loaded from the specified file before this function returns.
*/
void GCF::AbstractComponent::setGuiXmlFileName(const QString& name)
{
    if(d->initialized)
        qWarning("%s: Setting GUI XML filename after the component has been initialized\n",
                 d->componentName.toAscii().data());
    d->guiXmlFile = name;

    if(d->componentName.isEmpty())
        setComponentName( QString("%1").arg(metaObject()->className()) );

    GCF::ComponentGuiXmlParser parser;
    this->aboutToActivate();
    if( name.startsWith(':') )
        parser.parseGui(d->gui, d->guiXmlFile);
    else
        parser.parseGui(d->gui, QString("%1/%2").arg(guiXmlFileDir()).arg(d->guiXmlFile));
    d->active = true;
    this->justActivated();
}

/**
Adds dependency between two components. Dependency information is used while finalizing components.
Dependent components (components that depend on resources owned by this component) will be finalized
first. This happens recursively.
*/
void GCF::AbstractComponent::addDependency(GCF::AbstractComponent* comp1, GCF::AbstractComponent* comp2)
{
    if(!comp1 || !comp2)
        return;

    comp1->addDependentComponent(comp2);
    comp2->addDependingComponent(comp1);
}

/**
Removes dependency between components.
*/
void GCF::AbstractComponent::removeDependency(GCF::AbstractComponent* comp1, GCF::AbstractComponent* comp2)
{
    if(!comp1 || !comp2)
        return;

    comp1->removeDependentComponent(comp2);
    comp2->removeDependingComponent(comp1);
}

/**
This method can be used by subclasses to query the names of all component in the
system. Using this method components can discover each other.
*/
QStringList GCF::AbstractComponent::componentNames() const
{
    return GCF::ComponentFactory::instance().componentNames();
}

/**
This method can be used by subclasses to query the names and address of any component in the
system given their component name. Using this method components can discover each other.
*/
GCF::AbstractComponent* GCF::AbstractComponent::findComponent(const QString& componentName) const
{
    return GCF::ComponentFactory::instance().findComponent(componentName);
}

/**
Find the address of any action, object, widget, menu or
toolbar in the system given their complete name. Callers of this function can choose to
provide the address of a GCF::AbstractComponent pointer to these functions to find out
the component to which the returned item belongs.
*/
QAction* GCF::AbstractComponent::findAction(const QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;

    GCF::ComponentGuiNode node = d->gui.findNode(completeName, true);
    if(node.isValid() && node.hasKey("QAction"))
    {
        if(component)
            *component = node.component();
        return node["QAction"].value<QAction*>();
    }
    return 0;
}


/**
Find the address of any action, object, widget, menu or
toolbar in the system given their complete name. Callers of this function can choose to
provide the address of a GCF::AbstractComponent to find out the component the 
returned item belongs to.
*/
QObject* GCF::AbstractComponent::findObject(const QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;

    GCF::ComponentGuiNode node = d->gui.findNode(completeName, true);
    if(node.isValid())
    {
        if(node.hasKey("QObject"))
        {
            if(component)
                *component = node.component();
            return node["QObject"].value<QObject*>();
        }
        if(node.hasKey("QWidget"))
        {
            if(component)
                *component = node.component();
            return node["QWidget"].value<QWidget*>();
        }
        if(node.hasKey("QToolBar"))
        {
            if(component)
                *component = node.component();
            return node["QToolBar"].value<QToolBar*>();
        }
        if(node.hasKey("QMenu"))
        {
            if(component)
                *component = node.component();
            return node["QMenu"].value<QMenu*>();
        }
        if(node.hasKey("QAction"))
        {
            if(component)
                *component = node.component();
            return node["QAction"].value<QAction*>();
        }
#ifdef GCF_ACTIVEQT_AVAILABLE
        if(node.hasKey("QAxWidget"))
        {
            if(component)
                *component = node.component();
            return node["QAxWidget"].value<QAxWidget*>();
        }
#endif
#ifdef GCF_KPARTS_AVAILABLE
        if(node.hasKey("KParts::Part"))
        {
            if(component)
                *component = node.component();
            return node["KParts::Part"].value<KParts::Part*>();
        }
#endif
    }
    return 0;
}


/**
Find the address of any action, object, widget, menu or
toolbar in the system given their complete name. Callers of this function can choose to
provide the address of a GCF::AbstractComponent pointer to find out the component
the returned item belongs to.
*/
QWidget* GCF::AbstractComponent::findWidget(const QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;

    GCF::ComponentGuiNode node = d->gui.findNode(completeName, true);
    if(node.isValid() && node.hasKey("QWidget"))
    {
        if(component)
            *component = node.component();
        return node["QWidget"].value<QWidget*>();
    }
    return 0;
}


/**
Find the address of any action, object, widget, menu or
toolbar in the system given their complete name. Callers of this function can choose to
provide the address of a GCF::AbstractComponent pointer to find out the component
the returned item belongs to.
*/
QMenu* GCF::AbstractComponent::findMenu(const QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;

    GCF::ComponentGuiNode node = d->gui.findNode(completeName, true);
    if(node.isValid() && node.hasKey("QMenu"))
    {
        if(component)
            *component = node.component();
        return node["QMenu"].value<QMenu*>();
    }
    return 0;
}


/**
Find the address of any action, object, widget, menu or
toolbar in the system given their complete name. Callers of this function can choose to
provide the address of a GCF::AbstractComponent pointer to find out the component
the returned item belongs to.
*/
QToolBar* GCF::AbstractComponent::findToolBar(const QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;

    GCF::ComponentGuiNode node = d->gui.findNode(completeName, true);
    if(node.isValid() && node.hasKey("QToolBar"))
    {
        if(component)
            *component = node.component();
        return node["QToolBar"].value<QToolBar*>();
    }
    return 0;
}

/**
Find the first object in the system that implements the interface \c ifaceName. The complete
name of the node that implements the interface is returned via completeName. The component
to which the object belongs is returned via \c component. Finally the object itself is
returned from this function.

The following code samples are valid.
\code
{
    QString completeName;
    GCF::AbstractComponent* component = 0;
    QObject* object = findObject("IVisSystemCanvas", completeName, &component);
    if(object)
    {
        // IVisSystemCanvas was found
    }
}

{
    QString completeName;
    GCF::AbstractComponent* component = 0;
    QObject* object = findObject("CVisSystemCanvasComponent.IVisSystemCanvas", completeName, &component);
    // Now findObject first looks for IVisSystemCanvas in CVisSystemCanvasComponent only
    if(object)
    {
        // IVisSystemCanvas was found,
    }
}
\endcode
*/
QObject* GCF::AbstractComponent::findObject(const QString& ifaceName, QString& completeName, GCF::AbstractComponent** component) const
{
    if(component)
        *component = 0;
    completeName.clear();

    QStringList comps = ifaceName.split(".");
    QString compName = comps.count() == 2 ? comps.first() : "";
    QString iName = comps.last();

    GCF::AbstractComponent* ncThis = const_cast<GCF::AbstractComponent*>(this);
    QList<GCF::AbstractComponent*> compList = GCF::ComponentFactory::instance().components();
    compList.removeAll(ncThis);
    compList.prepend(ncThis);

    GCF::AbstractComponent* prefComp = GCF::ComponentFactory::instance().findComponent(compName);
    if(prefComp)
    {
        compList.removeAll(prefComp);
        compList.prepend(prefComp);
    }

    for(int cIndex=0; cIndex<compList.count(); ++cIndex)
    {
        GCF::AbstractComponent* comp = compList[cIndex];
        for(int i=0; i<comp->componentGui().nodeCount(); i++)
        {
            GCF::ComponentGuiNode node = comp->componentGui().node(i);
            QObject* object = 0;
            switch(node.type())
            {
            case GCF::ComponentGuiNode::Object:
                object = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : 0;
                break;
            case GCF::ComponentGuiNode::Widget:
                object = node.hasKey("QWidget") ? node["QWidget"].value<QWidget*>() : 0;
                break;
            case GCF::ComponentGuiNode::Action:
                object = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
                break;
            case GCF::ComponentGuiNode::Menu:
                object = node.hasKey("QMenu") ? node["QMenu"].value<QMenu*>() : 0;
                break;
            case GCF::ComponentGuiNode::Toolbar:
                object = node.hasKey("QToolBar") ? node["QToolBar"].value<QToolBar*>() : 0;
                break;
#ifdef GCF_ACTIVEQT_AVAILABLE
            case GCF::ComponentGuiNode::ActiveX:
                object = node.hasKey("QAxWidget") ? node["QAxWidget"].value<QAxWidget*>() : 0;
                break;
#endif
#ifdef GCF_KPARTS_AVAILABLE
            case GCF::ComponentGuiNode::KPart:
                object = node.hasKey("KParts::Part") ? node["KParts::Part"].value<KParts::Part*>() : 0;
                break;
#endif
            default:
                object = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : 0;
                break;
            }
            if(object && object->inherits(iName.toAscii().data()))
            {
                completeName = node.completeName();
                if(component)
                    *component = node.component();
                return object;
            }
        }

        // Search only in the preferred component.
        if(prefComp)
            break;
    }

    return 0;
}

/**
Find all objects in the system that implement the interface \c ifaceName. The complete
name of the node that implements the interface is returned via completeName. The component
to which the object belongs to is returned via \c component. Finally the object itself is
returned from this function.
*/
QList<QObject*> GCF::AbstractComponent::findObjects(const QString& ifaceName, QStringList& completeNames, QList<GCF::AbstractComponent*>* components) const
{
    QList<QObject*> ret;

    if(components)
        (*components).clear();
    completeNames.clear();

    QStringList comps = ifaceName.split(".");
    QString compName = comps.count() == 2 ? comps.first() : "";
    QString iName = comps.last();

    GCF::AbstractComponent* ncThis = const_cast<GCF::AbstractComponent*>(this);
    QList<GCF::AbstractComponent*> compList = GCF::ComponentFactory::instance().components();
    compList.removeAll(ncThis);
    compList.prepend(ncThis);

    GCF::AbstractComponent* prefComp = GCF::ComponentFactory::instance().findComponent(compName);
    if(prefComp)
    {
        compList.removeAll(prefComp);
        compList.prepend(prefComp);
    }

    for(int cIndex=0; cIndex<compList.count(); ++cIndex)
    {
        GCF::AbstractComponent* comp = compList[cIndex];
        for(int i=0; i<comp->componentGui().nodeCount(); i++)
        {
            GCF::ComponentGuiNode node = comp->componentGui().node(i);
            QObject* object = 0;
            switch(node.type())
            {
            case GCF::ComponentGuiNode::Object:
                object = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : 0;
                break;
            case GCF::ComponentGuiNode::Widget:
                object = node.hasKey("QWidget") ? node["QWidget"].value<QWidget*>() : 0;
                break;
            case GCF::ComponentGuiNode::Action:
                object = node.hasKey("QAction") ? node["QAction"].value<QAction*>() : 0;
                break;
            case GCF::ComponentGuiNode::Menu:
                object = node.hasKey("QMenu") ? node["QMenu"].value<QMenu*>() : 0;
                break;
            case GCF::ComponentGuiNode::Toolbar:
                object = node.hasKey("QToolBar") ? node["QToolBar"].value<QToolBar*>() : 0;
                break;
#ifdef GCF_ACTIVEQT_AVAILABLE
            case GCF::ComponentGuiNode::ActiveX:
                object = node.hasKey("QAxWidget") ? node["QAxWidget"].value<QAxWidget*>() : 0;
                break;
#endif
#ifdef GCF_KPARTS_AVAILABLE
            case GCF::ComponentGuiNode::KPart:
                object = node.hasKey("KParts::Part") ? node["KParts::Part"].value<KParts::Part*>() : 0;
                break;
#endif
            default:
                object = node.hasKey("QObject") ? node["QObject"].value<QObject*>() : 0;
                break;
            }
            if(object && object->inherits(iName.toAscii().data()))
            {
                if( ret.contains(object) )
                    continue;
                ret.append(object);
                completeNames << node.completeName();
                if(components)
                    *components << node.component();
            }
        }

        // Search only in the preferred component.
        if(prefComp)
            break;
    }

    return ret;
}

/**
This function is called when the component GUI (GCF::ComponentGui) class is being
constructed. Default implementations of fetchAction(), fetchMenu() and fetchToolBar()
methods return a new QAction, QMenu and QToolBar object, while fetchWidget() and
fetchObject() return null pointers.

These functions accept the complete name as parameter. Subclasses may implement these
functions to return
valid instances of action, object, widget, menu and toolbar based on their names.
These functions are called by the GCF::ComponentGui class.
*/
QAction* GCF::AbstractComponent::fetchAction(const QString& completeName) const
{
    GCF::AbstractComponent* that = const_cast<GCF::AbstractComponent*>(this);
    QAction* ret = new QAction(that);
    ret->setObjectName(completeName);
    return ret;
}

/**
These functions are called when the component GUI (GCF::ComponentGui) class is being
constructed. Default implementations of fetchAction(), fetchMenu() and fetchToolBar()
methods return a new QAction, QMenu and QToolBar object, while fetchWidget() and
fetchObject() return null pointers.

These functions accept the complete name as parameter. Subclasses may implement these
functions to return
valid instances of action, object, widget, menu and toolbar based on their names.
These functions are called by the GCF::ComponentGui class.
*/
QObject* GCF::AbstractComponent::fetchObject(const QString& completeName) const
{
    Q_UNUSED(completeName);
    return 0;
}

/**
These functions are called when the component GUI (GCF::ComponentGui) class is being
constructed. Default implementations of fetchAction(), fetchMenu() and fetchToolBar()
methods return a new QAction, QMenu and QToolBar object, while fetchWidget() and
fetchObject() return null pointers.

These functions accept the complete name as parameter. Subclasses may implement these
 functions to return
valid instances of action, object, widget, menu and toolbar based on their names.
These functions are called by the GCF::ComponentGui class.
*/
QWidget* GCF::AbstractComponent::fetchWidget(const QString& completeName) const
{
    Q_UNUSED(completeName);
    return 0;
}

/**
These functions are called when the component GUI (GCF::ComponentGui) class is being
constructed. Default implementations of fetchAction(), fetchMenu() and fetchToolBar()
methods return a new QAction, QMenu and QToolBar object, while fetchWidget() and
fetchObject() return null pointers.

These functions accept the complete name as parameter. Subclasses may implement these
functions to return
valid instances of action, object, widget, menu and toolbar based on their names.
These functions are called by the GCF::ComponentGui class.
*/
QMenu* GCF::AbstractComponent::fetchMenu(const QString& completeName) const
{
    QMenu* menu = new GCF::Menu;
    menu->setObjectName(completeName);
    return menu;
}

/**
These functions are called when the component GUI (GCF::ComponentGui) class is being
constructed. Default implementations of fetchAction(), fetchMenu() and fetchToolBar()
methods return a new QAction, QMenu and QToolBar object, while fetchWidget() and
fetchObject() return null pointers.

These functions accept the complete name as parameter. Subclasses may implement these
functions to return
valid instances of action, object, widget, menu and toolbar based on ther names.
These functions are called by the GCF::ComponentGui class.
*/
QToolBar* GCF::AbstractComponent::fetchToolBar(const QString& completeName) const
{
    QToolBar* tb = new QToolBar;
    tb->setObjectName(completeName);
    return tb;
}

/**
These functions are called when a child object is to be placed on top of a object that is
owned by this component. The child object parameter passed to addChildWidget() and
removeChildWidget() functions belong to a object that is owned by another component.
The parent object, however, belongs to this component. Subclasses of GCF::AbstractComponent
can govern how a child object should be added to a component's object.

The hint parameter is used to alter the way in which the child object should be placed on
the parent object. The meaning of hint completely depends on how the component has been
implemented.

The default implementation of this function simply sets parent as the parent object of child.
*/
void GCF::AbstractComponent::addChildObject(QObject* parent, QObject* child, const QString& hint)
{
    if(child && parent)
        child->setParent(parent);
    Q_UNUSED(hint);
}

/**
These functions are called when a child object is to be placed on top of a object that is
owned by this component. The child object parameter passed to addChildWidget() and
removeChildWidget() functions belong to a object that is owned by another component.
The parent object however belongs to this component. Subclasses of GCF::AbstractComponent can
govern how a child object should be added to a component's object.

The hint parameter is used to alter the way in which the child object can be placed on
the parent object. The meaning of hint completely depends on how the component has been
implemented.

The default implementation of this function simply sets parent as the parent object of child.
*/
void GCF::AbstractComponent::removeChildObject(QObject* parent, QObject* child, const QString& hint)
{
    if(child && parent)
        child->setParent(0);
    Q_UNUSED(hint);
}


/**
These functions are called when a child widget is to be placed on top of a widget that is
owned by this component. The child widget parameter passed to addChildWidget() and
removeChildWidget() functions belong to a widget that is owned by another component.
The parent widget however belongs to this component. Subclasses of GCF::AbstractComponent can
govern how a child widget should be added to a component's widget.

The hint parameter is used to alter the way in which the child widget can be placed on
the parent widget. The meaning of hint completely depends on how the component has been
implemented.

The default implementation of this function simply sets parent as the parent widget of child.
*/
void GCF::AbstractComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(child && parent)
        child->setParent(parent);
    Q_UNUSED(hint);
}

/**
These functions are called when a child widget is to be placed on top of a widget that is
owned by this component. The child widget parameter passed to addChildWidget() and
removeChildWidget() functions belong to a widget that is owned by another component.
The parent widget however belongs to this component. Subclasses of GCF::AbstractComponent can
govern how a child widget should be added to a component's widget.

The hint parameter is used to alter the way in which the child widget can be placed on
the parent widget. The meaning of hint completely depends on how the component has been
implemented.

The default implementation of this function simply sets parent as the parent widget of child.
*/
void GCF::AbstractComponent::removeChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(child && parent)
        child->setParent(0);
    Q_UNUSED(hint);
}

/**
These functions are called when a child widget placed on the parent widget is to be shown/hidden.
The showChildWidget() function is called when a child widget in parent needs to be shown.
The hideChildWidget() function is called when a child widget in parent needs to be hidden.
*/
void GCF::AbstractComponent::showChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    Q_UNUSED(parent);
    if(child)
        child->show();
    Q_UNUSED(hint);
}

/**
These functions are called when a child widget placed on the parent widget is to be shown/hidden.
The showChildWidget() function is called when a child widget in parent needs to be shown.
The hideChildWidget() function is called when a child widget in parent needs to be hidden.
*/
void GCF::AbstractComponent::hideChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    Q_UNUSED(parent);
    if(child)
        child->hide();
    Q_UNUSED(hint);
}

/**
These functions are called when a new component is introduced into or removed from the
system. The functions act as a broadcast service for announcing the arrival and
departure of components. Subclasses of GCF::AbstractComponent can implement these functions
to take some action when another component is loaded or unloaded.

\note comp will never be equal to this object.
\note The default implementation does nothing
*/
void GCF::AbstractComponent::componentCreated(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}

/**
These functions are called when a new component is introduced into or removed from the
system. The functions act as a broadcast service for announcing the arrival and
departure of components. Subclasses of GCF::AbstractComponent can implement these functions
to take some action when another component is loaded or unloaded.

\note comp will never be equal to this object.
\note The default implementation does nothing
*/
void GCF::AbstractComponent::componentDestroyed(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}

/**
These functions will be called when the creation or destruction of this component
has been announced to other components in the system.

\note The default implementation does nothing
*/
void GCF::AbstractComponent::creationAnnounced()
{

}

/**
These functions will be called when the creation or destruction of this component
has been announced to other components in the system.

\note The default implementation does nothing
*/
void GCF::AbstractComponent::destructionAnnounced()
{

}

/**
The registerDependentComponent() and registerDependingComponent() methods are called when a
new component is to be added to either one of the lists. The default implementation does nothing.
The unregisterDependentComponent() and unregisterDependingComponent() methods are called when
a component is removed from either of the lists. The default implementation does nothing.

\note This function is called the first time a dependency is detected
*/
void GCF::AbstractComponent::registerDependentComponent(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}

/**
The registerDependentComponent() and registerDependingComponent() methods are called when a
new component is to be added to either one of the lists. The default implementation does nothing.
The unregisterDependentComponent() and unregisterDependingComponent() methods are called when
a component is removed from either of the lists. The default implementation does nothing.

\note This function is called when the last dependency is broken
*/
void GCF::AbstractComponent::unregisterDependentComponent(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}

/**
The registerDependentComponent() and registerDependingComponent() methods are called when a
new component is to be added to either one of the lists. The default implementation does nothing.
The unregisterDependentComponent() and unregisterDependingComponent() methods are called when
a component is removed from either of the lists. The default implementation does nothing.

\note This function is called the first time a dependency is detected
*/
void GCF::AbstractComponent::registerDependingComponent(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}

/**
The registerDependentComponent() and registerDependingComponent() methods are called when a
new component is to be added to either one of the lists. The default implementation does nothing.
The unregisterDependentComponent() and unregisterDependingComponent() methods are called when
a component is removed from either of the lists. The default implementation does nothing.

\note This function is called when the last dependency is broken
*/
void GCF::AbstractComponent::unregisterDependingComponent(GCF::AbstractComponent* comp)
{
    Q_UNUSED(comp);
}


/**
\fn void GCF::AbstractComponent::initializeComponent() = 0

These pure virtual functions are called when the component is about to be initialized or
about to be destroyed. The  methods are called by CF at appropriate times. Subclasses must
implement these functions to perform  initialization and finalization operations.
*/

/**
\fn void GCF::AbstractComponent::finalizeComponent() = 0

These pure virtual functions are called when the component is about to be initialized or
about to be destroyed. The  methods are called by CF at appropriate times. Subclasses must
implement these functions to perform  initialization and finalization operations.
*/

/**
The function aboutToActivate() is called when the component is just about to be activated.
Within this function the component is not yet active. The justActivated() function is called
when the component has just been activated. Within this function the component has become
active. Similarly the aboutToDeactivate() method is called when the component is just about
to be deactivated and justDeactivated() method is called just after the method is deactivated.
The default implementations of these functions do nothing.
*/
void GCF::AbstractComponent::aboutToActivate()
{
    // TODO
}

/**
The function aboutToActivate() is called when the component is just about to be activated.
Within this function the component is not yet active. The justActivated() function is called
when the component has just been activated. Within this function the component has become
active. Similarly the aboutToDeactivate() method is called when the component is just about
to be deactivated and justDeactivated() method is called just after the method is deactivated.
The default implementations of these functions do nothing.
*/
void GCF::AbstractComponent::justActivated()
{
    // TODO
}

/**
The function aboutToActivate() is called when the component is just about to be activated.
Within this function the component is not yet active. The justActivated() function is called
when the component has just been activated. Within this function the component has become
active. Similarly the aboutToDeactivate() method is called when the component is just about
to be deactivated and justDeactivated() method is called just after the method is deactivated.
The default implementations of these functions do nothing.
*/
void GCF::AbstractComponent::aboutToDeactivate()
{
    // TODO
}

/**
The function aboutToActivate() is called when the component is just about to be activated.
Within this function the component is not yet active. The justActivated() function is called
when the component has just been activated. Within this function the component has become
active. Similarly the aboutToDeactivate() method is called when the component is just about
to be deactivated and justDeactivated() method is called just after the method is deactivated.
The default implementations of these functions do nothing.
*/
void GCF::AbstractComponent::justDeactivated()
{
    // TODO
}

/**
\internal
*/
void GCF::AbstractComponent::addDependentComponent(GCF::AbstractComponent* comp)
{
    if(!comp || comp == this)
        return;

    int depCount = 0;
    if(d->dependentComps.contains(comp))
        depCount = d->dependentComps[comp];
    ++depCount;
    d->dependentComps[comp] = depCount;
    if(depCount == 1)
        this->registerDependentComponent(comp);
}

/**
\internal
*/
void GCF::AbstractComponent::removeDependentComponent(GCF::AbstractComponent* comp)
{
    if(!comp || comp == this)
        return;

    if(!d->dependentComps.contains(comp))
        return;
    int depCount = d->dependentComps[comp];
    --depCount;
    if(!depCount)
    {
        d->dependentComps.remove(comp);
        this->unregisterDependentComponent(comp);
    }
}

/**
\internal
*/
void GCF::AbstractComponent::addDependingComponent(GCF::AbstractComponent* comp)
{
    if(!comp || comp == this)
        return;

    int depCount = 0;
    if(d->dependentComps.contains(comp))
        depCount = d->dependingComps[comp];
    ++depCount;
    d->dependingComps[comp] = depCount;
    if(depCount == 1)
        this->registerDependingComponent(comp);
}

/**
\internal
*/
void GCF::AbstractComponent::removeDependingComponent(GCF::AbstractComponent* comp)
{
    if(!comp || comp == this)
        return;

    if(!d->dependingComps.contains(comp))
        return;
    int depCount = d->dependingComps[comp];
    --depCount;
    if(!depCount)
    {
        d->dependingComps.remove(comp);
        this->unregisterDependingComponent(comp);
    }
}

/**
\internal
*/
void GCF::AbstractComponent::objectDestroyed(QObject* obj)
{
    if(!obj)
        return;

    GCF::ComponentGuiNode node = obj->property("_node_").value<GCF::ComponentGuiNode>();
    if(node.isValid())
    {
        bool removeNode = false;

        /*
        if(node.hasKey("QObject") && node["QObject"].value<QObject*>() == obj)
            node["QObject"] = qVariantFromValue<QObject*>(0);
        else if(node.hasKey("QWidget") && node["QWidget"].value<QWidget*>() == obj)
            node["QWidget"] = qVariantFromValue<QWidget*>(0);
        else if(node.hasKey("QAction") && node["QAction"].value<QAction*>() == obj)
            node["QAction"] = qVariantFromValue<QAction*>(0);
        else if(node.hasKey("QMenu") && node["QMenu"].value<QMenu*>() == obj)
            node["QMenu"] = qVariantFromValue<QMenu*>(0);
        else if(node.hasKey("QToolBar") && node["QToolBar"].value<QToolBar*>() == obj)
            node["QToolBar"] = qVariantFromValue<QToolBar*>(0);
        */

        // check if any other key is a QObject, QWidget, QAction, QMenu or QToolBar
        QStringList keys = node.keys();
        for(int i=0; i<keys.count(); i++)
        {
            QString key = keys[i];
            QVariant & value = node[key];
            if(!qstrcmp(value.typeName(), "QObject*") && value.value<QObject*>()==obj)
            {
                value = qVariantFromValue<QObject*>(0);
                removeNode |= true;
            }
            else if(!qstrcmp(value.typeName(), "QWidget*") && value.value<QWidget*>()==obj)
            {
                value = qVariantFromValue<QWidget*>(0);
                removeNode |= true;
            }
            else if(!qstrcmp(value.typeName(), "QAction*") && value.value<QAction*>()==obj)
            {
                value = qVariantFromValue<QAction*>(0);
                removeNode |= true;
            }
            else if(!qstrcmp(value.typeName(), "QMenu*") && value.value<QMenu*>()==obj)
            {
                value = qVariantFromValue<QMenu*>(0);
                removeNode |= true;
            }
            else if(!qstrcmp(value.typeName(), "QToolBar*") && value.value<QToolBar*>()==obj)
            {
                value = qVariantFromValue<QToolBar*>(0);
                removeNode |= true;
            }
#ifdef GCF_ACTIVEQT_AVAILABLE
            else if(!qstrcmp(value.typeName(), "QAxWidget*") && value.value<QAxWidget*>()==obj)
            {
                value = qVariantFromValue<QAxWidget*>(0);
                removeNode |= true;
            }
#endif
#ifdef GCF_KPARTS_AVAILABLE
            else if(!qstrcmp(value.typeName(), "KParts::Part*") && value.value<KParts::Part*>()==obj)
            {
                value = qVariantFromValue<KParts::Part*>(0);
                removeNode |= true;
            }
#endif
        }

        if( removeNode && node.hasKey("autoDeleteNode") && node["autoDeleteNode"].toBool() )
        {
            if( node.parentNode().isValid() )
                node.parentNode().removeChild( node );
            else
                componentGui().removeNode(node);
        }
    }
}

/**
This function can be implemented in subclasses to create a new
instance of the object referenced by \c completeName. The caller
has to take the responsibility of actually deleting the object
returned by this function.

Implementations can return a null pointer to signify that a
duplicate copy of that object cannot be created.
*/
QObject* GCF::AbstractComponent::createObject(const QString& completeName) const
{
    Q_UNUSED(completeName);
    return 0;
}

/**
This function can be implemented in subclasses to create a new
instance of the object referenced by \c completeName. The caller
has to take the responsibility of actually deleting the object
returned by this function.

Implementations can return a null pointer to signify that a
duplicate copy of that object cannot be created.
*/
QWidget* GCF::AbstractComponent::createWidget(const QString& completeName) const
{
    Q_UNUSED(completeName);
    return 0;
}

/**
Delete operator overload. Just in case the developer forgets to call finalize(),
this overload ensures proper finalization of the component.
*/
void GCF::AbstractComponent::operator delete(void* mem)
{
    // Suggested by Till Adam of KDAB

    GCF::AbstractComponent* comp = static_cast<GCF::AbstractComponent*>(mem);
    // For some strange reason static_cast<> doesnt work :(. It always returns a null pointer
    if(comp)
        comp->finalize();
}

/**
Same as \ref findObjects(const QString&, QStringList&, QList<GCF::AbstractComponent*>*). The last two parameters in the
earlier function are now coupled into a QPair.
*/
QList<QObject*> GCF::AbstractComponent::findObjects(const QString& ifaceName, QList< QPair<QString,GCF::AbstractComponent*> >* objectInfoList ) const
{
    // Suggested by Till Adam of KDAB

    QStringList compNames;
    QList<GCF::AbstractComponent*> comps;
    QList<QObject*> retList = findObjects(ifaceName, compNames, &comps);

    if( objectInfoList )
    {
        for(int i=0; i<comps.count(); i++)
            objectInfoList->append( QPair<QString,GCF::AbstractComponent*>(compNames[i], comps[i]) );
    }

    return retList;
}

/**
Returns a non-const reference to the component's settings object. This object is automatically loaded
and saved.

\note This method can only by used by subclasses of \ref AbstractComponent
*/
GCF::ComponentSettings& GCF::AbstractComponent::settings()
{
    return *(d->componentSettings);
}

/**
Returns a const reference to the component's settings object. This object is automatically loaded
and saved.
*/
const GCF::ComponentSettings& GCF::AbstractComponent::constSettings() const
{
    return *(d->componentSettings);
}

void GCF::AbstractComponentData::saveComponentSettings()
{
    QString settingsFile = QString("%1/%2Settings.xml").arg(guiXmlFileDir()).arg(componentName);
    if(this->componentSettings->isEmpty())
    {
        QFile::remove(settingsFile);
        return;
    }

    QFile file(settingsFile);
    if( !file.open(QFile::WriteOnly) )
    {
        qWarning("Cannot open %s for writing", qPrintable(settingsFile));
        return;
    }

    QDomDocument doc;
    QDomElement docE = doc.createElement("Settings");
    doc.appendChild(docE);

    this->componentSettings->writeSettings(doc, docE);

    QString docStr = doc.toString();
    file.write( docStr.toAscii() );
    file.close();
}

void GCF::AbstractComponentData::loadComponentSettings()
{
    QString settingsFile = QString("%1/%2Settings.xml").arg(guiXmlFileDir()).arg(componentName);
    QFile file(settingsFile);
    if( !file.open(QFile::ReadOnly) )
    {
        qWarning("Cannot open %s for reading", qPrintable(settingsFile));
        return;
    }

    QDomDocument doc;
    doc.setContent(&file);

    QDomElement docE = doc.documentElement();
    this->componentSettings->readSettings(doc, docE);
}

/**
This function can be implemented in subclasses to create a new
instance of the widget referenced by \c completeName. The caller
has to take the responsibility of actually deleting the widget
returned by this function.

Implementations can return a null pointer to signify that a
duplicate copy of that widget cannot be created.
*/
/*QWidget* GCF::AbstractComponent::createWidget(const QString& completeName)const
{
    Q_UNUSED(completeName);
    return 0;
}
*/

#include <GCF/IComponentPlugin.h>

void IComponentPlugin::registerComponentInfo(const QString& className, CreateComponentFunction createFn, const QMetaObject* meta)
{
    GCF::ComponentFactory::instance().registerComponentInfo(className, createFn, meta);
}

QString IComponentPlugin::systemBuildKey()
{
    return GCF_BUILD_KEY;
}

//////////////////////////////////////////////////////////////////////////////////////////
// GCF::VersionInfo
//////////////////////////////////////////////////////////////////////////////////////////

/**
\class GCF::VersionInfo Common.h
\brief Provides version information of the GCF library
*/

/**
Returns the complete version information as a string
*/
QString GCF::VersionInfo::versionInfoString()
{
    return QString("Generic Component Framework from VCreate Logic - %1.%2.%3").
                arg( majorVersion() ).arg( minorVersion() ).arg( revisionNumber() );
}

/**
Returns the major version number
*/
int GCF::VersionInfo::majorVersion()
{
    return 2;
}

/**
Returns the minor version number
*/
int GCF::VersionInfo::minorVersion()
{
    return 0;
}

/**
Returns a revision number
*/
int GCF::VersionInfo::revisionNumber()
{
    return 0; // FIXME: Should pick up SVN revision number and show here.
}



