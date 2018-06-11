#include "componenteditorregisterfileitem.h"
#include "componenteditorregisteritem.h"
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registerfilegraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <QApplication>
ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem(QSharedPointer<RegisterFile> registerFile,
                                                           ComponentEditorTreeModel* model,
                                                           LibraryInterface* libHandler,
                                                           QSharedPointer<Component> component,
                                                           QSharedPointer<ParameterFinder> parameterFinder,
                                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                           QSharedPointer<ReferenceCounter> referenceCounter,
                                                           QSharedPointer<ExpressionParser> expressionParser,
                                                           QSharedPointer<RegisterFileValidator> registerFileValidator,
                                                           ComponentEditorItem* parent):
    ComponentEditorItem( model, libHandler, component, parent),
    registerFile_(registerFile),
    visualizer_(NULL),
    registerFileDimensions_(),
    expressionParser_(expressionParser),
    registerFileValidator_(registerFileValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setObjectName(tr("ComponentEditorRegFileItem"));

    foreach (QSharedPointer<RegisterBase> regModel, *registerFile->getRegisterData())
    {
        QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
        if (reg)
        {
            QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg, model,
                libHandler, component, parameterFinder_, expressionFormatter_, referenceCounter_,
                expressionParser_, registerFileValidator_->getRegisterValidator(), this));
            childItems_.append(regItem);
            continue;
        }

        QSharedPointer<RegisterFile> regfile = regModel.dynamicCast<RegisterFile>();
        if(regfile){
            QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(regfile, model,
                  libHandler, component, parameterFinder_, expressionFormatter_, referenceCounter_,
                  expressionParser_, registerFileValidator_, this));
            childItems_.append(regFileItem);
            continue;
        }
    }

}



QString ComponentEditorRegisterFileItem::getTooltip() const {
    return tr("Contains details of a single register file within an address block or another register file");
}

QString ComponentEditorRegisterFileItem::text() const {
    return registerFile_->name();
}



//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterFileItem::isValid() const
{
    return registerFileValidator_->validate(registerFile_);
}


class dummy_editor: public ItemEditor{
  public:
    dummy_editor(QSharedPointer<Component> component,
                 LibraryInterface* handler,
                 QWidget *parent = 0) : ItemEditor(component, handler,parent){}
    void refresh() {}
};

ItemEditor* ComponentEditorRegisterFileItem::editor(){
    if(!editor_){
        editor_ = new dummy_editor(component_, libHandler_);
    }
    return editor_;
}


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
    resizeGraphicsToCurrentDimensionSize();

    // update the visualizers for field items
    foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
        QSharedPointer<ComponentEditorRegisterItem> regItem = item.dynamicCast<ComponentEditorRegisterItem>();
        if (regItem){
          regItem->setVisualizer(visualizer_);
          continue;
        }
        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem = item.dynamicCast<ComponentEditorRegisterFileItem>();
        if (regFileItem){
          regFileItem->setVisualizer(visualizer_);
        }

    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterFileItem::getGraphicsItem()
{
    if (!registerFileDimensions_.isEmpty())
    {
        return registerFileDimensions_.first();
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::updateGraphics()
{
    resizeGraphicsToCurrentDimensionSize();
    foreach (RegisterFileGraphItem* registerFileDimension, registerFileDimensions_)
    {
        registerFileDimension->refresh();
    }
}

void ComponentEditorRegisterFileItem::onGraphicsChanged()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ComponentEditorItem::onGraphicsChanged();

    parent()->updateGraphics();//addressBlock
    parent()->parent()->updateGraphics(); //Memory Map
    QApplication::restoreOverrideCursor();
}


//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeGraphicsItem()
{
    // get the graphics item for the address block.
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    foreach (RegisterFileGraphItem* registerFileDimension, registerFileDimensions_)
    {
        parentItem->removeChild(registerFileDimension);
        registerFileDimension->setParent(0);

        disconnect(registerFileDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        registerFileDimensions_.removeAll(registerFileDimension);

        delete registerFileDimension;
        registerFileDimension = NULL;
    }
}


//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::resizeToCurrentDimensionSize()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::resizeGraphicsToCurrentDimensionSize()
{
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());

    if(!parentItem)
    {
        return;
    }

    const int DIMENSION_SIZE = qMax(expressionParser_->parseExpression(registerFile_->getDimension()).toInt(), 1);
    for (int currentDimension = registerFileDimensions_.count(); currentDimension < DIMENSION_SIZE; currentDimension++)
    {
        createDimensionGraphicsItem(currentDimension, parentItem);
    }

    for (int currentDimension = registerFileDimensions_.count() - 1; currentDimension >= DIMENSION_SIZE;
        currentDimension--)
    {
        removeDimensionGraphicsItem(currentDimension, parentItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::createDimensionGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createDimensionGraphicsItem(int dimensionIndex,
    MemoryVisualizationItem* parentItem)
{
    RegisterFileGraphItem* newDimension = new RegisterFileGraphItem(registerFile_, expressionParser_, parentItem);
    newDimension->setDimensionIndex(dimensionIndex);

    parentItem->addChild(newDimension);
    registerFileDimensions_.append(newDimension);

    connect(newDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::removeDimensionGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeDimensionGraphicsItem(int dimensionIndex,
    MemoryVisualizationItem* parentItem)
{
    RegisterFileGraphItem* removedDimension = registerFileDimensions_.takeAt(dimensionIndex);
    parentItem->removeChild(removedDimension);
    removedDimension->setParent(0);

    disconnect(removedDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

    delete removedDimension;
    removedDimension = 0;
}
