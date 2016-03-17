//-----------------------------------------------------------------------------
// File: InstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "InstantiationsItem.h"

#include "ComponentInstantiationsItem.h"
#include "DesignInstantiationsItem.h"
#include "DesignConfigurationInstantiationsItem.h"

#include <editors/ComponentEditor/instantiations/InstantiationsEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::InstantiationsItem()
//-----------------------------------------------------------------------------
InstantiationsItem::InstantiationsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    expressionParser_(expressionParser),
    validator_(new InstantiationsValidator(expressionParser, component->getFileSets(), 
    QSharedPointer<ParameterValidator2014>(new ParameterValidator2014(expressionParser, component->getChoices())),
        libHandler)),
    componentInstantiationsItem_(new ComponentInstantiationsItem(model, libHandler, component, validator_,
        referenceCounter, parameterFinder, expressionFormatter, expressionParser, this)),
    designConfigurationInstantiationsItem_(new DesignConfigurationInstantiationsItem(model, 
        libHandler, component, validator_, referenceCounter, parameterFinder, expressionFormatter, expressionParser,
        this)),
    designInstantiationsItem_(new DesignInstantiationsItem(model, libHandler, component, validator_, this))
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("InstantiationsItem"));

    setReferenceCounter(referenceCounter);

    connect(componentInstantiationsItem_.data(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    childItems_.append(componentInstantiationsItem_);
    childItems_.append(designConfigurationInstantiationsItem_);
    childItems_.append(designInstantiationsItem_);
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::~InstantiationsItem()
//-----------------------------------------------------------------------------
InstantiationsItem::~InstantiationsItem()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString InstantiationsItem::getTooltip() const
{
    return tr("Contains the instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::getFont()
//-----------------------------------------------------------------------------
QFont InstantiationsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getComponentInstantiations()->isEmpty() || 
        !component_->getDesignConfigurationInstantiations()->isEmpty() ||
        !component_->getDesignInstantiations()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::text()
//-----------------------------------------------------------------------------
QString InstantiationsItem::text() const
{
	return tr("Instantiations");
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* InstantiationsItem::editor()
{
	if (!editor_)
    {
 		editor_ = new InstantiationsEditor(component_, parameterFinder_, libHandler_, validator_);
 		editor_->setProtection(locked_);
 
 		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
  		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(componentInstanceAdded(int)), 
            componentInstantiationsItem_.data(), SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(componentInstanceRemoved(int)),
            componentInstantiationsItem_.data(), SLOT(onRemoveChild(int)));

        connect(editor_, SIGNAL(designConfigurationInstanceAdded(int)), 
            designConfigurationInstantiationsItem_.data(), SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(designConfigurationInstanceRemoved(int)),
            designConfigurationInstantiationsItem_.data(), SLOT(onRemoveChild(int)));

        connect(editor_, SIGNAL(designInstanceAdded(int)), designInstantiationsItem_.data(), 
            SLOT(onAddChild(int)));
        connect(editor_, SIGNAL(designInstanceRemoved(int)), designInstantiationsItem_.data(), 
            SLOT(onRemoveChild(int)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void InstantiationsItem::createChild(int)
{
    // No new children allowed.
}
