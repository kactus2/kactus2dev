//-----------------------------------------------------------------------------
// File: componenteditorviewsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorviewsitem.h"
#include "componenteditorviewitem.h"
#include <editors/ComponentEditor/views/viewseditor.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/validators/ViewValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

#include <KactusAPI/include/ComponentInstantiationParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ModuleParameterInterface.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::ComponentEditorViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorViewsItem::ComponentEditorViewsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
views_(component->getViews()),
expressionParser_(expressionParser),
viewValidator_(new ViewValidator(expressionParser, component->getModel())),
viewsValidator_(new ViewsValidator(viewValidator_)),
moduleParameterInterface_(0)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    createInterfaces();

	setObjectName(tr("ComponentEditorViewsItem"));

    setReferenceCounter(referenceCounter);

	for (QSharedPointer<View> view : *views_)
    {
		QSharedPointer<ComponentEditorViewItem> viewItem(new ComponentEditorViewItem(view, model, libHandler,
            component, parameterFinder_, expressionFormatter_, expressionParser, viewValidator_,
            moduleParameterInterface_, this));

        viewItem->setReferenceCounter(referenceCounter);
		childItems_.append(viewItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorViewsItem::getTooltip() const
{
    return tr("Contains the views of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorViewsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!views_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorViewsItem::text() const
{
	return tr("Views");
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorViewsItem::editor()
{
	if (!editor_)
    {
		editor_ = new ViewsEditor(component_, libHandler_, viewValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorViewsItem::createChild( int index )
{
	QSharedPointer<ComponentEditorViewItem> viewItem(
        new ComponentEditorViewItem(views_->at(index), model_, libHandler_, component_, parameterFinder_,
            expressionFormatter_, expressionParser_, viewValidator_, moduleParameterInterface_, this));
	viewItem->setLocked(locked_);

    viewItem->setReferenceCounter(referenceCounter_);
	childItems_.insert(index, viewItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewsItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorViewsItem::isValid() const
{
    auto viewsAsNameGroups = CollectionValidators::itemListToNameGroupList(views_);
    viewsValidator_->childrenHaveUniqueNames(viewsAsNameGroups);
    return ComponentEditorItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewsitem::createInterfaces()
//-----------------------------------------------------------------------------
void ComponentEditorViewsItem::createInterfaces()
{
    QSharedPointer<ParameterValidator> validator(
        new ParameterValidator(expressionParser_, component_->getChoices(), component_->getRevision()));

    QSharedPointer<ComponentInstantiationParameterFinder> instantiationParameterFinder(
        new ComponentInstantiationParameterFinder(QSharedPointer<ComponentInstantiation>()));

    QSharedPointer<MultipleParameterFinder> combinedInstantiationFinder =
        QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
    combinedInstantiationFinder->addFinder(parameterFinder_);
    combinedInstantiationFinder->addFinder(instantiationParameterFinder);

    QSharedPointer<IPXactSystemVerilogParser> instantiationParser(
        new IPXactSystemVerilogParser(combinedInstantiationFinder));

    QSharedPointer<ExpressionFormatter> instantiationFormatter(
        new ExpressionFormatter(combinedInstantiationFinder));

    moduleParameterInterface_ = new ModuleParameterInterface(
        validator, instantiationParser, instantiationFormatter, instantiationParameterFinder);
}
