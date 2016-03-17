//-----------------------------------------------------------------------------
// File: ComponentInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The component instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationsItem.h"
#include "SingleComponentInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/ComponentInstantiationsEditor.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
ComponentInstantiationsItem::ComponentInstantiationsItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<InstantiationsValidator> validator,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    expressionParser_(expressionParser),
    validator_(validator)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("ComponentInstantiationsItem"));

    setReferenceCounter(referenceCounter);

    foreach(QSharedPointer<ComponentInstantiation> instantiation, *component->getComponentInstantiations())
    {
        QSharedPointer<ComponentEditorItem> componentInstantiationItem (new SingleComponentInstantiationItem(
            model, libHandler, component, instantiation, validator, referenceCounter, parameterFinder,
            expressionFormatter, expressionParser, this));

        childItems_.append(componentInstantiationItem);

        connect(componentInstantiationItem.data(), SIGNAL(openReferenceTree(QString)),
            this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::~ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
ComponentInstantiationsItem::~ComponentInstantiationsItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentInstantiationsItem::getTooltip() const
{
    return tr("Contains the component instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::text()
//-----------------------------------------------------------------------------
QString ComponentInstantiationsItem::text() const
{
	return tr("Component instantiations");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentInstantiationsItem::editor()
{
	if (!editor_)
    {
        editor_ = new ComponentInstantiationsEditor(component_, libHandler_, parameterFinder_, validator_, 0);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void ComponentInstantiationsItem::createChild( int index )
{
    QSharedPointer<ComponentInstantiation> instantiation = component_->getComponentInstantiations()->at(index);
    
    QSharedPointer<ComponentEditorItem> childItem(
        new SingleComponentInstantiationItem(model_, libHandler_, component_, instantiation, validator_,
        referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, this));

    childItem->setLocked(locked_);

    connect(childItem.data(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    childItems_.append(childItem);
}
