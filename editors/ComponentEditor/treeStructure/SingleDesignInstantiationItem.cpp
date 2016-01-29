//-----------------------------------------------------------------------------
// File: ComponentInstantiationsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The item for one design instantiation-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "SingleDesignInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/DesignInstantiationEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::SingleDesignInstantiationItem()
//-----------------------------------------------------------------------------
SingleDesignInstantiationItem::SingleDesignInstantiationItem(
    ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<DesignInstantiation> instantiation,
    QSharedPointer<InstantiationsValidator> validator,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    instantiation_(instantiation),
    validator_(validator)
{
    setObjectName(tr("ComponentInstantiationsItem"));
}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::~SingleDesignInstantiationItem()
//-----------------------------------------------------------------------------
SingleDesignInstantiationItem::~SingleDesignInstantiationItem()
{

}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleDesignInstantiationItem::getTooltip() const
{
    return tr("Contains the component instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::text()
//-----------------------------------------------------------------------------
QString SingleDesignInstantiationItem::text() const
{
	return instantiation_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::isValid()
//-----------------------------------------------------------------------------
bool SingleDesignInstantiationItem::isValid() const
{
    return validator_->validateDesignInstantiation(instantiation_);
}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleDesignInstantiationItem::editor()
{
	if (!editor_)
    {
 		editor_ = new DesignInstantiationEditor(component_, instantiation_, libHandler_, 0);
 		editor_->setProtection(locked_);
 
 		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
 		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: SingleDesignInstantiationItem::createChild()
//-----------------------------------------------------------------------------
void SingleDesignInstantiationItem::createChild(int)
{
    // No children allowed.
}
