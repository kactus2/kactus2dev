//-----------------------------------------------------------------------------
// File: componenteditorfielditem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The item for single field in component editor's navigation tree
//-----------------------------------------------------------------------------

#include "componenteditorfielditem.h"

#include <editors/ComponentEditor/memoryMaps/SingleFieldEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
    
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::ComponentEditorFieldItem()
//-----------------------------------------------------------------------------
ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Register> reg, QSharedPointer<Field> field,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> formatter,
    QSharedPointer<FieldValidator> fieldValidator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
field_(field),
expressionParser_(expressionParser),
fieldValidator_(fieldValidator)
{
	Q_ASSERT(field_);

    setParameterFinder(parameterFinder);
    setReferenceCounter(referenceCounter);
    setExpressionFormatter(formatter);

	setObjectName(tr("ComponentEditorFieldItem"));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::getTooltip() const
{
	return tr("Contains details of a single field within a register.");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::text() const
{
	return field_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFieldItem::isValid() const 
{
    return fieldValidator_->validate(field_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFieldItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleFieldEditor(field_, component_, libHandler_, parameterFinder_, expressionParser_,
            expressionFormatter_, fieldValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressInfoChanged()), this, SIGNAL(addressInfoChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorFieldItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	graphItem_ = new FieldGraphItem(field_, expressionParser_, nullptr);

	connect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorFieldItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::removeGraphicsItem()
{
	if (graphItem_)
    {
		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::updateGraphics()
{
    onGraphicsChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::onEditorChanged()
{
    emit contentChanged(this);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFieldItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::onGraphicsChanged()
{
    if (graphItem_)
    {
        graphItem_->updateDisplay();
    }
}
