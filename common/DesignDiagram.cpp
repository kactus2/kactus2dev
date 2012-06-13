//-----------------------------------------------------------------------------
// File: DesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#include "DesignDiagram.h"

#include <common/diagramgrid.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>

#include <AdHocEditor/AdHocEditor.h>

#include <LibraryManager/libraryinterface.h>

#include <models/designconfiguration.h>
#include <models/component.h>

#include <QWidget>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: DesignDiagram::DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::DesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                             GenericEditProvider& editProvider, QWidget* parent)
    : QGraphicsScene(parent),
      lh_(lh),
      mainWnd_(mainWnd),
      editProvider_(editProvider),
      component_(),
      designConf_(),
      mode_(MODE_SELECT),
      instanceNames_(),
      locked_(false)
{
    setSceneRect(0, 0, 100000, 100000);

    connect(this, SIGNAL(componentInstantiated(ComponentItem*)),
        this, SLOT(onComponentInstanceAdded(ComponentItem*)), Qt::UniqueConnection);
    connect(this, SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SLOT(onComponentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::~DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::~DesignDiagram()
{
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void DesignDiagram::clearScene()
{
    clear();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setDesign()
//-----------------------------------------------------------------------------
bool DesignDiagram::setDesign(QSharedPointer<Component> component, QSharedPointer<Design> design,
                              QSharedPointer<DesignConfiguration> designConf)
{
    // Clear the edit provider.
    editProvider_.clear();

    // clear the previous design configuration
    designConf_.clear();

    // Deselect items.
    emit clearItemSelection();

    // Clear the scene.
    clearScene();

    // Set the new component and open the design.
    component_ = component;
    designConf_ = designConf;

    loadDesign(design);
    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::attach()
//-----------------------------------------------------------------------------
void DesignDiagram::attach(AdHocEditor* editor)
{
    connect(this, SIGNAL(contentChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(destroyed(QObject*)), editor, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::detach()
//-----------------------------------------------------------------------------
void DesignDiagram::detach(AdHocEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::addInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::addInstanceName(QString const& name)
{
    instanceNames_.append(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::removeInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::removeInstanceName(const QString& name)
{
    instanceNames_.removeAll(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::updateInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::updateInstanceName(const QString& oldName, const QString& newName)
{
    instanceNames_.removeAll(oldName);
    instanceNames_.append(newName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setMode()
//-----------------------------------------------------------------------------
void DesignDiagram::setMode(DrawMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;

        if (mode_ != MODE_SELECT)
        {
            emit clearItemSelection();
        }

        emit modeChanged(mode);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setProtection()
//-----------------------------------------------------------------------------
void DesignDiagram::setProtection(bool locked)
{
    locked_ = locked;
    clearSelection();
    emit clearItemSelection();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMode()
//-----------------------------------------------------------------------------
DrawMode DesignDiagram::getMode() const
{
    return mode_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::isProtected()
//-----------------------------------------------------------------------------
bool DesignDiagram::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* DesignDiagram::getLibraryInterface() const
{
    return lh_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMainWindow()
//-----------------------------------------------------------------------------
MainWindow* DesignDiagram::getMainWindow() const
{
    return mainWnd_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DesignDiagram::getEditProvider()
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DesignDiagram::getEditedComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DesignDiagram::getDesignConfiguration() const
{
    return designConf_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceAdded(ComponentItem* item)
{
    instanceNames_.append(item->name());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    instanceNames_.removeAll(item->name());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QSharedPointer<Component> component)
{
    QString instanceName = component->getVlnv()->getName();
    instanceName.remove(QString(".comp"));
    return createInstanceName(instanceName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QString const& baseName)
{
    // Determine a unique name by using a running number.
    int runningNumber = 0;
    QString name = baseName + "_" + QString::number(runningNumber);

    while (instanceNames_.contains(name))
    {
        ++runningNumber;
        name = baseName + "_" + QString::number(runningNumber);
    }

    instanceNames_.append(name);
    return name;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::drawBackground()
//-----------------------------------------------------------------------------
void DesignDiagram::drawBackground(QPainter* painter, QRectF const& rect)
{
    painter->setWorldMatrixEnabled(true);
    painter->setBrush(Qt::lightGray);

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize ) {
        for (qreal y = top; y < rect.bottom(); y += GridSize )
            painter->drawPoint(x, y);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onShow()
//-----------------------------------------------------------------------------
void DesignDiagram::onShow()
{
    // Retrieve the new selection.
    QGraphicsItem *newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    onSelected(newSelection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setVisibilityControlState()
//-----------------------------------------------------------------------------
void DesignDiagram::setVisibilityControlState(QString const& name, bool state)
{
    // Empty default implementation.
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getInstances()
//-----------------------------------------------------------------------------
QList<ComponentItem*> DesignDiagram::getInstances() const
{
    // the list to store the diagram components to
    QList<ComponentItem*> instances;

    // ask for all graphics items.
    QList<QGraphicsItem*> graphItems = items();
    foreach (QGraphicsItem* graphItem, graphItems) {

        // make dynamic type conversion
        ComponentItem* diagComp = dynamic_cast<ComponentItem*>(graphItem);
        // if the item was a diagram component then add it to the list.
        if (diagComp) 
            instances.append(diagComp);
    }
    return instances;
}