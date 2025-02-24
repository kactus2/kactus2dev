//-----------------------------------------------------------------------------
// File: ComponentInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The component instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationsItem.h"
#include "SingleComponentInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/ComponentInstantiationsEditor.h>
#include <KactusAPI/include/ComponentInstantiationParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
ComponentInstantiationsItem::ComponentInstantiationsItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<InstantiationsValidator> validator, QSharedPointer<AllInstantiationsValidator> allInstantiationsValidator, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, ComponentInstantiationInterface* instantiationInterface,
    ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
validator_(validator),
allInstantiationsValidator_(allInstantiationsValidator),
expressionParser_(expressionParser),
instantiationInterface_(instantiationInterface)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("ComponentInstantiationsItem"));

    setReferenceCounter(referenceCounter);

    for (QSharedPointer<ComponentInstantiation> instantiation : *component->getComponentInstantiations())
    {
        QSharedPointer<SingleComponentInstantiationItem> componentInstantiationItem =
            createChildItem(instantiation);

        childItems_.append(componentInstantiationItem);

        connect(componentInstantiationItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
            this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);
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
        editor_ = new ComponentInstantiationsEditor(
            component_, libHandler_, parameterFinder_, instantiationInterface_, 0);
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
    
    QSharedPointer<SingleComponentInstantiationItem> childItem = createChildItem(instantiation);

    childItem->setLocked(locked_);

    connect(childItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    childItems_.append(childItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstantiationsItem::isValid() const
{
    // Validation for all instantiations only done once here (no need to do the same in design and design config instantiation)
    auto instantiationsAsNameGroups = CollectionValidators::itemListToNameGroupList(component_->getComponentInstantiations());
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component_->getDesignInstantiations()));
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component_->getDesignConfigurationInstantiations()));
    allInstantiationsValidator_->childrenHaveUniqueNames(instantiationsAsNameGroups);

    return ComponentEditorItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::createSingleComponentInstantiationItem()
//-----------------------------------------------------------------------------
QSharedPointer<SingleComponentInstantiationItem> ComponentInstantiationsItem::createChildItem(
    QSharedPointer<ComponentInstantiation> instantiation)
{
    QSharedPointer<MultipleParameterFinder> cimpFinder =
        QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
    cimpFinder->addFinder(parameterFinder_);
    cimpFinder->addFinder(QSharedPointer<ComponentInstantiationParameterFinder>
        (new ComponentInstantiationParameterFinder(instantiation)));

    QSharedPointer<ParameterReferenceCounter> cimpCounter =
        QSharedPointer<ParameterReferenceCounter>(new ParameterReferenceCounter(cimpFinder));
    QSharedPointer<ExpressionFormatter> cimpFormatter =
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(cimpFinder));

    QSharedPointer<IPXactSystemVerilogParser> cimpParser =
        QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(cimpFinder));

    QSharedPointer<SingleComponentInstantiationItem> componentInstantiationItem(
        new SingleComponentInstantiationItem(model_, libHandler_, component_, instantiation, validator_,
            cimpCounter, cimpFinder, cimpFormatter, cimpParser, instantiationInterface_, this));

    return componentInstantiationItem;
}
