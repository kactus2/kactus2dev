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

#include "DesignWidget.h"

#include <common/utils.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/StickyNote/StickyNote.h>
#include <designEditors/common/StickyNote/StickyNoteAddCommand.h>
#include <designEditors/HWDesign/AdHocEditor/AdHocEditor.h>
#include <designEditors/common/Association/AssociationAddCommand.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QGraphicsItem>
#include <QSharedPointer>


//-----------------------------------------------------------------------------
// Function: DesignDiagram::DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::DesignDiagram(LibraryInterface* lh,
                             GenericEditProvider& editProvider, DesignWidget* parent)
    : QGraphicsScene(parent),
      parent_(parent),
      lh_(lh),
      editProvider_(editProvider),
      component_(),
      designConf_(),
      layout_(new GraphicsColumnLayout(this)),
      mode_(MODE_SELECT),
      instanceNames_(),
      loading_(false),
      locked_(false),
	  XMLComments_(),
    vendorExtensions_(),
    interactionMode_(NORMAL),
    associationLine_(0)
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
	// save the XML header from the design
	XMLComments_ = design->getTopComments();

    vendorExtensions_ = design->getVendorExtensions();

    // Clear the edit provider.
    editProvider_.clear();

    // clear the previous design configuration
    designConf_.clear();

    // Deselect items.
    emit clearItemSelection();
    clearSelection();

    // Clear the scene.    
    clearLayout();
    clearScene();
    getParent()->clearRelatedVLNVs();
    getParent()->addRelatedVLNV(*component->getVlnv());

    // Set the new component and open the design.
    component_ = component;
    designConf_ = designConf;

    loading_ = true;
    loadDesign(design);
    loadStickyNotes();
    loading_ = false;

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

    setProtectionForStickyNotes();
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
    getParent()->addRelatedVLNV(*item->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    instanceNames_.removeAll(item->name());
    getParent()->removeRelatedVLNV(*item->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onVendorExtensionAdded()
//-----------------------------------------------------------------------------
void DesignDiagram::onVendorExtensionAdded(QSharedPointer<VendorExtension> extension)
{
    if (!vendorExtensions_.contains(extension))
    {
        vendorExtensions_.append(extension);
    }    
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onVendorExtensionRemoved()
//-----------------------------------------------------------------------------
void DesignDiagram::onVendorExtensionRemoved(QSharedPointer<VendorExtension> extension)
{
    vendorExtensions_.removeAll(extension);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onBeginAssociation()
//-----------------------------------------------------------------------------
void DesignDiagram::onBeginAssociation(Associable* startingPoint)
{
    setInteractionMode(ASSOCIATE);

    QPointF start = startingPoint->connectionPoint();
    associationLine_ = new QGraphicsLineItem(QLineF(start, start));
    addItem(associationLine_);
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
// Forms a unique identifier for a component instance
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QString const& baseName)
{
    QSettings settings; // this reads the application settings automatically
    QString format = settings.value("Policies/InstanceNames", "").toString();
    if (format == "")
    {
        format = "$ComponentName$_$InstanceNumber$";
    }

    // Determine a unique name by using a running number.
    int runningNumber = 0;
    
    QString name = format;
    Utils::replaceMagicWord(name, "ComponentName", baseName);
    Utils::replaceMagicWord(name, "InstanceNumber", QString::number(runningNumber));

    while (instanceNames_.contains(name))
    {
        ++runningNumber;
        
        name = format;
        Utils::replaceMagicWord(name, "ComponentName", baseName);
        Utils::replaceMagicWord(name, "InstanceNumber", QString::number(runningNumber));
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
    painter->setPen(QPen(Qt::black, 0));

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize ) {
        for (qreal y = top; y < rect.bottom(); y += GridSize )
            painter->drawPoint(x, y);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getTopmostComponent()
//-----------------------------------------------------------------------------
ComponentItem* DesignDiagram::getTopmostComponent(QPointF const& pos)
{
    QList<QGraphicsItem*> itemList = items(pos);

    foreach (QGraphicsItem* item, itemList)
    {
        ComponentItem* compItem = dynamic_cast<ComponentItem*>(item);

        if (compItem != 0)
        {
            return compItem;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getBaseItemOf()
//-----------------------------------------------------------------------------
QGraphicsItem* DesignDiagram::getBaseItemOf(QGraphicsItem* item) const
{
    QGraphicsItem* baseItem = item;

    while (baseItem != 0 && baseItem->parentItem() != 0 &&
        (baseItem->type() == QGraphicsTextItem::Type || baseItem->type() == QGraphicsPixmapItem::Type))
    {
        baseItem = baseItem->parentItem();
    }	

    return baseItem;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createNote()
//-----------------------------------------------------------------------------
void DesignDiagram::createNoteAt(QPointF const& position)
{
    QSharedPointer<Kactus2Group> noteExtension(new Kactus2Group("kactus2:note"));
    StickyNote* note = new StickyNote(noteExtension);
    note->setPos(position);

    connect(note, SIGNAL(beginAssociation(Associable*)), 
        this, SLOT(onBeginAssociation(Associable*)), Qt::UniqueConnection);

    QSharedPointer<StickyNoteAddCommand> cmd = createNoteAddCommand(note);
    cmd->redo(); 
    getEditProvider().addCommand(cmd);

    clearSelection();
    emit clearItemSelection();
    note->setSelected(true);
    onSelected(note);

    note->beginEditing();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createContextMenu()
//-----------------------------------------------------------------------------
QMenu* DesignDiagram::createContextMenu(QPointF const& /*pos*/){
	return 0;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::contextMenuEvent()
//-----------------------------------------------------------------------------
void DesignDiagram::contextMenuEvent(QGraphicsSceneContextMenuEvent* event){
	
	QMenu* menu = createContextMenu(event->scenePos());
	if ( menu != 0 )
	{
	    menu->exec(event->screenPos());
	    delete menu;
	}
	event->accept();
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
// Function: DesignDiagram::selectAll()
//-----------------------------------------------------------------------------
void DesignDiagram::selectAll()
{
    clearSelection();

    foreach (GraphicsColumn* column, getLayout()->getColumns())
    {
        column->setSelected(true);
    }
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void DesignDiagram::onVerticalScroll(qreal y)
{
    getLayout()->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setVisibilityControlState()
//-----------------------------------------------------------------------------
void DesignDiagram::setVisibilityControlState(QString const& name, bool state)
{
    if (name == tr("Sticky Notes"))
    { 
        foreach (QGraphicsItem* item, items())
        {       
            if (item->type() == StickyNote::Type || item->type() == Association::Type)
            {
                item->setVisible(state);
            }            
        }
    }
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

//-----------------------------------------------------------------------------
// Function: DesignDiagram::isLoading()
//-----------------------------------------------------------------------------
bool DesignDiagram::isLoading() const
{
    return loading_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getParent()
//-----------------------------------------------------------------------------
DesignWidget* DesignDiagram::getParent()
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> DesignDiagram::createDesign( VLNV const& vlnv ) const {
	QSharedPointer<Design> design(new Design(vlnv));
	design->setTopComments(XMLComments_);
    design->setVendorExtensions(vendorExtensions_);
	return design;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getCommonType()
//-----------------------------------------------------------------------------
int DesignDiagram::getCommonItemType(QList<QGraphicsItem*> const& items)
{
    if (items.empty())
    {
        return -1;
    }

    int type = items[0]->type();

    for (int i = 1; i < items.size(); ++i)
    {
        if (type != items[i]->type())
        {
            return -1;
        }
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::sortByX()
//-----------------------------------------------------------------------------
bool DesignDiagram::sortByX(QGraphicsItem* lhs, QGraphicsItem* rhs)
{
    return lhs->x() < rhs->x();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getLayout()
//-----------------------------------------------------------------------------
QSharedPointer<GraphicsColumnLayout> DesignDiagram::getLayout() const
{
    return layout_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setLayout()
//-----------------------------------------------------------------------------
void DesignDiagram::clearLayout()
{
    layout_.clear();
    layout_ = QSharedPointer<GraphicsColumnLayout>(new GraphicsColumnLayout(this));
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::loadStickyNotes()
//-----------------------------------------------------------------------------
void DesignDiagram::loadStickyNotes()
{
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
    {
        if (extension->type() == "kactus2:note")
        {
            QSharedPointer<Kactus2Group> noteExtension = extension.dynamicCast<Kactus2Group>();
            StickyNote* note = new StickyNote(noteExtension);

            connect(note, SIGNAL(beginAssociation(Associable*)), 
                this, SLOT(onBeginAssociation(Associable*)), Qt::UniqueConnection);

            QSharedPointer<StickyNoteAddCommand> cmd = createNoteAddCommand(note);
            cmd->redo();
            
            QList<QSharedPointer<VendorExtension> > associations = noteExtension->getByType("kactus2:associations");
            if (!associations.empty())
            {
                loadNoteAssociations(note, associations.first());
            }            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::loadNoteAssociations()
//-----------------------------------------------------------------------------
void DesignDiagram::loadNoteAssociations(StickyNote* note, QSharedPointer<VendorExtension> associationsExtension)
{
    QSharedPointer<Kactus2Group> associationGroup = associationsExtension.dynamicCast<Kactus2Group>();

    foreach(QSharedPointer<VendorExtension> endpoint, associationGroup->getByType("kactus2:position"))
    {
        QSharedPointer<Kactus2Position> endpointExtension = endpoint.dynamicCast<Kactus2Position>();

        QSharedPointer<QUndoCommand> addCommand = createAssociationAddCommand(note, endpointExtension);
        addCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setProtectionForStickyNotes()
//-----------------------------------------------------------------------------
void DesignDiagram::setProtectionForStickyNotes()
{
    foreach(QGraphicsItem* item, items())
    {
        if (item->type() == StickyNote::Type)
        {
            item->setEnabled(!locked_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::updateAssociationLine()
//-----------------------------------------------------------------------------
void DesignDiagram::updateAssociation(QPointF const& cursorPosition)
{
    QLineF line = associationLine_->line();
    line.setP2(cursorPosition);
    associationLine_->setLine(line);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::endAssociation()
//-----------------------------------------------------------------------------
void DesignDiagram::endAssociation(QPointF const& endpoint)
{
    QPointF startingPoint = associationLine_->line().p1();

    endAssociationLineDraw();

    QGraphicsItem* item = getBaseItemOf(itemAt(startingPoint, QTransform()));
    Associable* startItem = dynamic_cast<Associable*>(item);

    QSharedPointer<Kactus2Position> endPositionExtension(new Kactus2Position(endpoint));

    QSharedPointer<QUndoCommand> addCommand = createAssociationAddCommand(startItem, endPositionExtension);
    addCommand->redo();
    getEditProvider().addCommand(addCommand);

    setInteractionMode(NORMAL);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createAssociation()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> DesignDiagram::createAssociationAddCommand(Associable* startItem, 
    QSharedPointer<Kactus2Position> endPointExtension)
{
    QGraphicsItem* item = getBaseItemOf(itemAt(endPointExtension->position(), QTransform()));
    Associable* endItem = dynamic_cast<Associable*>(item);

    QSharedPointer<QUndoCommand> addCommand(new QUndoCommand());

    if (canAssociateItems(startItem, endItem))
    {
        Association* association = new Association(startItem, endItem, endPointExtension);
        addCommand = QSharedPointer<AssociationAddCommand>(new AssociationAddCommand(association, this));       
    }

    return addCommand;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::canAssociateItems()
//-----------------------------------------------------------------------------
bool DesignDiagram::canAssociateItems(Associable* startItem, Associable* endItem)
{
    return startItem && endItem && startItem != endItem;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::inAssociationMode()
//-----------------------------------------------------------------------------
bool DesignDiagram::inAssociationMode() const
{
    return interactionMode_ == ASSOCIATE;
}

//-----------------------------------------------------------------------------
// Function: HWDesignDiagram::associationEnds()
//-----------------------------------------------------------------------------
bool DesignDiagram::associationEnds() const
{
    return inAssociationMode() && associationLine_->line().p1() != associationLine_->line().p2();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::contextMenuEnabled()
//-----------------------------------------------------------------------------
bool DesignDiagram::contextMenuEnabled() const
{
    return getMode() == MODE_SELECT && interactionMode_ == CONTEXT_MENU;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::inReplaceMode()
//-----------------------------------------------------------------------------
bool DesignDiagram::inReplaceMode() const
{
    return interactionMode_ == REPLACE;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setInteractionMode()
//-----------------------------------------------------------------------------
void DesignDiagram::setInteractionMode(interactionMode mode)
{
    interactionMode_ = mode;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createNoteAddCommand()
//-----------------------------------------------------------------------------
QSharedPointer<StickyNoteAddCommand> DesignDiagram::createNoteAddCommand(StickyNote* note)
{
    QSharedPointer<StickyNoteAddCommand> cmd(new StickyNoteAddCommand(note, this, note->pos()));

    connect(cmd.data(), SIGNAL(addVendorExtension(QSharedPointer<VendorExtension>)),
        this, SLOT(onVendorExtensionAdded(QSharedPointer<VendorExtension>)), Qt::UniqueConnection);

    connect(cmd.data(), SIGNAL(removeVendorExtension(QSharedPointer<VendorExtension>)),
        this, SLOT(onVendorExtensionRemoved(QSharedPointer<VendorExtension>)), Qt::UniqueConnection);

    return cmd;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::wheelEvent()
//-----------------------------------------------------------------------------
void DesignDiagram::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        // Set the zoom level and center the view.
        getParent()->setZoomLevel(getParent()->getZoomLevel() + event->delta() / 12);

        // Retrieve the center point in diagram coordinates.
        QPointF centerPos = itemsBoundingRect().center();
        getParent()->centerViewTo(centerPos);        

        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::endInteraction()
//-----------------------------------------------------------------------------
void DesignDiagram::endInteraction()
{
    if (inAssociationMode())
    {
        endAssociationLineDraw();
    }

    setInteractionMode(NORMAL);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::endAssociationLineDraw()
//-----------------------------------------------------------------------------
void DesignDiagram::endAssociationLineDraw()
{
    removeItem(associationLine_);
    delete associationLine_;
    associationLine_ = 0;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::inOffPageMode()
//-----------------------------------------------------------------------------
bool DesignDiagram::inOffPageMode() const
{
    return interactionMode_ == OFFPAGE;
}



