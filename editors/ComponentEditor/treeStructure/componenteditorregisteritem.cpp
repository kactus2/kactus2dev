/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorregisteritem.h"
#include <editors/ComponentEditor/memoryMaps/registereditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include "componenteditorfielditem.h"
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
														 ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
                                                         QSharedPointer<ParameterFinder> parameterFinder,
                                                         QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                         QSharedPointer<ReferenceCounter> referenceCounter,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
fields_(reg->getFields()),
visualizer_(NULL),
graphItem_(NULL)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorRegisterItem"));

	foreach(QSharedPointer<Field> field, fields_)
    {
		if (field)
        {
			QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
				reg, field, model, libHandler, component, this));
			childItems_.append(fieldItem);
		}
	}

	Q_ASSERT(reg_);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::~ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::~ComponentEditorRegisterItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getToolTip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::getTooltip() const
{
	return tr("Contains details of a single register within an address block.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::text() const
{
	return reg_->getName();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterItem::isValid() const
{
	return reg_->isValid(component_->getChoices());
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterItem::editor()
{
	if (!editor_)
    {
		editor_ = new RegisterEditor(reg_, component_, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createChild( int index )
{
	QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
		reg_, fields_.at(index), model_, libHandler_, component_, this));
	fieldItem->setLocked(locked_);
	
	if (visualizer_)
    {
		fieldItem->setVisualizer(visualizer_);
	}
    updateGraphics();

	childItems_.insert(index, fieldItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onEditorChanged()
{
	// on register also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());

		// on register also the grand grand parent must be updated
		if (parent()->parent()->parent())
        {
			emit contentChanged(parent()->parent()->parent());
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new RegisterGraphItem(reg_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	// update the visualizers for field items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
		QSharedPointer<ComponentEditorFieldItem> fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);
	}

    updateGraphics();

	connect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::updateGraphics()
{
	if (graphItem_)
    {
		graphItem_->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItem()
{
	if (graphItem_)
    {
		// get the graphics item for the memory map
		MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(NULL);

		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;

		// tell the parent to refresh itself
		parentItem->refresh();
	}
}
