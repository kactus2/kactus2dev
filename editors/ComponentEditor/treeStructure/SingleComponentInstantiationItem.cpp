//-----------------------------------------------------------------------------
// File: ComponentInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "SingleComponentInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/ComponentInstantiationEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
SingleComponentInstantiationItem::SingleComponentInstantiationItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<ComponentInstantiation> instantiation,
    QSharedPointer<InstantiationsValidator> validator,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    instantiation_(instantiation),
    validator_(validator),
    expressionParser_(expressionParser)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("ComponentInstantiationsItem"));

    setReferenceCounter(referenceCounter);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::~ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
SingleComponentInstantiationItem::~SingleComponentInstantiationItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleComponentInstantiationItem::getTooltip() const
{
    return tr("Contains the component instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::text()
//-----------------------------------------------------------------------------
QString SingleComponentInstantiationItem::text() const
{
	return instantiation_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleComponentInstantiationItem::isValid()
//-----------------------------------------------------------------------------
bool SingleComponentInstantiationItem::isValid() const
{
    return validator_->validateComponentInstantiation(instantiation_);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleComponentInstantiationItem::editor()
{
	if (!editor_)
    {
 		editor_ = new ComponentInstantiationEditor(component_, libHandler_, instantiation_, parameterFinder_,
            expressionParser_, expressionFormatter_, 0);
 		editor_->setProtection(locked_);
 
 		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
 		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
 		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
 		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
        connectReferenceTree();
	}
	return editor_;
}
