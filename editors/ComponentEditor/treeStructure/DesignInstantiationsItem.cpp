//-----------------------------------------------------------------------------
// File: DesignInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The design instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "DesignInstantiationsItem.h"

#include "SingleDesignInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/DesignInstantiationsEditor.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::DesignInstantiationsItem()
//-----------------------------------------------------------------------------
DesignInstantiationsItem::DesignInstantiationsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<InstantiationsValidator> validator,
    QSharedPointer<AllInstantiationsValidator> allInstantiationsValidator,
    QSharedPointer<ParameterValidator> parameterValidator, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> componentParameterFinder, QSharedPointer<ReferenceCounter> referenceCounter,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
validator_(validator),
parameterValidator_(parameterValidator),
expressionParser_(expressionParser),
componentParameterFinder_(componentParameterFinder),
allInstantiationsValidator_(allInstantiationsValidator)
{
    setObjectName(tr("DesignInstantiationsItem"));
    setReferenceCounter(referenceCounter);

    for (QSharedPointer<DesignInstantiation> instantiation : *component->getDesignInstantiations())
    {
        childItems_.append(QSharedPointer<SingleDesignInstantiationItem>(new SingleDesignInstantiationItem(model,
            libHandler, component, instantiation, validator, parameterValidator, expressionParser, 
            componentParameterFinder, referenceCounter, this)));
    }
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::~DesignInstantiationsItem()
//-----------------------------------------------------------------------------
DesignInstantiationsItem::~DesignInstantiationsItem()
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString DesignInstantiationsItem::getTooltip() const
{
    return tr("Contains the design instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::text()
//-----------------------------------------------------------------------------
QString DesignInstantiationsItem::text() const
{
	return tr("Design instantiations");
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* DesignInstantiationsItem::editor()
{
	if (!editor_)
    {
        editor_ = new DesignInstantiationsEditor(component_, libHandler_, validator_, 0);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void DesignInstantiationsItem::createChild(int index)
{
    QSharedPointer<DesignInstantiation> instantiation = component_->getDesignInstantiations()->at(index);

    QSharedPointer<SingleDesignInstantiationItem> child(QSharedPointer<SingleDesignInstantiationItem>(
        new SingleDesignInstantiationItem(model_, libHandler_, component_, instantiation, validator_, 
            parameterValidator_, expressionParser_, componentParameterFinder_, referenceCounter_, this)));
    child->setLocked(locked_);

    childItems_.insert(index, child);
}
