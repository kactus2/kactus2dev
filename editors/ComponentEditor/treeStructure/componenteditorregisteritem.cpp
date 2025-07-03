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

#include <KactusAPI/include/ExpressionParser.h>

#include <editors/ComponentEditor/memoryMaps/SingleRegisterEditor.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> containingRegisterData, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator, RegisterInterface* registerInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
expressionParser_(expressionParser),
registerValidator_(registerValidator),
registerInterface_(registerInterface),
containingRegisterData_(containingRegisterData)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorRegisterItem"));

    const int fieldCount = reg_->getFields()->count();

	for (int i = 0; i < fieldCount; ++i)
    {
        createChild(i);
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
        editor_ = new SingleRegisterEditor(reg_, component_, libHandler_, parameterFinder_, expressionParser_,
            registerInterface_, containingRegisterData_);
        editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)), this, SLOT(onChildAddressingChangedLocally(int)), Qt::UniqueConnection);
        
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(fieldNameChanged(QString const&, QString const&)),
            this, SIGNAL(fieldNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
            editor_, SLOT(onRegisterNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

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
        registerInterface_->getSubInterface(), this));
	fieldItem->setLocked(locked_);
	
	if (visualizer_)
    {
		fieldItem->setVisualizer(visualizer_);

        auto childItems = fieldItem->getGraphicsItems();
        
        for (auto const& item : childItems)
        {
            registerItem_->addChild(item);
            connect(this, SIGNAL(fieldNameChanged(QString const&, QString const&)),
                fieldItem.data(), SIGNAL(fieldNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

            connect(fieldItem.data(), SIGNAL(addressingChanged()), this, SLOT(onChildAddressingChanged()), Qt::UniqueConnection);
        }

        registerItem_->redoChildLayout();
	}

	childItems_.insert(index, fieldItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeChild(int index)
{
    // Clear field visualizers for removed field
    auto fieldItem = childItems_.at(index).dynamicCast<ComponentEditorFieldItem>();

    fieldItem->removeGraphicsItems();

    // Then remove field
    childItems_.removeAt(index);
    registerItem_->redoChildLayout();
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

    auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    registerItem_ = new RegisterGraphItem(reg_, expressionParser_, parentItem);
    connect(registerItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);

	// update the visualizers for field items
	for (auto const& item : childItems_)
    {
		auto fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);

        for (auto const& child : fieldItem->getGraphicsItems())
        {
            registerItem_->addChild(child);
        }

        connect(item.data(), SIGNAL(addressingChanged()), this, SLOT(onChildAddressingChanged()));
	}

    registerItem_->redoChildLayout();
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
    onGraphicsChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onGraphicsChanged()
{
    if (registerItem_ != nullptr)
    {
        registerItem_->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onAddressingChanged()
{
    if (registerItem_ != nullptr)
    {
        registerItem_->redoChildLayout();

        emit addressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildAddressingChanged()
{
    if (registerItem_ != nullptr)
    {   
        if (auto editorFieldItem = dynamic_cast<ComponentEditorFieldItem*>(sender()))
        {
            // Delete old graph items and create new
            editorFieldItem->removeGraphicsItems();
            editorFieldItem->createGraphicsItems();

            // Re-add field graph items to register
            for (auto const& item : editorFieldItem->getGraphicsItems())
            {
                registerItem_->addChild(item);
            }
        }

        registerItem_->redoChildLayout();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onChildAddressingChangedLocally()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onChildAddressingChangedLocally(int childIndex)
{
    // Signal from fields table editor (RegisterEditor) is handled here.

    Q_ASSERT(childIndex < childItems_.size());
    auto targetItem = childItems_.at(childIndex).dynamicCast<ComponentEditorFieldItem>();

    if (!targetItem || !registerItem_)
    {
        return;
    }

    // Delete old graph items and create new
    targetItem->removeGraphicsItems();
    targetItem->createGraphicsItems();

    // Re-add field graph items to register
    for (auto const& item : targetItem->getGraphicsItems())
    {
        registerItem_->addChild(item);
    }

    registerItem_->redoChildLayout();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItem()
{
    if (registerItem_)
    {
        // get the graphics item for the address block.
        auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
        Q_ASSERT(parentItem);

        registerItem_->setParent(nullptr);

        disconnect(registerItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        delete registerItem_;
        registerItem_ = nullptr;
    }
}
