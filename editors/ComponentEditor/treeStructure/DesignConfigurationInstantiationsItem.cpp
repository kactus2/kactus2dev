//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiationsItem.h"

#include "SingleDesignConfigurationInstantiationItem.h"

#include <editors/ComponentEditor/common/DesignConfigurationInstantiationParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/instantiations/DesignConfigurationInstantiationsEditor.h>
#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::DesignConfigurationInstantiationsItem()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationsItem::DesignConfigurationInstantiationsItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<InstantiationsValidator> validator,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, 
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
validator_(validator)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    setObjectName(tr("DesignConfigurationInstantiationsItem"));

    setReferenceCounter(referenceCounter);

    foreach(QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *component->getDesignConfigurationInstantiations())
    {
        QSharedPointer<MultipleParameterFinder> instantiationMultiFinder =
            QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
        instantiationMultiFinder->addFinder(parameterFinder);
        instantiationMultiFinder->addFinder(QSharedPointer<DesignConfigurationInstantiationParameterFinder>
            (new DesignConfigurationInstantiationParameterFinder(instantiation)));

        QSharedPointer<ParameterReferenceCounter> instantiationReferenceCounter =
            QSharedPointer<ParameterReferenceCounter>(new ParameterReferenceCounter(instantiationMultiFinder));
        QSharedPointer<ExpressionFormatter> instantiationFormatter =
            QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(instantiationMultiFinder));

        childItems_.append(QSharedPointer<ComponentEditorItem>(new SingleDesignConfigurationInstantiationItem(
            model, libHandler, component, instantiationReferenceCounter, instantiation, validator,
            instantiationMultiFinder, instantiationFormatter, this)));
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::~DesignConfigurationInstantiationsItem()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationsItem::~DesignConfigurationInstantiationsItem()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationsItem::getTooltip() const
{
    return tr("Contains the design configuration instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::text()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationsItem::text() const
{
	return tr("Design configuration instantiations");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* DesignConfigurationInstantiationsItem::editor()
{
    if (!editor_)
    {
        editor_ = new DesignConfigurationInstantiationsEditor(component_, libHandler_, validator_, 0);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
    }
    return editor_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationsItem::createChild(int index)
{
    QSharedPointer<DesignConfigurationInstantiation> instantiation = 
        component_->getDesignConfigurationInstantiations()->at(index);

    QSharedPointer<MultipleParameterFinder> instantiationMultiFinder =
        QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
    instantiationMultiFinder->addFinder(parameterFinder_);
    instantiationMultiFinder->addFinder(QSharedPointer<DesignConfigurationInstantiationParameterFinder>
        (new DesignConfigurationInstantiationParameterFinder(instantiation)));

    QSharedPointer<ParameterReferenceCounter> instantiationCounter =
        QSharedPointer<ParameterReferenceCounter>(new ParameterReferenceCounter(instantiationMultiFinder));
    QSharedPointer<ExpressionFormatter> instantiationFormatter =
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(instantiationMultiFinder));

    QSharedPointer<SingleDesignConfigurationInstantiationItem> child(
        QSharedPointer<SingleDesignConfigurationInstantiationItem>(new SingleDesignConfigurationInstantiationItem(
        model_, libHandler_, component_, instantiationCounter, instantiation, validator_, instantiationMultiFinder,
        instantiationFormatter, this)));
    child->setLocked(locked_);

    childItems_.insert(index, child);
}
