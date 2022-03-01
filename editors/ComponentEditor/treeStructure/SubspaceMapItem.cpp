//-----------------------------------------------------------------------------
// File: SubspaceMapItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.02.2022
//
// Description:
// The item for a single subspace map in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "SubspaceMapItem.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/SingleSubspaceMapEditor.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::SubspaceMapItem()
//-----------------------------------------------------------------------------
SubspaceMapItem::SubspaceMapItem(QSharedPointer<MemoryMapBase> containingMap,
    QSharedPointer<SubSpaceMap> subspace, ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<SubspaceMapValidator> subspaceValidator,
    SubspaceMapInterface* subspaceInterface, ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
containingMap_(containingMap),
subspaceMap_(subspace),
visualizer_(NULL),
// graphItem_(NULL),
expressionParser_(expressionParser),
addressUnitBits_(0),
subspaceValidator_(subspaceValidator),
subspaceInterface_(subspaceInterface)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("SubspaceItem"));
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::getTooltip()
//-----------------------------------------------------------------------------
QString SubspaceMapItem::getTooltip() const
{
	return tr("Contains details of a single subspace map.");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::text()
//-----------------------------------------------------------------------------
QString SubspaceMapItem::text() const
{
	return subspaceMap_->name();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::isValid()
//-----------------------------------------------------------------------------
bool SubspaceMapItem::isValid() const 
{
    return subspaceValidator_->validate(subspaceMap_);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SubspaceMapItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleSubspaceMapEditor(subspaceInterface_, subspaceMap_, containingMap_, component_, libHandler_, parameterFinder_, expressionFormatter_, expressionParser_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        
        connect(this, SIGNAL(subspaceNameChanged(QString const&, QString const&)),
            editor_, SLOT(onSubspaceNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void SubspaceMapItem::onGraphicsChanged()
{
/*
    if (graphItem_)
    {
        graphItem_->updateDisplay();
    }
*/
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void SubspaceMapItem::onAddressingChanged()
{
/*
    if (graphItem_ != nullptr)
    {
        graphItem_->redoChildLayout();

        for (auto child : childItems_)
        {
            child->updateGraphics();
        }

        emit addressingChanged();
    }
*/
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* SubspaceMapItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::setVisualizer()
//-----------------------------------------------------------------------------
void SubspaceMapItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
/*
	graphItem_ = new AddressBlockGraphItem(addrBlock_, expressionParser_, parentItem);
    graphItem_->setAddressableUnitBits(addressUnitBits_);

	// update the visualizers for register items
	for (auto child : childItems_)
    {        
        auto regItem = child.dynamicCast<ComponentEditorRegisterItem>();
        auto regFileItem = child.dynamicCast<ComponentEditorRegisterFileItem>();

        if (regItem)
        {
          regItem->setVisualizer(visualizer_);
        }
        else if (regFileItem)
        {
          regFileItem->setVisualizer(visualizer_);
        }
   
        auto childGraphicItem = static_cast<MemoryVisualizationItem*>(child->getGraphicsItem());
        graphItem_->addChild(childGraphicItem);
	}

    graphItem_->redoChildLayout();

	connect(graphItem_, SIGNAL(selectEditor()),	this, SLOT(onSelectRequest()), Qt::UniqueConnection);
*/
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::updateGraphics()
//-----------------------------------------------------------------------------
void SubspaceMapItem::updateGraphics()
{
/*
	if (graphItem_)
    {
		graphItem_->updateDisplay();
	}
*/
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void SubspaceMapItem::removeGraphicsItem()
{
/*
	if (graphItem_)
    {
		// get the graphics item for the memory map
		auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(nullptr);

		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = nullptr;
	}
*/
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapItem::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void SubspaceMapItem::addressUnitBitsChanged(int newAddressUnitBits)
{
    addressUnitBits_ = newAddressUnitBits;

/*
    if (graphItem_)
    {
        graphItem_->setAddressableUnitBits(newAddressUnitBits);
        graphItem_->updateDisplay();
        graphItem_->redoChildLayout();
    }

    emit changeInAddressUnitBits(newAddressUnitBits);
*/
}
