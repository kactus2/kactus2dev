//-----------------------------------------------------------------------------
// File: componenteditorregisterfileitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Item representing a register file in the component editor browser tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisterfileitem.h"

#include "componenteditorregisteritem.h"

#include <editors/ComponentEditor/memoryMaps/SingleRegisterFileEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registerfilegraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem(QSharedPointer<RegisterFile> registerFile,
    ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    ComponentEditorItem* parent) :
    ComponentEditorItem(model, libHandler, component, parent),
    registerFile_(registerFile),
    
    expressionParser_(expressionParser),
    registerFileValidator_(registerFileValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setObjectName(tr("ComponentEditorRegFileItem"));

    const int registerCount = registerFile_->getRegisterData()->count();
    for (int i = 0; i < registerCount; ++i)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::getTooltip() const
{
    return tr("Contains details of a single register file.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::text() const
{
    return registerFile_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterFileItem::isValid() const
{
    return registerFileValidator_->validate(registerFile_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createChild( int index )
{
    QSharedPointer<RegisterBase> regmodel = registerFile_->getRegisterData()->at(index);

    QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
    if (reg)
    {
        QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg, model_,
            libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_,
            expressionParser_, registerFileValidator_->getRegisterValidator(), this));
        regItem->setLocked(locked_);


        if (reg->getFields()->isEmpty())
        {
            QSharedPointer<Field> newField (new Field());
            reg->getFields()->append(newField);

            regItem->createChild(0);
        }
        
        if (visualizer_)
        {
            regItem->setVisualizer(visualizer_);
        }

        connect(regItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        childItems_.insert(index, regItem);
    }

    QSharedPointer<RegisterFile> regFile = regmodel.dynamicCast<RegisterFile>();
    if (regFile)
    {
        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(regFile, model_,
            libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_,
            expressionParser_, registerFileValidator_, this));
        regFileItem->setLocked(locked_);
     
        if (visualizer_)
        {
            regFileItem->setVisualizer(visualizer_);
        }

        connect(regFileItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        childItems_.insert(index, regFileItem);
    }

    if (visualizer_)
    {
        auto childItem = static_cast<MemoryVisualizationItem*>(childItems_.at(index)->getGraphicsItem());
        Q_ASSERT(childItem);

        registerFileItem_->addChild(childItem);
        onAddressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeChild(int index)
{
    if (visualizer_)
    {
        auto childItem = static_cast<MemoryVisualizationItem*>(childItems_.at(index)->getGraphicsItem());
        Q_ASSERT(childItem);

        registerFileItem_->removeChild(childItem);
        onAddressingChanged();
    }

    ComponentEditorItem::removeChild(index);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterFileItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleRegisterFileEditor(registerFile_, component_, libHandler_, parameterFinder_, 
            expressionFormatter_, expressionParser_, registerFileValidator_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childGraphicsChanged(int)), this, SLOT(onChildGraphicsChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)),
            this, SLOT(onChildAddressingChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterFileItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
    visualizer_ = visualizer;

    auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    registerFileItem_ = new RegisterFileGraphItem(registerFile_, expressionParser_, parentItem);    

    connect(registerFileItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);

    // update the visualizers for field items
    for (QSharedPointer<ComponentEditorItem> child : childItems_)
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
        registerFileItem_->addChild(childGraphicItem);
    }

    registerFileItem_->redoChildLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterFileItem::getGraphicsItem()
{
    return registerFileItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::updateGraphics()
{
    onGraphicsChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onGraphicsChanged()
{
    if (registerFileItem_ != nullptr)
    {
        registerFileItem_->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onChildGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onChildGraphicsChanged(int index)
{
    childItems_.at(index)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onAddressingChanged()
{
    if (registerFileItem_ != nullptr)
    {
        registerFileItem_->redoChildLayout();

        emit addressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onChildAddressingChanged(int index)
{
    if (registerFileItem_ != nullptr)
    {
        childItems_.at(index)->updateGraphics();

        auto childRegister = childItems_.at(index).dynamicCast<ComponentEditorRegisterItem>();
        auto childRegisterFile = childItems_.at(index).dynamicCast<ComponentEditorRegisterFileItem>(); 

        if (childRegister)
        {
            childRegister->onChildAddressingChanged();
        }
        else if (childRegisterFile)
        {
            childRegisterFile->registerFileItem_->redoChildLayout();
        }
    }

    onAddressingChanged();
}


//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeGraphicsItem()
{
    if (registerFileItem_)
    {
        // get the graphics item for the address block.
        auto parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
        Q_ASSERT(parentItem);

        //sparentItem->removeChild(registerFileItem_);
        registerFileItem_->setParent(nullptr);

        disconnect(registerFileItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        delete registerFileItem_;
        registerFileItem_ = nullptr;
    }
}
