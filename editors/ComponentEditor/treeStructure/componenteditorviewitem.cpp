//-----------------------------------------------------------------------------
// File: componenteditorviewitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for a single view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorviewitem.h"
#include <editors/ComponentEditor/views/vieweditor.h>

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

#include <IPXACTmodels/Component/validators/ViewValidator.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view, ComponentEditorTreeModel* model,
                                                 LibraryInterface* libHandler, QSharedPointer<Component> component,
                                                 QSharedPointer<ParameterFinder> parameterFinder,
                                                 QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                 QSharedPointer<ExpressionParser> expressionParser,
                                                 QSharedPointer<ViewValidator> viewValidator,
                                                 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
view_(view),
editAction_(new QAction(tr("Edit"), this)),
expressionParser_(expressionParser),
viewValidator_(viewValidator),
instantiationsValidator_()
{
    createInstantiationsValidator();

    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	Q_ASSERT(view_);

	setObjectName(tr("ComponentEditorViewItem: %1").arg(view->name()));

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection); 
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::~ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::~ComponentEditorViewItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::text() const
{
	return view_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::isValid() const
{
    bool viewIsValid = viewValidator_->validate(view_);
    bool componentInstantiationIsValid = true;
    bool designInstantiationIsValid = true;
    bool designConfigurationInstantiationIsValid = true;

    if (!view_->getComponentInstantiationRef().isEmpty())
    {
        QSharedPointer<ComponentInstantiation> instantiation =
            component_->getModel()->findComponentInstantiation(view_->getComponentInstantiationRef());
        componentInstantiationIsValid = instantiationsValidator_->validateComponentInstantiation(instantiation);
    }

    if (!view_->getDesignInstantiationRef().isEmpty())
    {
        QSharedPointer<DesignInstantiation> instantiation =
            component_->getModel()->findDesignInstantiation(view_->getDesignInstantiationRef());
        designInstantiationIsValid= instantiationsValidator_->validateDesignInstantiation(instantiation);
    }

    if (!view_->getDesignConfigurationInstantiationRef().isEmpty())
    {
        QSharedPointer<DesignConfigurationInstantiation> instantiation = component_->getModel()->
            findDesignConfigurationInstantiation(view_->getDesignConfigurationInstantiationRef());
        designConfigurationInstantiationIsValid =
            instantiationsValidator_->validateDesignConfigurationInstantiation(instantiation);
    }

    return viewIsValid && componentInstantiationIsValid && designInstantiationIsValid &&
        designConfigurationInstantiationIsValid;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorViewItem::editor()
{
	if (!editor_)
    {
        editor_ = new ViewEditor(component_, view_, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::getTooltip() const
{
	return tr("Specifies a representation level of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::canBeOpened() const
{
    return viewValidator_->hasValidDesignInstantiationReference(view_) ||
        viewValidator_->hasValidDesignConfigurationInstantiationReference(view_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorViewItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);

    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorViewItem::openItem()
{
	if (canBeOpened())
    {
        emit openDesign(component_->getVlnv(), view_->name());
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::isHierarchical()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::isHierarchical() const
{
    return view_->isHierarchical();       
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::hasIcon()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::hasIcon() const
{
    return view_->isHierarchical();       
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::getIcon()
//-----------------------------------------------------------------------------
QIcon ComponentEditorViewItem::getIcon() const
{
    if (view_->isHierarchical())
    {
        return QIcon(":/icons/common/graphics/hierarchy.png");
    }
    else
    {
        return QIcon();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewitem::createInstantiationsValidator()
//-----------------------------------------------------------------------------
void ComponentEditorViewItem::createInstantiationsValidator()
{
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(expressionParser_, component_->getChoices()));

    QSharedPointer<InstantiationsValidator> temporaryValidator (new InstantiationsValidator(
        expressionParser_, component_->getFileSets(), parameterValidator, libHandler_));

    instantiationsValidator_ = temporaryValidator;
}
