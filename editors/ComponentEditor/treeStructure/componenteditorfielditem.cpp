/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfielditem.h"

#include <editors/ComponentEditor/memoryMaps/SingleFieldEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::ComponentEditorFieldItem()
//-----------------------------------------------------------------------------
ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Register> reg,
												   QSharedPointer<Field> field,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
                                                   QSharedPointer<ParameterFinder> parameterFinder,
                                                   QSharedPointer<ReferenceCounter> referenceCounter,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
field_(field),
visualizer_(NULL),
graphItem_(NULL)
{
	Q_ASSERT(field_);

    setParameterFinder(parameterFinder);
    setReferenceCounter(referenceCounter);

	setObjectName(tr("ComponentEditorFieldItem"));
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::~ComponentEditorFieldItem()
//-----------------------------------------------------------------------------
ComponentEditorFieldItem::~ComponentEditorFieldItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::getTooltip() const
{
	return tr("Contains details of a single field within a register.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::text() const
{
	return field_->getName();
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFieldItem::isValid() const 
{
	return field_->isValid(reg_->getSize(), component_->getChoices());
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFieldItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleFieldEditor(field_, component_, libHandler_, parameterFinder_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::onEditorChanged()
{
	// on field also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());

		// on field also the grand grand parent must be updated
		if (parent()->parent()->parent())
        {
			emit contentChanged(parent()->parent()->parent());

			// on field also the grand grand grand parent must be updated
			if (parent()->parent()->parent()->parent())
            {
				emit contentChanged(parent()->parent()->parent()->parent());
			}
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorFieldItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new FieldGraphItem(field_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	connect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorFieldItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::updateGraphics()
{
	if (graphItem_)
    {
		graphItem_->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::removeGraphicsItem()
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

		disconnect(graphItem_, SIGNAL(selectEditor()),
			this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;

		// tell the parent to refresh itself
		parentItem->refresh();
	}
}
