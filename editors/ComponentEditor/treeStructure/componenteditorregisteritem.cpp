//-----------------------------------------------------------------------------
// File: componenteditorregisteritem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisteritem.h"
#include "componenteditorfielditem.h"

#include <editors/ComponentEditor/memoryMaps/SingleRegisterEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
expressionParser_(expressionParser),
registerValidator_(registerValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorRegisterItem"));

	for (QSharedPointer<Field> field : *reg_->getFields())
    {
        QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
            reg, field, model, libHandler, component, parameterFinder, referenceCounter, expressionParser_,
            expressionFormatter, registerValidator_->getFieldValidator(), this));
        childItems_.append(fieldItem);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getToolTip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::getTooltip() const
{
	return tr("Contains details of a single register.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::text() const
{
	return reg_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterItem::isValid() const
{
    return registerValidator_->validate(reg_);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleRegisterEditor(reg_, component_, libHandler_, parameterFinder_, expressionFormatter_,
            expressionParser_, registerValidator_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressInfoChanged(int)), this, SLOT(onChildAddressInfoChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

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
		reg_, reg_->getFields()->at(index), model_, libHandler_, component_, parameterFinder_, 
        referenceCounter_, expressionParser_, expressionFormatter_, registerValidator_->getFieldValidator(),
        this));
	fieldItem->setLocked(locked_);
	
	if (visualizer_)
    {
		fieldItem->setVisualizer(visualizer_);

        MemoryVisualizationItem* childItem = static_cast<MemoryVisualizationItem*>(fieldItem->getGraphicsItem());
        Q_ASSERT(childItem);

        registerItem_->addChild(childItem);
        registerItem_->redoChildLayout();

        connect(fieldItem.data(), SIGNAL(addressInfoChanged()), this, SLOT(onChildAddressInfoChanged()));
	}

  //  connect(fieldItem.data(), SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);

	childItems_.insert(index, fieldItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeChild(int index)
{
    QSharedPointer<ComponentEditorFieldItem> fieldItem = childItems_.at(index).dynamicCast<ComponentEditorFieldItem>();

    MemoryVisualizationItem* childItem = static_cast<MemoryVisualizationItem*>(fieldItem->getGraphicsItem());
    Q_ASSERT(childItem);

    registerItem_->removeChild(childItem);
    registerItem_->redoChildLayout();

    ComponentEditorItem::removeChild(index);
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
void ComponentEditorRegisterItem::setVisualizer(MemoryMapsVisualizer* visualizer)
{
	visualizer_ = visualizer;
    //connect(visualizer_, SIGNAL(displayed()), this, SLOT(updateGraphics()), Qt::UniqueConnection);

    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    registerItem_ = new RegisterGraphItem(reg_, expressionParser_, parentItem);
    parentItem->addChild(registerItem_);
    
    connect(registerItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);

	// update the visualizers for field items
	for (auto& item : childItems_)
    {
		QSharedPointer<ComponentEditorFieldItem> fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);

        MemoryVisualizationItem* childItem = static_cast<MemoryVisualizationItem*>(fieldItem->getGraphicsItem());
        Q_ASSERT(childItem);

        registerItem_->addChild(childItem);

        connect(item.data(), SIGNAL(addressInfoChanged()), this, SLOT(onChildAddressInfoChanged()));
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterItem::getGraphicsItem()
{
    return registerItem_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::updateGraphics()
{
	if (registerItem_ != nullptr)
    {
        registerItem_->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onGraphicsChanged()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (registerItem_ != nullptr)
    {
        registerItem_->updateDisplay();
    }

    //ComponentEditorItem::onGraphicsChanged();

    //parent()->updateGraphics();
    //parent()->parent()->updateGraphics();
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildAddressInfoChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildAddressInfoChanged()
{
    if (registerItem_ != nullptr)
    {
        registerItem_->redoChildLayout();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItem()
{
    // get the graphics item for the address block.
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    parentItem->removeChild(registerItem_);
    registerItem_->setParent(0);

    disconnect(registerItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

    delete registerItem_;
    registerItem_ = 0;
}
