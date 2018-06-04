#include "componenteditorregisterfileitem.h"
#include "componenteditorregisteritem.h"
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <editors/ComponentEditor/itemeditor.h>
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
    //visualizer_(NULL),
    //graphItem_(NULL),
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

